/*
 * eeprom.h
 *
 *  Created on: Jul 6, 2022
 *      Author: KhaplanovY
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

uint32_t eeprom_read_setting(uint32_t addr);
void eeprom_write_setting(uint32_t data, uint32_t addr);

#endif /* INC_EEPROM_H_ */
