#include "ch.h"
#include "hal.h"
#include "EepromVirtual.h"

uint8_t read_eeprom(uint8_t num)
{
	return (dato_emr3[num]);
}

void write_eeprom(uint8_t num ,uint8_t inf )
{
	dato_emr3[num] = inf;
	return;
}