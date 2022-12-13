#include "stm32f10x.h"
#include "my_lib.h"
#include "interrupt.h"
#include "ds18b20.h"


#define TH_tr 	0x1D	//28 cels	
#define TL_tr		0x14	//20 cels
#define CONF_REG_9bit 0x1F
#define CONF_REG_10bit 0x3F
#define CONF_REG_11bit 0x5F
#define CONF_REG_12bit 0x7F

int main (void) {
	SystemCoreClockConfigure();                             
  SystemCoreClockUpdate();
	//GPIO_Init();
	//TIM4_Init ();
	
/* DS18B20	
struct ID_ds18b20 ID_X [2];
float T1 = 0, T2 = 0;

READ_ROM_DS (&ID_X);

read_scr(&ID_X[0]);
read_scr(&ID_X[1]);
//ID_X[0].TH = 0x1E;
//ID_X[0].TL = 0x14;
//ID_X[0].ctrl = CONF_REG_9bit;
//write_scr	(&ID_X[0]);	

read_scr(&ID_X[1]);	
ID_X[1].TH = TH_tr;
ID_X[1].TL = TL_tr;
ID_X[1].ctrl = CONF_REG_10bit;
write_scr	(&ID_X[1]);


do{	
	convert();
	
	DELAY_us(1000000);
	
	alarm_search();
	T1 = read_T(ID_X[0].ID);
	T2 = read_T(ID_X[1].ID);
}	

	while (1);

*/

do{	
	
}	

	while (1);
}


