#ifndef __SYS_H
#define __SYS_H

#include <stm32f10x.h>
#include <Driver_USART.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include <RTE_Device.h>
//#include <USART_STM32F10x.h>

extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART3;

void Clock_Init(void);
void Delay_Init(void);

/**
 * delay_us()
 * time delay in us
 */
void delay_us(u32 nus);

/**
 * delay_ms()
 * time delay in ms
 */
void delay_ms(u16 nms);

/**
 * Init usart1
 * baud: baud rate
 * cb_event: intr callback function
 */
void UART1_Init(uint32_t baud, ARM_USART_SignalEvent_t cb_event);

/**
 * USART1_Receive
 * data: Pointer to buffer for data to receive from USART receiver
 * num:  Number of data items to receive
 * return: execution_status
 */
int32_t UART1_Receive(void *data, uint32_t num);

/**
 * USART1_Send
 * data: Pointer to buffer with data to send to USART transmitter
 * num:  Number of data items to send
 * return: execution_status
 */
int32_t UART1_Send(const void *data, uint32_t num);

/**
 * PPM_USART_Init
 * baud: baud rate
 * cb_event: intr callback function
 */
void PPM_UART_Init(uint32_t baud, ARM_USART_SignalEvent_t cb_event);

/**
 * PPM_Info_Receive
 * data: Pointer to buffer for data to receive from USART receiver
 * num:  Number of data items to receive
 * return: execution_status
 */
int32_t PPM_Info_Receive(void *data, uint32_t num);

/**
 * PPM_Info_Send
 * data: Pointer to buffer with data to send to USART transmitter
 * num:  Number of data items to send
 * return: execution_status
 */
int32_t PPM_Info_Send(const void *data, uint32_t num);
#endif
