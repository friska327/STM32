#include "stm32f10x.h"
#include "my_lib.h"

extern char RX_BUFF[];
char	TX_BUFF [30];

char 	cnt = 0;
int		data[3] = {0,0,0};


void SysTick_Handler()
{
	
}

void USART1_IRQHandler()
{

	
			//DELAY_us(15000);	//interrupt fuster then DMA update CNDTR(
			char tmp = BUFF_SIZE - DMA1_Channel5->CNDTR;
			
			for(int i = 0; i < tmp; i++)
			{
				data[cnt] += (RX_BUFF[i] - 0x30) * power_10(tmp - i - 1);
			}

			
			if (cnt == 0)
			{
				RX_BUFF [tmp + 0] = '*';
				RX_BUFF [tmp + 1] = '2';
				RX_BUFF [tmp + 2] = '^';
				DMI_channel4_go(RX_BUFF, tmp + 3);
			}
			else
			{
				RX_BUFF [tmp + 0] = '*';
				RX_BUFF [tmp + 1] = '3';
				RX_BUFF [tmp + 2] = '=';
				DMI_channel4_go(RX_BUFF, tmp + 3);
			}
			
			while(DMA1_Channel4->CNDTR != 0);
			
			
			
			if(++cnt == 2)
			{	
				
					int p = 0;
					cnt = 0;
					data[0] *= 2;
					data[1] *= 3;
					data[2] = data[0]^data[1];
						
					data[0] = 0;
					data[1] = 0;
				
				
					while(data[2] != 0)
					{
						TX_BUFF[p] = (data[2] % 10) + 0x30;
						data[2] /= 10;
						p++;
					}
										
					char tmp = 0;
					char t = 0;
					while (t < (p/2))
					{
						tmp = TX_BUFF[t];
						TX_BUFF[t] = TX_BUFF[p - 1];
						TX_BUFF[p - 1] = tmp;	
						t++;						
					}
					
					
					TX_BUFF[p] = '\n';
					DMI_channel4_go(TX_BUFF, p + 1);//TX mess
			}
			
		DMI_channel5_go(RX_BUFF, BUFF_SIZE);//RX mess
		
			
		//IDLE interrupt
		USART1->SR; //IDLE bit clear
		USART1->DR; //
}


void DMA1_Channel4_IRQHandler()
{
	DMI_channel5_go(RX_BUFF, BUFF_SIZE);//RX mess
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
}
