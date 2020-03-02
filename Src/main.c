/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "myButton.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mylcd.h"
#include "ds1302.h"
#include"wordLib.h"
#include "led.h"
#include "485.h"
#include "string.h"
#include "record.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

SRAM_HandleTypeDef hsram1;

osThreadId defaultTaskHandle;
osThreadId scanButtonHandle;
osThreadId dataGet;

QueueHandle_t disSignal;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_CAN1_Init(void);
static void MX_FSMC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);
void scanButtonTask(void const * argument);
void dataGetTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  WORDInit();
  MX_DMA_Init();
  MX_SDIO_SD_Init();
  MX_CAN1_Init();
  MX_FSMC_Init();
  gpioInit();
  MX_USART1_UART_Init();

  MX_USART2_UART_Init();

  vSemaphoreCreateBinary(disSignal);
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(dataGet, dataGetTask, osPriorityNormal, 0, 256);
  dataGet = osThreadCreate(osThread(dataGet), NULL);

  osThreadDef(scanButton, scanButtonTask, osPriorityNormal, 0, 256);
  scanButtonHandle = osThreadCreate(osThread(scanButton), NULL);


  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */

#define F407VET6_BOARD_CAN_ID    	  0x001
#define SENSOR_BOARD_CAN_ID  	  	  0x002
#define ANOTHER_SENSOR_BOARD_CAN_ID       0x003
#define THIRD_SENSOR_BOARD_CAN_ID  	  0x004
#define   CAN1FIFO   CAN_RX_FIFO0
#define   CAN2FIFO   CAN_RX_FIFO1
static void MX_CAN1_Init(void)
{

	/* USER CODE BEGIN CAN1_Init 0 */
	/* USER CODE END CAN1_Init 0 */

	/* USER CODE BEGIN CAN1_Init 1 */
	/* USER CODE END CAN1_Init 1 */
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 6;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_7TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_6TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;


	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	//

	CAN_FilterTypeDef  sFilterConfig;
	sFilterConfig.FilterBank = 0;                       //过滤器0
	sFilterConfig.FilterMode =  CAN_FILTERMODE_IDMASK;  //设为列表模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;;
	sFilterConfig.FilterIdHigh = F407VET6_BOARD_CAN_ID<<5;   //基本ID放入到STID中
	sFilterConfig.FilterIdLow  = SENSOR_BOARD_CAN_ID <<5;

	sFilterConfig.FilterMaskIdHigh =0;
	sFilterConfig.FilterMaskIdLow  =0;
	sFilterConfig.FilterFIFOAssignment = CAN1FIFO;    //接收到的报文放入到FIFO0中

	sFilterConfig.FilterActivation = ENABLE;  	//激活过滤器
	sFilterConfig.SlaveStartFilterBank  = 0;
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

	/* USER CODE END CAN1_Init 2 */

}


void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX1_IRQn 0 */

  /* USER CODE END CAN1_RX1_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX1_IRQn 1 */

  /* USER CODE END CAN1_RX1_IRQn 1 */
}
/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}


uint8_t uartBuf1[200];
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */
  HAL_UART_Receive_IT(&huart1, uartBuf1, 200);
  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
uint8_t uartBuf2[200];
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 3, 3);
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}



/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE4 PE5 PE6 
                           PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6 
                          |GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 PC6 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB12 PB13 
                           PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  /*Configure GPIO pin : PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 PB6
                           PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* FSMC initialization function */
static void MX_FSMC_Init(void)
{

  /* USER CODE BEGIN FSMC_Init 0 */

  /* USER CODE END FSMC_Init 0 */

  FSMC_NORSRAM_TimingTypeDef Timing = {0};
  FSMC_NORSRAM_TimingTypeDef ExtTiming = {0};

  /* USER CODE BEGIN FSMC_Init 1 */

  /* USER CODE END FSMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  hsram1.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  hsram1.Init.ContinuousClock =FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
  /* Timing */
  Timing.AddressSetupTime = 15;
  Timing.AddressHoldTime = 15;
  Timing.DataSetupTime = 255;
  Timing.BusTurnAroundDuration = 15;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */
  ExtTiming.AddressSetupTime = 15;
  ExtTiming.AddressHoldTime = 15;
  ExtTiming.DataSetupTime = 255;
  ExtTiming.BusTurnAroundDuration = 15;
  ExtTiming.CLKDivision = 16;
  ExtTiming.DataLatency = 17;
  ExtTiming.AccessMode = FSMC_ACCESS_MODE_A;

  if (HAL_SRAM_Init(&hsram1, &Timing, &ExtTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FSMC_Init 2 */

  /* USER CODE END FSMC_Init 2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

void checkSelf(void){
	allLedOn();
	buzzerOn();
	lcdClean(RED);
	osDelay(200);
	lcdClean(GREEN);
	osDelay(200);
	lcdClean(BLUE);
	osDelay(200);
	lcdClean(WHITE);
	osDelay(200);
	lcdClean(BLACK);
	osDelay(200);
	allLedOff();
	buzzerOff();
	operationRecord(selfTest, 0);
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();
  /* USER CODE BEGIN 5 */
  lcdInit();
  ledGpioInit();
  DS1302_ReadTime(&datatime);
  operationRecord(setResetRecordValue, 0);
  checkSelf();
  int curPage;
  /* Infinite loop */
  for(;;)
  {
	xSemaphoreTake(disSignal, portMAX_DELAY);
	for(int i = 0; i < pageNum; i++){
		if(pageInfoList[i].isDis){
			if(curPage != i){
				curPage = i;
				lcdClean(0x2945);
			}
			break;
		}
	}
	pageInfoList[curPage].baseDis(&pageInfoList[curPage].baseInfo);
	pageInfoList[curPage].circleInfoFunc(&pageInfoList[curPage].circleInfo);
	pageInfoList[curPage].dateInfoFunc(pageInfoList[curPage].dateInfo);
	pageInfoList[curPage].deviceNameInfoFunc(pageInfoList[curPage].deviceNameInfo);
	pageInfoList[curPage].unitInfoFunc(&pageInfoList[curPage].unitInfo);
	pageInfoList[curPage].detectValueInfoFunc(&pageInfoList[curPage].detectValueInfo);
	pageInfoList[curPage].arrowInfoFunc(&pageInfoList[curPage].arrowInfo);
  }
  /* USER CODE END 5 */ 
}


void scanButtonTask(void const * argument)
{
	char data[8];
	int refresh;
	extern struct deviceInfo otherDeviceList[];
	for(int i = 0; i < 65; i++)
		otherDeviceList[i].addr = i;
	for(;;){
		//扫描按键
		refresh = 0;
		for(int i = 0; i < 7; i++)
			refresh |= (data[i] = (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3<<i)));
		refresh |= (data[SURE] = (!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)));
		//处理按键
		if(refresh){
			for(int i = 0; i < buttonNum; i++){
				if(pageInfoList[i].isDis){
					buttonList[i].backFunc(data[BACK]);
					buttonList[i].biFunc(data[BI]);
					buttonList[i].sureFunc(data[SURE]);
					buttonList[i].leftFunc(data[LEFT]);
					buttonList[i].bottomFunc(data[BOTTOM]);
					buttonList[i].rightFunc(data[RIGHT]);
					buttonList[i].setFunc(data[SETCONFIG]);
					buttonList[i].topFunc(data[TOP]);
					break;
				}
			}
			xSemaphoreGive(disSignal);
			osDelay(10);
		}
		osDelay(100);
	}
}


void faultThing(int count){
	static int x;
	if(deviceOwnInfo.uint == 1){
		deviceOwnInfo.currentHighLimit = deviceOwnInfo.highPercent * 100;
		deviceOwnInfo.currentLowLimit = deviceOwnInfo.lowPercent * 100;
	}
	else{
		deviceOwnInfo.currentHighLimit = deviceOwnInfo.highPercent * 5;
		deviceOwnInfo.currentLowLimit = deviceOwnInfo.lowPercent * 5;
	}
	if((deviceOwnInfo.sensorFault||(deviceOwnInfo.noDemarcate)||(deviceOwnInfo.noConnect))){
		if(x != 1){
			x = 1;
			if(deviceOwnInfo.noDemarcate)
				faultRecord(noDemarcateRecordValue, 0);
			if(deviceOwnInfo.noDemarcate)
				faultRecord(disconnectRecordValue, 0);
			if(deviceOwnInfo.sensorFault)
				faultRecord(sensorFaultValue, 0);
		}
		if(!(count%20)){
			hardWareFault();
		}
	}
	else if((deviceOwnInfo.sensorValue > deviceOwnInfo.currentHighLimit)){
		if(x != 3){
			x = 3;
			warnRecord(highWarnRecordValue, deviceOwnInfo.sensorValue);
		}
		if(!(count%5)){
			HALARM();
		}
	}
	else if((deviceOwnInfo.sensorValue > deviceOwnInfo.currentLowLimit)){
		if(x != 2){
			x = 2;
			warnRecord(lowWarnRecordValue, deviceOwnInfo.sensorValue);
		}
		if(!(count%10)){
			LALARM();
		}
	}
	else if(!deviceOwnInfo.noConnect){
		if(x != 4){
			x = 4;
		}
		normal();
		buzzerOff();
	}
	if(deviceOwnInfo.mainPowerFault||deviceOwnInfo.bakPowerFault){
		if(!(count%10)){
			powerFault();
		}
	}
	else{
		powerLedOn();
	}
}



CAN_TxHeaderTypeDef     TxMeg;
CAN_RxHeaderTypeDef     RxMeg;

uint8_t CANx_SendNormalData(CAN_HandleTypeDef* hcan,uint16_t ID,uint8_t *pData,uint16_t Len)
{
	HAL_StatusTypeDef	HAL_RetVal;
    uint16_t SendTimes,SendCNT=0;
	uint8_t  FreeTxNum=0;
	TxMeg.StdId=ID;
	if(!hcan || ! pData ||!Len)  return 1;
	SendTimes=Len/8+(Len%8?1:0);

	TxMeg.DLC=8;
	while(SendTimes--){
		if(0==SendTimes){
			if(Len%8)
				TxMeg.DLC=Len%8;
		}
		FreeTxNum=HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
		if(0==FreeTxNum){
			return 0;
		}
		osDelay(10);   //没有延时很有可能会发送失败
		HAL_RetVal=HAL_CAN_AddTxMessage(&hcan1,&TxMeg,pData+SendCNT,(uint32_t*)CAN_TX_MAILBOX0);
		if(HAL_RetVal!=HAL_OK)
		{
			return 2;
		}
		SendCNT+=8;
	}

  return 0;
}

enum {
	canDataNormal,
	canDataDisConnect,
	canDataNoDemarcate,
	canDatafault,
};

uint8_t  Data[8];
extern struct deviceInfo otherDeviceList[64];
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)  //接收回调函数
{
	HAL_StatusTypeDef	HAL_RetVal;
	if(hcan ==&hcan1){
	  HAL_RetVal=HAL_CAN_GetRxMessage(&hcan1,  CAN1FIFO, &RxMeg,  Data);
	  int addr = Data[5]>>8;
	  if ( HAL_OK==HAL_RetVal){
		  otherDeviceList[addr].use = 50;
		  switch(RxMeg.StdId & 0x00ff){
		  case 0:
			  otherDeviceList[addr].sensorValue = Data[0]*256+Data[1];
			  otherDeviceList[addr].erorrValue = Data[2];
			  switch(otherDeviceList[(RxMeg.StdId&0xff00)>>8].erorrValue){
				  case canDataNoDemarcate: otherDeviceList[addr].noDemarcate = 1; break;
				  case canDatafault:	   otherDeviceList[addr].sensorFault = 1; break;
				  case canDataDisConnect:  otherDeviceList[addr].noConnect = 1; break;
				  default:
					  otherDeviceList[addr].noDemarcate = 0;
					  otherDeviceList[addr].sensorFault = 0;
					  otherDeviceList[addr].noConnect = 0;
					  break;
			  };
			  otherDeviceList[addr].locInfoLenth = Data[3];
			  otherDeviceList[addr].uint = Data[4];
			  otherDeviceList[addr].addr = Data[5];
			  otherDeviceList[addr].highPercent = Data[6];
			  otherDeviceList[addr].lowPercent = Data[7];
			  break;
		  case 1:
			  otherDeviceList[addr].mainPowerFault = Data[0];
			  otherDeviceList[addr].bakPowerFault = Data[1];
			  break;
		  case 2:
			  memcpy(otherDeviceList[addr].loc, Data, 8);
			  break;
		  case 3:
			  memcpy(otherDeviceList[addr].loc+8, Data, 8);
			  break;
		  }
	  }
	}
}



extern struct otherSetStruct otherSetValue;
void sendCanInfo(void){
	uint8_t info[8];
	memset(info, 0, 8);
	if(deviceOwnInfo.addr == 64||deviceOwnInfo.isSetting == 1)
		return;
	int deviceAddr = (deviceOwnInfo.addr<<8)+0;
	info[0] = (deviceOwnInfo.sensorValue&0xff00)>>8;
	info[1] = deviceOwnInfo.sensorValue&0xff;
	if(!deviceOwnInfo.noConnect){
		if(deviceOwnInfo.noDemarcate){
			info[2] = canDataNoDemarcate;
		}
		else if(deviceOwnInfo.sensorFault){
			info[2] = canDatafault;
		}
		else
			info[2] = canDataNormal;
	}
	else{
		info[2] = canDataDisConnect;
	}
	info[3] = deviceOwnInfo.locInfoLenth;
	info[4] = deviceOwnInfo.uint;
	info[5] = deviceOwnInfo.addr;
	info[6] = deviceOwnInfo.highPercent;
	info[7] = deviceOwnInfo.lowPercent;
	CANx_SendNormalData(&hcan1, deviceAddr, info, 8);

	info[0] = deviceOwnInfo.mainPowerFault;
	info[1] = deviceOwnInfo.bakPowerFault;
	CANx_SendNormalData(&hcan1, deviceAddr+1, info, 8);


	memcpy(info, deviceOwnInfo.loc, 8);
	CANx_SendNormalData(&hcan1, deviceAddr+2, info, 8);
	memcpy(info, deviceOwnInfo.loc+8, 8);
	CANx_SendNormalData(&hcan1, deviceAddr+3, info, 8);
}

void detectPowerFault(void){
	if(!mainPowerNormal()&&deviceOwnInfo.mainPowerfix ==0){
		deviceOwnInfo.mainPowerFault = 1;
		deviceOwnInfo.mainPowerfix = 1;
		warnRecord(MainPowerlowRecordValue, 0);
	}
	else if(mainPowerNormal()){
		deviceOwnInfo.mainPowerfix = 0;
		deviceOwnInfo.mainPowerFault = 0;
	}
	if(!bakPowerNormal()&&deviceOwnInfo.bakPowerFault ==0){
		deviceOwnInfo.bakPowerFault = 1;
		deviceOwnInfo.bakPowerfix = 1;
		warnRecord(MainPowerlowRecordValue, 0);
	}
	else if(bakPowerNormal()){
		deviceOwnInfo.bakPowerfix = 0;
		deviceOwnInfo.bakPowerFault = 0;
	}
}

void addOwnToList(void){
	deviceOwnInfo.use = 100;
	if(deviceOwnInfo.isSetting == 0){
		otherDeviceList[deviceOwnInfo.addr] = deviceOwnInfo;
	}
}

void canTxAndKeepAlive(int count){
	if(!(count%5)){
		sendCanInfo();
		for(int i = 0; i < 64; i++)
			if(otherDeviceList[i].use)
				otherDeviceList[i].use--;
	}
}
void rollDisplay(int count){
	if(deviceOwnInfo.rollDisplay < 40)
		deviceOwnInfo.rollDisplay++;
	if(!(count%20)){
		if(deviceOwnInfo.mannerge && (deviceOwnInfo.rollDisplay == 40)){
			static int y=-1;
			static int i;
			for(; i < 64; i++){
				if((otherDeviceList[i].noConnect||  otherDeviceList[i].noDemarcate|| \
						otherDeviceList[i].mainPowerFault|| otherDeviceList[i].bakPowerFault ||\
						otherDeviceList[i].sensorFault )&& i > y){
					deviceInfoP = &otherDeviceList[i];
					y = i;
					break;
				}
			}
			if(i == 64){
				i = 0;
				y = -1;
			}
		}
	}
}

void uartDataSolveAndKeepAlive(int count){
	static int fault;
	static int recNumBak;
	if(uart2RecLoc > 3){
		osDelay(50);
		uartDataGet();
	}
	if(!(count%2)){
		sendDIsEnable();
		uartDataSend();
		sendDIsEnable();
	}
	if(recNumBak != uartSendInfo.recNum){
		recNumBak = uartSendInfo.recNum;
		deviceOwnInfo.noConnect = 0;
		fault = 0;
	}
	else{
		fault++;
		if(fault > 90){
			fault = 90;
			deviceOwnInfo.noConnect = 1;
		}
	}
}

void timeDisplayRefresh(int count){

	if(!(count%10)){
		DS1302_ReadTime(&datatime);
		if(pageInfoList[0].isDis)
			xSemaphoreGive(disSignal);
	}
}
extern uint8_t temporayValue;
void dataGetTask(void const * argument)
{
	int count = 0;
	gpioInit();
	Ds1302_Gpio_Init();
	HAL_UART_Receive_IT(&huart2, &temporayValue, 1);
	osDelay(1000);
	for(;;){
		osDelay(100);
		count++;
		timeDisplayRefresh(count);
		faultThing(count);
		detectPowerFault();
		addOwnToList();
		uartDataSolveAndKeepAlive(count);
		canTxAndKeepAlive(count);
		rollDisplay(count);
		count %= 40;
	}
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
