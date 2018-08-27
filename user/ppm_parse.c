#include <ppm_parse.h>
#include "sys.h"
#include <utils.h>

extern bool ppm_update_flag;

/**
 * PPM Parse Init
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void PPM_Parse_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/*enanle GPIOB clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/*init GPIOB7 to Input mode*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_7);

	/*enable TIM4 clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/*set Auto-reload register*/
	TIM_TimeBaseStructure.TIM_Period = arr;
	/*set Prescaler register*/
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	/*set clock division*/
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	/*set counter direction*/
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	///*init TIM4 channel2 to input capture mode*/ 
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	///*enable capture polarity*/
	TIM_ICInitStructure.TIM_ICPolarity= TIM_ICPolarity_Rising;
	///*set input capture prescaler*/
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	/*set input capture selection*/
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	///*set input capture filter*/
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	/*INTR group init*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/*enable TIM4 intr*/
	TIM_ITConfig(TIM4, TIM_IT_Update|TIM_IT_CC2, ENABLE);

	/*enable TIM4*/
	TIM_Cmd(TIM4, ENABLE);
}

#define PPM_IN_MIN_SYNC_PULSE_US    2700  // microseconds
#define PPM_IN_MIN_CHANNEL_PULSE_US 750   // microseconds
#define PPM_IN_MAX_CHANNEL_PULSE_US 2250  // microseconds
#define PPM_NUM_CHANNELS  8
#define PPM_STABLE_CHANNEL_COUNT 25
#define PIOS_PPM_IN_MIN_NUM_CHANNELS 4
#define PIOS_PPM_IN_MAX_NUM_CHANNELS 16

static bool Tracking = true, Fresh = true;
static uint32_t previousTime, currentTime, deltaTime, largeCounter;
static uint8_t pulseIndex, numChannels, numChannelCounter;
uint16_t captureValue[PPM_NUM_CHANNELS];
static uint16_t captureValueNewFrame[PPM_NUM_CHANNELS];
static int numChannelsPrevFrame = -1;

/*! Define error codes for PIOS_RCVR_Get */
enum RCVR_errors {
	/*! Indicates that a failsafe condition or missing receiver detected for that channel */
	RCVR_TIMEOUT  = -1,
	/*! Channel is invalid for this driver (usually out of range supported) */
	RCVR_INVALID  = -2,
	/*! Indicates that the driver for this channel has not been initialized */
	RCVR_NODRIVER = -3
};

static void PPM_tim_edge_cb(uint16_t edge_count)
{
	/* Shift the last measurement out */
	previousTime = currentTime;

	/* Grab the new count */
	currentTime  = edge_count;

	/* Convert to 32-bit timer result */
	currentTime += largeCounter;

	/* Capture computation */
	deltaTime    = currentTime - previousTime;

	previousTime = currentTime;
	/* Sync pulse detection */
	if (deltaTime > PPM_IN_MIN_SYNC_PULSE_US) {
		if (pulseIndex == numChannelsPrevFrame
				&& pulseIndex >= PIOS_PPM_IN_MIN_NUM_CHANNELS
				&& pulseIndex <= PIOS_PPM_IN_MAX_NUM_CHANNELS) {
			/* If we see n simultaneous frames of the same
			   number of channels we save it as our frame size */
			if (numChannelCounter < PPM_STABLE_CHANNEL_COUNT) {
				numChannelCounter++;
			} else {
				numChannels = pulseIndex;
			}
		} else {
			numChannelCounter = 0;
		}

		/* Check if the last frame was well formed */
		if (pulseIndex ==  numChannels && Tracking) {
			/* The last frame was well formed */
			for (int32_t i = 0; i < numChannels && i < PPM_NUM_CHANNELS; i++) {
				captureValue[i] = captureValueNewFrame[i];
			}
			for (int32_t i = numChannels; i < PPM_NUM_CHANNELS; i++) {
				captureValue[i] = RCVR_TIMEOUT;
			}
			ppm_update_flag = true;
		} else {
			for (uint32_t i = 0; i < PPM_NUM_CHANNELS; i++) {
				captureValue[i] = RCVR_TIMEOUT;
			}
		}
		Fresh      = true;
		Tracking   = true;
		numChannelsPrevFrame = pulseIndex;
		pulseIndex = 0;

		/* We rely on the supervisor to set CaptureValue to invalid
		   if no valid frame is found otherwise we ride over it */
	} else if (Tracking) {
		/* Valid pulse duration 0.75 to 2.5 ms*/
		if (deltaTime > PPM_IN_MIN_CHANNEL_PULSE_US
				&& deltaTime < PPM_IN_MAX_CHANNEL_PULSE_US
				&& pulseIndex < PPM_NUM_CHANNELS) {
			captureValueNewFrame[pulseIndex] = (uint16_t)deltaTime;
			pulseIndex++;
		} else {
			/* Not a valid pulse duration */
			Tracking = false;
			for (uint32_t i = 0; i < PPM_NUM_CHANNELS; i++) {
				captureValueNewFrame[i] = RCVR_TIMEOUT;
			}
		}
	}
}

static void PPM_tim_overflow_cb(uint16_t overflow_count)
{
	largeCounter += overflow_count;
}
#include <stdio.h>
#include <string.h>
void TIM4_IRQHandler(void)
{
	/* Check for an overflow event on this timer */
	bool overflow_event = false, edge_event = false;
	uint16_t overflow_count = 0, edge_count = 0;

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		overflow_count = TIM4->ARR;
		overflow_event = true;
	}

	if (TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
		edge_count = TIM_GetCapture2(TIM4);
		edge_event = true;
	} else {
		edge_event = false;
		edge_count = 0;
	}

	/* Generate the appropriate callbacks */
	if (overflow_event & edge_event) {
		/*
		 * When both edge and overflow happen in the same interrupt, we
		 * need a heuristic to determine the order of the edge and overflow
		 * events so that the callbacks happen in the right order.  If we
		 * get the order wrong, our pulse width calculations could be off by up
		 * to ARR ticks.  That could be bad.
		 *
		 * Heuristic: If the edge_count is < 32 ticks above zero then we assume the
		 *            edge happened just after the overflow.
		 */

		if (edge_count < 32) {
			/* Call the overflow callback first */
			PPM_tim_overflow_cb(overflow_count);
			/* Call the edge callback second */
			PPM_tim_edge_cb(edge_count);
		} else {
			/* Call the edge callback first */
			PPM_tim_edge_cb(edge_count);
			/* Call the overflow callback second */
			PPM_tim_overflow_cb(overflow_count);
		}
	} else if (overflow_event) {
		PPM_tim_overflow_cb(overflow_count);
	} else if (edge_event) {
		PPM_tim_edge_cb(edge_count);
	}
}
