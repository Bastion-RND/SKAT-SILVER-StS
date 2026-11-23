/*
 * one_second_event_handler.c
 *
 *  Created on: Mar 26, 2025
 *      Author: GalkinN
 */

#include "main.h"

extern struct sBoiler boiler;
extern uint32_t tickstart_1sec;

extern struct sSensor* list_of_sensors[2];
extern uint8_t sensors_list_size;
extern uint8_t ten1_output, ten2_output, ten3_output;

void ten_rotate(struct sTEN* dst, struct sTEN* src)
{
	dst->zero_cross = src->zero_cross;
	dst->physical_output = src->physical_output;
	if (boiler.upn.parameters.type == 3)
		dst->upn.signal = src->upn.signal;
}

void check_rotation()
{
	  if (get_e03() == 1)
		  return;

	  if (boiler.timers.rotation_timer < TEN_ROTATION_PERIOD_SEC)  //if (boiler.timers.rotation_timer < TEN_ROTATION_PERIOD_SEC)
	    ++boiler.timers.rotation_timer;
	  else
	  {
		  struct sTEN* activeTENs[3] = {NULL,NULL,NULL};
		  uint8_t iterator = 0;

		  if (*(boiler.ten1.zero_cross) == 1)
		  {
			  activeTENs[iterator] = &boiler.ten1;
			  iterator++;
		  }

		  if (*(boiler.ten2.zero_cross) == 1)
		  {
			  activeTENs[iterator] = &boiler.ten2;
			  iterator++;
		  }

		  if (*(boiler.ten3.zero_cross) == 1)
		  {
			  activeTENs[iterator] = &boiler.ten3;
			  iterator++;
		  }

		  boiler.timers.rotation_timer = 0;

		  if (iterator < 2)
			  return;

		  struct sTEN ten_accum = {0};

		  ten_rotate(&ten_accum, activeTENs[0]);
		  ten_rotate(activeTENs[0], activeTENs[1]);

		  if (activeTENs[2] != NULL)
		  {
			  ten_rotate(activeTENs[1], activeTENs[2]);
			  ten_rotate(activeTENs[2], &ten_accum);
		  }
		  else
			  ten_rotate(activeTENs[1], &ten_accum);
	  }
}

void one_second_event_handler(void)
{
	  if((HAL_GetTick() - tickstart_1sec) < 1000)
		  return;

	  tickstart_1sec = HAL_GetTick();
	  HAL_IWDG_Refresh(&hiwdg);

	  if ((boiler.testMode.flag == 1) || (boiler.parameters.boot_flag == 0))
		  return;

	  if (boiler.parameters.temperature_alarm.tC_Average < 100)
		  set_e03();

	  if (get_e04() == 1)
		  boiler.parameters.boiler_type = 0;

	  if (boiler.heater.output.sensor.Flag_Enable == 0)
		  set_e01();

	  boiler.pump.time_of_activity = (boiler.pump.pump_timer_ON == 1) ?  boiler.pump.time_of_activity + 1 : 0;
	  boiler.timers.counter_15_min = (boiler.timers.counter_15_min < 960) ? boiler.timers.counter_15_min + 1 : 0;

	  if (get_e03() == 0)
	  {
		  if (boiler.zero_cross.phase_1 == 0)
			  set_f01();
		  else
			  reset_f01();

		  if (boiler.zero_cross.phase_2 == 0)
			  set_f02();
		  else
			  reset_f02();

		  if (boiler.zero_cross.phase_3 == 0)
			  set_f03();
		  else
			  reset_f03();
	  }
	  else
	  {
		  reset_f01();
		  reset_f02();
		  reset_f03();
	  }

	  check_rotation();

	  if (boiler.ten1.upn.is_active == 1)
	  {
		  boiler.ten1.upn.timer++;
		  set_u01();
	  }
	  else
		  reset_u01();

	  if (boiler.ten2.upn.is_active == 1)
	  {
		  boiler.ten2.upn.timer++;
		  set_u02();
	  }
	  else
		  reset_u02();

	  if (boiler.ten3.upn.is_active == 1)
	  {
		  boiler.ten3.upn.timer++;
		  set_u03();
	  }
	  else
		  reset_u03();

	  if (boiler.timers.counter_5_sec < 5)
		  boiler.timers.counter_5_sec++;
	  else
	  {
		  boiler.timers.counter_5_sec = 0;
		  boiler.ten1.upn.check = 1;
		  boiler.ten2.upn.check = 1;
		  boiler.ten3.upn.check = 1;
	  }

	  if ((get_e01() == 0) && (get_e03() == 0) && (boiler.termostat.value == 1) && (boiler.pump.activate == 1))
	  {
		  *(boiler.ten1.physical_output) = (boiler.ten1.is_active) ? 1 : 0;
		  *(boiler.ten2.physical_output) = (boiler.ten2.is_active) ? 1 : 0;
		  *(boiler.ten3.physical_output) = (boiler.ten3.is_active) ? 1 : 0;

		  HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, (get_f01() == 0) ? ten1_output : 0);
		  HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, (get_f02() == 0) ? ten2_output : 0);
		  HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, (get_f03() == 0) ? ten3_output : 0);
	  }
	  else
	  {
		  *(boiler.ten1.physical_output) = 0;
		  *(boiler.ten2.physical_output) = 0;
		  *(boiler.ten3.physical_output) = 0;

		  HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, ten1_output);
		  HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, ten2_output);
		  HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, ten3_output);
		  boiler.display.led_status = LED_OFF;
	  }

	  if ((ten1_output == 1) || (ten2_output == 1) || (ten3_output == 1))
		  boiler.display.led_status = LED_LIGHT;
	  else
		  boiler.display.led_status = LED_OFF;

	  if (get_u01() || get_u02() || get_u03())
		  boiler.display.led_status = LED_BLINK;

	  if (boiler.pump.activate == 1)
		  HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, 1);
	  else
		  HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, 0);

	  fan_ctrl();

	  boiler.zero_cross.phase_1 = 0;
	  boiler.zero_cross.phase_2 = 0;
	  boiler.zero_cross.phase_3 = 0;
}
