#include "stm32f10x.h"

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

#define FLASH_ADDR 0x0801FC00

void FLASH_Unlock(void)
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}	


void FLASH_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}

void FLASH_InitT(void) {
	
		FLASH->ACR |= FLASH_ACR_HLFCYA;
    FLASH->ACR |= FLASH_ACR_PRFTBS;      
   	FLASH->ACR |= FLASH_ACR_LATENCY_2;   
	
}



_Bool flash_ready(void) 
{
	return !(FLASH->SR & FLASH_SR_BSY);
}

_Bool check_EOP(void)
{
	if(FLASH->SR & FLASH_SR_EOP)
	{	
		FLASH->SR |= FLASH_SR_EOP;
		return 1;
	}	
	return 0;
}





int FLASH_Erase_PageE(uint32_t address) 
{
	while(!flash_ready()); //??????? ?????????? ????? ? ??????
	
	FLASH->CR|= FLASH_CR_PER; //????????????? ??? ???????? ????? ????????
	FLASH->AR = address; // ?????? ?? ?????
	FLASH->CR|= FLASH_CR_STRT; // ????????? ????????
	while(!flash_ready());  //???? ???? ???????? ????????
	FLASH->CR &= ~FLASH_CR_PER; //?????????? ??? ???????? ????? ????????
	
	return check_EOP();//???????? ?????????, ??????? ????
}

void FLASH_Program(uint32_t address, uint32_t data)
{
	while(!flash_ready()); //??????? ?????????? ????? ? ??????
	
	FLASH->CR |= FLASH_CR_PG; //????????? ???????????????? ?????
	*(__IO uint16_t*)address = (uint16_t)data; //????? ??????? 2 ?????
	while(!flash_ready());//???? ?????????? ????????
	//if(!check_EOP())return 0;
	
	address+=2;//?????????? ? ?????? ??? ?????
	data>>=16;//???????? ??????
	*(__IO uint16_t*)address = (uint16_t)data; //????? ??????? 2 ?????
	while(!flash_ready());//???? ?????????? ????????
	FLASH->CR &= ~(FLASH_CR_PG); //????????? ???????????????? ?????
	
	//return check_EOP();
}




