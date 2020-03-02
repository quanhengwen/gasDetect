/*
 * DS1302.c
 *
 *  Created on: 2019年8月23日
 *      Author: Administrator
 */

#include "DS1302.h"
DateTime datatime;
void DS1302_Delay(uint16_t time)
{
	uint16_t temp;
	for(temp = 0; temp < time; temp++);
}

//struct DateTime ds1302Data = {0,0,0,0,0,0,0};
uint8_t ascii_time[7] = {0};     //保存ascii格式数据

uint8_t bcd_time[7] = {0};       //保存bcd码数据

static uint8_t AsciiToBcd(uint8_t asciiData)
{
	uint8_t bcdData = 0;
    bcdData = (((asciiData/10)<<4)|((asciiData%10)));
    return bcdData;
}

static uint8_t BcdToAscii(uint8_t bcdData)
{
	uint8_t asciiData = 0;
    asciiData = (((bcdData&0xf0)>>4)*10 + (bcdData&0x0f));
    return asciiData;
}

//IO口初始化
void Ds1302_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Ds1302_sda_input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void Ds1302_sda_output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS1302_Init(void)
{
    DS1302_Write(0x8E,0x00);  //取消写保护
    DS1302_Write(0x80,AsciiToBcd(6));  //秒
    DS1302_Write(0x82,AsciiToBcd(6));  //分
    DS1302_Write(0x84,AsciiToBcd(6));  //时
    DS1302_Write(0x86,AsciiToBcd(23));  //日
    DS1302_Write(0x88,AsciiToBcd(8));    //月
    DS1302_Write(0x8c,AsciiToBcd(19));  //年
    DS1302_Write(0x8E,0x80);  //打开写保护
}

void DS1302_SetTime(DateTime datatime)
{
  taskENTER_CRITICAL();
  DS1302_Write(0x8E,0x00);  //取消写保护
  DS1302_Write(0x80,00);
  DS1302_Write(0x80,AsciiToBcd(datatime.Sec));  //秒
  DS1302_Write(0x82,AsciiToBcd(datatime.Min));  //分
  DS1302_Write(0x84,AsciiToBcd(datatime.Hour));  //时
  DS1302_Write(0x86,AsciiToBcd(datatime.Day));  //日
  DS1302_Write(0x88,AsciiToBcd(datatime.Month));//月
  DS1302_Write(0x8c,AsciiToBcd(datatime.Year));  //年
  DS1302_Write(0x8E,0x80);  //打开写保护
  taskEXIT_CRITICAL();
}

//写入一个字节
void DS1302_WriteByte(uint8_t dat)
{
  uint8_t i = 0;

  DS1302_DAT_OUTPUT();
  DS_CEH;

  for(i = 8; i > 0; i--)
  {
    if(dat& 0x01)
    	DS1302_DATOUT(1);
    else
    	DS1302_DATOUT(0);
    DS_CLKL;
    DS1302_Delay(10);
    DS_CLKH;
    DS1302_Delay(10);
    dat >>= 1;
  }
}

//写入一个寄存器
void DS1302_Write(uint8_t address, uint8_t dat)
{
	DS_CEL;
	DS_CLKL;
	DS_CEH;

	DS1302_WriteByte(address);
  DS1302_WriteByte(dat);

	DS_CLKH;
	DS_CEL;
}

//读取一个字节
uint8_t DS1302_ReadByte(void)
{
  uint8_t i = 0, dat = 0;
  DS_CEH;
  DS1302_DAT_INPUT();

  for(i=8; i>0; i--)
  {
    DS_CLKH;
    DS1302_Delay(10);
    DS_CLKL;
    DS1302_Delay(10);
    dat >>= 1;
    if(DS1302_DATIN) dat |= 0x80;
  }
  return dat;
}

//读取一个寄存器
uint8_t DS1302_Read(uint8_t address)
{
  uint8_t data = 0;
  DS_CEL;
  DS_CLKL;
  DS_CEH;

  DS1302_WriteByte(address);
  data = DS1302_ReadByte();

  DS_CLKH;
  DS_CEL;
  return data;
}
extern int timeData[];
void DS1302_ReadTime(DateTime *datatime)
{
	taskENTER_CRITICAL();
	timeData[5] = datatime->Sec = BcdToAscii(DS1302_Read(0x81));  //秒
	timeData[4] = datatime->Min = BcdToAscii(DS1302_Read(0x83));  //分
	timeData[3] = datatime->Hour = BcdToAscii(DS1302_Read(0x85));   //时
	timeData[2] = datatime->Day = BcdToAscii(DS1302_Read(0x87)); //日
	timeData[1] = datatime->Month = BcdToAscii(DS1302_Read(0x89)); //月
	timeData[0] = datatime->Year = BcdToAscii(DS1302_Read(0x8D)); //年
	timeData[0] += 2000;
	taskEXIT_CRITICAL();
}
