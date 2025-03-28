/*
 * one_second_event_handler.c
 *
 *  Created on: Mar 26, 2025
 *      Author: GalkinN
 */

#include "main.h"

extern struct sBoiler boiler;
extern uint32_t tickstart_1sec;
extern uint8_t ten1_output, ten2_output, ten3_output;
struct sTEN ten_accum = {0};

void ten_rotate(struct sTEN* dst, struct sTEN* src)
{
	dst->zero_cross = src->zero_cross;
	dst->physical_output = src->physical_output;
	if (boiler.upn.parameters.type == 3)
	{
		dst->upn.signal = src->upn.signal;
		//dst->upn.is_active = src->upn.is_active;
		//dst->upn.timer = src->upn.timer;
	}
}

void one_second_event_handler(void)
{
	  if((HAL_GetTick() - tickstart_1sec) < 1000)
		  return;

	  boiler.pump.time_of_activity = (boiler.pump.pump_timer_ON == 1) ?  boiler.pump.time_of_activity + 1 : 0;
	  boiler.timers.counter_15_min = (boiler.timers.counter_15_min < 960) ? boiler.timers.counter_15_min + 1 : 0;

	  if (boiler.heatsink.sensor.tC_Average > 95)
		  boiler.ten1.is_active = 0;
	  else if (boiler.heatsink.sensor.tC_Average > 90)
		  boiler.ten2.is_active = 0;
	  else if (boiler.heatsink.sensor.tC_Average > 85)
		  boiler.ten3.is_active = 0;
	  else if (boiler.heatsink.sensor.tC_Average > 60)
	  {
		  boiler.fan.low_speed_flag = 0;
		  boiler.fan.high_speed_flag = 1;
	  }
	  else if (boiler.heatsink.sensor.tC_Average > 40)
	  {
		  boiler.fan.low_speed_flag = 1;
		  boiler.fan.high_speed_flag = 0;
	  }
	  else if (boiler.heatsink.sensor.tC_Average < 30)
	  {
		  boiler.fan.low_speed_flag = 0;
		  boiler.fan.high_speed_flag = 0;
	  }

	  if (boiler.heatsink.sensor.tC_Average < 80)
	  	  boiler.errors.e04 = 0;
	  else if (boiler.heatsink.sensor.tC_Average > 85)
	  	  boiler.errors.e04 = 1;

	  HAL_GPIO_WritePin(FAN_LOW_GPIO_Port, FAN_LOW_Pin, boiler.fan.low_speed_flag);
	  HAL_GPIO_WritePin(FAN_HIGH_GPIO_Port, FAN_HIGH_Pin, boiler.fan.high_speed_flag);

	  /*if (boiler.zero_cross.phase_1 == 0 || boiler.zero_cross.phase_2 == 0 || boiler.zero_cross.phase_3 == 0)
		  boiler.errors.e05 = 1;
	  else
		  boiler.errors.e05 = 0;*/

	  boiler.zero_cross.phase_1 = 0;
	  boiler.zero_cross.phase_2 = 0;
	  boiler.zero_cross.phase_3 = 0;

	  if (boiler.ten1.upn.is_active == 1)
		  boiler.ten1.upn.timer++;

	  if (boiler.ten2.upn.is_active == 1)
		  boiler.ten2.upn.timer++;

	  if (boiler.ten3.upn.is_active == 1)
		  boiler.ten3.upn.timer++;

	  if (boiler.timers.counter_5_sec < 5)
		  boiler.timers.counter_5_sec++;
	  else
	  {
		  boiler.timers.counter_5_sec = 0;
		  boiler.ten1.upn.check = 1;
		  boiler.ten2.upn.check = 1;
		  boiler.ten3.upn.check = 1;
	  }

	  if (boiler.timers.rotation_timer < 30)
	    ++boiler.timers.rotation_timer;
	  else
	  {
		  boiler.timers.rotation_timer = 0;
		  ten_accum = boiler.ten1;
		  ten_rotate(&(boiler.ten1), &(boiler.ten2));
		  ten_rotate(&(boiler.ten2), &(boiler.ten3));
		  ten_rotate(&(boiler.ten3), &(ten_accum));
	  }

	  if ((boiler.errors.e01 == 0) && (boiler.errors.e03 == 0) && (boiler.termostat.value == 1) && (boiler.errors.e04 == 0))
	  {
		  *(boiler.ten1.physical_output) = (boiler.ten1.is_active) ? 1 : 0;
		  *(boiler.ten2.physical_output) = (boiler.ten2.is_active) ? 1 : 0;
		  *(boiler.ten3.physical_output) = (boiler.ten3.is_active) ? 1 : 0;


		  HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, ten1_output);
		  HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, ten2_output);
		  HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, ten3_output);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, 0);
		  HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, 0);
		  HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, 0);
	  }

	  HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, boiler.pump.activate);

	  tickstart_1sec = HAL_GetTick();
	  HAL_IWDG_Refresh(&hiwdg);
}
