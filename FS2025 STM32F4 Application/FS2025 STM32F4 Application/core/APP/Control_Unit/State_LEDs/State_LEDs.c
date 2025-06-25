/**
  ******************************************************************************
  * @file           : State_LEDs.h
  * @brief          : State_LEDs header file
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
	
#include "State_LEDs.h"

/*******************************************************************************
********************************************************************************
***************									Green Led Init      	  			   ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_Init (void)
{
	MCU_GPIO_Init_C9_Output();
}


/*******************************************************************************
********************************************************************************
***************									Yellow Led Init      	  			   ***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_Init (void)
{
	MCU_GPIO_Init_C8_Output();
}


/*******************************************************************************
********************************************************************************
***************									State Leds Init      	  			   ***************	
********************************************************************************
*******************************************************************************/
void State_LEDs_Init(Control_Unit_TypeDef* Control_Unit)
{	
	Green_LED_Init();
	Yellow_LED_Init();
	
	//Inicializacion del LED Verde
	Control_Unit->Green_Led.Color=GREEN;
	Control_Unit->Green_Led.Mode=PERMANENT_OFF;
	Control_Unit->Green_Led.Mode_ant=PERMANENT_OFF;
	Timer_10ms_Init(&Control_Unit->Green_Led.Led_Tim,1,MILISECONDS,200);
	
	//Inicializacion del LED Yellow
	Control_Unit->Yellow_Led.Color=YELLOW;
	Control_Unit->Yellow_Led.Mode=PERMANENT_OFF;
	Control_Unit->Yellow_Led.Mode_ant=PERMANENT_OFF;
	Timer_10ms_Init(&Control_Unit->Yellow_Led.Led_Tim,1,MILISECONDS,50);
}

/*******************************************************************************
********************************************************************************
***************										Green LED On          				 ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_On (void)
{
	MCU_GPIO_Write_C9_Output(TRUE);
}


/*******************************************************************************
********************************************************************************
***************										Yellow LED On          				 ***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_On (void)
{

	MCU_GPIO_Write_C8_Output(TRUE);
}

/*******************************************************************************
********************************************************************************
***************									Green LED Off       	  			   ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_Off (void)
{
	MCU_GPIO_Write_C9_Output(FALSE);
}


/*******************************************************************************
********************************************************************************
***************									Yellow LED Off       	  			   ***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_Off (void)
{
	MCU_GPIO_Write_C8_Output(FALSE);
}


/*******************************************************************************
********************************************************************************
***************								Yellow LED Permanent On          	 ***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_Permanent_On (Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Yellow_Led.Mode=PERMANENT_ON;

}

/*******************************************************************************
********************************************************************************
***************								Yellow LED Permanent On          	 ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_Permanent_On (Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Green_Led.Mode=PERMANENT_ON;
}

/*******************************************************************************
********************************************************************************
***************								Yellow LED Permanent Off         	 ***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_Permanent_Off (Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Yellow_Led.Mode=PERMANENT_OFF;
}

/*******************************************************************************
********************************************************************************
***************								Yellow LED Permanent Off           ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_Permanent_Off (Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Green_Led.Mode=PERMANENT_OFF;
}

/*******************************************************************************
********************************************************************************
***************								Green LED TIMED On          	  	 ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_Blink (Control_Unit_TypeDef* Control_Unit,unsigned int time)
{
	Timer_10ms_Change_Timing(&Control_Unit->Green_Led.Led_Tim, time);
	Control_Unit->Green_Led.Blink_Rate=time;
	Control_Unit->Green_Led.Mode=BLINKING;
}


/*******************************************************************************
********************************************************************************
***************								Yellow LED TIMED On          			***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_Blink (Control_Unit_TypeDef* Control_Unit,unsigned int time)
{
	Timer_10ms_Change_Timing(&Control_Unit->Yellow_Led.Led_Tim, time);
	Control_Unit->Yellow_Led.Blink_Rate=time;
	Control_Unit->Yellow_Led.Mode=BLINKING;
}


/*******************************************************************************
********************************************************************************
***************								Green LED TIMED On          	  	 ***************	
********************************************************************************
*******************************************************************************/
void Green_LED_One_Blink (Control_Unit_TypeDef* Control_Unit,unsigned int time)
{
	Timer_10ms_Change_Timing(&Control_Unit->Green_Led.Led_Tim, time);
	Control_Unit->Green_Led.Blink_Rate=time;
	Control_Unit->Green_Led.Mode=ONE_BLINK;
}


/*******************************************************************************
********************************************************************************
***************								Yellow LED TIMED On          			***************	
********************************************************************************
*******************************************************************************/
void Yellow_LED_One_Blink (Control_Unit_TypeDef* Control_Unit,unsigned int time)
{
	Timer_10ms_Change_Timing(&Control_Unit->Yellow_Led.Led_Tim, time);
	Control_Unit->Yellow_Led.Blink_Rate=time;
	Control_Unit->Yellow_Led.Mode=ONE_BLINK;
}


/*******************************************************************************
********************************************************************************
***************									 State_LEDs_Task       			 		 ***************	
********************************************************************************
*******************************************************************************/
void State_LEDs_Task(Control_Unit_Led_Typedef* LED)
{
	if(LED->Mode_ant != LED->Mode)
	{
		LED->Mode_ant = LED->Mode;
		
		//LED permenente OFF
		if(LED->Mode==PERMANENT_OFF)
		{
			LED->State=OFF;
			Timer_10ms_Stop(&LED->Led_Tim);
			Timer_10ms_Reset(&LED->Led_Tim);
			if(LED->Color==GREEN)
			{
				Green_LED_Off();
			}
			else if(LED->Color==YELLOW)
			{
				Yellow_LED_Off();
			}
		}
		
		//LED permenente ON
		if(LED->Mode==PERMANENT_ON)
		{
			LED->State=ON;
			Timer_10ms_Stop(&LED->Led_Tim);
			Timer_10ms_Reset(&LED->Led_Tim);
			if(LED->Color==GREEN)
			{
				Green_LED_On();
			}
			else if(LED->Color==YELLOW)
			{
				Yellow_LED_On();
			}
		}
		
		//LED blink
		if(LED->Mode==BLINKING)
		{
			LED->State=ON;
			Timer_10ms_Restart(&LED->Led_Tim);
			if(LED->Color==GREEN)
			{
				Green_LED_On();
			}
			else if(LED->Color==YELLOW)
			{
				Yellow_LED_On();
			}
		}
		
		//LED one blink
		if(LED->Mode==ONE_BLINK)
		{
			LED->State=ON;
			Timer_10ms_Restart(&LED->Led_Tim);
			if(LED->Color==GREEN)
			{
				Green_LED_On();
			}
			else if(LED->Color==YELLOW)
			{
				Yellow_LED_On();
			}
		}		
	}
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Green Led   			   ***************	
********************************************************************************
*******************************************************************************/
void Green_Led_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Green_Led.Led_Tim.Overflowed==TRUE)
	{
		if(Control_Unit->Green_Led.Mode==BLINKING)
		{
			if(Control_Unit->Green_Led.State==ON)
			{
				Control_Unit->Green_Led.State=OFF;
				Green_LED_Off();
			}
			else
			{
				Control_Unit->Green_Led.State=ON;
				Green_LED_On();
			}
			Timer_10ms_Restart(&Control_Unit->Green_Led.Led_Tim);
		}
		else if(Control_Unit->Green_Led.Mode==ONE_BLINK)
		{
			Green_LED_Off();
			Control_Unit->Green_Led.State=OFF;
			Timer_10ms_Stop(&Control_Unit->Green_Led.Led_Tim);
			Timer_10ms_Reset(&Control_Unit->Green_Led.Led_Tim);
		}
	}
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Yellow Led   			   ***************	
********************************************************************************
*******************************************************************************/
void Yellow_Led_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	if(Control_Unit->Yellow_Led.Led_Tim.Overflowed==TRUE)
	{
		if(Control_Unit->Yellow_Led.Mode==BLINKING)
		{
			if(Control_Unit->Yellow_Led.State==ON)
			{
				Control_Unit->Yellow_Led.State=OFF;
				Yellow_LED_Off();
			}
			else
			{
				Control_Unit->Yellow_Led.State=ON;
				Yellow_LED_On();
			}
			Timer_10ms_Restart(&Control_Unit->Yellow_Led.Led_Tim);
		}
		else if(Control_Unit->Yellow_Led.Mode==ONE_BLINK)
		{
			Yellow_LED_Off();
			Control_Unit->Yellow_Led.State=OFF;
			Timer_10ms_Stop(&Control_Unit->Yellow_Led.Led_Tim);
			Timer_10ms_Reset(&Control_Unit->Yellow_Led.Led_Tim);
		}
	}
}

/*******************************************************************************
********************************************************************************
***************						Interrupt Task for  Led   					   ***************	
********************************************************************************
*******************************************************************************/
void State_LEDs_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
	Yellow_Led_Interrupt_Task(Control_Unit);
	Green_Led_Interrupt_Task(Control_Unit);
}

/*******************************************************************************
********************************************************************************
***************							Tick Timer for  Led   					 		 ***************	
********************************************************************************
*******************************************************************************/
void State_LEDs_10ms_Tick(Control_Unit_TypeDef* Control_Unit)
{
	Timer_10ms_Tick(&Control_Unit->Green_Led.Led_Tim);
	 Timer_10ms_Tick(&Control_Unit->Yellow_Led.Led_Tim);
}

