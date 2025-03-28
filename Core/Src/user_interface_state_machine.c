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

void user_interface_update(void)
{
	switch(boiler.parameters.user_interface_step)
	{
	case 0:
		switch (boiler.parameters.boiler_type)
		{
		case 4:
			display_print_text(0, 0, '4');
			break;
		case 8:
			display_print_text(0, 0, '8');
			break;
/*		case 12:
			display_print_text(0, '1', '2');
			break;*/
		}

		boiler.parameters.user_interface_step = 1;
		delay_counter_1ms = 3000;
		break;

	case 1:
		if (delay_counter_1ms != 0)
			break;

		if (boiler.termostat.value == 0)
		{
			display_print_text('0', 'F', 'F');
			break;
		}

		display_convetr_temperature(boiler.heater.output.sensor.tC_Average);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 2;

		break;

	case 2:
		if (delay_counter_1ms != 0)
			break;

		boiler.upn.parameters.phase_detector_accum = TIM14->CNT - TIM16->CNT;
		if (boiler.upn.parameters.phase_detector_accum < 10 && boiler.upn.parameters.phase_detector_accum > -10)
		{
			boiler.upn.parameters.phase_detector_accum = TIM14->CNT - TIM17->CNT;
			if (boiler.upn.parameters.phase_detector_accum < 10 && boiler.upn.parameters.phase_detector_accum > -10)
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

		display_convetr_temperature(boiler.heater.output.sensor.tC_Average);
		//display_convetr_temperature((uint8_t)*(boiler.ten1.upn.signal));

		if (boiler.heater.output.sensor.Flag_Enable == 0)
		{
			display_print_text('E', '0', '1');
			boiler.errors.e01 = 1;
			delay_counter_1ms = 3000;
			boiler.display.blink_status = 1;
			boiler.parameters.user_interface_step = 10;
			break;
		}

		if (boiler.errors.e03 == 1)
		{
			display_print_text('E', '0', '3');
			delay_counter_1ms = 3000;
			boiler.display.blink_status = 1;
			boiler.parameters.user_interface_step = 10;
			break;
		}

		if (boiler.errors.e02 == 1 || boiler.errors.e04 == 1 || boiler.errors.e05 == 1)
		{
			if (boiler.errors.e02 == 1)
				display_print_text('E', '0', '2');
			else if (boiler.errors.e04 == 1)
				display_print_text('E', '0', '4');
			else
				display_print_text('E', '0', '5');

			delay_counter_1ms = 3000;
			boiler.display.blink_status = 1;
			boiler.parameters.user_interface_step = 11;
			break;
		}

		if (boiler.ten1.upn.is_active || boiler.ten2.upn.is_active || boiler.ten3.upn.is_active)
		{
			display_print_text('y', 'p', 'n');
			delay_counter_1ms = 3000;
			boiler.display.blink_status = 1;
			boiler.parameters.user_interface_step = 12;
			break;
		}

		if(boiler.button[LEFT_KEY].flag_short == 1 || boiler.button[RIGHT_KEY].flag_short == 1)
		{
			boiler.parameters.user_interface_step = 5;
			break;
		}

		boiler.display.blink_status = 0;
		delay_counter_500ms = 0;
		boiler.parameters.user_interface_step = 4;
		break;

	case 4:
		if (delay_counter_500ms == 10)
			boiler.parameters.user_interface_step = 3;
		break;

	case 5:
		if(boiler.button[LEFT_KEY].flag_off && boiler.button[RIGHT_KEY].flag_off)
		{
			display_convetr_temperature(boiler.heater.parameters.aim_temp);
			delay_counter_1ms = 5000;
			boiler.display.blink_status = 1;
			boiler.parameters.user_interface_step = 6;
			break;
		}

		if ((boiler.button[LEFT_KEY].flag_long == 1) || (boiler.button[RIGHT_KEY].flag_long == 1))
		{
			if ((boiler.button[LEFT_KEY].flag_short == 1) && (boiler.button[RIGHT_KEY].flag_short == 1))
			{
				switch(boiler.parameters.boiler_type)
				{
				case 4:
					display_print_text(0, 0, '4');
					break;
				case 8:
					display_print_text(0, 0, '8');
					break;
/*				case 12:
					display_print_text(0, '1', '2');
					break;*/
				}

				boiler.parameters.user_interface_step = 14;
				delay_counter_1ms = 5000;
				boiler.display.blink_status = 1;
			}
			else
			{
				boiler.parameters.user_interface_step = 6;
				delay_counter_1ms = 3000;
				boiler.display.blink_status = 1;
			}
		}
		break;

	case 6:
		if (delay_counter_1ms == 0)
		{
			boiler.display.blink_status = 0;
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
		display_convetr_temperature(boiler.heater.parameters.aim_temp);
		boiler.parameters.user_interface_step = 7;
		break;

	case 7:
		if (delay_counter_500ms == 500)
			boiler.parameters.user_interface_step = 5;

		break;

	case 10:
		delay_counter_1ms = 3000;
		break;

	case 11:
		if (delay_counter_1ms == 0)
		{
			display_convetr_temperature(boiler.heater.output.sensor.tC_Average);
			delay_counter_1ms = 3000;
			boiler.parameters.user_interface_step = 2;
		}
		break;

	case 12:
		if (delay_counter_1ms == 0)
		{
			display_convetr_temperature(boiler.heater.output.sensor.tC_Average);
			delay_counter_1ms = 3000;
			boiler.parameters.user_interface_step = 13;
		}
		break;

	case 13:
		if (delay_counter_1ms == 0)
		{
			if (boiler.ten1.upn.is_active == 0 && boiler.ten2.upn.is_active == 0 && boiler.ten3.upn.is_active == 0)
				boiler.display.blink_status = 0;

			boiler.parameters.user_interface_step = 3;
		}
		break;

	case 14:
		if(boiler.button[LEFT_KEY].flag_off && boiler.button[RIGHT_KEY].flag_off)
		{
			boiler.parameters.user_interface_step = 15;
			boiler.display.blink_status = 1;
		}
		break;

	case 15:
		if (delay_counter_1ms == 0)
		{
			boiler.display.blink_status = 0;
			boiler.parameters.user_interface_step = 20;
		}

		if(boiler.button[LEFT_KEY].flag_short == 0 && boiler.button[RIGHT_KEY].flag_short == 0)
			break;

		delay_counter_1ms = 5000;

		if(boiler.button[LEFT_KEY].flag_short)
		{
			switch(boiler.parameters.boiler_type)
			{
/*			case 4:
				boiler.parameters.boiler_type = 12;
				display_print_text(0, '1', '2');
				break;*/
			case 8:
				boiler.parameters.boiler_type = 4;
				display_print_text(0, 0, '4');
				break;
			case 4:
				boiler.parameters.boiler_type = 8;
				display_print_text(0, 0, '8');
				break;
			}
		}
		else if(boiler.button[RIGHT_KEY].flag_short)
		{
			switch(boiler.parameters.boiler_type)
			{
			case 4:
				boiler.parameters.boiler_type = 8;
				display_print_text(0, 0, '8');
				break;
/*			case 8:
				boiler.parameters.boiler_type = 12;
				display_print_text(0, '1', '2');
				break;*/
			case 8:
				boiler.parameters.boiler_type = 4;
				display_print_text(0, 0, '4');
				break;
			}
		}
		delay_counter_500ms = 200;
		boiler.parameters.user_interface_step = 16;
		break;

	case 16:
		if (delay_counter_500ms == 500)
			boiler.parameters.user_interface_step = 14;
		break;

	case 20:
		eeprom_write_setting(boiler.heater.parameters.aim_temp << 8 | boiler.parameters.boiler_type, FLASH_PAGE_ADDR_31);
		display_convetr_temperature(boiler.heater.output.sensor.tC_Average);
		boiler.parameters.user_interface_step = 3;
		break;
	}
}
