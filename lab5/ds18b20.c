#include "stm32f10x.h"
#include "my_lib.h"
#include "ds18b20.h"
#include <math.h >

float T_ds18b20;

void ds18b20 ()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                /* Enable GPIOA clock            */

		
	/* Configure alt fun (PA.0) pins as output (output) */
	GPIOA->BSRR = GPIO_BSRR_BR0 ;
	GPIOA->CRL &= ~GPIO_CRL_MODE0;
	GPIOA->CRL |= GPIO_CRL_MODE0_1; // output
	GPIOA->CRL &= ~GPIO_CRL_CNF0;
	GPIOA->CRL |= GPIO_CRL_CNF0_0;	//output open drain
	
}





void convert() //broadcast mess
{
	initial_ds18b20 ();
	send_Byte(SKIP_ROM);		
	send_Byte(CONVERT_T);
}

void read_T()
{
		
	initial_ds18b20 ();
	//MATCH_R(addr);
	send_Byte(SKIP_ROM);
	send_Byte(Read_Scratchpad);
	
	
	T_ds18b20 = 0;
	char lsb = 0, msb = 0;
	char tmp = 0;
	int n = -5;
	
		
	lsb = read_Byte();
	msb = read_Byte();
	
	
	while(tmp++ < 8)
	{	
		n++;
		if(lsb & 0x01)
		{
			
				if(n < 0)
				{
						T_ds18b20 += 1/pow(2, abs(n));
				}
				else
				{
					T_ds18b20 += pow(2, abs(n));
				}
		}
	lsb = lsb >> 1;
	}
	
	tmp = 0;
	while(tmp++ < 3)
	{
		n++;
		if(msb & 0x01)
		{
				T_ds18b20 += pow(2, n);
		}
		
	msb = msb >> 1;
	}
}


char initial_ds18b20 ()
{	
	GPIOA ->BSRR |= GPIO_BSRR_BR0;
	DelayMicros(650);
	GPIOA ->BSRR |= GPIO_BSRR_BS0;
	DelayMicros(100);
	if (GPIOA->IDR & GPIO_IDR_IDR0)
	{	
		return 0;//not okey
	}
	else
	{	
		DelayMicros(2000);
		return 1;//okey
	}
}


void send_Byte (char tmp)
{
	char num = 0;
		while (num++ < 8)
	{
		GPIOA ->BSRR |= GPIO_BSRR_BR0;
		DelayMicros(5);
		
		if (tmp & 0x01)
		{
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		DelayMicros(70);
		}
		else
		{
		DelayMicros(70);
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		}
		DelayMicros(9);
		tmp = tmp >> 1;
	}

}
char read_Byte ()
{
	char tmp = 0;
	char num = 0;
		while (num++ < 8)
	{
		
		tmp = tmp >> 1;
		GPIOA ->BSRR |= GPIO_BSRR_BR0;
		DelayMicros(2);
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		DelayMicros(1);
		
		if(GPIOA ->IDR & GPIO_IDR_IDR0)
		{
		tmp += 128;
		}	
		DelayMicros(70);
	}
return tmp;
}

void send_Bit(char tmp)
{
		GPIOA ->BSRR |= GPIO_BSRR_BR0;
		DelayMicros(5);
		
		if (tmp == 1)
		{
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		DelayMicros(70);
		}
		else
		{
		DelayMicros(70);
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		}
		
		DelayMicros(9);
}

char recive_Bit()
{
		char tmp = 0;
		GPIOA ->BSRR |= GPIO_BSRR_BR0;
		DelayMicros(2);
		GPIOA ->BSRR |= GPIO_BSRR_BS0;
		DelayMicros(2);
		
		if(GPIOA ->IDR & GPIO_IDR_IDR0)
		{
		tmp = 1;
		}	
		DelayMicros(70);
		
		return tmp;
}







