	/**
  ******************************************************************************
  * @file           : Control_Unit.h
  * @brief          : Control Unit Functions header file
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
#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

/*******************************************************************************
********************************************************************************
***************										 Includes                      ***************	
********************************************************************************
*******************************************************************************/
#include "Control_Unit_Selection.h"
#include "TypeDefs.h"


/*******************************************************************************
********************************************************************************
***************									GLOBAL Control Unit              ***************	
********************************************************************************
*******************************************************************************/
extern Control_Unit_TypeDef CONTROL_UNIT;



/*******************************************************************************
********************************************************************************
***************										 Init Function                 ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_MCU_Init(void);
void Control_Unit_Init(void);

/*******************************************************************************
********************************************************************************
***************											MAIN TASK                		 ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_Main_Task(void);



/*******************************************************************************
********************************************************************************
***************								Timer 10ms Interrupt               ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_Timer_10ms_Interrupt(void);



/*******************************************************************************
********************************************************************************
***************									CAN Bus Interrupt                ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_CAN1_Interrupt(void);


#endif

	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : Control_Unit.h
  * @brief          : Control Unit Functions header file
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
	
	
	