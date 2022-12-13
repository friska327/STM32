
#include "stm32f10x.h"
#include "stdio.h"

#define Freq 100000
#define Addr_Recive 		0x91
#define Addr_Transmite 	0b01111000
//#define Addr_Transmite 	0b01111010
#define CFG 0

extern double U;
extern double TT;

float T;

int ptt = 0;

char BUFF_TX [1025];
int	 BUFF_DATA [2];


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


void I2C1_Init()
{
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->CFGR	|= RCC_CFGR_PPRE1_DIV2;
	
			
		I2C1->CR2 |= I2C_CR2_LAST;//set next DMA EOT is last transfer
	
		I2C1->CR2 |= I2C_CR2_DMAEN; //enable DMA 
	
		//I2C1->CR2 &= ~I2C_CR2_FREQ;
		//I2C1->CR2 |=	14;	
	
	
		I2C1->CCR |= 100;  //based on calculation
		I2C1->TRISE= 72; //output max rise 
	
		//I2C1->CR2 |= I2C_CR2_ITBUFEN;
		I2C1->CR2 |= I2C_CR2_ITERREN;
		//I2C1->CR2 |= I2C_CR2_ITEVTEN;
		NVIC_EnableIRQ (I2C1_ER_IRQn);
				
		I2C1->CR1 |=I2C_CR1_PE;

}
	
	
void DMA_Init()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	
	//6 chennel //TX I2C
	DMA1_Channel6->CCR |= DMA_CCR6_MINC;
	DMA1_Channel6->CCR |= DMA_CCR6_DIR;
	DMA1_Channel6->CPAR = &I2C1->DR;
		
	DMA1_Channel6->CCR |= DMA_CCR6_TCIE;
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	
}
	

void DMA_TX_GO(char *mass, int num) // 6 channel
{
	
	
	DMA1_Channel6->CMAR = mass;
	DMA1_Channel6->CNDTR = num;
	
	DMA1_Channel6->CCR |= DMA_CCR6_EN;

}


void DMA1_Channel6_IRQHandler()
{
	DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
	
	while(!(I2C_SR1_BTF && I2C1->SR1)){;}
		
	I2C1->CR1 |= I2C_CR1_STOP;	
	DMA1->IFCR |= DMA_IFCR_CTCIF6;
}	


void Send_Addr(char ADD)
{
		I2C1->CR1 |= I2C_CR1_START;
 
		while(!(I2C1->SR1&I2C_SR1_SB)){;}  
  
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
}



SSD1306_Write_CONF(char inst)
{
		while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
		BUFF_TX[0] = 	0x00;
		BUFF_TX[1] = 	inst;
			
		DMA_TX_GO(BUFF_TX, 2);	
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
	
	ptt = 128;
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_U[p];
		ptt++;
		}	
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_NN[p];
		ptt++;
		}	

		ptt = 384;
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_T[p];
		ptt++;
		}	
	for(int p = 0; p < 8; p++)
		{
		BUFF_TX[ptt] = letter_NN[p];
		ptt++;
		}	

		
}




void convert_f_to_char(double num)
{
	
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


void SSD1306_Write_DATA()
{
			
		ptt = 128;
		convert_f_to_char(U);
		
		ptt = 384;
		convert_f_to_char(TT);
	
	while(I2C1->SR2&I2C_SR2_BUSY){;}
		Send_Addr(Addr_Transmite);
		
		DMA_TX_GO(BUFF_TX, 1024 + 1);	
		//DMA_TX_GO(BUFF_TX, 1020 + 1);	//krasivoe
}


