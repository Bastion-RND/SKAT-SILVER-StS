#include "main.h"
#include "eeprom.h"

//----------------------------------------------------------------------------------------------
uint32_t eeprom_read_setting(uint32_t addr)
{
	return (*(__IO uint32_t*) addr);
}
//----------------------------------------------------------------------------------------------
void eeprom_write_setting(uint32_t data, uint32_t addr)
{
	FLASH_EraseInitTypeDef 	EraseInitStruct;
	uint32_t 				PAGEError = 0;

	if(data != eeprom_read_setting(addr))	// Если данные изменились, то сохраняем
	{
		EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = addr;
		EraseInitStruct.NbPages     = 1;

		HAL_FLASH_Unlock();
		while(HAL_FLASHEx_Erase(&EraseInitStruct,&PAGEError) != HAL_OK) {};
		while(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, (uint64_t)data) != HAL_OK) {};

		HAL_FLASH_Lock();
	}
}
//----------------------------------------------------------------------------------------------
