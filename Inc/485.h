/*
 * 485.h
 *
 *  Created on: 2019年8月27日
 *      Author: Administrator
 */

#ifndef uart485_H_
#define uart485_H_

#include "main.h"
#include "cmsis_os.h"

extern int uart2RecLoc;
struct deviceInfoInUart{
	int recNum;
	int zeroDemarcateFlag;
	int spanDemarcateFlag;
	int highLimitFlag;
	int lowLimitFlag;
	int demarcateFlag;
	int locFlag;
	int sensorValueFlag;
	int unitFlag;
	int unitChangeFlag;
	int highLimitSetFlag;
	int lowLimitSetFlag;
	int measureFlag;
};

extern struct deviceInfoInUart uartSendInfo;

enum{
	alarmNormal,
	alarmLow,
	alarmHigh,
	alarmHardfault,
	alarmNoDemarcate
};

enum {
	 highLimit = 1,
	 lowLimit,
	 hasDemarcate,
	 unitQueries,
	 rangeQueries,
	 locQueries,
	 sensorValueQuery,
	 zeroDemarcate,
	 spanDemarcate,
	 unitChange,
	 highLimitSet,
	 lowLimitSet,
};

void gpioInit(void);
void sendEnable(void);
void sendDIsEnable(void);
void uartDataGet(void);
void uartDataSend(void);
#endif /* 485_H_ */
