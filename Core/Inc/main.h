/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_dma.h"

#include "stm32f0xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "display.h"
#include "control.h"
#include "eeprom.h"
#include "stm32f0xx_it.h"
#include "event_handlers.h"
#include "iwdg.h"
#include "heating_mode.h"
#include "upn.h"
#include "user_interface_state_machine.h"
#include "one_second_event_handler.h"
#include "test.h"
#include "fan.h"

#define LEFT_KEY 0
#define RIGHT_KEY 1

#define PUMP_ACTIVITY_TIME 180

#define SetTC_MIN 10
#define SetTC_MAX 80
#define SetTC_DEFAULT 60

#define ADC_CHANNELS_NUMBER 7

#define SHORT_INTERVAL 20	 		// 50ms   -> время определения нажатия и отпускания кнопки
#define LONG_INTERVAL 1500			// 1,5сек -> время определения длинного нажатия кнопки
#define LONG_INTERVAL_OFF 20		// 0,3сек -> время определения отпускания кнопки
#define RESET_INTERVAL 3000

#define MIN_OPERATION_TEMPERATURE 5

#define ONE_MINUTE 60

#define WAIT_TIME_5_SEC 5
#define WAIT_TIME_15_MIN 900

#define ALARM_COUNTER_MAX 200

#define UPN_WAITING_TIME 180

#define TEN_ROTATION_PERIOD_SEC 3600

#define HEATING_GISTEREZIS 5
#define HEATING_COEFFICIENT 2

#define FAN_10SEC 10000		// При включении медленной скорости вентилятора сначала включаем его
#define FAN_5SEC 9500		// на быстрой скорости на время (FAN_10sec - FAN_5sec) или 0,5сек
#define FAN_SPEED_OFF 0b0
#define FAN_SPEED_NORMAL 0b1
#define FAN_SPEED_FAST 0b11

#define HEATSINK_FAN_OFF_TEMPERATURE 69
#define HEATSINK_FAN_SLOW_MIN_TEMPERATURE 71
#define HEATSINK_FAN_SLOW_MAX_TEMPERATURE 79
#define HEATSINK_FAN_FAST_MIN_TEMPERATURE 81

#define OVERHEAT_TEMPERATURE_ONE_SIMMISTOR_OFF 85
#define OVERHEAT_TEMPERATURE_TWO_SIMMISTORS_OFF 90
#define OVERHEAT_TEMPERATURE_ALL_SIMMISTORS_OFF 95

#define LED_OFF 0
#define LED_BLINK 1
#define LED_LIGHT 2

#define DISPLAY_SHOW 0
#define DISPLAY_BLINK 1

//#define FLASH_PAGE_ADDR_30 0x08007800
#define FLASH_PAGE_ADDR_31 0x08007C00
//#define FLASH_PAGE_ADDR_32 0x08008000

#define E01 0x01
#define E02 0x02
#define E03 0x04
#define E04 0x08
#define E05 0x10
#define E06 0x20
#define E07 0x40
#define E08 0x80
#define E09 0x100
#define E10 0x200
#define E11 0x400
#define E12 0x800
#define E13 0x1000
#define E14 0x2000
#define E15 0x4000
#define E16 0x8000
#define U01 0x10000
#define U02 0x20000
#define U03 0x40000
#define U04 0x80000
#define U05 0x100000
#define U06 0x200000
#define U07 0x400000
#define U08 0x800000
#define U09 0x1000000
#define U10 0x2000000
#define U11 0x4000000
#define U12 0x8000000
#define U13 0x10000000
#define U14 0x20000000
#define U15 0x40000000
#define U16 0x80000000

struct sSensor
{
	float 		tC_Instant;
	float 		tC_Average;
	uint8_t		Flag_Enable;
	uint8_t	counter_alarm;
	uint16_t adc_value;
};

struct sHeaterOut
{
	struct sSensor sensor;
};

struct sHeaterParams
{
	uint8_t aim_temp;
	uint16_t algorithm_step;
};

struct sBoilerParams
{
	uint8_t try_activate_ten;
	uint8_t boiler_type;
	uint8_t user_choice;
	uint8_t current;
	uint8_t boot_flag;
	uint16_t user_interface_step;
	struct sSensor temperature_alarm;
};

struct sHeater
{
	struct 	sHeaterOut output;
	struct 	sHeaterParams parameters;
};

struct sUPNStatus
{
    float *signal;
    uint8_t is_active;
    uint8_t check;
    uint8_t timer;
};

struct sTEN
{
	struct sUPNStatus upn;
	uint8_t is_active;
	uint8_t *zero_cross;
	uint8_t* physical_output;
};

struct sDisplay
{
	uint16_t accum;
	uint8_t counter;
	uint8_t digits[3];
	uint8_t blink_status;
	uint8_t led_status;
	uint8_t blink;
	uint32_t value_to_show;
	uint8_t error_flag;
};

struct sPump {
	uint8_t pump_timer_ON;
	uint8_t activate;
	uint16_t time_of_activity;
};

struct sUPNsignal
{
	float Current_Instant;
};

struct sUPNParams {
	uint8_t type;
	short phase_detector_accum;
};

struct sUPN
{
	struct sUPNsignal phase_A;
	struct sUPNsignal phase_B;
	struct sUPNsignal phase_C;
	struct sUPNParams parameters;
};

struct sButtons
{
	uint8_t flag_short;
	uint8_t flag_long;
	uint8_t flag_off;
	uint16_t counter_on;
	uint16_t counter_off;
};

struct sTimers {
	uint16_t rotation_timer;
	uint8_t counter_5_sec;
	uint16_t counter_15_min;
};

struct sTermostat
{
	uint8_t value;	// 1 - start heating, 0 - stop heating
};

struct sADC
{
    uint16_t 	Vref_Const;
    float 		Vref_k;
	uint16_t value[ADC_CHANNELS_NUMBER];
};

struct sHeatsink
{
	struct sSensor sensor;
};

struct sZeroCross
{
	uint8_t phase_1;
	uint8_t phase_2;
	uint8_t phase_3;
};

struct sFanControl
{
	uint8_t speed;
};

struct sTestMode
{
	uint8_t flag;
};

struct sBoiler
{
	struct sHeater heater;
	struct sBoilerParams parameters;
	struct sTEN ten1, ten2, ten3;
	struct sDisplay display;
	struct sPump pump;
	struct sUPN upn;
	struct sADC adc;
	uint32_t errors;
	struct sHeatsink heatsink;
	struct sZeroCross zero_cross;
	struct sFanControl fan;

	struct sButtons button[2];
	struct sTimers timers;
	struct sTermostat termostat;
	struct sTestMode testMode;
};

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PUMP_Pin GPIO_PIN_0
#define PUMP_GPIO_Port GPIOF
#define FAN_HIGH_Pin GPIO_PIN_1
#define FAN_HIGH_GPIO_Port GPIOF
#define ADC_T_BOILER_Pin GPIO_PIN_0
#define ADC_T_BOILER_GPIO_Port GPIOA
#define TERMOSTAT_Pin GPIO_PIN_1
#define TERMOSTAT_GPIO_Port GPIOA
#define TERMOSTAT_EXTI_IRQn EXTI0_1_IRQn
#define OVERTEMP_ALARM_Pin GPIO_PIN_2
#define OVERTEMP_ALARM_GPIO_Port GPIOA
#define ADC_T_RAD_Pin GPIO_PIN_3
#define ADC_T_RAD_GPIO_Port GPIOA
#define ADC_I3_Pin GPIO_PIN_6
#define ADC_I3_GPIO_Port GPIOA
#define ADC_I2_Pin GPIO_PIN_0
#define ADC_I2_GPIO_Port GPIOB
#define ADC_I1_Pin GPIO_PIN_1
#define ADC_I1_GPIO_Port GPIOB
#define FAN_LOW_Pin GPIO_PIN_8
#define FAN_LOW_GPIO_Port GPIOA
#define JMP8_Pin GPIO_PIN_9
#define JMP8_GPIO_Port GPIOA
#define JMP4_Pin GPIO_PIN_10
#define JMP4_GPIO_Port GPIOA
#define KEY_1_Pin GPIO_PIN_11
#define KEY_1_GPIO_Port GPIOA
#define KEY_2_Pin GPIO_PIN_12
#define KEY_2_GPIO_Port GPIOA
#define TEN_3_Pin GPIO_PIN_15
#define TEN_3_GPIO_Port GPIOA
#define ZC_3_Pin GPIO_PIN_3
#define ZC_3_GPIO_Port GPIOB
#define ZC_3_EXTI_IRQn EXTI2_3_IRQn
#define TEN_2_Pin GPIO_PIN_4
#define TEN_2_GPIO_Port GPIOB
#define ZC_2_Pin GPIO_PIN_5
#define ZC_2_GPIO_Port GPIOB
#define ZC_2_EXTI_IRQn EXTI4_15_IRQn
#define TEN_1_Pin GPIO_PIN_6
#define TEN_1_GPIO_Port GPIOB
#define ZC_1_Pin GPIO_PIN_7
#define ZC_1_GPIO_Port GPIOB
#define ZC_1_EXTI_IRQn EXTI4_15_IRQn

/* USER CODE BEGIN Private defines */
void adc_update();
void adc_check();
void ten_default_initialization();
void check_if_sensor_is_broken(struct sSensor* sensor_ptr);
void init_param_from_eeprom();
void boiler_init(void);
void check_alarms(void);
void set_e01(void);
void reset_e01(void);
uint8_t get_e01(void);
void set_e02(void);
void reset_e02(void);
uint8_t get_e02(void);
void set_e03(void);
void reset_e03(void);
uint8_t get_e03(void);
void set_e04(void);
void reset_e04(void);
uint8_t get_e04(void);
void set_e05(void);
void reset_e05(void);
uint8_t get_e05(void);

void set_f01(void);
void reset_f01(void);
uint8_t get_f01(void);

void set_f02(void);
void reset_f02(void);
uint8_t get_f02(void);

void set_f03(void);
void reset_f03(void);
uint8_t get_f03(void);

void set_u01(void);
void reset_u01(void);
uint8_t get_u01(void);

void set_u02(void);
void reset_u02(void);
uint8_t get_u02(void);

void set_u03(void);
void reset_u03(void);
uint8_t get_u03(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
