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
#include "Accu_Master_Control_Unit.h"

#ifdef ACCU_MASTER_CONTROL_UNIT_BOARD



/*******************************************************************************
********************************************************************************
***************					 Precharge Relay Control Init        		 ***************	
********************************************************************************
*******************************************************************************/
void Precharge_Relay_Control_Init (void)
{
	MCU_GPIO_Init_C5_Output();
}

/*******************************************************************************
********************************************************************************
***************					 Air Plus Relay Control Init        		 ***************	
********************************************************************************
*******************************************************************************/
void Air_Plus_Relay_Control_Init (void)
{
	MCU_GPIO_Init_C6_Output();
}

/*******************************************************************************
********************************************************************************
***************					 Air Minus Relay Control Init        		 ***************	
********************************************************************************
*******************************************************************************/
void Air_Minus_Relay_Control_Init (void)
{
	MCU_GPIO_Init_C7_Output();
}

/*******************************************************************************
********************************************************************************
***************					    Fan Control Init        		         ***************	
********************************************************************************
*******************************************************************************/
void Fan_Control_Init (void)
{
	MCU_GPIO_Init_B2_Output();
}

/*******************************************************************************
********************************************************************************
***************					   BMS Status Control Init        		   ***************	
********************************************************************************
*******************************************************************************/
void BMS_Status_Control_Init (void)
{
	MCU_GPIO_Init_B15_Output();
}




/*******************************************************************************
********************************************************************************
***************						Precharge Relay Control On          	 ***************	
********************************************************************************
*******************************************************************************/
void Precharge_Relay_Control_On (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C5_Output(TRUE);
	Control_Unit->Status.Precharge_Relay.Intentional_State = STATE_ON;
}

/*******************************************************************************
********************************************************************************
***************						Air Plus Relay Control On          	 ***************	
********************************************************************************
*******************************************************************************/
void Air_Plus_Relay_Control_On (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C6_Output(TRUE);
	Control_Unit->Status.Air_Plus_Relay.Intentional_State = STATE_ON;
}

/*******************************************************************************
********************************************************************************
***************						Air Minus Relay Control On          	 ***************	
********************************************************************************
*******************************************************************************/
void Air_Minus_Relay_Control_On (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C7_Output(TRUE);
	Control_Unit->Status.Air_Minus_Relay.Intentional_State = STATE_ON;
}

/*******************************************************************************
********************************************************************************
***************					       	Fan Control On          	       ***************	
********************************************************************************
*******************************************************************************/
void Fan_Control_On (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_B2_Output(TRUE);
}

/*******************************************************************************
********************************************************************************
***************					   	BMS Status Control On          	     ***************	
********************************************************************************
*******************************************************************************/
void BMS_Status_Control_On (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_B15_Output(TRUE);
}




/*******************************************************************************
********************************************************************************
***************						Precharge Relay Control Off          	 ***************	
********************************************************************************
*******************************************************************************/
void Precharge_Relay_Control_Off (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C5_Output(FALSE);
	Control_Unit->Status.Precharge_Relay.Intentional_State = STATE_OFF;
}

/*******************************************************************************
********************************************************************************
***************						Air Plus Relay Control Off          	 ***************	
********************************************************************************
*******************************************************************************/
void Air_Plus_Relay_Control_Off (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C6_Output(FALSE);
	Control_Unit->Status.Air_Plus_Relay.Intentional_State = STATE_OFF;
}

/*******************************************************************************
********************************************************************************
***************						Air Minus Relay Control Off          	 ***************	
********************************************************************************
*******************************************************************************/
void Air_Minus_Relay_Control_Off (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_C7_Output(FALSE);
	Control_Unit->Status.Air_Minus_Relay.Intentional_State = STATE_OFF;
}

/*******************************************************************************
********************************************************************************
***************					       	Fan Control Off          	       ***************	
********************************************************************************
*******************************************************************************/
void Fan_Control_Off (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_B2_Output(FALSE);
}

/*******************************************************************************
********************************************************************************
***************					   	BMS Status Control Off          	   ***************	
********************************************************************************
*******************************************************************************/
void BMS_Status_Control_Off (Control_Unit_TypeDef* Control_Unit)
{
	MCU_GPIO_Write_B15_Output(FALSE);
}




/*******************************************************************************
********************************************************************************
***************									BMS Status Init        			     ***************	
********************************************************************************
*******************************************************************************/
void BMS_Status_Init (void)
{
	MCU_GPIO_Init_A9_Input();
}

/*******************************************************************************
********************************************************************************
***************									SDC Feedback Init        			   ***************	
********************************************************************************
*******************************************************************************/
void SDC_Feedback_Init (void)
{
	MCU_GPIO_Init_C3_Input();
}




/*******************************************************************************
********************************************************************************
***************									BMS Status Read        			     ***************	
********************************************************************************
*******************************************************************************/
void BMS_Status_Check (Control_Unit_TypeDef* Control_Unit)
{
	//RELAYS CHECK
	BoolTypeDef Relay_Fail=FALSE;
	
	if (Control_Unit->Status.Precharge_Relay.Real_State == STATE_FAIL || Control_Unit->Status.Air_Plus_Relay.Real_State == STATE_FAIL
		|| Control_Unit->Status.Air_Minus_Relay.Real_State == STATE_FAIL)
	{
		Relay_Fail=TRUE;
	}
	
	
	//BPCU CHECK
	if(Control_Unit->Status.BPCU1_Fails >0 || Control_Unit->Status.BPCU2_Fails >0 || Control_Unit->Status.BPCU3_Fails >0 || Control_Unit->Status.BPCU4_Fails >0)
	{
		Control_Unit->Status.Temp_Fail=TRUE;
	}
	else
	{
		Control_Unit->Status.Temp_Fail=FALSE;
	}
	if( Control_Unit->Status.BPCU1_Status>0x01 || Control_Unit->Status.BPCU2_Status>0x01 || Control_Unit->Status.BPCU3_Status>0x01 || Control_Unit->Status.BPCU4_Status>0x01)
	{
		Control_Unit->Status.BPCU_Fail=TRUE;
	}
	else
	{
		Control_Unit->Status.BPCU_Fail=FALSE;
	}
	
	
	
	//Si ha fallado algo cortamos la salida.
	if (MCU_GPIO_Read_A9_Input()==TRUE  && Control_Unit->Status.Temp_Timeout==FALSE && Control_Unit->Status.BPCU_Fail==FALSE &&
	(Relay_Fail==FALSE || (Relay_Fail==TRUE && SDC_Feedback_Read(Control_Unit)==FALSE)) &&  Control_Unit->Status.Temp_Fail==FALSE) 
	{
		Control_Unit->Status.BMS_Status = FALSE;
		BMS_Status_Control_On(Control_Unit);
	}
	else
	{
		Control_Unit->Status.BMS_Status = TRUE;
		BMS_Status_Control_Off(Control_Unit);
		
		//Control_Unit->State = SDC_BREAK;
		//Control_Unit->Status.SDC_Status = FALSE;
	}
	
	

}


/*******************************************************************************
********************************************************************************
***************									SDC Feedback Read        			   ***************	
********************************************************************************
*******************************************************************************/
BoolTypeDef SDC_Feedback_Read (Control_Unit_TypeDef* Control_Unit)
{
/*
	if (MCU_GPIO_Read_C3_Input()==TRUE)
	{
		Control_Unit->State = NORMAL_OPERATION;
		Control_Unit->Status.SDC_Status = TRUE;
	}
	else
	{
		Control_Unit->State = SDC_BREAK;
		Control_Unit->Status.SDC_Status = FALSE;
	}
*/
	return MCU_GPIO_Read_C3_Input();
}




/*******************************************************************************
********************************************************************************
***************								Precharge Relay ADC Init      	 	 ***************	
********************************************************************************
*******************************************************************************/
void Precharge_Relay_ADC_Init (void)
{
	MCU_ADC1_Channel_Init(ADC_CHANNEL_10, 1); 
}

/*******************************************************************************
********************************************************************************
***************								Air Plus Relay ADC Init      	 	   ***************	
********************************************************************************
*******************************************************************************/
void Air_Plus_Relay_ADC_Init (void)
{
	MCU_ADC1_Channel_Init(ADC_CHANNEL_11, 2); 
}

/*******************************************************************************
********************************************************************************
***************								Air Minus Relay ADC Init      	 	 ***************	
********************************************************************************
*******************************************************************************/
void Air_Minus_Relay_ADC_Init (void)
{
	MCU_ADC1_Channel_Init(ADC_CHANNEL_12, 3); 
}

/*******************************************************************************
********************************************************************************
***************								HV Feedback ADC Init      	 	     ***************	
********************************************************************************
*******************************************************************************/
void HV_Feedback_ADC_Init (void)
{
	MCU_ADC1_Channel_Init(ADC_CHANNEL_0, 4); 
}



/*******************************************************************************
********************************************************************************
***************									Init Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_MCU_Init(void)
{
	MCU_Init();
	
	Precharge_Relay_Control_Init();
	Air_Plus_Relay_Control_Init();
	Air_Minus_Relay_Control_Init();
	Fan_Control_Init();
	BMS_Status_Control_Init();

	BMS_Status_Init();
	SDC_Feedback_Init();
	
	MCU_DMA_Init();
	MCU_ADC1_Init(4);
	
	Precharge_Relay_ADC_Init();
	Air_Plus_Relay_ADC_Init();
	Air_Minus_Relay_ADC_Init();
	HV_Feedback_ADC_Init();
}

/*******************************************************************************
********************************************************************************
***************									Task Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_Task_Init(Control_Unit_TypeDef* Control_Unit)
{
	MCU_ADC_Start(&Control_Unit->Status.Relays_Value[0], 4);
	
	//Inicializacion del TIMER de Status Send
	Timer_10ms_Init(&Control_Unit->Timing.Status_Send_Tim, 1, MILISECONDS, 400);
	Timer_10ms_Start(&Control_Unit->Timing.Status_Send_Tim);
	
	//Inicializacion del TIMER de Precarge Status
	Timer_10ms_Init(&Control_Unit->Timing.Precharge_Status_Tim, 1, MILISECONDS, 100);
	Timer_10ms_Start(&Control_Unit->Timing.Precharge_Status_Tim);
	
	//Inicializacion del TIMER de Precarga
	Timer_10ms_Init(&Control_Unit->Timing.Precharge_Tim, 1, MILISECONDS, 5000);
	
	//Inicializacion del TIMER de lectura ADCs
	Timer_10ms_Init(&Control_Unit->Timing.ADCs_Reading_Tim, 1, MILISECONDS, 500);
	Timer_10ms_Start(&Control_Unit->Timing.ADCs_Reading_Tim);
	
	//Inicializacion del TIMER de temps timeout
	Timer_10ms_Init(&Control_Unit->Timing.Temp1_Timeout, 1, MILISECONDS, 1000);
	Timer_10ms_Start(&Control_Unit->Timing.Temp1_Timeout);
	Timer_10ms_Init(&Control_Unit->Timing.Temp2_Timeout, 1, MILISECONDS, 1000);
	Timer_10ms_Start(&Control_Unit->Timing.Temp2_Timeout);
	Timer_10ms_Init(&Control_Unit->Timing.Temp3_Timeout, 1, MILISECONDS, 1000);
	Timer_10ms_Start(&Control_Unit->Timing.Temp3_Timeout);
	Timer_10ms_Init(&Control_Unit->Timing.Temp4_Timeout, 1, MILISECONDS, 1000);
	Timer_10ms_Start(&Control_Unit->Timing.Temp4_Timeout);
	
	Timer_10ms_Init(&Control_Unit->Timing.Temp_Measure, 1, MILISECONDS, 3000);
	Timer_10ms_Start(&Control_Unit->Timing.Temp_Measure);
	
	Control_Unit->State = NORMAL_OPERATION;
	Control_Unit->Status.RTD_Send = FALSE;
	Control_Unit->Status.BPCU1_Status=0;
	Control_Unit->Status.BPCU2_Status=0;
	Control_Unit->Status.BPCU3_Status=0;
	Control_Unit->Status.BPCU4_Status=0;
	
	Control_Unit->Status.Temp_Fail=FALSE;
	Control_Unit->Status.Temp_Timeout=FALSE;
	Control_Unit->Status.BPCU_Fail=FALSE;
	
	Control_Unit->Status.BPCU1_Fails=0;
	Control_Unit->Status.BPCU2_Fails=0;
	Control_Unit->Status.BPCU3_Fails=0;
	Control_Unit->Status.BPCU4_Fails=0;
	
}

/*******************************************************************************
********************************************************************************
***************						 		  WATCHDOG TASK       			   		 ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_WDT_Task(void)
{
	MCU_WDT_Refresh();
}


/*******************************************************************************
********************************************************************************
***************						 		  INTERRUPT TASK       			       ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	// INTERRUPCIONES DE LAS TASKS CREADAS QUE REQUIEREN CIERTO TIEMPO
	CAN_Status_Send_Interrupt_Task(Control_Unit);
	Precharge_CAN_Received_Interrupt_Task(Control_Unit);
	Precharge_Status_Send_Interrupt_Task(Control_Unit);
	ADCs_Reading_Interrupt_Task(Control_Unit);
	Temp_Timeout_Interrupt_Task(Control_Unit);
	Temp_Measure_Send_Interrupt_Task(Control_Unit);
}


/*******************************************************************************
********************************************************************************
***************						 		  STATE MACHINE TASK       			   ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_State_Machine_Task(Control_Unit_TypeDef* Control_Unit)
{
	//SDC_Feedback_Read(Control_Unit);
	BMS_Status_Check(Control_Unit);
	switch(Control_Unit->State)
	{
		case NORMAL_OPERATION:
			if (Control_Unit->Status.SDC_Status==TRUE)
			{
				Control_Unit->Status.RTD_Send=FALSE;
			}
		break;
		
		case SDC_BREAK:
			if (Control_Unit->Status.RTD_Send==FALSE)
			{
				Control_Unit->Status.RTD_Send=TRUE;
				Timer_10ms_Stop(&Control_Unit->Timing.Precharge_Tim);
				Timer_10ms_Reset(&Control_Unit->Timing.Precharge_Tim);
				Timer_10ms_Restart(&Control_Unit->Timing.ADCs_Reading_Tim);
				Precharge_Relay_Control_Off(Control_Unit);
				Air_Plus_Relay_Control_Off(Control_Unit);
				Air_Minus_Relay_Control_Off(Control_Unit);
				Control_Unit->Status.Precharge_Status = FALSE;
				Generate_Ready_To_Drive_Message(Control_Unit);
				CAN2_Send(&Control_Unit->Tx_Message);
				Control_Unit->State = NORMAL_OPERATION;
				Fan_Control_Off(Control_Unit);
			}
		break;
		
		default:
		break;
	}
		if (SDC_Feedback_Read(Control_Unit)==TRUE)
		{
			Control_Unit->State = NORMAL_OPERATION;
			Control_Unit->Status.SDC_Status = TRUE;
		}
}


/*******************************************************************************
********************************************************************************
***************									 CAN BUS 1 Interrupt      		   ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_CAN1_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
//*
	switch(Control_Unit->Rx_Message.Header.ExtId)
	{
		case BPCU1_STATUS:
			Proccess_BPCU1_Status_Frame(Control_Unit);
		break;
		
		case BPCU2_STATUS:
			Proccess_BPCU2_Status_Frame(Control_Unit);
		break;
		
		case BPCU3_STATUS:
			Proccess_BPCU3_Status_Frame(Control_Unit);
		break;
		
		case BPCU4_STATUS:
			Proccess_BPCU4_Status_Frame(Control_Unit);
		break;
		
		default:
		break;
	}
//*/
}

/*******************************************************************************
********************************************************************************
***************									 CAN BUS 2 Interrupt      		     *************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_CAN2_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	switch(Control_Unit->Rx_Message.Header.StdId)
	{
		case START_PRECHARGE:
			Proccess_Start_Precharge_Frame(Control_Unit);
		break;
		
		case RTD:
			Proccess_Fan_Control_Frame(Control_Unit);
		break;
		
		default:
		break;
	}
}


/*******************************************************************************
********************************************************************************
***************							 Timer 10ms Interrupt        		     ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_10ms_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	Timer_10ms_Tick(&Control_Unit->Timing.Status_Send_Tim);
	Timer_10ms_Tick(&Control_Unit->Timing.Precharge_Tim);
	Timer_10ms_Tick(&Control_Unit->Timing.Precharge_Status_Tim);
	Timer_10ms_Tick(&Control_Unit->Timing.ADCs_Reading_Tim);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp1_Timeout);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp2_Timeout);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp3_Timeout);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp4_Timeout);
	Timer_10ms_Tick(&Control_Unit->Timing.Temp_Measure);
}

	
/*******************************************************************************
********************************************************************************
***************									 Main_Task        			 		     ***************	
********************************************************************************
*******************************************************************************/
void Accu_Master_Control_Unit_Main_Task(Control_Unit_TypeDef* Control_Unit)
{
	Accu_Master_Control_Unit_WDT_Task();
	Accu_Master_Control_Unit_Interrupt_Task(Control_Unit);
	Accu_Master_Control_Unit_State_Machine_Task(Control_Unit);
}






/*******************************************************************************
********************************************************************************
***************				Interrupt Task for CAN STATUS SEND        ***************	
********************************************************************************
*******************************************************************************/
void CAN_Status_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Status_Send_Tim.Overflowed==TRUE)
	{
		Generate_BMS_Master_Status_Message(Control_Unit);
		Timer_10ms_Restart(&Control_Unit->Timing.Status_Send_Tim);
		CAN2_Send(&Control_Unit->Tx_Message);

	}
}

/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Temp Measure SEND        ***************	
********************************************************************************
*******************************************************************************/
void Temp_Measure_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Temp_Measure.Overflowed==TRUE)
	{
		Generate_Temp_Measure_Message(Control_Unit);
		Timer_10ms_Change_Timing(&Control_Unit->Timing.Temp_Measure,1000);
		Timer_10ms_Restart(&Control_Unit->Timing.Temp_Measure);
		CAN1_Send(&Control_Unit->Tx_Message);

	}
}

/*******************************************************************************
********************************************************************************
***************				Interrupt Task for PRECHARGE RCV        		***************	
********************************************************************************
*******************************************************************************/
void Precharge_CAN_Received_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Precharge_Tim.Overflowed==TRUE)
	{
		Timer_10ms_Stop(&Control_Unit->Timing.Precharge_Tim);
		Timer_10ms_Reset(&Control_Unit->Timing.Precharge_Tim);
		Timer_10ms_Restart(&Control_Unit->Timing.ADCs_Reading_Tim);
		Control_Unit->Status.Precharge_Status = TRUE;
		Air_Plus_Relay_Control_On(Control_Unit);
		Precharge_Relay_Control_Off(Control_Unit);
		Generate_Done_Precharge_Message(Control_Unit);
		CAN2_Send(&Control_Unit->Tx_Message);
	}
}


/*******************************************************************************
********************************************************************************
***************			Interrupt Task for PRECHARGE STATUS SEND     ***************	
********************************************************************************
*******************************************************************************/
void Precharge_Status_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Precharge_Status_Tim.Overflowed==TRUE)
	{
		Generate_Precarge_Status_Message(Control_Unit);
		Timer_10ms_Restart(&Control_Unit->Timing.Precharge_Status_Tim);
		CAN2_Send(&Control_Unit->Tx_Message);
	}
}


/*******************************************************************************
********************************************************************************
***************			Interrupt Task for ADCs READING					     ***************	
********************************************************************************
*******************************************************************************/
void ADCs_Reading_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.ADCs_Reading_Tim.Overflowed==TRUE)
	{
		Timer_10ms_Restart(&Control_Unit->Timing.ADCs_Reading_Tim);
		Proccess_ADCs_Reading(Control_Unit);
	}
}


/*******************************************************************************
********************************************************************************
***************			Interrupt Task for ADCs READING					     ***************	
********************************************************************************
*******************************************************************************/
void Temp_Timeout_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Timing.Temp1_Timeout.Overflowed==TRUE || Control_Unit->Timing.Temp2_Timeout.Overflowed==TRUE || Control_Unit->Timing.Temp3_Timeout.Overflowed==TRUE || Control_Unit->Timing.Temp4_Timeout.Overflowed==TRUE)
	{
		Timer_10ms_Stop(&Control_Unit->Timing.Temp1_Timeout);
		Timer_10ms_Stop(&Control_Unit->Timing.Temp2_Timeout);
		Timer_10ms_Stop(&Control_Unit->Timing.Temp3_Timeout);
		Timer_10ms_Stop(&Control_Unit->Timing.Temp4_Timeout);
		Control_Unit->Status.Temp_Timeout=TRUE;
	}
	else
	{
		Control_Unit->Status.Temp_Timeout=FALSE;
	}
}


/*******************************************************************************
********************************************************************************
***************					Generate Status Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_BMS_Master_Status_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.ID=BMS_MASTER_STATUS;
	Control_Unit->Tx_Message.DLC=4;
	
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Status.Air_Plus_Relay.Intentional_State;
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Tx_Message.Data[0] | (Control_Unit->Status.Air_Plus_Relay.Real_State << 1);
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Tx_Message.Data[0] | (Control_Unit->Status.Air_Minus_Relay.Intentional_State << 3);
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Tx_Message.Data[0] | (Control_Unit->Status.Air_Minus_Relay.Real_State << 4);
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Tx_Message.Data[0] | (Control_Unit->Status.SDC_Status << 6);
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Tx_Message.Data[0] | (Control_Unit->Status.BMS_Status << 7);
	
	Control_Unit->Tx_Message.Data[1]=Control_Unit->Status.Precharge_Relay.Intentional_State;
	Control_Unit->Tx_Message.Data[1]=Control_Unit->Tx_Message.Data[1] | (Control_Unit->Status.Precharge_Relay.Real_State << 1);
	Control_Unit->Tx_Message.Data[1]=Control_Unit->Tx_Message.Data[1] | (Control_Unit->Status.HV_Feedback.Intentional_State << 3);
	Control_Unit->Tx_Message.Data[1]=Control_Unit->Tx_Message.Data[1] | (Control_Unit->Status.HV_Feedback.Real_State << 4);
	
	Control_Unit->Tx_Message.Data[2]=Control_Unit->Min_Temperature_Value;
	
	Control_Unit->Tx_Message.Data[3]=Control_Unit->Max_Temperature_Value;
	
	Control_Unit->Tx_Message.Data[4]=Control_Unit->Min_Voltage_Value;
	
	Control_Unit->Tx_Message.Data[5]=Control_Unit->Max_Voltage_Value;
}


/*******************************************************************************
********************************************************************************
***************					Generate Status Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Temp_Measure_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.ID=BMS_MASTER_STATUS;
	Control_Unit->Tx_Message.DLC=1;
	
	Control_Unit->Tx_Message.Data[0]=0x01;	
}

/*******************************************************************************
********************************************************************************
***************					Generate Precharge Status Message        ***************	
********************************************************************************
*******************************************************************************/
void Generate_Precarge_Status_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.ID=PRECHARGUE_STATUS;
	Control_Unit->Tx_Message.DLC=1;
	Control_Unit->Tx_Message.Data[0]=Control_Unit->Status.Precharge_Status;
}


/*******************************************************************************
********************************************************************************
***************					Generate Precharge Status Message        ***************	
********************************************************************************
*******************************************************************************/
void Generate_Done_Precharge_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.ID=DONE_PRECHARGE;
	Control_Unit->Tx_Message.DLC=1;
	Control_Unit->Tx_Message.Data[0]=0x01;
}


/*******************************************************************************
********************************************************************************
***************					Generate Precharge Status Message        ***************	
********************************************************************************
*******************************************************************************/
void Generate_Ready_To_Drive_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.ID=RTD;
	Control_Unit->Tx_Message.DLC=1;
	Control_Unit->Tx_Message.Data[0]=0x00;
}


/*******************************************************************************
********************************************************************************
***************				 Proccess Start Precharge Frame            ***************	
********************************************************************************
*******************************************************************************/
void Proccess_Start_Precharge_Frame(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Rx_Message.Data[0]==0x01 && Control_Unit->Rx_Message.Header.DLC==1 && Control_Unit->Status.Precharge_Status==FALSE && Control_Unit->State==NORMAL_OPERATION)
	{
		Precharge_Relay_Control_On(Control_Unit);
		Air_Minus_Relay_Control_On(Control_Unit);
		Timer_10ms_Start(&Control_Unit->Timing.Precharge_Tim);
		Timer_10ms_Restart(&Control_Unit->Timing.ADCs_Reading_Tim);
	}
}


/*******************************************************************************
********************************************************************************
***************				 Proccess BPCU 1 Status Frame              ***************	
********************************************************************************
*******************************************************************************/
void Proccess_BPCU1_Status_Frame(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Status.BPCU1_Status=Control_Unit->Rx_Message.Data[0];
	Control_Unit->Status.BPCU1_Fails=Control_Unit->Rx_Message.Data[1]+ Control_Unit->Rx_Message.Data[2];
	Timer_10ms_Restart(&Control_Unit->Timing.Temp1_Timeout);
}

/*******************************************************************************
********************************************************************************
***************				 Proccess BPCU 2 Status Frame              ***************	
********************************************************************************
*******************************************************************************/
void Proccess_BPCU2_Status_Frame(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Status.BPCU2_Status=Control_Unit->Rx_Message.Data[0];
	Control_Unit->Status.BPCU2_Fails=Control_Unit->Rx_Message.Data[1]+ Control_Unit->Rx_Message.Data[2];
	Timer_10ms_Restart(&Control_Unit->Timing.Temp2_Timeout);
}

/*******************************************************************************
********************************************************************************
***************				 Proccess BPCU 3 Status Frame              ***************	
********************************************************************************
*******************************************************************************/
void Proccess_BPCU3_Status_Frame(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Status.BPCU3_Status=Control_Unit->Rx_Message.Data[0];
	Control_Unit->Status.BPCU3_Fails=Control_Unit->Rx_Message.Data[1]+ Control_Unit->Rx_Message.Data[2];
	Timer_10ms_Restart(&Control_Unit->Timing.Temp3_Timeout);
}

/*******************************************************************************
********************************************************************************
***************				 Proccess BPCU 4 Status Frame              ***************	
********************************************************************************
*******************************************************************************/
void Proccess_BPCU4_Status_Frame(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Status.BPCU4_Status=Control_Unit->Rx_Message.Data[0];
	Control_Unit->Status.BPCU4_Fails=Control_Unit->Rx_Message.Data[1]+ Control_Unit->Rx_Message.Data[2];
	Timer_10ms_Restart(&Control_Unit->Timing.Temp4_Timeout);
}


/*******************************************************************************
********************************************************************************
***************				 Proccess Fan Control Frame            		 ***************	
********************************************************************************
*******************************************************************************/
void Proccess_Fan_Control_Frame(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Rx_Message.Data[0]==0x01 && Control_Unit->Rx_Message.Header.DLC==1)
	{
		Fan_Control_On(Control_Unit);
	}
}


/*******************************************************************************
********************************************************************************
***************				 			Proccess ADCs Reading            		 ***************	
********************************************************************************
*******************************************************************************/
void Proccess_ADCs_Reading(Control_Unit_TypeDef* Control_Unit)
{
	uint16_t Precharge_Relay_Real_State_Value, Air_Plus_Relay_Real_State_Value, Air_Minus_Relay_Real_State_Value, HV_Feedback_Real_State_Value;
	Precharge_Relay_Real_State_Value = Control_Unit->Status.Relays_Value[0] & 0xFFFF;
	Air_Plus_Relay_Real_State_Value = (Control_Unit->Status.Relays_Value[0] & 0xFFFF0000) >> 16;
	Air_Minus_Relay_Real_State_Value = Control_Unit->Status.Relays_Value[1] & 0xFFFF;
	HV_Feedback_Real_State_Value = (Control_Unit->Status.Relays_Value[1] & 0xFFFF0000) >> 16;
	
	
	/***********************************************************
							TRATAMIENTO DE FALLO PRECHARGE RELAY
	************************************************************/
	if (Precharge_Relay_Real_State_Value <= ADC_READ_AT_OFF)
	{
		Control_Unit->Status.Precharge_Relay.Real_State = STATE_FAIL;
	}
	else if (Precharge_Relay_Real_State_Value > ADC_READ_AT_OFF && Precharge_Relay_Real_State_Value <= ADC_READ_AT_ON)
	{
		if (Control_Unit->Status.Precharge_Relay.Intentional_State == STATE_ON)
		{
			Control_Unit->Status.Precharge_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Precharge_Relay.Real_State = STATE_OFF;
		}
	}
	else 
	{
		if (Control_Unit->Status.Precharge_Relay.Intentional_State == STATE_OFF)
		{
			Control_Unit->Status.Precharge_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Precharge_Relay.Real_State = STATE_ON;
		}
	}
	
	
	/***********************************************************
							TRATAMIENTO DE FALLO AIR + RELAY
	************************************************************/
	if (Air_Plus_Relay_Real_State_Value <= ADC_READ_AT_OFF)
	{
		Control_Unit->Status.Air_Plus_Relay.Real_State = STATE_FAIL;
	}
	else if (Air_Plus_Relay_Real_State_Value > ADC_READ_AT_OFF && Air_Plus_Relay_Real_State_Value <= ADC_READ_AT_ON)
	{
		if (Control_Unit->Status.Air_Plus_Relay.Intentional_State == STATE_ON)
		{
			Control_Unit->Status.Air_Plus_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Air_Plus_Relay.Real_State = STATE_OFF;
		}
	}
	else 
	{
		if (Control_Unit->Status.Air_Plus_Relay.Intentional_State == STATE_OFF)
		{
			Control_Unit->Status.Air_Plus_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Air_Plus_Relay.Real_State = STATE_ON;
		}
	}
	
	
	/***********************************************************
							TRATAMIENTO DE FALLO AIR - RELAY
	************************************************************/
	if (Air_Minus_Relay_Real_State_Value <= ADC_READ_AT_OFF)
	{
		Control_Unit->Status.Air_Minus_Relay.Real_State = STATE_FAIL;
	}
	else if (Air_Minus_Relay_Real_State_Value > ADC_READ_AT_OFF && Air_Minus_Relay_Real_State_Value <= ADC_READ_AT_ON)
	{
		if (Control_Unit->Status.Air_Minus_Relay.Intentional_State == STATE_ON)
		{
			Control_Unit->Status.Air_Minus_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Air_Minus_Relay.Real_State = STATE_OFF;
		}
	}
	else 
	{
		if (Control_Unit->Status.Air_Minus_Relay.Intentional_State == STATE_OFF)
		{
			Control_Unit->Status.Air_Minus_Relay.Real_State = STATE_FAIL;
		}
		else
		{
			Control_Unit->Status.Air_Minus_Relay.Real_State = STATE_ON;
		}
	}
	
	
	/***********************************************************
							TRATAMIENTO DE FALLO HV FEEDBACK
	************************************************************/
	if (HV_Feedback_Real_State_Value <= ADC_READ_AT_OFF)
	{
		Control_Unit->Status.HV_Feedback.Real_State = STATE_FAIL;
	}
	else if (HV_Feedback_Real_State_Value > ADC_READ_AT_OFF && HV_Feedback_Real_State_Value <= ADC_READ_AT_ON)
	{
		Control_Unit->Status.HV_Feedback.Real_State = STATE_OFF;
	}
	else 
	{
		Control_Unit->Status.HV_Feedback.Real_State = STATE_ON;
	}
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