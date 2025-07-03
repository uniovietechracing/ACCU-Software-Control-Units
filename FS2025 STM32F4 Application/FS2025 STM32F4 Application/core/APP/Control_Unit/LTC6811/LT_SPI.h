#ifndef LT_SPI_H
#define LT_SPI_H

#include <stdint.h>

#define LTC6811_CHIP_SELECT_GPIO_PIN            GPIO_PIN_4     // D10
#define LTC6811_CHIP_SELECT_GPIO_PORT           GPIOA

//Initialize the SPI Peripheral

void LTC6811_Initialize_1(void);

// LTC6811_CS_Low_1

void LTC6811_CS_Low_1(void);

// LTC6811_CS_High_1

void LTC6811_CS_High_1(void);

// LTC6811_SPI_Write_1

void LTC6811_SPI_Write_1(uint8_t data); //Bytes to be written on the SPI port

// LTC6811_SPI_Write_Array_1

void LTC6811_SPI_Write_Array_1(uint8_t len, // Option: Number of bytes to be written on the SPI port
                     uint8_t data[] //Array of bytes to be written on the SPI port
                    );

/*
 Writes and read a set number of bytes using the SPI port.
*/

void LTC6811_SPI_Write_Read_1(uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   );

uint8_t LTC6811_SPI_Read_Byte_1(uint8_t tx_dat);//name conflicts with linduino also needs to take a byte as a parameter




void LTC6811_Initialize_2(void);

// LTC6811_CS_Low_1

void LTC6811_CS_Low_2(void);

// LTC6811_CS_High_1

void LTC6811_CS_High_2(void);

// LTC6811_SPI_Write_1

void LTC6811_SPI_Write_2(uint8_t data); //Bytes to be written on the SPI port

// LTC6811_SPI_Write_Array_1

void LTC6811_SPI_Write_Array_2(uint8_t len, // Option: Number of bytes to be written on the SPI port
                     uint8_t data[] //Array of bytes to be written on the SPI port
                    );

/*
 Writes and read a set number of bytes using the SPI port.
*/

void LTC6811_SPI_Write_Read_2(uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   );

uint8_t LTC6811_SPI_Read_Byte_2(uint8_t tx_dat);//name conflicts with linduino also needs to take a byte as a parameter
#endif
