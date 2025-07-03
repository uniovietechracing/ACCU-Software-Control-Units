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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LT_SPI.h"
#include "LTC6811.h"
#include "LTC681x.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <math.h>
#include "Battery_Pack_Control_Unit.h"





/* Private function prototypes -----------------------------------------------*/

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


	Battery_Pack_Control_Unit_MCU_Init();
	
	//initialize error counter and LTC/SPI

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			
			LTC6811_Measure_Temperatures_and_Voltages(&CONTROL_UNIT);
			HAL_Delay(5000);
  }
  /* USER CODE END 3 */
}





