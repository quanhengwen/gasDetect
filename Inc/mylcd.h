/*
 * mylcd.h
 *
 *  Created on: 2019年8月21日
 *      Author: Administrator
 */

#ifndef MYLCD_H_
#define MYLCD_H_
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "ds1302.h"
#define RESETPORT GPIOD
#define RESETPIN GPIO_PIN_13
#define RESETVALUESET(x) HAL_GPIO_WritePin(RESETPORT, RESETPIN, x)
#define LCDRESETDELAY()	osDelay(20)

enum{
	BI=0,
	SETCONFIG,
	BACK,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	SURE,
};

enum{
	firstPage=0,
	passWordInput,
	setPage1,
	setPage2,
	otherSet,
	detectSet,
	passWordChange,
	warning,
	demarcate,
	operation,
	faultRefer,
};

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色

struct arrow{
	const unsigned char hLoc;
	const unsigned char wLoc;
	const unsigned char h;
	const unsigned char w;
	const unsigned char * p;
};
struct ImageInfo{
	char locWidth;
	char locHight;
	char width;
	char hight;
	const short * p;
	short size;
};
struct menu_loc_value{
	unsigned short loc;
	unsigned short value;
};
struct MenuInfo{
	int size;
	const struct menu_loc_value * p;
};

struct lcdInitArray{
	uint32_t addr;
	uint16_t data;
};
typedef struct
{
	volatile uint16_t LCD_REG;
	volatile uint16_t LCD_RAM;
} LCD_TypeDef;


DateTime otherSetTime;

#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0001FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

struct pageInfo{
	char isDis;
	char arrowInfo;
	void (*arrowInfoFunc)(void * p);
	char circleInfo;
	void (*circleInfoFunc)(void * p);
	int * dateInfo;
	void (*dateInfoFunc)(void * p);
	char unitInfo;
	void (*unitInfoFunc)(void * p);
	void * deviceNameInfo;
	void (*deviceNameInfoFunc)(void * p);
	int  detectValueInfo;
	void (*detectValueInfoFunc)(void * p);
	char baseInfo;
	void (*baseDis)(void * p);
	char * locationInfo;
	void (*locationShow)(void * p);
};

struct buttonFunc{
	void (*biFunc)(int);
	void (*setFunc)(int);
	void (*sureFunc)(int);
	void (*backFunc)(int);
	void (*topFunc)(int);
	void (*bottomFunc)(int);
	void (*leftFunc)(int);
	void (*rightFunc)(int);
};

struct deviceInfo{
	int id;
	uint8_t loc[20];
	int locInfoLenth;
	int use;
	int sensorValue;
	int erorrValue;
	int uint;
	int highPercent;
	int lowPercent;
	int currentHighLimit;
	int currentLowLimit;
	int spanValue;
	int addr;
	int limit;
	int mannerge;
	int alarm;
	int noConnect;
	int sensorFault;
	int noDemarcate;
	int isSetting;
	int backMainPage;
	int measureRange;
	int rollDisplay;
	int mainPowerFault;
	int mainPowerfix;
	int bakPowerFault;
	int bakPowerfix;
	int sensorAlarm;
};

extern int pageNum;
extern int buttonNum;

extern struct pageInfo pageInfoList[];
extern struct buttonFunc buttonList[];
extern struct deviceInfo deviceOwnInfo;
extern struct otherSetStruct otherSetValue;
extern struct deviceInfo * deviceInfoP;
extern struct deviceInfo deviceSelectList[];
extern int disArraySize;
void menu_show(int x);
void progress_show(int x);
void arrowShow(int x);
void showLocationInfo(void);
void showMessureInfo(int x);
void first_page_show_time(int year, int month, int day, int hour, int min, int sec);
void first_page_lel_unit(int i);
void first_page_head_line(void);


void lcdInit(void);
void lcdReset(void);
void lcdClean(uint16_t color);
void tftSetWin(uint8_t xStart,uint8_t yStart,uint8_t xEnd,uint8_t yEnd);
void ColorFill(const uint8_t * data, int length);
void tftRamColorFill(uint16_t  data, int length);
void tftRamWriteWord(const uint16_t * data, int length);
void tftRamWriteColorChange(const uint8_t * data, int length, int oldColor, int newColor);
void tftRamWrite(const uint8_t * data, int length);
void showNumber(void);
void funcBottom(int i);
#endif /* MYLCD_H_ */
