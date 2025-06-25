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
void Battery_Pack_Control_Unit_Init_Values(Control_Unit_TypeDef* Control_Unit)
{
	for(unsigned char i=0; i<24; i++)
	{
		Control_Unit->Status.Temperatures[i].Actual_Value=25.0f;
		Control_Unit->Status.Temperatures[i].Readed_Value=25.0f;
		Control_Unit->Status.Voltages[i]=3.6f;
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
	Battery_Pack_Control_Unit_Init_Values(Control_Unit);
	Timer_10ms_Init(&Control_Unit->Timing.Status_Send_Timer,1,MILISECONDS,100);
	
	Timer_10ms_Init(&Control_Unit->Timing.Init_State_Timer,1,MILISECONDS,1000);
	Timer_10ms_Start(&Control_Unit->Timing.Init_State_Timer);
	
	Timer_10ms_Init(&Control_Unit->Timing.Temp_Send_Timer,1,MILISECONDS,200);
	
	LTC6811_Init(Control_Unit);

}

/*******************************************************************************
********************************************************************************
***************					Generate Status Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Status_Message(Control_Unit_TypeDef* Control_Unit)
{
		Control_Unit->Tx_Message.ID=BPCU_STATUS_DEF;
		Control_Unit->Tx_Message.DLC=3;
		Control_Unit->Tx_Message.Data[0]=Control_Unit->State;
		Control_Unit->Tx_Message.Data[1]=Control_Unit->Status.Temperatures_Hot;
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
	Control_Unit->Tx_Message.ID=BPCU_TEMP_1_DEF+Control_Unit->Status.CAN_Message;
	uint8_t base = Control_Unit->Status.CAN_Message * 6;
	if (base + 5 >= 24) return;  // Protege overflow

	for (uint8_t i = 0; i < 6; i++) 
	{
    Control_Unit->Tx_Message.Data[i] = LTC6811_Enconde_Temp(Control_Unit->Status.Temperatures[base + i].Actual_Value);
	}
	Control_Unit->Tx_Message.Data[6]=Control_Unit->Status.Temperatures_Hot;
	Control_Unit->Tx_Message.Data[7]=Control_Unit->Status.Temperatures_Failed;	
}

/*******************************************************************************
********************************************************************************
***************					Generate Volt Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Volt_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.DLC=8;
	Control_Unit->Tx_Message.ID=BPCU_TEMP_1_DEF+Control_Unit->Status.CAN_Message;
	uint8_t base_index = (Control_Unit->Status.CAN_Message - SEND_VOLT_1) * 8;

	// Protección contra overflow:
	if (base_index + 7 >= 24) {
		return;
	}

	Control_Unit->Tx_Message.DLC = 8;
	Control_Unit->Tx_Message.ID = BPCU_TEMP_1_DEF + Control_Unit->Status.CAN_Message;

	for (uint8_t i = 0; i < 8; i++) 
	{
		Control_Unit->Tx_Message.Data[i] =LTC6811_Encode_Volt_10mV(Control_Unit->Status.Voltages[base_index + i]);
	}
}

/*******************************************************************************
********************************************************************************
***************					Generate Finished Measure       			 	   ***************	
********************************************************************************
*******************************************************************************/
void Generate_Finish_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.DLC=1;
	Control_Unit->Tx_Message.ID=BPCU_FINISHED_MEASURE;
	Control_Unit->Tx_Message.Data[0]=0x01;
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
		
		case LTC6811_FAIL_MODE:
			Green_LED_Blink(Control_Unit,500);
			Yellow_LED_Blink(Control_Unit,500);
		break;
		
		case TEMP_FAIL_MODE:
			Green_LED_Permanent_Off(Control_Unit);
			Yellow_LED_Blink(Control_Unit,500);
		break;
		
		case TEMP_PLUS_60_FAIL_MODE:
			Green_LED_Permanent_Off(Control_Unit);
			Yellow_LED_Blink(Control_Unit,1000);
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
		if(Control_Unit->Status.CAN_Message<=SEND_TEMP_4)
		{
			Generate_Temp_Message(Control_Unit);
		}
		else if(Control_Unit->Status.CAN_Message>SEND_TEMP_4 && Control_Unit->Status.CAN_Message<=SEND_VOLT_3)
		{
			Generate_Volt_Message(Control_Unit);
		}
		CAN1_Send(&Control_Unit->Tx_Message);	
		Control_Unit->Status.CAN_Message++;

		// Si sale del rango permitido, reinicia desde el primer mensaje
		if (Control_Unit->Status.CAN_Message < SEND_TEMP_1 || Control_Unit->Status.CAN_Message > SEND_VOLT_3)
		{
			Control_Unit->Status.CAN_Message = SEND_TEMP_1;
		}

		Timer_10ms_Restart(&Control_Unit->Timing.Temp_Send_Timer);		
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
		Yellow_LED_Permanent_Off(Control_Unit);
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
	State_LEDs_Interrupt_Task(Control_Unit);
}



/*******************************************************************************
********************************************************************************
***************							 Timer 10ms Interrupt        		     ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_10ms_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Status.Read_Temperatures!=READING && Control_Unit->State!=INIT && Control_Unit->State!=LTC6811_FAIL_MODE)
	{
		Timer_10ms_Tick(&Control_Unit->Timing.Status_Send_Timer);
		Timer_10ms_Tick(&Control_Unit->Timing.Init_State_Timer);
		Timer_10ms_Tick(&Control_Unit->Timing.Temp_Send_Timer);
		State_LEDs_10ms_Tick(Control_Unit);
	}
}


/*******************************************************************************
********************************************************************************
***************							 	Check Temperatures        		     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Verifica y actualiza el estado de los sensores de temperatura de la unidad de control.
 *
 * Esta función recorre los 24 sensores de temperatura:
 * - Si la lectura ha cambiado significativamente (>0.5°C), se actualiza el valor actual.
 * - Si hay un salto brusco y la lectura es extrema (>60°C o <–10°C), se suaviza.
 * - Detecta fallos por temperatura fuera de rango repetida.
 * - Si un sensor está deshabilitado, se le asigna 25°C fijo.
 */
void Battery_Pack_Control_Unit_Check_Temperatures (Control_Unit_TypeDef* Control_Unit)
{
	for (uint8_t i = 0; i < 24; i++)
	{
		// Solo actualiza si la lectura cambió más de ±0.5°C desde la última
		if (fabsf(Control_Unit->Status.Temperatures[i].Readed_Value - Control_Unit->Status.Temperatures[i].Actual_Value) > 0.5f)
		{
			// Si el sensor está habilitado
			if (Control_Unit->Status.Temperatures[i].Disabled == FALSE)
			{
				// Si el cambio es brusco (>10°C) y además el valor leído es crítico
				if (fabsf(Control_Unit->Status.Temperatures[i].Readed_Value - Control_Unit->Status.Temperatures[i].Actual_Value) > 5.0f &&
				    (Control_Unit->Status.Temperatures[i].Readed_Value > 60.0f || Control_Unit->Status.Temperatures[i].Readed_Value < 0.0f))
				{
					// Suavizado: 70% valor anterior + 30% valor nuevo
					float alpha = 0.1f;
					Control_Unit->Status.Temperatures[i].Actual_Value =(1.0f - alpha) * Control_Unit->Status.Temperatures[i].Actual_Value + alpha * Control_Unit->Status.Temperatures[i].Readed_Value;
				}
				else
				{
					// Si el cambio es razonable, se toma el valor directamente
					Control_Unit->Status.Temperatures[i].Actual_Value =Control_Unit->Status.Temperatures[i].Readed_Value;
				}

				// Si la temperatura actual está fuera de rango permitido
				if (Control_Unit->Status.Temperatures[i].Actual_Value > 60.0f || Control_Unit->Status.Temperatures[i].Actual_Value < 0.0f)
				{
					// Se permite hasta 3 errores antes de marcar fallo
					if (Control_Unit->Status.Temperatures[i].Cont_Fail <= 2)
					{
						Control_Unit->Status.Temperatures[i].Cont_Fail++;
					}
					else
					{
						// Marca como Hot o Failed según el extremo
						if (Control_Unit->Status.Temperatures[i].Actual_Value > 60.0f)
						{
							Control_Unit->Status.Temperatures[i].Hot = TRUE;
						}
						else if (Control_Unit->Status.Temperatures[i].Actual_Value < 0.0f)
						{
							Control_Unit->Status.Temperatures[i].Failed = TRUE;
						}
					}
				}
				else
				{
					// Si la temperatura es normal, se limpian los flags de fallo
					Control_Unit->Status.Temperatures[i].Failed = FALSE;
					Control_Unit->Status.Temperatures[i].Hot = FALSE;
				}
			}
			else
			{
				// Sensor deshabilitado: se fuerza valor a 25°C
				Control_Unit->Status.Temperatures[i].Actual_Value = 107.5f;
			}
		}
	}
}

/*******************************************************************************
********************************************************************************
***************									 Check fails Task        			 	 ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Check_Fails(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Status.Temperatures_Failed=0;
	Control_Unit->Status.Temperatures_Hot=0;
	for (uint8_t i = 0; i < 24; i++)
	{
		if(Control_Unit->Status.Temperatures[i].Hot==TRUE)
		{
				Control_Unit->Status.Temperatures_Hot++;
		}
		if(Control_Unit->Status.Temperatures[i].Failed==TRUE)
		{
			Control_Unit->Status.Temperatures_Failed++;
		}
	}
	
	if(Control_Unit->Status.Temperatures_Hot>0 && Control_Unit->Status.Temperatures_Hot > Control_Unit->Status.Temperatures_Failed)
	{
		Control_Unit->State=TEMP_PLUS_60_FAIL_MODE;
	}
	
	if(Control_Unit->Status.Temperatures_Failed>0 && Control_Unit->Status.Temperatures_Failed > Control_Unit->Status.Temperatures_Hot)
	{
		Control_Unit->State=TEMP_FAIL_MODE;
	}

}
	

/*******************************************************************************
********************************************************************************
***************									 Read Task        			 	   ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Read_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Status.Read_Temperatures==READ_RECEIVED && Control_Unit->State!=INIT && Control_Unit->State!=LTC6811_FAIL_MODE)
	{
		Control_Unit->Status.Read_Temperatures=READING;
		LTC6811_Measure_Temperatures_and_Voltages(Control_Unit);
		if(Control_Unit->Status.LTC6811_1.Fail==FALSE && Control_Unit->Status.LTC6811_2.Fail==FALSE)
		{
			Battery_Pack_Control_Unit_Check_Temperatures(Control_Unit);
			Battery_Pack_Control_Check_Fails(Control_Unit);
			Generate_Finish_Message(Control_Unit);
			CAN1_Send(&Control_Unit->Tx_Message);
			Control_Unit->Status.Read_Temperatures=IDLE;
		}
		else
		{
			Control_Unit->Status.Read_Temperatures=IDLE;
		}
		
	}
}

/*******************************************************************************
********************************************************************************
***************										 Main Task        		    		 ***************	
********************************************************************************
*******************************************************************************/
void Battery_Pack_Control_Unit_Main_Task(Control_Unit_TypeDef* Control_Unit)
{
	State_LEDs_Task(&Control_Unit->Yellow_Led);
	State_LEDs_Task(&Control_Unit->Green_Led);
	Battery_Pack_Control_Read_Task(Control_Unit);
	Battery_Pack_Control_Unit_WDT_Task();
	Battery_Pack_Control_State_Machine_Task(Control_Unit);
	Battery_Pack_Control_Interrupt_Task(Control_Unit);
}
	
void Battery_Pack_Control_Unit_CAN1_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	switch(Control_Unit->Rx_Message.Header.StdId)
	{
		case BPCU_INIT_MEASURE_DEF:
			if(Control_Unit->Status.Read_Temperatures==IDLE && Control_Unit->State!=INIT && Control_Unit->State!=LTC6811_FAIL_MODE)
			{
				Control_Unit->Status.Read_Temperatures=READ_RECEIVED;
			}
		break;
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