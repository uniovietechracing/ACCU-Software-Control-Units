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
***************										 DEFINES                       ***************	
********************************************************************************
*******************************************************************************/
#ifdef BATTERY_PACK_CONTROL_UNIT_1
	#define BPCU_INIT_MEASURE_DEF				0x50 	//Init Measure
	#define BPCU_TEMP_1_DEF 						0x51	//Temperature Message 1
	#define BPCU_TEMP_2_DEF 						0x52	//Temperature Message 2
	#define BPCU_TEMP_3_DEF 						0x53	//Temperature Message 3
	#define BPCU_TEMP_4_DEF 						0x54	//Temperature Message 4
	#define BPCU_VOLT_1_DEF 						0x55	//Temperature Message 1
	#define BPCU_VOLT_2_DEF 						0x56	//Temperature Message 2
	#define BPCU_VOLT_3_DEF 						0x57	//Temperature Message 3
	#define BPCU_FINISHED_MEASURE 			0x60	//Finished Measure
	#define BPCU_STATUS_DEF 						0x100 //Status Message
	#define BPCU_CANCEL_SENSOR_1_DEF 		0x410	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_2_DEF 		0x411	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_3_DEF 		0x412	//Active/Cancel sensor
	#define BPCU_REBOOT_DEF							0x506 //Reboot
#endif

#ifdef BATTERY_PACK_CONTROL_UNIT_2
	#define BPCU_INIT_MEASURE_DEF				0x60 	//Init Measure
	#define BPCU_TEMP_1_DEF 						0x61	//Temperature Message 1
	#define BPCU_TEMP_2_DEF 						0x62	//Temperature Message 2
	#define BPCU_TEMP_3_DEF 						0x63	//Temperature Message 3
	#define BPCU_TEMP_4_DEF 						0x64	//Temperature Message 4
	#define BPCU_VOLT_1_DEF 						0x65	//Temperature Message 1
	#define BPCU_VOLT_2_DEF 						0x66	//Temperature Message 2
	#define BPCU_VOLT_3_DEF 						0x67	//Temperature Message 3
	#define BPCU_FINISHED_MEASURE 			0x70	//Finished Measure
	#define BPCU_STATUS_DEF 						0x101 //Status Message
	#define BPCU_CANCEL_SENSOR_1_DEF 		0x420	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_2_DEF 		0x421	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_3_DEF 		0x422	//Active/Cancel sensor
	#define BPCU_REBOOT_DEF							0x507 //Reboot
#endif

#ifdef BATTERY_PACK_CONTROL_UNIT_3
	#define BPCU_INIT_MEASURE_DEF				0x70 	//Init Measure
	#define BPCU_TEMP_1_DEF 						0x71	//Temperature Message 1
	#define BPCU_TEMP_2_DEF 						0x72	//Temperature Message 2
	#define BPCU_TEMP_3_DEF 						0x73	//Temperature Message 3
	#define BPCU_TEMP_4_DEF 						0x74	//Temperature Message 4
	#define BPCU_VOLT_1_DEF 						0x75	//Temperature Message 1
	#define BPCU_VOLT_2_DEF 						0x76	//Temperature Message 2
	#define BPCU_VOLT_3_DEF 						0x77	//Temperature Message 3
	#define BPCU_FINISHED_MEASURE 			0x80	//Finished Measure
	#define BPCU_STATUS_DEF 						0x102 //Status Message
	#define BPCU_CANCEL_SENSOR_1_DEF 		0x430	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_2_DEF 		0x431	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_3_DEF 		0x432	//Active/Cancel sensor
	#define BPCU_REBOOT_DEF							0x508 //Reboot
#endif

#ifdef BATTERY_PACK_CONTROL_UNIT_4
	#define BPCU_INIT_MEASURE_DEF				0x80 	//Init Measure
	#define BPCU_TEMP_1_DEF 						0x81	//Temperature Message 1
	#define BPCU_TEMP_2_DEF 						0x82	//Temperature Message 2
	#define BPCU_TEMP_3_DEF 						0x83	//Temperature Message 3
	#define BPCU_TEMP_4_DEF 						0x84	//Temperature Message 4
	#define BPCU_VOLT_1_DEF 						0x85	//Temperature Message 1
	#define BPCU_VOLT_2_DEF 						0x86	//Temperature Message 2
	#define BPCU_VOLT_3_DEF 						0x87	//Temperature Message 3
	#define BPCU_FINISHED_MEASURE 			0x90	//Finished Measure
	#define BPCU_STATUS_DEF 						0x103 //Status Message
	#define BPCU_CANCEL_SENSOR_1_DEF 		0x440	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_2_DEF 		0x441	//Active/Cancel sensor
	#define BPCU_CANCEL_SENSOR_3_DEF 		0x442	//Active/Cancel sensor
	#define BPCU_REBOOT_DEF							0x509 //Reboot
#endif




/*******************************************************************************
********************************************************************************
***************						  			 	CAN 1					  		    		 ***************
********************************************************************************
*******************************************************************************/
void CAN1_Send(CAN_Tx_Message_TypeDef* CAN_Message);
void CAN1_Interrupt_DoTask(void);



#endif