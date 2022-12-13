#include "stm32f10x.h"
#include "my_lib.h"


void SysTick_Handler()
{
	
}

void TIM4_IRQHandler () {													 // TIM4 interruprt handler
		TIM4->SR &= ~TIM_SR_UIF;	
		//LM75A_T_READ();
		LM75A_Tos_READ();
}

I2C1_ER_IRQHandler()
{
	if(I2C1->SR1 && I2C_SR1_AF)
	{
	
	//TIM4->SR &= ~TIM_SR_UIF;
	I2C1->CR1 |= I2C_CR1_STOP;
		
	DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
	DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
		
	}
	
	
	
}