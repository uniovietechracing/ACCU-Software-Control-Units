	/**
  ******************************************************************************
  * @file           : TypeDefs.h
  * @brief          : Typedefs header file
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

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

/*******************************************************************************
********************************************************************************
***************										 Includes                      ***************	
********************************************************************************
*******************************************************************************/
#include "Control_Unit_Selection.h"
#include "Common_Functions.h"
#include "MCU.h"





#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
/*******************************************************************************
********************************************************************************
***************							MAX TEMPERATURE VALUES               ***************	
********************************************************************************
*******************************************************************************/
#define MAX_TEMPERATURE_VALUES			80
#endif


/*******************************************************************************
********************************************************************************
***************									Boot Status        							 ***************
********************************************************************************
*******************************************************************************/
typedef enum
{
	BOOTLOADER_NO,
	BOOTLOADER_YES,
	BOOTLOADER_NEW_DEVICE=255
} Bootloader_Status_TypeDef;


/*******************************************************************************
********************************************************************************
***************												DEVICE       				  		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	uint8_t											Unique_ID[4];											
	uint8_t											Device;
	uint8_t											Version;
	BoolTypeDef									Reboot;
} Device_TypeDef;


typedef enum
{
	STATE_OFF,
	STATE_ON,
	STATE_FAIL
} Accu_Relay_State_Typedef;

typedef struct
{
	Accu_Relay_State_Typedef	Intentional_State;
	Accu_Relay_State_Typedef	Real_State;
} Relay_State_Typedef;


/*******************************************************************************
********************************************************************************
***************												CAN TX       				  		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	uint16_t										ID;						//11 BITS for Standard ID
	uint8_t											DLC;					//Data Length
	uint8_t											Data[8];			//Data Array
	uint32_t           			    Mailbox;			//MAILBOX
	uint32_t           			    Mailbox_2;			//MAILBOX
} CAN_Tx_Message_TypeDef;


/*******************************************************************************
********************************************************************************
***************												CAN RX       				  		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	CAN_RxHeaderTypeDef					Header;
	uint8_t											Data[8];			//Data Array

} CAN_Rx_Message_TypeDef;


/*******************************************************************************
********************************************************************************
***************									Estructura TIMERS				    		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Timer_10ms_TypeDef	Initialisation_Tim;
	  Timer_10ms_TypeDef	Status_Send_Tim;
		Timer_10ms_TypeDef	RTDS_Tim;
		Timer_10ms_TypeDef	Fuses_Read_Tim;
	#endif
	
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
	  Timer_10ms_TypeDef	Status_Send_Tim;
		Timer_10ms_TypeDef	Precharge_Tim;
		Timer_10ms_TypeDef	Precharge_Status_Tim;
		Timer_10ms_TypeDef	ADCs_Reading_Tim;
		Timer_10ms_TypeDef  Temp_Timeout;
	#endif

} Control_Unit_Time_TypeDef;


/*******************************************************************************
********************************************************************************
***************			Rear Control Unit Power Distribution Status	 ***************
********************************************************************************
*******************************************************************************/
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD

typedef struct
{
	BoolTypeDef 	Relay_State;
	BoolTypeDef 	Fuse_State;
	
} Rear_Control_Unit_Relay_Status_TypeDef;
	#endif



/*******************************************************************************
********************************************************************************
***************									  STATUS Struct			        		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
		Rear_Control_Unit_Relay_Status_TypeDef  Relays[7];
		BoolTypeDef															Battery_Relay;
		BoolTypeDef															Brake_Light;
		BoolTypeDef															RTDS;
		uint32_t																LV_Voltage[1];
	#endif
	#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
		Relay_State_Typedef				Precharge_Relay;
		Relay_State_Typedef				Air_Plus_Relay;
		Relay_State_Typedef				Air_Minus_Relay;
		Relay_State_Typedef				HV_Feedback;
		BoolTypeDef 							Precharge_Status;
		BoolTypeDef 							SDC_Status;
		BoolTypeDef 							BMS_Status;
		BoolTypeDef 							RTD_Send;
		uint32_t									Relays_Value[2];
	#endif
} Control_Unit_Status_Typdef;

/*******************************************************************************
********************************************************************************
***************									  enum    state				      		 ***************
********************************************************************************
*******************************************************************************/

typedef enum
{
#ifdef REAR_CONTROL_UNIT_POWER_DISTRIBUTION_BOARD
	INIT,
	RELAY_INIT_0,
	RELAY_INIT_1,
	RELAY_INIT_2,
	RELAY_INIT_3,
	NORMAL_OPERATION,
	TESTING_MODE
#endif
#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
	NORMAL_OPERATION,
	SDC_BREAK
#endif
} Control_Unit_State_Typdef;



/*******************************************************************************
********************************************************************************
***************											LED				      						 ***************
********************************************************************************
*******************************************************************************/
typedef enum
{
	PERMANENT_OFF,
	PERMANENT_ON,
	BLINKING,
	ONE_BLINK

} LEDs_Mode_Typdef;

typedef enum
{
	ON,
	OFF

} LEDs_State_Typdef;

typedef enum
{
	GREEN,
	YELLOW

} LEDs_Color_Typdef;
typedef struct
{
		LEDs_State_Typdef												State;
		LEDs_Mode_Typdef												Mode;
		LEDs_Mode_Typdef												Mode_ant;
		Timer_10ms_TypeDef											Led_Tim;
		LEDs_Color_Typdef												Color;
		unsigned int														Blink_Rate;

} Control_Unit_Led_Typedef;

typedef struct
{
		int8_t						Thermistor_ID;
		int8_t						Real_Value;
	  uint8_t						cont;
} Control_Unit_Thermistor_Typedef;

typedef struct
{
		int8_t																Max_Temperature_Value;
		int8_t																Min_Temperature_Value;
		Control_Unit_Thermistor_Typedef				Thermistor_Module[MAX_TEMPERATURE_VALUES];
} Control_Unit_Temperature_Typedef;

/*******************************************************************************
********************************************************************************
***************							CAN CONTROL UNIT       				  		 ***************
********************************************************************************
*******************************************************************************/
typedef struct
{
	Device_TypeDef												Device;
	CAN_Tx_Message_TypeDef							  Tx_Message;
	CAN_Rx_Message_TypeDef							  Rx_Message;		
	Control_Unit_Time_TypeDef			    	  Timing;
	Control_Unit_State_Typdef						  State;
	Control_Unit_Status_Typdef						Status;
	Control_Unit_Led_Typedef							Green_Led;
	Control_Unit_Led_Typedef							Yellow_Led;
#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD
	Control_Unit_Temperature_Typedef			Temperature;
#endif
} Control_Unit_TypeDef;

#endif
	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  ******************************************************************************
  * @file           : Common_Functions.h
  * @brief          : Common Functions and Typedefs header file
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


