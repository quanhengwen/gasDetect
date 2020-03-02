/*
 * mylcd.c
 *
 *  Created on: 2019年8月21日
 *      Author: Administrator
 */
#include"mylcd.h"
#include"lcdImage.h"
#include "ds1302.h"
#include "wordLib.h"
#include "string.h"
#include "stdio.h"
#include "485.h"
#include "record.h"
#define clean(x) for(int i=0; i < sizeof(x); i++){if(x[i] == ' ') x[i] = '0' ;}

struct stack{
	int curLoc;
	char deep[4];
};
struct pageCourse{
	int otherSet;
};
struct pageCourse pageCourseInfo;
struct stack myStack;

char myPop(void){
	char x;
	if(myStack.curLoc >0)
	{
		myStack.curLoc--;
		x = myStack.deep[myStack.curLoc];
	}
	else
		x = -1;
	return x;
}
void myPush(char x){
	if(myStack.curLoc == 4)
		return;
	myStack.deep[myStack.curLoc] = x;
	myStack.curLoc++;
}

void cleanStack(void){
	myStack.curLoc = 0;
}

void arrowInfoDisFunc(void * p){
	char value = *(char*)p;
	if(value == 0)
	{
		if(deviceOwnInfo.mannerge){
			tftSetWin(198, 6, 13, 11);
			tftRamWrite(arraowList[0].p, arraowList[0].length);
			tftSetWin(198, 159, 13, 11);
			tftRamWrite(arraowList[1].p, arraowList[1].length);
		}
	}
	else if(value>0 && value<5)
	{
		for(int i = 1; i<5; i++){
			tftSetWin(178-50*(i-1), 10, 13, 11);
			tftRamColorFill(0x2945, arraowList[1].length);
		}
		tftSetWin(178-50*(value-1), 10, 13, 11);
		tftRamWrite(arraowList[1].p, arraowList[1].length);
	}
}

void circleInfoDisFunc(void * p){
	int color;
	int percentage;
	const char loc[] = {
			45,		7,		38, 	27,
			83,		4,		41,		11,
			125,	7,		37,		27,
			158,	30,		26, 	37,
			177,	68,		11,		40,
			158,	109,	26,		37,
			125,	142,	37,		27,
			83,		161,	41,		11,
			45,		142,	38, 	27,
	};



	//确认上下限
	if(deviceInfoP->uint == 2){
		percentage = deviceInfoP->sensorValue / 5;
	}
	else{
		percentage = deviceInfoP->sensorValue / 100;
	}
	//确认颜色
	if((!deviceInfoP->sensorFault) && (!deviceInfoP->noDemarcate)){
		color = 0X7D7C;
		deviceInfoP->sensorAlarm = 0;
		int num;
		if(deviceInfoP->uint == 1){
			num = 100;
		}
		else{
			num = 5;
		}
		if(deviceInfoP->sensorValue > deviceOwnInfo.lowPercent*num){
			color = 0XFC07;
			deviceInfoP->sensorAlarm = 1;
		}
		if(deviceInfoP->sensorValue > deviceOwnInfo.highPercent*num){
			color = 0xF800;
			deviceInfoP->sensorAlarm = 1;
		}
	}
	//显示个数
	int circleDisNum = percentage * 9 /100+1;

	for(int i = 0; i < 9; i++){
		tftSetWin(loc[i*4+0],loc[i*4+1],loc[i*4+2],loc[i*4+3]);
		if(circleDisNum > i)
			tftRamWriteColorChange(circleList[i].p, circleList[i].length,0xffff,color);
		else
			tftRamWrite(circleList[i].p, circleList[i].length);
	}
}

void dateInfoDisFunc(void * p){
	int * x = (int *)p;
	uint8_t temporaryFirestLine[10];
	uint8_t temporarySecLine[8];
	temporaryFirestLine[0] = x[0]/1000;
	temporaryFirestLine[1] = x[0]/100%10;
	temporaryFirestLine[2] = x[0]/10%10;
	temporaryFirestLine[3] = x[0]%10;
	temporaryFirestLine[4] = 10;
	temporaryFirestLine[5] = x[1]/10;
	temporaryFirestLine[6] = x[1]%10;
	temporaryFirestLine[7] = 10;
	temporaryFirestLine[8] = x[2]/10;
	temporaryFirestLine[9] = x[2]%10;
	temporarySecLine[0] = x[3]/10;
	temporarySecLine[1] = x[3]%10;
	temporarySecLine[2] = 11;
	temporarySecLine[3] = x[4]/10;
	temporarySecLine[4] = x[4]%10;
	temporarySecLine[5] = 11;
	temporarySecLine[6] = x[5]/10;
	temporarySecLine[7] = x[5]%10;
	tftSetWin(60, 50, 11, 8*10);
	for(int i = 0; i < 10; i++)
		tftRamWrite(smallNumList[temporaryFirestLine[i]].p, smallNumList[temporaryFirestLine[i]].length);
	tftSetWin(43, 59, 11, 8*8);
	for(int i = 0; i < 8; i++)
		tftRamWrite(smallNumList[temporarySecLine[i]].p, smallNumList[temporarySecLine[i]].length);
}
extern int otherPageNum;
uint8_t name[12];
void deviceNameInfoDisFunc(void * p){
	uint8_t addr[] = {0xc9, 0xe8, 0xb1, 0xb8, 0xA3, 0xB0, 0xA3, 0xB0};
	uint8_t noUse[] = {0xA3, 0xB0, 0xA3, 0xB0, 0xC0, 0xEB, 0xCF, 0xDF};
	uint8_t owner[] = {0xB1, 0xBE, 0xBB, 0xFA};

	static int addrbak = 1000;
	static int backMainPageBak;


	if(memcmp(name, deviceInfoP->loc, 12) ){
		memcpy(name, deviceInfoP->loc, 12);
		tftSetWin(0, 176-24*6, 24, 24*6);
		tftRamColorFill(0x2945, 24*24*6);
		for(int i = 0; i < deviceInfoP->locInfoLenth; i++){
			uint16_t * word = (uint16_t *)getGB2312Word(1,1,&deviceInfoP->loc[i*2], 24);
			tftSetWin(0, 176-deviceInfoP->locInfoLenth*24+24*i, 24, 24);
			tftRamWriteWord(word, 24*24);
		}
	}
	else{
		for(int i = 0; i < deviceInfoP->locInfoLenth; i++){
			uint16_t * word = (uint16_t *)getGB2312Word(1,1,&deviceInfoP->loc[i*2], 24);
			tftSetWin(0, 176-deviceInfoP->locInfoLenth*24+24*i, 24, 24);
			tftRamWriteWord(word, 24*24);
		}
	}


	if((addrbak == deviceInfoP->addr) && (deviceOwnInfo.backMainPage == backMainPageBak)){
		return;
	}
	else
	{
		addrbak = deviceInfoP->addr;
		backMainPageBak = deviceOwnInfo.backMainPage;
	}

	tftSetWin(220-24, (176/2)-48, 24, 24*6);
	tftRamColorFill(0x2945, 24*24*6);


	getGB2312Word(1,1,&owner[0], 24);
	if(deviceInfoP->addr == deviceOwnInfo.addr){
		for(int i = 0; i < 2; i++){
			uint16_t * word = (uint16_t *)getGB2312Word(1,1,&owner[i*2], 24);
			tftSetWin(220-24, (176/2)-24+24*i, 24, 24);
			tftRamWriteWord(word, 24*24);
		}
	}
	else if(deviceInfoP->use){
		addr[5] += deviceInfoP->addr/10;
		addr[7] += deviceInfoP->addr%10;
		for(int i = 0; i < sizeof(addr)/2; i++){
			uint16_t * word = (uint16_t *)getGB2312Word(1,1,&addr[i*2], 24);
			tftSetWin(220-24, (176/2)-24*2+24*i, 24, 24);
			tftRamWriteWord(word, 24*24);
		}
	}
	else{
		noUse[1] += deviceInfoP->addr/10;
		noUse[3] += deviceInfoP->addr%10;
		for(int i = 0; i < sizeof(noUse)/2; i++){
			uint16_t * word = (uint16_t *)getGB2312Word(1,1,&noUse[i*2], 24);
			tftSetWin(220-24, (176/2)-24*2+24*i, 24, 24);
			tftRamWriteWord(word, 24*24);
		}
	}


}

void unitInfoFuncDisFunc(void * p){
	//0 vol
	int x = deviceInfoP->uint;
	tftSetWin(144, 62, 15, 52);
	if(x == 2)
		tftRamWrite(unit[0].p, unit[0].length);
	else
		tftRamWrite(unit[1].p, unit[1].length);
}

void detectUnitSetPageDisFunc(void * p){
	int x = deviceInfoP->uint;
	tftSetWin(179, 124, 15, 52);
	if(x == 2)
		tftRamWrite(unit[0].p, unit[0].length);
	else
		tftRamWrite(unit[1].p, unit[1].length);
}

void demarcateUnitPageDisFunc(void * p){
	int x = *(char*)p;
	tftSetWin(128, 124, 15, 52);
	if(x == 2)
		tftRamWrite(unit[0].p, unit[0].length);
	else
		tftRamWrite(unit[1].p, unit[1].length);
}
uint8_t mainPowerFault[] = {0xD6, 0xF7, 0xB5, 0xE7, 0xD4, 0xB4, 0xB9, 0xCA, 0xD5, 0xCF};
uint8_t bakPowerFault[] = {0xB8, 0xB1, 0xB5, 0xE7, 0xD4, 0xB4, 0xB9, 0xCA, 0xD5, 0xCF };
void  detectValueAreaClean(void){
	tftSetWin(80, 30, 60, 22*5+10);
	tftRamColorFill(0x2945, 60*120);
	tftSetWin(80, 30, 24, 24*5+12);
	tftRamColorFill(0x2945, 132*24);
}
void detectValueInfoDisFunc(void * p){
	uint8_t x[5];
	static int bak;
	static int backMainPageBak;
	if(deviceOwnInfo.backMainPage != backMainPageBak)
	{
		backMainPageBak= deviceOwnInfo.backMainPage;
		bak = 11;
	}
	if(deviceInfoP->noConnect){
		if(bak != 1){
			bak = 1;
			detectValueAreaClean();
			tftSetWin(100, 65, 17, 44);
			tftRamWrite(gImageErrorList[1].p, gImageErrorList[1].length);
		}
		return;
	}
	if(deviceInfoP->sensorFault){
		if(bak != 2){
			bak = 2;
			detectValueAreaClean();
			tftSetWin(100, 65, 17, 44);
			tftRamWrite(gImageErrorList[0].p, gImageErrorList[0].length);
		}
		return;
	}
	if(deviceInfoP->noDemarcate){
		if(bak != 3){
			bak = 3;
			detectValueAreaClean();
			tftSetWin(100, 65, 17, 44);
			tftRamWrite(gImageErrorList[2].p, gImageErrorList[2].length);
		}
		return;
	}
	if(deviceInfoP->sensorAlarm == 0){
		if(deviceInfoP->mainPowerFault){
			if(bak != 4){
				bak = 4;
				detectValueAreaClean();
				tftSetWin(87, 30, 24, 24*sizeof(mainPowerFault)/2);
				for(int i = 0; i < sizeof(mainPowerFault)/2; i++){
					uint16_t * word = (uint16_t *)getGB2312Word(1,1,&mainPowerFault[i*2], 24);
					tftRamWriteWord(word, 24*24);
				}
			}
			return;
		}
		if(deviceInfoP->bakPowerFault){
			if(bak != 5){
				bak = 5;
				detectValueAreaClean();
				tftSetWin(87, 30, 24, 24*sizeof(bakPowerFault)/2);
				for(int i = 0; i < sizeof(bakPowerFault)/2; i++){
					uint16_t * word = (uint16_t *)getGB2312Word(1,1,&bakPowerFault[i*2], 24);
					tftRamWriteWord(word, 24*24);
				}
			}
			return;
		}
	}

	if(bak != 0){
		bak = 0;
		detectValueAreaClean();
	}
	x[0] =  deviceInfoP->sensorValue/1000;
	x[1] =  deviceInfoP->sensorValue/100%10;
	x[3] =  deviceInfoP->sensorValue/10%10;
	x[4] =  deviceInfoP->sensorValue%10;
	if(!deviceOwnInfo.limit){//显示两位
		tftSetWin(87, 32, 44, 22*5);
		for(int i = 0; i < 5; i++){
			if(i == 2)
				tftRamWrite(bigNumList[10].p, bigNumList[10].length);
			else
				tftRamWrite(bigNumList[x[i]].p, bigNumList[x[i]].length);
		}
	}
	else{
		tftSetWin(87, 43, 44, 22*4);
		for(int i = 0; i < 4; i++){
			if(i == 2)
				tftRamWrite(bigNumList[10].p, bigNumList[10].length);
			else
				tftRamWrite(bigNumList[x[i]].p, bigNumList[x[i]].length);
		}
	}

}

void configPage1InfoDisFunc(void * p){
	tftSetWin(0, 0, 220, 176);
	tftRamWrite(setPageList[0].p, setPageList[0].length);
}

void configPage2InfoDisFunc(void * p){
	tftSetWin(0, 0, 220, 176);
	tftRamWrite(setPageList[1].p, setPageList[1].length);
}
extern struct otherSetStruct otherSetValue;
void otherSetPageInfoDisFunc(void * p){
	tftSetWin(0, 0, 220, 176);
	tftRamWrite(otherSetPage, 77440);
	int cursorDisSwitch = *(char*)p;
	//本机地址
	tftSetWin(179, 155, 11, 8*2);
	int x = deviceOwnInfo.addr/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.addr%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);


	//精度显示
	tftSetWin(127, 162, 11, 8*2);
	if(deviceOwnInfo.limit){
		tftRamWrite(smallNumList[1].p, smallNumList[1].length);
	}
	else{
		tftRamWrite(smallNumList[2].p, smallNumList[2].length);
	}

	//日期显示
	//???
	x = otherSetTime.Year/10;
	tftSetWin(25, 63, 11, 8*2);
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = otherSetTime.Year%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(25, 87, 11, 8*2);
	x = otherSetTime.Month/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = otherSetTime.Month%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(25, 110, 11, 8*2);
	x = otherSetTime.Day/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = otherSetTime.Day%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(25, 132, 11, 8*2);
	x = otherSetTime.Hour/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = otherSetTime.Hour%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(25, 155, 11, 8*2);
	x = otherSetTime.Min/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = otherSetTime.Min%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	//const struct imageInfo managerList[]
	tftSetWin(76, 142, 14, 27);
	if(deviceOwnInfo.mannerge){
		tftRamWrite(managerList[0].p, managerList[0].length);
	}
	else{
		tftRamWrite(managerList[1].p, managerList[1].length);
	}


	if(cursorDisSwitch){
		switch(pageInfoList[4].arrowInfo){
			case 1: tftSetWin(178, 155, 1, 8*2);tftRamColorFill(0xffff, 16); break;
			case 2: tftSetWin(126, 162, 1, 8);tftRamColorFill(0xffff, 16); break;
			case 3: tftSetWin(76, 142, 1, 28);tftRamColorFill(0xffff, 28); break;
			case 4:
					switch(pageCourseInfo.otherSet){
						case 0:tftSetWin(24, 63, 1, 8*2);tftRamColorFill(0xffff, 16);break;
						case 1:tftSetWin(24, 87, 1, 8*2);tftRamColorFill(0xffff, 16);break;
						case 2:tftSetWin(24, 110, 1, 8*2);tftRamColorFill(0xffff, 16);break;
						case 3:tftSetWin(24, 132, 1, 8*2);tftRamColorFill(0xffff, 16);break;
						case 4:tftSetWin(24, 155, 1, 8*2);tftRamColorFill(0xffff, 16);break;
					}
		}
	}
}

void detectSetPageInfoDisFunc(void * p){
	int cursorDisSwitch = *(char*)p;
	tftSetWin(178, 22, 15, 61);
	tftRamWrite(detectPageList[0].p,detectPageList[0].length);
	tftSetWin(127, 22, 14, 63);
	tftRamWrite(detectPageList[1].p,detectPageList[1].length);
	tftSetWin(76, 23, 14, 62);
	tftRamWrite(detectPageList[2].p,detectPageList[2].length);
	tftSetWin(25, 23, 14, 62);
	tftRamWrite(detectPageList[3].p,detectPageList[3].length);

	if(deviceOwnInfo.lowPercent > 99)
		deviceOwnInfo.lowPercent = 99;
	else if(deviceOwnInfo.lowPercent < 0)
		deviceOwnInfo.lowPercent = 0;

	if(deviceOwnInfo.highPercent > 99)
		deviceOwnInfo.highPercent = 99;
	else if(deviceOwnInfo.highPercent < 0)
		deviceOwnInfo.highPercent = 0;

	int x;
	tftSetWin(127, 156, 11, 8*2);
	x = deviceOwnInfo.lowPercent/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.lowPercent%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(76, 156, 11, 8*2);
	x = deviceOwnInfo.highPercent/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.highPercent%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(25, 172-8*4, 11, 8*4);
	x = deviceOwnInfo.measureRange/1000%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.measureRange/100%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.measureRange/10%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.measureRange%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);

	tftSetWin(178, 124, 1, 52);
	tftRamColorFill(0x2945, 52);
	tftSetWin(126, 156, 1, 16);
	tftRamColorFill(0x2945, 16);
	tftSetWin(75, 156, 1, 16);
	tftRamColorFill(0x2945, 16);
	if(cursorDisSwitch){
		switch(pageInfoList[5].arrowInfo){
			case 1: tftSetWin(178, 124, 1, 52);tftRamColorFill(0xffff, 52); break;
			case 2: tftSetWin(126, 156, 1, 16);tftRamColorFill(0xffff, 16); break;
			case 3: tftSetWin(76, 156, 1, 16);tftRamColorFill(0xffff, 16); break;
		}
	}

}

void firstPagebaseInfoDisFunc(void * p){
	//显示定位图标
	tftSetWin(0, 0, 21, 16);
	tftRamWrite(gImage_locationMesg, sizeof(gImage_locationMesg));
}


void firstPageLocationDisFunc(void * p){
	uint8_t wordDis[] = {0xc9, 0xe8, 0xb1, 0xb8, 0xA3, 0xB1, 0};
	for(int i = 0; i < (sizeof(wordDis)/2); i++){
		uint16_t * word;
		word = (uint16_t *)getGB2312Word(1,1,&wordDis[i*2],24);
		tftSetWin(220-24, (176/2)-24*2+10+24*i, 24, 24);
		tftRamWriteWord(word, 24*24);
	}
}

void inputPassWordInfoDisFunc(void * p){
	tftSetWin(0, 0, 220, 176);
	tftRamWrite(passWordInputPage, 77440);
	for(int i = 0; i < myStack.curLoc; i++)
	{
		tftSetWin(143, 29+36*i, 11, 12);
		tftRamWrite(passwordWord, sizeof(passwordWord));
	}
}


void demarcateBaseDisFunc(void * p){
	int k = *(char *)p;
	tftSetWin(178, 23, 14, 57);//零点标定
	tftRamWrite(demarcatePage[0].p, demarcatePage[0].length);
	tftSetWin(127, 23, 14, 77);//跨度值标定
	tftRamWrite(demarcatePage[1].p, demarcatePage[1].length);
	tftSetWin(75, 23, 14, 86);//执行跨度值标定
	tftRamWrite(demarcatePage[2].p, demarcatePage[2].length);
	//detectSetValue.spanValue
	tftSetWin(128, 105, 11, 8*2);
	int x = deviceOwnInfo.spanValue/10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	x = deviceOwnInfo.spanValue%10;
	tftRamWrite(smallNumList[x].p, smallNumList[x].length);
	if(k == 1){
		tftSetWin(127, 105, 1, 8*2);
		tftRamColorFill(0xffff, 16);
	}
	else{
		tftSetWin(127, 105, 1, 8*2);
		tftRamColorFill(0x2945, 16);
	}
}

struct recordInfo FileInfo;
uint8_t gb2312PassWordChange[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xC3, 0xDC, 0xC2, 0xEB};
uint8_t gb2312HighLimitChange[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xB8, 0xDF, 0xB1, 0xA8, 0xBE, 0xAF };
uint8_t gb2312LowLimitChange[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xB5, 0xCD, 0xB1, 0xA8, 0xBE, 0xAF};
uint8_t gb2312Slience[] = {0xBE, 0xB2, 0xD2, 0xF4};
uint8_t gb2312Reset[] = {0xB8, 0xB4, 0xCE, 0xBB};
uint8_t gb2312Span[] = {0xBF, 0xE7, 0xB6, 0xC8, 0xB1, 0xEA, 0xB6, 0xA8};
uint8_t gb2312Zero[] = {0xC1, 0xE3, 0xB5, 0xE3, 0xB1, 0xEA, 0xB6, 0xA8};
uint8_t gb2312ChangeAddr[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xB5, 0xD8, 0xD6, 0xB7};
uint8_t gb2312ChangeUnit[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xB5, 0xA5, 0xCE, 0xBB};
uint8_t gb2312ChangeDis[] = {0xD0, 0xDE, 0xB8, 0xC4, 0xCF, 0xD4, 0xCA, 0xBE, 0xBE, 0xAB, 0xB6, 0xC8};
uint8_t gb2312ChangeManerger1[] ={0xB3, 0xC9, 0xCE, 0xAA, 0xD6, 0xF7, 0xBF, 0xD8, 0xD6, 0xC6, 0xC6, 0xF7};
uint8_t gb2312ChangeManerger0[] ={0xC8, 0xA1, 0xCF, 0xFB, 0xD6, 0xF7, 0xBF, 0xD8, 0xD6, 0xC6, 0xC6, 0xF7};
uint8_t gb2312SelfTest[] = {0xD7, 0xD4, 0xBC, 0xEC};

void gb2313WordShow(uint8_t * wordAddr, int length, int value1, int value2){

	for(int i = 0; i < length/2; i++){
		uint16_t * word = (uint16_t *)getGB2312Word(1,1,&wordAddr[i*2],16);
		tftSetWin(value1, value2+16*i, 16, 16);
		tftRamWriteWord(word, 16*16);
	}

}
void gb2313NumShow(int x, int value1, int value2){

	uint8_t num[8] = {0xA3,0xB0,0xA3,0xB0,0xA3,0xB0,0xA3,0xB0};
	num[1] += x/1000%10;
	num[3] += x/100%10;
	num[5] += x/10%10;
	num[7] += x%10;

	for(int i = 0; i < 4; i++){
		uint16_t * word = (uint16_t *)getGB2312Word(1,1,&num[i*2],16);
		tftSetWin(value1, value2+16*i, 16, 16);
		tftRamWriteWord(word, 16*16);
	}
}

uint8_t gb2312HighWarn[] ={0xB8, 0xDF, 0xB1, 0xA8, 0xBE, 0xAF};
uint8_t gb2312LowWarn[] ={0xB5, 0xCD, 0xB1, 0xA8, 0xBE, 0xAF};
uint8_t gb2312MainPowerlow[] ={0xD6, 0xF7, 0xB5, 0xE7, 0xD4, 0xB4, 0xB5, 0xCD};
uint8_t gb2312BakPowerlow[] ={0xB1, 0xB8, 0xD3, 0xC3, 0xB5, 0xE7, 0xD4, 0xB4, 0xB5, 0xCD};

int warnReferPageNum;
int faultReferPageNum;
int oprateReferPageNum;
void warnReferBaseDisFunc(void * p){
	int u = *(int *)p;

	if(warnReferPageNum != u){
		if(fileGetLine("Warning.txt", u, &FileInfo)==-1){
			fileGetLine("Warning.txt", warnReferPageNum, &FileInfo);
			(*(int *)p) = warnReferPageNum;
		}
		else
			warnReferPageNum = u;
		lcdClean(0x2945);
		uint8_t headLine[] = {0xBE, 0xAF, 0xB8, 0xE6, 0xB2, 0xE9, 0xD1, 0xAF};
		gb2313WordShow(headLine, sizeof(headLine), 220-20, 60);
	}

	tftSetWin(198, 6, 1, 164);
	tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));

	tftSetWin(170-24-2, 6, 1, 164);
	tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));



	if(FileInfo.time[0] != 0){
		tftSetWin(170, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 10; i++){
			if(i == 4||i == 7)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j]-0x30].p, smallNumList[0].length);
				j++;
			}
		}
		tftSetWin(170-24, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 5; i++){
			if(i == 2)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j+8]-0x30].p, smallNumList[0].length);
				j++;
			}
		}
		switch(FileInfo.operationFunc){
		case highWarnRecordValue:
			gb2313WordShow(gb2312HighWarn,sizeof(gb2312HighWarn), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48, 10);
			break;
		case lowWarnRecordValue:
			gb2313WordShow(gb2312LowWarn,sizeof(gb2312LowWarn), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48, 10);
			break;
		case MainPowerlowRecordValue:
			gb2313WordShow(gb2312MainPowerlow,sizeof(gb2312MainPowerlow), 110-24, 10);
			break;
		case BakPowerlowRecordValue:
			gb2313WordShow(gb2312BakPowerlow,sizeof(gb2312BakPowerlow), 110-24, 10);
			break;
		}
	}
}

uint8_t gb2312SensorFault[] = {0xB4, 0xAB, 0xB8, 0xD0, 0xC6, 0xF7, 0xB9, 0xCA, 0xD5, 0xCF};
uint8_t gb2312Disconnect[] = {0xC1, 0xAC, 0xBD, 0xD3, 0xD6, 0xD0, 0xB6, 0xCF};
uint8_t gb2312NoDemarcate[] = {0xB4, 0xAB, 0xB8, 0xD0, 0xC6, 0xF7, 0xCE, 0xB4, 0xB1, 0xEA, 0xB6, 0xA8};
uint8_t gb2312MainPowerFault[] = {0xD6, 0xF7, 0xB5, 0xE7, 0xD4, 0xB4, 0xB9, 0xCA, 0xD5, 0xCF};
uint8_t gb2312BakPowerFault[] = {0xB8, 0xB1, 0xB5, 0xE7, 0xD4, 0xB4, 0xB9, 0xCA, 0xD5, 0xCF};
void faultBaseDisFunc(void * p){
	int u = *(int *)p;

	if(faultReferPageNum != u){
		if(fileGetLine("Fault.txt", u, &FileInfo)==-1){
			fileGetLine("Fault.txt", faultReferPageNum, &FileInfo);
			*(int *)p = faultReferPageNum;
		}
		else
			faultReferPageNum = u;
		lcdClean(0x2945);
		uint8_t headLine[] = {0xB9, 0xCA, 0xD5, 0xCF, 0xB2, 0xE9, 0xD1, 0xAF};
		gb2313WordShow(headLine, sizeof(headLine), 220-20, 60);
	}
	if(FileInfo.time[0] != 0){
		tftSetWin(170, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 10; i++){
			if(i == 4||i == 7)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j]-0x30].p, smallNumList[0].length);
				j++;
			}
		}
		tftSetWin(170-24, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 5; i++){
			if(i == 2)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j+8]-0x30].p, smallNumList[0].length);
				j++;
			}
		}
		switch(FileInfo.operationFunc){
			case sensorFaultValue:			gb2313WordShow(gb2312SensorFault,sizeof(gb2312SensorFault), 110-24, 10);	break;
			case disconnectRecordValue:		gb2313WordShow(gb2312Disconnect,sizeof(gb2312Disconnect), 110-24, 10);	break;
			case noDemarcateRecordValue:	gb2313WordShow(gb2312NoDemarcate,sizeof(gb2312NoDemarcate), 110-24, 10);	break;
		}
	}

	tftSetWin(198, 6, 1, 164);
	tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));

	tftSetWin(170-24-2, 6, 1, 164);
	tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));
}

void oprateBaseDisFunc(void * p){
	int u = *(int *)p;
	if(oprateReferPageNum != u){
		if(fileGetLine("Opera.txt", u, &FileInfo)==-1){
			*(int *)p = oprateReferPageNum;
			fileGetLine("Opera.txt", oprateReferPageNum, &FileInfo);
		}
		else
			oprateReferPageNum = u;
		lcdClean(0x2945);
		uint8_t headLine[] = {0xB2, 0xD9, 0xD7, 0xF7, 0xB2, 0xE9, 0xD1, 0xAF};
		gb2313WordShow(headLine, sizeof(headLine), 220-20, 60);
	}
	if(FileInfo.time[0] != 0){
		tftSetWin(170, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 10; i++){
			if(i == 4||i == 7)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j]-0x30].p, smallNumList[0].length);
				j++;
			}
		}
		tftSetWin(170-24, 10, 11, 10*10);
		for(int i = 0,j = 0; i < 5; i++){
			if(i == 2)
				tftRamWrite(smallNumList[10].p, smallNumList[0].length);
			else{
				tftRamWrite(smallNumList[FileInfo.time[j+8]-0x30].p, smallNumList[0].length);
				j++;
			}
		}

		tftSetWin(198, 6, 1, 164);
		tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));

		tftSetWin(170-24-2, 6, 1, 164);
		tftRamWrite(segmentationSymbols, sizeof(segmentationSymbols));

		switch(FileInfo.operationFunc){
		case changePassWordRecordValue:
			gb2313WordShow(gb2312PassWordChange,sizeof(gb2312PassWordChange), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48,10);
			break;
		case setHighLimitRecordValue:
			gb2313WordShow(gb2312HighLimitChange,sizeof(gb2312HighLimitChange), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 130-48,10);
			break;
		case setLowLimitRecordValue:
			gb2313WordShow(gb2312LowLimitChange,sizeof(gb2312LowLimitChange), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48,10);
			break;
		case setBuzzerSilenceRecordValue:
			gb2313WordShow(gb2312Slience,sizeof(gb2312Slience), 110-24, 10);
			break;
		case setResetRecordValue:
			gb2313WordShow(gb2312Reset,sizeof(gb2312Reset), 110-24, 10);
			break;
		case spanDemarcateRecordValue:
			gb2313WordShow(gb2312Span,sizeof(gb2312Span), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48,10);
			break;
		case zeroDemarcateRecordValue:
			gb2313WordShow(gb2312Zero,sizeof(gb2312Zero), 110-24, 10);
			break;
		case changeAddrRecordValue:
			gb2313WordShow(gb2312ChangeAddr,sizeof(gb2312ChangeAddr), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48,10);
			break;
		case changeUnitRecordValue:
			gb2313WordShow(gb2312ChangeUnit,sizeof(gb2312ChangeUnit), 110-24, 10);
			break;
		case changeDisRecordValue:
			gb2313WordShow(gb2312ChangeDis,sizeof(gb2312ChangeDis), 110-24, 10);
			gb2313NumShow(FileInfo.operationValue, 110-48,10);
			break;
		case changeMangerRecordValue:
			if(FileInfo.operationValue)
				gb2313WordShow(gb2312ChangeManerger1,sizeof(gb2312ChangeManerger1), 110-24, 10);
			else
				gb2313WordShow(gb2312ChangeManerger0,sizeof(gb2312ChangeManerger0), 110-24, 10);
			break;
		case selfTest:
			gb2313WordShow(gb2312SelfTest,sizeof(gb2312SelfTest), 110-24, 10);
			break;
		}
	}
}

void nullFunc(void * p)
{
	return;
}
int timeData[] = {2019, 07, 18, 18, 30, 20};
//char wordDis[] = {0xc9, 0xe8, 0xb1, 0xb8, 0};

struct deviceInfo deviceOwnInfo ={
	 .id = 0x22,
	 .locInfoLenth = 0,
	 .highPercent = 50,
	 .lowPercent = 20,
	 .uint = 1,
	 .use = 1,
	 .addr = 64,
};


struct deviceInfo * deviceInfoP = &deviceOwnInfo;
struct deviceInfo otherDeviceList[65];
struct pageInfo pageInfoList[] ={
	//0
	{
		.isDis = 1,
		.arrowInfo = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.circleInfoFunc = circleInfoDisFunc,
		.circleInfo = 5,
		.dateInfoFunc = dateInfoDisFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = deviceNameInfoDisFunc,
		.deviceNameInfo = &deviceOwnInfo,
		.unitInfo = 1,
		.unitInfoFunc = unitInfoFuncDisFunc,
		.detectValueInfo = 0,
		.detectValueInfoFunc = detectValueInfoDisFunc,
		.baseInfo = 1,
		.baseDis = firstPagebaseInfoDisFunc,
	},
	//1密码输入界面
	{
		.isDis = 0,
		.arrowInfoFunc = nullFunc,
		.circleInfoFunc = nullFunc,
		.dateInfoFunc = nullFunc,
		.deviceNameInfoFunc = nullFunc,
		.unitInfoFunc = nullFunc,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = inputPassWordInfoDisFunc,
	},
	//2设置界面1
	{
		.isDis = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.unitInfoFunc = nullFunc,
		.detectValueInfo = 1234,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = configPage1InfoDisFunc,
	},
	//3设置界面2
	{
		.isDis = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.unitInfoFunc = nullFunc,
		.detectValueInfo = 1234,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = configPage2InfoDisFunc,
	},
	//4子界面-其他设置
	{
		.isDis = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.unitInfoFunc = nullFunc,
		.detectValueInfo = 1234,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = otherSetPageInfoDisFunc,
	},
	//5子界面-探测器设置
	{
		.isDis = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.unitInfoFunc = detectUnitSetPageDisFunc,
		.detectValueInfo = 1234,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = detectSetPageInfoDisFunc,
	}
	,
	//6子界面-密码设置
	{
		.isDis = 0,
		.arrowInfoFunc = nullFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.unitInfoFunc = nullFunc,
		.detectValueInfo = 1234,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = inputPassWordInfoDisFunc,
	}
	,
	//7子界面-警告查询
	{
		.isDis = 0,
		.arrowInfoFunc = nullFunc,
		.circleInfoFunc = nullFunc,
		.dateInfoFunc = nullFunc,
		.deviceNameInfoFunc = nullFunc,
		.unitInfoFunc = nullFunc,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 1,
		.baseDis = warnReferBaseDisFunc,
	},
	//8标定界面
	{
		.isDis = 0,
		.arrowInfoFunc = arrowInfoDisFunc,
		.arrowInfo = 1,
		.circleInfoFunc = nullFunc,
		.circleInfo = 0,
		.dateInfoFunc = nullFunc,
		.dateInfo = timeData,
		.deviceNameInfoFunc = nullFunc,
		.deviceNameInfo = NULL,
		.unitInfo = 1,
		.detectValueInfo = 1234,
		.unitInfoFunc = demarcateUnitPageDisFunc,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 0,
		.baseDis = demarcateBaseDisFunc,
	},
	//9子界面-操作查询 "Opera.txt" "Fault.txt" "Warning.txt"
	{
		.isDis = 0,
		.arrowInfoFunc = nullFunc,
		.circleInfoFunc = nullFunc,
		.dateInfoFunc = nullFunc,
		.deviceNameInfoFunc = nullFunc,
		.unitInfoFunc = nullFunc,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 1,
		.baseDis = oprateBaseDisFunc,
	},
	//10子界面-故障查询
	{
		.isDis = 0,
		.arrowInfoFunc = nullFunc,
		.circleInfoFunc = nullFunc,
		.dateInfoFunc = nullFunc,
		.deviceNameInfoFunc = nullFunc,
		.unitInfoFunc = nullFunc,
		.detectValueInfoFunc = nullFunc,
		.baseInfo = 1,
		.baseDis = faultBaseDisFunc,
	}
};
int disArraySize = sizeof(pageInfoList);


void assignPage(int x)
{
	for(int i =0; i < pageNum; i++)
		pageInfoList[i].isDis = 0;
	pageInfoList[x].isDis = 1;
	if(x == 0)
	{
		pageInfoList[firstPage].arrowInfo = 0;
		pageInfoList[setPage1].arrowInfo = 1;
		pageInfoList[setPage2].arrowInfo = 1;
		pageInfoList[otherSet].arrowInfo = 1;
		deviceOwnInfo.backMainPage = !deviceOwnInfo.backMainPage;
	}
}

void configPage1BackClickFunc(int i)
{
	if(i == 0)
		return;
	assignPage(firstPage);
	osDelay(200);
}

void configPage2BackClickFunc(int i)
{
	if(i == 0)
		return;
	assignPage(firstPage);
	osDelay(200);
}
extern int alamBan;
void biFirstClickFunc(int i)
{
	if(i == 0)
		return;
	operationRecord(setBuzzerSilenceRecordValue, 0);
	alamBan = 0;
}


void configPage1EnterClickFunc(int i)
{
	if(i == 0)
		return;
	if(pageInfoList[setPage1].arrowInfo == 1)
		checkSelf();
	if(pageInfoList[setPage1].arrowInfo == 2)
		assignPage(detectSet);
	if(pageInfoList[setPage1].arrowInfo == 3)
		assignPage(demarcate);
	if(pageInfoList[setPage1].arrowInfo == 4)
		assignPage(otherSet);
	osDelay(100);
}

void configPage2SureClickFunc(int i)
{
	if(i == 0)
		return;
	if(pageInfoList[setPage2].arrowInfo == 1)
		assignPage(faultRefer);
	if(pageInfoList[setPage2].arrowInfo == 2)
		assignPage(warning);
	if(pageInfoList[setPage2].arrowInfo == 3)
		assignPage(operation);
	if(pageInfoList[setPage2].arrowInfo == 4)
		assignPage(passWordChange);
	osDelay(200);
}




void setFirstClickFunc(int i)
{
	if(i == 0)
		return;
	assignPage(passWordInput);
	cleanStack();
	osDelay(200);
}
void configPage1TopClickFunc(int i)
{
	if(i == 0)
		return;
	pageInfoList[setPage1].arrowInfo--;
	if(pageInfoList[setPage1].arrowInfo < 1){
		pageInfoList[setPage1].arrowInfo =1;
	}
	osDelay(200);
}
void configPage1BottomClickFunc(int i)
{
	if(i == 0)
		return;
	pageInfoList[setPage1].arrowInfo ++;
	if(pageInfoList[setPage1].arrowInfo>4){
		pageInfoList[setPage1].arrowInfo = 4;
		assignPage(setPage2);
	}
	osDelay(200);
}
void configPage2TopClickFunc(int i)
{
	if(i == 0)
		return;
	pageInfoList[setPage2].arrowInfo--;
	if(pageInfoList[setPage2].arrowInfo == 0){
		pageInfoList[setPage2].arrowInfo = 1;
		assignPage(setPage1);
	}
	osDelay(200);
}
void configPage2BottomClickFunc(int i)
{
	if(i == 0)
		return;
	pageInfoList[setPage2].arrowInfo++;
	if(pageInfoList[setPage2].arrowInfo>4){
		pageInfoList[setPage2].arrowInfo = 4;
	}
	osDelay(200);
}



void passwordInputPageBackClickFunc(int i){
	if(i == 0)
		return;
	myStack.curLoc = 0;
	if(pageInfoList[passWordInput].isDis)
		assignPage(firstPage);
	if(pageInfoList[passWordChange].isDis)
		assignPage(setPage2);
	osDelay(200);
}



void referBackClickFunc(int i){
	if(i == 0)
		return;
	pageInfoList[warning].baseInfo = 1;
	pageInfoList[operation].baseInfo = 1;
	pageInfoList[faultRefer].baseInfo = 1;
	warnReferPageNum = 0;
	faultReferPageNum = 0;
	oprateReferPageNum = 0;
	assignPage(setPage2);
	osDelay(200);
}

void referPageTopClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[warning].isDis){
		if(pageInfoList[warning].baseInfo > 1)
			pageInfoList[warning].baseInfo--;
	}
	if(pageInfoList[operation].isDis){
		if(pageInfoList[operation].baseInfo > 1)
			pageInfoList[operation].baseInfo--;
	}
	if(pageInfoList[faultRefer].isDis){
		if(pageInfoList[faultRefer].baseInfo > 1)
			pageInfoList[faultRefer].baseInfo--;
	}
	osDelay(200);
}
void referPageBottomClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[warning].isDis){
		if(pageInfoList[warning].baseInfo < 1500)
			pageInfoList[warning].baseInfo++;
	}
	if(pageInfoList[operation].isDis){
		if(pageInfoList[operation].baseInfo < 1500)
			pageInfoList[operation].baseInfo++;
	}
	if(pageInfoList[faultRefer].isDis){
		if(pageInfoList[faultRefer].baseInfo < 1500)
			pageInfoList[faultRefer].baseInfo++;
	}
	osDelay(200);
}

void demarcatePageTopClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[demarcate].baseInfo == 0){
		pageInfoList[demarcate].arrowInfo--;
		if(pageInfoList[demarcate].arrowInfo < 1)
			pageInfoList[demarcate].arrowInfo = 1;
	}
	else{
		deviceOwnInfo.spanValue++;
		if(deviceOwnInfo.spanValue>99)
			deviceOwnInfo.spanValue = 99;
	}
	osDelay(200);

}
void demarcatePageLeftClickFunc(int i){
	if(i == 0)
		return;
}
void demarcatePageRightClickFunc(int i){
	if(i == 0)
		return;
}
void demarcatePageBottomClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[demarcate].baseInfo == 0){
			pageInfoList[demarcate].arrowInfo++;
			if(pageInfoList[demarcate].arrowInfo > 3)
				pageInfoList[demarcate].arrowInfo = 3;
		}
	else{
		deviceOwnInfo.spanValue--;
		if(deviceOwnInfo.spanValue < 0)
			deviceOwnInfo.spanValue = 0;
	}
	osDelay(200);
}

void demarcateBackClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[demarcate].baseInfo == 1){
		pageInfoList[demarcate].baseInfo = 0;
	}
	else
		assignPage(setPage1);
	osDelay(200);
}

void demarcateSureClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[demarcate].arrowInfo == 1){
		uartSendInfo.zeroDemarcateFlag = 0;
		operationRecord(zeroDemarcateRecordValue,0);
	}
	if(pageInfoList[demarcate].arrowInfo == 2){
		if(pageInfoList[demarcate].baseInfo == 1){
			pageInfoList[demarcate].baseInfo = 0;
		}
		else{
			pageInfoList[demarcate].baseInfo = 1;
		}
	}
	if(pageInfoList[demarcate].arrowInfo == 3){
		uartSendInfo.spanDemarcateFlag = 0;
		operationRecord(spanDemarcateRecordValue,deviceOwnInfo.spanValue);
	}

	osDelay(200);
}

extern uint16_t passWord;
void passwordInputPageSureClickFunc(int i){
	if(i == 0)
		return;
	int k = myPop()+myPop()*10+myPop()*100+myPop()*1000;
	if(pageInfoList[1].isDis && k == passWord){
		assignPage(setPage1);
	}
	if(pageInfoList[6].isDis){
		char newPassWord[10];
		char oldPassWord[10];
		sprintf(oldPassWord, "P%4d.txt", passWord);
		clean(oldPassWord);
		f_unlink(oldPassWord);
		passWord = k;
		sprintf(newPassWord, "P%4d.txt", passWord);
		clean(newPassWord);
		f_open(&SDFile, newPassWord, FA_OPEN_ALWAYS | FA_WRITE | FA_READ|FA_OPEN_APPEND);
		f_close(&SDFile);
		assignPage(setPage2);
		operationRecord(changePassWordRecordValue, passWord);
	}
	osDelay(200);
}
void passwordInputPageTopClickFunc(int i){
	if(i == 0)
		return;
	myPush(1);
	osDelay(200);
}
void passwordInputPageBottomClickFunc(int i){
	if(i == 0)
		return;
	myPush(2);
	osDelay(200);
}
void passwordInputPageLeftClickFunc(int i){
	if(i == 0)
		return;
	myPush(3);
	osDelay(200);
}
void passwordInputPageRightClickFunc(int i){
	if(i == 0)
		return;
	myPush(4);
	osDelay(200);
}

void otherSetPageRightClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[otherSet].arrowInfo == 4)
		pageCourseInfo.otherSet++;
	osDelay(200);
}

void otherSetPageLeftClickFunc(int i){
	if(i == 0)
		return;
	if(pageInfoList[otherSet].arrowInfo == 4)
		pageCourseInfo.otherSet--;
	osDelay(200);
}
void otherSetPageTopClickFunc(int i){
	if(i == 0)
		return;
	DS1302_ReadTime(&datatime);
	otherSetTime = datatime;
	otherSetTime.Year = otherSetTime.Year%100;
	if(!pageInfoList[otherSet].baseInfo){
		pageInfoList[otherSet].arrowInfo--;
		if(pageInfoList[otherSet].arrowInfo<1)
			pageInfoList[otherSet].arrowInfo = 1;
	}
	else{
			switch(pageInfoList[4].arrowInfo){
			case 1: if(deviceOwnInfo.addr<65)deviceOwnInfo.addr++; break;
			case 2: if(deviceOwnInfo.limit)deviceOwnInfo.limit = 0;else deviceOwnInfo.limit = 1; break;
			case 3: if(deviceOwnInfo.mannerge){
						otherPageNum = -1;
						deviceOwnInfo.mannerge = 0;
						deviceInfoP = &deviceOwnInfo;
					}
					else
						deviceOwnInfo.mannerge = 1;
					break;
			case 4:
				if(pageCourseInfo.otherSet == 0)
					otherSetTime.Year++;
				else if(pageCourseInfo.otherSet == 1)
					otherSetTime.Month++;
				else if(pageCourseInfo.otherSet == 2)
					otherSetTime.Day++;
				else if(pageCourseInfo.otherSet == 3)
					otherSetTime.Hour++;
				else if(pageCourseInfo.otherSet == 4)
					otherSetTime.Min++;
				break;
			}
	}
	DS1302_SetTime(otherSetTime);
	osDelay(200);
}

void otherSetPageSureClickFunc(int i){
	if(i == 0)
		return;
	//指明进入只页面
	if(pageInfoList[4].baseInfo == 0){
		deviceOwnInfo.isSetting = 1;
		pageInfoList[4].baseInfo = 1;
	}
	else{
		deviceOwnInfo.isSetting = 0;
		pageInfoList[4].baseInfo = 0;
		if(pageInfoList[otherSet].arrowInfo == 1){
			operationRecord(changeAddrRecordValue, deviceOwnInfo.addr);
		}
		if(pageInfoList[otherSet].arrowInfo == 2){
			operationRecord(changeDisRecordValue, deviceOwnInfo.limit);
		}
		if(pageInfoList[otherSet].arrowInfo == 3){
			operationRecord(changeMangerRecordValue, deviceOwnInfo.mannerge);
		}
	}
	osDelay(200);
}
void otherSetPageBackClickFunc(int i){
	if(i == 0)
		return;
	//指明进入只页面
	if(pageInfoList[otherSet].baseInfo == 1){
		pageInfoList[otherSet].baseInfo = 0;
	}
	else{
		assignPage(setPage1);
	}
	deviceOwnInfo.isSetting = 0;
	osDelay(200);
}

void otherSetPageBottomClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项
	DS1302_ReadTime(&datatime);
	otherSetTime = datatime;

	otherSetTime.Year = otherSetTime.Year%100;
	if(!pageInfoList[otherSet].baseInfo){
			pageInfoList[otherSet].arrowInfo++;
			if(pageInfoList[otherSet].arrowInfo>4)
				pageInfoList[otherSet].arrowInfo = 4;
	}
	else{
		switch(pageInfoList[otherSet].arrowInfo){
		case 1: if(deviceOwnInfo.addr>0)deviceOwnInfo.addr--; break;
		case 2: if(deviceOwnInfo.limit)deviceOwnInfo.limit = 0;else deviceOwnInfo.limit = 1;break;
		case 3: if(deviceOwnInfo.mannerge){
					otherPageNum = -1;
					deviceOwnInfo.mannerge = 0;
					deviceInfoP = &deviceOwnInfo;
				}
				else
					deviceOwnInfo.mannerge = 1;
				break;
		case 4:
			if(pageCourseInfo.otherSet == 0)
				otherSetTime.Year--;
			else if(pageCourseInfo.otherSet == 1)
				otherSetTime.Month--;
			else if(pageCourseInfo.otherSet == 2)
				otherSetTime.Day--;
			else if(pageCourseInfo.otherSet == 3)
				otherSetTime.Hour--;
			else if(pageCourseInfo.otherSet == 4)
				otherSetTime.Min--;
			break;
		}
	}
	DS1302_SetTime(otherSetTime);
	osDelay(200);
}

void detectSetPageTopClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项

	if(!pageInfoList[detectSet].baseInfo){
			pageInfoList[detectSet].arrowInfo--;
			if(pageInfoList[detectSet].arrowInfo<1)
				pageInfoList[detectSet].arrowInfo = 1;
	}
	else{
		switch(pageInfoList[detectSet].arrowInfo){
			case 1:
				if(deviceOwnInfo.uint==1)
					deviceOwnInfo.uint = 2;
				else
					deviceOwnInfo.uint = 1;
				break;
			case 2:
				if(deviceOwnInfo.lowPercent<99)
					deviceOwnInfo.lowPercent++;
				break;
			case 3:
				if(deviceOwnInfo.highPercent<99)
					deviceOwnInfo.highPercent++;
				break;
			default:break;
		}

	}
	osDelay(200);
}

void detectSetPageBottomClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项

	if(!pageInfoList[detectSet].baseInfo){
			pageInfoList[detectSet].arrowInfo++;
			if(pageInfoList[detectSet].arrowInfo>4)
				pageInfoList[detectSet].arrowInfo = 4;
	}
	else{
		switch(pageInfoList[detectSet].arrowInfo){
			case 1:
				if(deviceOwnInfo.uint==1)
					deviceOwnInfo.uint = 2;
				else
					deviceOwnInfo.uint = 1;
				break;
			case 2:
				if(deviceOwnInfo.lowPercent > 0)
					deviceOwnInfo.lowPercent--;
				break;
			case 3:
				if(deviceOwnInfo.highPercent > 0)
					deviceOwnInfo.highPercent--;
				break;
			default:break;
		}
	}
	osDelay(200);
}

void detectSetPageLeftClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项
	osDelay(200);
}

void detectSetPageRightClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项
	osDelay(200);
}

void detectSetPageBackClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项
	if(pageInfoList[detectSet].baseInfo == 1){
		pageInfoList[detectSet].baseInfo = 0;
	}
	else{
		assignPage(setPage1);
		uartSendInfo.unitChangeFlag = 0;
		uartSendInfo.highLimitSetFlag =0;
		uartSendInfo.lowLimitSetFlag =0;

	}
	osDelay(200);
}

void detectSetPageSureClickFunc(int i){
	if(i == 0)
		return;
	//未进入设置项
	if(pageInfoList[detectSet].baseInfo == 0)
		pageInfoList[detectSet].baseInfo = 1;
	else{
		pageInfoList[detectSet].baseInfo = 0;
		if(pageInfoList[detectSet].arrowInfo == 1){
			uartSendInfo.unitChangeFlag = 0;
			operationRecord(changeUnitRecordValue, pageInfoList[detectSet].unitInfo);
		}
		if(pageInfoList[detectSet].arrowInfo == 2){
			uartSendInfo.lowLimitSetFlag =0;
			operationRecord(setHighLimitRecordValue, deviceOwnInfo.highPercent);
		}
		if(pageInfoList[detectSet].arrowInfo == 3){
			uartSendInfo.highLimitSetFlag =0;
			operationRecord(setLowLimitRecordValue, deviceOwnInfo.lowPercent);
		}
	}
	osDelay(200);
}

void nullButtonFunc(int i){
	return;
}

struct myFileInfo myFile;
extern UART_HandleTypeDef huart1;

void funcBottom(int i){
	if(!i)
		return;
	osDelay(200);
}
FIL fp;
int otherPageNum = -1;
extern struct deviceInfoInUart uartSendInfoOther;
enum {
	canDataDisConnect,
	canDataNoDemarcate,
	canDatafault,
	canDataAlarmLow,
	canDataAlarmHigh,
};
void funcRight(int i){
	if(!i)
		return;
	if(deviceOwnInfo.mannerge == 0)
		return;
	if(otherPageNum<63){
		otherPageNum++;
		deviceInfoP = &otherDeviceList[otherPageNum];
		deviceOwnInfo.rollDisplay = 0;
	}
	osDelay(200);
}

void funcLeft(int i){
	if(!i)
		return;
	if(deviceOwnInfo.mannerge == 0)
		return;
	if(otherPageNum>0){
		otherPageNum--;
		deviceInfoP = &otherDeviceList[otherPageNum];
		deviceOwnInfo.rollDisplay = 0;
	}
	osDelay(200);
}

void funcBack(int i){
	if(!i)
		return;
	otherPageNum = -1;
	pageInfoList[firstPage].deviceNameInfo = &deviceOwnInfo;
	deviceInfoP = &deviceOwnInfo;
	deviceOwnInfo.rollDisplay = 0;
	osDelay(200);
}

char y[50];
void funcTop(int i){
	if(!i)
		return;
	osDelay(200);
}

#define clean(x) for(int i=0; i < sizeof(x); i++){if(x[i] == ' ') x[i] = '0' ;}


	//首页按键功能
struct buttonFunc buttonList[] = {
	{
		.backFunc = funcBack,
		.biFunc = biFirstClickFunc,
		.sureFunc = nullButtonFunc,
		.setFunc = setFirstClickFunc,
		.topFunc = funcTop,
		.leftFunc = funcLeft,
		.rightFunc = funcRight,
		.bottomFunc = funcBottom,
  	},
	//密码页按键功能
	{
		.backFunc = passwordInputPageBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = passwordInputPageSureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = passwordInputPageTopClickFunc,
		.leftFunc = passwordInputPageLeftClickFunc,
		.rightFunc = passwordInputPageRightClickFunc,
		.bottomFunc = passwordInputPageBottomClickFunc,
	},
	//设置页一按键功能
	{
		.backFunc = configPage1BackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = configPage1EnterClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = configPage1TopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = configPage1BottomClickFunc,
	},
	//设置页二按键功能
	{
		.backFunc = configPage2BackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = configPage2SureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = configPage2TopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = configPage2BottomClickFunc,
	},
	//子界面-其他设置
	{
		.backFunc = otherSetPageBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = otherSetPageSureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = otherSetPageTopClickFunc,
		.leftFunc = otherSetPageLeftClickFunc,
		.rightFunc = otherSetPageRightClickFunc,
		.bottomFunc = otherSetPageBottomClickFunc,
	},
	//子界面-探测器设置
	{
		.backFunc = detectSetPageBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = detectSetPageSureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = detectSetPageTopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = detectSetPageBottomClickFunc,
	},
	//子界面-更改密码
	{
		.backFunc = passwordInputPageBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = passwordInputPageSureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = passwordInputPageTopClickFunc,
		.leftFunc = passwordInputPageLeftClickFunc,
		.rightFunc = passwordInputPageRightClickFunc,
		.bottomFunc = passwordInputPageBottomClickFunc,
	},
	//子界面-警告查询
	{
		.backFunc = referBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = nullButtonFunc,
		.setFunc = nullButtonFunc,
		.topFunc = referPageTopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = referPageBottomClickFunc,
	},
	//标定设置
	{
		.backFunc = demarcateBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = demarcateSureClickFunc,
		.setFunc = nullButtonFunc,
		.topFunc = demarcatePageTopClickFunc,
		.leftFunc = demarcatePageLeftClickFunc,
		.rightFunc = demarcatePageRightClickFunc,
		.bottomFunc = demarcatePageBottomClickFunc,
	},
	//操作查询
	{
		.backFunc = referBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = nullButtonFunc,
		.setFunc = nullButtonFunc,
		.topFunc = referPageTopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = referPageBottomClickFunc,
	},
	//故障查询
	{
		.backFunc = referBackClickFunc,
		.biFunc = nullButtonFunc,
		.sureFunc = nullButtonFunc,
		.setFunc = nullButtonFunc,
		.topFunc = referPageTopClickFunc,
		.leftFunc = nullButtonFunc,
		.rightFunc = nullButtonFunc,
		.bottomFunc = referPageBottomClickFunc,
	},
};



void lcdWrReg(volatile uint16_t regval){
	regval=regval;
	LCD->LCD_REG=regval;
}

void lcdWrData(volatile uint16_t data){
	data=data;
	LCD->LCD_RAM=data;
}

void lcdWrRegData(uint16_t LCD_Reg,uint16_t LCD_RegValue){
	lcdWrReg(LCD_Reg);
	lcdWrData(LCD_RegValue);
}

void tftRamColorFill(uint16_t  data, int length){
	lcdWrReg(0x22);
	for(int i = 0; i < length; i++){
		lcdWrData(data);
	}
}

void tftRamWrite(const uint8_t * data, int length){
	lcdWrReg(0x22);
	for(int i = 0; i < length; i+=2){
		lcdWrData(data[i+1]*256+data[i]);
	}
}
void tftRamWriteWord(const uint16_t * data, int length){
	lcdWrReg(0x22);
	for(int i = 0; i < length; i++){
		lcdWrData(data[i]);
	}
}

void tftRamWriteColorChange(const uint8_t * data, int length, int oldColor, int newColor){
	lcdWrReg(0x22);
	for(int i = 0; i < length; i+=2){
		int color = data[i+1]*256+data[i];
		if(color == 0x2945)
			lcdWrData(color);
		else
			lcdWrData(newColor);
	}
}

void lcdReset(void){
	RESETVALUESET(1);
	LCDRESETDELAY();
	RESETVALUESET(0);
	LCDRESETDELAY();
	RESETVALUESET(1);
}
//y为横
void tftSetWin(uint8_t xStart,uint8_t yStart,uint8_t xEnd,uint8_t yEnd){
	xEnd += (xStart-1);
	yEnd += yStart;
	const struct lcdInitArray lcdLocSetArrayList[]={
		{0x36,yEnd},{0x37,yStart},{0x38,xEnd},
		{0x39,xStart},{0x21, xStart},{0x20,yStart},
	};

	for(int i = 0; i < sizeof(lcdLocSetArrayList)/sizeof(lcdLocSetArrayList[0]); i++){
		lcdWrRegData(lcdLocSetArrayList[i].addr, lcdLocSetArrayList[i].data);
	}
	lcdWrReg(0x22);
}

void lcdInit(void){
	const struct lcdInitArray lcdInitArrayList[]={
		{0x00D0,0x0003},{0x00EB,0x0B00},{0x00EC,0x004F},{0x00C7,0x030F},{0x0001,0x031C},
		{0x0003,0x1038},{0x0002,0x0100},{0x0008,0x0808},{0x000F,0x0901},{0x0000,0x0000},
		{0x0010,0x0000},{0x0011,0x1B41},{0x0000,0x0000},{0x0012,0x200E},{0x0013,0x006B},
		{0x0014,0x5A66},{0x0030,0x0000},{0x0031,0x00DB},{0x0032,0x0000},{0x0033,0x0000},
		{0x0034,0x00DB},{0x0035,0x0000},{0x0050,0x0000},{0x0051,0x0109},{0x0052,0x0A02},
		{0x0053,0x0401},{0x0054,0x020A},{0x0055,0x0901},{0x0056,0x0000},{0x0057,0x0104},
		{0x0058,0x0E02},{0x0059,0x020E},{0x0007,0x1017},
	};
	lcdReset();
	for(int i = 0; i < sizeof(lcdInitArrayList)/sizeof(lcdInitArrayList[0]); i++){
		if(lcdInitArrayList[i].addr == 0x0)
			osDelay(120);
		else
			lcdWrRegData(lcdInitArrayList[i].addr, lcdInitArrayList[i].data);
	}
}

void lcdClean(uint16_t color)
{
	tftSetWin(0,0,220,177);
	for(int i = 0; i < 38720; i++){
		lcdWrData(color);
	}
}

int pageNum = sizeof(pageInfoList)/sizeof(pageInfoList[0]);
int buttonNum = sizeof(buttonList)/sizeof(buttonList[0]);



