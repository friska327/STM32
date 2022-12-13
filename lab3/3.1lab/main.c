#include "stm32f10x.h"
#include "my_lib.h"
#include "interrupt.h"

extern char RX_BUFF[];




int main (void) {
	SystemCoreClockConfigure();                             
  SystemCoreClockUpdate();
	USART_init();
		
	DMI_init();
	
	while (1)
  {
   

		
  } 

}


