	/**
  ******************************************************************************
  * @file           : Signals.h
  * @brief          : View of signals from control units header file
  ******************************************************************************
  * @attention
  *
  * (c) 2023 Uniovi E-tech Racing.
  * 
  *
  ******************************************************************************  
  ******************************************************************************
																Version Control
	******************************************************************************
	******************************************************************************		
  Version | dd mmm yyyy |       Who        | Description of changes
  ========|=============|==================|====================================
    1.0   | 08 ABR 2024 | Ivan Garcia		   | Creation
	========|=============|==================|====================================
	
  ******************************************************************************
  ******************************************************************************
  */
	
	
	/**
			*****    EXAMPLE    *****    
	
	  FROM    |    TO    |    Brief description    
	
	  REAR		->	DASH	 :   	Green led ON of dashboard



		*****    REAR PDB    *****



		*****    REAR MCU    *****



		*****    FRONT MCU    *****  



		*****    ACCU MASTER    *****  
		
		
		
		*****    SDC CHARGER    ***** 
		
    SDC Charger -> Comunication : CAN_H & CAN_L        
    SDC Charger -> Shutdown_Circuit : SDC_OUT (emergency button) - INPUT_5                    Reading
    SDC Charger -> Charger : BUT4 (button) - INPUT_3                                          Reading
    SDC Charger -> BMS/IMD (Check LEDs Status) : BMS_LED_ON & IMD_LED_ON - INPUT_7 | INPUT_8  Reading
		
    -----------------
    
    Accumulator -> SDC Charger : ACCU_ON_SIGNAL - OUTPUT_7                                    Activation    activated every 500ms at board start-up.
    Charger -> SDC Charger : CHARGER_ON_SIGNAL (CAN msg -- charging bit '1') - OUTPUT_6       Activation    activated every 500ms at board start-up.
    BMS -> SDC Charger : BMS_LED - OUTPUT_4                                                   Activation
    IMD -> SDC Charger : IMD_LED - OUTPUT_3                                                   Activation
    Shutdown Circuit -> SDC Charger : SDC_ON_SIGNAL - OUTPUT_1                                Activation    activated every 500ms at board start-up.
    Charger -> SDC Charger : buzzer_on  (CAN msg -- charging bit '1') - OUTPUT_5              Activation
    Charger -> SDC Charger : LED_Charging_ON (CAN msg -- charging bit '1') - OUTPUT_2         Activation  


		*****    DASHBOARD    *****  
		
		
		
		*****    STEERING WHEEL    ***** 


	
	
	*/
	

	/*****************************************************************************
	** 																END OF FILE																**			
	******************************************************************************
	******************************************************************************
  * @file           : Signals.h
  * @brief          : View of signals from control units header file
  ******************************************************************************
  * @attention
  *
  * (c) 2023 Uniovi E-tech Racing.
  * 
  *
  ******************************************************************************  
  ******************************************************************************
																Version Control
	******************************************************************************
	******************************************************************************		
  Version | dd mmm yyyy |       Who        | Description of changes
  ========|=============|==================|====================================
    1.0   | 08 ABR 2024 | Ivan Garcia		   | Creation
	========|=============|==================|====================================
	
  ******************************************************************************
  ******************************************************************************
  */