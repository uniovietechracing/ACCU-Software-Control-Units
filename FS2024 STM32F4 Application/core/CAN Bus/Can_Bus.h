	/**
  ******************************************************************************
  * @file           : CAN_BUS.h
  * @brief          : CAN_BUS header file
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
	
#ifndef CAN_BUS_H
#define CAN_BUS_H

/*******************************************************************************
********************************************************************************
***************						  			 	INCLUDES					  				 ***************
********************************************************************************
*******************************************************************************/
#include "Control_Unit_Selection.h"
#include "TypeDefs.h"
#include "Control_Unit.h"

/*******************************************************************************
********************************************************************************
***************								CAN MESSAGES                       ***************	
********************************************************************************
*******************************************************************************/
typedef enum
{
	BPCU_INIT_MEASURE_1=		0x50, 	//Init Measure
	BPCU1_TEMP_1=						0x51,	//Temperature Message 1
	BPCU1_TEMP_2=						0x52,	//Temperature Message 2
	BPCU1_TEMP_3=						0x53,	//Temperature Message 3
	BPCU1_TEMP_4=						0x54,	//Temperature Message 4
	BPCU1_VOLT_1=						0x55,	//Temperature Message 1
	BPCU1_VOLT_2=						0x56,	//Temperature Message 2
	BPCU1_VOLT_3=						0x57,	//Temperature Message 3
	BPCU1_FINISHED_MEASURE=	0x60,	//Finished Measure
	BPCU1_STATUS=						0x100, //Status Message
	BPCU1_CANCEL_SENSOR_1=	0x410,	//Active/Cancel sensor
	BPCU1_CANCEL_SENSOR_2=	0x411,	//Active/Cancel sensor
	BPCU1_CANCEL_SENSOR_3=	0x412,	//Active/Cancel sensor
	BPCU1_REBOOT=						0x506, //Reboot
	
	BPCU2_TEMP_1= 					0x61,	//Temperature Message 1
	BPCU2_TEMP_2= 					0x62,	//Temperature Message 2
	BPCU2_TEMP_3= 					0x63,	//Temperature Message 3
	BPCU2_TEMP_4= 					0x64,	//Temperature Message 4
	BPCU2_VOLT_1= 					0x65,	//Temperature Message 1
	BPCU2_VOLT_2= 					0x66,	//Temperature Message 2
	BPCU2_VOLT_3= 					0x67,	//Temperature Message 3
	BPCU2_FINISHED_MEASURE= 0x70,	//Finished Measure
	BPCU2_STATUS= 					0x101, //Status Message
	BPCU2_CANCEL_SENSOR_1= 	0x420,	//Active/Cancel sensor
	BPCU2_CANCEL_SENSOR_2= 	0x421,	//Active/Cancel sensor
	BPCU2_CANCEL_SENSOR_3= 	0x422,	//Active/Cancel sensor
	BPCU2_REBOOT=						0x507, //Reboot
	
	BPCU3_TEMP_1= 					0x71,	//Temperature Message 1
	BPCU3_TEMP_2= 					0x72,	//Temperature Message 2
	BPCU3_TEMP_3= 					0x73,	//Temperature Message 3
	BPCU3_TEMP_4= 					0x74,	//Temperature Message 4
	BPCU3_VOLT_1= 					0x75,	//Temperature Message 1
	BPCU3_VOLT_2= 					0x76,	//Temperature Message 2
	BPCU3_VOLT_3= 					0x77,	//Temperature Message 3
	BPCU3_FINISHED_MEASURE= 0x80,	//Finished Measure
	BPCU3_STATUS= 					0x102, //Status Message
	BPCU3_CANCEL_SENSOR_1= 	0x430,	//Active/Cancel sensor
	BPCU3_CANCEL_SENSOR_2= 	0x431,	//Active/Cancel sensor
	BPCU3_CANCEL_SENSOR_3= 	0x432,	//Active/Cancel sensor
	BPCU3_REBOOT=						0x508, //Reboot
	
	BPCU4_TEMP_1= 					0x81,	//Temperature Message 1
	BPCU4_TEMP_2= 					0x82,	//Temperature Message 2
	BPCU4_TEMP_3= 					0x83,	//Temperature Message 3
	BPCU4_TEMP_4= 					0x84,	//Temperature Message 4
	BPCU4_VOLT_1= 					0x85,	//Temperature Message 1
	BPCU4_VOLT_2= 					0x86,	//Temperature Message 2
	BPCU4_VOLT_3= 					0x87,	//Temperature Message 3
	BPCU4_FINISHED_MEASURE= 0x90,	//Finished Measure
	BPCU4_STATUS= 					0x103, //Status Message
	BPCU4_CANCEL_SENSOR_1= 	0x440,	//Active/Cancel sensor
	BPCU4_CANCEL_SENSOR_2= 	0x441,	//Active/Cancel sensor
	BPCU4_CANCEL_SENSOR_3= 	0x442,	//Active/Cancel sensor
	BPCU4_REBOOT=						0x509, //Reboot
	
	
	BMS_MASTER_STATUS=0x253,
	PRECHARGUE_STATUS=0x221,
	START_PRECHARGE=0x223,
	DONE_PRECHARGE=0x224,
	RTD=0x222,
	FAN_CONTROL=0x370,
	PDB_REBOOT=0x500,
} CAN_Messages_Enum;




/*******************************************************************************
********************************************************************************
***************						  			 	CAN 1					  		    		 ***************
********************************************************************************
*******************************************************************************/
void CAN1_Send(CAN_Tx_Message_TypeDef* CAN_Message);
void CAN1_Interrupt_DoTask(void);


/*******************************************************************************
********************************************************************************
***************						  			 	CAN 2					  		    		 ***************
********************************************************************************
*******************************************************************************/
void CAN2_Send(CAN_Tx_Message_TypeDef* CAN_Message);
void CAN2_Interrupt_DoTask(void);



#endif