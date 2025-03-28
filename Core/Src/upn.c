/*
 * upn.c
 *
 *  Created on: Mar 26, 2025
 *      Author: GalkinN
 */

#include "main.h"

#define SAFETY_DELTA 2
#define SAFETY_DELTA_TWO 8

extern struct sBoiler boiler;

void set_upn_type(uint8_t type)
{
	  boiler.upn.parameters.type = type;
}

void upn_update() {

	switch (boiler.parameters.boiler_type)
	{
	case 4:
		boiler.ten3.is_active = 0;
		boiler.ten2.is_active = 0;
		break;
	case 8:
		boiler.ten3.is_active = 0;
		break;
	}


	if (boiler.upn.parameters.type == 1)
	{
		if (*(boiler.ten1.upn.signal) < 0)
		{
			if (boiler.ten1.upn.check)
			{
				if (boiler.ten3.upn.is_active == 0)
					boiler.ten3.upn.is_active = 1;
				else if (boiler.ten2.upn.is_active == 0)
					boiler.ten2.upn.is_active = 1;
				else if (boiler.ten1.upn.is_active == 0)
					boiler.ten1.upn.is_active = 1;

				boiler.ten1.upn.check = 0;
			}
		}
		else
		{
			if (boiler.ten1.upn.check)
			{
				if (boiler.ten1.upn.is_active && (boiler.ten1.upn.timer > UPN_WAITING_TIME))
				{
					if (*(boiler.ten1.upn.signal) > boiler.parameters.current)
						boiler.ten1.upn.is_active = 0;

					boiler.ten1.upn.timer = 0;
				}
				else if (boiler.ten2.upn.is_active && (boiler.ten2.upn.timer > UPN_WAITING_TIME))
				{
					if (*(boiler.ten1.upn.signal) > boiler.parameters.current)
						boiler.ten2.upn.is_active = 0;

					boiler.ten2.upn.timer = 0;
				}
				else if (boiler.ten3.upn.is_active && (boiler.ten3.upn.timer > UPN_WAITING_TIME))
				{
					if (*(boiler.ten1.upn.signal) > boiler.parameters.current)
						boiler.ten3.upn.is_active = 0;

					boiler.ten3.upn.timer = 0;
				}
			}
		}
	}
	else if (boiler.upn.parameters.type == 3)
	{
		if (*(boiler.ten1.upn.signal) < 0)
		{
			if (boiler.ten1.upn.check)
			{
				boiler.ten1.upn.is_active = 1;
				boiler.ten1.upn.check = 0;
			}
		}
		else if (boiler.ten1.upn.timer > UPN_WAITING_TIME)
		{
			if (*(boiler.ten1.upn.signal) > boiler.parameters.current)
				boiler.ten1.upn.is_active = 0;

			boiler.ten1.upn.timer = 0;
		}

		if (*(boiler.ten2.upn.signal) < 0)
		{
			if (boiler.ten2.upn.check)
			{
				boiler.ten2.upn.is_active = 1;
				boiler.ten2.upn.check = 0;
			}
		}
		else if (boiler.ten2.upn.timer > UPN_WAITING_TIME)
		{
			if (*(boiler.ten2.upn.signal) > boiler.parameters.current)
				boiler.ten2.upn.is_active = 0;

			boiler.ten2.upn.timer = 0;
		}

		if (*(boiler.ten3.upn.signal) < 0)
		{
			if (boiler.ten3.upn.check)
			{
				boiler.ten3.upn.is_active = 1;
				boiler.ten3.upn.check = 0;
			}
		}
		else if (boiler.ten3.upn.timer > UPN_WAITING_TIME)
		{
			if (*(boiler.ten3.upn.signal) > boiler.parameters.current)
				boiler.ten3.upn.is_active = 0;

			boiler.ten3.upn.timer = 0;
		}
	}
	else
	{
		boiler.ten1.upn.is_active = 0;
		boiler.ten2.upn.is_active = 0;
		boiler.ten3.upn.is_active = 0;
	}

	if (boiler.ten1.upn.is_active)
		boiler.ten1.is_active = 0;

	if (boiler.ten2.upn.is_active)
		boiler.ten2.is_active = 0;

	if (boiler.ten3.upn.is_active)
		boiler.ten3.is_active = 0;
}

