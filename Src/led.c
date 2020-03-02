/*
 * led.c
 *
 *  Created on: 2019年8月26日
 *      Author: Administrator
 */
#include "led.h"
#include "main.h"
#include "cmsis_os.h"
#include "mylcd.h"
void ledGpioInit(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ERRPORT, ERRPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(POWERLEDPORT, POWERLEDPIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = NORMALPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(NORMALPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = ERRPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ERRPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = HALARMPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(HALARMPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LALARMPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LALARMPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = POWERLEDPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(POWERLEDPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = BuzzerPIN;;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BuzzerPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = MAINPOWERDETECTPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(MAINPOWERDETECTPORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = BAKPOWERDETECTPIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BAKPOWERDETECTPORT, &GPIO_InitStruct);
}

void allLedOn(void){
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ERRPORT, ERRPIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_RESET);
	buzzerOn();
}
int alamBan = 1;
void buzzerOn(void){
	if(alamBan == 1)
		HAL_GPIO_WritePin(BuzzerPORT, BuzzerPIN, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(BuzzerPORT, BuzzerPIN, GPIO_PIN_RESET);
}

void buzzerOff(void){
	HAL_GPIO_WritePin(BuzzerPORT, BuzzerPIN, GPIO_PIN_RESET);
}

void allLedOff(void){
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ERRPORT, ERRPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
	buzzerOff();
}

void normal(void)
{
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_RESET);
	buzzerOff();
}

void HALARM(void)
{
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_RESET);
	buzzerOn();
	osDelay(100);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
}
void LALARM(void)
{
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
	buzzerOn();
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_RESET);
	osDelay(100);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
	buzzerOff();
}

void hardWareFault(void){
	HAL_GPIO_WritePin(NORMALPORT, NORMALPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LALARMPORT, LALARMPIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HALARMPORT, HALARMPIN, GPIO_PIN_SET);
	buzzerOn();
	HAL_GPIO_WritePin(ERRPORT, ERRPIN, GPIO_PIN_RESET);
	osDelay(100);
	HAL_GPIO_WritePin(ERRPORT, ERRPIN, GPIO_PIN_SET);
	buzzerOff();
}
void powerLedOn(void){
	HAL_GPIO_WritePin(POWERLEDPORT, POWERLEDPIN, GPIO_PIN_SET);
}

void powerLedOff(void){
	HAL_GPIO_WritePin(POWERLEDPORT, POWERLEDPIN, GPIO_PIN_RESET);
}
extern struct deviceInfo deviceOwnInfo;
void powerFault(void){
	powerLedOn();
	buzzerOn();
	osDelay(100);
	powerLedOff();
	if(deviceOwnInfo.sensorValue < deviceOwnInfo.currentLowLimit)
		buzzerOff();
}

int mainPowerNormal(void){
	return HAL_GPIO_ReadPin(MAINPOWERDETECTPORT, MAINPOWERDETECTPIN);
}

int bakPowerNormal(void){
	return HAL_GPIO_ReadPin(BAKPOWERDETECTPORT, BAKPOWERDETECTPIN);
}
