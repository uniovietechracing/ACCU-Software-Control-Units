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
***************									LTC6811 Init      	  			   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_SPI_Init()
{	
	MCU_SPI1_Init();
	MCU_SPI2_Init();
}


/*******************************************************************************
********************************************************************************
***************									LTC6811 TIMERS Init      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Init(Control_Unit_TypeDef* Control_Unit)
{		
	Control_Unit->Status.LTC6811_1.SPI_Handler=&STM32_SPI1;
	Control_Unit->Status.LTC6811_1.CS_PORT=GPIOA;
	Control_Unit->Status.LTC6811_1.CS_PIN=GPIO_PIN_4;
	
	Control_Unit->Status.LTC6811_2.SPI_Handler=&STM32_SPI2;
	Control_Unit->Status.LTC6811_2.CS_PORT=GPIOA;
	Control_Unit->Status.LTC6811_2.CS_PIN=GPIO_PIN_15;
	
	Control_Unit->Status.LTC6811_1.Fail=FALSE;
	Control_Unit->Status.LTC6811_2.Fail=FALSE;
	
	// Despierta ambos LTC6811
  LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_1);
  LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_2);
  HAL_Delay(1);

  // Desactiva balanceo al final (por seguridad)
  LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
  LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
		
	if(Control_Unit->Status.LTC6811_1.Fail==TRUE || Control_Unit->Status.LTC6811_2.Fail==TRUE)
	{
		Control_Unit->State=LTC6811_FAIL_MODE;
	}	
}




/*******************************************************************************
********************************************************************************
***************							 	Float Volt to UINT8      	  	    ***************	
********************************************************************************
*******************************************************************************/
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
uint8_t LTC6811_Enconde_Temp(float Temp) 
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
***************								SPI Send						      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_SPI_Transfer(LTC6811_Typdef* LTC6811,uint8_t *tx, uint16_t len) 
{
	HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);
	HAL_StatusTypeDef Status;
	Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len, SPI_MAX_DELAY);  // Timeout de 100 ms
	HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

	if (Status != HAL_OK) 
	{
			LTC6811->Fail=TRUE;
	}
}

void LTC6811_SPI_Transfer_No_CS(LTC6811_Typdef* LTC6811,uint8_t *tx, uint16_t len) 
{
	HAL_StatusTypeDef Status;
	Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len, SPI_MAX_DELAY);  // Timeout de 100 ms

	if (Status != HAL_OK) 
	{
     LTC6811->Fail=TRUE;
	}
}
/*******************************************************************************
********************************************************************************
***************								SPI Send and Receive						 	 ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_SPI_Transmit_Receive(LTC6811_Typdef* LTC6811, uint8_t *tx, uint8_t *rx, uint16_t len_tx, uint16_t len_rx)
{
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);
		HAL_StatusTypeDef Status;

		Status=HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len_tx, SPI_MAX_DELAY);
    Status=HAL_SPI_Receive(LTC6811->SPI_Handler, rx, len_rx, SPI_MAX_DELAY);

    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

    if (Status != HAL_OK)
    {
        LTC6811->Fail=TRUE;
    }
}

/*******************************************************************************
********************************************************************************
***************								  	 PEC Calc			 					 	 	 	 ***************	
********************************************************************************
*******************************************************************************/
uint16_t LTC6811_PEC15_Calc(uint8_t *data, uint8_t len) {
    uint16_t remainder = 16; // PEC seed
    uint16_t POLY = 0x4599; // PEC polynomial

    for (uint8_t i = 0; i < len; i++) {
        remainder ^= (uint16_t)data[i] << 7;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (remainder & 0x4000) {
                remainder = (remainder << 1) ^ POLY;
            } else {
                remainder <<= 1;
            }
        }
    }
    return (remainder >> 1);
}
/*******************************************************************************
********************************************************************************
***************								Wake Up						      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Wake_Up(LTC6811_Typdef* LTC6811) 
{
    LTC6811_SPI_Transfer(LTC6811, 0x00, 1);
}

/*******************************************************************************
********************************************************************************
***************								WRITE CFG					      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Write_CFG(LTC6811_Typdef* LTC6811) {
    // Comando WRCFG con su PEC15 ya calculado
    uint8_t cmd[4] = { 0x00, 0x01, 0x3D, 0x6E }; 

    // Calculamos PEC15 para los 6 bytes de configuración
    uint16_t pec = LTC6811_PEC15_Calc(LTC6811->Config, 6);

    // Preparamos paquete de 6 bytes + 2 de PEC
    uint8_t config_pec[8];
    memcpy(config_pec, LTC6811->Config, 6);
    config_pec[6] = (pec >> 8) & 0xFF;
    config_pec[7] = pec & 0xFF;
		
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);
    // Enviar comando WRCFG
    LTC6811_SPI_Transfer_No_CS(LTC6811, cmd, 4);
    // Enviar datos de configuración + PEC
    LTC6811_SPI_Transfer_No_CS(LTC6811, config_pec, 8);
		HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);
}

/*******************************************************************************
********************************************************************************
***************								START ADC				      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Start_ADC_Conv(LTC6811_Typdef* LTC6811) {
    uint8_t cmd[4] = { 0x03, 0x60, 0xF4, 0x6C }; // ADCV: All cells
    LTC6811_SPI_Transfer(LTC6811, cmd, 4);
}

/*******************************************************************************
********************************************************************************
***************								Activar balanceo par				      	  ***************	
********************************************************************************
*******************************************************************************/
void LTC_Active_Even_Balancing(LTC6811_Typdef* LTC6811) 
	{
		if(LTC6811->Balancing!=EVEN_BALANCING)
		{
			memset(LTC6811->Config, 0, 6);
			LTC6811->Config[0] = 0b00000001; // REFON = 1, ADCOPT = 0
			LTC6811->Config[4] = 0b10101010; // DCC bits pares: 2,4,6,8,10,12
			LTC6811->Config[5] = 0b00001010;
			LTC6811_Write_CFG(LTC6811);
			LTC6811->Balancing=EVEN_BALANCING;
		}
		else
		{
        LTC6811->Fail=TRUE;
		}
}
	
/*******************************************************************************
********************************************************************************
***************								Activar balanceo IMpar				     ***************	
********************************************************************************
*******************************************************************************/
void LTC_Active_Odd_Balancing(LTC6811_Typdef* LTC6811) 
	{
		if(LTC6811->Balancing!=ODD_BALANCING)
		{
			memset(LTC6811->Config, 0, 6);
			LTC6811->Config[0] = 0b00000001; // REFON = 1, ADCOPT = 0
			LTC6811->Config[4] = 0b01010101; // DCC bits pares: 2,4,6,8,10,12
			LTC6811->Config[5] = 0b00000101;
			LTC6811_Write_CFG(LTC6811);
			LTC6811->Balancing=ODD_BALANCING;
		}
		else
		{
        LTC6811->Fail=TRUE;
		}
}
	
/*******************************************************************************
********************************************************************************
***************								Desactivar Balanceo				     		 ***************	
********************************************************************************
*******************************************************************************/
void LTC_Disable_Balancing(LTC6811_Typdef* LTC6811)
{
    if (LTC6811->Balancing != NO_BALANCING)
    {
        memset(LTC6811->Config, 0, 6);
        LTC6811->Config[0] = 0b00000000; // REFON = 0, ADCOPT = 0
        // DCC bits en 0 => desactiva todo el balanceo
        LTC6811_Write_CFG(LTC6811);
        LTC6811->Balancing = NO_BALANCING;
    }
		else
		{
        LTC6811->Fail=TRUE;
		}
			
}

/*******************************************************************************
********************************************************************************
***************								Read Cell Block				     		 ***************	
********************************************************************************
*******************************************************************************/
BoolTypeDef LTC6811_Read_Cell_Block(LTC6811_Typdef* LTC6811,uint8_t cmd1, uint8_t cmd2, uint16_t *cell_voltages) {
    uint8_t tx[2] = {cmd1, cmd2};
    uint8_t rx[8] = {0}; // 6 data + 2 PEC

    uint16_t pec = LTC6811_PEC15_Calc(tx, 2);
    uint8_t cmd[4] = {cmd1, cmd2, (uint8_t)(pec >> 8), (uint8_t)(pec & 0xFF)};

		LTC6811_SPI_Transmit_Receive(LTC6811,cmd,rx,sizeof(cmd),sizeof(rx));

    // Verificar PEC si quieres aquí
		uint16_t received_pec = (rx[6] << 8) | rx[7];
    uint16_t calc_pec = LTC6811_PEC15_Calc(rx, 6); // PEC de los primeros 6 bytes

    if(received_pec != calc_pec) {
        // Error de comunicación
        return 0;
    }
    // Extraer tres voltajes
    cell_voltages[0] = rx[0] | (rx[1] << 8); // Cx
    cell_voltages[1] = rx[2] | (rx[3] << 8); // Cx+1
    cell_voltages[2] = rx[4] | (rx[5] << 8); // Cx+2
		return 1;
}
/*******************************************************************************
********************************************************************************
***************									Read_Voltages				     		 		***************	
********************************************************************************
*******************************************************************************/
void LTC_Read_All_Voltages(LTC6811_Typdef *LTC6811, uint16_t *voltages) 
{
    uint16_t buf[3];

    LTC6811_Start_ADC_Conv(LTC6811);

    // RDCVA: C1, C2, C3
    if(LTC6811_Read_Cell_Block(LTC6811,0x04, 0x00, buf)) {
        voltages[0] = buf[0] * 0.0001f; // C1
        voltages[1] = buf[1] * 0.0001f; // C2
        voltages[2] = buf[2] * 0.0001f; // C3
    }

    // RDCVB: C4, C5, C6
    if(LTC6811_Read_Cell_Block(LTC6811,0x06, 0x00, buf)) {
        voltages[3] = buf[0] * 0.0001f; // C4
        voltages[4] = buf[1] * 0.0001f; // C5
        voltages[5] = buf[2] * 0.0001f; // C6
    }

    // RDCVC: C7, C8, C9
    if(LTC6811_Read_Cell_Block(LTC6811,0x08, 0x00, buf)) {
        voltages[6] = buf[0] * 0.0001f; // C7
        voltages[7] = buf[1] * 0.0001f; // C8
        voltages[8] = buf[2] * 0.0001f; // C9
    }

    // RDCVD: C10, C11, C12
    if(LTC6811_Read_Cell_Block(LTC6811,0x0A, 0x00, buf)) {
        voltages[9] = buf[0] * 0.0001f;  // C10
        voltages[10] = buf[1] * 0.0001f; // C11
        voltages[11] = buf[2] * 0.0001f; // C12
    }
}


/*******************************************************************************
********************************************************************************
***************									LTC Measure (UNUSED)			     	 ***************	
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
    uint16_t voltages_1[12]; // Voltajes de las 12 celdas del LTC6811_1
    uint16_t voltages_2[12]; // Voltajes de las 12 celdas del LTC6811_2

    // Despierta ambos LTC6811
    LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_1);
    LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(1); 

    // === Medición de celdas pares ===
    // Activa el balanceo de celdas pares
    LTC_Active_Even_Balancing(&Control_Unit->Status.LTC6811_1);
    LTC_Active_Even_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5); // Permite estabilización

    // Inicia conversión ADC en ambos LTC
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_1);
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3); // Espera la finalización de la conversión

    // Lee los voltajes
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_1, voltages_1);
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_2, voltages_2);

    // Convertir voltajes pares a temperaturas (índices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(voltages_1[i]);
        Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(voltages_2[i]);
    }

    // Guardar voltajes de celdas impares (índices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = voltages_1[i];
        Control_Unit->Status.Voltages[i + 12] = voltages_2[i];
    }

		
    // Desactiva balanceo
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5);

		
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
		}
		
    // === Medición de celdas impares ===
		// Activa el balanceo de celdas impares
    LTC_Active_Odd_Balancing(&Control_Unit->Status.LTC6811_1);
    LTC_Active_Odd_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5);

		// Inicia conversión ADC en ambos LTC
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_1);
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3);

    // Lee los voltajes
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_1, voltages_1);
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_2, voltages_2);

    // Convertir voltajes impares a temperatura (índices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(voltages_1[i]);
        Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(voltages_2[i]);
    }

    // Guardar voltajes de celdas pares (índices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = voltages_1[i];
        Control_Unit->Status.Voltages[i + 12] = voltages_2[i];
    }

		
		
    // Desactiva balanceo al final (por seguridad)
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
		
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
		}
}


