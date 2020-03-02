/*
 * DS1302.h
 *
 *  Created on: 2019年8月23日
 *      Author: Administrator
 */

#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"
#include "cmsis_os.h"
extern uint8_t ascii_time[7];     //保存ascii格式数据

extern uint8_t bcd_time[7];       //保存bcd码数据

typedef struct
{
	uint16_t Year;
	uint8_t   Month;
	uint8_t   Day;
	uint8_t	 Hour;
	uint8_t    Min;
	uint8_t    Sec;
} DateTime;
extern  DateTime datatime;

#define DS1302_DATIN HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)
#define DS1302_DATOUT(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, x)

#define DS1302_DAT_INPUT()  Ds1302_sda_input();
#define DS1302_DAT_OUTPUT()  Ds1302_sda_output();

#define DS_CEL HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0)
#define DS_CLKL HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0)
#define DS_CEH HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1)
#define DS_CLKH HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1)

void DS1302_Init(void);
void Ds1302_Gpio_Init(void);
void DS1302_WriteByte(uint8_t dat);
void DS1302_Write(uint8_t address, uint8_t dat);
uint8_t DS1302_ReadByte(void);
uint8_t DS1302_Read(uint8_t address);
void DS1302_ReadTime(DateTime *datatime);
void DS1302_SetTime(DateTime datatime);
#endif
