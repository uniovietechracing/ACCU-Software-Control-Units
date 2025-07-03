#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "LT_SPI.h"
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef hspi1;

void LTC6811_Initialize(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;      // CPOL = 1
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;           // CPHA = 1
  hspi1.Init.NSS = SPI_NSS_SOFT;                   // Usar GPIO para CS
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 16 MHz / 32 = 500 kHz
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    // Manejo de error
    //STM32F4_Error_Handler();
  }
}

void cs_low(void)
{
    // Set chip select pin low.
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void cs_high(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void spi_write(uint8_t data)
{
		uint8_t ret_val;
		HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&data, &ret_val, 1, HAL_MAX_DELAY);
}

void spi_write_array(uint8_t len, 
                     uint8_t data[])
{
    uint8_t ret_val;
    uint8_t i;

    for ( i = 0; i < len; i++ )
    {
        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&data[i], &ret_val, 1, HAL_MAX_DELAY);
    }
}

// spi_write_read

void spi_write_read(uint8_t  tx_Data[],
                    uint8_t  tx_len, 
                    uint8_t* rx_data, 
                    uint8_t  rx_len)
{
    uint8_t i;
    uint8_t data;

    // Transfer data to LTC6803
    for ( i = 0; i < tx_len; i++ )
    {
        // Transmit byte.
        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&tx_Data[i], &data, 1, HAL_MAX_DELAY);
    }

    // Receive data from DC2259A board.
    for ( i = 0; i < rx_len; i++ )
    {
        // Receive byte.
        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&rx_data[i], 1, HAL_MAX_DELAY);
    }
}

// spi_read_byte

uint8_t spi_read_byte(uint8_t tx_dat)
{
    uint8_t data;

    if ( HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&data, 1, HAL_MAX_DELAY) == HAL_OK )
    {
        return(data);
    }
		return(-1);
}
