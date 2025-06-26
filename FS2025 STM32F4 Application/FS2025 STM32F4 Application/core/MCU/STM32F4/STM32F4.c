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
CAN_HandleTypeDef 	STM32_CAN1;
RTC_HandleTypeDef 	STM32_RTC;
TIM_HandleTypeDef 	STM32_TIM7;
IWDG_HandleTypeDef  Watchdog;           //IWDG
SPI_HandleTypeDef  	STM32_SPI1;
SPI_HandleTypeDef  	STM32_SPI2;



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
void STM32F4_GPIO_Init_C9_Output(void)
{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		//Configure frist state
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 SPI1  INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_SPI1_Init(void)
{
  STM32_SPI1.Instance = SPI1;
  STM32_SPI1.Init.Mode = SPI_MODE_MASTER;
  STM32_SPI1.Init.Direction = SPI_DIRECTION_2LINES;
  STM32_SPI1.Init.DataSize = SPI_DATASIZE_8BIT;
  STM32_SPI1.Init.CLKPolarity = SPI_POLARITY_HIGH;      // CPOL = 1
  STM32_SPI1.Init.CLKPhase = SPI_PHASE_2EDGE;           // CPHA = 1
  STM32_SPI1.Init.NSS = SPI_NSS_SOFT;                   // Usar GPIO para CS
  STM32_SPI1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 16 MHz / 32 = 500 kHz
  STM32_SPI1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  STM32_SPI1.Init.TIMode = SPI_TIMODE_DISABLE;
  STM32_SPI1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  STM32_SPI1.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&STM32_SPI1) != HAL_OK)
  {
    // Manejo de error
    STM32F4_Error_Handler();
  }
}

/*******************************************************************************
********************************************************************************
***************          	 	STM32F4 SPI2  INIT			 		  	 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates SPI2 FOR STM32F4
  * @retval NOTHING
  */
void STM32F4_SPI2_Init(void)
{
  STM32_SPI2.Instance = SPI3;
  STM32_SPI2.Init.Mode = SPI_MODE_MASTER;
  STM32_SPI2.Init.Direction = SPI_DIRECTION_2LINES;
  STM32_SPI2.Init.DataSize = SPI_DATASIZE_8BIT;
  STM32_SPI2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  STM32_SPI2.Init.CLKPhase = SPI_PHASE_2EDGE;
  STM32_SPI2.Init.NSS = SPI_NSS_SOFT;  // Control manual del CS (NSS)
  STM32_SPI2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  STM32_SPI2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  STM32_SPI2.Init.TIMode = SPI_TIMODE_DISABLE;
  STM32_SPI2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  STM32_SPI2.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&STM32_SPI2) != HAL_OK)
  {
    // Manejo de error
    STM32F4_Error_Handler();
  }
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
	
	#ifndef NO_WDT
		STM32F4_IWDG_Init();
	#endif
	// Initialize timers
	STM32F4_TIM7_Init();
	
	// Initialize CAN Module 1
	STM32F4_CAN1_Init();	
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
void STM32F4_GPIO_Write_C9_Output(BoolTypeDef value)
{
	if(value==TRUE)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	}
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
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



/***************************************************************************************************************************************************************
****************************************************************************************************************************************************************
****************************************************************************************************************************************************************
***************									FLASH FUNCTIONS				 			 		************************************************************************************************
****************************************************************************************************************************************************************
****************************************************************************************************************************************************************
****************************************************************************************************************************************************************/


/*******************************************************************************
********************************************************************************
***************								STM32F4 Read Word	            		****************	
********************************************************************************
********************************************************************************
	* @brief	Reads a word of flash memory at specified address
	* @param 	address Address to program
  * @retval word read
  */
uint32_t	STM32F4_Flash_Read_Word	(uint32_t address)
{
	return *(volatile uint32_t*)address;
}

/*******************************************************************************
********************************************************************************
***************								STM32F4 Read Byte	            		****************	
********************************************************************************
********************************************************************************
	* @brief	Reads a byte of flash memory at specified address
	* @param 	address Address to program
  * @retval byte read
  */
uint8_t	STM32F4_Flash_Read_Byte	(uint32_t address)
{
	return *(volatile uint8_t*)address;
}

/*******************************************************************************
********************************************************************************
***************					STM32F4 FLASH Program Byte	            ****************	
********************************************************************************
********************************************************************************
	* @brief	Programs a byte of flash memory at specified address
	* @param 	address Address to program
	* @param 	data Data to be programmed
  * @retval NOTHING
  */
void	STM32F4_Flash_Program_Byte	(uint32_t address, uint8_t data)
{
	//Unlock Flash to perform any operation
	HAL_FLASH_Unlock(); //Should return HAL Status, maybe implement check
	
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address, data) != HAL_OK)
  {
    STM32F4_Error_Handler();
  }
	
	//Lock Flash after operation end
	HAL_FLASH_Lock();
}

/*******************************************************************************
********************************************************************************
***************					STM32F4 FLASH Program Word	            ****************	
********************************************************************************
********************************************************************************
	* @brief	Programs a Word (32bit) of flash memory at specified address
	* @param 	address Address to program
	* @param 	data Data to be programmed
  * @retval NOTHING
  */
void	STM32F4_Flash_Program_Word	(uint32_t address, uint32_t data)
{
	//Unlock Flash to perform any operation
	HAL_FLASH_Unlock(); //Should return HAL Status, maybe implement check
	
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) != HAL_OK)
  {
		STM32F4_Error_Handler();
  }
	
	//Lock Flash after operation end
	HAL_FLASH_Lock();
}

/*******************************************************************************
********************************************************************************
***************					STM32F4 FLASH Erase Sector	            ****************	
********************************************************************************
********************************************************************************
	* @brief	Erases flash sector
	* @param 	sector Sector to be erased
  * @retval NOTHING
	*/
void 	STM32F4_Flash_Erase_Sector	(uint32_t sector)
{
	FLASH_EraseInitTypeDef Flash_Erase_InitStruct;
	uint32_t SectorError;
	
	//Unlock Flash to perform any operation
	HAL_FLASH_Unlock(); 		//Should return HAL Status, maybe implement check
	
	// Erase Struct Structure
  Flash_Erase_InitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;		// Sector Erase
  Flash_Erase_InitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; 	//2.7V to 3.6V
  Flash_Erase_InitStruct.Sector = sector; 											// Sector to erase
  Flash_Erase_InitStruct.NbSectors = 1;   											// Erase a single sector
	
	// Erase Flash Sector
	if (HAL_FLASHEx_Erase(&Flash_Erase_InitStruct, &SectorError) != HAL_OK) 
	{
		STM32F4_Error_Handler(); 
  }
	
	//Sector error mangement??
	
	//Lock Flash after operation end
	HAL_FLASH_Lock();
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

