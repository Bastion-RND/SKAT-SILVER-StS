/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
struct sBoiler boiler = {0};
uint32_t tickstart_1sec, tickstart_adc;
uint16_t delay_counter_1ms;
uint16_t delay_counter_500ms;
uint16_t led_counter_500ms;
uint8_t ten1_output, ten2_output, ten3_output;

struct sSensor* list_of_sensors[2] = {&(boiler.heater.output.sensor), &(boiler.heatsink.sensor)};
uint8_t sensors_list_size = sizeof(list_of_sensors) / sizeof(list_of_sensors[0]);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC_Init();
  MX_IWDG_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  boiler_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  adc_update();
	  adc_check();
	  if (boiler.testMode.flag == 0)
	  {
		  key_state_update();
		  user_interface_update();
		  heating_update();
		  upn_update();
	  }
	  else
		  run_test();

	  one_second_event_handler();
	  display_update();
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void check_IWDG_refresh()
{
	  if((HAL_GetTick() - tickstart_1sec) < 1000)
		  return;

	  tickstart_1sec = HAL_GetTick();
	  HAL_IWDG_Refresh(&hiwdg);
}

void adc_update()
{
	if ((HAL_GetTick() - tickstart_adc) > 50)
	{
		HAL_ADC_Start_DMA(&hadc, (uint32_t*)(&boiler.adc.value), ADC_CHANNELS_NUMBER);
		tickstart_adc = HAL_GetTick();
	}
}

void adc_check()
{
	for (uint8_t j = 0; j < sensors_list_size; ++j)
			  check_if_sensor_is_broken(list_of_sensors[j]);
}

void set_e01(void) {boiler.errors |= 0x01;};
//void reset_e01(void) {boiler.errors &= ~0x01;};
uint8_t get_e01() { return (boiler.errors & 0x01) ? 1 : 0;};
void set_e02(void) {boiler.errors |= 0x02;};
void reset_e02(void) {boiler.errors &= ~0x02;};
uint8_t get_e02() { return (boiler.errors & 0x02) ? 1 : 0;};
void set_e03(void) {boiler.errors |= 0x04;};
//void reset_e03(void) {boiler.errors &= ~0x04;};
uint8_t get_e03() { return (boiler.errors & 0x04) ? 1 : 0;};
void set_e04(void) {boiler.errors |= 0x08;};
//void reset_e04(void) {boiler.errors &= ~0x08;};
uint8_t get_e04() { return (boiler.errors & 0x08) ? 1 : 0;};
void set_e05(void) {boiler.errors |= 0x10;};
void reset_e05(void) {boiler.errors &= ~0x10;};
uint8_t get_e05() { return (boiler.errors & 0x10) ? 1 : 0;};

void set_f01(void) {boiler.errors |= 0x20;};
void reset_f01(void) {boiler.errors &= ~0x20;};
uint8_t get_f01() { return (boiler.errors & 0x20) ? 1 : 0;};

void set_f02(void) {boiler.errors |= 0x40;};
void reset_f02(void) {boiler.errors &= ~0x40;};
uint8_t get_f02() { return (boiler.errors & 0x40) ? 1 : 0;};

void set_f03(void) {boiler.errors |= 0x80;};
void reset_f03(void) {boiler.errors &= ~0x80;};
uint8_t get_f03() { return (boiler.errors & 0x80) ? 1 : 0;};

void set_u01(void) {boiler.errors |= 0x100;};
void reset_u01(void) {boiler.errors &= ~0x100;};
uint8_t get_u01() { return (boiler.errors & 0x100) ? 1 : 0;};

void set_u02(void) {boiler.errors |= 0x200;};
void reset_u02(void) {boiler.errors &= ~0x200;};
uint8_t get_u02() { return (boiler.errors & 0x200) ? 1 : 0;};

void set_u03(void) {boiler.errors |= 0x400;};
void reset_u03(void) {boiler.errors &= ~0x400;};
uint8_t get_u03() { return (boiler.errors & 0x400) ? 1 : 0;};

void ten_default_initialization()
{
	  boiler.ten3.upn.signal = &(boiler.upn.phase_C.Current_Instant);
	  boiler.ten3.zero_cross = &boiler.zero_cross.phase_3;
	  boiler.ten3.physical_output = &ten3_output;

	  boiler.ten2.upn.signal = &(boiler.upn.phase_B.Current_Instant);
	  boiler.ten2.zero_cross = &boiler.zero_cross.phase_2;
	  boiler.ten2.physical_output = &ten2_output;

	  boiler.ten1.upn.signal = &(boiler.upn.phase_A.Current_Instant);
	  boiler.ten1.zero_cross = &boiler.zero_cross.phase_1;
	  boiler.ten1.physical_output = &ten1_output;
}

void check_if_sensor_is_broken(struct sSensor* sensor_ptr)
{
	if((*sensor_ptr).tC_Instant == 250)
	{
		if((*sensor_ptr).counter_alarm < ALARM_COUNTER_MAX)	(*sensor_ptr).counter_alarm ++;
		else												(*sensor_ptr).Flag_Enable = 0;
	}
	else
	{
		if((*sensor_ptr).counter_alarm)	(*sensor_ptr).counter_alarm --;
		else							(*sensor_ptr).Flag_Enable = 1;
	}
}

void init_param_from_eeprom()
{
	uint32_t data = eeprom_read_setting(FLASH_PAGE_ADDR_31);
	boiler.parameters.boiler_type = (uint8_t)data;
	data = data >> 8;
	boiler.heater.parameters.aim_temp = (uint8_t)data;

	if((boiler.heater.parameters.aim_temp < SetTC_MIN) || (boiler.heater.parameters.aim_temp > SetTC_MAX))
	{
		boiler.heater.parameters.aim_temp = SetTC_DEFAULT;
		eeprom_write_setting(boiler.heater.parameters.aim_temp << 8 | boiler.parameters.boiler_type, FLASH_PAGE_ADDR_31);
	}

	if(boiler.parameters.boiler_type > 2)
	{
		boiler.parameters.boiler_type = 2;
		eeprom_write_setting(boiler.heater.parameters.aim_temp << 8 | boiler.parameters.boiler_type, FLASH_PAGE_ADDR_31);
	}
}

void boiler_init()
{
	HAL_ADC_MspInit(&hadc);
	HAL_ADCEx_Calibration_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc, (uint32_t*)(&boiler.adc.value), ADC_CHANNELS_NUMBER);
	LL_SPI_Enable(SPI1);

	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim17);

	ten_default_initialization();
	init_param_from_eeprom();

	boiler.display.value_to_show = 1;

	boiler.parameters.user_interface_step = 0;
	boiler.display.counter = 0;

	boiler.heater.output.sensor.tC_Average = 25;
	boiler.heatsink.sensor.tC_Average = 25;

	boiler.errors = 0;

	boiler.adc.Vref_Const = (uint16_t)(*(__IO uint32_t*)(0x1FFFF7BA));

	boiler.termostat.value = HAL_GPIO_ReadPin(TERMOSTAT_GPIO_Port, TERMOSTAT_Pin);
	tickstart_1sec = HAL_GetTick();
	tickstart_adc = HAL_GetTick();

	if ((HAL_GPIO_ReadPin(JMP4_GPIO_Port, JMP4_Pin) == 0) && (HAL_GPIO_ReadPin(JMP8_GPIO_Port, JMP8_Pin) == 0))
	{
		boiler.testMode.flag = 1;
		display_print_text('-', '-', '-');
		boiler.parameters.user_interface_step = 100;
		delay_counter_1ms = 3000;
	}
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
