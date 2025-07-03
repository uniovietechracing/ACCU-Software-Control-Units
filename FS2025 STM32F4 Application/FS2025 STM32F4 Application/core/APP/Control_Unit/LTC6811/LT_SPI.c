/**
  ******************************************************************************
  * @file           : LT_SPI.C
  * @brief          : LT_SPI header file
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
	
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "LT_SPI.h"
#include "stm32f4xx_hal.h"

// SPI handlers para dos interfaces independientes
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

//
// Inicialización de SPI1 para comunicación con LTC6811
//
void LTC6811_Initialize_1(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;                   // Modo maestro
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;         // Línea MISO y MOSI
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;             // Transmisión de 8 bits
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;          // Reloj inactivo en nivel alto
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;               // Datos capturados en flanco de bajada
  hspi1.Init.NSS = SPI_NSS_SOFT;                       // Control de CS por software
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Frecuencia SPI = 16MHz / 32 = 500kHz
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;              // Bit más significativo primero
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    // Manejo de error si falla la inicialización
  }
}

//
// Inicialización de SPI3 para comunicación con un segundo LTC6811
//
void LTC6811_Initialize_2(void)
{
  hspi2.Instance = SPI3;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    // Manejo de error si falla la inicialización
  }
}

//
// Control manual de pines CS (Chip Select) para SPI1
//
void LTC6811_CS_Low_1(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);  // Activar CS
}

void LTC6811_CS_High_1(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);    // Desactivar CS
}

//
// Control manual de pines CS (Chip Select) para SPI2
//
void LTC6811_CS_Low_2(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
}

void LTC6811_CS_High_2(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
}

//
// Transmisión de un solo byte a través de SPI1
//
void LTC6811_SPI_Write_1(uint8_t data)
{
    uint8_t ret_val;
    HAL_SPI_TransmitReceive(&hspi1, &data, &ret_val, 1, HAL_MAX_DELAY);
}

//
// Transmisión de un solo byte a través de SPI2
//
void LTC6811_SPI_Write_2(uint8_t data)
{
    uint8_t ret_val;
    HAL_SPI_TransmitReceive(&hspi2, &data, &ret_val, 1, HAL_MAX_DELAY);
}

//
// Transmisión de múltiples bytes por SPI1
//
void LTC6811_SPI_Write_Array_1(uint8_t len, uint8_t data[])
{
    uint8_t ret_val;
    for (uint8_t i = 0; i < len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi1, &data[i], &ret_val, 1, HAL_MAX_DELAY);
    }
}

//
// Transmisión de múltiples bytes por SPI2
//
void LTC6811_SPI_Write_Array_2(uint8_t len, uint8_t data[])
{
    uint8_t ret_val;
    for (uint8_t i = 0; i < len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi2, &data[i], &ret_val, 1, HAL_MAX_DELAY);
    }
}

//
// Transmisión + recepción (comando y respuesta) por SPI1
//
void LTC6811_SPI_Write_Read_1(uint8_t tx_Data[], uint8_t tx_len, uint8_t* rx_data, uint8_t rx_len)
{
    uint8_t dummy;
    for (uint8_t i = 0; i < tx_len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi1, &tx_Data[i], &dummy, 1, HAL_MAX_DELAY);
    }

    for (uint8_t i = 0; i < rx_len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)"\xFF", &rx_data[i], 1, HAL_MAX_DELAY);
    }
}

//
// Transmisión + recepción (comando y respuesta) por SPI2
//
void LTC6811_SPI_Write_Read_2(uint8_t tx_Data[], uint8_t tx_len, uint8_t* rx_data, uint8_t rx_len)
{
    uint8_t dummy;
    for (uint8_t i = 0; i < tx_len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi2, &tx_Data[i], &dummy, 1, HAL_MAX_DELAY);
    }

    for (uint8_t i = 0; i < rx_len; i++)
    {
        HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)"\xFF", &rx_data[i], 1, HAL_MAX_DELAY);
    }
}

//
// Lectura de un solo byte por SPI1 (enviar dummy y leer respuesta)
//
uint8_t LTC6811_SPI_Read_Byte_1(uint8_t tx_dat)
{
    uint8_t rx;
    if (HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)"\xFF", &rx, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        return rx;
    }
    return 0xFF; // Valor de error si falla
}

//
// Lectura de un solo byte por SPI2
//
uint8_t LTC6811_SPI_Read_Byte_2(uint8_t tx_dat)
{
    uint8_t rx;
    if (HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)"\xFF", &rx, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        return rx;
    }
    return 0xFF;
}
