	/**
  ******************************************************************************
  * @file           : Control_Unit.c
  * @brief          : Control Unit Functions file
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
#include "Control_Unit.h"



/*******************************************************************************
********************************************************************************
***************									GLOBAL Control Unit              ***************	
********************************************************************************
*******************************************************************************/
Control_Unit_TypeDef CONTROL_UNIT;


/*******************************************************************************
********************************************************************************
***************										 Init Function                 ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_MCU_Init()
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Power_Distribution_MCU_Init();
	#endif

	#ifdef REAR_CONTROL_UNIT_BOARD
		Rear_Control_Unit_MCU_Init();
	#endif

	#ifdef FRONT_CONTROL_UNIT_BOARD
		Front_Control_Unit_MCU_Init();
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_MCU_Init();
	#endif
}


/*******************************************************************************
********************************************************************************
***************										 Init Function                 ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_Init()
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Power_Distribution_Init(&CONTROL_UNIT);
	#endif

	#ifdef REAR_CONTROL_UNIT_BOARD
		Rear_Control_Unit_Init(Control_Unit_TypeDef* Control_Unit);
	#endif

	#ifdef FRONT_CONTROL_UNIT_BOARD
		Front_Control_Unit_Init(Control_Unit_TypeDef* Control_Unit);
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_Task_Init(&CONTROL_UNIT);
	#endif
}


/*******************************************************************************
********************************************************************************
***************											MAIN TASK                		 ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_Main_Task()
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Power_Distribution_Main_Task(&CONTROL_UNIT);
	#endif

	#ifdef REAR_CONTROL_UNIT_BOARD
		Rear_Control_Unit_Main_Task(&CONTROL_UNIT);
	#endif

	#ifdef FRONT_CONTROL_UNIT_BOARD
		Front_Control_Unit_Main_Task(&CONTROL_UNIT);
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_Main_Task(&CONTROL_UNIT);
	#endif
}

/*******************************************************************************
********************************************************************************
***************								Timer 10ms Interrupt               ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_Timer_10ms_Interrupt()
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Power_Distribution_10ms_Interrupt(&CONTROL_UNIT);
	#endif

	#ifdef REAR_CONTROL_UNIT_BOARD
		Rear_Control_Unit_10ms_Interrupt(&CONTROL_UNIT);
	#endif

	#ifdef FRONT_CONTROL_UNIT_BOARD
		Front_Control_Unit_10ms_Interrupt(&CONTROL_UNIT);
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_10ms_Interrupt(&CONTROL_UNIT);
	#endif
}


/*******************************************************************************
********************************************************************************
***************								CAN Bus Interrupt               ***************	
********************************************************************************
*******************************************************************************/
void Control_Unit_CAN1_Interrupt()
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Power_Distribution_CAN1_Interrupt(&CONTROL_UNIT);
	#endif

	#ifdef REAR_CONTROL_UNIT_BOARD
		Rear_Control_Unit_CAN1_Interrupt(&CONTROL_UNIT);
	#endif

	#ifdef FRONT_CONTROL_UNIT_BOARD
		Front_Control_Unit_CAN1_Interrupt(&CONTROL_UNIT);
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_CAN1_Interrupt(&CONTROL_UNIT);
	#endif
}

void Control_Unit_CAN2_Interrupt()
{
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Accu_Master_Control_Unit_CAN2_Interrupt(&CONTROL_UNIT);
	#endif
}

	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : Control_Unit.c
  * @brief          : Control Unit Functions file
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
	
	