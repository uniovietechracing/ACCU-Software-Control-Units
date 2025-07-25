/**
  ******************************************************************************
  * @file           : MCU.c
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
#include "MCU.h"
#include "Control_Unit.h"
#include "Battery_Pack_Control_Unit.h"



/*******************************************************************************
********************************************************************************
***************           MCU INIT ERROR					         *****************	
********************************************************************************
********************************************************************************
  * @brief  Init Error Handler
  * @retval NOTHING
  */
void MCU_Init_Error(void)
{
	#ifdef STM32F4_MCU
		STM32F4_Init_Error();
	#endif 
}


/*******************************************************************************
********************************************************************************
***************           	 MCU FUNCTION ERROR 					     *****************	
********************************************************************************
********************************************************************************
  * @brief  Error Function 
  * @retval NOTHING
  */
void MCU_Error_Handler(void)
{
  #ifdef STM32F4_MCU
		STM32F4_Error_Handler();
	#endif 				
}



/*******************************************************************************
********************************************************************************
***************          	 			MCU GPIO C8 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR MCU
  * @retval NOTHING
  */
void MCU_GPIO_Init_C8_Output(void)
{
	#ifdef STM32F4_MCU
		STM32F4_GPIO_Init_C8_Output();
	#endif
}


/*******************************************************************************
********************************************************************************
***************          	 			MCU GPIO C9 INIT			  			 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initializates GPIO FOR MCU
  * @retval NOTHING
  */
void MCU_GPIO_Init_C9_Output(void)
{
	#ifdef STM32F4_MCU
		STM32F4_GPIO_Init_C9_Output();
	#endif
}



/*******************************************************************************
********************************************************************************
***************									MCU Write C8 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C8 output
  * @retval NOTHING
  */
void MCU_GPIO_Write_C8_Output(BoolTypeDef value)
{
	#ifdef STM32F4_MCU
		STM32F4_GPIO_Write_C8_Output(value);
	#endif
}


/*******************************************************************************
********************************************************************************
***************								MCU Write C9 OUTPUT						 *****************	
********************************************************************************
********************************************************************************
  * @brief  Write C9 output
  * @retval NOTHING
  */
void MCU_GPIO_Write_C9_Output(BoolTypeDef value)
{
	#ifdef STM32F4_MCU
		STM32F4_GPIO_Write_C9_Output(value);
	#endif
}


/*******************************************************************************
********************************************************************************
***************				MCU Initizalization Common interfaces		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie all FOR MCU MCU
  * @retval NOTHING
  */
void MCU_Init(void)
{
	#ifdef STM32F4_MCU
		STM32F4_Init();
	#endif
}

/*******************************************************************************
********************************************************************************
***************				SPI Initizalization Common interfaces		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie all FOR SPI MCU
  * @retval NOTHING
  */
void MCU_SPI1_Init(void)
{
	#ifdef STM32F4_MCU
		STM32F4_SPI1_Init();
	#endif
}

/*******************************************************************************
********************************************************************************
***************				SPI Initizalization Common interfaces		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Initialzie all FOR SPI MCU
  * @retval NOTHING
  */
void MCU_SPI2_Init(void)
{
	#ifdef STM32F4_MCU
		STM32F4_SPI2_Init();
	#endif
}



/*******************************************************************************
********************************************************************************
***************									MCU CAN BUS SEND					     *****************	
********************************************************************************
********************************************************************************
  * @brief  SEND CAN MESSAGE TO CAN1
  * @retval NOTHING
  */
void MCU_CAN1_Send(CAN_TxHeaderTypeDef* CAN_Header, uint8_t* Data , uint32_t* Mailbox)
{
	#ifdef STM32F4_MCU
		STM32F4_CAN1_Send(CAN_Header,Data,Mailbox);
	 #endif
}



/*******************************************************************************
********************************************************************************
***************							RESET WDT				 										 ***************
********************************************************************************
*******************************************************************************/

void MCU_WDT_Refresh(void)
{
	#ifdef STM32F4
	if(CONTROL_UNIT.Device.Reboot==FALSE)
	{
	  STM32F4_WDT_Refresh();
	}
	#endif
}

/*******************************************************************************
********************************************************************************
***************         	 	MCU Read Byte Flash 			    		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads a byte from flash memory 
	* @param	address Address to be read
  * @retval value stored at specified address
  */
uint8_t	MCU_Flash_Read_Byte	(uint32_t address)
{
	#ifdef STM32F4_MCU
		return STM32F4_Flash_Read_Byte	(address);
	#endif 	
}

/*******************************************************************************
********************************************************************************
***************         	 	MCU Read Word Flash 			    		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Reads a word from flash memory 
	* @param	address Address to be read
  * @retval value stored at specified address
  */
uint32_t	MCU_Flash_Read_Word	(uint32_t address)
{
	#ifdef STM32F4_MCU
		return STM32F4_Flash_Read_Word	(address);
	#endif 	
}

/*******************************************************************************
********************************************************************************
***************         	 	MCU Write Byte Flash 			    		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Writes a byte to flash memory 
	* @param	address Address to be written
	*	@param	data Data to be written
  * @retval NOTHING
  */
void	MCU_Flash_Program_Byte	(uint32_t address, uint8_t data)
{
	#ifdef STM32F4_MCU
		STM32F4_Flash_Program_Byte	(address,data);
	#endif 	
}


/*******************************************************************************
********************************************************************************
***************         	 	MCU Write Word Flash 			    		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Writes a word to flash memory 
	* @param	address Address to be written
	*	@param	data Data to be written
  * @retval NOTHING
  */
void	MCU_Flash_Program_Word	(uint32_t address, uint32_t data)
{
	#ifdef STM32F4_MCU
		STM32F4_Flash_Program_Word	(address,data);
	#endif 	
}


/*******************************************************************************
********************************************************************************
***************         	 MCU Erase Sector Flash			    		 *****************	
********************************************************************************
********************************************************************************
  * @brief  Writes a word to flash memory 
	* @param	sector Sector to be erased
  * @retval NOTHING
  */
void 	MCU_Flash_Erase_Sector	(uint32_t sector)
{
	#ifdef STM32F4_MCU
		STM32F4_Flash_Erase_Sector	(sector);
	#endif 	
}



	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : MCU.c
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

