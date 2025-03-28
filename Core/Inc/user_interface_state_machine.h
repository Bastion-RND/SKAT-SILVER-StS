/*
 * user_interface_state_machine.h
 *
 *  Created on: Mar 25, 2025
 *      Author: GalkinN
 */

#ifndef INC_USER_INTERFACE_STATE_MACHINE_H_
#define INC_USER_INTERFACE_STATE_MACHINE_H_

void key_state_update();
void key_polling(uint8_t n);
uint8_t read_key(uint8_t n);
void user_interface_update(void);

#endif /* INC_USER_INTERFACE_STATE_MACHINE_H_ */
