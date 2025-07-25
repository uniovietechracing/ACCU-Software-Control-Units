/**
  ******************************************************************************
  * @file           : MCU.H
  * @brief          : MCU configuration and initizalitation header FILE
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

#ifndef MCU_H
#define MCU_H

/*******************************************************************************
********************************************************************************
***************										 Includes                      ***************	
********************************************************************************
*******************************************************************************/
#include "STM32F4.h"
#include "Common_Functions.H"
#include "Control_Unit_Selection.h"

/*******************************************************************************
********************************************************************************
***************							Error Hanlders Functions             ***************	
********************************************************************************
*******************************************************************************/
void MCU_Init_Error(void);
void MCU_Error_Handler(void);




/*******************************************************************************
********************************************************************************
***************							OUTPUTS Initialization      		     ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_GPIO_Init_C8_Output			(void);
void 	MCU_GPIO_Init_C9_Output			(void);

	

/*******************************************************************************
********************************************************************************
***************								OUTPUTS WRITE     		  				   ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_GPIO_Write_C8_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C9_Output			(BoolTypeDef value);


/*******************************************************************************
********************************************************************************
***************							MCU INITIALIZATION	                 ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_Init				(void);
void 	MCU_SPI1_Init		(void);
void 	MCU_SPI2_Init		(void);

/*******************************************************************************
********************************************************************************
***************												wdt	             			     ***************	
********************************************************************************
*******************************************************************************/
void MCU_WDT_Refresh(void);


/*******************************************************************************
********************************************************************************
***************											CAN BUS	       		           ***************	
********************************************************************************
*******************************************************************************/
void MCU_CAN1_Send(CAN_TxHeaderTypeDef* CAN_Header, uint8_t* Data , uint32_t* Mailbox);



/*******************************************************************************
********************************************************************************
***************					 			FLASH Memory Structure	          ****************	
********************************************************************************
*******************************************************************************/
/*******************************************************************************
********************************************************************************
***************					STM32F4 FLASH Memory Structure          ****************	
********************************************************************************
*******************************************************************************/

#define STM32F4_FLASH_BASE 0x08000000U

#define STM32F4_FLASH_SECTOR_1 STM32F4_FLASH_BASE
#define STM32F4_FLASH_SECTOR_2 0x08004000U
#define STM32F4_FLASH_SECTOR_3 0x08008000U
#define STM32F4_FLASH_SECTOR_4 0x08010000U
#define STM32F4_FLASH_SECTOR_5 0x08020000U
#define STM32F4_FLASH_SECTOR_6 0x08040000U
#define STM32F4_FLASH_SECTOR_7 0x08060000U

#define MCU_ADDRESS_BOOTLOADER 						STM32F4_FLASH_SECTOR_1
#define MCU_ADDRESS_BOOTLOADER_DATA 			STM32F4_FLASH_SECTOR_2 
#define MCU_ADDRESS_BOOTLOADER_DATA_BKUP	STM32F4_FLASH_SECTOR_3 
#define MCU_ADDRESS_APP										STM32F4_FLASH_SECTOR_5 
#define MCU_ADDRESS_APP_BKUP							STM32F4_FLASH_SECTOR_6 


/*******************************************************************************
********************************************************************************
***************									MCU FLASH                   		****************	
********************************************************************************
*******************************************************************************/
uint32_t	MCU_Flash_Read_Word	(uint32_t address);									/* [NEW FS25 Untested] */
uint8_t		MCU_Flash_Read_Byte	(uint32_t address);									/* [NEW FS25 Untested] */
void	MCU_Flash_Program_Byte	(uint32_t address, uint8_t data);		/* [NEW FS25 Untested] */
void	MCU_Flash_Program_Word	(uint32_t address, uint32_t data);	/* [NEW FS25 Untested] */
void 	MCU_Flash_Erase_Sector	(uint32_t sector);									/* [NEW FS25 Untested] */


#endif


	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : MCU.H
  * @brief          : MCU configuration and initizalitation header FILE
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

