/*
 * myButton.c
 *
 *  Created on: Aug 22, 2019
 *      Author: Administrator
 */
#include "myButton.h"

char button[9];

void buttonInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_Initure.Pin= GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5\
						|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;                //PA0
	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
	GPIO_Initure.Pull=GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);

//	GPIO_Initure.Pin= GPIO_PIN_6;
//	GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
//	GPIO_Initure.Pull=GPIO_PULLUP;
//	HAL_GPIO_Init(GPIOD,&GPIO_Initure);

	HAL_NVIC_SetPriority(EXTI3_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_SetPriority(EXTI4_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);//调用中断处理公用函数
}

void EXTI9_5_IRQHandler(void)
{
	for(int i = 0; i < 5; i++)
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5 << i);//调用中断处理公用函数
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

}

