
#include "stm32f10x.h"


#define Freq 100000
#define Addr_Recive 		0x91
#define Addr_Transmite 	0x90
#define Addr_Transmite_LCD 	0b01111000
#define TIRE 	0b01010101

#define CFG 0

int ptt = 0;

char mass_char [13][8] = {
{0x00, 0x18, 0x18,0x18,0x18,0x18,0x18,0x00}, // =
{0x00, 0x00, 0x00,0xC0,0xC0,0x00,0x00,0x00}, // .
{0x00, 0x00, 0x00,0xC0,0xC0,0x00,0x00,0x00}, // /
{0x00, 0x7E, 0xFF,0xC3,0xC3,0xFF,0x7E,0x00}, // 0
{0x00, 0x00, 0x06,0x07,0xFF,0xFF,0x00,0x00}, // 1
{0x00, 0xF2, 0xFB,0xDB,0xDB,0xDF,0x8E,0x00}, // 2
{0x00, 0x5A, 0xDB,0xDB,0xDB,0xFF,0x7E,0x00}, // 3
{0x00, 0x1F, 0x1F,0x18,0x18,0xFF,0xFF,0x00}, // 4
{0x00, 0x4F, 0xCF,0xCB,0xCB,0xFB,0x73,0x00}, // 5
{0x00, 0x7E, 0xFF,0xCB,0xCB,0xFB,0x72,0x00}, // 6
{0x00, 0xC3, 0xE3,0x73,0x3B,0x1F,0x0F,0x00}, // 7
{0x00, 0x7E, 0xFF,0xCB,0xCB,0xFF,0x7E,0x00}, // 8
{0x00, 0x4E, 0xDF,0xD3,0xD3,0xFF,0x7E,0x00}};// 9



char letter_T [] = 
{0x00, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0x00};

char letter_U [] = 
{0x00, 0x7F, 0xFF,0xC0,0xC0,0xFF,0x7F,0x00};

char letter_NN [] = 
{0x00, 0x66,0x66,0x66,0x66,0x66,0x66,0x00};


float T_I2C;


extern float 	T_I2C;
extern float 	T_ADC;
extern float  T_ds18b20;

char BUFF_TX [1025];
char BUFF_RX [10];
int	 BUFF_DATA [2];

void TIM4_Init () {																 // TIM4 using for start conversion
	
	  RCC  -> APB1ENR |= RCC_APB1ENR_TIM4EN; 				 // TIM4 clock enable
		TIM4 -> CR1 = TIM_CR1_CEN;										 // TIM4 counter enable
	
		TIM4->PSC = (SystemCoreClock / 10000 ) - 1;	
		TIM4->ARR = 300;
				
		TIM4->DIER |= TIM_DIER_UIE;										 // Enable TIM4 interrupt
		NVIC_EnableIRQ (TIM4_IRQn);										 // Enable TIM4 interrupt	in interrupt controller
}

void I2C1_Init()
{
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->CFGR	|= RCC_CFGR_PPRE1_DIV2;
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; /* Enable GPIOB clock            */
	
	GPIOB->CRL	|= GPIO_CRL_CNF6;				//Alternative fan open drain 
	GPIOB->CRL	|= GPIO_CRL_MODE6;			//50 Mhz
	
	GPIOB->CRL	|= GPIO_CRL_CNF7;				//Alternative fan open drain 
	GPIOB->CRL	|= GPIO_CRL_MODE7;			//50 Mhz
	
	
		I2C1->CR2 |= I2C_CR2_LAST;//set next DMA EOT is last transfer
	
		I2C1->CR2 |= I2C_CR2_DMAEN; //enable DMA 
		
		I2C1->CCR |= 10;  //based on calculation
		I2C1->TRISE= 8; //output max rise 
	
		//I2C1->CR2 |= I2C_CR2_ITERREN;
		NVIC_EnableIRQ (I2C1_ER_IRQn);
				
		I2C1->CR1 |=I2C_CR1_PE;

}
	
	
void DMA_I2C_Init()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	//6 chennel //TX I2C
	DMA1_Channel6->CCR |= DMA_CCR6_MINC;
	DMA1_Channel6->CCR |= DMA_CCR6_DIR;
	DMA1_Channel6->CPAR = &I2C1->DR;
		
	DMA1_Channel6->CCR |= DMA_CCR6_TCIE;
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
		
	//7 chaneel //RX I2C
	
	DMA1_Channel7->CCR |= DMA_CCR7_MINC;
	DMA1_Channel7->CPAR = &I2C1->DR;
	
	DMA1_Channel7->CCR |= DMA_CCR7_TCIE;		//full transfer
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
}
	

void DMA_TX_GO(char *mass, int num) // 6 channel
{
	
	
	DMA1_Channel6->CMAR = mass;
	DMA1_Channel6->CNDTR = num;
	
	DMA1_Channel6->CCR |= DMA_CCR6_EN;

}

void DMA_RX_GO(char *mass, int num) // 7 channel
{
	
	DMA1_Channel7->CMAR = mass;
	DMA1_Channel7->CNDTR = num;
	
	DMA1_Channel7->CCR |= DMA_CCR7_EN;
}


void DMA1_Channel6_IRQHandler()
{
		DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
	
	while(!(I2C_SR1_BTF && I2C1->SR1)){;}
		
	I2C1->CR1 |= I2C_CR1_STOP;	

	DMA1->IFCR |= DMA_IFCR_CGIF6;

		
}	


void DMA1_Channel7_IRQHandler()  
{
	DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
	
	I2C1->CR1 |= I2C_CR1_STOP;
	DMA1->IFCR |= DMA_IFCR_CGIF7;
	
	//int T_tmp = 0;
	
	//T_tmp = BUFF_RX[0];
	
	//T_tmp <<= 1;
	//BUFF_RX[1] >>= 7;
	
	//T_tmp |= BUFF_RX[1];
	
	//T_I2C = ((float)T_tmp) * 0.5;
	

	}

void Send_Addr(char ADD)
{
		I2C1->CR1 |= I2C_CR1_START;
 
		while(!(I2C1->SR1&I2C_SR1_SB)){;}  
  
		DelayMicros(10);
		(void)I2C1->SR1;	//nado
			
		I2C1->DR = ADD;	
			
		while(((I2C1->SR1)&I2C_SR1_ADDR)==0){;}
		(void)I2C1->SR1; //nado
		(void)I2C1->SR2;
}

void Send_Byte(char byte)
{
	while(!(I2C1->SR1 && I2C_SR1_TXE)){;}
		I2C1->DR = byte;	
		//while(!(I2C_SR1_BTF && I2C1->SR1)){;}
	
}

void LM75A_Tos_Write(char Tos)
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
		
		BUFF_TX[0] = 	3;
		BUFF_TX[1] = 	Tos;	
		//BUFF_TX[2] = 	128;
		BUFF_TX[2] = 	0;	
		DMA_TX_GO(BUFF_TX, 3);
}

void LM75A_TH_Write(char TH)
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
		
		BUFF_TX[0] = 	2;
		BUFF_TX[1] = 	TH;
		BUFF_TX[2] = 	128;		
		DMA_TX_GO(BUFF_TX, 3);
}

LM75A_CONF_Write()
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
			
		BUFF_TX[0] = 	1;
		BUFF_TX[1] = 	CFG;
			
		DMA_TX_GO(BUFF_TX, 2);	
}

void LM75A_Init()
{
		LM75A_TH_Write(25);
		LM75A_Tos_Write(26);
}


void LM75A_SP_Write()
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
		
		BUFF_TX[0] = 	0;
		DMA_TX_GO(BUFF_TX, 1);
}



	
void LM75A_T_READ()
{
		I2C1->CR1 |= I2C_CR1_ACK;
		Send_Addr(Addr_Recive);	
		DMA_RX_GO(BUFF_RX, 2);
}

SSD1306_Write_CONF(char inst)
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite_LCD);
		BUFF_TX[0] = 	0x00;
		BUFF_TX[1] = 	inst;
			
		DMA_TX_GO(BUFF_TX, 2);	
}






void convert_f_to_char(double num, int start)
{
	ptt = start;
	char buff [6];
	sprintf (buff, "%f",num);
		
	ptt += 16;
	
	for(int i = 0; i < 6; i ++)
	{
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char[buff [i] - 0x2d][p];
		ptt++;
		}	
	}
}


void INIT_1(){	

	for(int p = 1; p < 1025; p++)
	{
		BUFF_TX[p] = 0;
	}
	
	for(int i = 1; i < 4; i++)
	{
		
		
		ptt = 128*(i-1)*2 + 128;
	

		
		
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_T[p];
		ptt++;
		}	
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char[i + 0x30 - 0x2d][p];
		ptt++;
		}	
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_NN[p];
		ptt++;
		}	
	}
}

void INIT_2(){		
	for(int i = 1; i < 4; i++)
	{
		ptt = 128*(i-1)*2 + 128;
	
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_T[p];
		ptt++;
		}	
		for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char[i + 0x30 - 0x2d][p];
		ptt++;
		}	
		BUFF_TX[ptt] = 0xFF;
		ptt++;
		
		for(int p = 0; p < 101; p++)
		{
		if(!(p % 10))
		{
			BUFF_TX[ptt] = 0xC3;
		}
		else
		{
			BUFF_TX[ptt] = 0x81;
		}
		
		ptt++;
		}

		BUFF_TX[ptt] = 0xFF;
	}
	
	ptt = 128*6 + 16;
	BUFF_TX[ptt] = TIRE;
	
	ptt = 128*6 + 16 + 102;
	BUFF_TX[ptt] = TIRE;
	
	ptt = 128*6 + 16 + 51;
	BUFF_TX[ptt] = TIRE;
	
	
	ptt = 128*7 ;
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['2' - 0x2d][p];
		ptt++;
		}	
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['0' - 0x2d][p];
		ptt++;
		}	
	

	ptt = 128*7 + 51;
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['3' - 0x2d][p];
		ptt++;
		}	
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['0' - 0x2d][p];
		ptt++;
		}	
		
	ptt = 128*7 + 95 + 8;
	//for(int p = 0; p < 8; p++)
	//	{
	//	BUFF_TX[ptt] = mass_char['1' - 0x2d][p];
	//	ptt++;
	//	}	
		
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['4' - 0x2d][p];
		ptt++;
		}	
		
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = mass_char['0' - 0x2d][p];
		ptt++;
		}	
	
}


void SSD1306_Write_DATA_1()
{
		INIT_1();
	
		convert_f_to_char(T_I2C, 128 + 10);
		convert_f_to_char(T_ADC, 128 * 3 + 10);
		convert_f_to_char(T_ds18b20, 128 * 5 + 10);
	
	//I2C1->CR1 |= I2C_CR1_STOP;
	int tmp = 0;
	while(I2C1->SR2&I2C_SR2_BUSY)
{
if(tmp++ == 10000) //kostil`
{
	goto a;
}
}
		Send_Addr(Addr_Transmite_LCD);
		
		DMA_TX_GO(BUFF_TX, 1024 + 1);	

a:__NOP();		
	
}



int normir(float tmp)
{
	int a = (tmp*100/(40 - 20)) - 100;
	
	if(a > 100)
	{
		return 100;
	}
	if(a < 0)
	{
		return 0;
	}
			return a;
		
}

void SSD1306_Write_DATA_2()
{
		INIT_2();
	
		ptt = 128 * 1 + 18;
		for(int p = 0; p < normir(T_I2C); p++)
		{
		BUFF_TX[ptt] |= 0xBD;
		ptt++;
		}
	
		ptt = 128 * 3 + 18;
		for(int p = 0; p < normir(T_ADC); p++)
		{
		BUFF_TX[ptt] |= 0xBD;
		ptt++;
		}
		
		ptt = 128 * 5 + 18;
		for(int p = 0; p < normir(T_ds18b20); p++)
		{
		BUFF_TX[ptt] |= 0xBD;
		ptt++;
		}
	
		
	int tmp = 0;
	while(I2C1->SR2&I2C_SR2_BUSY)
{
if(tmp++ == 10000) //kostil`
{
	goto a;
}
}
		Send_Addr(Addr_Transmite_LCD);
		
		DMA_TX_GO(BUFF_TX, 1024 + 1);	

a:__NOP();		
	
}



SSD1306_Init()
{
	

	SSD1306_Write_CONF(0xAE); //--turn on SSD1306 panel
	
	SSD1306_Write_CONF(0xD5); //--turn on SSD1306 panel
	SSD1306_Write_CONF(0x80); //--turn on SSD1306 panel
	
	SSD1306_Write_CONF(0x8D); //--turn on SSD1306 panel
	SSD1306_Write_CONF(0x14); //--turn on SSD1306 panel
	
	SSD1306_Write_CONF(0x40);
	
	SSD1306_Write_CONF(0xA1);
	
	SSD1306_Write_CONF(0xC8);
	
	SSD1306_Write_CONF(0xDA);
	SSD1306_Write_CONF(0x12);
	
	SSD1306_Write_CONF(0xA8);
	SSD1306_Write_CONF(63);
	
	SSD1306_Write_CONF(0xD3);
	SSD1306_Write_CONF(0);
	
	SSD1306_Write_CONF(0x20);
	SSD1306_Write_CONF(0);
	
	SSD1306_Write_CONF(0x21);
	SSD1306_Write_CONF(0);
	SSD1306_Write_CONF(127);
	
	SSD1306_Write_CONF(0x22);
	SSD1306_Write_CONF(0);
	SSD1306_Write_CONF(7);
		
	SSD1306_Write_CONF(0x81);
	SSD1306_Write_CONF(0x7F);		
	
	SSD1306_Write_CONF(0xA4);	
	
	SSD1306_Write_CONF(0xA6);

	SSD1306_Write_CONF(0xAF);	
	
	for(int i = 0; i < 1025; i++)
	{
		BUFF_TX[i] = 0x00;
	}
	
	BUFF_TX[0] = 	0x40;
	
		INIT_1();
		
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite_LCD);
		
		DMA_TX_GO(BUFF_TX, 1024 + 1);	
			
			
}
	
	
	