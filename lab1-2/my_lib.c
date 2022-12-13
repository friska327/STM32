#include "stm32f10x.h"




void ExINTx_PA0()
{
	AFIO->EXTICR[0] |= (AFIO_EXTICR1_EXTI0_PA );				//porta 0 pin
	EXTI->IMR |= (EXTI_IMR_MR0 );												//unmask int
	EXTI->FTSR |= (EXTI_FTSR_TR0 );											//falling edge
	NVIC_EnableIRQ (EXTI0_IRQn);	
}


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

void TIM4_Init () {																 // TIM4 using for start conversion
	  RCC  -> APB1ENR |= RCC_APB1ENR_TIM4EN; 				 // TIM4 clock enable
		//TIM4 -> CR1 = TIM_CR1_CEN;										 // TIM4 counter enable
	
		TIM4->PSC = (SystemCoreClock / 10000 ) - 1;	
		TIM4->ARR = 1000;
				
		TIM4->DIER |= TIM_DIER_UIE;										 // Enable TIM4 interrupt
		NVIC_EnableIRQ (TIM4_IRQn);										 // Enable TIM4 interrupt	in interrupt controller
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

void DELAY_us (int us)
{		
		SysTick->LOAD = us * (SystemCoreClock/1000000);
		SysTick->VAL = 10;
		SysTick->CTRL |= (SysTick_CTRL_ENABLE |SysTick_CTRL_TICKINT| SysTick_CTRL_CLKSOURCE);

		while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
		SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG | SysTick_CTRL_ENABLE);
}

