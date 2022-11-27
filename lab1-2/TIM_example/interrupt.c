#include "stm32f10x.h"
#include "my_lib.h"

void SysTick_Handler()
{
}


void TIM4_IRQHandler () {													 // TIM4 interruprt handler
		TIM4->SR &= ~TIM_SR_UIF;	
		if(GPIOA->ODR & GPIO_ODR_ODR5)
		{
		GPIOA->BSRR |= GPIO_BSRR_BR5;
		}
		else
		{
		GPIOA->BSRR |= GPIO_BSRR_BS5;	
		}
}


void EXTI0_IRQHandler ()
{
	
}
