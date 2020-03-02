/*
 * led.h
 *
 *  Created on: 2019年8月26日
 *      Author: Administrator
 */

#ifndef LED_H_
#define LED_H_
#define BuzzerPORT GPIOC
#define BuzzerPIN GPIO_PIN_7
#define POWERLEDPORT GPIOC
#define POWERLEDPIN GPIO_PIN_6
#define NORMALPORT GPIOB
#define NORMALPIN GPIO_PIN_0
#define ERRPORT GPIOB
#define ERRPIN GPIO_PIN_1
#define HALARMPORT GPIOC
#define HALARMPIN GPIO_PIN_4
#define LALARMPORT GPIOC
#define LALARMPIN GPIO_PIN_5
#define MAINPOWERDETECTPORT GPIOC
#define MAINPOWERDETECTPIN GPIO_PIN_0
#define BAKPOWERDETECTPORT GPIOC
#define BAKPOWERDETECTPIN GPIO_PIN_1
void ledGpioInit(void);
void allLedOn(void);
void allLedOff(void);
void HALARM(void);
void LALARM(void);
void normal(void);
void buzzerOn(void);
void buzzerOff(void);
void hardWareFault(void);
int mainPowerNormal(void);
int bakPowerNormal(void);
void powerFault(void);
void powerLedOn(void);
void powerLedOff(void);
#endif /* LED_H_ */
