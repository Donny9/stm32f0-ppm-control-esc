#ifndef __PPM_PARSE_H
#define __PPM_PARSE_H

#include <stm32f10x.h>
#include <Driver_USART.h>
#include <stm32f10x_rcc.h>
#include <misc.h>

/**
 * PPM Parse Init
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void PPM_Parse_Init(u16 arr, u16 psc);
#endif
