
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



uint64_t float_data;


void Imprimir_EMR3(uint8_t Data[], uint8_t P)
{
  	
	uartStartSend(&UARTD1, P, Data);
    chThdSleepMilliseconds(50);
    uartStopSend(&UARTD1);

}

uint8_t che_emr3(const uint8_t Data[], uint8_t P)
{
	
	uint8_t i;
	uint16_t valor=0;
	uint8_t valor_envio;

	for(i=1; i<P; i++)
	{
		valor = Data[i] + valor;
	}

	valor = valor & 0XFF;
	valor = (0xFF + 0x01) - valor;
	valor_envio = (valor & 0xFF);

    return valor_envio;
}

void ButtonEmr(uint8_t data, uint8_t pos)
{

	uint8_t P;
	uint8_t ENVIO[20];
	uint8_t ENVIO_aux[20];

	//MODO 7E 01 FF 53 75 02 36 7E
	if(data == TECLADO_MODO)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X02;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	//FIN 7E 01 FF 53 75 01 37 7E
	else if(data == TECLADO_FIN)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X01;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	//ESTADO DISPLAY 7E 01 FF 54 08 A4 7E
	else if(data == ESTADO_DISPLAY)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X54;
		ENVIO[4] = 0X08;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	//LEER DINERO, VOLUMEN 7E 01 FF 47 6B 00 4E 7E
	else if(data == LEER_DISPLAY)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X47;
		ENVIO[4] = 0X6B;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
		if (pos == 0x01) emr3_sale1(ENVIO,P,1);
		else emr3_sale2(ENVIO,P,1);
	}

	//AUTORIZACION 7E 01 FF 4F 06 01 AA 7E
	else if(data == AUTORIZAR)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X4F;
		ENVIO[4] = 0X06;
		ENVIO[5] = 0X01;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	//ESTADO SURTIDOR
	else if(data == ESTADO_SURTIDOR)
	{
		//7E 01 FF 54 03 A9 7E
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X54;
		ENVIO[4] = 0X03;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	//INICIO 7E 01 FF 53 75 00 38 7E
	else if (data == TECLADO_INICIO)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X00;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	/// INICIO
	else if(data == INICIO)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X4F;
		ENVIO[4] = 0X01;
		ENVIO[5] = 0X00;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	//FINALIZAR 7E 01 FF 4F 03 AE 7E
	else if(data == FIN)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X4F;
		ENVIO[4] = 0X03;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	//SIGUIENTE 7E 01 FF 53 75 04 34 7E
	else if(data == TECLADO_SIGUIENTE)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X04;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	//ENTRADA 7E 01 FF 53 75 08 30 7E
	else if(data == TECLADO_ENTRADA)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X08;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 0 7E 01 FF 53 75 09 2F 7E
	else if(data == TECLADO_0)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X09;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 1 7E 01 FF 53 75 0A 2E 7E
	else if(data == TECLADO_1)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X0A;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 2 7E 01 FF 53 75 0B 2D 7E
	else if(data == TECLADO_2)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X0B;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 3 7E 01 FF 53 75 0C 2C 7E
	else if(data == TECLADO_3)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X0C;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 4 7E 01 FF 53 75 0D 2B 7E
	else if(data == TECLADO_4)
	{
		ENVIO[0]=0X7E;
		ENVIO[1]=pos;
		ENVIO[2]=0XFF;
		ENVIO[3]=0X53;
		ENVIO[4]=0X75;
		ENVIO[5]=0X0D;
		ENVIO[6]=che_emr3(ENVIO, 6);
		ENVIO[7]=0X7E;
		P=8;
	}

	// 5 7E 01 FF 53 75 0E 2A 7E
	else if(data == TECLADO_5)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X0E;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 6 7E 01 FF 53 75 0F 29 7E
	else if(data == TECLADO_6)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X0F;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 7 7E 01 FF 53 75 10 28 7E
	else if(data == TECLADO_7)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X10;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 8 7E 01 FF 53 75 11 27 7E
	else if(data == TECLADO_8)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X11;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P=8;
	}

	// 9 7E 01 FF 53 75 12 26 7E
	else if(data == TECLADO_9)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X12;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P = 8;
	}

	//PRESET 7E 01 FF 53 75 03 35 7E
	else if(data == TECLADO_PRESET)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X75;
		ENVIO[5] = 0X03;
		ENVIO[6] = che_emr3(ENVIO, 6);
		ENVIO[7] = 0X7E;
		P = 8;
	}

	//Total de volumen 7E 01 FF 47 6A 4F 7E
	else if (data == TOTAL_VOLUMEN)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X47;
		ENVIO[4] = 0X6C;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	//ENVIAR PPU 7E 01 FF 4F 08 XX XX XX XX 8B 7E
	else if (data == ENVIAR_PPU)
	{

		ENVIO_aux[0] = 0X7E;
		ENVIO_aux[1] = pos;
		ENVIO_aux[2] = 0XFF;
		ENVIO_aux[3] = 0X4F;
		ENVIO_aux[4] = 0X08;
		ENVIO_aux[5] = read_eeprom(ieee_1);
		ENVIO_aux[6] = read_eeprom(ieee_2);
		ENVIO_aux[7] = read_eeprom(ieee_3);
		ENVIO_aux[8] = read_eeprom(ieee_4);
		
		P=5;
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X4F;
		ENVIO[4] = 0X08;
	
		ENVIO[P] = read_eeprom(ieee_1);

		if(read_eeprom(ieee_1) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_1) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_2);

		if(read_eeprom(ieee_2) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_2) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_3);

		if(read_eeprom(ieee_1) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_3) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_4);

		if(read_eeprom(ieee_4) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_4) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;
		
		chThdSleepMilliseconds(1);
		ENVIO[P] = che_emr3(ENVIO_aux,9);

		if(ENVIO[P] == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (ENVIO[P] == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;
		ENVIO[P] = 0X7E;
		P++;
	}

	//ENVIAR VOLUMEN 7E 01 FF 53 6E XX XX XX XX 7F 7E
	else if (data == ENVIAR_VOLUMEN)
	{

		ENVIO_aux[0] = 0X7E;
		ENVIO_aux[1] = pos;
		ENVIO_aux[2] = 0XFF;
		ENVIO_aux[3] = 0X53;
		ENVIO_aux[4] = 0X6E;
		ENVIO_aux[5] = read_eeprom(ieee_1);
		ENVIO_aux[6] = read_eeprom(ieee_2);
		ENVIO_aux[7] = read_eeprom(ieee_3);
		ENVIO_aux[8] = read_eeprom(ieee_4);
		//ENVIO_aux[9] = che_emr3(ENVIO_aux,9);

		P=5;
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X53;
		ENVIO[4] = 0X6E;

		ENVIO[P] = read_eeprom(ieee_1);

		if(read_eeprom(ieee_1) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_1) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_2);

		if(read_eeprom(ieee_2) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_2) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_3);

		if(read_eeprom(ieee_1) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_3) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		ENVIO[P] = read_eeprom(ieee_4);

		if(read_eeprom(ieee_4) == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (read_eeprom(ieee_4) == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}

		P++;

		chThdSleepMilliseconds(1);
		ENVIO[P] = che_emr3(ENVIO_aux,9);

		if(ENVIO[P] == 0x7E)
		{
			ENVIO[P] = 0X7D;
			P++;
			ENVIO[P] = 0x5E;
		}	

		else if (ENVIO[P] == 0x7D)
		{
			P++;
			ENVIO[P] = 0x5D;
		}
		
		P++;
		ENVIO[P] = 0X7E;
		P++;		
	}

	//Hora 7E 01 FF 47 69 50 7E
	else if (data == HORAS)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X47;
		ENVIO[4] = 0X69;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P = 7;
	}

	//FECHA 7E 01 FF 47 64 55 7E
	else if (data == FECHAS)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X47;
		ENVIO[4] = 0X64;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	//LEER EL PPU 7E 01 FF 54 07 A5 7E
   	else if (data == LEER_PPU)
   	{
      
      	ENVIO[0] = 0X7E;
      	ENVIO[1] = pos;
      	ENVIO[2] = 0XFF;
      	ENVIO[3] = 0X54;
      	ENVIO[4] = 0X07;
      	ENVIO[5] = che_emr3(ENVIO, 5);
      	ENVIO[6] = 0X7E;
      	P=7;
   	}

   	//LEER EL PRESET VOLUMNE 7E 01 FF 47 6E 4B 7E
   	else if (data == LEER_PRES_VOL)
   	{
      	ENVIO[0] = 0X7E;
      	ENVIO[1] = pos;
      	ENVIO[2] = 0XFF;
      	ENVIO[3] = 0X47;
      	ENVIO[4] = 0X6E;
      	ENVIO[5] = che_emr3(ENVIO, 5);
      	ENVIO[6] = 0X7E;
      	P=7;
   	}

    //pedir volumen ieee venta;
   	else if (data == PEDIR_VENTA)
   	{
      	ENVIO[0] = 0X7E;
      	ENVIO[1] = pos;
      	ENVIO[2] = 0XFF;
      	ENVIO[3] = 0X47;
      	ENVIO[4] = 0X67;
      	ENVIO[5] = che_emr3(ENVIO, 5);
      	ENVIO[6] = 0X7E;
      	chThdSleepMilliseconds(1);
      	P=7;
      	if(pos == 0x01) emr3_sale1(ENVIO,P,1);
      	else emr3_sale2(ENVIO,P,1);
      	
   	}

   	else if(data == TEST_PUNTO)
   	{
 
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 'E';
		ENVIO[4] = 16;
		
		chThdSleepMilliseconds(1);
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;

		P=7;
   	}

   	else if(data == SALIR_CONSALA)
   	{
   		ENVIO[0] = 0X7E;
        ENVIO[1] = pos;
        ENVIO[2] = 0XFF;
        ENVIO[3] = 0X44;
        ENVIO[4] = 0X25;
        ENVIO[5] = 0X00;
        ENVIO[6] = che_emr3(ENVIO, 6);
        ENVIO[7] = 0X7E;

        P=8;
   	}

   	else if(data == TECLADO_PAUSA)
	{
		ENVIO[0] = 0X7E;
		ENVIO[1] = pos;
		ENVIO[2] = 0XFF;
		ENVIO[3] = 0X4F;
		ENVIO[4] = 0X02;
		ENVIO[5] = che_emr3(ENVIO, 5);
		ENVIO[6] = 0X7E;
		P=7;
	}

	Imprimir_EMR3(ENVIO, P);

	if(read_eeprom(log) == 1) emr3_log(&ENVIO,P,1);


	return;
}

void LeerTotales(uint8_t pos)
{
	
	uint8_t recibo[20];
			
	do
    {
        chThdSleepMilliseconds(300);

        ButtonEmr(TOTAL_VOLUMEN,pos);

        uartStartReceive(&UARTD1,17, recibo);
        chThdSleepMilliseconds(500);
        uartStopReceive(&UARTD1);

        if(read_eeprom(log) == 1) emr3_log(&recibo,17,2);

    } while(recibo[11] != '.');

    if(pos == 0x01)
    {
    	write_eeprom(tv1_1,(recibo[5] & 0X0F));
		write_eeprom(tv1_2,(recibo[6] & 0X0F));
		write_eeprom(tv1_3,(recibo[7] & 0X0F));
		write_eeprom(tv1_4,(recibo[8] & 0X0F));
		write_eeprom(tv1_5,(recibo[9] & 0X0F));
		write_eeprom(tv1_6,(recibo[10] & 0X0F));
	
		write_eeprom(tv1_7,(recibo[12] & 0X0F));
		write_eeprom(tv1_8,(recibo[13] & 0X0F));

		sd_Tvolumen(pos);
		return;
    }

    else if (pos == 0x02)
    {
		
    	write_eeprom(tv2_1,(recibo[5] & 0X0F));
		write_eeprom(tv2_2,(recibo[6] & 0X0F));
		write_eeprom(tv2_3,(recibo[7] & 0X0F));
		write_eeprom(tv2_4,(recibo[8] & 0X0F));
		write_eeprom(tv2_5,(recibo[9] & 0X0F));
		write_eeprom(tv2_6,(recibo[10] & 0X0F));
	
		write_eeprom(tv2_7,(recibo[12] & 0X0F));
		write_eeprom(tv2_8,(recibo[13] & 0X0F));

		sd_Tvolumen(pos);
		return;
	}

	
	return; 
}

uint64_t expo2(bool bit_data, uint32_t eleva)
{
    uint32_t i;
    uint64_t mul=1;

    if(bit_data == 0) return 0;
    else
    {

        for(i = 0 ; i < eleva ; i++)
        {
            mul = mul * 2;
        }
        return mul;
    }    

}

void f_pf(uint32_t Y, uint32_t DP, uint8_t tipo)
{

   	uint8_t CY;                             //COPIA DE LA PARTE REAL DEL PUNTO FLOTANTE
   	uint8_t I;                           //DETERMINAR LA POSICION DEL BIT MAS SIGNIFICATIVO
   	uint8_t CI;                          //DETERMINA LA CUANTOS BIT HAY QUE MOVERSE
   	uint8_t J;                           //RECORRE LOS ESPACIOS BASIDO
   	uint8_t W;                           //PONE EN LOS ESPACIOS BASIDO LOS BIT
   	uint8_t VALOR;                       //PRACTICAMENTE ES EL B1 (127+BIT(MSB))
   	uint8_t B1;                          //BYTE 1
   	uint8_t B2;                          //BYTE 2
   	uint8_t B3;                          //BYTE 3
   	uint8_t B4;                          //BYTE 4
   	uint8_t D;                           //BYTE 4
    uint16_t Divi;

    if(tipo == 1) Divi = mult(read_eeprom(pppu));
    else if(tipo == 3) Divi = mult(read_eeprom(PD));
    else Divi = mult(read_eeprom(PV));

    if(Y>0)
    {   
        I=20;                                    //PONEMOS LA VARIABLE EN I EN 20 PORQUE EL MAXIMO BIT MSB SE ENCONTRARA EN LA 19

       while (I>=0)                             //DETERMINAMOS DONDE SE ENCUENTRA EL BIT MSB
       {
          I--;                                //LE RESTAMOS 1 PARA EMPEZAR EN LA POSICION 19 Y SI NO ESTA SE EMPIEZA A DISMINUIR
          D= (Y>>I) & 1;                      //TESTEO SI EL BIT EN LA POSICION I ES IGUAL A 1, SI LO ES SALTO LA FUNCION CICLICA SINO SIGO BUSCANDO EN OTRAS POSICIONES
          if(D==1) break;
       }


       VALOR=127+I;                              //DETERMINO EL PRACTICAMENTE EL B1 POR NORMA


       B1=VALOR>>1;                              //CORREMOS EL VALOR 1 BIT A LA DERECHA PARA DARLE CABIA EL BIT DEL SIGNO(0)

       CI=I+1;                                   //DETEMININO EN QUE POSICION SE ENCUENTRA LOS DEMAS BIT DE LA PARTE ENTERA

       if (CI>=8)                              //PREGUNTO SI TENGA LA CANTIDAD DE BIT PARA HACER EL SIGUENTE BYTE(B2)
       {
          CI=CI-8;                              //LE RESTAMOS LA CANTIDAD DE BIT 8 POSICIONES
          CY=Y>>CI;                             //CORREMOS A LA IZQUIERDA LA CANTIDAD DE BIT QUE SOBRAN Y LO GUARDAMOS EN LA COPIA DE Y
          B2=CY;                                // GUARDAMOS LA COPIA DE Y EN EL BYTE 2

            if(I == 0)
            {
                B2=0;
            }

          if ((VALOR & 1) == 1) 
          {
             B2=0x80 | B2;                   //COMO AL VALOR SE LE QUITO BIT, ESTE SE LO PONEMOS AL BIT MSB DEL BYTE 2
          }
          
          else B2=0x7F & B2;                  //TESTIMOS SI EL BIT ES 0 SE PONE UN 0

          if(CI>=8)                           //REVISAMO SI LA CANTIDAD DE BIT DEL DATO ENTERO ALCANZA PARA LLENAR EL SIGUENTE BYTE 3
          {
             CI=CI-8;                        //DESCONTAMOS 8 BIT A LA CANTIDAD EXISTENTE
             CY=Y>>CI;                         //CORREMOS LA CANTIDAD BIT EXISTENTE A LA DERECHA
             B3=CY;                            //LLENAMOS EL BYTE 3 CON

             if(CI>=8)
             {
                B4=Y;
             }
             
             else 
             {
                CI=8-CI;
                CY=Y<<CI;
                B4=CY;
                CI--;

                for(J=0;J<=CI;J++)
                {
                   W=CI-J;
                   DP=DP*2;

                   if(DP >= Divi)
                   {
                      B4= B4| (1<<W);
                      DP=DP-Divi;
                   }
                }
             }
          }

          else
          {
             CI=8-CI;
             CY=Y<<CI;
             B3=CY;
             CI--;

             for(J=0;J<=CI;J++)
             {
                W=(CI)-J;
                DP=DP*2;

                if(DP>=Divi)
                {
                   B3=B3 | (1 << W);
                   DP=DP-Divi;
                }
             }

             for(J=0;J<=7;J++)
             {
                W=7-J;
                DP=DP*2;
                
                if(DP>=Divi)
                {
                   B4=B4 | (1 << W);
                   DP=DP-Divi;
                }
             }

             if(DP >= 600)
             {
                B4 = B4 | 1;
             }
          }
       }

       else
       {

          CI=8-CI;
          CY=(Y<<CI);
          B2=CY;
           
          if ((VALOR & 1) == 1) 
          {
             B2=0x80 | B2;                       //COMO AL VALOR SE LE QUITO BIT, ESTE SE LO PONEMOS AL BIT MSB DEL BYTE 2
          }

          else B2=0x7F & B2;//TESTIMOS SI EL BIT ES 0 SE PONE UN 0

          CI--;

          for(J=0;J<=CI;J++)
          {
             W=CI-J;
             DP=DP*2;

             if(DP>=Divi)
             {
                B2=B2 | (1 << W);
                DP=DP-Divi;
             }
          }

            if(DP == 0X00)
            {
                B3 = 0;
                B4 = 0;
            }

            else if( DP > 0X00)
            {
              B3=0;


              for(J=0;J<8;J++)
              {
                 W= 7-J;
                 DP = DP * 2;

                 if(DP >= Divi)
                 {
                    B3=B3 | (1 << W);
                    DP=DP-Divi;
                 }
              }

              B4=0;

              for(J=0;J<8;J++)
              {
                 W=7-J;
                 DP = DP * 2;

                 if(DP >= Divi)
                 {
                    B4=B4 | (1 << W);
                    DP=DP-Divi;
                 }
              }
            }
       }
    }
    
    else if(Y == 0)
    {
        I=0;
        while(DP>=1)
        {
            DP=DP*2;
            if(DP>=Divi) break;
            I++;   
        }

        DP = (DP - Divi);

        VALOR=126-I;

        B1=VALOR>>1;

        B2 = 0;

        if ((VALOR & 1) == 1) 
        {
            B2 = (0x80 | B2);                       //COMO AL VALOR SE LE QUITO BIT, ESTE SE LO PONEMOS AL BIT MSB DEL BYTE 2
        }

        
        
        for(J=0;J<7;J++)
        {
            W = 6-J;
            DP = DP*2;

            if(DP >= Divi)
            {
                B2 = (B2 | (1 << W));
                DP = DP-Divi;
            }
        }

        B3=0;

        for(J=0;J<8;J++)
        {
            W = 7-J;
            DP = DP * 2;

            if(DP >= Divi)
            {
                B3 = (B3 | (1 << W));
                DP = DP-Divi;
            }
        }

        B4=0;

        for(J=0;J<8;J++)
        {
            W = 7-J;
            DP = DP*2;

            if(DP >= Divi)
            {
                B4 = (B4 | (1 << W));
                DP = DP-Divi;
            }
        }

    }

    
    write_eeprom(ieee_1,B4); 
    write_eeprom(ieee_2,B3);
    write_eeprom(ieee_3,B2);
    write_eeprom(ieee_4,B1);

   return;
}

void Pf_p(uint8_t dato_f[])
{

    uint8_t dato1;
    uint8_t dato2;
    uint8_t dato3;
    uint8_t dato4;
    uint8_t dato5;
    uint8_t dato6;
    uint8_t dato7;
    uint8_t dato8;
    uint8_t i;
    uint8_t j;
    uint64_t fraction;
    
    uint64_t mod_data;
     
    
    uint16_t expon;
    uint16_t exponente;
    uint8_t aux_expo;

    

    dato1 = dato_f[12];
    dato2 = dato_f[11];
    dato3 = dato_f[10];
    dato4 = dato_f[9];
    dato5 = dato_f[8];
    dato6 = dato_f[7];
    dato7 = dato_f[6];
    dato8 = dato_f[5];

    chThdSleepMilliseconds(5);

    // dato1 = 0x3F;
    // dato2 = 0X50;
    // dato3 = 0X62;
    // dato4 = 0X4D;
    // dato5 = 0XD2;
    // dato6 = 0XF1;
    // dato7 = 0XA9;
    // dato8 = 0XFC;

    expon = dato1;
    expon = expon << 4;
    aux_expo = (dato2 >> 4) & 0x0F;

    expon = expon | aux_expo;

    fraction = dato2 & 0x0F;
    fraction = (fraction << 8);

    fraction = (fraction | dato3);
    fraction = (fraction << 8);

    fraction = (fraction | dato4);
    fraction = (fraction << 8);

    fraction = (fraction | dato5);
    fraction = (fraction << 8);

    fraction = (fraction | dato6);
    fraction = (fraction << 8);

    fraction = (fraction | dato7);
    fraction = (fraction << 8);

    fraction = (fraction | dato8);

   
    if(dato1 != 0)
    {
        //float_data = fraction * exponente;
        

        if(expon >= 1023)
        {
            float_data = 1000000000 + (fraction / ((expo2(1,(52)) / 1000000000) ));
            mod_data = float_data ;
            
            float_data = (float_data * expo2(1,(expon-1023))) / (1000000000 / mult(read_eeprom(PV)));

            // if(mod_data%10 != 0)
            // {
            //     float_data = float_data + 1;
            // }
            
        }
        else
        {
            float_data = 1000000000 + (fraction / ((expo2(1,(52)) / 1000000000)));
            float_data = (float_data / expo2(1,(1023-expon))) / (1000000000 / mult(read_eeprom(PV)));
        } 
    }
    else 
    {
        float_data = 0;
    }
    

    return; 
}

uint16_t mult(uint8_t punto)
{
	switch(punto)
	{
		case 0: return 1;
		break;

		case 1: return 10;
		break;

		case 2: return 100;
		break;

		case 3: return 1000;
		break;
	}
}
