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
***************									Init MCU Function	    		    	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicializa los periféricos básicos del microcontrolador necesarios para operar la unidad de control de baterías.
 *        Incluye la configuración general del MCU y la interfaz SPI utilizada por los dispositivos LTC6811.
 */
void Battery_Pack_Control_Unit_MCU_Init(void)
{
    // Inicialización general del microcontrolador (relojes, GPIOs, interrupciones, etc.)
    MCU_Init();

    // Inicializa la interfaz SPI dedicada a la comunicación con los chips LTC6811
    LTC6811_SPI_Init();
}
/*******************************************************************************
********************************************************************************
***************					Deviece Initialization Function	    		  **************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Lee desde memoria Flash el mapa de sensores activados y actualiza el estado de habilitación de cada sensor de temperatura.
 * 
 * Cada bit del valor leído representa el estado (habilitado/deshabilitado) de uno de los 24 sensores.
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control.
 */
void Battery_Pack_Control_Unit_Read_Memory_Init(Control_Unit_TypeDef* Control_Unit)
{
    // Lee desde la dirección de memoria Flash la palabra de 32 bits que representa el mapa de sensores activados
    uint32_t Activated_Sensors = MCU_Flash_Read_Word(Address_APP_BPCU_Activated_Sensors);

    // Itera sobre los 24 sensores de temperatura
    for (uint8_t i = 0; i < 24; i++)
    {
        // Extrae el estado del bit correspondiente al sensor i
        uint8_t bit_state = (Activated_Sensors >> i) & 0x01;

        if (bit_state)
        {
            // Si el bit está en 1 ? el sensor está habilitado
            Control_Unit->Status.Temperatures[i].Disabled = FALSE;
        }
        else
        {
            // Si el bit está en 0 ? el sensor está deshabilitado
            Control_Unit->Status.Temperatures[i].Disabled = TRUE;
        }
    }
}
/*******************************************************************************
********************************************************************************
***************									Init Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicializa los valores por defecto de temperaturas y voltajes en la unidad de control.
 *        También carga configuraciones almacenadas desde memoria persistente (Flash).
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control.
 */
void Battery_Pack_Control_Unit_Init_Values(Control_Unit_TypeDef* Control_Unit)
{
    // Lee configuración inicial desde la memoria Flash o EEPROM
    Battery_Pack_Control_Unit_Read_Memory_Init(Control_Unit);

    // Inicializa valores por defecto para los 24 sensores
    for (unsigned char i = 0; i < 24; i++)
    {
        // Temperatura inicial estimada (°C)
        Control_Unit->Status.Temperatures[i].Actual_Value = 25.0f;

        // Último valor leído (inicialmente igual al valor real)
        Control_Unit->Status.Temperatures[i].Readed_Value = 25.0f;

        // Voltaje inicial estimado (V)
        Control_Unit->Status.Voltages[i] = 3.6f;
    }
}


/*******************************************************************************
********************************************************************************
***************									Init Function	    		    	 	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicializa todos los subsistemas y variables internas de la unidad de control de baterías.
 *        Esto incluye LEDs de estado, temporizadores, valores iniciales y la comunicación con los LTC6811.
 * 
 * @param Control_Unit Puntero a la estructura principal que representa la unidad de control.
 */
void Battery_Pack_Control_Unit_Init(Control_Unit_TypeDef* Control_Unit)
{
    // Inicializa el sistema de LEDs de estado (verde y amarillo)
    State_LEDs_Init(Control_Unit);

    // Inicializa las variables internas y estructuras de estado de la unidad de control
    Battery_Pack_Control_Unit_Init_Values(Control_Unit);

    // Configura temporizador para envío de estado CAN cada 100 ms (aún no se inicia)
    Timer_10ms_Init(&Control_Unit->Timing.Status_Send_Timer, 1, MILISECONDS, 100);

    // Configura e inicia temporizador para duración del estado INIT (1000 ms)
    Timer_10ms_Init(&Control_Unit->Timing.Init_State_Timer, 1, MILISECONDS, 1000);
    Timer_10ms_Start(&Control_Unit->Timing.Init_State_Timer);

    // Configura temporizador para envío periódico de temperatura CAN cada 200 ms (no iniciado aún)
    Timer_10ms_Init(&Control_Unit->Timing.Temp_Send_Timer, 1, MILISECONDS, 200);

    // Inicializa los módulos de medición LTC6811 (voltaje y temperatura)
    LTC6811_Init(Control_Unit);
}

/*******************************************************************************
********************************************************************************
***************					Generate Status Message       			 	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Genera un mensaje CAN de estado general del sistema de baterías.
 *        El mensaje contiene el estado global, y contadores de sensores en sobretemperatura y fallidos.
 * 
 * @param Control_Unit Puntero a la estructura que contiene los estados actuales y el buffer de transmisión CAN.
 */
void Generate_Status_Message(Control_Unit_TypeDef* Control_Unit)
{
    // Asigna el identificador CAN correspondiente al mensaje de estado general
    Control_Unit->Tx_Message.ID = BPCU_STATUS_DEF;

    // Define la longitud del mensaje como 3 bytes
    Control_Unit->Tx_Message.DLC = 3;

    // Byte 0: estado actual de la unidad de control (por ejemplo, INIT, NORMAL_OPERATION, FAIL_MODE, etc.)
    Control_Unit->Tx_Message.Data[0] = Control_Unit->State;

    // Byte 1: número total de sensores en condición de sobretemperatura
    Control_Unit->Tx_Message.Data[1] = Control_Unit->Status.Temperatures_Hot;

    // Byte 2: número total de sensores marcados como fallidos
    Control_Unit->Tx_Message.Data[2] = Control_Unit->Status.Temperatures_Failed;
}
/*******************************************************************************
********************************************************************************
***************					Generate Temp Message       			 	     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Genera un mensaje CAN que contiene 6 temperaturas codificadas, junto con indicadores globales de sensores en fallo o sobrecalentados.
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control que contiene los datos de temperatura.
 */
  void Generate_Temp_Message(Control_Unit_TypeDef* Control_Unit)
	{  
	// Define longitud del mensaje CAN: 8 bytes
    Control_Unit->Tx_Message.DLC = 8;

    // Asigna el identificador CAN, variando en función del mensaje actual
    Control_Unit->Tx_Message.ID = BPCU_TEMP_1_DEF + Control_Unit->Status.CAN_Message;

    // Calcula el índice base para seleccionar las temperaturas que se van a enviar
    uint8_t base = Control_Unit->Status.CAN_Message * 6;

    // Protección contra desbordamiento del array de temperaturas (máximo índice permitido es 23)
    if (base + 5 >= 24) return;

    // Codifica y almacena las 6 temperaturas seleccionadas en los primeros 6 bytes
    for (uint8_t i = 0; i < 6; i++) 
    {
        Control_Unit->Tx_Message.Data[i] =
        LTC6811_Encode_Temp(Control_Unit->Status.Temperatures[base + i].Actual_Value);
    }

    // Byte 6: número total de sensores en sobretemperatura
    Control_Unit->Tx_Message.Data[6] = Control_Unit->Status.Temperatures_Hot;

    // Byte 7: número total de sensores fallidos
    Control_Unit->Tx_Message.Data[7] = Control_Unit->Status.Temperatures_Failed;	
}

/*******************************************************************************
********************************************************************************
***************					Generate Volt Message       			 	   	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Genera un mensaje CAN de voltajes (8 canales) codificados en resolución de 10 mV por muestra.
 *        El índice base se calcula según el estado actual de envío (`CAN_Message`) y se limita a un total de 24 canales.
 * 
 * @param Control_Unit Puntero a la estructura de control que contiene la información de voltajes y estado de envío.
 */
void Generate_Volt_Message(Control_Unit_TypeDef* Control_Unit)
{
	Control_Unit->Tx_Message.DLC = 8;

	// Asegura que el mensaje es válido
	if (Control_Unit->Status.CAN_Message < SEND_VOLT_1 || Control_Unit->Status.CAN_Message > SEND_VOLT_3)
		return;

	// ID correcto con offset de tipo de mensaje
	Control_Unit->Tx_Message.ID = BPCU_VOLT_1_DEF + (Control_Unit->Status.CAN_Message - SEND_VOLT_1);

	uint8_t base_index = (Control_Unit->Status.CAN_Message - SEND_VOLT_1) * 8;

	if (base_index + 7 >= 24) return;

	for (uint8_t i = 0; i < 8; i++)
	{
		Control_Unit->Tx_Message.Data[i] =
			LTC6811_Encode_Volt_10mV(Control_Unit->Status.Voltages[base_index + i]);
	}
}

/*******************************************************************************
********************************************************************************
***************					Generate Finished Measure       			 	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Genera un mensaje CAN que indica que la lectura de temperaturas y voltajes ha finalizado exitosamente.
 *        Este mensaje se utiliza como confirmación o "acknowledgement"
 * 
 * @param Control_Unit Puntero a la estructura de control que contiene el buffer de transmisión CAN.
 */
void Generate_Finish_Message(Control_Unit_TypeDef* Control_Unit)
{
    // Establece longitud del mensaje en 1 byte
    Control_Unit->Tx_Message.DLC = 1;

    // Asigna el identificador CAN que representa "medición finalizada"
    Control_Unit->Tx_Message.ID = BPCU_FINISHED_MEASURE;

    // Coloca un valor fijo (0x01) en el primer byte de datos para indicar estado 'OK' o 'finalizado'
    Control_Unit->Tx_Message.Data[0] = 0x01;
}

/*******************************************************************************
********************************************************************************
***************						 		  WATCHDOG TASK       			   		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Tarea encargada de refrescar (reiniciar) el Watchdog Timer (WDT) del microcontrolador.
 * 
 * Esta función debe ser llamada periódicamente dentro del ciclo principal del sistema para evitar
 * que el microcontrolador se reinicie por inactividad o bloqueo del sistema.
 * 
 * Su presencia garantiza que el software está ejecutándose correctamente. Si no se llama a tiempo,
 * el sistema asumirá un fallo y realizará un reinicio controlado.
 */
void Battery_Pack_Control_Unit_WDT_Task(void)
{
    MCU_WDT_Refresh();  // Refresca el temporizador de vigilancia (evita reset del microcontrolador)
}



/*******************************************************************************
********************************************************************************
***************						 		  STATE MACHINE TASK       			   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Gestiona el comportamiento visual de los LEDs según el estado actual del sistema de control.
 * 
 * Esta función actúa como una máquina de estados simple, en la que cada estado del sistema
 * se asocia con un patrón de comportamiento de los LEDs verde y amarillo.
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control.
 */
void Battery_Pack_Control_State_Machine_Task(Control_Unit_TypeDef* Control_Unit)
{
    switch (Control_Unit->State)
    {
        // Estado de inicialización: LED verde encendido fijo, amarillo parpadeando rápido (cada 200 ms)
        case INIT:
            Green_LED_Permanent_On(Control_Unit);
            Yellow_LED_Blink(Control_Unit, 200);
        break;

        // Estado operativo normal: solo LED verde encendido fijo, LED amarillo apagado
        case NORMAL_OPERATION:
            Green_LED_Permanent_On(Control_Unit);
        break;

        // Fallo en la comunicación con los chips LTC6811: ambos LEDs parpadean a 500 ms
        case LTC6811_FAIL_MODE:
            Green_LED_Blink(Control_Unit, 500);
            Yellow_LED_Blink(Control_Unit, 500);
        break;

        // Falla de temperatura (sensor caído o fuera de rango): LED verde apagado, LED amarillo parpadea a 500 ms
        case TEMP_FAIL_MODE:
            Green_LED_Permanent_Off(Control_Unit);
            Yellow_LED_Blink(Control_Unit, 500);
        break;

        // Temperatura crítica (>60°C): LED verde apagado, LED amarillo parpadea más lento (1000 ms)
        case TEMP_PLUS_60_FAIL_MODE:
            Green_LED_Permanent_Off(Control_Unit);
            Yellow_LED_Blink(Control_Unit, 1000);
        break;
    }
}



/*******************************************************************************
********************************************************************************
***************				Interrupt Task for CAN STATUS SEND         ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Tarea periódica encargada de generar y enviar por CAN el mensaje de estado general del sistema.
 *        Se activa cuando expira el temporizador asociado al envío de estado.
 * 
 * @param Control_Unit Puntero a la estructura de la unidad de control que contiene temporizadores y buffer de transmisión CAN.
 */
void CAN_Status_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Comprueba si el temporizador de envío de estado ha expirado
    if (Control_Unit->Timing.Status_Send_Timer.Overflowed == TRUE)
    {
        // Genera el mensaje de estado del sistema en el buffer Tx
        Generate_Status_Message(Control_Unit);

        // Reinicia el temporizador para el próximo envío
        Timer_10ms_Restart(&Control_Unit->Timing.Status_Send_Timer);

        // Envía el mensaje generado por el bus CAN
        CAN1_Send(&Control_Unit->Tx_Message);	
    }
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for CAN Temp SEND        ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Tarea periódica encargada de enviar por CAN los mensajes de temperaturas y voltajes de la batería.
 *        Alterna de forma cíclica entre los diferentes bloques de datos a transmitir.
 * 
 * @param Control_Unit Puntero a la estructura de la unidad de control que contiene el estado del envío CAN y temporizadores.
 */
void CAN_Temp_Send_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Verifica si el temporizador de envío de temperatura ha desbordado
    if (Control_Unit->Timing.Temp_Send_Timer.Overflowed == TRUE)
    {
        // Si el mensaje actual es uno de temperaturas (TEMP_1 a TEMP_4)
        if (Control_Unit->Status.CAN_Message <= SEND_TEMP_4)
        {
            Generate_Temp_Message(Control_Unit);  // Prepara el mensaje de temperatura correspondiente
        }
        // Si el mensaje actual es uno de voltajes (VOLT_1 a VOLT_3)
        else if (Control_Unit->Status.CAN_Message > SEND_TEMP_4 && Control_Unit->Status.CAN_Message <= SEND_VOLT_3)
        {
            Generate_Volt_Message(Control_Unit);  // Prepara el mensaje de voltaje correspondiente
        }

        // Envía el mensaje generado a través del bus CAN
        CAN1_Send(&Control_Unit->Tx_Message);

        // Avanza al siguiente tipo de mensaje
        Control_Unit->Status.CAN_Message++;

        // Si el índice se sale del rango válido, vuelve a empezar desde el primer mensaje de temperatura
        if (Control_Unit->Status.CAN_Message < SEND_TEMP_1 || Control_Unit->Status.CAN_Message > SEND_VOLT_3)
        {
            Control_Unit->Status.CAN_Message = SEND_TEMP_1;
        }

        // Reinicia el temporizador para controlar el próximo envío
        Timer_10ms_Restart(&Control_Unit->Timing.Temp_Send_Timer);
    }
}


/*******************************************************************************
********************************************************************************
***************				Interrupt Task for Initialitation timer    ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Gestiona la transición desde el estado de inicialización (INIT) hacia operación normal
 *        una vez transcurrido el tiempo definido por el temporizador de inicialización.
 * 
 * Esta función se ejecuta periódicamente y verifica si el temporizador asociado a la fase INIT ha expirado.
 * Si es así, detiene ese temporizador, activa otros necesarios para el funcionamiento normal, y cambia el estado del sistema.
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control de baterías.
 */
void Initialitation_Tim_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Verifica si ha transcurrido el tiempo definido para el estado INIT
    if (Control_Unit->Timing.Init_State_Timer.Overflowed == TRUE && Control_Unit->State==INIT)
    {
        // Detiene el temporizador de inicialización para evitar múltiples entradas
        Timer_10ms_Stop(&Control_Unit->Timing.Init_State_Timer);

        // Inicia el temporizador que controla el envío periódico del estado por CAN
        Timer_10ms_Start(&Control_Unit->Timing.Status_Send_Timer);

        // Inicia el temporizador que controla el envío periódico de temperaturas por CAN
        Timer_10ms_Start(&Control_Unit->Timing.Temp_Send_Timer);

        // Apaga permanentemente el LED amarillo como señal de que finalizó el INIT
        Yellow_LED_Permanent_Off(Control_Unit);

        // Cambia el estado del sistema a operación normal
        Control_Unit->State = NORMAL_OPERATION;
    }
}



/*******************************************************************************
********************************************************************************
***************									 Interrupt Task        			 	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Ejecuta tareas periódicas relacionadas con interrupciones internas del sistema de control de baterías.
 *        Agrupa las funciones de temporización, envío de estado, monitoreo de temperatura y gestión de LEDs.
 * 
 * @param Control_Unit Puntero a la estructura de control del sistema de baterías.
 */
void Battery_Pack_Control_Interrupt_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Tarea que evalúa y gestiona las transiciones desde el estado de inicialización (INIT)
    Initialitation_Tim_Interrupt_Task(Control_Unit);

    // Tarea que envía el estado general del sistema por CAN si ha transcurrido el tiempo correspondiente
    CAN_Status_Send_Interrupt_Task(Control_Unit);

    // Tarea que envía los datos de temperatura por CAN cuando corresponde
    CAN_Temp_Send_Interrupt_Task(Control_Unit);

    // Tarea que gestiona los cambios de estado o parpadeo de los LEDs según su temporización
    State_LEDs_Interrupt_Task(Control_Unit);
}


/*******************************************************************************
********************************************************************************
***************							 Timer 10ms Interrupt        		     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Rutina de servicio de interrupción que se ejecuta cada 10 ms.
 *        Gestiona los temporizadores internos y el estado de los LEDs,
 *        siempre que no se esté realizando una lectura de temperaturas ni el sistema esté en INIT o fallo LTC6811.
 * 
 * @param Control_Unit Puntero a la estructura principal de la unidad de control.
 */
void Battery_Pack_Control_Unit_10ms_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
    // Verifica que no se esté realizando una lectura de temperaturas,
    // y que el sistema esté fuera del estado de inicialización o fallo grave
    if (Control_Unit->Status.Read_Temperatures != READING &&
        Control_Unit->State != INIT &&
        Control_Unit->State != LTC6811_FAIL_MODE)
    {
        // Incrementa el temporizador de envío de estado por CAN
        Timer_10ms_Tick(&Control_Unit->Timing.Status_Send_Timer);

        // Incrementa el temporizador que controla las transiciones del estado INIT
        Timer_10ms_Tick(&Control_Unit->Timing.Init_State_Timer);

        // Incrementa el temporizador para el envío de temperaturas
        Timer_10ms_Tick(&Control_Unit->Timing.Temp_Send_Timer);

        // Actualiza temporización y efectos de los LEDs según su estado
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
/**
 * @brief Verifica los resultados de la lectura de temperaturas para detectar condiciones de error o fallo térmico.
 *        Establece el estado de la unidad de control si se detectan temperaturas críticas o sensores fallidos.
 * 
 * @param Control_Unit Puntero a la estructura de la unidad de control que contiene los estados térmicos individuales.
 */
void Battery_Pack_Control_Check_Fails(Control_Unit_TypeDef* Control_Unit)
{
    // Inicializa contadores de sensores con problemas térmicos
    Control_Unit->Status.Temperatures_Failed = 0;
    Control_Unit->Status.Temperatures_Hot = 0;

    // Recorre todos los sensores (24 en total)
    for (uint8_t i = 0; i < 24; i++)
    {
        // Si el sensor indica temperatura excesiva (Hot)
        if (Control_Unit->Status.Temperatures[i].Hot == TRUE)
        {
            Control_Unit->Status.Temperatures_Hot++;
        }

        // Si el sensor está marcado como fallido (por ejemplo, error de lectura)
        if (Control_Unit->Status.Temperatures[i].Failed == TRUE)
        {
            Control_Unit->Status.Temperatures_Failed++;
        }
    }

    // Condición 1: Hay sensores calientes y son más que los fallidos -> fallo térmico (>60°C)
    if (Control_Unit->Status.Temperatures_Hot > 0 &&
        Control_Unit->Status.Temperatures_Hot > Control_Unit->Status.Temperatures_Failed)
    {
        Control_Unit->State = TEMP_PLUS_60_FAIL_MODE;
    }

    // Condición 2: Hay sensores fallidos y son más que los calientes -> fallo de medición
    if (Control_Unit->Status.Temperatures_Failed > 0 && Control_Unit->Status.Temperatures_Failed >= Control_Unit->Status.Temperatures_Hot)
    {
        Control_Unit->State = TEMP_FAIL_MODE;
    }
}
	

/*******************************************************************************
********************************************************************************
***************									 Read Task        			 	   		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Ejecuta la lectura de temperaturas y voltajes del paquete de baterías cuando se ha solicitado.
 *        Valida que el sistema esté en un estado válido y que los dispositivos de medición (LTC6811) estén operativos.
 * 
 * @param Control_Unit Puntero a la estructura de control del sistema de baterías.
 */
void Battery_Pack_Control_Read_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Verifica que se haya recibido una solicitud de lectura y que el sistema no esté en estado INIT ni en modo de fallo de los LTC
    if (Control_Unit->Status.Read_Temperatures == READ_RECEIVED &&
        Control_Unit->State != INIT &&
        Control_Unit->State != LTC6811_FAIL_MODE)
    {
        // Cambia el estado a 'leyendo' para evitar reentradas simultáneas
        Control_Unit->Status.Read_Temperatures = READING;

        // Inicia la medición de temperaturas y voltajes a través de los dos chips LTC6811
        LTC6811_Measure_Temperatures_and_Voltages(Control_Unit);

        // Verifica que ambas cadenas de medición estén funcionando correctamente
        if (Control_Unit->Status.LTC6811_1.Fail == FALSE )//&& Control_Unit->Status.LTC6811_2.Fail == FALSE)
        {
            // Evalúa temperaturas medidas para verificar límites seguros
            Battery_Pack_Control_Unit_Check_Temperatures(Control_Unit);

            // Revisa condiciones de fallo generales tras la lectura
            Battery_Pack_Control_Check_Fails(Control_Unit);

            // Prepara el mensaje de finalización de lectura para ser enviado por CAN
            Generate_Finish_Message(Control_Unit);
            CAN1_Send(&Control_Unit->Tx_Message);

            // Devuelve el estado a IDLE indicando que se completó la lectura
            Control_Unit->Status.Read_Temperatures = IDLE;
        }
        else
        {
            // En caso de fallo en la lectura, se libera el estado sin procesamiento adicional
            Control_Unit->Status.Read_Temperatures = IDLE;
        }
    }
}

/*******************************************************************************
********************************************************************************
***************										 Main Task        		    		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Tarea principal del sistema de control de baterías. Se ejecuta cíclicamente y coordina la operación del sistema.
 * 
 * Esta función representa el núcleo operativo del sistema de la unidad de control de baterías.
 * Coordina las tareas de:
 * - Gestión de LEDs de estado
 * - Lectura de sensores
 * - Temporización de vigilancia (watchdog)
 * - Máquina de estados
 * - Gestión de interrupciones
 * 
 * @param Control_Unit Puntero a la estructura que contiene el estado completo de la unidad de control.
 */
void Battery_Pack_Control_Unit_Main_Task(Control_Unit_TypeDef* Control_Unit)
{
    // Actualiza el estado y parpadeo del LED amarillo según el estado actual
    State_LEDs_Task(&Control_Unit->Yellow_Led);

    // Actualiza el estado y parpadeo del LED verde según el estado actual
    State_LEDs_Task(&Control_Unit->Green_Led);

    // Ejecuta tareas relacionadas con la lectura de sensores de temperatura y voltaje
    Battery_Pack_Control_Read_Task(Control_Unit);

    // Tarea del watchdog: actualiza o reinicia el temporizador de vigilancia
    Battery_Pack_Control_Unit_WDT_Task();

    // Ejecuta la lógica de transición de estados del sistema (por ejemplo, INIT ? RUNNING ? FAULT)
    Battery_Pack_Control_State_Machine_Task(Control_Unit);

    // Procesa interrupciones o mensajes CAN pendientes recibidos
    Battery_Pack_Control_Interrupt_Task(Control_Unit);
}
	
	

/*******************************************************************************
********************************************************************************
***************										Cancel Sensors       		 		   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Cancela (habilita o deshabilita) sensores de temperatura de un grupo específico según el offset recibido.
 *        Aplica los cambios en RAM y, si es necesario, actualiza la memoria Flash con la nueva configuración.
 * 
 * @param Control_Unit Puntero a la estructura que contiene el estado de la unidad de control de baterías y el mensaje CAN recibido.
 * @param sensor_offset Índice base del grupo de sensores a modificar (0 = grupo 1, 8 = grupo 2, 16 = grupo 3).
 */
void Battery_Pack_Control_Unit_Cancel_Sensors(Control_Unit_TypeDef* Control_Unit, uint8_t sensor_offset)
{
    BoolTypeDef Changed = FALSE;  // Indica si hubo algún cambio en los estados de los sensores

    // Recorre los 8 bytes de datos del mensaje CAN para este grupo de sensores
    for (uint8_t i = 0; i < 8; i++)
    {
        if (Control_Unit->Rx_Message.Data[i] == 0x01)
        {
            // Solicitud para habilitar el sensor i+offset
            Control_Unit->Status.Temperatures[i + sensor_offset].Disabled = FALSE;
            Changed = TRUE;
        }
        else if (Control_Unit->Rx_Message.Data[i] == 0x02)
        {
            // Solicitud para deshabilitar el sensor i+offset
            Control_Unit->Status.Temperatures[i + sensor_offset].Disabled = TRUE;
            Changed = TRUE;
        }
        // Se ignoran otros valores (distintos de 0x01 o 0x02)
    }

    // Solo se actualiza la Flash si se detectaron cambios
    if (Changed == TRUE)
    {
        // Leer el estado actual del mapa de sensores activados desde la memoria Flash
        uint32_t Activated_Sensors = MCU_Flash_Read_Word(Address_APP_BPCU_Activated_Sensors);
        uint32_t New_Activated_Sensors = Activated_Sensors;

        // Recorre todos los sensores (0–23) para construir el nuevo mapa de bits actualizado
        for (uint8_t i = 0; i < 24; i++)
        {
            uint8_t bit_state = (Activated_Sensors >> i) & 0x01;

            if (bit_state == 0 && Control_Unit->Status.Temperatures[i].Disabled == FALSE)
            {
                // El sensor está habilitado pero el bit está en 0 ? activarlo (poner bit en 1)
                New_Activated_Sensors |= (1UL << i);
            }
            else if (bit_state == 1 && Control_Unit->Status.Temperatures[i].Disabled == TRUE)
            {
                // El sensor está deshabilitado pero el bit está en 1 ? desactivarlo (poner bit en 0)
                New_Activated_Sensors &= ~(1UL << i);
            }
        }

        // Si el nuevo mapa es diferente al anterior, se escribe en memoria Flash
        if (New_Activated_Sensors != Activated_Sensors)
        {
            MCU_Flash_Program_Word(Address_APP_BPCU_Activated_Sensors, New_Activated_Sensors);
        }
    }
}

/*******************************************************************************
********************************************************************************
***************										 CAN 1 INTERRUPT        		   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Maneja las interrupciones CAN1 recibidas en la unidad de control del paquete de baterías.
 *        Evalúa el identificador del mensaje CAN (StdId) y ejecuta acciones específicas según el tipo de comando recibido.
 * 
 * @param Control_Unit Puntero a la estructura que representa el estado de la unidad de control de batería.
 */
void Battery_Pack_Control_Unit_CAN1_Interrupt(Control_Unit_TypeDef* Control_Unit)
{
	switch(Control_Unit->Rx_Message.Header.StdId)
	{
		case BPCU_INIT_MEASURE_DEF:
			// Inicia la lectura de temperaturas si el estado es IDLE, el sistema no está en INIT ni en modo de fallo LTC6811,
			// el mensaje tiene un solo byte de datos (DLC == 0x01), y ese byte es 0x01.
			if(Control_Unit->Status.Read_Temperatures == IDLE &&  Control_Unit->State != INIT && Control_Unit->State != LTC6811_FAIL_MODE &&
			   Control_Unit->Rx_Message.Header.DLC == 0x01 && Control_Unit->Rx_Message.Data[0] == 0x01)
			{
				Control_Unit->Status.Read_Temperatures = READ_RECEIVED;
			}
		break;
			
			
		case BPCU_CANCEL_SENSOR_1_DEF:
			// Cancela sensores del grupo 1 si el mensaje contiene 8 bytes (DLC == 0x08)
			if(Control_Unit->Rx_Message.Header.DLC == 0x08)
			{
				Battery_Pack_Control_Unit_Cancel_Sensors(Control_Unit,0);
			}
		break;
			
			
		case BPCU_CANCEL_SENSOR_2_DEF:
			// Cancela sensores del grupo 2 si el mensaje contiene 8 bytes (DLC == 0x08)
			if(Control_Unit->Rx_Message.Header.DLC == 0x08)
			{
				Battery_Pack_Control_Unit_Cancel_Sensors(Control_Unit,8);
			}
		break;
			
			
		case BPCU_CANCEL_SENSOR_3_DEF:
			// Cancela sensores del grupo 3 si el mensaje contiene 8 bytes (DLC == 0x08)
			if(Control_Unit->Rx_Message.Header.DLC == 0x08)
			{
				Battery_Pack_Control_Unit_Cancel_Sensors(Control_Unit,16);
			}
		break;
			
		default:
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