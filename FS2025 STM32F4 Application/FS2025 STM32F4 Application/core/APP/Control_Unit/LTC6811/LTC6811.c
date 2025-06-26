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
	// Espera corta antes de transmisión SPI
	for (volatile int i = 0; i < 500; i++) __NOP();
	HAL_StatusTypeDef Status;
	Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len, SPI_MAX_DELAY);  // Timeout de 100 ms
	
	// Espera corta antes de liberar CS
	for (volatile int i = 0; i < 500; i++) __NOP();
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
		// Espera corta antes de transmisión SPI
		for (volatile int i = 0; i < 500; i++) __NOP();
	
		HAL_StatusTypeDef Status,Status1;

		Status=HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len_tx, SPI_MAX_DELAY);
    Status1=HAL_SPI_Receive(LTC6811->SPI_Handler, rx, len_rx, SPI_MAX_DELAY);
	
		// Espera corta antes de liberar CS
		for (volatile int i = 0; i < 500; i++) __NOP();
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

    if (Status != HAL_OK || Status1!=HAL_OK)
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

BoolTypeDef LTC6811_Read_CFG(LTC6811_Typdef* LTC6811, uint8_t* config_out)
{
    uint8_t cmd[4] = { 0x02, 0x00, 0xB2, 0x2B }; // RDCFG + PEC15
    uint8_t rx[8] = {0};

    LTC6811_SPI_Transmit_Receive(LTC6811, cmd, rx, 4, 8);

    // Verificación de PEC recibido
    uint16_t received_pec = (rx[6] << 8) | rx[7];
    uint16_t calc_pec = LTC6811_PEC15_Calc(rx, 6);

    if (received_pec != calc_pec) {
        return FALSE;
    }

    // Copia configuración si todo está bien
    memcpy(config_out, rx, 6);
    return TRUE;
}
/*******************************************************************************
********************************************************************************
***************								Wake Up						      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Wake_Up(LTC6811_Typdef* LTC6811) 
{
    uint8_t wake_frame[2] = { 0x00, 0x00 }; // 2 bytes a 0 para asegurar actividad
    LTC6811_SPI_Transfer(LTC6811, wake_frame, 2);
		
		// Intenta leer el registro de configuración
    uint8_t cfg[6];
    if (LTC6811_Read_CFG(LTC6811, cfg)==FALSE) 
		{
        LTC6811->Fail = TRUE;
		}

}

/*******************************************************************************
********************************************************************************
***************								WRITE CFG					      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Write_CFG(LTC6811_Typdef* LTC6811) 
{
    // Paso 1: Calcula PEC15 de los 6 bytes de configuración
    uint16_t pec = LTC6811_PEC15_Calc(LTC6811->Config, 6);

    // Paso 2: Arma el paquete completo: WRCFG (4) + Config (6) + PEC (2) = 12 bytes
    uint8_t tx[12];

    // Comando WRCFG con su PEC15
    tx[0] = 0x00;
    tx[1] = 0x01;
    tx[2] = 0x3D; // PEC15 MSB para 0x0001
    tx[3] = 0x6E; // PEC15 LSB

    // Configuración
    memcpy(&tx[4], LTC6811->Config, 6);

    // PEC15 de configuración
    tx[10] = (pec >> 8) & 0xFF;
    tx[11] = pec & 0xFF;

    // Transmisión con CS bajo durante todo el mensaje
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);
    for (volatile int i = 0; i < 500; i++) __NOP();

    HAL_StatusTypeDef status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, 12, HAL_MAX_DELAY);

    for (volatile int i = 0; i < 500; i++) __NOP();
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

    if (status != HAL_OK)
		{
        LTC6811->Fail = TRUE;
		}

}


/*******************************************************************************
********************************************************************************
***************										wait ADC				      	  	   ***************	
********************************************************************************
*******************************************************************************/
BoolTypeDef LTC6811_Wait_ADC_Completion(LTC6811_Typdef* LTC6811, uint16_t timeout_ms)
{
    uint8_t cmd[4] = { 0x07, 0x14, 0x65, 0xAF }; // PLADC + PEC15
    uint8_t response[4] = {0};
    uint8_t dummy_tx[4] = {0x00, 0x00, 0x00, 0x00};

    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < timeout_ms)
    {
        // Enviar comando PLADC y leer 4 bytes
        LTC6811_SPI_Transmit_Receive(LTC6811, cmd, response, 4, 4);

        // response[0] = status byte (otros son padding y PEC)
        if (response[0] & 0x01)
            return TRUE; // ADC terminó
    }

    return FALSE; // timeout
}
/*******************************************************************************
********************************************************************************
***************								START ADC				      	  	   ***************	
********************************************************************************
*******************************************************************************/
void LTC6811_Start_ADC_Conv(LTC6811_Typdef* LTC6811) {
    uint8_t cmd[4] = { 0x03, 0x60, 0xF4, 0x6C }; // ADCV: All cells
    LTC6811_SPI_Transfer(LTC6811, cmd, 4);
		
		if (!LTC6811_Wait_ADC_Completion(LTC6811, 5)) { // espera hasta 5 ms
        LTC6811->Fail = TRUE; // ADC no respondió a tiempo
    }
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
			
			// Confirmación con RDCFG
        uint8_t read_cfg[6];
        if (LTC6811_Read_CFG(LTC6811, read_cfg)) {
            if (read_cfg[4] == 0xAA && (read_cfg[5] & 0x0F) == 0x0A) {
                LTC6811->Balancing = EVEN_BALANCING;
            } else {
                LTC6811->Fail = TRUE;
            }
        } else {
            LTC6811->Fail = TRUE;
        }
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
			LTC6811->Config[4] = 0b01010101; // DCC bits impares
			LTC6811->Config[5] = 0b00000101;
			LTC6811_Write_CFG(LTC6811);
			LTC6811->Balancing=ODD_BALANCING;
			
			// Confirmar que se aplicó correctamente
        uint8_t read_cfg[6];
        if (LTC6811_Read_CFG(LTC6811, read_cfg)) {
            if (read_cfg[4] == 0x55 && (read_cfg[5] & 0x0F) == 0x05) {
                LTC6811->Balancing = ODD_BALANCING;
            } else {
                LTC6811->Fail = TRUE;
            }
        } else {
            LTC6811->Fail = TRUE;
        }		
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
			
			// Paso 2: Leer de vuelta para verificar que se aplicó correctamente
        uint8_t read_cfg[6] = {0};
        if (LTC6811_Read_CFG(LTC6811, read_cfg))
        {
            // Comprobamos que DCC bits (bytes 4 y 5) están en 0
            if (read_cfg[4] == 0x00 && (read_cfg[5] & 0x0F) == 0x00) {
                // Confirmación OK
                LTC6811->Balancing = NO_BALANCING;
            } else {
                // DCC no está apagado ? posible fallo
                LTC6811->Fail = TRUE;
            }
        }
        else {
            // Error de comunicación
            LTC6811->Fail = TRUE;
        }
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
void LTC_Read_All_Voltages(LTC6811_Typdef *LTC6811, float *voltages) 
{
    const struct {
        uint8_t cmd1;
        uint8_t cmd2;
        uint8_t index;
    } blocks[] = {
        {0x04, 0x00, 0}, // RDCVA: C1–C3
        {0x06, 0x00, 3}, // RDCVB: C4–C6
        {0x08, 0x00, 6}, // RDCVC: C7–C9
        {0x0A, 0x00, 9}  // RDCVD: C10–C12
    };

    uint16_t buf[3];

    for (int i = 0; i < 4; i++) {
        BoolTypeDef success = FALSE;

        // Hasta 2 intentos
        for (int attempt = 0; attempt < 2; attempt++) {
            if (LTC6811_Read_Cell_Block(LTC6811, blocks[i].cmd1, blocks[i].cmd2, buf)) {
                voltages[blocks[i].index + 0] = buf[0] * 0.0001f;
                voltages[blocks[i].index + 1] = buf[1] * 0.0001f;
                voltages[blocks[i].index + 2] = buf[2] * 0.0001f;
                success = TRUE;
                break;
            }
        }

        if (!success) {
            LTC6811->Fail = TRUE;
        }
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
    float voltages_1[12]; // Voltajes de las 12 celdas del LTC6811_1
    float voltages_2[12]; // Voltajes de las 12 celdas del LTC6811_2

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

		//Si ha fallado algo dejamos de medir
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
			return;
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
		
		
		//Si ha fallado algo dejamos de medir
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
		}
}


