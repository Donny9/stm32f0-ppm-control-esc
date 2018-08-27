#include <peripheral.h>
#include <sys.h>
#include <utils.h>

static const uint16_t crc16tab[256] __attribute__((unused)) = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

/*led pattern*/
struct led_pwm_info led_pattern[] = {
	[IO_LED_OFF] = {0,0,0,0,0},
	[IO_LED_RED_ON] = {0,0,1023,35155/5,1023},
	[IO_LED_GREEN_ON] = {0,1023,1023,35155/5,1023},
	[IO_LED_BLUE_ON] = {1023,0,1023,35155/5,1023},
	[IO_LED_RED_S_BLINK] = {0,0,1023/2,35155/5,1023},
	[IO_LED_GREEN_S_BLINK] = {0,1023/2,1023/2,35155/5,1023},
	[IO_LED_BLUE_S_BLINK] = {1023/2,0,1023/2,35155/5,1023},
	[IO_LED_RED_F_BLINK] = {0,0,1023/4,35155/10,1023},
	[IO_LED_GREEN_F_BLINK] = {0,1023/4,1023/4,35155/10,1023},
	[IO_LED_BLUE_F_BLINK] = {1023/4,0,1023/4,35155/10,1023},
};

/********************************************** LED ****************************************************/
/**
 * Init led in gpio mode
 */
void Led_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/*enanle GPIOA clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*init GPIOA1.2.3 to AF_PP mode and use 50HZ speed*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/********************************************** LED ****************************************************/
/**
 * Init led in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Led_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/*enanle GPIOA clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*init GPIOA1.2.3 to AF_PP mode and use 50HZ speed*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*enable TIM2 clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/*set Auto-reload register*/
	TIM_TimeBaseStructure.TIM_Period = arr;
	/*set Prescaler register*/
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	/*set clock division*/
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	/*set counter direction*/
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/*init TIM2 channel2 3 4 to pwm2 mode*/ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	/*enable capture/compare output*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*set output polarity*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	/*enable TIM2 CCR2 CCR3 CCR4 Preload register*/
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_SetCompare2(TIM2,0);
	TIM_SetCompare3(TIM2,0);
	TIM_SetCompare4(TIM2,0);

	/*enable TIM2*/
	TIM_Cmd(TIM2, ENABLE);
}

/**
 * set led to different pattern
 */
static uint16_t last_psc = 0, last_arr = 0;
void Led_Set_Pattern(u8 pattern)
{
	if (!last_psc && !last_arr) {
		Led_GPIO_Init();
	}

	switch(pattern) {
		case IO_LED_OFF:
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
			GPIO_ResetBits(GPIOA,GPIO_Pin_3);
			break;
		case IO_LED_RED_ON:
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			GPIO_SetBits(GPIOA,GPIO_Pin_2);
			GPIO_SetBits(GPIOA,GPIO_Pin_3);
			break;
		case IO_LED_GREEN_ON:
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
			GPIO_SetBits(GPIOA,GPIO_Pin_2);
			GPIO_SetBits(GPIOA,GPIO_Pin_3);
			break;
		case IO_LED_BLUE_ON:
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
			GPIO_SetBits(GPIOA,GPIO_Pin_3);
			break;
		default :
			if (last_psc != led_pattern[pattern].psc || last_arr != led_pattern[pattern].arr) {
				Led_Init(led_pattern[pattern].arr,led_pattern[pattern].psc);
				last_psc = led_pattern[pattern].psc;
				last_arr = led_pattern[pattern].arr;
			}
			TIM_SetCompare2(TIM2, led_pattern[pattern].r_pwm);
			TIM_SetCompare3(TIM2, led_pattern[pattern].g_pwm);
			TIM_SetCompare4(TIM2, led_pattern[pattern].b_pwm);
			break;
	}
}

/********************************************** BEEP ****************************************************/
/**
 * Init beep in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 72000000/arr/psc=1
 */
void Beep_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/*enanle GPIOA clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*init GPIOA8 to AF_PP mode and use 50HZ speed*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*enable TIM1 clock*/
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/*set Auto-reload register*/
	TIM_TimeBaseStructure.TIM_Period = arr;
	/*set Prescaler register*/
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	/*set clock division*/
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	/*set counter direction*/
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/*init TIM1 channel1 to pwm2 mode*/ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	/*enable capture/compare output*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*set output polarity*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	/*enable TIM1 CCR1Preload register*/
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_SetCompare1(TIM1,0);

	/*enable TIM1*/
	TIM_Cmd(TIM1, ENABLE);
}

void Beep_Enable(u8 flag)
{
	if (flag == IO_BEEP_ENABLE) 
		TIM_SetCompare1(TIM1, 255);
	else if (flag == IO_BEEP_DISABLE)
		TIM_SetCompare1(TIM1, 0);
}

/********************************************** FAN&WARM ****************************************************/
/**
 * Init warm&fan in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Warm_Fan_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/*enanle GPIOB clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/*init GPIOB8.9 to AF_PP mode and use 50HZ speed*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*enable TIM4 clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/*set Auto-reload register*/
	TIM_TimeBaseStructure.TIM_Period = arr;
	/*set Prescaler register*/
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	/*set clock division*/
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	/*set counter direction*/
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/*init TIM4 channel3.4 to pwm2 mode*/ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	/*enable capture/compare output*/
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/*set output polarity*/
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

	/*enable TIM4 CCR3 CCR4 Preload register*/
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_SetCompare3(TIM4,0);
	TIM_SetCompare4(TIM4,0);

	/*enable TIM4*/
	TIM_Cmd(TIM4, ENABLE);
}

void Warm_Enable(u8 flag)
{
	if (flag == IO_WARM_ENABLE) 
		TIM_SetCompare4(TIM4, 255);
	else if (flag == IO_WARM_DISABLE)
		TIM_SetCompare4(TIM4, 0);
}

void Fan_Enable(u8 flag)
{
	if (flag == IO_FAN_ENABLE) 
		TIM_SetCompare3(TIM4, 255);
	else if (flag == IO_FAN_DISABLE)
		TIM_SetCompare3(TIM4, 0);
}
/********************************************** ADC ****************************************************/
/**
 * Init ADC
 */
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);	

	/* Set ADC divison clock factor:6. Note:this ADC1 clock don't more than 14Mhz */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* config PC0 as along input pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	/* reset ADC1 */
	ADC_DeInit(ADC1);

	/* Set ADC Mode*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* Set ADC ScanConvMode */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	/* Set ADC ContinuousConvMode */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	/* softwate trigger ADC to convert */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* Set ADC DateAlign */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Start reset calibration and wait wait unitl complete */
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start calibrate ADC and wait wait unitl complete */
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

#define IMU_SAMPLE_TEMP_CHANNEL 10
u16 Get_ADC(u8 ch)
{
	/* set adc convert channel and sample time 
	 *239.5+12.5 = 252 cycle sample cost time: 1/12Mhz*252=21us 
	 */
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );		    

	/* start convert in software and wait unitl complete */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	return ADC_GetConversionValue(ADC1);
}

u16 Get_Average_ADC(u8 times, u8 ch)
{
	uint32_t total = 0, i = times;
	while(i--)
		total += Get_ADC(ch);
	return total/times;
}

/********************************************** Servo ****************************************************/
#define ESC_PWM_STOP 1100
#define ESC_PWM_INIT 1100
#define ESC_PWM_DEFAULT_MAX 2000
#define LED_PWM_DEFAULT_MIN 0
#define LED_PWM_DEFAULT_MAX 255
/**
 * Init servo in pwm mode
 * arr: Auto-reload register value
 * psc: Presaler register value
 * 36000000/arr/psc=1
 */
void Servo_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* enanle GPIOC and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* enable time3 remapping */
	AFIO->MAPR |= 3 << 10;

	/* init GPIOC6.7.8.9 to AF_PP mode and use 50HZ speed */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* enable TIM3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* set Auto-reload register */
	TIM_TimeBaseStructure.TIM_Period = arr;
	/* set Prescaler register */
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	/* set clock division */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	/* set counter direction */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* init TIM3 channel1 2 3 4 to pwm2 mode */ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	/* enable capture/compare output */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	/* set output polarity */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	/* enable TIM3 CCR1 CCR2 CCR3 CCR4 Preload register */
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_SetCompare1(TIM3, 0);
	TIM_SetCompare2(TIM3, 0);
	TIM_SetCompare3(TIM3, 0);
	TIM_SetCompare4(TIM3, 0);

	/* enable TIM3 */
	TIM_Cmd(TIM3, ENABLE);

	/* set ESC_PWM_INIT because of when inititlization of ESC needing */
	TIM_SetCompare1(TIM3, ESC_PWM_INIT);
	TIM_SetCompare2(TIM3, ESC_PWM_INIT);
	TIM_SetCompare3(TIM3, ESC_PWM_INIT);
	TIM_SetCompare4(TIM3, ESC_PWM_INIT);
}

/**
 * set servo pwm cycle
 */
int Servo_Set_Pwm(u16 esc_channel, u16 highcycle)
{
	switch(esc_channel) {
		case 0:
			TIM_SetCompare1(TIM3, highcycle);
			break;
		case 1:
			TIM_SetCompare2(TIM3, highcycle);
			break;
		case 2:
			TIM_SetCompare3(TIM3, highcycle);
			break;
		case 3:
			TIM_SetCompare4(TIM3, highcycle);
			break;
		default:
			return -1;
	}
	return 0;
}

/**
 * set servo pwm pluswidth
 */
void Servo_Set_PlusWidth(u16 arr)
{
	arr = TIM1->ARR = arr;
	TIM2->ARR = arr;
	TIM3->ARR = arr;
}

/********************************************** packet handle ****************************************************/
void crc_packet(struct IOPacket *pkt)
{
	int counter;
	u16 crc = 0;
	int len = PKT_SIZE(*pkt);
	const char * buf =(const char *)pkt;

	pkt->crc = 0;
	for (counter = 0; counter < len; counter++) {
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *buf++) & 0x00FF];
	}
	pkt->crc =crc;
}

int parse_write_cmd(u16 cmd, u16 *values, unsigned num_values)
{
	u16 reg_value;
	u16 indx;
	ram_printf ("%s:cmd:%d,values:%d,num_values:%d\n", __func__, cmd, values[0], num_values);
	switch (cmd) {
		case CMD_SET_BEEP :
			Beep_Enable(*values);
			break;
		case CMD_SET_FAN :
			Fan_Enable(*values);
			break;
		case CMD_SET_WARM :
			Fan_Enable(*values);
			break;
		case CMD_SET_LED :
			Led_Set_Pattern(*values);
			break;
		case CMD_SET_ESC_PWM_SINGLE :
			if (num_values < 2)
				return -1;
			reg_value = values[1];
			if (values[1] < ESC_PWM_STOP)
				reg_value = ESC_PWM_STOP;
			else if (values[1] > ESC_PWM_DEFAULT_MAX)
				reg_value = ESC_PWM_DEFAULT_MAX;
			if (Servo_Set_Pwm(values[0], reg_value) < 0)
				return -1;
			break;
		case CMD_SET_ESC_PWM_ALL :
			if (num_values < 4)
				return -1;
			for (indx = 0; indx < num_values; indx++) {
				reg_value = values[indx];
				if (values[indx] < ESC_PWM_STOP)
					reg_value = ESC_PWM_STOP;
				else if (values[indx] > ESC_PWM_DEFAULT_MAX)
					reg_value = ESC_PWM_DEFAULT_MAX;
				if (Servo_Set_Pwm(indx, reg_value) < 0)
					return -1;
			}
			break;
		case CMD_SET_ESC_FREQ :
			Servo_Set_PlusWidth(1000000/(*values) - 1);
			break;
		default :
			return -1;
	};
	return 0;
}

int parse_read_cmd(u16 cmd, u16 *values)
{
	switch (cmd) {
		case CMD_GET_ADC :
			*values = Get_Average_ADC(5, IMU_SAMPLE_TEMP_CHANNEL);
			break;
		default :
			return -1;
	};
	return 0;
}

/**
 * Rx_Handle_Packet
 * packet: Pointer to receive data form USART
 */
void Rx_Handle_Packet(struct IOPacket *packet)
{
	/* check packet crc */
	u16 crc = packet->crc;

	crc_packet(packet);
	if (crc != packet->crc) {
		/* send a crc error reply */
		ram_printf("%s,communication failed because of crc error.count_code:0x%x,cmd:0x%x\n", __func__, packet->count_code,packet->cmd);
		packet->count_code = PKT_CODE_CORRUPT;
		packet->cmd = 0xff;
		goto reply;
	}

	/* write registers */
	if (PKT_CODE(*packet) == PKT_CODE_WRITE) {
		/*it's blind write - pass it on */
		if (parse_write_cmd(packet->cmd, &packet->data[0], PKT_COUNT(*packet))) {
			ram_printf("%s,write communication failed:count_code:0x%x,cmd:0x%x\n", __func__, packet->count_code,packet->cmd);
			packet->count_code = PKT_CODE_ERROR;
		} else {
			ram_printf("%s,write communication success:count_code:0x%x,cmd:0x%x\n", __func__, packet->count_code,packet->cmd);
			packet->count_code = PKT_CODE_SUCCESS;
		}
		goto reply;
	}

	/* read registers */
	if (PKT_CODE(*packet) == PKT_CODE_READ) {
		/*it's blind write - pass it on */
		if (parse_read_cmd(packet->cmd, &packet->data[0])) {
			packet->count_code = PKT_CODE_ERROR;
			ram_printf("%s,read communication failed:count_code:0x%x,cmd:0x%x\n", __func__, packet->count_code,packet->cmd);
		} else {
			packet->count_code = PKT_CODE_SUCCESS;
			ram_printf("%s,readcommunication success:count_code:0x%x,cmd:0x%x\n", __func__, packet->count_code,packet->cmd);
		}
		goto reply;
	}

	/* send a bad-packet error reply */
	packet->count_code = PKT_CODE_CORRUPT;
	packet->cmd = 0xff;

reply:
	crc_packet(packet);
#if 0
	/* start dma transfer from sram to usart */
	UART1_Send(packet, sizeof(struct IOPacket));
#endif
}

