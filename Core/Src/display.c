/*
 * display.c
 *
 *  Created on: Mar 25, 2025
 *      Author: GalkinN
 */

#include "main.h"

extern struct sBoiler boiler;

void display_update(void)
{
	if (boiler.display.blink_status == 1 && boiler.display.blink == 1)
		boiler.display.accum = 0;
	else
	{
		boiler.display.counter++;
		if (boiler.display.counter > 2) boiler.display.counter = 0;

		switch(boiler.display.counter)
		{
		case 0:
			boiler.display.accum = 0b0000010000000000;
			break;
		case 1:
			boiler.display.accum = 0b0100000000000000;
			break;
		case 2:
			boiler.display.accum = 0b0001000000000000;
			break;
		}

		if (boiler.errors.e03 == 1)
			boiler.display.accum |= 0b0000000001000000;

		switch (boiler.display.digits[boiler.display.counter])
		{
			case '1':	boiler.display.accum |= 0b0000001000001000;	break;
			case '2':	boiler.display.accum |= 0b0000101100110000;	break;
			case '3':	boiler.display.accum |= 0b0000101100011000;	break;
			case '4':	boiler.display.accum |= 0b0010001000011000;	break;
			case '5':	boiler.display.accum |= 0b0010100100011000;	break;
			case '6':	boiler.display.accum |= 0b0010100100111000;	break;
			case '7':	boiler.display.accum |= 0b0000101000001000;	break;
			case '8':	boiler.display.accum |= 0b0010101100111000;	break;
			case '9':	boiler.display.accum |= 0b0010101100011000;	break;
			case '0':	boiler.display.accum |= 0b0010101100101000;	break;

			case '-':	boiler.display.accum |= 0b0000000000010000;	break;
			case 'c':	boiler.display.accum |= 0b0000000100110000;	break;
			case 'E':	boiler.display.accum |= 0b0010100100110000;	break;
			case 'y':	boiler.display.accum |= 0b0010001100011000;	break;		// У
			case 'p':	boiler.display.accum |= 0b0010101000101000;	break;		// П
			case 'n':	boiler.display.accum |= 0b0010001000111000;	break;		// Н
			case 'F':	boiler.display.accum |= 0b0010100000110000;	break;
			case 'P':	boiler.display.accum |= 0b0010101000110000;	break;
			case 'G':	boiler.display.accum |= 0b0010100000100000;	break;
			default:	boiler.display.accum = 0;
		}
	}

	LL_SPI_TransmitData16(SPI1, boiler.display.accum);
	HAL_Delay(1);
}

void display_print_text(uint8_t digit1, uint8_t digit2, uint8_t digit3)
{
	boiler.display.digits[0] = digit1;
	boiler.display.digits[1] = digit2;
	boiler.display.digits[2] = digit3;
}

void display_convetr_temperature(short temperature)
{
	uint8_t first_digit = 0;
	uint8_t second_digit = 0;
	uint8_t negative_value_flag = 0;

	if (temperature < 0)
	{
		negative_value_flag = 1;
		temperature = -temperature;
	}

	first_digit = (temperature / 10) % 10;
	second_digit = temperature % 10;

	if (negative_value_flag)
	{
		if (second_digit != 0)
			first_digit = 255;
		else
			second_digit = 255;
	}

	uint8_t var_digit = first_digit;

	for(uint8_t i = 0; i < 2; ++i)
	{
		switch(var_digit)
		{
		case 0:
			boiler.display.digits[i] = '0';
			break;
		case 1:
			boiler.display.digits[i] = '1';
			break;
		case 2:
			boiler.display.digits[i] = '2';
			break;
		case 3:
			boiler.display.digits[i] = '3';
			break;
		case 4:
			boiler.display.digits[i] = '4';
			break;
		case 5:
			boiler.display.digits[i] = '5';
			break;
		case 6:
			boiler.display.digits[i] = '6';
			break;
		case 7:
			boiler.display.digits[i] = '7';
			break;
		case 8:
			boiler.display.digits[i] = '8';
			break;
		case 9:
			boiler.display.digits[i] = '9';
			break;
		case 255:
			boiler.display.digits[i] = '-';
			break;
		}

		if (i == 0)
			var_digit = second_digit;
	}

	boiler.display.digits[2] = 'c';
}
