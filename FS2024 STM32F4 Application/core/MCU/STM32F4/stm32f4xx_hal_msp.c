	/**
  ******************************************************************************
  * @file           : STM32F4.c
  * @brief          : MCU INTIALIZATION AND DEINITIALIZATION FUNCTIONS AUTO-CALLED
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
***************           		STM32F4  INIT					      		 *****************	
********************************************************************************
********************************************************************************
  * @brief  STM32F4  INIT	
  * @retval NOTHING
  */
void HAL_MspInit(void)
{
	 //Peripheral clock enable
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}


/*******************************************************************************
********************************************************************************
***************           		STM32F4 ADC INIT					       *****************	
********************************************************************************
********************************************************************************
* @brief ADC MSP Initialization
* This function configures the hardware resources used 
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    //Peripheral clock enable
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
		
    /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    */
    //ADC1 DMA Init 
    //ADC1 Init 
    STM32_ADC1_DMA.Instance = DMA2_Stream0;
    STM32_ADC1_DMA.Init.Channel = DMA_CHANNEL_0;
    STM32_ADC1_DMA.Init.Direction = DMA_PERIPH_TO_MEMORY;
    STM32_ADC1_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
    STM32_ADC1_DMA.Init.MemInc = DMA_MINC_ENABLE;
    STM32_ADC1_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    STM32_ADC1_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    STM32_ADC1_DMA.Init.Mode = DMA_CIRCULAR;
    STM32_ADC1_DMA.Init.Priority = DMA_PRIORITY_LOW;
    STM32_ADC1_DMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&STM32_ADC1_DMA) != HAL_OK)
    {
      STM32F4_Error_Handler();
    }

    __HAL_LINKDMA(hadc,DMA_Handle,STM32_ADC1_DMA);
  }
}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 ADC DEINIT					     *****************	
********************************************************************************
********************************************************************************
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
    //Peripheral clock disable 
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);

    //ADC1 DMA DeInit
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 CAN INIT					       *****************	
********************************************************************************
********************************************************************************
* @brief CAN MSP Initialization
* This function configures the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)
  {
    //Peripheral clock enable 
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
		
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		//CAN1 interrupt Init
		HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  }
	
	if(hcan->Instance==CAN2)
  {
    //Peripheral clock enable 
		__HAL_RCC_CAN2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
		
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//CAN2 interrupt Init
		HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
  }
}


/*******************************************************************************
********************************************************************************
***************           		STM32F4 CAN DEINIT					     *****************	
********************************************************************************
********************************************************************************
* @brief CAN MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN1)
  {
    //Peripheral clock disable
    __HAL_RCC_CAN1_CLK_DISABLE();

    /*CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
		
		//CAN1 interrupt DeInit
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  }
	
	if(hcan->Instance==CAN2)
  {
    //Peripheral clock disable
		__HAL_RCC_CAN2_CLK_DISABLE();

    /*CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);
	
		//CAN2 interrupt DeInit
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
  }
}




/*******************************************************************************
********************************************************************************
***************           		STM32F4 RTC DEINIT					     *****************	
********************************************************************************
********************************************************************************
* @brief RTC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    //Peripheral clock disable 
    __HAL_RCC_RTC_DISABLE();
  }
}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 TIM7 INIT					       *****************	
********************************************************************************
********************************************************************************
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM7)
  {
    //Peripheral clock enable 
    __HAL_RCC_TIM7_CLK_ENABLE();
		
    //TIM7 interrupt Init 
    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }

}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 TIM7 DEINIT					     *****************	
********************************************************************************
********************************************************************************
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM7)
  {
    //Peripheral clock disable
    __HAL_RCC_TIM7_CLK_DISABLE();

    //TIM7 interrupt DeInit 
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  }
}

	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
  ******************************************************************************
  * @file           : STM32F4.c
  * @brief          : MCU INTIALIZATION AND DEINITIALIZATION FUNCTIONS AUTO-CALLED
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
