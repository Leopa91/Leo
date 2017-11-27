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
#include "Emr3V210.h"
#include "serial.h"
#include "ff.h"
#include "ascii.h"
#include "EepromVirtual.h"
#include "LectureSw.h"
#include "ButtonEmr3.h"
#include "DataSd.h"
#include "RxWire.h"

/*
ESTA FUNCION SOLO SE UTILIZA PARA SABER CUANTAS CABEZAS DISPLAY TIENE LA IB,
LA FUNCION MANDA A LA IB EL COMANDO DE QUE TODAS LAS VENTA DEBEN TANER UNA
AUTORIZACION DEPENDIENDO DE LA RESPUESTA SE PUEDE DETERMINAR LA CANTIDAD.
*/

void ValidarEmr3 (void)
{
	uint8_t i;
	uint8_t DATO;
	uint8_t envio[20];
	uint8_t recibo[20];
	uint8_t row_string [16];
	

	while (true)
	{
	
		//7E 01 FF 44 25 01 96 7E
		envio[0]=0x7E;
		envio[1]=0x01;
		envio[2]=0xFF;
		envio[3]=0x44;
		envio[4]=0x25;
		envio[5]=0x01;
		envio[6]=0x96;
		envio[7]=0x7E;	
    		
    	uartStartSend(&UARTD1, 8, envio);
      	chThdSleepMilliseconds(20);
      	uartStopSend(&UARTD1);
    	    	      
      	uartStartReceive(&UARTD1, 5, recibo);
   		chThdSleepMilliseconds(500);
   		uartStopReceive(&UARTD1);

		if (recibo[2] == 0X01)
		{

  			LCDPrintRow2(0,"   EMR3_1=ON    ");
			break;
		}	

		chThdSleepMilliseconds(1000);
	}
	
	chThdSleepMilliseconds(1000);
	
	//LADO 2
	envio[0]=0x7E;
	envio[1]=0x02;
	envio[2]=0xFF;
	envio[3]=0x44;
	envio[4]=0x25;
	envio[5]=0x01;
	envio[6]=0x95;
	envio[7]=0x7E;	    

	uartStartSend(&UARTD1, 8, envio);
	chThdSleepMilliseconds(20);
	uartStopSend(&UARTD1);
 	     	
	uartStartReceive(&UARTD1, 5, recibo);
  	chThdSleepMilliseconds(500);
   	uartStopReceive(&UARTD1);
	
	if ( recibo[2] == 0X02)
	{
		
  		write_eeprom(lado2,0x01);
        write_eeprom(turno_aux,0x00);
        LCDPrintRow(0,"   EMR3_1=ON    ");
        LCDPrintRow2(0,"   EMR3_2=ON    ");
  		return;
		
	}
	else if (recibo[2] != 0X02)
	{
		
  		write_eeprom(lado2,0x00);
        write_eeprom(turno_aux,0x01);
        LCDPrintRow(0,"   EMR3_1=ON    ");
        LCDPrintRow2(0,"   EMR3_2=OFF   ");
  		return; 
	}	
}




