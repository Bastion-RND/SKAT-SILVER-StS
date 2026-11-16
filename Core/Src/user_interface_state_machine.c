/*
 * user_interface_state_machine.c
 *
 *  Created on: Mar 25, 2025
 *      Author: GalkinN
 */

#include "main.h"

extern struct sBoiler boiler;
extern uint16_t delay_counter_1ms;
extern uint16_t delay_counter_500ms;

void key_state_update()
{
	key_polling(RIGHT_KEY);
	key_polling(LEFT_KEY);
}

uint8_t read_key(uint8_t n)
{
	if(n == 0)	return(HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin));
	else		return(HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin));
}
//--------------------------------------------------------------------------------------------
void key_polling(uint8_t n)
{
	if(read_key(n) == 0)
	{
		if(boiler.button[n].counter_off)
			boiler.button[n].counter_off --;

		if(boiler.button[n].counter_on < LONG_INTERVAL)
			boiler.button[n].counter_on ++;

		if((boiler.button[n].counter_on > SHORT_INTERVAL) && (boiler.button[n].counter_on < 4*SHORT_INTERVAL))
		{
			boiler.button[n].flag_short = 1;
			boiler.button[n].flag_off = 0;
		}
		else if(boiler.button[n].counter_on == LONG_INTERVAL)
			boiler.button[n].flag_long  = 1;
	}
	else
	{
		if(boiler.button[n].counter_on)
			boiler.button[n].counter_on --;

		if(boiler.button[n].counter_off < LONG_INTERVAL_OFF)
		{
			boiler.button[n].counter_off ++;
			if(boiler.button[n].counter_off == SHORT_INTERVAL)
				boiler.button[n].counter_on = 0;
		}
		else
		{
			boiler.button[n].flag_short = 0;
			boiler.button[n].flag_long  = 0;
			boiler.button[n].flag_off = 1;
		}
	}
}

void show_list_of_possible_values()
{
	if (delay_counter_1ms != 0)
		return;

	for (; boiler.display.value_to_show < U16; boiler.display.value_to_show <<= 1)
	{
		if (boiler.errors&boiler.display.value_to_show)
		{
			boiler.display.error_flag = 1;
			break;
		}
	}

	if (boiler.display.error_flag == 1)
	{
		switch(boiler.display.value_to_show)
		{
		case (E01):
			display_print_text('E', '0', '1');
			break;
		case (E02):
			display_print_text('E', '0', '2');
			break;
		case (E03):
			display_print_text('E', '0', '3');
			break;
		case (E04):
			display_print_text('E', '0', '4');
			break;
		case (E05):
			display_print_text('E', '0', '5');
			break;
		case (E06):
			if (boiler.testMode.flag == 0)
				display_print_text('F', '0', '1');
			else
				display_print_text('E', '0', '6');
			break;
		case (E07):
			if (boiler.testMode.flag == 0)
				display_print_text('F', '0', '2');
			else
				display_print_text('E', '0', '7');
			break;
		case (E08):
			if (boiler.testMode.flag == 0)
				display_print_text('F', '0', '3');
			else
				display_print_text('E', '0', '8');
			break;
		case (E09):
			if (boiler.testMode.flag == 0)
				display_print_text('U', '0', '1');
			else
				display_print_text('E', '0', '9');
			break;
		case (E10):
			if (boiler.testMode.flag == 0)
				display_print_text('U', '0', '2');
			else
				display_print_text('E', '1', '0');
			break;
		case (E11):
			if (boiler.testMode.flag == 0)
				display_print_text('U', '0', '3');
			else
				display_print_text('E', '1', '1');
			break;
		case (E12):
			display_print_text('E', '1', '2');
			break;
		case (E13):
			display_print_text('E', '1', '3');
			break;
		case (E14):
			display_print_text('E', '1', '4');
			break;
		case (E15):
			display_print_text('E', '1', '5');

		case (U01):
			display_print_text('U', '0', '1');
			break;
		case (U02):
			display_print_text('U', '0', '2');
			break;
		case (U03):
			display_print_text('U', '0', '3');
			break;
		case (U04):
			display_print_text('U', '0', '4');
			break;
		case (U05):
			display_print_text('U', '0', '5');
			break;
		case (U06):
			display_print_text('U', '0', '6');
			break;
		case (U07):
			display_print_text('U', '0', '7');
			break;
		case (U08):
			display_print_text('U', '0', '8');
			break;
		case (U09):
			display_print_text('U', '0', '9');
			break;
		case (U10):
			display_print_text('U', '1', '0');
			break;
		case (U11):
			display_print_text('U', '1', '1');
			break;
		case (U12):
			display_print_text('U', '1', '2');
			break;
		case (U13):
			display_print_text('U', '1', '3');
			break;
		case (U14):
			display_print_text('U', '1', '4');
			break;
		case (U15):
			display_print_text('U', '1', '5');
			break;
		case (U16):
			display_print_text('U', '1', '6');
			break;
		}

	}

	if (boiler.testMode.flag == 0)
		delay_counter_1ms = 3000;
	else
		delay_counter_1ms = 1000;

	boiler.display.error_flag = 0;

	if (boiler.display.value_to_show == U16)
	{
		boiler.display.value_to_show = 1;
		if (boiler.testMode.flag == 0)
		{
			display_convert_temperature(boiler.heater.output.sensor.tC_Average);
			delay_counter_1ms = 3000;
		}
		else
			delay_counter_1ms = 1000;

	}
	else
		boiler.display.value_to_show <<= 1;
}

void user_interface_update(void)
{
	switch(boiler.parameters.user_interface_step)
	{
	case 0:
		if (boiler.testMode.flag == 1)
			return;

		switch (boiler.parameters.boiler_type)
		{
		case 0:
			display_print_text('P', '_', '1');
			break;
		case 1:
			display_print_text('P', '_', '2');
			break;
		case 2:
			display_print_text('P', '_', '3');
			break;
		}

		boiler.parameters.user_interface_step = 1;
		delay_counter_1ms = 5000;
		break;

	case 1:
		if (delay_counter_1ms != 0)
			break;

		boiler.parameters.boot_flag = 1;

		if (boiler.termostat.value == 0)
		{
			display_print_text('0', 'F', 'F');
			break;
		}

		display_convert_temperature(boiler.heater.output.sensor.tC_Average);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 2;

		break;

	case 2:
		if (delay_counter_1ms != 0)
			break;

		boiler.upn.parameters.phase_detector_accum = TIM14->CNT - TIM16->CNT;
		if ((boiler.upn.parameters.phase_detector_accum < 10) && (boiler.upn.parameters.phase_detector_accum > -10))
		{
			boiler.upn.parameters.phase_detector_accum = TIM14->CNT - TIM17->CNT;
			if ((boiler.upn.parameters.phase_detector_accum < 10) && (boiler.upn.parameters.phase_detector_accum > -10))
				set_upn_type(1);
			else
				set_upn_type(3);
		}
		else
			set_upn_type(3);

		boiler.parameters.user_interface_step = 3;
		boiler.parameters.boot_flag = 1;
		break;

	case 3:
		if (boiler.termostat.value == 0)
		{
			display_print_text('0', 'F', 'F');
			break;
		}

		show_list_of_possible_values();

		if((boiler.button[LEFT_KEY].flag_short == 1) || (boiler.button[RIGHT_KEY].flag_short == 1))
		{
			boiler.parameters.user_interface_step = 5;
			break;
		}

		boiler.display.blink_status = DISPLAY_SHOW;
		delay_counter_500ms = 0;
		boiler.parameters.boot_flag = 1;
		boiler.parameters.user_interface_step = 4;
		break;

	case 4:
		if (delay_counter_500ms > 400)
			boiler.parameters.user_interface_step = 3;
		break;

	case 5:
		if(boiler.button[LEFT_KEY].flag_off && boiler.button[RIGHT_KEY].flag_off)
		{
			display_convert_temperature(boiler.heater.parameters.aim_temp);
			delay_counter_1ms = 5000;
			boiler.display.blink_status = DISPLAY_BLINK;
			boiler.parameters.user_interface_step = 6;
			break;
		}

		if ((boiler.button[LEFT_KEY].flag_long == 1) || (boiler.button[RIGHT_KEY].flag_long == 1))
		{
			if ((boiler.button[LEFT_KEY].flag_short == 1) && (boiler.button[RIGHT_KEY].flag_short == 1))
			{
				switch(boiler.parameters.boiler_type)
				{
				case 0:
					display_print_text('P', '_', '1');
					break;
				case 1:
					display_print_text('P', '_', '2');
					break;
				case 2:
					display_print_text('P', '_', '3');
					break;
				}

				boiler.parameters.user_choice = boiler.parameters.boiler_type;
				boiler.parameters.user_interface_step = 14;
				delay_counter_1ms = 5000;
			}
			else
			{
				boiler.parameters.user_interface_step = 6;
				delay_counter_1ms = 3000;
			}
			boiler.display.blink_status = DISPLAY_BLINK;
		}
		break;

	case 6:
		if (delay_counter_1ms == 0)
		{
			boiler.display.blink_status = DISPLAY_SHOW;
			boiler.parameters.user_choice = boiler.parameters.boiler_type;
			boiler.parameters.user_interface_step = 20;
		}

		if(boiler.button[LEFT_KEY].flag_short == 0 && boiler.button[RIGHT_KEY].flag_short == 0)
			break;

		if(boiler.button[LEFT_KEY].flag_short)
		{
			if(boiler.heater.parameters.aim_temp > SetTC_MIN)
				boiler.heater.parameters.aim_temp--;
		}
		else if(boiler.button[RIGHT_KEY].flag_short)
		{
			if(boiler.heater.parameters.aim_temp < SetTC_MAX)
				boiler.heater.parameters.aim_temp++;
		}

		delay_counter_500ms = 200;
		display_convert_temperature(boiler.heater.parameters.aim_temp);
		boiler.parameters.user_interface_step = 7;
		break;

	case 7:
		if (delay_counter_500ms > 400)
			boiler.parameters.user_interface_step = 5;

		break;

	case 10:
		delay_counter_1ms = 3000;
		break;

	case 11:
		if (delay_counter_1ms == 0)
		{
			display_convert_temperature(boiler.heater.output.sensor.tC_Average);
			delay_counter_1ms = 3000;
			boiler.parameters.user_interface_step = 2;
		}
		break;

	case 12:
		if (delay_counter_1ms == 0)
		{
			display_convert_temperature(boiler.heater.output.sensor.tC_Average);
			delay_counter_1ms = 3000;
			boiler.parameters.user_interface_step = 13;
		}
		break;

	case 13:
		if (delay_counter_1ms == 0)
		{
			if (boiler.ten1.upn.is_active == 0 && boiler.ten2.upn.is_active == 0 && boiler.ten3.upn.is_active == 0)
				boiler.display.blink_status = DISPLAY_SHOW;

			boiler.parameters.user_interface_step = 3;
		}
		break;

	case 14:
		if(boiler.button[LEFT_KEY].flag_off && boiler.button[RIGHT_KEY].flag_off)
		{
			boiler.parameters.user_interface_step = 15;
			boiler.display.blink_status = DISPLAY_BLINK;
		}
		break;

	case 15:
		if (delay_counter_1ms < 10)
		{
			boiler.display.blink_status = DISPLAY_SHOW;
			boiler.parameters.user_interface_step = 20;
		}

		if(boiler.button[LEFT_KEY].flag_short == 0 && boiler.button[RIGHT_KEY].flag_short == 0)
			break;

		delay_counter_1ms = 5000;

		if(boiler.button[LEFT_KEY].flag_short)
		{
			switch(boiler.parameters.user_choice)
			{
			case 0:
				boiler.parameters.user_choice = 2;
				display_print_text('P', '_', '3');
				break;
			case 1:
				boiler.parameters.user_choice = 0;
				display_print_text('P', '_', '1');
				break;
			case 2:
				boiler.parameters.user_choice = 1;
				display_print_text('P', '_', '2');
				break;
			}
		}
		else if(boiler.button[RIGHT_KEY].flag_short)
		{
			switch(boiler.parameters.user_choice)
			{
			case 0:
				boiler.parameters.user_choice = 1;
				display_print_text('P', '_', '2');
				break;
			case 1:
				boiler.parameters.user_choice = 2;
				display_print_text('P', '_', '3');
				break;
			case 2:
				boiler.parameters.user_choice = 0;
				display_print_text('P', '_', '1');
				break;
			}
		}
		delay_counter_500ms = 200;
		boiler.parameters.user_interface_step = 16;
		break;

	case 16:
		if (delay_counter_500ms > 400)
			boiler.parameters.user_interface_step = 14;
		break;

	case 20:
		boiler.parameters.boiler_type = (boiler.parameters.user_choice > 2) ? 2 : boiler.parameters.user_choice;
		eeprom_write_setting((boiler.heater.parameters.aim_temp << 8 | boiler.parameters.boiler_type), FLASH_PAGE_ADDR_31);
		display_convert_temperature(boiler.heater.output.sensor.tC_Average);
		boiler.parameters.user_interface_step = 3;
		break;
	}
}
