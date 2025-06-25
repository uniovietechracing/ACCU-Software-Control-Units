/**
******************************************************************************
* @file           : Rear_Control_Unit_Power_Distribution.c
* @brief          : Rear Control Unit Power Distribution Functions file
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
#include "Battery_Pack_Control_Unit.h"
#ifdef BATTERY_PACK_CONTROL_UNIT

/*******************************************************************************
********************************************************************************
***************									Init MCU Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_MCU_Init(void)
{
	//General MCU Initiliatation
	MCU_Init();
	LTC6811_SPI_Init();
	

}
/*******************************************************************************
********************************************************************************
***************									Init Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_Init_Temps(Control_Unit_TypeDef* Control_Unit)
{
	for(unsigned char i=0; i<24; i++)
	{
		Control_Unit->Status.Temperatures[i].Actual_Value=(uint8_t)(127+25);
		Control_Unit->Status.Temperatures[i].Last_Value=(uint8_t)(127+25);
	}
}


/*******************************************************************************
********************************************************************************
***************									Init Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_Init(Control_Unit_TypeDef* Control_Unit)
{
	State_LEDs_Init(Control_Unit);
	Battery_Pack_Control_Unit_Init_Temps(Control_Unit);
	Timer_10ms_Init(&Control_Unit->Timing.Status_Send_Timer,1,MILISECONDS,100);
	
	Timer_10ms_Init(&Control_Unit->Timing.Init_State_Timer,1,MILISECONDS,1000);
	Timer_10ms_Start(&Control_Unit->Timing.Init_State_Timer);
	
	Timer_10ms_Init(&Control_Unit->Timing.Temp_Send_Timer,1,MILISECONDS,50);
	Timer_10ms_Init(&Control_Unit->Timing.Temp_Timeout_Timer,1,MILISECONDS,1000);
	
	LTC6811_Init(Control_Unit);

}

/*******************************************************************************
********************************************************************************
***************					Generate Status Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Status_Message(Control_Unit_TypeDef* Control_Unit)
{
		Control_Unit->Tx_Message.ID=BPCU_STATUS;
		Control_Unit->Tx_Message.DLC=3;
		Control_Unit->Tx_Message.Data[0]=Control_Unit->State;
		Control_Unit->Tx_Message.Data[1]=Control_Unit->Status.Temperatures_Out_of_Range;
		Control_Unit->Tx_Message.Data[2]=Control_Unit->Status.Temperatures_Failed;
}

/*******************************************************************************
********************************************************************************
***************					Generate Temp Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Temp_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.DLC=8;
	Control_Unit->Tx_Message.ID=BPCU_TEMP_DEF+0;
	Control_Unit->Tx_Message.Data[0]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[0+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[1]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[1+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[2]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[2+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[3]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[3+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[4]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[4+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[5]=LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[5+Control_Unit->Status.Temperature_Message*6].Actual_Value);
	Control_Unit->Tx_Message.Data[6]=Control_Unit->Status.Temperatures_Out_of_Range;
	Control_Unit->Tx_Message.Data[7]=Control_Unit->Status.Temperatures_Failed;	
}



/*******************************************************************************
********************************************************************************
***************						 		  WATCHDOG TASK       			   		 ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_WDT_Task(void)
{
	MCU_WDT_Refresh();
}



/*******************************************************************************
********************************************************************************
***************						 		  STATE MACHINE TASK       			   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_State_Machine_Task(Control_Unit_TypeDef* Control_Unit)
{
	switch(Control_Unit->State)
	{
		case INIT:
			Green_LED_Permanent_On(Control_Unit);
			Yellow_LED_Blink(Control_Unit,200);
		break;
		
		case NORMAL_OPERATION:
			Green_LED_Permanent_On(Control_Unit);
		break;
		
		case FAIL_MODE:
			Green_LED_Permanent_Off(Control_Unit);
			Yellow_LED_Blink(Control_Unit,300);
		break;

	}
}



/*******************************************************************************
********************************************************************************
***************				Interrupt Task for CAN STATUS SEND        ***************	
********************************************************************************
*******************************************************************************/
void CAN_Status_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Status_Send_Timer.Overflowed==TRUE)
	{
		Generate_Status_Message(Control_Unit);
		Timer_10ms_Restart(&Control_Unit->Timing.Status_Send_Timer);
		CAN1_Send(&Control_Unit->Tx_Message);	
	}
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for CAN Temp SEND        ***************	
********************************************************************************
*******************************************************************************/
void CAN_Temp_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Temp_Send_Timer.Overflowed==TRUE)
	{
		Generate_Temp_Message(Control_Unit);
		CAN1_Send(&Control_Unit->Tx_Message);	
		Control_Unit->Status.Temperature_Message++;
		if(Control_Unit->Status.Temperature_Message>3)
		{
			Control_Unit->Status.Temperature_Message=0;
			Timer_10ms_Stop(&Control_Unit->Timing.Temp_Send_Timer);
			Timer_10ms_Start(&Control_Unit->Timing.Temp_Timeout_Timer);
			Yellow_LED_Permanent_Off(Control_Unit);
		}
		else
		{
			Timer_10ms_Restart(&Control_Unit->Timing.Temp_Send_Timer);
		}
	}
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Initialitation timer    ***************	
********************************************************************************
*******************************************************************************/
void CAN_Temp_Timeout_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Temp_Timeout_Timer.Overflowed==TRUE)
	{
		Timer_10ms_Stop(&Control_Unit->Timing.Temp_Timeout_Timer);
		Timer_10ms_Start(&Control_Unit->Timing.Temp_Send_Timer);
		Yellow_LED_Permanent_On(Control_Unit);
	
	}
}

/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Initialitation timer    ***************	
********************************************************************************
*******************************************************************************/
void Initialitation_Tim_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Init_State_Timer.Overflowed==TRUE)
	{
		Timer_10ms_Stop(&Control_Unit->Timing.Init_State_Timer);
		Timer_10ms_Start(&Control_Unit->Timing.Status_Send_Timer);
		Timer_10ms_Start(&Control_Unit->Timing.Temp_Send_Timer);
		Yellow_LED_Permanent_On(Control_Unit);
		Control_Unit->State=NORMAL_OPERATION;
	}
}




/*******************************************************************************
********************************************************************************
***************									 Interrupt Task        			 	   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	Initialitation_Tim_Interrupt_Task(Control_Unit);
	CAN_Status_Send_Interrupt_Task(Control_Unit);
	CAN_Temp_Send_Interrupt_Task(Control_Unit);
	CAN_Temp_Timeout_Interrupt_Task(Control_Unit);
	State_LEDs_Interrupt_Task(Control_Unit);
}



/*******************************************************************************
********************************************************************************
***************							 Timer 10ms Interrupt        		     ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_10ms_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	Timer_10ms_Tick(&Control_Unit->Timing.Status_Send_Timer);
	Timer_10ms_Tick(&Control_Unit->Timing.Init_State_Timer);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp_Send_Timer);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp_Timeout_Timer);
	State_LEDs_10ms_Tick(Control_Unit);
}



/*******************************************************************************
********************************************************************************
***************										 Main Task        		    		 ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_Main_Task(Control_Unit_TypeDef* Control_Unit)
{
	State_LEDs_Task(&Control_Unit->Green_Led);
	State_LEDs_Task(&Control_Unit->Yellow_Led);
	Battery_Pack_Control_Unit_WDT_Task();
	Battery_Pack_Control_State_Machine_Task(Control_Unit);
	Battery_Pack_Control_Interrupt_Task(Control_Unit);
}
	
void Battery_Pack_Control_Unit_CAN1_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
}

#endif


	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : Rear_Control_Unit.c
  * @brief          : Rear Control Unit Functions header file
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