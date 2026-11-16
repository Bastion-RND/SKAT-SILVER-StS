/*
 * test.c
 *
 *  Created on: Jul 28, 2025
 *      Author: GalkinN
 */
#include "main.h"

extern struct sBoiler boiler;
extern uint16_t delay_counter_1ms;
extern uint16_t delay_counter_500ms;

void fun_relay_on()
{
	  HAL_GPIO_WritePin(FAN_LOW_GPIO_Port, FAN_LOW_Pin, 1);
	  HAL_GPIO_WritePin(FAN_HIGH_GPIO_Port, FAN_HIGH_Pin, 1);
}

void fun_relay_off()
{
	  HAL_GPIO_WritePin(FAN_LOW_GPIO_Port, FAN_LOW_Pin, 0);
	  HAL_GPIO_WritePin(FAN_HIGH_GPIO_Port, FAN_HIGH_Pin, 0);
}

void run_test()
{
	switch(boiler.parameters.user_interface_step)
	{
	case 100:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 101;
		break;

	case 101:
		print_display_check_layout();

		if (boiler.zero_cross.phase_1 == 0)
			boiler.errors |= E08;

		if (boiler.zero_cross.phase_2 == 0)
			boiler.errors |= E09;

		if (boiler.zero_cross.phase_3 == 0)
			boiler.errors |= E10;

		if (((boiler.upn.phase_A.Current_Instant < 5) || (boiler.upn.phase_A.Current_Instant > 15)) && ((boiler.upn.phase_B.Current_Instant < 35) || (boiler.upn.phase_B.Current_Instant > 45)))
			boiler.errors |= E11;

		if (((boiler.errors&E11) == 0) && (boiler.termostat.value == 0))
			boiler.errors |= E07;

		if ((boiler.parameters.temperature_alarm.tC_Instant > 100))
			boiler.errors |= U06;

		fun_relay_on();

		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 102;
		break;

	case 102:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 103;
		break;

	case 103:
		if ((boiler.heater.output.sensor.adc_value < 100) &&
			(boiler.parameters.temperature_alarm.tC_Instant < 100))
		{
			boiler.errors |= E01;
			boiler.errors |= E05;
			boiler.errors |= E06;
		}
		else
		{

			if((boiler.heater.output.sensor.adc_value > 3700) && (boiler.heater.output.sensor.Flag_Enable == 0))
				boiler.errors |= U05;
			else if((boiler.heater.output.sensor.adc_value < 100) && (boiler.heater.output.sensor.Flag_Enable == 0))
				boiler.errors |= E05;

			if ((boiler.parameters.temperature_alarm.tC_Instant < 100))
				boiler.errors |= E06;

			if (((boiler.upn.phase_A.Current_Instant > 5) && (boiler.upn.phase_A.Current_Instant < 15)) && ((boiler.upn.phase_B.Current_Instant > 35) && (boiler.upn.phase_B.Current_Instant < 45)))
				boiler.errors |= U11;

			if (boiler.termostat.value == 1)
				boiler.errors |= U07;

			if (((boiler.upn.phase_A.Current_Instant < 3)))
				boiler.errors |= U02;

			if (((boiler.upn.phase_B.Current_Instant < 3)))
				boiler.errors |= U03;

			if (((boiler.upn.phase_C.Current_Instant < 3)))
				boiler.errors |= U04;

		}

		fun_relay_off();
		HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, 1);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 104;

		break;

	case 104:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 105;
		break;

	case 105:
		if ((boiler.heater.output.sensor.adc_value < 100) &&
			(boiler.parameters.temperature_alarm.tC_Instant < 100))
		{
			boiler.errors |= E12;
		}
		else
		{
			boiler.errors &= ~(E05);
			boiler.errors &= ~(E06);

			if((boiler.heater.output.sensor.adc_value > 3700) && (boiler.heater.output.sensor.Flag_Enable == 0))
				boiler.errors |= U05;
			else if((boiler.heater.output.sensor.adc_value < 100) && (boiler.heater.output.sensor.Flag_Enable == 0))
				boiler.errors |= E05;

			if ((boiler.parameters.temperature_alarm.tC_Instant < 100))
				boiler.errors |= E06;

			if (((boiler.upn.phase_A.Current_Instant > 5) && (boiler.upn.phase_A.Current_Instant < 15)) && ((boiler.upn.phase_B.Current_Instant > 35) && (boiler.upn.phase_B.Current_Instant < 45)))
				boiler.errors |= U11;

			if (((boiler.errors&U11) == 0) && (boiler.termostat.value == 1))
				boiler.errors |= U07;

			if (boiler.termostat.value == 1)
				boiler.errors |= U07;

			if (((boiler.upn.phase_A.Current_Instant < 3)))
				boiler.errors |= U02;

			if (((boiler.upn.phase_B.Current_Instant < 3)))
				boiler.errors |= U03;

			if (((boiler.upn.phase_C.Current_Instant < 3)))
				boiler.errors |= U04;
		}

		if ((boiler.errors&E11) == 0)
		{
			if (((boiler.upn.phase_A.Current_Instant > 5) && (boiler.upn.phase_A.Current_Instant < 15)))
				boiler.errors |= U13;

			if ((boiler.upn.phase_B.Current_Instant > 35) && (boiler.upn.phase_B.Current_Instant < 45))
				boiler.errors |= U14;
		}

		if ((boiler.upn.phase_C.Current_Instant > 60) && (boiler.upn.phase_C.Current_Instant < 75))
			boiler.errors |= U15;

		//display_convert_temperature(boiler.adc.value[0]/100);

		fun_relay_on();
		HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, 1);
		delay_counter_1ms = 3000;

		if (((boiler.errors&E01) != 0) && (((boiler.errors&E12) != 0)))
			boiler.parameters.user_interface_step = 116;
		else
			boiler.parameters.user_interface_step = 106;

		break;

	case 106:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 107;
		break;

	case 107:
		if (((boiler.errors&E08) != 0) || ((boiler.errors&E06) != 0))
		{
			boiler.parameters.user_interface_step = 116;
			break;
		}

		HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, 1);
		HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, 0);
		HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, 0);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 108;
		break;

	case 108:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 109;
		break;

	case 109:
		if (((boiler.termostat.value == 0) && ((boiler.errors&E07) == 0)))
		{
			if (((boiler.upn.phase_A.Current_Instant < 5) || (boiler.upn.phase_A.Current_Instant > 15)))
				boiler.errors |= E13;
			else
				boiler.errors |= E07;
		}
		else
		{
			if ((boiler.errors&E07) == 0)
			{
				if ((boiler.upn.phase_A.Current_Instant < 5) || (boiler.upn.phase_A.Current_Instant > 15))
					boiler.errors |= E02;
			}
			else
			{
				if (((boiler.upn.phase_A.Current_Instant < 5) || (boiler.upn.phase_A.Current_Instant > 15)))
					boiler.errors |= E13;
			}
		}

		boiler.parameters.user_interface_step = 110;
		break;

	case 110:
		if (((boiler.errors&E09) != 0) || ((boiler.errors&E06) != 0))
		{
			boiler.parameters.user_interface_step = 116;
			break;
		}

		HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, 0);
		HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, 1);
		HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, 0);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 111;
		break;

	case 111:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 112;
		break;

	case 112:
		if ((boiler.termostat.value == 0) && ((boiler.errors&E07) == 0))
		{
			if ((boiler.upn.phase_B.Current_Instant < 35) || (boiler.upn.phase_B.Current_Instant > 45))
				boiler.errors |= E14;
			else
				boiler.errors |= E07;
		}
		else
		{
			if ((boiler.errors&E07) == 0)
			{
				if ((boiler.upn.phase_B.Current_Instant < 35) || (boiler.upn.phase_B.Current_Instant > 45))
					boiler.errors |= E03;
			}
			else
			{
				if ((boiler.upn.phase_B.Current_Instant < 35) || (boiler.upn.phase_B.Current_Instant > 45))
					boiler.errors |= E03;
			}
		}
		boiler.parameters.user_interface_step = 113;
		break;

	case 113:
		if (((boiler.errors&E10) != 0) || ((boiler.errors&E06) != 0))
		{
			boiler.parameters.user_interface_step = 116;
			break;
		}

		HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, 0);
		HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, 0);
		HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, 1);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 114;
		break;

	case 114:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 115;
		break;

	case 115:
		if ((boiler.termostat.value == 0) && ((boiler.errors&E07) == 0))
		{
			if ((boiler.upn.phase_C.Current_Instant < 60) || (boiler.upn.phase_C.Current_Instant > 75))
				boiler.errors |= E15;
			else
				boiler.errors |= E07;
		}
		else
		{
			if ((boiler.errors&E07) == 0)
			{
				if ((boiler.upn.phase_C.Current_Instant < 60) || (boiler.upn.phase_C.Current_Instant > 75))
					boiler.errors |= E04;
			}
			else
			{
				if ((boiler.upn.phase_C.Current_Instant < 60) || (boiler.upn.phase_C.Current_Instant > 75))
					boiler.errors |= E04;
			}
		}
		boiler.parameters.user_interface_step = 116;
		break;

	case 116:
		fun_relay_off();
		HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, 0);
		HAL_GPIO_WritePin(TEN_1_GPIO_Port, TEN_1_Pin, 0);
		HAL_GPIO_WritePin(TEN_2_GPIO_Port, TEN_2_Pin, 0);
		HAL_GPIO_WritePin(TEN_3_GPIO_Port, TEN_3_Pin, 0);
		delay_counter_1ms = 3000;
		boiler.parameters.user_interface_step = 117;
		break;

	case 117:
		if (delay_counter_1ms == 0)
			boiler.parameters.user_interface_step = 118;
		break;

	case 118:
		if ((boiler.heater.output.sensor.tC_Average > 80) && (boiler.heater.output.sensor.tC_Average < 100))
		{
			boiler.errors &= ~(E11);
			boiler.errors &= ~(U06);

			boiler.errors |= U01;
			boiler.errors |= U12;
		}	

		if (boiler.errors == 0)
			boiler.parameters.user_interface_step = 119;
		else
			boiler.parameters.user_interface_step = 120;
		break;

	case 119:
		display_print_text('H', '0', 'P');
		break;

	case 120:
		show_list_of_possible_values();
		break;
	}
}
