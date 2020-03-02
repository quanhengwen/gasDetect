/*
 * record.h
 *
 *  Created on: 2019年8月26日
 *      Author: Administrator
 */

#ifndef RECORD_H_
#define RECORD_H_
#include "main.h"
#include "ds1302.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "stdio.h"

struct myFileInfo{
	uint8_t * fileName;
	uint32_t lineAddr;
	uint8_t hasSet;
};

struct recordInfo{
	uint8_t time[12];
	uint8_t operationFunc;
	uint32_t operationValue;
};

enum{
	//警告记录常量
	highWarnRecordValue,
	lowWarnRecordValue,
	MainPowerlowRecordValue,
	BakPowerlowRecordValue,
	//硬件故障常量
	sensorFaultValue,
	disconnectRecordValue,
	noDemarcateRecordValue,
	//操作常量
	changePassWordRecordValue,
	setHighLimitRecordValue,
	setLowLimitRecordValue,
	setBuzzerSilenceRecordValue,
	setResetRecordValue,
	spanDemarcateRecordValue,
	zeroDemarcateRecordValue,
	changeAddrRecordValue,
	changeUnitRecordValue,
	changeDisRecordValue,
	changeMangerRecordValue,
	changeTimeRecordValue,
	selfTest,
};


void recordCheck(void);
void operationRecord(int operationValue, int xx);
void warnRecord(int operationValue, int xx);
void faultRecord(int operationValue, int xx);
int fileGetLine(char * fileName, int i, struct recordInfo * p);

#endif /* RECORD_H_ */
