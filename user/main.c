#include <peripheral.h>
#include <sys.h>
#include <string.h>
#include <utils.h>
#include <ppm_parse.h>

#define RECE_COMPLETE	1
#define RECE_WAIT	2

#define SEND_COMPLETE	1
#define SEND_ERROR	2

static int send_flag = 0;
static int rece_flag = RECE_WAIT;

struct IOPacket g1_iopacket;
struct IOPacket g3_iopacket;

static void UART1_Callback(u32 event)
{
	if (event & ARM_USART_EVENT_RECEIVE_COMPLETE){
		rece_flag = RECE_COMPLETE;
	}

	if ((event & ARM_USART_EVENT_TX_COMPLETE) || (event & ARM_USART_EVENT_SEND_COMPLETE)) {
		send_flag = SEND_COMPLETE;
	}
}

extern uint16_t captureValue[];
bool ppm_update_flag = false;
int main(void)
{
	int i = 0;
	/* pll will HSI clock muilt 9 to init sysclk */
	Clock_Init();
	/* init systick */
	Delay_Init();
	/* pwm freq:100hz(72000000/x/255=100) */
	Beep_Init(255, 2808);
	/* pwm freq:500hz(36000000/x/2500=400), plus width:2500 */
	Servo_Init(2499,35);
	/* init adc */
	Adc_Init();
	/* init uart and set baud rate to 115200 */
	UART1_Init(921600, UART1_Callback);
	/* init uart for transfering RC input singal*/
//	PPM_UART_Init(460800, NULL);
	/* init time for caputre ppm singal */
//	PPM_Parse_Init(0xffff, 35);

	memset(&g1_iopacket, 0 ,sizeof(g1_iopacket));
	memset(&g3_iopacket, 0 ,sizeof(g3_iopacket));

	Led_Set_Pattern(IO_LED_GREEN_S_BLINK);

	/* prepare receiving msg */
	UART1_Receive(&g1_iopacket, sizeof(g1_iopacket));
	while(1)
	{
		if (ppm_update_flag) {
			ppm_update_flag = false;
			memset(&g3_iopacket, 0 ,sizeof(g3_iopacket));
			g3_iopacket.count_code = 8 | PKT_CODE_WRITE;
			g3_iopacket.cmd = CMD_UPDATE_PPM;
			for (i = 0; i < 8; i++)
				g3_iopacket.data[i] = captureValue[i];
			crc_packet(&g3_iopacket);
			PPM_Info_Send(&g3_iopacket, sizeof(g3_iopacket));
		}

		if (rece_flag != RECE_WAIT) {
			rece_flag = RECE_WAIT;
			/* handle packet */
			Rx_Handle_Packet(&g1_iopacket);

			/* wait reveive data form usart */
			UART1_Receive(&g1_iopacket, sizeof(g1_iopacket));
		}
	}
}
