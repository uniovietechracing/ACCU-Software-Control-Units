	/**
  ******************************************************************************
  * @file           : LTC6811.h
  * @brief          : LTC6811 header file
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
#ifndef LT_SPI_H
#define LT_SPI_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "MCU.h"

// ========== Inicialización SPI ==========
void LTC6811_Init_1(Control_Unit_TypeDef* Control_Unit);
void LTC6811_Init_2(Control_Unit_TypeDef* Control_Unit);


// Inicializa SPI1 para LTC6811 en el canal 1
void LTC6811_Initialize_1(void);

// Inicializa SPI3 para LTC6811 en el canal 2
void LTC6811_Initialize_2(void);



// ========== Control de Chip Select (CS) ==========

// Baja el pin CS del canal 1 (selecciona el LTC6811)
void LTC6811_CS_Low_1(void);

// Sube el pin CS del canal 1 (deselecciona el LTC6811)
void LTC6811_CS_High_1(void);

// Baja el pin CS del canal 2
void LTC6811_CS_Low_2(void);

// Sube el pin CS del canal 2
void LTC6811_CS_High_2(void);



// ========== Envío de datos ==========

// Envía un byte por SPI1
void LTC6811_SPI_Write_1(uint8_t data);

// Envía un byte por SPI2
void LTC6811_SPI_Write_2(uint8_t data);

// Envía múltiples bytes por SPI1
void LTC6811_SPI_Write_Array_1(uint8_t len, uint8_t data[]);

// Envía múltiples bytes por SPI2
void LTC6811_SPI_Write_Array_2(uint8_t len, uint8_t data[]);



// ========== Envío + Lectura (comandos con respuesta) ==========

// Envia tx_len bytes y recibe rx_len bytes usando SPI1
void LTC6811_SPI_Write_Read_1(uint8_t tx_Data[], uint8_t tx_len, uint8_t* rx_data, uint8_t rx_len);

// Envia tx_len bytes y recibe rx_len bytes usando SPI2
void LTC6811_SPI_Write_Read_2(uint8_t tx_Data[], uint8_t tx_len, uint8_t* rx_data, uint8_t rx_len);



// ========== Lectura directa ==========

// Lee un byte desde el bus SPI1
uint8_t LTC6811_SPI_Read_Byte_1(uint8_t tx_dat);

// Lee un byte desde el bus SPI2
uint8_t LTC6811_SPI_Read_Byte_2(uint8_t tx_dat);

#endif // LT_SPI_H

/**
  ******************************************************************************
  * @file           : State_LEDs.h
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