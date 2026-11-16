/*
 * display.h
 *
 *  Created on: Jul 5, 2022
 *      Author: KhaplanovY
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

//----------------------------------------------
void display_update(void);
void display_print_text(uint8_t digit1, uint8_t digit2, uint8_t digit3);
void display_convert_temperature(short temperature);
void print_display_check_layout();

#endif /* INC_DISPLAY_H_ */
