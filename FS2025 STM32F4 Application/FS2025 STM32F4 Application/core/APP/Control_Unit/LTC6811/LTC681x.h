/**
  ******************************************************************************
  * @file           : LTC681x.h
  * @brief          : Header file for LTC6811/LTC681x communication and control
  ******************************************************************************
  * @attention
  *
  * (c) 2024 Uniovi E-tech Racing.
  *
  * Provides definitions and function prototypes for interacting with LTC6811
  * battery monitoring ICs. Functions cover initialization, configuration,
  * voltage reading, diagnostics, and PEC (Packet Error Code) management.
  *
  ******************************************************************************
*/

#ifndef LTC681X_H
#define LTC681X_H

#include <stdint.h>
#include <stdbool.h>
#include "LT_SPI.h"

// ================= ADC Modes =================
#define MD_422HZ_1KHZ     0   // ADC Mode: Low noise, low speed
#define MD_27KHZ_14KHZ    1   // ADC Mode: Medium speed
#define MD_7KHZ_3KHZ      2   // ADC Mode: Default mode (used for general measurement)
#define MD_26HZ_2KHZ      3   // ADC Mode: Very low speed

#define ADC_OPT_ENABLED   1   // Enable optimized ADC
#define ADC_OPT_DISABLED  0   // Disable optimized ADC

// =============== Cell Channel Selection ===============
#define CELL_CH_ALL       0
#define CELL_CH_1and7     1
#define CELL_CH_2and8     2
#define CELL_CH_3and9     3
#define CELL_CH_4and10    4
#define CELL_CH_5and11    5
#define CELL_CH_6and12    6

// =============== Self-Test Types ===============
#define SELFTEST_1        1
#define SELFTEST_2        2

// =============== Auxiliary Channels ===============
#define AUX_CH_ALL        0
#define AUX_CH_GPIO1      1
#define AUX_CH_GPIO2      2
#define AUX_CH_GPIO3      3
#define AUX_CH_GPIO4      4
#define AUX_CH_GPIO5      5
#define AUX_CH_VREF2      6

// =============== Status Channels ===============
#define STAT_CH_ALL       0
#define STAT_CH_SOC       1
#define STAT_CH_ITEMP     2
#define STAT_CH_VREGA     3
#define STAT_CH_VREGD     4

// =============== Discharge Control ===============
#define DCP_DISABLED      0
#define DCP_ENABLED       1

#define PULL_UP_CURRENT   1   // For open wire testing
#define PULL_DOWN_CURRENT 0

// =============== Register Types ===============
#define NUM_RX_BYT        8
#define CELL              1
#define AUX               2
#define STAT              3
#define CFGRA             0
#define CFGRB             4
#define CS_PIN            13

// =================== Structures ===================

// Structure to hold cell voltage data
typedef struct {
  uint16_t c_codes[18];     // Cell voltage values
  uint8_t pec_match[6];     // PEC match status for each voltage group
} cv;

// Structure to hold auxiliary (GPIO, Vref2) voltages
typedef struct {
  uint16_t a_codes[9];      // AUX voltage values
  uint8_t pec_match[4];     // PEC match status for each AUX group
} ax;

// Structure for status register content
typedef struct {
  uint16_t stat_codes[4];   // Status voltage values (SOC, Temp, VREGA, VREGD)
  uint8_t flags[3];         // Status flags (UV/OV)
  uint8_t mux_fail[1];      // MUX failure flag
  uint8_t thsd[1];          // Thermal shutdown flag
  uint8_t pec_match[2];     // PEC match status for STAT register groups
} st;

// Generic structure for any 6-byte TX / 8-byte RX register with PEC
typedef struct {
  uint8_t tx_data[6];       // Data to be transmitted
  uint8_t rx_data[8];       // Data received (with PEC)
  uint8_t rx_pec_match;     // Indicates PEC error on RX
} ic_register;

// Counter for PEC error tracking
typedef struct {
  uint16_t pec_count;       // Total PEC errors
  uint16_t cfgr_pec;        // PEC errors in CFGR registers
  uint16_t cell_pec[6];     // PEC errors per cell voltage group
  uint16_t aux_pec[4];      // PEC errors per aux group
  uint16_t stat_pec[2];     // PEC errors per stat group
} pec_counter;

// Holds info about register configuration (used in parsing)
typedef struct {
  uint8_t cell_channels;
  uint8_t stat_channels;
  uint8_t aux_channels;
  uint8_t num_cv_reg;
  uint8_t num_gpio_reg;
  uint8_t num_stat_reg;
} register_cfg;

// Complete IC structure representing a single LTC681x device
typedef struct {
  ic_register config;
  ic_register configb;
  cv cells;
  ax aux;
  st stat;
  ic_register com;
  ic_register pwm;
  ic_register pwmb;
  ic_register sctrl;
  ic_register sctrlb;
  bool isospi_reverse;         // For reverse daisy-chain order
  pec_counter crc_count;       // PEC counters
  register_cfg ic_reg;         // Register channel configuration
  long system_open_wire;       // Open wire status bitfield
} cell_asic;



// ================= Function Prototypes =================

extern const uint16_t crc15Table[256];  // CRC lookup table (precomputed)

// Wake-up utilities
void wakeup_idle(uint8_t total_ic);
void wakeup_sleep(uint8_t total_ic);

// SPI Command I/O
void cmd_68(uint8_t tx_cmd[2]);
void write_68(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t data[]);
int8_t read_68(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t *rx_data);

// PEC generator
uint16_t pec15_calc(uint8_t len, uint8_t *data);

// Voltage conversions
void LTC681x_adcv(uint8_t MD, uint8_t DCP, uint8_t CH);
void LTC681x_adcvsc(uint8_t MD, uint8_t DCP);
void LTC681x_adcvax(uint8_t MD, uint8_t DCP);
void LTC681x_adol(uint8_t MD, uint8_t DCP);
void LTC681x_cvst(uint8_t MD, uint8_t ST);

// AUX and STAT conversions
void LTC681x_axst(uint8_t MD, uint8_t ST);
void LTC681x_statst(uint8_t MD, uint8_t ST);
void LTC681x_adstat(uint8_t MD, uint8_t CHST);
void LTC681x_adstatd(uint8_t MD, uint8_t CHST);
void LTC681x_adax(uint8_t MD, uint8_t CHG);
void LTC681x_adaxd(uint8_t MD, uint8_t CHG);
void LTC681x_adow(uint8_t MD, uint8_t PUP);

// ADC status
uint8_t  LTC681x_pladc(void);
uint32_t LTC681x_pollAdc(void);

// Raw register reads
void LTC681x_rdcv_reg(uint8_t reg, uint8_t total_ic, uint8_t *data);
void LTC681x_rdaux_reg(uint8_t reg, uint8_t total_ic, uint8_t *data);
void LTC681x_rdstat_reg(uint8_t reg, uint8_t total_ic, uint8_t *data);

// Parsing register data
int8_t parse_cells(uint8_t current_ic, uint8_t cell_reg, uint8_t cell_data[], uint16_t *cell_codes, uint8_t *ic_pec);

// Clear command registers
void LTC681x_clrcell(void);
void LTC681x_clraux(void);
void LTC681x_clrstat(void);
void LTC681x_clrsctrl(void);

// Diagnostics
void LTC681x_diagn(void);

// Voltage register decoders
uint8_t LTC681x_rdcv(uint8_t reg, uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdaux(uint8_t reg, uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdstat(uint8_t reg, uint8_t total_ic, cell_asic ic[]);

// Configuration interface
void   LTC681x_wrcfg(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcfg(uint8_t total_ic, cell_asic ic[]);
void   LTC681x_wrcfgb(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcfgb(uint8_t total_ic, cell_asic ic[]);


// Helpers to set up configuration
void LTC681x_init_cfg(uint8_t total_ic, cell_asic ic[]);
void LTC681x_set_cfgr(uint8_t nIC, cell_asic ic[], bool refon, bool adcopt, bool gpio[5], bool dcc[12]);
void LTC681x_set_cfgr_refon(uint8_t nIC, cell_asic ic[], bool refon);
void LTC681x_set_cfgr_adcopt(uint8_t nIC, cell_asic ic[], bool adcopt);
void LTC681x_set_cfgr_gpio(uint8_t nIC, cell_asic ic[], bool gpio[5]);
void LTC681x_set_cfgr_dis(uint8_t nIC, cell_asic ic[], bool dcc[12]);
void LTC681x_set_cfgr_uv(uint8_t nIC, cell_asic ic[], uint16_t uv);
void LTC681x_set_cfgr_ov(uint8_t nIC, cell_asic ic[], uint16_t ov);


// COMM and PWM
void   LTC681x_wrcomm(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcomm(uint8_t total_ic, cell_asic ic[]);
void   LTC681x_stcomm(void);
void   LTC681x_wrpwm(uint8_t total_ic, uint8_t pwmReg, cell_asic ic[]);
int8_t LTC681x_rdpwm(uint8_t total_ic, uint8_t pwmReg, cell_asic ic[]);

// PEC diagnostics
void LTC681x_check_pec(uint8_t total_ic, uint8_t reg, cell_asic ic[]);
void LTC681x_reset_crc_count(uint8_t total_ic, cell_asic ic[]);

// Self test routines
int16_t LTC681x_run_cell_adc_st(uint8_t adc_reg, uint8_t total_ic, cell_asic ic[]);
int16_t LTC681x_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, cell_asic ic[]);
uint16_t LTC681x_st_lookup(uint8_t MD, uint8_t ST);
uint16_t LTC681x_run_adc_overlap(uint8_t total_ic, cell_asic ic[]);
void LTC681x_run_openwire(uint8_t total_ic, cell_asic ic[]);

// Discharge control
void clear_discharge(uint8_t total_ic, cell_asic ic[]);











// Wake-up utilities
void wakeup_idle_2(uint8_t total_ic);
void wakeup_sleep_2(uint8_t total_ic);

// SPI Command I/O
void cmd_68_2(uint8_t tx_cmd[2]);
void write_68_2(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t data[]);
int8_t read_68_2(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t *rx_data);

// PEC generator
uint16_t pec15_calc_2(uint8_t len, uint8_t *data);

// Voltage conversions
void LTC681x_adcv_2(uint8_t MD, uint8_t DCP, uint8_t CH);
void LTC681x_adcvsc_2(uint8_t MD, uint8_t DCP);
void LTC681x_adcvax_2(uint8_t MD, uint8_t DCP);
void LTC681x_adol_2(uint8_t MD, uint8_t DCP);
void LTC681x_cvst_2(uint8_t MD, uint8_t ST);

// AUX and STAT conversions
void LTC681x_axst_2(uint8_t MD, uint8_t ST);
void LTC681x_statst_2(uint8_t MD, uint8_t ST);
void LTC681x_adstat_2(uint8_t MD, uint8_t CHST);
void LTC681x_adstatd_2(uint8_t MD, uint8_t CHST);
void LTC681x_adax_2(uint8_t MD, uint8_t CHG);
void LTC681x_adaxd_2(uint8_t MD, uint8_t CHG);
void LTC681x_adow_2(uint8_t MD, uint8_t PUP);

// ADC status
uint8_t  LTC681x_pladc_2(void);
uint32_t LTC681x_pollAdc_2(void);

// Raw register reads
void LTC681x_rdcv_reg_2(uint8_t reg, uint8_t total_ic, uint8_t *data);
void LTC681x_rdaux_reg_2(uint8_t reg, uint8_t total_ic, uint8_t *data);
void LTC681x_rdstat_reg_2(uint8_t reg, uint8_t total_ic, uint8_t *data);

// Parsing register data
int8_t parse_cells_2(uint8_t current_ic, uint8_t cell_reg, uint8_t cell_data[], uint16_t *cell_codes, uint8_t *ic_pec);

// Clear command registers
void LTC681x_clrcell_2(void);
void LTC681x_clraux_2(void);
void LTC681x_clrstat_2(void);
void LTC681x_clrsctrl_2(void);

// Diagnostics
void LTC681x_diagn_2(void);

// Voltage register decoders
uint8_t LTC681x_rdcv_2(uint8_t reg, uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdaux_2(uint8_t reg, uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdstat_2(uint8_t reg, uint8_t total_ic, cell_asic ic[]);

// Configuration interface
void   LTC681x_wrcfg_2(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcfg_2(uint8_t total_ic, cell_asic ic[]);
void   LTC681x_wrcfgb_2(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcfgb_2(uint8_t total_ic, cell_asic ic[]);


// Helpers to set up configuration
void LTC681x_init_cfg_2(uint8_t total_ic, cell_asic ic[]);
void LTC681x_set_cfgr_2(uint8_t nIC, cell_asic ic[], bool refon, bool adcopt, bool gpio[5], bool dcc[12]);
void LTC681x_set_cfgr_refon_2(uint8_t nIC, cell_asic ic[], bool refon);
void LTC681x_set_cfgr_adcopt_2(uint8_t nIC, cell_asic ic[], bool adcopt);
void LTC681x_set_cfgr_gpio_2(uint8_t nIC, cell_asic ic[], bool gpio[5]);
void LTC681x_set_cfgr_dis_2(uint8_t nIC, cell_asic ic[], bool dcc[12]);
void LTC681x_set_cfgr_uv_2(uint8_t nIC, cell_asic ic[], uint16_t uv);
void LTC681x_set_cfgr_ov_2(uint8_t nIC, cell_asic ic[], uint16_t ov);


// COMM and PWM
void   LTC681x_wrcomm_2(uint8_t total_ic, cell_asic ic[]);
int8_t LTC681x_rdcomm_2(uint8_t total_ic, cell_asic ic[]);
void   LTC681x_stcomm_2(void);
void   LTC681x_wrpwm_2(uint8_t total_ic, uint8_t pwmReg, cell_asic ic[]);
int8_t LTC681x_rdpwm_2(uint8_t total_ic, uint8_t pwmReg, cell_asic ic[]);

// PEC diagnostics
void LTC681x_check_pec_2(uint8_t total_ic, uint8_t reg, cell_asic ic[]);
void LTC681x_reset_crc_count_2(uint8_t total_ic, cell_asic ic[]);

// Self test routines
int16_t LTC681x_run_cell_adc_st_2(uint8_t adc_reg, uint8_t total_ic, cell_asic ic[]);
int16_t LTC681x_run_adc_redundancy_st_2(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, cell_asic ic[]);
uint16_t LTC681x_st_lookup_2(uint8_t MD, uint8_t ST);
uint16_t LTC681x_run_adc_overlap_2(uint8_t total_ic, cell_asic ic[]);
void LTC681x_run_openwire_2(uint8_t total_ic, cell_asic ic[]);

// Discharge control
void clear_discharge_2(uint8_t total_ic, cell_asic ic[]);

#endif /* LTC681X_H */