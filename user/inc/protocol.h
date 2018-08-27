#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "stm32f10x.h"
#include "stdio.h"

#define CMD_SET_BEEP            0
#define CMD_SET_FAN             1
#define CMD_SET_WARM            2
#define CMD_SET_LED             3
#define CMD_SET_ESC_PWM_ALL     4
#define CMD_SET_ESC_PWM_SINGLE  5
#define CMD_SET_ESC_FREQ        6
#define CMD_GET_ADC             7
#define CMD_UPDATE_PPM          8

/* The status of beep */
#define IO_BEEP_ENABLE   1
#define IO_BEEP_DISABLE  0

/* The status of fan */
#define IO_FAN_ENABLE    1
#define IO_FAN_DISABLE   0

/* The status of warm */
#define IO_WARM_ENABLE    1
#define IO_WARM_DISABLE   0

/* The pattern of led */
#define IO_LED_OFF           0
#define IO_LED_RED_ON        1
#define IO_LED_GREEN_ON      2
#define IO_LED_BLUE_ON       3
#define IO_LED_RED_S_BLINK   4
#define IO_LED_RED_F_BLINK   5
#define IO_LED_GREEN_S_BLINK 6
#define IO_LED_GREEN_F_BLINK 7
#define IO_LED_BLUE_S_BLINK  8
#define IO_LED_BLUE_F_BLINK  9

#define PKT_MAX_DATA 8
struct IOPacket {
	u16 count_code;
	u16 crc;
	u16 cmd;
	u16 data[PKT_MAX_DATA];
};

#define PKT_CODE_READ		  0x0	    /* FMU->IO read transaction */
#define PKT_CODE_WRITE		0x0400	/* FMU->IO write transaction */
#define PKT_CODE_SUCCESS	0x0	    /* IO->FMU success reply */
#define PKT_CODE_CORRUPT	0x0400	/* IO->FMU bad packet reply */
#define PKT_CODE_ERROR		0x0800	/* IO->FMU register op error reply */

#define PKT_CODE_MASK		  0xff00
#define PKT_COUNT_MASK		0x00ff

#define PKT_COUNT(_p)	((_p).count_code & PKT_COUNT_MASK)
#define PKT_CODE(_p)	((_p).count_code & PKT_CODE_MASK)
#define PKT_SIZE(_p)  (sizeof(struct IOPacket) + (PKT_COUNT(_p) - PKT_MAX_DATA) * 2)

#endif
