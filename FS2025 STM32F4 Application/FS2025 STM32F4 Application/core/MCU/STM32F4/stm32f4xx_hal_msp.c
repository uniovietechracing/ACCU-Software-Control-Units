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
}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 SPI INIT					     *****************	
********************************************************************************
********************************************************************************
* @brief SPI MSP Initialization
* This function freeze the hardware resources used in this example
* @param HSPI: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(hspi->Instance == SPI1)
    {
        /* 1. Habilitar relojes */
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* 2. Configurar pines SCK, MISO, MOSI en AF5 */
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // PA5, PA6, PA7
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* 3. Configurar NSS (PA4) como GPIO salida para control manual */
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Inicializar NSS en alto (desactivado) */
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    }
    else if(hspi->Instance == SPI3)
    {
        /* 1. Habilitar relojes */
        __HAL_RCC_SPI3_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* 2. Configurar pines SCK, MISO, MOSI en AF6 */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12; // PC10, PC11, PC12
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* 3. Configurar NSS (PA15) como GPIO salida para control manual */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Inicializar NSS en alto (desactivado) */
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
    }
}

/*******************************************************************************
********************************************************************************
***************           		STM32F4 SPI DEINIT					     *****************	
********************************************************************************
********************************************************************************
* @brief SPI MSP Initialization
* This function freeze the hardware resources used in this example
* @param HSPI: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if(hspi->Instance == SPI1)
    {
        /* 1. Deshabilitar reloj SPI1 */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /* 2. Desinicializar pines PA4, PA5, PA6, PA7 */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

        /* 3. Deshabilitar interrupciones si las configuraste */
        // HAL_NVIC_DisableIRQ(SPI1_IRQn);
    }
    else if(hspi->Instance == SPI2)
    {
        /* 1. Deshabilitar reloj SPI2 */
        __HAL_RCC_SPI2_CLK_DISABLE();

        /* 2. Desinicializar pines PC10, PC11, PC12 y PA15 */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

        /* 3. Deshabilitar interrupciones */
        // HAL_NVIC_DisableIRQ(SPI2_IRQn);
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
