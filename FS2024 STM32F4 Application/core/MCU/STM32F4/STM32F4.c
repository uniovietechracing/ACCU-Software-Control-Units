	/**
  ******************************************************************************
  * @file           : STM32F4.c
  * @brief          : MCU configuration and initizalitation Code
  ******************************************************************************
  * @attention
  *
  * (c) 2024 Uniovi E-tech Racing.
  * 
  *
  ******************************************************************************  
  ******************************************************************************
																Version Control
	******************************************************************************
	******************************************************************************		
  Version | dd mmm yyyy |       Who        | Description of changes
  ========|=============|==================|====================================
    1.0   | 01 MAR 2023 | Diego Rodríguez  | Creation
	========|=============|==================|====================================
	
  ******************************************************************************
  ******************************************************************************
  */


/*******************************************************************************
********************************************************************************
***************										 Includes                      ***************	
********************************************************************************
*******************************************************************************/
#include "STM32F4.h"


/*******************************************************************************
********************************************************************************
***************										Global Handlers                ***************	
********************************************************************************
*******************************************************************************/
ADC_HandleTypeDef 	STM32_ADC1;
CAN_HandleTypeDef 	STM32_CAN1;
CAN_HandleTypeDef 	STM32_CAN2;
RTC_HandleTypeDef 	STM32_RTC;
TIM_HandleTypeDef 	STM32_TIM7;
DMA_HandleTypeDef		STM32_ADC1_DMA;
IWDG_HandleTypeDef  Watchdog;           //IWDG



/*******************************************************************************
********************************************************************************
***************           STM32F4 INIT ERROR					         *****************	
********************************************************************************
********************************************************************************
  * @brief  Init Error Handler
  * @retval NOTHING
  */
void STM32F4_Init_Error(void){
	while(1){}
}


/*******************************************************************************
********************************************************************************
***************           STM32F4 FUNCTION ERROR 					     *****************	
********************************************************************************
********************************************************************************
  * @brief  Error Function 
  * @retval NOTHING
  */
void STM32F4_Error_Handler(void)
{
  __disable_irq();	//Disable Interrupts
  while (1){}				
}



/*******************************************************************************
********************************************************************************
***************           STM32F4 System Clock Init 					 *****************	
********************************************************************************
********************************************************************************
  * @brief  System Clock Config STM32F4
  * @retval NOTHING
  */
void STM32F4_SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure the main internal regulator output voltage
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  // Initializes the RCC Oscillators according to the specified parameters
	// in the RCC_OscInitTypeDef structure.
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	
  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
}



/*******************************************************************************
********************************************************************************
***************         	  STM32F4 ADC1  Init 						  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  ADC1 Config STM32F4
  * @retval NOTHING
  */
void STM32F4_ADC1_Init(unsigned char number_of_conversions)
{
  //Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  STM32_ADC1.Instance = ADC1;
  STM32_ADC1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  STM32_ADC1.Init.Resolution = ADC_RESOLUTION_12B;
  STM32_ADC1.Init.ScanConvMode = ENABLE;
  STM32_ADC1.Init.ContinuousConvMode = ENABLE;
  STM32_ADC1.Init.DiscontinuousConvMode = DISABLE;
  STM32_ADC1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  STM32_ADC1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  STM32_ADC1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  STM32_ADC1.Init.NbrOfConversion = number_of_conversions;
  STM32_ADC1.Init.DMAContinuousRequests = ENABLE;
  STM32_ADC1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&STM32_ADC1) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
}
	
/*******************************************************************************
********************************************************************************
***************         	  STM32F4 ADC1 CHANNEL 1  Init 			 *****************	
********************************************************************************
********************************************************************************
  * @brief  ADC1 Channel 1 Config STM32F4
  * @retval NOTHING
  */
void STM32F4_ADC1_Channel_Init(unsigned char Channel,unsigned char rank)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
		ADC_ChannelConfTypeDef Channel_Config = {0};
		// Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
		Channel_Config.Channel = Channel;
		Channel_Config.Rank = rank;
		Channel_Config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
		if (HAL_ADC_ConfigChannel(&STM32_ADC1, &Channel_Config) != HAL_OK)
		{
			STM32F4_Error_Handler();
		}
		
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		
		if(Channel==ADC_CHANNEL_0)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_0;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_1)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_1;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_2)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_2;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_3)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_3;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_4)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_4;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_10)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_0;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_11)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_1;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		}
		else if(Channel==ADC_CHANNEL_12)
		{
			GPIO_InitStruct.Pin = GPIO_PIN_2;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		}
		
		
}


/*******************************************************************************
********************************************************************************
***************         	  STM32F4 DMA 4 ADC  Init 					*****************	
********************************************************************************
********************************************************************************
  * @brief  DMA 4 ADC Config STM32F4
  * @retval NOTHING
  */
void STM32F4_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/*******************************************************************************
********************************************************************************
***************         	  STM32F4 adc start   			 				 *****************	
********************************************************************************
********************************************************************************
  * @brief  ADC start STM32F4
  * @retval NOTHING
  */
void STM32F4_ADC_Start(uint32_t *Buffer,unsigned char lenght)
{
	HAL_ADC_Start_DMA(&STM32_ADC1, Buffer, lenght);
}


/*******************************************************************************
********************************************************************************
***************           STM32F4 CAN 1 MODULE Init 					 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates CAN 1 MODULE
  * @retval NOTHING
  */
void STM32F4_CAN1_Init(void)
{
	// Configure for the selected CAN: Prescaler, mode, Adquisition time and baudrate
  STM32_CAN1.Instance = CAN1;
  STM32_CAN1.Init.Prescaler = 2;
  STM32_CAN1.Init.Mode = CAN_MODE_NORMAL;
  STM32_CAN1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  STM32_CAN1.Init.TimeSeg1 = CAN_BS1_13TQ;
  STM32_CAN1.Init.TimeSeg2 = CAN_BS2_2TQ;
  STM32_CAN1.Init.TimeTriggeredMode = DISABLE;
  STM32_CAN1.Init.AutoBusOff = DISABLE;
  STM32_CAN1.Init.AutoWakeUp = DISABLE;
  STM32_CAN1.Init.AutoRetransmission = DISABLE;
  STM32_CAN1.Init.ReceiveFifoLocked = DISABLE;
  STM32_CAN1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&STM32_CAN1) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&STM32_CAN1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    STM32F4_Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&STM32_CAN1) != HAL_OK)
  {
    /* Start Error */
    STM32F4_Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&STM32_CAN1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    STM32F4_Error_Handler();
  }
}


/*******************************************************************************
********************************************************************************
***************           STM32F4 CAN 2 MODULE Init 					 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates CAN 1 MODULE
  * @retval NOTHING
  */
void STM32F4_CAN2_Init(void)
{
	// Configure for the selected CAN: Prescaler, mode, Adquisition time and baudrate
  STM32_CAN2.Instance = CAN2;
  STM32_CAN2.Init.Prescaler = 2;
  STM32_CAN2.Init.Mode = CAN_MODE_NORMAL;
  STM32_CAN2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  STM32_CAN2.Init.TimeSeg1 = CAN_BS1_13TQ;
  STM32_CAN2.Init.TimeSeg2 = CAN_BS2_2TQ;
  STM32_CAN2.Init.TimeTriggeredMode = DISABLE;
  STM32_CAN2.Init.AutoBusOff = DISABLE;
  STM32_CAN2.Init.AutoWakeUp = DISABLE;
  STM32_CAN2.Init.AutoRetransmission = DISABLE;
  STM32_CAN2.Init.ReceiveFifoLocked = DISABLE;
  STM32_CAN2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&STM32_CAN2) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 14;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&STM32_CAN2, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    STM32F4_Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&STM32_CAN2) != HAL_OK)
  {
    /* Start Error */
    STM32F4_Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&STM32_CAN2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    STM32F4_Error_Handler();
  }
}


/*******************************************************************************
********************************************************************************
***************          	 STM32F4 TIMER 7 INIT			 				   *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates TIMER 7 for 10ms interrupt
  * @retval NOTHING
  */
void STM32F4_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  STM32_TIM7.Instance = TIM7;
  STM32_TIM7.Init.Prescaler = 15;
  STM32_TIM7.Init.CounterMode = TIM_COUNTERMODE_UP;
  STM32_TIM7.Init.Period = 999;
  STM32_TIM7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&STM32_TIM7) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&STM32_TIM7, &sMasterConfig) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	__HAL_TIM_SET_COUNTER(&STM32_TIM7, 0);	 
	__HAL_TIM_CLEAR_IT(&STM32_TIM7,TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&STM32_TIM7);
}



/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A1 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
 void STM32F4_GPIO_Init_A1_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A2 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
 void STM32F4_GPIO_Init_A2_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A4 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
 void STM32F4_GPIO_Init_A4_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C0 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
 void STM32F4_GPIO_Init_C0_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C5 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C5_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C6 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C6_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,GPIO_PIN_RESET);
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C7 INIT			 			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C7_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,GPIO_PIN_RESET);

		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}



/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C8 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C8_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET);

		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}




/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C13 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C13_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B2 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B2_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B12 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B12_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B13 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B13_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B14 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B14_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B15 INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B15_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A3 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A3_Output(void)
{
			GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C3 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C3_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C10 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C10_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C11 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C11_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO C12 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_C12_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B4 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B4_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B5 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B5_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B6 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B6_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO B7 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_B7_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A1 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A1_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A1 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A2_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A1 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A3_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A1 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A4_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO A9 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_A9_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 GPIO D2 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_GPIO_Init_D2_Input(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOD_CLK_ENABLE();
		
		//Configure pin
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}





/*******************************************************************************
********************************************************************************
***************		STM32F4 Initizalization Common interfaces		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie all FOR MCU STM32F4
  * @retval NOTHING
  */
void STM32F4_Common_Init(void)
{
	
	// Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();

  // Configure the system clock
  STM32F4_SystemClock_Config();
	//STM32F4_IWDG_Init();
	
	// Initialize timers
	STM32F4_TIM7_Init();
	
	// Initialize CAN Module 1
	STM32F4_CAN1_Init();
	
#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
	STM32F4_CAN2_Init();
#endif
}


/*******************************************************************************
********************************************************************************
***************		STM32F4 Initizalization Common interfaces		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie all FOR MCU STM32F4
  * @retval NOTHING
  */
void STM32F4_Init(void)
{
	STM32F4_Common_Init();
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read C3 INPUT						   *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads C3 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_C3_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_3)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read C10 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads C10 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_C10_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}

/*******************************************************************************
********************************************************************************
***************							STM32F4 Read C11 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads C11 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_C11_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read C12 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads C12 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_C12_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Read B4 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads B4 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_B4_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read B5 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads B5 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_B5_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read B6 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads B6 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_B6_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read B7 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads B7 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_B7_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Read A1 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads A1 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_A1_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read A2 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads A2 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_A2_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Read A3 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads A3 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_A3_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Read A4 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads A4 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_A4_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read A9 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads A9 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_A9_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write A01 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write A1 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_A1_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write A02 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write A2 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_A2_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write A03 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write A3 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_A3_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write A04 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write A4 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_A4_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Read D2 INTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads D2 input
  * @retval Readed value
  */
BoolTypeDef STM32F4_GPIO_Read_D2_Input(void)
{
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)==GPIO_PIN_SET)
	{
		return TRUE;
	}
	else return FALSE;
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C0 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C0 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C0_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C5 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C5 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C5_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C6 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C6 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C6_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C7 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C7 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C7_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C8 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C8 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C8_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write C13 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C13 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_C13_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write B2 OUTPUT					   *****************	
********************************************************************************
********************************************************************************
  * @Crief  Write B2 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_B2_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	}
	else 
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	}
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write B12 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write B12 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_B12_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	}
	else 
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	}
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write B13 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write B13 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_B13_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}


/*******************************************************************************
********************************************************************************
***************							STM32F4 Write B14 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write B14 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_B14_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}



/*******************************************************************************
********************************************************************************
***************							STM32F4 Write B15 OUTPUT					*****************	
********************************************************************************
********************************************************************************
  * @Crief  Write B15 output
  * @retval NOTHING
  */
void STM32F4_GPIO_Write_B15_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
}



/*******************************************************************************
********************************************************************************
***************						 STM32F4 CAN BUS SEND					       *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie CAN FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_CAN1_Send(CAN_TxHeaderTypeDef* CAN_Header, uint8_t* Data , uint32_t* Mailbox)
{
	if (HAL_CAN_AddTxMessage(&STM32_CAN1, CAN_Header, Data, Mailbox) != HAL_OK)
	{
		STM32F4_Error_Handler ();
	}
}


/*******************************************************************************
********************************************************************************
***************						 STM32F4 CAN 2 BUS SEND					       ***************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie CAN FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_CAN2_Send(CAN_TxHeaderTypeDef* CAN_Header, uint8_t* Data , uint32_t* Mailbox)
{
	if (HAL_CAN_AddTxMessage(&STM32_CAN2, CAN_Header, Data, Mailbox) != HAL_OK)
	{
		STM32F4_Error_Handler ();
	}
}




/*******************************************************************************
********************************************************************************
*****************  									IWDG INIT      *****************************
********************************************************************************
********************************************************************************
  * @brief  WDT CONFIGURATION
  * @retval nothing
  */

void  STM32F4_IWDG_Init(void)
{
	Watchdog.Instance = IWDG;
  Watchdog.Init.Prescaler = IWDG_PRESCALER_16;
  Watchdog.Init.Reload = 4095;
  if (HAL_IWDG_Init(&Watchdog) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	
}


/*******************************************************************************
********************************************************************************
***************			          REFRESH WDT		                   *****************	
********************************************************************************
*******************************************************************************/

void STM32F4_WDT_Refresh (void)
{
		HAL_IWDG_Refresh(&Watchdog);
}


	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : STM32F4.c
  * @brief          : MCU configuration and initizalitation Code
  ******************************************************************************
  * @attention
  *
  * (c) 2024 Uniovi E-tech Racing.
  * 
  *
  ******************************************************************************  
  ******************************************************************************
																Version Control
	******************************************************************************
	******************************************************************************		
  Version | dd mmm yyyy |       Who        | Description of changes
  ========|=============|==================|====================================
    1.0   | 01 MAR 2023 | Diego Rodríguez  | Creation
	========|=============|==================|====================================
	
  ******************************************************************************
  ******************************************************************************
  */

