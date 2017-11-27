/* ========================================
*
* Copyright SISTEMAS INSEPET, 2017
* All Rights Reserved
* UNPUBLISHED, LICENSED SOFTWARE.
*
* CONFIDENTIAL AND PROPRIETARY INFORMATION
* WHICH IS THE PROPERTY OF SISTEMAS INSEPET.
*
* ========================================
*/

#include "ch.h"
#include "hal.h"
#include "ConfEmr3.h"
#include "Emr3V210.h"
#include "EepromVirtual.h"
#include "ButtonEmr3.h"

/********************************************
ESTA FUNCION SE DEDICA A TESTIAR LOS PUNTOS
DE LAS CABEZAS DISPLAY. LAS CONDICIONES SON 
LAS SIGUIENTE: 
*LOS PUNTOS TANTOS EN EL PRESET = REGISTRADOR
*EL PUNTO DEL TOTALIZADOR DEBE SER 2.
*LAS DOS CABEZA DEBE TIENE LOS MISMO PUNTO.
*********************************************/

uint8_t ConfEmr3(void)
{
	uint8_t recibo[17];
	
    ButtonEmr(TEST_PUNTO,0x01);

    uartStartReceive(&UARTD1, 17, recibo);
    chThdSleepMilliseconds(2000);
    uartStopReceive(&UARTD1);

    if(recibo[0x03] == 0x43)
    {
        if(recibo[7] == recibo[10] && recibo[8] == recibo[12] && recibo[9] == recibo[13] && recibo[14] == 0X02)
        {
            write_eeprom(PV,recibo[10]);
            write_eeprom(PD,recibo[12]);
            write_eeprom(pppu,recibo[9]);
            
            if(read_eeprom(turno_aux) == 0x00 && read_eeprom(lado2) == 0x01)
            {
                ButtonEmr(TEST_PUNTO,0x02);

                uartStartReceive(&UARTD1, 17, recibo);
                chThdSleepMilliseconds(2000);
                uartStopReceive(&UARTD1);

                if(recibo[0x03] == 0x43)
                {
                    if(recibo[7] == recibo[10] && recibo[8] == recibo[12] && recibo[9] == recibo[13] && recibo[14] == 0X02)
                    {
                        if(read_eeprom(PV) == recibo[10] && read_eeprom(PD) == recibo[12] && read_eeprom(pppu) == recibo[9])
                        {
                            sd_punto();       
                            return 1; 
                        }

                        else
                        {
                            return 0;
                        }
                    }

                    else
                    {
                        return 0;
                    }
                }

                else
                {
                    return 0;
                }
            }

            else
            {
                sd_punto();       
                return 1;
            }            
        }

        else
        {
           return 0;
        }        
    }

    else
    {
        return 0;
    }
		
}



