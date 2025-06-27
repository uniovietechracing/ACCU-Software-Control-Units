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
    1.0   | 01 MAR 2023 | Diego Rodr�guez  | Creation
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
// Tabla de conversi�n V -> �C (Vout de sensor vs temperatura)
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
/**
 * @brief Inicializa las interfaces SPI utilizadas por los dispositivos LTC6811.
 *
 * Esta funci�n configura los perif�ricos SPI del microcontrolador asociados
 * a los dos LTC6811 (por ejemplo, `STM32_SPI1` y `STM32_SPI2`), llamando a las
 * funciones de inicializaci�n correspondientes del MCU.
 *
 * Debe ser llamada antes de cualquier operaci�n de comunicaci�n SPI con los LTC6811.
 */
void LTC6811_SPI_Init()
{
    MCU_SPI1_Init();  ///< Inicializa SPI1 (asociado al LTC6811_1)
    //MCU_SPI2_Init();  ///< Inicializa SPI2 (asociado al LTC6811_1)
}


/*******************************************************************************
********************************************************************************
***************									LTC6811 TIMERS Init      	  	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicializa los dos dispositivos LTC6811 en la unidad de control.
 *
 * Esta funci�n configura los pines y handlers SPI para los dos LTC6811 conectados
 * a la unidad de control, los despierta del modo de bajo consumo y desactiva 
 * el balanceo de celdas como medida de seguridad inicial.
 *
 * Tambi�n verifica si alguno de los dispositivos fall� durante la inicializaci�n
 * (por ejemplo, error de comunicaci�n), y si es as�, cambia el estado global 
 * de la unidad de control a `LTC6811_FAIL_MODE`.
 *
 * @param[in,out] Control_Unit  Puntero a la estructura principal de control del sistema.
 */
void LTC6811_Init(Control_Unit_TypeDef* Control_Unit)
{
    // Asignar configuraci�n SPI y pines CS para LTC6811_1
    Control_Unit->Status.LTC6811_1.SPI_Handler = &STM32_SPI1;
    Control_Unit->Status.LTC6811_1.CS_PORT     = GPIOA;
    Control_Unit->Status.LTC6811_1.CS_PIN      = GPIO_PIN_4;

    // Asignar configuraci�n SPI y pines CS para LTC6811_2
    //Control_Unit->Status.LTC6811_2.SPI_Handler = &STM32_SPI2;
    //Control_Unit->Status.LTC6811_2.CS_PORT     = GPIOA;
    //Control_Unit->Status.LTC6811_2.CS_PIN      = GPIO_PIN_15;

    // Resetear indicadores de fallo
    Control_Unit->Status.LTC6811_1.Fail = FALSE;
    //Control_Unit->Status.LTC6811_2.Fail = FALSE;

    // Despertar ambos LTC6811
    LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_1);
    //LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(1); // Peque�a espera para estabilizaci�n

    // Desactivar balanceo de celdas en ambos dispositivos
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
    //LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
}




/*******************************************************************************
********************************************************************************
***************							 	Float Volt to UINT8      	  	     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Codifica un voltaje en un valor entero de 8 bits con resoluci�n de 10 mV.
 *
 * Esta funci�n convierte un voltaje flotante en el rango [2.00 V, 4.55 V] a un valor
 * entero de 8 bits, con una resoluci�n de 10 mV por unidad (0.01 V).
 *
 * Si el voltaje est� fuera del rango, se limita (clampa) al valor m�s cercano permitido.
 * Se aplica redondeo para mejorar la precisi�n de la cuantizaci�n.
 *
 * F�rmula de codificaci�n:
 * - `0` representa 2.00 V
 * - `1` representa 2.01 V
 * - ...
 * - `255` representa 4.55 V
 *
 * @param[in] volt  Voltaje en voltios a codificar.
 * @return Valor codificado como uint8_t (resoluci�n de 10 mV).
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
 * Esta funci�n convierte una temperatura en grados Celsius a un valor codificado 
 * de 8 bits (uint8_t), que representa temperaturas desde -20.0 �C hasta 107.5 �C 
 * con una resoluci�n de 0.5 �C por unidad.
 *
 * Si la temperatura est� fuera del rango [-20.0, 107.5], se limita al valor m�s cercano.
 * Se aplica un redondeo para que los pasos de 0.5 �C se traduzcan correctamente.
 *
 * F�rmula de codificaci�n:
 * - `0` representa -20.0��C
 * - `1` representa -19.5��C
 * - ...
 * - `255` representa 107.5��C
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
 * @brief Convierte un voltaje medido a temperatura usando interpolaci�n lineal.
 *
 * Esta funci�n busca convertir un valor de voltaje a su correspondiente temperatura
 * estimada utilizando dos tablas (`Temp_Table_V` y `Temp_Table_C`) que representan
 * una relaci�n voltaje-temperatura.
 *
 * Se realiza una b�squeda secuencial en la tabla de voltajes (`Temp_Table_V`)
 * para encontrar el intervalo donde se encuentra el voltaje `v`, y luego se interpola
 * linealmente entre los puntos adyacentes para calcular la temperatura.
 *
 * Si el voltaje est� fuera del rango de la tabla, se retorna -100.0 como indicador de error.
 *
 * @param[in] v  Voltaje en voltios a convertir.
 * @return Temperatura estimada en grados Celsius, o -100.0 si est� fuera de rango.
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
***************								SPI Send						      	  	   ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Transmite datos por SPI al LTC6811 controlando autom�ticamente el pin CS.
 *
 * Esta funci�n activa el pin CS (baj�ndolo), transmite un bloque de datos al LTC6811
 * mediante SPI, y luego libera el CS (lo vuelve a subir). Se insertan peque�os retardos
 * antes y despu�s de la transmisi�n para cumplir con los requisitos temporales del LTC6811.
 *
 * En caso de error durante la transmisi�n SPI, se marca la estructura `LTC6811` como fallida
 * estableciendo el campo `Fail = TRUE`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura del LTC6811 con el handler SPI y pines asociados.
 * @param[in]     tx       Puntero al arreglo de datos que se desea transmitir.
 * @param[in]     len      Longitud del arreglo a transmitir, en bytes.
 */
void LTC6811_SPI_Transfer(LTC6811_Typdef* LTC6811, uint8_t *tx, uint16_t len) 
{
    // Activar CS (bajo)
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);

    // Peque�o retardo antes de la transmisi�n SPI
    for (volatile int i = 0; i < 500; i++) __NOP();

    // Enviar los datos por SPI
    HAL_StatusTypeDef Status;
    Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len, SPI_MAX_DELAY);

    // Peque�o retardo antes de liberar CS
    for (volatile int i = 0; i < 500; i++) __NOP();
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

    // Si hubo error, marcar el fallo en la estructura
    if (Status != HAL_OK) 
    {
        LTC6811->Fail = TRUE;
    }
}

/*******************************************************************************
********************************************************************************
***************								SPI Send no cs						      	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Transmite datos por SPI al LTC6811 sin controlar el pin CS (Chip Select).
 *
 * Esta funci�n transmite un arreglo de bytes a trav�s del bus SPI hacia el LTC6811,
 * pero no modifica el estado del pin CS. Se asume que CS ya est� en el estado correcto 
 * (activo en bajo) y que ser� manejado externamente por otra funci�n o secuencia.
 *
 * Es �til para operaciones donde el control de CS debe mantenerse a lo largo de m�ltiples transferencias.
 * Si la transmisi�n falla, se marca el campo `Fail` de la estructura `LTC6811` como `TRUE`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura del LTC6811 con el handler SPI.
 * @param[in]     tx       Puntero al buffer de datos a transmitir.
 * @param[in]     len      Cantidad de bytes a transmitir.
 */
void LTC6811_SPI_Transfer_No_CS(LTC6811_Typdef* LTC6811, uint8_t *tx, uint16_t len) 
{
    HAL_StatusTypeDef Status;

    // Transmitir datos v�a SPI (sin tocar el pin CS)
    Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len, SPI_MAX_DELAY);  // Espera indefinida (bloqueante)

    // Si hubo error durante la transmisi�n, marcar fallo
    if (Status != HAL_OK) 
    {
        LTC6811->Fail = TRUE;
    }
}
/*******************************************************************************
********************************************************************************
***************								SPI Send and Receive						 	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Realiza una transmisi�n SPI seguida de una recepci�n desde el LTC6811.
 *
 * Esta funci�n baja el pin CS (Chip Select), transmite un bloque de datos por SPI,
 * luego recibe un bloque de respuesta, y finalmente libera el CS. Se insertan
 * retardos breves antes y despu�s de la comunicaci�n para cumplir con los requisitos 
 * de tiempo del LTC6811.
 *
 * En caso de que falle la transmisi�n o la recepci�n (seg�n el estado de `HAL_SPI_Transmit`
 * o `HAL_SPI_Receive`), se marca el campo `Fail` de la estructura del LTC6811 como `TRUE`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura con configuraci�n del LTC6811 y su handler SPI.
 * @param[in]     tx       Puntero al buffer con los datos a transmitir.
 * @param[out]    rx       Puntero al buffer donde se almacenar� la respuesta recibida.
 * @param[in]     len_tx   Longitud del bloque a transmitir, en bytes.
 * @param[in]     len_rx   Longitud del bloque a recibir, en bytes.
 */
void LTC6811_SPI_Transmit_Receive(LTC6811_Typdef* LTC6811, uint8_t *tx, uint8_t *rx, uint16_t len_tx, uint16_t len_rx)
{
    // Activar CS (chip select bajo)
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);

    // Peque�o retardo antes de comenzar SPI
    for (volatile int i = 0; i < 500; i++) __NOP();

    // Transmitir datos por SPI
    HAL_StatusTypeDef Status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, len_tx, SPI_MAX_DELAY);
    // Recibir datos por SPI
    HAL_StatusTypeDef Status1 = HAL_SPI_Receive(LTC6811->SPI_Handler, rx, len_rx, SPI_MAX_DELAY);

    // Peque�o retardo antes de liberar CS
    for (volatile int i = 0; i < 500; i++) __NOP();
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET);

    // Marcar error si alguna operaci�n SPI fall�
    if (Status != HAL_OK || Status1 != HAL_OK)
    {
        LTC6811->Fail = TRUE;
    }
}

/*******************************************************************************
********************************************************************************
***************								  	 PEC Calc			 					 	 	 	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Calcula el c�digo PEC15 (Packet Error Code) para un arreglo de datos.
 *
 * Esta funci�n implementa el algoritmo de CRC-15 utilizado por el LTC6811 para proteger
 * los comandos y respuestas SPI. El c�lculo se basa en un polinomio est�ndar (0x4599)
 * y un seed inicial de 0x0010 (16 decimal).
 *
 * Se usa para verificar la integridad de los datos transmitidos o recibidos
 * (comandos, configuraci�n, lectura de celdas, etc.).
 *
 * @param[in]  data   Puntero al arreglo de datos sobre el cual se calcular� el PEC.
 * @param[in]  len    Longitud del arreglo de datos (en bytes).
 *
 * @return El c�digo PEC15 resultante (15 bits, retornado en un entero de 16 bits).
 */
uint16_t LTC6811_PEC15_Calc(uint8_t *data, uint8_t len) {
    uint16_t remainder = 16;     // Valor inicial del registro (seed = 0x0010)
    uint16_t POLY = 0x4599;      // Polinomio generador para PEC15

    for (uint8_t i = 0; i < len; i++) {
        remainder ^= (uint16_t)data[i] << 7; // Alinear el byte con el MSB del registro
        for (uint8_t bit = 0; bit < 8; bit++) {
            // Verifica si el bit m�s significativo (bit 14) est� en 1
            if (remainder & 0x4000) {
                remainder = (remainder << 1) ^ POLY; // Aplica XOR con el polinomio
            } else {
                remainder <<= 1; // Desplaza a la izquierda sin aplicar XOR
            }
        }
    }

    return (remainder >> 1); // El PEC15 es de 15 bits (descarta el LSB extra)
}

/*******************************************************************************
********************************************************************************
***************								Read Config					      	  	  ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Lee el registro de configuraci�n (CFGR) del LTC6811 y verifica la integridad con PEC.
 *
 * Esta funci�n env�a el comando `RDCFG` al LTC6811 para leer sus 6 bytes de configuraci�n.
 * Despu�s de recibir los 8 bytes (6 de datos + 2 de PEC), se calcula el c�digo de comprobaci�n
 * PEC15 sobre los datos recibidos y se compara con el PEC incluido en la respuesta.
 *
 * Si la verificaci�n es correcta, se copia la configuraci�n en `config_out`. Si el PEC no
 * coincide, se considera un error de comunicaci�n y no se copia nada.
 *
 * @param[in,out] LTC6811     Puntero a la estructura del LTC6811 con la configuraci�n de SPI.
 * @param[out]    config_out  Arreglo de 6 bytes donde se almacenar� la configuraci�n le�da.
 *
 * @retval TRUE   Si la lectura y verificaci�n PEC fueron exitosas.
 * @retval FALSE  Si la verificaci�n PEC fall� (posible corrupci�n de datos).
 */
BoolTypeDef LTC6811_Read_CFG(LTC6811_Typdef* LTC6811, uint8_t* config_out)
{
    uint8_t cmd[4] = { 0x02, 0x00, 0xB2, 0x2B }; // Comando RDCFG con su PEC15
    uint8_t rx[8] = {0};                         // 6 bytes de datos + 2 bytes de PEC

    // Enviar comando y recibir respuesta del LTC6811
    LTC6811_SPI_Transmit_Receive(LTC6811, cmd, rx, 4, 8);

    // Verificar PEC recibido
    uint16_t received_pec = (rx[6] << 8) | rx[7];
    uint16_t calc_pec = LTC6811_PEC15_Calc(rx, 6);

    if (received_pec != calc_pec) {
        // Error de verificaci�n de integridad
        return FALSE;
    }

    // Copiar la configuraci�n v�lida al arreglo de salida
    memcpy(config_out, rx, 6);
    return TRUE;
}
/*******************************************************************************
********************************************************************************
***************								Wake Up						      	  	  	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Despierta al LTC6811 del modo de bajo consumo (Sleep).
 *
 * Esta funci�n env�a un "wake-up frame" (dos bytes en cero) por SPI
 * para generar actividad en el bus, lo cual saca al LTC6811 de su modo 
 * de bajo consumo. Luego intenta verificar la comunicaci�n leyendo 
 * el registro de configuraci�n (CFGR).
 *
 * Si no se puede leer correctamente el registro CFGR, se marca
 * la estructura con `Fail = TRUE`, indicando un posible fallo en la activaci�n.
 *
 * @param[in,out] LTC6811  Puntero a la estructura del dispositivo LTC6811.
 */
void LTC6811_Wake_Up(LTC6811_Typdef* LTC6811) 
{
    uint8_t wake_frame[2] = { 0x00, 0x00 }; // Wake-up frame est�ndar (2 bytes a 0)
    LTC6811_SPI_Transfer(LTC6811, wake_frame, 2);    // Enviar el frame para generar actividad en el bus SPI

    // Verificar que el dispositivo est� activo leyendo su configuraci�n
    uint8_t cfg[6];
    if (LTC6811_Read_CFG(LTC6811, cfg) == FALSE) 
    {
        // Error en la comunicaci�n posterior al intento de activaci�n
        LTC6811->Fail = TRUE;
    }
}

/*******************************************************************************
********************************************************************************
***************								WRITE CFG					      	  	  	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Escribe la configuraci�n del LTC6811 (registro CFGR).
 *
 * Esta funci�n env�a el comando `WRCFG` al LTC6811 para escribir los 6 bytes
 * de configuraci�n del registro CFGR, seguidos de su correspondiente c�digo 
 * de verificaci�n PEC15. El comando se transmite con el pin CS en estado bajo 
 * durante toda la transferencia para cumplir con el protocolo del LTC6811.
 *
 * Si ocurre un error durante la transmisi�n SPI, se activa el indicador `Fail`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura que contiene la configuraci�n
 *                         y el manejador SPI del dispositivo LTC6811.
 */
void LTC6811_Write_CFG(LTC6811_Typdef* LTC6811) 
{
    // Paso 1: Calcular el PEC15 para los 6 bytes de configuraci�n
    uint16_t pec = LTC6811_PEC15_Calc(LTC6811->Config, 6);

    // Paso 2: Preparar el paquete completo de 12 bytes
    uint8_t tx[12];

    // Bytes de comando WRCFG con su PEC15 (fijo para 0x0001)
    tx[0] = 0x00;
    tx[1] = 0x01;
    tx[2] = 0x3D; // PEC15 MSB para 0x0001
    tx[3] = 0x6E; // PEC15 LSB

    // Copiar los 6 bytes de configuraci�n CFGR[0-5]
    memcpy(&tx[4], LTC6811->Config, 6);

    // A�adir PEC15 calculado para los datos de configuraci�n
    tx[10] = (pec >> 8) & 0xFF;
    tx[11] = pec & 0xFF;

    // Mantener CS en bajo durante toda la transmisi�n
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_RESET);
    for (volatile int i = 0; i < 500; i++) __NOP(); // peque�o retardo

    // Transmitir los 12 bytes por SPI
    HAL_StatusTypeDef status = HAL_SPI_Transmit(LTC6811->SPI_Handler, tx, 12, HAL_MAX_DELAY);

    for (volatile int i = 0; i < 500; i++) __NOP(); // retardo posterior
    HAL_GPIO_WritePin(LTC6811->CS_PORT, LTC6811->CS_PIN, GPIO_PIN_SET); // liberar CS

    // Si hubo error en la transmisi�n SPI, marcar fallo
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
/**
 * @brief Espera a que finalice la conversi�n ADC del LTC6811.
 *
 * Esta funci�n env�a repetidamente el comando `PLADC` al LTC6811 para consultar
 * el estado del ADC, dentro del l�mite de tiempo especificado en milisegundos.
 * El primer byte de la respuesta contiene un bit de estado: si el bit 0 est� en 1,
 * indica que la conversi�n ha finalizado.
 *
 * Si el bit no se activa antes de que expire el tiempo, se considera un fallo.
 *
 * @param[in,out] LTC6811      Puntero a la estructura de configuraci�n del LTC6811.
 * @param[in]     timeout_ms   Tiempo m�ximo de espera en milisegundos.
 *
 * @retval TRUE  Si el ADC complet� la conversi�n dentro del tiempo.
 * @retval FALSE Si se agot� el tiempo de espera sin que el ADC finalizara.
 */
BoolTypeDef LTC6811_Wait_ADC_Completion(LTC6811_Typdef* LTC6811, uint16_t timeout_ms)
{
    // Comando PLADC (Poll ADC), seguido de su CRC PEC15
    uint8_t cmd[4] = { 0x07, 0x14, 0x65, 0xAF };
    uint8_t response[4] = {0};             // Donde se almacenar� la respuesta
    uint8_t dummy_tx[4] = {0x00, 0x00, 0x00, 0x00}; // TX dummy para recibir datos

    uint32_t start = HAL_GetTick();        // Marcar el inicio del tiempo

    while ((HAL_GetTick() - start) < timeout_ms)
    {
        // Transmitir PLADC y recibir 4 bytes de respuesta
        LTC6811_SPI_Transmit_Receive(LTC6811, cmd, response, 4, 4);

        // Verificar el bit de estado en el primer byte
        if (response[0] & 0x01)
            return TRUE; // Conversi�n ADC completada
    }

    return FALSE; // Timeout alcanzado sin respuesta positiva
}
/*******************************************************************************
********************************************************************************
***************								START ADC				      	  	   		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Inicia una conversi�n ADC para todas las celdas en el LTC6811.
 *
 * Esta funci�n env�a el comando `ADCV` al LTC6811 para iniciar la conversi�n 
 * de todos los voltajes de celda (modo normal, todas las celdas al mismo tiempo).
 * Luego espera hasta 5 ms a que la conversi�n se complete utilizando la funci�n
 * `LTC6811_Wait_ADC_Completion`.
 *
 * Si el ADC no finaliza dentro del tiempo esperado, se marca la estructura del LTC
 * con el indicador de fallo (`Fail = TRUE`).
 *
 * @param[in,out] LTC6811  Puntero a la estructura del LTC6811 que contiene su estado y configuraci�n.
 */
void LTC6811_Start_ADC_Conv(LTC6811_Typdef* LTC6811) {
    // Comando ADCV (All cells, normal mode, discharge disabled)
    uint8_t cmd[4] = { 0x03, 0x60, 0xF4, 0x6C }; 

    // Enviar comando por SPI
    LTC6811_SPI_Transfer(LTC6811, cmd, 4);

    // Esperar hasta 5 ms por la finalizaci�n del ADC
    if (!LTC6811_Wait_ADC_Completion(LTC6811, 5)) {
        // Error: el ADC no respondi� a tiempo
        LTC6811->Fail = TRUE;
    }
}
/*******************************************************************************
********************************************************************************
***************								Activar balanceo par				     	 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Activa el balanceo de celdas pares en un LTC6811.
 *
 * Esta funci�n configura el LTC6811 para activar el balanceo de celdas pares 
 * (C2, C4, C6, C8, C10, C12) utilizando los bits DCC (Discharge Cell Control).
 * Tambi�n habilita el regulador de referencia interno (`REFON = 1`) necesario 
 * para operar el circuito de descarga.
 *
 * Despu�s de aplicar la configuraci�n, se verifica que haya sido correctamente 
 * escrita mediante una lectura de vuelta de los registros de configuraci�n.
 * Si la verificaci�n falla o no hay comunicaci�n, se marca la estructura con 
 * `Fail = TRUE`.
 *
 * @param[in,out] LTC6811  Puntero a la estructura de estado del LTC6811.
 */
void LTC_Active_Even_Balancing(LTC6811_Typdef* LTC6811) 
{
    // Verifica si ya est� activado el balanceo de pares
    if (LTC6811->Balancing != EVEN_BALANCING)
    {
        // Limpiar configuraci�n anterior
        memset(LTC6811->Config, 0, 6);

        // REFON = 1 permite que el regulador interno est� activo
        LTC6811->Config[0] = 0b00000001;

        // Activar balanceo para celdas pares: 2, 4, 6, 8, 10, 12
        LTC6811->Config[4] = 0b10101010; // Bits para C2 a C8
        LTC6811->Config[5] = 0b00001010; // Bits para C10 y C12

        // Aplicar la configuraci�n al LTC6811
        LTC6811_Write_CFG(LTC6811);
        LTC6811->Balancing = EVEN_BALANCING;

        // Verificaci�n de la configuraci�n con lectura RDCFG
        uint8_t read_cfg[6];
        if (LTC6811_Read_CFG(LTC6811, read_cfg)) {
            if (read_cfg[4] == 0xAA && (read_cfg[5] & 0x0F) == 0x0A) {
                LTC6811->Balancing = EVEN_BALANCING;
            } else {
                // DCC bits no coinciden: error
                LTC6811->Fail = TRUE;
            }
        } else {
            // Fallo en la lectura
            LTC6811->Fail = TRUE;
        }
    }
    else
    {
        // Ya estaba en modo balanceo par: marcar como fallo si se repite
        LTC6811->Fail = TRUE;
    }
}
	
/*******************************************************************************
********************************************************************************
***************								Activar balanceo IMpar				     ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Activa el balanceo de celdas impares en un LTC6811.
 *
 * Esta funci�n configura los bits DCC (Discharge Cell Control) para activar el balanceo
 * solo en las celdas impares (C1, C3, C5, ..., C11) del LTC6811. Tambi�n enciende el regulador 
 * interno (`REFON = 1`) para permitir la operaci�n del circuito de balanceo.
 *
 * Luego de escribir la configuraci�n, se realiza una lectura de verificaci�n para confirmar
 * que los bits DCC se hayan aplicado correctamente. Si ocurre un fallo en la comunicaci�n
 * o los valores no coinciden, se activa el indicador de fallo (`Fail = TRUE`).
 *
 * @param[in,out] LTC6811   Puntero a la estructura del LTC6811 que contiene configuraci�n
 *                          y estado del dispositivo.
 */
void LTC_Active_Odd_Balancing(LTC6811_Typdef* LTC6811) 
{
    // Solo configurar si no est� ya en modo de balanceo impar
    if (LTC6811->Balancing != ODD_BALANCING)
    {
        // Limpiar configuraci�n anterior
        memset(LTC6811->Config, 0, 6);

        // REFON = 1 para permitir el balanceo, ADCOPT = 0
        LTC6811->Config[0] = 0b00000001;

        // Activar DCC en celdas impares: C1, C3, C5, C7, C9, C11
        LTC6811->Config[4] = 0b01010101; // C1 a C8
        LTC6811->Config[5] = 0b00000101; // C9 a C12

        // Aplicar configuraci�n al LTC6811
        LTC6811_Write_CFG(LTC6811);
        LTC6811->Balancing = ODD_BALANCING;

        // Verificaci�n: leer configuraci�n de vuelta
        uint8_t read_cfg[6];
        if (LTC6811_Read_CFG(LTC6811, read_cfg)) {
            if (read_cfg[4] == 0x55 && (read_cfg[5] & 0x0F) == 0x05) {
                // Confirmado: configuraci�n aplicada
                LTC6811->Balancing = ODD_BALANCING;
            } else {
                // Bits DCC no coinciden: posible error
                LTC6811->Fail = TRUE;
            }
        } else {
            // Error de comunicaci�n al leer
            LTC6811->Fail = TRUE;
        }
    }
    else
    {
        // Ya estaba en balanceo impar: marcar como fallo si se intenta aplicar de nuevo
        LTC6811->Fail = TRUE;
    }
}
	
/*******************************************************************************
********************************************************************************
***************								Desactivar Balanceo				     		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Desactiva el balanceo de celdas en un LTC6811.
 *
 * Esta funci�n limpia los bits DCC (Discharge Cell Control) en la configuraci�n del LTC6811
 * para detener cualquier balanceo activo. Tambi�n actualiza el estado interno `Balancing` y 
 * verifica que la configuraci�n haya sido aplicada correctamente leyendo de vuelta los registros.
 *
 * Si se detecta un error de comunicaci�n o un fallo al desactivar completamente el balanceo,
 * se establece el flag `Fail` dentro de la estructura del LTC.
 *
 * @param[in,out] LTC6811   Puntero a la estructura del LTC6811 que contiene la configuraci�n
 *                          y estado actual del dispositivo.
 */
void LTC_Disable_Balancing(LTC6811_Typdef* LTC6811)
{
    // Solo procede si el balanceo est� activo
    if (LTC6811->Balancing != NO_BALANCING)
    {
        // Limpia la configuraci�n: desactiva REFON, ADCOPT y todos los DCC
        memset(LTC6811->Config, 0, 6);
        LTC6811->Config[0] = 0b00000000; // REFON = 0, ADCOPT = 0

        // Escribe la configuraci�n al LTC6811
        LTC6811_Write_CFG(LTC6811);

        // Marca el estado interno como sin balanceo
        LTC6811->Balancing = NO_BALANCING;

        // Paso 2: Leer la configuraci�n para verificar que se aplic� correctamente
        uint8_t read_cfg[6] = {0};
        if (LTC6811_Read_CFG(LTC6811, read_cfg))
        {
            // Verificar que los bits DCC (control de descarga) est�n en cero
            if (read_cfg[4] == 0x00 && (read_cfg[5] & 0x0F) == 0x00) {
                // Confirmaci�n: balanceo efectivamente desactivado
                LTC6811->Balancing = NO_BALANCING;
            } else {
                // Los bits DCC a�n est�n activos: posible error
                LTC6811->Fail = TRUE;
            }
        }
        else {
            // Fallo en la comunicaci�n al leer la configuraci�n
            LTC6811->Fail = TRUE;
        }
    }
    else
    {
        // Ya estaba sin balanceo, pero algo no cuadra: marcar fallo
        LTC6811->Fail = TRUE;
    }
}

/*******************************************************************************
********************************************************************************
***************								Read Cell Block				     		 		 ***************	
********************************************************************************
*******************************************************************************/
/**
 * @brief Lee un bloque de 3 voltajes de celda del LTC6811 mediante SPI.
 *
 * Esta funci�n env�a un comando de lectura de celdas (RDCVx) al LTC6811,
 * recibe 6 bytes de datos (tres voltajes de 2 bytes cada uno) y verifica
 * la integridad de los datos mediante el c�digo de detecci�n de errores PEC15.
 * Si el PEC recibido no coincide con el calculado, se considera un error
 * de comunicaci�n y no se actualiza el resultado.
 *
 * @param[in,out] LTC6811          Puntero a la estructura de configuraci�n del LTC6811.
 * @param[in]     cmd1             Primer byte del comando RDCVx (ej: 0x04 para RDCVA).
 * @param[in]     cmd2             Segundo byte del comando RDCVx (usualmente 0x00).
 * @param[out]    cell_voltages    Puntero a un arreglo de 3 enteros de 16 bits donde
 *                                 se almacenar�n los voltajes crudos (sin conversi�n).
 *
 * @return TRUE si la lectura fue exitosa y el PEC fue v�lido, FALSE si hubo error de comunicaci�n.
 */
BoolTypeDef LTC6811_Read_Cell_Block(LTC6811_Typdef* LTC6811,uint8_t cmd1, uint8_t cmd2, uint16_t *cell_voltages) 
{
    uint8_t tx[2] = {cmd1, cmd2};         // Comando a enviar (2 bytes)
    uint8_t rx[8] = {0};                  // Buffer de recepci�n: 6 datos + 2 bytes de PEC

    // Calcular PEC para el comando
    uint16_t pec = LTC6811_PEC15_Calc(tx, 2);
    uint8_t cmd[4] = {cmd1, cmd2, (uint8_t)(pec >> 8), (uint8_t)(pec & 0xFF)};

    // Enviar comando y recibir respuesta del LTC6811
    LTC6811_SPI_Transmit_Receive(LTC6811, cmd, rx, sizeof(cmd), sizeof(rx));

    // Extraer y verificar el PEC recibido
    uint16_t received_pec = (rx[6] << 8) | rx[7];     // PEC recibido del LTC6811
    uint16_t calc_pec = LTC6811_PEC15_Calc(rx, 6);    // PEC calculado de los 6 datos

    if (received_pec != calc_pec) {
        // Error de comunicaci�n por discrepancia de PEC
        return 0; // FALSE
    }

    // Si el PEC es v�lido, extraer los 3 voltajes crudos
    cell_voltages[0] = rx[0] | (rx[1] << 8); // Celda X
    cell_voltages[1] = rx[2] | (rx[3] << 8); // Celda X+1
    cell_voltages[2] = rx[4] | (rx[5] << 8); // Celda X+2

    return 1; // TRUE
}


/*******************************************************************************
********************************************************************************
***************              Read_Voltages                       ***************    
********************************************************************************
*******************************************************************************/
/**
 * @brief Lee los voltajes de todas las celdas conectadas a un LTC6811.
 *
 * Esta funci�n recorre los 4 bloques de lectura del LTC6811 (RDCVA, RDCVB, RDCVC y RDCVD),
 * correspondientes a las 12 celdas (3 por bloque), e intenta leer los valores dos veces
 * en caso de error. Si la lectura es exitosa, convierte el valor crudo a voltaje en voltios
 * y lo almacena en el arreglo proporcionado. Si falla, marca la estructura del LTC como fallida.
 *
 * @param[in,out] LTC6811       Puntero a la estructura de configuraci�n y estado del LTC6811.
 * @param[out]    voltages      Arreglo de 12 elementos donde se almacenar�n los voltajes le�dos,
 *                              en el orden C1 a C12, expresados en voltios.
 */
void LTC_Read_All_Voltages(LTC6811_Typdef *LTC6811, float *voltages) 
{
    // Comandos de lectura de bloques de celdas y su �ndice inicial en el arreglo voltages
    const struct {
        uint8_t cmd1;  // Primer byte del comando
        uint8_t cmd2;  // Segundo byte del comando
        uint8_t index; // �ndice base en el arreglo voltages
    } blocks[] = {
        {0x04, 0x00, 0}, // RDCVA: celdas C1�C3
        {0x06, 0x00, 3}, // RDCVB: celdas C4�C6
        {0x08, 0x00, 6}, // RDCVC: celdas C7�C9
        {0x0A, 0x00, 9}  // RDCVD: celdas C10�C12
    };

    uint16_t buf[3];  // Buffer temporal para almacenar las lecturas de 3 celdas

    for (int i = 0; i < 4; i++) {
        BoolTypeDef success = FALSE;

        // Se intenta leer hasta 2 veces por bloque en caso de falla
        for (int attempt = 0; attempt < 2; attempt++) {
            // Si la lectura es exitosa...
            if (LTC6811_Read_Cell_Block(LTC6811, blocks[i].cmd1, blocks[i].cmd2, buf)) {
                // Convertimos y almacenamos los valores de voltaje en el arreglo de salida
                voltages[blocks[i].index + 0] = buf[0] * 0.0001f;  // celda 1
                voltages[blocks[i].index + 1] = buf[1] * 0.0001f;  // celda 2
                voltages[blocks[i].index + 2] = buf[2] * 0.0001f;  // celda 3
                success = TRUE;
                break;  // Salimos del bucle de reintento si fue exitoso
            }
        }

        // Si despu�s de 2 intentos no fue posible leer, se marca la falla
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
 * Esta funci�n activa el balanceo de celdas de manera alterna (pares/impares), 
 * mide los voltajes de las 12 celdas por LTC, convierte las lecturas a temperaturas 
 * (para las celdas pares en una pasada y las impares en la otra) y almacena 
 * los resultados en los arreglos TempArray y VoltajeArray.
 *
 * @param[in,out] Control_Unit   Puntero a la unidad de control que contiene el estado de los dos LTC6811.
 * @param[out]    TempArray      Arreglo de 24 elementos donde se guardan temperaturas codificadas. 
 *                               Las primeras 12 son del LTC6811_1, las �ltimas 12 del LTC6811_2.
 * @param[out]    VoltajeArray   Arreglo de 24 elementos donde se guardan voltajes. 
 *                               Misma distribuci�n que TempArray.
 */
void LTC6811_Measure_Temperatures_and_Voltages(Control_Unit_TypeDef* Control_Unit)
{
    float voltages_1[12]; // Voltajes de las 12 celdas del LTC6811_1
    float voltages_2[12]; // Voltajes de las 12 celdas del LTC6811_2

    // Despierta ambos LTC6811
    LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_1);
    //LTC6811_Wake_Up(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(1); 

    // === Medici�n de celdas pares ===
    // Activa el balanceo de celdas pares
    LTC_Active_Even_Balancing(&Control_Unit->Status.LTC6811_1);
    //LTC_Active_Even_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5); // Permite estabilizaci�n

    // Inicia conversi�n ADC en ambos LTC
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_1);
    //LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3); // Espera la finalizaci�n de la conversi�n

    // Lee los voltajes
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_1, voltages_1);
    //LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_2, voltages_2);

    // Convertir voltajes pares a temperaturas (�ndices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(voltages_1[i]);
        //Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(voltages_2[i]);
    }

    // Guardar voltajes de celdas impares (�ndices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = voltages_1[i];
        //Control_Unit->Status.Voltages[i + 12] = voltages_2[i];
    }

		
    // Desactiva balanceo
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
    //LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5);

		//Si ha fallado algo dejamos de medir
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE)// || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
			return;
		}
		
    // === Medici�n de celdas impares ===
		// Activa el balanceo de celdas impares
    LTC_Active_Odd_Balancing(&Control_Unit->Status.LTC6811_1);
    //LTC_Active_Odd_Balancing(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(5);

		// Inicia conversi�n ADC en ambos LTC
    LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_1);
    //LTC6811_Start_ADC_Conv(&Control_Unit->Status.LTC6811_2);
    HAL_Delay(3);

    // Lee los voltajes
    LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_1, voltages_1);
    //LTC_Read_All_Voltages(&Control_Unit->Status.LTC6811_2, voltages_2);

    // Convertir voltajes impares a temperatura (�ndices pares)
    for (int i = 0; i < 12; i += 2) 
		{
        Control_Unit->Status.Temperatures[i].Readed_Value = LTC_Voltage_to_Temperature(voltages_1[i]);
        //Control_Unit->Status.Temperatures[i+12].Readed_Value = LTC_Voltage_to_Temperature(voltages_2[i]);
    }

    // Guardar voltajes de celdas pares (�ndices impares)
    for (int i = 1; i < 12; i += 2) 
		{
        Control_Unit->Status.Voltages[i] = voltages_1[i];
        //Control_Unit->Status.Voltages[i + 12] = voltages_2[i];
    }

		
		
    // Desactiva balanceo al final (por seguridad)
    LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_1);
    //LTC_Disable_Balancing(&Control_Unit->Status.LTC6811_2);
		
		
		//Si ha fallado algo dejamos de medir
		if(Control_Unit->Status.LTC6811_1.Fail==TRUE)// || Control_Unit->Status.LTC6811_2.Fail==TRUE)
		{
			Control_Unit->State=LTC6811_FAIL_MODE;
		}
}


