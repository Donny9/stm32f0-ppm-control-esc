#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <utils.h>
#include <stm32f10x.h>

extern uint32_t __StackTop;
extern void Reset_Handler (void);
extern void HardFault_Handler (void);
extern void DMA1_Channel2_IRQHandler (void);
extern void DMA1_Channel3_IRQHandler (void);
extern void DMA1_Channel4_IRQHandler (void);
extern void DMA1_Channel5_IRQHandler (void);
extern void TIM4_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void SystemInit (void);

int main (int argc, char *argv[]);
void DefaultHandler(void);

extern uint32_t __text_end__, __data_start__, __data_end__, 
       __bss_start__, __bss_end__, __LogbufBase, __StackLimit;

#define HANDLER(x) [x] = DefaultHandler##x,

#define DefaultHandler(x)                          \
	void DefaultHandler##x (void) {            \
		ram_printf("%s\n", __func__);      \
		while(1);                          \
	}	                                   \

#define INTR_NVIC_FOREACH_IRQ(G)                                              \
		      G (2)         G (4)  G (5)  G (6)  G (7)                \
	       G (11) G (12)        G (14) G (15) G (16) G (17) G (18) G (19) \
	G (20) G (21) G (22) G (23) G (24) G (25) G (26) G (27)               \
		      G (32) G (33) G (34) G (35) G (36) G (37) G (38) G (39) \
	G (40) G (41) G (42) G (43) G (44) G (45)        G (47) G (48) G (49) \
	G (50) G (51) G (52)        G (54)        G (56) G (57) G (58) G (59) \
	G (60) G (61) G (62) G (63)

INTR_NVIC_FOREACH_IRQ(DefaultHandler)

void *__vector_table[] __attribute__ ((section(".vector_table"))) = {
	[0] = &__StackTop,
	[1] = Reset_Handler,
	[3] = HardFault_Handler,
	[28] = DMA1_Channel2_IRQHandler,
	[29] = DMA1_Channel3_IRQHandler,
	[30] = DMA1_Channel4_IRQHandler,
	[31] = DMA1_Channel5_IRQHandler,
	[46] = TIM4_IRQHandler,
	[53] = USART1_IRQHandler,
	[55] = USART3_IRQHandler,
	INTR_NVIC_FOREACH_IRQ(HANDLER)
};

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler (void)
{
	uint32_t *src = &__text_end__;
	uint32_t *dst = &__data_start__;

	/* copy data section from LMA(__text_end) to VMA(__data_start)*/
	while (dst < &__data_end__)
		*dst++ = *src++;

	/* clear bss section */
	dst = &__bss_start__;
	while (dst < &__bss_end__)
		*dst++ = 0;

	dst = &__LogbufBase;
	while (dst < &__StackLimit)
		*dst++ = 0;

	SystemInit();

	main (0, NULL);

	while (1);
}


/*----------------------------------------------------------------------------
  HardFault Handler called when controller happened hardfault
 *----------------------------------------------------------------------------*/
void HardFault_Handler (void)
{
	__asm volatile (
		"mrs r0, psp        \n"
		"mrs r1, msp        \n"
		"b _hardfault_handler\n"
	);
}

void _hardfault_handler(uint32_t *psp, uint32_t *msp)
{
	uint32_t i = 0;
	ram_printf ("%s: hardirq isn't handled\n", __func__);
	ram_printf ("CFSR  = %08x\n", SCB->CFSR);
	ram_printf ("HFSR  = %08x\n", SCB->HFSR);
	ram_printf ("DFSR  = %08x\n", SCB->DFSR);
	ram_printf ("MMFAR = %08x\n", SCB->MMFAR);
	ram_printf ("BFAR  = %08x\n", SCB->BFAR);
	ram_printf ("AFSR  = %08x\n", SCB->AFSR);

	for (i = 0; i < 64 && psp; i += 4) {
		ram_printf ("%p: %08x %08x %08x %08x\n", psp + i,
			psp[i], psp[i + 1], psp[i + 2], psp[i + 3]);
	}

	for (i = 0; i < 64 && msp; i += 4) {
		ram_printf ("%p: %08x %08x %08x %08x\n", msp + i,
			msp[i], msp[i + 1], msp[i + 2], msp[i + 3]);
	}
}

