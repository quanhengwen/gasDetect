/*
 * wordLib.h
 *
 *  Created on: 2019年8月23日
 *      Author: Administrator
 */

#ifndef WORDLIB_H_
#define WORDLIB_H_

#include "main.h"
#include "cmsis_os.h"

void wordLIbInit(void);
uint8_t * getWordData(uint8_t* p);
void WORDInit(void);
void send_command_to_ZK(uint8_t dat);
uint8_t ZK_read();
void ZK_Read_1_n(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pBuff,uint8_t DataLen );
char * getGB2312Word(uint32_t  x,uint32_t  y, uint8_t * text, int size);
extern uint16_t word[];
#endif /* WORDLIB_H_ */
