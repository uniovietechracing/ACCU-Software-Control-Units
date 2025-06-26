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
***************							ADC Initialization      		         ***************	
********************************************************************************
*******************************************************************************/
void  MCU_ADC1_Init								(unsigned char number_of_conversions);
void 	MCU_ADC1_Channel_Init				(unsigned char Channel,		unsigned char rank);
void 	MCU_DMA_Init								(void);
void 	MCU_ADC_Start								(uint32_t *Buffer,unsigned char lenght);



/*******************************************************************************
********************************************************************************
***************							OUTPUTS Initialization      		     ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_GPIO_Init_C0_Output			(void);
void 	MCU_GPIO_Init_C5_Output			(void);
void 	MCU_GPIO_Init_C6_Output			(void);
void 	MCU_GPIO_Init_C7_Output			(void);
void 	MCU_GPIO_Init_C8_Output			(void);
void 	MCU_GPIO_Init_C13_Output		(void);
void 	MCU_GPIO_Init_B2_Output			(void);
void 	MCU_GPIO_Init_B12_Output		(void);
void 	MCU_GPIO_Init_B13_Output		(void);
void 	MCU_GPIO_Init_B14_Output		(void);
void 	MCU_GPIO_Init_B15_Output		(void);
void 	MCU_GPIO_Init_A1_Output		(void);
void 	MCU_GPIO_Init_A2_Output		(void);
void 	MCU_GPIO_Init_A3_Output		(void);
void 	MCU_GPIO_Init_A4_Output		(void);

/*******************************************************************************
********************************************************************************
***************								INPUTS Initialization      		     ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_GPIO_Init_C3_Input			(void);
void 	MCU_GPIO_Init_C10_Input			(void);
void 	MCU_GPIO_Init_C11_Input			(void);
void 	MCU_GPIO_Init_C12_Input			(void);
void 	MCU_GPIO_Init_B4_Input			(void);
void 	MCU_GPIO_Init_B5_Input			(void);
void 	MCU_GPIO_Init_B6_Input			(void);
void 	MCU_GPIO_Init_B7_Input			(void);
void 	MCU_GPIO_Init_D2_Input			(void);
void 	MCU_GPIO_Init_A1_Input			(void);
void 	MCU_GPIO_Init_A2_Input			(void);
void 	MCU_GPIO_Init_A3_Input			(void);
void 	MCU_GPIO_Init_A4_Input			(void);
void 	MCU_GPIO_Init_A9_Input			(void);
	
/*******************************************************************************
********************************************************************************
***************										INPUTS READ     		  			   ***************	
********************************************************************************
*******************************************************************************/
BoolTypeDef 	MCU_GPIO_Read_C3_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_C10_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_C11_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_C12_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_B4_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_B5_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_B6_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_B7_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_D2_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_A1_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_A2_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_A3_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_A4_Input			(void);
BoolTypeDef 	MCU_GPIO_Read_A9_Input			(void);


/*******************************************************************************
********************************************************************************
***************								OUTPUTS WRITE     		  				   ***************	
********************************************************************************
*******************************************************************************/
void 	MCU_GPIO_Write_C0_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C5_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C6_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C7_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C8_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_C13_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_B2_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_B12_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_B13_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_B14_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_B15_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_A1_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_A2_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_A3_Output			(BoolTypeDef value);
void 	MCU_GPIO_Write_A4_Output			(BoolTypeDef value);

/*******************************************************************************
********************************************************************************
***************							MCU INITIALIZATION	                 ***************	
********************************************************************************
*******************************************************************************/
void MCU_Init(void);


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
void MCU_CAN2_Send(CAN_TxHeaderTypeDef* CAN_Header, uint8_t* Data , uint32_t* Mailbox);



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

