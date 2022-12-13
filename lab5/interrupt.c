#include "stm32f10x.h"
#include "my_lib.h"
//#include "I2C"

void SysTick_Handler()
{
	
}


void TIM3_IRQHandler () {													 // TIM4 interruprt handler
		TIM3->SR &= ~TIM_SR_UIF;	
		SSD1306_Write_DATA();
}