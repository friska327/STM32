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
	
	//LM75A_Init();
	
	//LM75A_TH_READ();
	//LM75A_Tos_READ();
	
	
	//LM75A_CONF_Write();	
	//LM75A_CONF_READ();
	
	//LM75A_SP_Write();
	TIM4_Init ();
	
	
	while (1)
  {
		
  } 
}


