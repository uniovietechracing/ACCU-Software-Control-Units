	/**
  ******************************************************************************
  * @file           : LTC6811_SPI.h
  * @brief          : State_LEDs header file
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
#include "MCU.h"


void LTC6811_1_Initialize(void)
{
	MCU_SPI1_Init();
}

void LTC6811_2_Initialize(void)
{
	MCU_SPI2_Init();
}


void LTC6811_1_CS_Low(void)
{
	HAL_GPIO_WritePin(LTC6811_1_CHIP_SELECT_GPIO_PORT, LTC6811_1_CHIP_SELECT_GPIO_PIN, GPIO_PIN_RESET);
}
void LTC6811_2_CS_Low(void)
{
	HAL_GPIO_WritePin(LTC6811_2_CHIP_SELECT_GPIO_PORT, LTC6811_2_CHIP_SELECT_GPIO_PIN, GPIO_PIN_RESET);
}

void LTC6811_1_CS_High(void)
{
	HAL_GPIO_WritePin(LTC6811_1_CHIP_SELECT_GPIO_PORT, LTC6811_1_CHIP_SELECT_GPIO_PIN, GPIO_PIN_SET);
}

void LTC6811_2_CS_High(void)
{
	HAL_GPIO_WritePin(LTC6811_2_CHIP_SELECT_GPIO_PORT, LTC6811_2_CHIP_SELECT_GPIO_PIN, GPIO_PIN_SET);
}


void LTC6811_1_SPI_Write(uint8_t data)
{
		uint8_t ret_val;
		HAL_SPI_TransmitReceive(&STM32_SPI1, (uint8_t*)&data, &ret_val, 1, HAL_MAX_DELAY);
}

void LTC6811_2_SPI_Write(uint8_t data)
{
		uint8_t ret_val;
		HAL_SPI_TransmitReceive(&STM32_SPI2, (uint8_t*)&data, &ret_val, 1, HAL_MAX_DELAY);
}

void LTC6811_1_SPI_Write_Array(uint8_t len, uint8_t data[])
{
    uint8_t ret_val;
    uint8_t i;

    for ( i = 0; i < len; i++ )
    {
        HAL_SPI_TransmitReceive(&STM32_SPI1, (uint8_t*)&data[i], &ret_val, 1, HAL_MAX_DELAY);
    }
}

void LTC6811_2_SPI_Write_Array(uint8_t len, uint8_t data[])
{
    uint8_t ret_val;
    uint8_t i;

    for ( i = 0; i < len; i++ )
    {
        HAL_SPI_TransmitReceive(&STM32_SPI2, (uint8_t*)&data[i], &ret_val, 1, HAL_MAX_DELAY);
    }
}

// spi_write_read

void LTC6811_1_SPI_Write_Read(uint8_t  tx_Data[],uint8_t  tx_len,  uint8_t* rx_data, uint8_t  rx_len)
{
    uint8_t i;
    uint8_t data;

    // Transfer data
    for ( i = 0; i < tx_len; i++ )
    {
        // Transmit byte.
        HAL_SPI_TransmitReceive(&STM32_SPI1, (uint8_t*)&tx_Data[i], &data, 1, HAL_MAX_DELAY);
    }

    // Receive data.
    for ( i = 0; i < rx_len; i++ )
    {
        // Receive byte.
        HAL_SPI_TransmitReceive(&STM32_SPI1, (uint8_t*)0xFF, (uint8_t*)&rx_data[i], 1, HAL_MAX_DELAY);
    }
}

void LTC6811_2_SPI_Write_Read(uint8_t  tx_Data[],uint8_t  tx_len,  uint8_t* rx_data, uint8_t  rx_len)
{
    uint8_t i;
    uint8_t data;

    // Transfer data
    for ( i = 0; i < tx_len; i++ )
    {
        // Transmit byte.
        HAL_SPI_TransmitReceive(&STM32_SPI2, (uint8_t*)&tx_Data[i], &data, 1, HAL_MAX_DELAY);
    }

    // Receive data.
    for ( i = 0; i < rx_len; i++ )
    {
        // Receive byte.
        HAL_SPI_TransmitReceive(&STM32_SPI2, (uint8_t*)0xFF, (uint8_t*)&rx_data[i], 1, HAL_MAX_DELAY);
    }
}

// spi_read_byte

uint8_t LTC6811_1_SPI_Read_Byte(uint8_t tx_dat)
{
    uint8_t data;

    if ( HAL_SPI_TransmitReceive(&STM32_SPI1, (uint8_t*)0xFF, (uint8_t*)&data, 1, HAL_MAX_DELAY) == HAL_OK )
    {
        return(data);
    }
		return(-1);
}

uint8_t LTC6811_2_SPI_Read_Byte(uint8_t tx_dat)
{
    uint8_t data;

    if ( HAL_SPI_TransmitReceive(&STM32_SPI2, (uint8_t*)0xFF, (uint8_t*)&data, 1, HAL_MAX_DELAY) == HAL_OK )
    {
        return(data);
    }
		return(-1);
}
