/**
  ******************************************************************************
  * @file           : LTC6811.h
  * @brief          : LTC6811 header file
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
	
#include "LTC6811.h"
#include "MCU.h"

/*******************************************************************************
********************************************************************************
***************										Tablas      	  			 			  ***************	
********************************************************************************
*******************************************************************************/
// Tabla de conversión V -> °C (Vout de sensor vs temperatura)
const float Temp_Table_V[] = {
    2.44, 2.42, 2.40, 2.38, 2.35, 2.32, 2.27, 2.23, 2.17, 2.11,
    2.05, 1.99, 1.92, 1.86, 1.80, 1.74, 1.68, 1.63, 1.59, 1.55,
    1.51, 1.48, 1.45, 1.43, 1.40, 1.38, 1.37, 1.35, 1.34, 1.33,
    1.32, 1.31, 1.30
};

const int8_t Temp_Table_C[] = {
    -40, -35, -30, -25, -20, -15, -10,  -5,   0,   5,
     10,  15,  20,  25,  30,  35,  40,  45,  50,  55,
     60,  65,  70,  75,  80,  85,  90,  95, 100, 105,
    110, 115, 120
};



/*******************************************************************************
********************************************************************************
***************									LTC6811 TIMERS Init      	  	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicializa los dos dispositivos LTC6811 en la unidad de control.
 *
 * Esta función configura los pines y handlers SPI para los dos LTC6811 conectados
 * a la unidad de control, los despierta del modo de bajo consumo y desactiva 
 * el balanceo de celdas como medida de seguridad inicial.
 *
 * También verifica si alguno de los dispositivos falló durante la inicialización
 * (por ejemplo, error de comunicación), y si es así, cambia el estado global 
 * de la unidad de control a `LTC6811_FAIL_MODE`.
 *
 * @param[in,out] Control_Unit  Puntero a la estructura principal de control del sistema.
 */
void LTC6811_Init(Control_Unit_TypeDef* Control_Unit)
{
    // Inicializar los ltc con la configuración de inicio
    LTC6811_init_cfg(&Control_Unit->Status.LTC6811_1);
    LTC6811_init_cfg(&Control_Unit->Status.LTC6811_2);
	
	  LTC6811_set_cfgr(&Control_Unit->Status.LTC6811_1,REFON_1,ADCOPT_1,gpioBits_a_1,dccBits_a_1);
		LTC6811_set_cfgr(&Control_Unit->Status.LTC6811_2,REFON_2,ADCOPT_2,gpioBits_a_2,dccBits_a_2);
	
		LTC6811_reset_crc_count(&Control_Unit->Status.LTC6811_1);
		LTC6811_reset_crc_count(&Control_Unit->Status.LTC6811_2);
	
		LTC6811_init_reg_limits(&Control_Unit->Status.LTC6811_1);
		LTC6811_init_reg_limits(&Control_Unit->Status.LTC6811_2);

     // Despierta ambos LTC6811
    wakeup_idle(1);
    wakeup_idle(2);
    HAL_Delay(1); 

		// Graba la configuración
		LTC6811_wrcfg(1,&Control_Unit->Status.LTC6811_1);
		LTC6811_wrcfg(2,&Control_Unit->Status.LTC6811_2);
}


/*******************************************************************************
********************************************************************************
***************							 	Float Volt to UINT8      	  	     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Codifica un voltaje en un valor entero de 8 bits con resolución de 10 mV.
 *
 * Esta función convierte un voltaje flotante en el rango [2.00 V, 4.55 V] a un valor
 * entero de 8 bits, con una resolución de 10 mV por unidad (0.01 V).
 *
 * Si el voltaje está fuera del rango, se limita (clampa) al valor más cercano permitido.
 * Se aplica redondeo para mejorar la precisión de la cuantización.
 *
 * Fórmula de codificación:
 * - `0` representa 2.00 V
 * - `1` representa 2.01 V
 * - ...
 * - `255` representa 4.55 V
 *
 * @param[in] volt  Voltaje en voltios a codificar.
 * @return Valor codificado como uint8_t (resolución de 10 mV).
 */
uint8_t LTC6811_Encode_Volt_10mV(float volt) {
    if (volt < 2.0f) volt = 2.0f;
    if (volt > 4.55f) volt = 4.55f;

    return (uint8_t)((volt - 2.0f) * 100.0f + 0.5f); // +0.5 para redondeo correcto
}

/*******************************************************************************
********************************************************************************
***************								INT8 Temp to UINT8      	  	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Codifica una temperatura flotante en un valor entero de 8 bits.
 *
 * Esta función convierte una temperatura en grados Celsius a un valor codificado 
 * de 8 bits (uint8_t), que representa temperaturas desde -20.0 °C hasta 107.5 °C 
 * con una resolución de 0.5 °C por unidad.
 *
 * Si la temperatura está fuera del rango [-20.0, 107.5], se limita al valor más cercano.
 * Se aplica un redondeo para que los pasos de 0.5 °C se traduzcan correctamente.
 *
 * Fórmula de codificación:
 * - `0` representa -20.0 °C
 * - `1` representa -19.5 °C
 * - ...
 * - `255` representa 107.5 °C
 *
 * @param[in] Temp  Temperatura en grados Celsius a codificar.
 * @return Valor codificado como uint8_t.
 */
uint8_t LTC6811_Encode_Temp(float Temp) 
{
    if (Temp < -20.0f) Temp = -20.0f;
    if (Temp > 107.5f) Temp = 107.5f;

    return (uint8_t)((Temp + 20.0f) * 2.0f + 0.5f);  // +0.5 para redondeo
}

/*******************************************************************************
********************************************************************************
***************										Voltage to Temp	      	  	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Convierte un voltaje medido a temperatura usando interpolación lineal.
 *
 * Esta función busca convertir un valor de voltaje a su correspondiente temperatura
 * estimada utilizando dos tablas (`Temp_Table_V` y `Temp_Table_C`) que representan
 * una relación voltaje-temperatura.
 *
 * Se realiza una búsqueda secuencial en la tabla de voltajes (`Temp_Table_V`)
 * para encontrar el intervalo donde se encuentra el voltaje `v`, y luego se interpola
 * linealmente entre los puntos adyacentes para calcular la temperatura.
 *
 * Si el voltaje está fuera del rango de la tabla, se retorna -100.0 como indicador de error.
 *
 * @param[in] v  Voltaje en voltios a convertir.
 * @return Temperatura estimada en grados Celsius, o -100.0 si está fuera de rango.
 */
float LTC_Voltage_to_Temperature(float v) {
    for (uint8_t i = 0; i < sizeof(Temp_Table_V) / sizeof(float) - 1; i++) {
        if (v <= Temp_Table_V[i] && v >= Temp_Table_V[i + 1]) {
            float t = Temp_Table_C[i] + (v - Temp_Table_V[i]) /
                      (Temp_Table_V[i + 1] - Temp_Table_V[i]) *
                      (Temp_Table_C[i + 1] - Temp_Table_C[i]);
            return t;
        }
    }
    return -100.0f;  // fuera de rango
}




/*******************************************************************************
********************************************************************************
***************								Activar balanceo par				     	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Activa el balanceo de celdas pares en un LTC6811.
 *
 * Esta función configura el LTC6811 para activar el balanceo de celdas pares 
 * (C2, C4, C6, C8, C10, C12) utilizando los bits DCC (Discharge Cell Control).
 * También habilita el regulador de referencia interno (`REFON = 1`) necesario 
 * para operar el circuito de descarga.
 *
 * Después de aplicar la configuración, se verifica que haya sido correctamente 
 * escrita mediante una lectura de vuelta de los registros de configuración.
 * Si la verificación falla o no hay comunicación, se marca la estructura con 
 * `Fail = TRUE`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura de estado del LTC6811.
 */
void LTC_Active_Even_Balancing(uint8_t n_ic, cell_asic* LTC6811) 
{
    bool dcc[12] = {false};

    for (int i = 1; i < 12; i += 2)  // índices 1, 3, 5, 7, 9, 11 ? C2, C4, ..., C12
        dcc[i] = true;

    LTC6811_set_cfgr_dis(LTC6811, dcc);
    LTC6811_wrcfg(n_ic, LTC6811);  // Asume que hay solo un IC. Cambia si usás varios.
}



/*******************************************************************************
********************************************************************************
***************								Activar balanceo IMpar				     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Activa el balanceo de celdas impares en un LTC6811.
 *
 * Esta función configura los bits DCC (Discharge Cell Control) para activar el balanceo
 * solo en las celdas impares (C1, C3, C5, ..., C11) del LTC6811. También enciende el regulador 
 * interno (`REFON = 1`) para permitir la operación del circuito de balanceo.
 *
 * Luego de escribir la configuración, se realiza una lectura de verificación para confirmar
 * que los bits DCC se hayan aplicado correctamente. Si ocurre un fallo en la comunicación
 * o los valores no coinciden, se activa el indicador de fallo (`Fail = TRUE`).
 *
 * @param[in,out] LTC6811   Puntero a la estructura del LTC6811 que contiene configuración
 *                          y estado del dispositivo.
 */
void LTC_Active_Odd_Balancing(uint8_t n_ic, cell_asic* LTC6811) 
{
    bool dcc[12] = {false};

    for (int i = 0; i < 12; i += 2)  // índices 0, 2, 4, 6, 8, 10 ? C1, C3, ..., C11
        dcc[i] = true;

    LTC6811_set_cfgr_dis(LTC6811, dcc);
    LTC6811_wrcfg(n_ic, LTC6811);
}
	
/*******************************************************************************
********************************************************************************
***************								Desactivar Balanceo				     		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Desactiva el balanceo de celdas en un LTC6811.
 *
 * Esta función limpia los bits DCC (Discharge Cell Control) en la configuración del LTC6811
 * para detener cualquier balanceo activo. También actualiza el estado interno `Balancing` y 
 * verifica que la configuración haya sido aplicada correctamente leyendo de vuelta los registros.
 *
 * Si se detecta un error de comunicación o un fallo al desactivar completamente el balanceo,
 * se establece el flag `Fail` dentro de la estructura del LTC.
 *
 * @param[in,out] LTC6811   Puntero a la estructura del LTC6811 que contiene la configuración
 *                          y estado actual del dispositivo.
 */
void LTC_Disable_Balancing(uint8_t n_ic, cell_asic* LTC6811)
{
	    bool dcc[12] = {false};

    LTC6811_set_cfgr_dis(LTC6811, dcc);
    LTC6811_wrcfg(n_ic, LTC6811);
}


/*******************************************************************************
********************************************************************************
***************									LTC Measure 							     	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Mide las temperaturas y voltajes de las celdas conectadas a dos LTC6811
 * 
 * Esta función activa el balanceo de celdas de manera alterna (pares/impares), 
 * mide los voltajes de las 12 celdas por LTC, convierte las lecturas a temperaturas 
 * (para las celdas pares en una pasada y las impares en la otra) y almacena 
 * los resultados en los arreglos TempArray y VoltajeArray.
 *
 * @param[in,out] Control_Unit   Puntero a la unidad de control que contiene el estado de los dos LTC6811.
 * @param[out]    TempArray      Arreglo de 24 elementos donde se guardan temperaturas codificadas. 
 *                               Las primeras 12 son del LTC6811_1, las últimas 12 del LTC6811_2.
 * @param[out]    VoltajeArray   Arreglo de 24 elementos donde se guardan voltajes. 
 *                               Misma distribución que TempArray.
 */
void LTC6811_Measure_Temperatures_and_Voltages(Control_Unit_TypeDef* Control_Unit)
{
	
		int8_t error_1=0;
		int8_t error_2=0;
		uint8_t intentos_1=0;
		uint8_t intentos_2=0;

    // Despierta ambos LTC6811
    wakeup_idle(1);
    wakeup_idle(2);
    HAL_Delay(1); 

    // === Medición de celdas pares ===
    // Activa el balanceo de celdas pares
    LTC_Active_Even_Balancing(1,&Control_Unit->Status.LTC6811_1);
    LTC_Active_Even_Balancing(2,&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3); // Permite estabilización

    // Inicia conversión ADC en ambos LTC
    LTC6811_adcv(1,MD_7KHZ_3KHZ,DCP_ENABLED,CELL_CH_ALL);
    LTC6811_adcv(2,MD_7KHZ_3KHZ,DCP_ENABLED,CELL_CH_ALL);
    HAL_Delay(3); // Espera la finalización de la conversión

    // Lee los voltajes
    error_1=LTC6811_rdcv(1,CELL_CH_ALL,&Control_Unit->Status.LTC6811_1);
    error_2=LTC6811_rdcv(2,CELL_CH_ALL,&Control_Unit->Status.LTC6811_2);
		
		//Si ha fallado algo volvemos a medir una vez
		while(error_1 && intentos_1<=2)
		{
				intentos_1++;
				error_1=LTC6811_rdcv(1,CELL_CH_ALL,&Control_Unit->Status.LTC6811_1);
		}
		
		while(error_2 && intentos_2<=2)
		{
				intentos_2++;
				error_1=LTC6811_rdcv(1,CELL_CH_ALL,&Control_Unit->Status.LTC6811_1);
		}
		
		//Si ha vuelto a fallar damos fallo
		if(error_1 || error_2)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
			LTC_Disable_Balancing(1,&Control_Unit->Status.LTC6811_1);
			LTC_Disable_Balancing(2,&Control_Unit->Status.LTC6811_2);
			return;
		}
		intentos_1=0;
		intentos_2=0;
		
    // Convertir voltajes pares a temperaturas (índices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(Control_Unit->Status.LTC6811_1.cells.c_codes[i]*0.0001);
        Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(Control_Unit->Status.LTC6811_2.cells.c_codes[i]*0.0001);
    }

    // Guardar voltajes de celdas impares (índices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = Control_Unit->Status.LTC6811_1.cells.c_codes[i]*0.0001;
        Control_Unit->Status.Voltages[i + 12] = Control_Unit->Status.LTC6811_2.cells.c_codes[i]*0.0001;
    }

		
    // Desactiva balanceo
    LTC_Disable_Balancing(1,&Control_Unit->Status.LTC6811_1);
    LTC_Disable_Balancing(2,&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3);


		
    // === Medición de celdas impares ===
		// Activa el balanceo de celdas impares
    LTC_Active_Odd_Balancing(1,&Control_Unit->Status.LTC6811_1);
    LTC_Active_Odd_Balancing(2,&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3);

		// Inicia conversión ADC en ambos LTC
    LTC6811_adcv(1,MD_7KHZ_3KHZ,DCP_ENABLED,CELL_CH_ALL);
    LTC6811_adcv(2,MD_7KHZ_3KHZ,DCP_ENABLED,CELL_CH_ALL);
    HAL_Delay(3); // Espera la finalización de la conversión

    //Si ha fallado algo volvemos a medir tres veces
		while(error_1 && intentos_1<=2)
		{
				intentos_1++;
				error_1=LTC6811_rdcv(1,CELL_CH_ALL,&Control_Unit->Status.LTC6811_1);
		}
		
		while(error_2 && intentos_2<=2)
		{
				intentos_2++;
				error_1=LTC6811_rdcv(1,CELL_CH_ALL,&Control_Unit->Status.LTC6811_1);
		}
		
		//Si ha vuelto a fallar damos fallo
		if(error_1 || error_2)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
			LTC_Disable_Balancing(1,&Control_Unit->Status.LTC6811_1);
			LTC_Disable_Balancing(2,&Control_Unit->Status.LTC6811_2);
			return;
		}

    // Convertir voltajes impares a temperatura (índices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(Control_Unit->Status.LTC6811_1.cells.c_codes[i]*0.0001);
        Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(Control_Unit->Status.LTC6811_2.cells.c_codes[i]*0.0001);
    }

    // Guardar voltajes de celdas pares (índices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = Control_Unit->Status.LTC6811_1.cells.c_codes[i]*0.0001;
        Control_Unit->Status.Voltages[i + 12] = Control_Unit->Status.LTC6811_2.cells.c_codes[i]*0.0001;
    }

		
		
    // Desactiva balanceo al final (por seguridad)
    LTC_Disable_Balancing(1,&Control_Unit->Status.LTC6811_1);
    LTC_Disable_Balancing(2,&Control_Unit->Status.LTC6811_2);
		
		
}


