#include "stm32f10x.h"
#include "my_lib.h"

//#define BUFF_SIZE 50

extern char mass[];


USART1_IRQHandler()
{
			//IDLE interrupt
			USART1->SR; //IDLE bit clear
			USART1->DR; //
			
			char mass_tmp[6] = {' ','0','7','9','2','\n'};		
			char tmp = BUFF_SIZE - DMA1_Channel5->CNDTR;
			char i;
			for(i = tmp; i < tmp + 6; i++)
			{
				mass[i] = mass_tmp[i - tmp];
			}
			
			DMI_channel4_off(); //TX off
			DMI_channel4_go(mass, i);//TX mess
			
			
}

void DMA1_Channel4_IRQHandler()
{
																		//TCIE
	DMI_channel5_off(); 							//RX off
	DMI_channel5_go(mass, BUFF_SIZE); //RX mess
		
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
	
}

