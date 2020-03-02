/*
 * wordLib.c
 *
 *  Created on: 2019年8月23日
 *      Author: Administrator
 */


#include"wordLib.h"
#include "stm32f4xx_hal_spi.h"
extern SPI_HandleTypeDef hspi1;;
uint8_t    FontBuf[128];//字库缓存
uint8_t    Font_Map[128];//字库缓存

#define CSH 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET)
#define CSL 		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, RESET)
#define Rom_SCKL 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, RESET)
#define Rom_SCKH 	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET)
#define Rom_INH  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET)
#define Rom_INL  	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, RESET)
#define SDO_ZK 		HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)

void CL_Mem(void)
{
	uint8_t  i;
	for(i=0;i<128;i++ )
	{
	  Font_Map[i]=0;
	}
}
void WORDInit(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_5|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6 ;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void send_command_to_ZK( uint8_t dat )
{
	uint8_t i;
	for(i=0;i<8;i++ )
	{
		if(dat&0x80){
			Rom_INH;
		}
		else
			Rom_INL;
		dat = dat<<1;
		Rom_SCKL;
		Rom_SCKH;
	}
}

uint8_t ZK_read()
{
	unsigned char cnt_clk;
	unsigned char ret_data=0;

	for( cnt_clk = 0; cnt_clk < 8; cnt_clk++ )
	{
		Rom_SCKL;  //字库时钟拉低
		ret_data=ret_data<<1;
		if( SDO_ZK ){
			ret_data=ret_data+1;
		}
		else{
			ret_data=ret_data+0;
		}

		Rom_SCKH; //字库时钟拉高
	 	}
	return(ret_data);
}

void ZK_Read_1_n(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pBuff,uint8_t DataLen )
{
	unsigned char i;
	CSL;
	Rom_SCKL;
	send_command_to_ZK(0x03);
	send_command_to_ZK(addrHigh);
	send_command_to_ZK(addrMid);
	send_command_to_ZK(addrLow);
	for(i = 0; i < DataLen; i++ )
  		 *(pBuff+i)=ZK_read();
	CSH;
}



uint16_t word[32*32] = {};
uint16_t wordBak[32*32] = {};

void zk_map(  uint8_t *getdate, uint8_t *putdata, int oop)	 //byte
{
	for(int j = 0; j < oop*oop; j++)
		wordBak[j] = 0x2945;
	for(int j = 0; j < oop*oop/8; j++)
		for(int i = 0; i < 8; i++)
		{
			if(putdata[j]&(0x80>>i))
				wordBak[i+j*8] = 0xffff;
			else
				wordBak[i+j*8] = 0x2945;
		}
	for(int j = 0; j < oop; j++)
		for(int i = 0; i < oop; i++){
			word[i*oop+oop-j-1] = wordBak[j*oop+i];
	}
}

//HAL_CAN_Transmit
char * getGB2312Word(uint32_t  x,uint32_t  y, uint8_t * text, int size)
{

	uint8_t  i= 0;
	uint8_t  AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	unsigned long  FontAddr=0; //字地址
	unsigned long  BaseAdd=0; //字库基地址
	uint8_t  n,h,w,d;// 不同点阵字库的计算变量
	switch(size)
	{  // n个数，h：字高度，w：字宽度， d：字间距，c：页大小
		case 12:  BaseAdd=0x0;     n=24;  h=12; w=12; d=12;   break;  // 12*12
		case 16:  BaseAdd=0x2C9D0; n=32;  h=16; w=16; d=16;   break;   // 15*16
		case 24:  BaseAdd=0x68190; n=72;  h=24; w=24; d=24;   break;   // 24*24
		case 32:  BaseAdd=0xEDF00; n=128; h=32; w=32; d=32;   break;   // 32*32
	}

	if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
	{
		/*国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：*/
		/*Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址*/
		FontAddr = (text[i]- 0xA1)*94;
		FontAddr += (text[i+1]-0xA1);
		FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);

		AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
		AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
		AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
		ZK_Read_1_n(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
		zk_map(Font_Map,FontBuf,size);//字符转换
		x=x+d; //下一个字坐标
		i+=2;  //下个字符
	}
	else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
	{
		/*国标简体（GB2312） 字库IC中的地址由以下公式来计算：*/
		/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址*/

		FontAddr = (text[i]- 0xB0)*94;
		FontAddr += (text[i+1]-0xA1)+846;
		FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);

		AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
		AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
		AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
		ZK_Read_1_n(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[ ]"*/
		zk_map(Font_Map,FontBuf,size);//字符转换
		x=x+d; //下一个字坐标
		i+=2;  //下个字符
	}
	return word;
}





