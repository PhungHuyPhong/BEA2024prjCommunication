/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t uart3_receive;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;
CAN_TxHeaderTypeDef CAN1_pHeader;
CAN_RxHeaderTypeDef CAN1_pHeaderRx;
CAN_FilterTypeDef CAN1_sFilterConfig;
CAN_TxHeaderTypeDef CAN2_pHeader;
CAN_RxHeaderTypeDef CAN2_pHeaderRx;
CAN_FilterTypeDef CAN2_sFilterConfig;
uint32_t CAN1_pTxMailbox;
uint32_t CAN2_pTxMailbox;

uint16_t NumBytesReq = 0;
uint8_t  REQ_BUFFER  [4096];
uint8_t  REQ_1BYTE_DATA;

uint8_t CAN1_DATA_TX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t CAN1_DATA_RX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t CAN2_DATA_TX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t CAN2_DATA_RX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint16_t Num_Consecutive_Tester;
uint8_t  Flg_Consecutive = 0;

unsigned int TimeStamp;
// maximum characters send out via UART is 30
char bufsend[30]="XXX: D1 D2 D3 D4 D5 D6 D7 D8  ";

uint8_t MessageCounter = 0;
uint8_t flag = 0;
uint8_t flag1 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void MX_CAN1_Setup();
void MX_CAN2_Setup();
void USART3_SendString(uint8_t *ch);
void PrintCANLog(uint16_t CANID, uint8_t * CAN_Frame);
void SID_22_Practice();
void SID_2E_Practice();
void SID_27_Practice();
void delay(uint16_t delay);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t calc_SAE_J1850(uint8_t data[], uint8_t crc_len);
void CAN_Tx(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint32_t *pTxMailbox, uint8_t *data_rx, uint8_t *data_tx, uint16_t id);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/* USER CODE END 0 */
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	//uint16_t i,j = 0;
	//uint16_t Consecutive_Cntr = 0;
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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  MX_CAN1_Setup();
  MX_CAN2_Setup();
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // Example Function to print can message via uart
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	CAN_Tx(&hcan2, &CAN2_pHeader, &CAN2_pTxMailbox, CAN2_DATA_RX, CAN2_DATA_TX, 0xA2);
	delay(2000);
	CAN_Tx(&hcan1, &CAN1_pHeader, &CAN1_pTxMailbox, CAN1_DATA_RX, CAN1_DATA_TX, 0x12);
	delay(1990);
	MessageCounter = MessageCounter + 1;
  }

  memset(&REQ_BUFFER,0x00,4096);
  NumBytesReq = 0;

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  CAN1_sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  CAN1_sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  CAN1_sFilterConfig.SlaveStartFilterBank = 13;
  CAN1_sFilterConfig.FilterBank = 8;
  CAN1_sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN1_sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  CAN1_sFilterConfig.FilterIdHigh = 0x0A2 << 5;
  CAN1_sFilterConfig.FilterIdLow = 0;
  CAN1_sFilterConfig.FilterMaskIdHigh = 0x0A2 << 5;
  CAN1_sFilterConfig.FilterMaskIdLow = 0;
  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 1;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  CAN2_sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  CAN2_sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  CAN2_sFilterConfig.SlaveStartFilterBank = 13;
  CAN2_sFilterConfig.FilterBank = 19;
  CAN2_sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN2_sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  CAN2_sFilterConfig.FilterIdHigh = 0x012 << 5;
  CAN2_sFilterConfig.FilterIdLow = 0;
  CAN2_sFilterConfig.FilterMaskIdHigh = 0x012 << 5;
  CAN2_sFilterConfig.FilterMaskIdLow = 0;
  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC4 PC5 PC6
                           PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void MX_CAN1_Setup()
{
	HAL_CAN_ConfigFilter(&hcan1, &CAN1_sFilterConfig);
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}
void MX_CAN2_Setup()
{
	HAL_CAN_ConfigFilter(&hcan2, &CAN2_sFilterConfig);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void USART3_SendString(uint8_t *ch)
{
   while(*ch!=0)
   {
      HAL_UART_Transmit(&huart3, ch, 1,HAL_MAX_DELAY);
      ch++;
   }
}
void PrintCANLog(uint16_t CAN_ID, uint8_t * CAN_Frame)
{
	uint16_t loopIndx = 0;
		char bufID[3] = "   ";
		char bufDat[2] = "  ";
		char bufTime [8]="        ";

		sprintf(bufTime,"%d",TimeStamp);
		USART3_SendString((uint8_t*)bufTime);
		USART3_SendString((uint8_t*)" ");

		sprintf(bufID,"%03X",CAN_ID);
		for(loopIndx = 0; loopIndx < 3; loopIndx ++)
		{
			bufsend[loopIndx]  = bufID[loopIndx];
		}
		bufsend[3] = ':';
		bufsend[4] = ' ';

		for(loopIndx = 0; loopIndx < 8; loopIndx ++ )
		{
			sprintf(bufDat,"%02X",CAN_Frame[loopIndx]);
			bufsend[loopIndx*3 + 5] = bufDat[0];
			bufsend[loopIndx*3 + 6] = bufDat[1];
			bufsend[loopIndx*3 + 7] = ' ';
		}
		bufsend[29] = '\n';
		USART3_SendString((unsigned char*)bufsend);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	REQ_BUFFER[NumBytesReq] = REQ_1BYTE_DATA;
	NumBytesReq++;
	//REQ_BUFFER[7] = NumBytesReq;
}
void delay(uint16_t delay)
{
	HAL_Delay(delay);
}
uint8_t calc_SAE_J1850(uint8_t data[], uint8_t crc_len)
{
    uint8_t idx, crc, temp1, temp2, idy;
    crc = 0;
    idx = 0;
    idy = 0;
    temp1 = 0;
    temp2 = 0;
    for(idx=0;idx < crc_len+1;idx++)
    {
        if(idx == 0)
        {
            temp1 = 0;
        }
        else
        {
            temp1 = data[crc_len-idx];
        }
        crc = (crc^temp1);
        for(idy=(uint8_t)8; idy>0; idy--)
        {
            // Save the value before the top bit is shifted out.
            temp2 = crc;
            crc <<= 1;
            if (0 != (temp2 & (uint8_t)128))
                crc ^= 0x1D;
        }
    }
    return crc;
}
void CAN_Tx(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint32_t *pTxMailbox, uint8_t *data_rx, uint8_t *data_tx, uint16_t id){
	pHeader->StdId = id;
	pHeader->DLC = 8;
	pHeader->IDE = CAN_ID_STD;
	pHeader->RTR = CAN_RTR_DATA;
	if(flag1 == 1){
		memcpy(CAN2_DATA_RX, CAN1_DATA_TX, 8*sizeof(uint8_t));
		flag1 = 0;
	}
	if(((data_rx[7] == calc_SAE_J1850(data_rx,7)) && (flag == 0)) || ((flag == 1) && (hcan == &hcan1))){
		data_tx[0] = 0x0A;
		data_tx[1] = 0x02;
		if(data_rx == CAN1_DATA_RX){
			data_tx[2] = 0x0C;
		}
	}
	else{
		data_tx[0] = 0x00;
		data_tx[1] = 0x00;
	}
	data_tx[6] = MessageCounter;
	if(flag == 0){
		data_tx[7] = calc_SAE_J1850(data_tx,7);
	}
	else{
		if(hcan == &hcan2) CAN2_DATA_TX[7] = calc_SAE_J1850(CAN2_DATA_TX,7);
	}
	char buffer[10];
	sprintf(buffer, "CAN %u TX\n", (hcan == &hcan1) ? 1 : 2);
	USART3_SendString((unsigned char *)buffer);
	PrintCANLog(id, data_tx);
	HAL_CAN_AddTxMessage(hcan, pHeader, data_tx, pTxMailbox);
	MessageCounter = MessageCounter & 0xF;
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
	if(hcan == &hcan1){
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_pHeaderRx, CAN1_DATA_RX);
		char buffer1[10] = "CAN 1 RX\n";
		USART3_SendString((unsigned char *)buffer1);
		PrintCANLog(0xA2, CAN1_DATA_RX);
	}
	else if(hcan == &hcan2){
		HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &CAN2_pHeaderRx, CAN2_DATA_RX);
		if(CAN2_DATA_RX[7] == calc_SAE_J1850(CAN2_DATA_RX,7)){
			char buffer2[10] = "CAN 2 RX\n";
			USART3_SendString((unsigned char *)buffer2);
			PrintCANLog(0x12, CAN2_DATA_RX);
		}
		else{
			char buffer3[26] = "CAN 2 RX ERROR WRONG CRC\n";
			USART3_SendString((unsigned char *)buffer3);
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_1)
	{
		delay(20);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
		{
			switch(flag)
			{
				case 0:
					flag = flag + 1;
					USART3_SendString((uint8_t *)"IG OFF\n");
					CAN1_DATA_TX[7] = calc_SAE_J1850(CAN1_DATA_TX,7) & 0x0;
					break;
				case 1:
					flag = 0;
					USART3_SendString((uint8_t *)"-> IG ON\n");
					CAN1_DATA_TX[7] = calc_SAE_J1850(CAN1_DATA_TX,7);
					flag1 = flag1 +1;
					break;
				default:
					break;
			}
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
