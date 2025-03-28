/*
 * heating_mode.c
 *
 *  Created on: Mar 25, 2025
 *      Author: GalkinN
 */
#include "main.h"
extern struct sBoiler boiler;

uint8_t temperature_threshold_minimum, temperature_threshold_maximum = 0;


uint8_t boiler_temperature_below_limit(float boiler_temp, uint8_t temperature_threshold, uint8_t sensor_enable_flag) {
	return (boiler_temp < temperature_threshold)&&sensor_enable_flag;
}

void activate_ten(struct sTEN* ten, uint8_t try_ten)
{
	ten->is_active = 1;
	boiler.parameters.try_activate_ten = try_ten;
}

void adjust_ten_according_temperature_threshold_minimum(struct sTEN* ten, float threshold_minimum)
{
	if (boiler.heater.parameters.temp_accum > threshold_minimum)
	{
		boiler.heater.parameters.temp_accum = threshold_minimum;
		ten->is_active = 1;
	}
}

void adjust_ten_according_temperature_threshold_maximum(struct sTEN* ten, float threshold_maximum,  uint8_t ten_activity) {
	if (boiler.heater.parameters.temp_accum < threshold_maximum)
	{
		boiler.heater.parameters.temp_accum = threshold_maximum;
		ten->is_active = ten_activity;
	}
}

void set_pump_timer(uint8_t timer_status, uint16_t _time) {
	boiler.pump.time_of_activity = _time;
	boiler.pump.pump_timer_ON = timer_status;
}

void reset_heating_algorithm() {
	boiler.heater.parameters.algorithm_step = 0;
}

void turn_on_all_heaters() {
	boiler.ten1.is_active = 1;
	boiler.ten2.is_active = 1;
	boiler.ten3.is_active = 1;
}

void disable_all_heaters(){
	boiler.ten1.is_active = 0;
	boiler.ten2.is_active = 0;
	boiler.ten3.is_active = 0;
}

void heating_update() {

	if (boiler.parameters.boot_flag == 0)
		return;

	switch (boiler.heater.parameters.algorithm_step)
	{
	case 0:
		boiler.pump.activate = 1;
		set_pump_timer(1, 0);
		boiler.heater.parameters.algorithm_step = 1;
		break;

	case 1:
		if (boiler.pump.time_of_activity > 10)				////if (boiler.pump.time_of_activity > INITIAL_WAIT_TIME)
		{
			set_pump_timer(0, 0);
			boiler.heater.parameters.algorithm_step = 2;
		}
		break;

	case 2:
		if (boiler_temperature_below_limit(boiler.heater.output.sensor.tC_Average, MIN_OPERATION_TEMPERATURE, boiler.heater.output.sensor.Flag_Enable))
			boiler.heater.parameters.algorithm_step = 3;
		else
			boiler.heater.parameters.algorithm_step = 10;
		break;

	case 3:
		boiler.errors.e02 = 1;
		set_pump_timer(1, 0);
		boiler.heater.parameters.algorithm_step = 4;
		break;

	case 4:
		if (boiler.pump.time_of_activity > WAIT_TIME_5_SEC)
		{
			boiler.timers.counter_15_min = 0;
			set_pump_timer(0, 0);
			boiler.heater.parameters.algorithm_step = 5;
		}
		break;

	case 5:
		if (boiler.heater.output.sensor.tC_Average < 30 && boiler.timers.counter_15_min < 900)
		{
			boiler.pump.activate = 1;
			turn_on_all_heaters();
		}
		else
			boiler.heater.parameters.algorithm_step = 6;
		break;

	  case 6:
		  boiler.errors.e02 = 0;
		  disable_all_heaters();
		  set_pump_timer(1, 0);
		  boiler.heater.parameters.algorithm_step = 7;
		  break;

	  case 7:
		  if (boiler.pump.time_of_activity > boiler.pump.pump_time_activity)
		  {
			  set_pump_timer(0, 0);
			  if (boiler.heater.output.sensor.tC_Average > boiler.heater.parameters.aim_temp)
				  boiler.pump.activate = 0;

			  boiler.heater.parameters.algorithm_step = 2;
		  }
		  break;

	  case 10:
		  if (boiler.timers.counter_15_min > 900)
		  {
			  boiler.pump.activate = 1;
			  set_pump_timer(1, 0);
			  boiler.timers.counter_15_min = 0;
			  boiler.heater.parameters.algorithm_step = 11;
		  }
		  else
			  boiler.heater.parameters.algorithm_step = 12;
		  break;

	  case 11:
		  if (boiler.pump.time_of_activity >  ONE_MINUTE)
		  {
			  set_pump_timer(0, 0);
			  boiler.heater.parameters.algorithm_step = 12;
		  }
		  break;

	  case 12:
		  boiler.heater.parameters.algorithm_step = (boiler.termostat.value == 1) ? 13 : 6;
		  break;

	  case 13:
		  boiler.parameters.try_activate_ten = 0;

		  if (boiler.heater.output.sensor.tC_Average >= (boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis - 3 + boiler.heater.parameters.gisterezis_coefficient))
			  boiler.heater.parameters.algorithm_step = 20;
		  else
		  {
			  disable_all_heaters();
			  boiler.pump.activate = 1;
			  boiler.heater.parameters.algorithm_step = 14;
		  }
		  break;

	  case 14:
		  set_pump_timer(1, 0);
		  switch(boiler.parameters.try_activate_ten)
		  {
		  case 0:
			  if (!boiler.ten1.is_active)
				  boiler.ten1.is_active = 1;

			  boiler.parameters.try_activate_ten = 1;
			  break;
		  case 1:
			  if (!boiler.ten2.is_active)
				  boiler.ten2.is_active = 1;

			  boiler.parameters.try_activate_ten = 2;
			  break;
		  case 2:
			  if (!boiler.ten3.is_active)
				  boiler.ten3.is_active = 1;

			  boiler.parameters.try_activate_ten = 0;
			  break;
		  }
		  boiler.timers.waiting_time = HALF_MINUTE;
		  boiler.heater.parameters.algorithm_step = 15;
		  break;

	case 15:
		if (boiler.pump.time_of_activity > boiler.timers.waiting_time)
			boiler.heater.parameters.algorithm_step = 14;
		else
		{
			if (boiler.termostat.value && (boiler.heater.output.sensor.tC_Average > (boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis_coefficient)))
			{
				boiler.heater.parameters.temp_accum = boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis_coefficient;
				set_pump_timer(0, 0);
				boiler.heater.parameters.algorithm_step = 20;
			}
			else if (boiler.termostat.value == 0)
				boiler.heater.parameters.algorithm_step = 6;
			else if (boiler_temperature_below_limit(boiler.heater.output.sensor.tC_Average, MIN_OPERATION_TEMPERATURE, boiler.heater.output.sensor.Flag_Enable))
			{
				boiler.heater.parameters.algorithm_step = 3;
				boiler.timers.counter_15_min = 0;
			}
		}
		break;

	case 20:
		if (boiler.heater.output.sensor.tC_Average > (boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis_coefficient))
			boiler.heater.parameters.algorithm_step = 30;
		else
		{
			if (boiler.heater.output.sensor.tC_Average < (boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis + boiler.heater.parameters.gisterezis_coefficient))
				boiler.heater.parameters.algorithm_step = 21;
			else
				boiler.heater.parameters.algorithm_step = 23;
		}
		break;

	case 21:
		boiler.pump.activate = 1;
		boiler.heater.parameters.algorithm_step = 22;
		temperature_threshold_minimum = boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis + boiler.heater.parameters.gisterezis_coefficient;
		break;

	case 22:
		if (boiler.heater.output.sensor.tC_Average > (temperature_threshold_minimum - 1))
			adjust_ten_according_temperature_threshold_minimum(&(boiler.ten1), (temperature_threshold_minimum - 1));
		else if (boiler.heater.output.sensor.tC_Average > (temperature_threshold_minimum - 2))
			adjust_ten_according_temperature_threshold_minimum(&(boiler.ten2), (temperature_threshold_minimum - 2));
		else
			adjust_ten_according_temperature_threshold_minimum(&(boiler.ten3), (temperature_threshold_minimum - 3));

		set_pump_timer(1, 0);
		boiler.heater.parameters.algorithm_step = 23;
		break;

	case 23:
		if (boiler.pump.time_of_activity > WAIT_TIME_5_SEC)
		{
			set_pump_timer(0, 0);
			boiler.heater.parameters.algorithm_step = 2;
		}
		break;

	case 30:
		boiler.heater.parameters.algorithm_step = 31;
		temperature_threshold_maximum = boiler.heater.parameters.aim_temp - boiler.heater.parameters.gisterezis_coefficient;
		break;

	case 31:
		if (boiler.heater.output.sensor.tC_Average > (temperature_threshold_maximum + 2))
		{
			boiler.heater.parameters.temp_accum = temperature_threshold_maximum + 2;
			boiler.heater.parameters.algorithm_step = 32;
		}
		else
		{
			if (boiler.heater.output.sensor.tC_Average > (temperature_threshold_maximum + 1))
				adjust_ten_according_temperature_threshold_maximum(&(boiler.ten2), (temperature_threshold_maximum + 1), 0);
			else
				adjust_ten_according_temperature_threshold_maximum(&(boiler.ten3), temperature_threshold_maximum, 0);

			set_pump_timer(1, 0);
			boiler.heater.parameters.algorithm_step = 23;
		}
		break;

	case 32:
		disable_all_heaters();
		set_pump_timer(1, 0);
		boiler.heater.parameters.algorithm_step = 33;
		break;

	case 33:
		if (boiler.pump.time_of_activity > boiler.pump.pump_time_activity)
		{
			set_pump_timer(1, 0);
			if (boiler.heater.output.sensor.tC_Average > temperature_threshold_maximum)
				boiler.pump.activate = 0;
			else
			{
				boiler.pump.activate = 1;
				boiler.timers.counter_15_min = 0;
			}

			boiler.heater.parameters.algorithm_step = 23;
		}
		else
		{
			if (boiler.heater.output.sensor.tC_Average < temperature_threshold_maximum)
			{
				set_pump_timer(1, 0);
				boiler.heater.parameters.algorithm_step = 23;
			}
		}

		break;
	}
}

