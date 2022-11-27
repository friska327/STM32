#include "stm32f10x.h"

char mass[100];

void SystemCoreClockConfigure(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

  RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock
	
  RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

   RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
  RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL7);

  RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

  RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}


void GPIO_Init (void) {

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                /* Enable GPIOA clock            */

	/* Configure alt fun (PA.0) pins as (input)???? */ 
	GPIOA->CRL &= ~GPIO_CRL_MODE0; // input
	GPIOA->CRL &= ~GPIO_CRL_CNF0;
	GPIOA->CRL |=  GPIO_CRL_CNF0_1; 	//pull up
	GPIOA->BSRR = GPIO_BSRR_BS0 ;
	
	/* Configure alt fun (PA.1) pins as output (output) */
	GPIOA->BSRR = GPIO_BSRR_BR1 ;
	GPIOA->CRL &= ~GPIO_CRL_MODE1;
	GPIOA->CRL |= GPIO_CRL_MODE1_1; // output
	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	GPIOA->CRL |= GPIO_CRL_CNF1_0;	//output open drain
	
  /* Configure LED (PA.5) pins as output  */
 	GPIOA->CRL 	 &=  ~GPIO_CRL_CNF5_0;
	GPIOA->CRL 	 &=  ~GPIO_CRL_CNF5_1;
	GPIOA->CRL   |=  GPIO_CRL_MODE5_0;
	
}

