/*
 * fan.c
 *
 *  Created on: Jul 28, 2025
 *      Author: GalkinN
 */
#include "main.h"

extern struct sBoiler boiler;

void fan_ctrl()
{
	  if (boiler.heatsink.sensor.Flag_Enable == 0)
	  {
		  boiler.fan.speed = FAN_SPEED_FAST;
		  set_e04();
	  }
	  else
	  {
		  if (boiler.heatsink.sensor.tC_Average > OVERHEAT_TEMPERATURE_ALL_SIMMISTORS_OFF)
			  boiler.ten1.is_active = 0;
		  else if (boiler.heatsink.sensor.tC_Average > OVERHEAT_TEMPERATURE_TWO_SIMMISTORS_OFF)
			  boiler.ten2.is_active = 0;
		  else if (boiler.heatsink.sensor.tC_Average > OVERHEAT_TEMPERATURE_ONE_SIMMISTOR_OFF)
			  boiler.ten3.is_active = 0;
		  else if (boiler.heatsink.sensor.tC_Average > HEATSINK_FAN_FAST_MIN_TEMPERATURE)
			  boiler.fan.speed = FAN_SPEED_FAST;
		  else if ((boiler.heatsink.sensor.tC_Average > HEATSINK_FAN_SLOW_MIN_TEMPERATURE) && (boiler.heatsink.sensor.tC_Average < HEATSINK_FAN_SLOW_MAX_TEMPERATURE))
			  boiler.fan.speed = FAN_SPEED_NORMAL;
		  else if (boiler.heatsink.sensor.tC_Average < HEATSINK_FAN_OFF_TEMPERATURE)
			  boiler.fan.speed = FAN_SPEED_OFF;

		  if (boiler.heatsink.sensor.tC_Average > OVERHEAT_TEMPERATURE_ONE_SIMMISTOR_OFF)
		  {
			  boiler.fan.speed = FAN_SPEED_FAST;
			  set_e05();
		  }
		  else
			  reset_e05();
	  }

	  HAL_GPIO_WritePin(FAN_LOW_GPIO_Port, FAN_LOW_Pin, boiler.fan.speed);
	  HAL_GPIO_WritePin(FAN_HIGH_GPIO_Port, FAN_HIGH_Pin, boiler.fan.speed >> 1);
}
