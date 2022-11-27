#include "stm32f10x.h"
#include "my_lib.h"
#include "interrupt.h"
#include "SPI.h"
#include "ds18b20.h"
#include "FLASH.h"

//#include "RTC.h"

extern char BUFF_RX[];
extern char BUFF_TX[];
extern float 	T_I2C;
extern double T_ADC;
extern float T_ds18b20;
extern uint32_t timer;
#define FLASH_ADDR 0x0801FC00

void Internal_Flash_Erase(unsigned int pageAddress) {
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
}

void Internal_Flash_Write(unsigned char* data, unsigned int address, unsigned int count) {
	unsigned int i;

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) {
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & FLASH_SR_EOP));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~(FLASH_CR_PG);
}

void ExINTx_PA3()
{
	AFIO->EXTICR[0] |= (AFIO_EXTICR1_EXTI3_PA );				//porta 3 pin
	EXTI->IMR |= (EXTI_IMR_MR3 );												//unmask int
	EXTI->FTSR |= (EXTI_FTSR_TR3 );											//falling edge
	NVIC_EnableIRQ (EXTI3_IRQn);	
}


int main (void) {
	SystemCoreClockConfigure();                             
  SystemCoreClockUpdate();
	
	RTC_INIT(); 
	RTC_SetCounter_(timer);

	
	FLASH_Unlock();
	FLASH_Erase_PageE(FLASH_ADDR);
	FLASH_Lock();
	
	ExINTx_PA3();
	
	//I2C          // PB6 - I2C1_SCL; PB7 - I2C1_SDA
	I2C1_Init();
	DMA_I2C_Init();
	//LM75A_Init();
	//LM75A_SP_Write();
	//I2C1->CR1 |= I2C_CR1_START;
	SSD1306_Init();
	
	// T_I2C
			
	//ADC					// PA1 - voltage
	ADC_Init();
	TIM2_Init ();
	// out T_ADC
	
	//ds18b20					// PA0 - data
	ds18b20 ();
	// out T_ds18b20
	
	//request
	TIM4_Init ();
	
	
	//SPI1 				//PA5 - SCK ,PA6 - MISO ,PA7 - MOSI, PB5 - CS
	SPI1_Init ();
	//DMA_SPI();
	//
	
	//USART     // PA9 - TX, PA10 - RX
	//USART_init();
	//DMI_init();
	//
	
	
	/*
	EEPROM_init();
	DelayMicros(5000);
	EEPROM_send();
	DelayMicros(5000);
	EEPROM_recive();
	
	
	DMA_SPI_TX_GO(letter, 8);
	*/
	
	while (1)
  {
		
  } 
}


