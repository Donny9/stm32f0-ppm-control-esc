#ifndef __PERIPHERAL_H
#define __PERIPHERAL_H

#include <protocol.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_gpio.h>


struct led_pwm_info {
	u16 r_pwm;
	u16 g_pwm;
	u16 b_pwm;
	u16 psc;
	u16 arr;
};

/**
 * Init led in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Led_Init(u16 arr, u16 psc);

/**
 * Init led in gpio mode
 */
void Led_GPIO_Init(void);

/**
 * set led to different pattern
 */
void Led_Set_Pattern(u8 pattern);

/**
 * Init beep in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 72000000/arr/psc=1
 */
void Beep_Init(u16 arr, u16 psc);

/**
 * Init warm&fan in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Warm_Fan_Init(u16 arr, u16 psc);

/**
 * Init ADC
 */
void  Adc_Init(void);

/**
 * Init servo in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Servo_Init(u16 arr, u16 psc);

/**
 * set servo pwm cycle
 */
int Servo_Set_Pwm(u16 esc_channel, u16 highcycle);

/**
 * Rx_Handle_Packet
 * packet: Pointer to receive data form USART
 */
void Rx_Handle_Packet(struct IOPacket *packet);

void crc_packet(struct IOPacket *pkt);
#endif
