/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern struct sBoiler boiler;
extern uint16_t delay_counter_1ms;
extern uint16_t delay_counter_500ms;
extern 	uint8_t Flag_TEN_Chice_Start;
extern struct sSensor* list_of_sensors[2];
extern uint8_t sensors_list_size;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  if(delay_counter_1ms)		delay_counter_1ms --;

  if (delay_counter_500ms == 500)
  {
	  delay_counter_500ms = 0;
	  boiler.display.blink = (boiler.display.blink) ? 0 : 1;
  }
  else
	  delay_counter_500ms++;


  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 and 1 interrupts.
  */
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(TERMOSTAT_Pin);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

//--------------------------------------------------------------------------------
// Period - 10ms
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	boiler.adc.Vref_k = (float)boiler.adc.Vref_Const/(float)boiler.adc.value[6];

	boiler.heater.output.sensor.tC_Instant = Convert_Adc_To_Float_NTC(boiler.adc.value[0]);
	boiler.heater.output.sensor.tC_Average = boiler.heater.output.sensor.tC_Average*0.95 + boiler.heater.output.sensor.tC_Instant*0.05;

	if(boiler.adc.value[1] > 2430)
	{
		if(boiler.adc.anti_jitter_accum[1])
			boiler.adc.anti_jitter_accum[1] --;
	}
	else if(boiler.adc.value[1] < 1890)
	{
		if(boiler.adc.anti_jitter_accum[1] < 20)
			boiler.adc.anti_jitter_accum[1] ++;
		else
		{
			boiler.adc.anti_jitter_accum[1] = 20;
			boiler.errors.e03 = 1;
		}
	}

	boiler.heatsink.sensor.tC_Instant = convert_adc_to_float_T(boiler.adc.value[2]);
	boiler.heatsink.sensor.tC_Average = boiler.heatsink.sensor.tC_Average*0.95 + boiler.heatsink.sensor.tC_Instant*0.05;

	boiler.upn.phase_C.Current_Instant = Convert_Adc_To_Float_UPN_Current(boiler.adc.value[3]);
	boiler.upn.phase_B.Current_Instant = Convert_Adc_To_Float_UPN_Current(boiler.adc.value[4]);
	boiler.upn.phase_A.Current_Instant = Convert_Adc_To_Float_UPN_Current(boiler.adc.value[5]);

	for (uint8_t j = 0; j < sensors_list_size; ++j)
		check_if_sensor_is_broken(list_of_sensors[j]);
}
//--------------------------------------------------------------------------------------------

float Convert_Adc_To_Float_UPN_Current(uint16_t adc_data)
{
	return (100 * (1-((float)((adc_data) * boiler.adc.Vref_k)/2698)));
}

//--------------------------------------------------------------------------------------------
// Делитель сопротивления 1кОм и датчика KTY81/110
// Подпрограмма по значению АЦП возвращает температуру в гр.С
// Если КЗ датчика - возвращает 200
// Если обрыв датчика - возвращает -200

float convert_adc_to_float_T(uint16_t adc_result)
{
	uint16_t R;
	float T;
	float f1, f2;
	uint8_t i;

	const uint8_t STEP = 17;
	const uint16_t ADC_RESULT_MIN = 1392;
	const uint16_t ADC_RESULT_MAX = 2819;
	const uint16_t Rt[17] = {567, 624, 684, 747, 815, 886, 961, 1040, 1122, 1209, 1299, 1392, 1490, 1591, 1696, 1805, 1915};

//------------------------------

	if((adc_result < ADC_RESULT_MIN) || (adc_result > ADC_RESULT_MAX)) 	return(250);		// // Обрыв или КЗ
//------------------------------
	R = (1000 * adc_result)/(4095 - adc_result);

	if(R < Rt[0])				T = -40;
	else if (R > Rt[STEP-1])	T = +120;
	else
	{
		i = 0;
		while(R > Rt[i])	i++;

		f1 = R - Rt[i-1];
		f2 = Rt[i] - Rt[i-1];
		T = -50 + (i * 10) +  10 * f1 / f2;
	}
	return(T);
}
//------------------------------------------------------------------------------------------------------------------------------------
// Делитель сопротивления 5,1кОм и датчика NTC
// Подпрограмма по значению АЦП возвращает температуру в гр.С
// Если КЗ датчика - возвращает 250
// Если обрыв датчика - возвращает 250

float Convert_Adc_To_Float_NTC(uint16_t adc_data)
{
	uint32_t R;
	float T;
	float f1, f2;
	uint8_t i;

	const uint8_t STEP = 17;
	const uint16_t ADC_RESULT_MIN = 300;
	const uint16_t ADC_RESULT_MAX = 4000;

	// R(25) = 10K; B(25/85) = 3435
	const uint32_t Rt[17] = {190953, 112440, 68260, 42636, 27348, 17979, 12094,	8311, 5825,	4158, 3020,	2228, 1668,	1267, 974, 758, 597};


//------------------------------
	if((adc_data < ADC_RESULT_MIN) || (adc_data > ADC_RESULT_MAX))		return(250);		// Обрыв или КЗ

	//------------------------------
	R = 5100 * adc_data;
	R = R / (4095 - adc_data);

	if(R > Rt[0])				T = -40;
	else if (R < Rt[STEP-1])	T = +120;
	else
	{
		i = 0;
		while(R < Rt[i])	i++;

		f1 = Rt[i-1] - R;
		f2 = Rt[i-1] - Rt[i];
		T = -50 + (i * 10) +  10 * f1 / f2;
	}

	return(T);
}
//------------------------------------------------------------------------------------------------------------------------------------
void delay_1ms(uint16_t t)
{
	delay_counter_1ms = t;

	while(delay_counter_1ms)
		HAL_IWDG_Refresh(&hiwdg);			// IWDG настроен на 500мсек
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == ZC_3_Pin)
		boiler.zero_cross.phase_3 = 1;
	else if(GPIO_Pin == ZC_2_Pin)
		boiler.zero_cross.phase_2 = 1;
	else if(GPIO_Pin == ZC_1_Pin)
		boiler.zero_cross.phase_1 = 1;
	else if(GPIO_Pin == TERMOSTAT_Pin)
		boiler.termostat.value = HAL_GPIO_ReadPin(TERMOSTAT_GPIO_Port, TERMOSTAT_Pin);
	else if (GPIO_Pin == OVERTEMP_ALARM_Pin)
		boiler.errors.e03 = !HAL_GPIO_ReadPin(TERMOSTAT_GPIO_Port, TERMOSTAT_Pin);
}
//--------------------------------------------------------------------------------
/* USER CODE END 1 */
