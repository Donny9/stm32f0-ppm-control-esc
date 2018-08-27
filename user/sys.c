#include <sys.h>

void Clock_Init(void)
{
	/* reset and config vectortab */
	RCC_DeInit();

	/* enable HSION clock */
	RCC_HSICmd(ENABLE);

	/* APB1=DIV2;APB2=DIV1;AHB=DIV1 */
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* select HSION as PLL clock */
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);

	/* delay FLASH 2 clock cycle */
	FLASH->ACR |= 0x32;

	/* enable PLLON */
	RCC_PLLCmd(ENABLE);

	/* select PLL as SYSCLK */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* wait SYSCLK ready */
	while(RCC_GetSYSCLKSource() != 0x00);
}
static u8  fac_us=0;
static u16 fac_ms=0;
void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SystemCoreClock/8000000;
	fac_ms=(u16)fac_us*1000;
}

/**
 * delay_us()
 * time delay in us
 */
void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD = nus * fac_us;	/* time load */
	SysTick->VAL = 0x00;	/* cleat counter */
	SysTick->CTRL = 0x01;	/* start count down */
	do {
		temp=SysTick->CTRL;
	} while((temp & 0x01) && (!(temp & (1 << 16))));	/* wait time arrive */
	SysTick->CTRL = 0x00;	/* disable counter */
	SysTick->VAL = 0X00;	/* clear counter */
}

/**
 * delay_ms()
 * time delay in ms
 */
void delay_ms(u16 nms)
{
	u32 temp;
	SysTick->LOAD = (u32)nms * fac_ms;	/* time load */
	SysTick->VAL = 0x00;	/* cleat counter */
	SysTick->CTRL = 0x01;	/* start count down */
	do {
		temp = SysTick->CTRL;
	}
	while((temp & 0x01) && (!(temp & (1 << 16))));	/* wait time arrive */
	SysTick->CTRL = 0x00;	/* disable counter */
	SysTick->VAL = 0X00;	/* clear counter */
}


/********************************************** Usart ****************************************************/
/**
 * Init usart1
 * baud: baud rate
 * cb_event: intr callback function
 */
void UART1_Init(uint32_t baud, ARM_USART_SignalEvent_t cb_event)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART1;
	USARTdrv->Initialize(cb_event);
	USARTdrv->PowerControl(ARM_POWER_FULL);
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
			ARM_USART_DATA_BITS_8 |
			ARM_USART_PARITY_NONE |
			ARM_USART_STOP_BITS_1 |
			ARM_USART_FLOW_CONTROL_NONE, baud);
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
}

/**
 * USART1_Receive
 * data: Pointer to buffer for data to receive from USART receiver
 * num:  Number of data items to receive
 * return: execution_status
 */
int32_t UART1_Receive(void *data, uint32_t num)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART1;
	return USARTdrv->Receive(data, num);
}

/**
 * USART1_Send
 * data: Pointer to buffer with data to send to USART transmitter
 * num:  Number of data items to send
 * return: execution_status
 */
int32_t UART1_Send(const void *data, uint32_t num)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART1;
	return USARTdrv->Send(data, num);
}

/********************************************** Usart3 ****************************************************/
/**
 * PPM_UART_Init
 * baud: baud rate
 * cb_event: intr callback function
 */
void PPM_UART_Init(uint32_t baud, ARM_USART_SignalEvent_t cb_event)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;
	USARTdrv->Initialize(cb_event);
	USARTdrv->PowerControl(ARM_POWER_FULL);
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
			ARM_USART_DATA_BITS_8 |
			ARM_USART_PARITY_NONE |
			ARM_USART_STOP_BITS_1 |
			ARM_USART_FLOW_CONTROL_NONE, baud);
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
}

/**
 * PPM_Info_Receive
 * data: Pointer to buffer for data to receive from USART receiver
 * num:  Number of data items to receive
 * return: execution_status
 */
int32_t PPM_Info_Receive(void *data, uint32_t num)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;
	return USARTdrv->Receive(data, num);
}

/**
 * PPM_Info_Send
 * data: Pointer to buffer with data to send to USART transmitter
 * num:  Number of data items to send
 * return: execution_status
 */
int32_t PPM_Info_Send(const void *data, uint32_t num)
{
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;
	return USARTdrv->Send(data, num);
}
