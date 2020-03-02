/*
 * 485.c
 *
 *  Created on: 2019年8月27日
 *      Author: Administrator
 */
#include "485.h"
#include "string.h"
#include "mylcd.h"
#include "record.h"
extern UART_HandleTypeDef huart2;
extern QueueHandle_t disSignal;


extern struct detectSetStruct detectSetValue;
void gpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
}

void sendEnable(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET);
}

void sendDIsEnable(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);
}

char uartBuf[100];
int uart2RecLoc;
struct uartStruct{
	int funNum;
	int length;
	int addr;
	char data[10];
};
struct uartStruct  uartRecieveInfo;



void setLimit(int func,int value){
	switch(func){
		case 1: deviceOwnInfo.highPercent = value; break;
		case 2: deviceOwnInfo.lowPercent = value; break;
	}
	if(pageInfoList[5].isDis)
		xSemaphoreGive(disSignal);
}

void locationSolve(char * x){
	memcpy(deviceOwnInfo.loc, x, uartRecieveInfo.length);
	deviceOwnInfo.locInfoLenth = uartRecieveInfo.length/2;
}


struct deviceInfoInUart uartSendInfo ={
	.highLimitSetFlag = 1,//上电从探测器去读
	.lowLimitSetFlag = 1,
	.unitChangeFlag = 1,
	.zeroDemarcateFlag = 1,
	.spanDemarcateFlag = 1,
};
struct deviceInfoInUart uartSendInfoOther;

void uartDataSend(void){
	static int x = 0;
	x++;
	//高浓度查询
	if(!uartSendInfo.highLimitFlag&&x==1){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x01, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	//低浓度查询
	if(!uartSendInfo.lowLimitFlag&&x==2){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x02, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	//未收到标定信息
	if(!uartSendInfo.demarcateFlag&&x==3){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x03, 0x00, 0x00, 0x45, 0xcc};
 		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.zeroDemarcateFlag&&x==4){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x08, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.spanDemarcateFlag&&x==5){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x09, 0x00, 0x03, 0x00,0x00,0x00, 0x45, 0xcc};
		dataSend[7] = deviceOwnInfo.spanValue;
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
//	//位置查询
	if(!uartSendInfo.locFlag &&x==11){
		uint8_t dataSend[] = {0x76, 0xf5, 0x06, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.sensorValueFlag&&x==6){
		uint8_t  dataSend[] = {0x76, 0xf5, 0x07, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.unitFlag&&x==7){
		uint8_t dataSend[] = {0x76, 0xf5, 0x04, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}

	if(!uartSendInfo.highLimitSetFlag&&x==8){
		uint8_t dataSend[] = {0x76, 0xf5, 0x0B, 0x00, 0x03, 0x01, 0x00, 0x00,0x45, 0xcc};
		dataSend[5] = pageInfoList[0].unitInfo;
		dataSend[6] = deviceOwnInfo.highPercent /256;
		dataSend[7] = deviceOwnInfo.highPercent %256;
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.lowLimitSetFlag&&x==9){
		uint8_t dataSend[] = {0x76, 0xf5, 0x0c, 0x00, 0x03, 0x01, 0x00, 0x00,0x45, 0xcc};
		dataSend[5] = pageInfoList[0].unitInfo;
		dataSend[6] = deviceOwnInfo.lowPercent /256;
		dataSend[7] = deviceOwnInfo.lowPercent %256;
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}
	if(!uartSendInfo.unitChangeFlag&&x==10){
		uint8_t dataSend[] = {0x76, 0xf5, 0x0A, 0x00, 0x01, 0x00, 0x45, 0xcc};
		dataSend[5] = deviceOwnInfo.uint;
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}

	if(!uartSendInfo.measureFlag && x ==11)
	{
		uint8_t dataSend[] = {0x76, 0xf5, 0x05, 0x00, 0x00, 0x45, 0xcc};
		HAL_UART_Transmit(&huart2, dataSend, sizeof(dataSend), 1000);
	}

	if(x == 12)
		x = 0;
}



//operationRecord(zeroDemarcateRecordValue, deviceOwnInfo.addr);
void dataSolve(void){
	uartSendInfo.recNum++;
	int value =  uartRecieveInfo.data[1]*256 + uartRecieveInfo.data[2];
	if(value < 0)
		return;
	switch(uartRecieveInfo.funNum){
		case highLimit:
			if(uartRecieveInfo.addr)
				operationRecord(setHighLimitRecordValue, value);
			setLimit(highLimit, value);
			uartSendInfo.highLimitFlag = 1;
			break;
		case lowLimit:
			if(uartRecieveInfo.addr)
				operationRecord(setLowLimitRecordValue, value);
			setLimit(lowLimit, value);
			uartSendInfo.lowLimitFlag = 1;
			break;
		case hasDemarcate:
			if((uartRecieveInfo.data[0]*10 + uartRecieveInfo.data[1]) == 11){
				uartSendInfo.demarcateFlag = 1;
				deviceOwnInfo.noDemarcate = 0;
			}
			else{
				uartSendInfo.demarcateFlag = 0;
				deviceOwnInfo.noDemarcate = 1;
			}
			break;
		case zeroDemarcate:
			if(uartRecieveInfo.addr)
				operationRecord(zeroDemarcateRecordValue, 0);
			uartSendInfo.zeroDemarcateFlag = 1;
			break;
		case rangeQueries:
			deviceOwnInfo.measureRange = value;
			uartSendInfo.measureFlag = 1;
			if(uartRecieveInfo.addr)
				operationRecord(spanDemarcateRecordValue, deviceOwnInfo.measureRange);
			xSemaphoreGive(disSignal);
			break;
		case locQueries:
			uartSendInfo.locFlag = 1;
			locationSolve(uartRecieveInfo.data);break;
		case sensorValueQuery:
			if(!deviceOwnInfo.noDemarcate){
				if(value < 10000){
					deviceOwnInfo.sensorValue  = value;
					deviceOwnInfo.sensorFault = 0;
				}
				else{
					deviceOwnInfo.sensorFault = 1;
				}
			}
			if(pageInfoList[0].isDis)
				xSemaphoreGive(disSignal);
			break;
		case unitQueries:
			deviceOwnInfo.uint = uartRecieveInfo.data[0];
			if(uartRecieveInfo.addr)
				operationRecord(changeUnitRecordValue, deviceOwnInfo.uint);
			uartSendInfo.unitFlag = 1;
			if(pageInfoList[0].isDis||pageInfoList[5].isDis)
				xSemaphoreGive(disSignal);
			break;
		case spanDemarcate:
			if(uartRecieveInfo.addr)
				operationRecord(spanDemarcateRecordValue, 0);
			 uartSendInfo.spanDemarcateFlag = 1;
			 break;
		case unitChange: uartSendInfo.unitChangeFlag = 1;	break;
		case highLimitSet: uartSendInfo.highLimitSetFlag =1;	break;
		case lowLimitSet: uartSendInfo.lowLimitSetFlag =1;	break;
	}
}

void uartDataGet(void)
{
	int recOverFlag = 0;
	for(int i = 0; i < uart2RecLoc-1; i++){
		if(uartBuf[i] == 0x45 && 0xcc == uartBuf[i+1]){
			recOverFlag = 1;
		}
	}
	if(!recOverFlag)
		return;
	for(int i = 0; i < uart2RecLoc; i++){
		if(uartBuf[i] == 0x76 && uartBuf[i+1]== 0xf5){
			uartRecieveInfo.funNum = uartBuf[i+2];
			uartRecieveInfo.length = uartBuf[i+4];
			uartRecieveInfo.addr = uartBuf[i+3];
			memcpy(uartRecieveInfo.data, uartBuf+5+i, uartRecieveInfo.length);
			dataSolve();
			memset(uartBuf, 0, 100);
		}
	}
	uart2RecLoc = 0;
}

uint8_t temporayValue;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(uart2RecLoc == 100)
		uart2RecLoc = 0;
	uartBuf[uart2RecLoc] = temporayValue;
	while(HAL_UART_Receive_IT(&huart2,&temporayValue,1)!=HAL_BUSY);
	uart2RecLoc++;
}

