#include "stm32f10x.h"
#include "my_lib.h"
#include "interrupt.h"

extern char BUFF_RX[];
extern char BUFF_TX[];
extern float T;


int main (void) {
	SystemCoreClockConfigure();                             
  SystemCoreClockUpdate();
	GPIO_Init();
	
	I2C1_Init();
	DMA_Init();
	
	ADC_Init();
	
	SSD1306_Init();
	
	TIM2_Init ();
	TIM3_Init ();

	//DELAY_us(10000);
	//SSD1306_Write_DATA();
	
	
	while (1)
  {
		
  } 
}


