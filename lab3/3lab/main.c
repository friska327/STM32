#include "stm32f10x.h"
#include "my_lib.h"
#include "interrupt.h"

extern char mass[];




int main (void) {
	SystemCoreClockConfigure();                             
  SystemCoreClockUpdate();
	USART_init();
		
	DMI_init();
	
	DMI_channel5_go(mass, BUFF_SIZE);
	
  while (1)
  {
    
  } 

}


