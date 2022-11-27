#include "stm32f10x.h"
#include "my_lib.h"
#include "ds18b20.h"
#include "SPI.h"
#include "RTC.h"


#define FLASH_ADDR 0x0801FC00

			float BUFFER_TX[400];
			float BUFFER_RX[400];
			int		ptr = 0;
extern char RX_BUFF[];
extern float 	T_I2C;
extern float 	T_ADC;
extern float  T_ds18b20;

char flag = 0;

unix_cal unix_time;


union simple
{
		float number;
    char num[4];
};

union convert
{
		float number_float;
    int		number_int;
};


union simple tmpp;
union convert tmpp_conv;


void TIM4_IRQHandler () {													 // LM75A
		TIM4->SR &= ~TIM_SR_UIF;	
		//LM75A_T_READ();
		convert();
		read_T();
	//
		//timer = RTC_GetCounter_();
		timer_to_cal (RTC_GetCounter_(), &unix_time); 

	
		if(ptr > 197)
		{
			ptr = 0;
			FLASH_Unlock();  
			FLASH_Erase_PageE(FLASH_ADDR);
			FLASH_Lock(); 
		}
		
		tmpp.num[0] = unix_time.sec;
		tmpp.num[1] = unix_time.min;
		tmpp.num[2] = unix_time.hour;
		tmpp.num[3] = unix_time.mday;
		
		BUFFER_TX[ptr++] = tmpp.number;
		BUFFER_TX[ptr++] = T_I2C;
		BUFFER_TX[ptr++] = T_ADC;
		BUFFER_TX[ptr++] = T_ds18b20;
			

		
		FLASH_Unlock();  
		for(int i = 0; i < 4; i++)
		{
			tmpp_conv.number_float = BUFFER_TX[ptr - 4 + i];
			FLASH_Program((FLASH_ADDR + (ptr - 4 + i) * 4)  ,tmpp_conv.number_int);
			WriteData((ptr - 4 + i) * 4, &BUFFER_TX[ptr - 4 + i]  , 4);
			DelayMicros(5000);
			  //WriteData(0, BUFFER_TX , 20);
						
		}
		FLASH_Lock();   
		
		//ReadData(0, BUFFER_RX, 800);
		if(flag == 1)
		{
			SSD1306_Write_DATA_2();
		}
		else
		{
			SSD1306_Write_DATA_1();
		}
		
}


void I2C1_ER_IRQHandler()
{
	if(I2C1->SR1 && I2C_SR1_AF)
	{
	
	//TIM4->SR &= ~TIM_SR_UIF;
	I2C1->CR1 |= I2C_CR1_STOP;
		
	DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
	DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
		
	}
}

void USART1_IRQHandler() // IDLE
{

		if(RX_BUFF[0] == '1')
		{
			ReadData(0, BUFFER_RX, 1200);
		}
		else
		{
			for(int i = 0; i < 400; i++)
			{
				WriteData(i * 4, &(BUFFER_TX[i])  , 4);
			  //WriteData(0, BUFFER_TX , 20);
				DelayMicros(5000);
			}
		}
			
		//IDLE interrupt
		USART1->SR; //IDLE bit clear
		USART1->DR; //
	
		DMI_channel5_go(RX_BUFF, BUFF_SIZE);//RX mess
}

void EXTI3_IRQHandler ()
{
	EXTI->PR |= EXTI_PR_PR3;
	flag ^= 1;	
}