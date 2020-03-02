/*
 * record.c
 *
 *  Created on: 2019年8月26日
 *      Author: Administrator
 */

#include "record.h"
#include "mylcd.h"
#include "string.h"



int curFileRecordLine;
char newCountFile[40];
char oldCountFile[40];
char recordFile[20];

uint8_t retSD;    /* Return value for SD */
char SDPath[4] = "0:";   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

DIR dir;
FILINFO fno;
uint16_t passWord = 1234;

void recordCheck(void){
	if(f_opendir(&dir, "0:") == FR_OK){
		while(f_readdir(&dir, &fno) == FR_OK){
				//有密码记录文件
			 if(fno.fname[0] == 'P'){
				 passWord = (fno.fname[1]-0x30)*1000+(fno.fname[2]-0x30)*100+(fno.fname[3]-0x30)*10+(fno.fname[4]-0x30);
				 break;
			 }
			 if(fno.fname[0] == 0){
				 f_open(&SDFile, "P1234.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
				 f_close(&SDFile);
				 break;
			 }
		}
	}
	f_open(&SDFile, "Opera.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_close(&SDFile);
	f_open(&SDFile, "Fault.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_close(&SDFile);
	f_open(&SDFile, "Warning.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_close(&SDFile);
	f_closedir(&dir);
}

#define clean(x) for(int i=0; i < sizeof(x); i++){if(x[i] == ' ') x[i] = '0' ;}
void operationRecord(int operationValue, int xx){
	char x[50];
	if(datatime.Year == 0)
		return;
	static int oRecordLine;
	oRecordLine++;
	sprintf(x, "%4d%2d%2d%2d%2d-%2d-%5d\r\n", datatime.Year,datatime.Month,datatime.Day,datatime.Hour, datatime.Min,operationValue, xx);
	clean(x);
	f_open(&SDFile, "Opera.txt",FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_puts(x, &SDFile);
	f_close(&SDFile);
}

void warnRecord(int operationValue, int xx){
	char x[50];
	if(datatime.Year == 0)
		return;
	static int oRecordLine;
	oRecordLine++;
	sprintf(x, "%4d%2d%2d%2d%2d-%2d-%5d\r\n", datatime.Year,datatime.Month,datatime.Day,datatime.Hour, datatime.Min,operationValue, xx);
	clean(x);
	f_open(&SDFile, "Warning.txt",FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_puts(x, &SDFile);
	f_close(&SDFile);
}


void faultRecord(int operationValue, int xx){
	char x[50];
	if(datatime.Year == 0)
		return;
	static int oRecordLine;
	oRecordLine++;
	sprintf(x, "%4d%2d%2d%2d%2d-%2d-%5d\r\n", datatime.Year,datatime.Month,datatime.Day,datatime.Hour, datatime.Min,operationValue, xx);
	clean(x);
	f_open(&SDFile, "Fault.txt",FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
	f_puts(x, &SDFile);
	f_close(&SDFile);
}

FIL bak;
char xxxx[200];
int fileGetLine(char * fileName, int i, struct recordInfo * p){
	p->operationValue = 0;
	memset(xxxx, 0, 200);
	f_open(&bak, fileName, FA_OPEN_ALWAYS | FA_WRITE | FA_READ | FA_OPEN_APPEND);

	if(bak.fptr > 24*i-1)
		f_lseek(&bak, bak.fptr - 24*i);
	f_gets(xxxx, 200, &bak);
	f_close(&bak);
	p->operationFunc = (xxxx[13]-0x30)*10+xxxx[14]-0x30;
	if(xxxx[0] == 0)
		return -1;
	for(int i = 16; xxxx[i] != '\n'; i++){
		p->operationValue = xxxx[i]-0x30 + (p->operationValue*10);
	}
	memcpy(p->time, xxxx, 12);
	return 0;
}










