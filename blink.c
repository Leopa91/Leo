/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

void CONV_F_PF(float FL);

static UARTConfig uart_cfg_2= {
  0,
  0,
  0,
  0,
  0,
  9600,
  0,
  USART_CR2_LINEN,
  0
};

void CONV_F_PF(float FL)
{
    //float x=200;      //VALOR DEL PUNTO FLOTANTE
    int Y;      //PARTE REAL DEL PUNTO FLOTANTE
    int CY;     //COPIA DE LA PARTE REAL DEL PUNTO FLOTANTE
    uint8_t I;            //DETERMINAR LA POSICION DEL BIT MAS SIGNIFICATIVO
    uint8_t CI;           //DETERMINA LA CUANTOS BIT HAY QUE MOVERSE
    uint8_t J;            //RECORRE LOS ESPACIOS BASIDO
    uint8_t W;            //PONE EN LOS ESPACIOS BASIDO LOS BIT
    uint8_t VALOR;        //PRACTICAMENTE ES EL B1 (127+BIT(MSB))
    int DP;         //PUNTO DECIMAL LO CONVIERTE EN UN ENTERO(*1000)
    uint8_t B1;           //BYTE 1
    uint8_t B2;           //BYTE 2
    uint8_t B3;           //BYTE 3
    uint8_t B4;           //BYTE 4
    uint8_t D;           //BYTE 4
    uint8_t ENVIO[20];




    Y=FL;//PASAMOS LA PARTE ENTERA A LA VARIABLE Y, PARA PODER DETERMINAR EL BIT MSB


    I=20;             //PONEMOS LA VARIABLE EN I EN 20 PORQUE EL MAXIMO BIT MSB SE ENCONTRARA EN LA 19

    while (I>=0)       //DETERMINAMOS DONDE SE ENCUENTRA EL BIT MSB
    {
        I--;                //LE RESTAMOS 1 PARA EMPEZAR EN LA POSICION 19 Y SI NO ESTA SE EMPIEZA A DISMINUIR
        D= (Y>>I) & 1;              //TESTEO SI EL BIT EN LA POSICION I ES IGUAL A 1, SI LO ES SALTO LA FUNCION CICLICA SINO SIGO BUSCANDO EN OTRAS POSICIONES
        if(D==1) break;
    }

    FL=(FL-Y)*1000;       //CONVIERTO EL 0.5 A 500 PARA PASARLO A NUMERO ENTERO

    DP=FL;

    VALOR=127+I;         //DETERMINO EL PRACTICAMENTE EL B1 POR NORMA

    B1=VALOR>>1;         //CORREMOS EL VALOR 1 BIT A LA DERECHA PARA DARLE CABIA EL BIT DEL SIGNO(0)
    //CORRER DATOS
    CI=I+1;              //DETEMININO EN QUE POSICION SE ENCUENTRA LOS DEMAS BIT DE LA PARTE ENTERA
    if (CI>=8)           //PREGUNTO SI TENGA LA CANTIDAD DE BIT PARA HACER EL SIGUENTE BYTE(B2)
    {
        CI=CI-8;             //LE RESTAMOS LA CANTIDAD DE BIT 8 POSICIONES
        CY=Y>>CI;            //CORREMOS A LA IZQUIERDA LA CANTIDAD DE BIT QUE SOBRAN Y LO GUARDAMOS EN LA COPIA DE Y
        B2=CY;               // GUARDAMOS LA COPIA DE Y EN EL BYTE 2
        if ((VALOR & 1) == 1) 
        {
            B2=0x80 | B2; //COMO AL VALOR SE LE QUITO BIT, ESTE SE LO PONEMOS AL BIT MSB DEL BYTE 2
        }
        else B2=0x7F & B2;//TESTIMOS SI EL BIT ES 0 SE PONE UN 0

        if(CI>=8)            //REVISAMO SI LA CANTIDAD DE BIT DEL DATO ENTERO ALCANZA PARA LLENAR EL SIGUENTE BYTE 3
        {
            CI=CI-8;             //DESCONTAMOS 8 BIT A LA CANTIDAD EXISTENTE
            CY=Y>>CI;            //CORREMOS LA CANTIDAD BIT EXISTENTE A LA DERECHA
            B3=CY;               //LLENAMOS EL BYTE 3 CON
            
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
                    
                    if(DP>=1000)
                    {
                        B4>>W | 1;
                        DP=DP-1000;
                    }

                    else
                    {
                        B4>>W & 0xFE;
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

                if(DP>=1000)
                {
                    B3>>W | 1;
                    DP=DP-1000;
                }
                else 
                {
                    B3>>W & 0XFE;
                }
            }

            for(J=0;J<=7;J++)
            {
                W=7-J;
                DP=DP*2;
                if(DP>=1000)
                {
                    B4>>W | 1;
                    DP=DP-1000;
                }
                
                else 
                {
                    B3>>W & 0XFE;
                }
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
            B2=0x80 | B2; //COMO AL VALOR SE LE QUITO BIT, ESTE SE LO PONEMOS AL BIT MSB DEL BYTE 2
        }
        else B2=0x7F & B2;//TESTIMOS SI EL BIT ES 0 SE PONE UN 0
        
        CI--;
        
        for(J=0;J<=CI;J++)
        {
            W=CI-J;
            DP=DP*2;
            
            if(DP>=1000)
            {
                B2=B2 | (1 << W);

                DP=DP-1000;
            }

            
        }

        B3=0;
        for(J=0;J<8;J++)
        {
            W=8-J;
            DP=DP*2;

            if(DP >= 1000)
            {
                B3=B3 | (1 << W);
                DP=DP-1000;
            }
        }

        B4=0;
        for(J=0;J<8;J++)
        {
            W=8-J;
            DP=DP*2;

            if(DP>=1000)
            {
                B4=B4 | (1 << W);
                DP=DP-1000;
            }
                        
        }
    }

    ENVIO[0]=B4;
    ENVIO[1]=B3;
    ENVIO[2]=B2;
    ENVIO[3]=B1;

    uartStartSend(&UARTD1, 4, ENVIO);
    chThdSleepMilliseconds(20);
    uartStopSend(&UARTD1);


    // uartStartSend(&UARTD3,4,ENVIO);
    // chThdSleepMilliseconds(15);
    // uartStopSend(&UARTD3);
    return;
}


int main(void)
{
halInit();
chSysInit();
uint8_t timer=100;

palSetPadMode(GPIOC, 7 , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOC, 9 , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOA, 9 , PAL_MODE_OUTPUT_PUSHPULL);
palSetPadMode(GPIOA, 13 , PAL_MODE_OUTPUT_PUSHPULL);
	
    while(true)
	{
	 //        palWritePad(GPIOC, 7, 0);
		// palWritePad(GPIOC, 9, 0);
		// palWritePad(GPIOA, 9, 0);
		// palWritePad(GPIOA, 13, 1);
		// chThdSleepMilliseconds(timer);
	 //        palWritePad(GPIOC, 7, 0);
		// palWritePad(GPIOC, 9, 0);
		// palWritePad(GPIOA, 9, 1);
		// palWritePad(GPIOA, 13, 1);
		// chThdSleepMilliseconds(timer);
	 //        palWritePad(GPIOC, 7, 0);
		// palWritePad(GPIOC, 9, 0);
		// palWritePad(GPIOA, 9, 1);
		// palWritePad(GPIOA, 13, 0);
		// chThdSleepMilliseconds(timer);
		// palWritePad(GPIOC, 7, 0);
		// palWritePad(GPIOC, 9, 1);
		// palWritePad(GPIOA, 9, 1);
		// palWritePad(GPIOA, 13, 0);
		// chThdSleepMilliseconds(timer);
	 //        palWritePad(GPIOC, 7, 0);
		// palWritePad(GPIOC, 9, 1);
		// palWritePad(GPIOA, 9, 0);
		// palWritePad(GPIOA, 13, 0);
		// chThdSleepMilliseconds(timer);
		// palWritePad(GPIOC, 7, 1);
		// palWritePad(GPIOC, 9, 1);
		// palWritePad(GPIOA, 9, 0);
		// palWritePad(GPIOA, 13, 0);
		// chThdSleepMilliseconds(timer);
	 //        palWritePad(GPIOC, 7, 1);
		// palWritePad(GPIOC, 9, 0);
		// palWritePad(GPIOA, 9, 0);
		// palWritePad(GPIOA, 13, 0);
		// chThdSleepMilliseconds(timer);
	 //        palWritePad(GPIOC, 7, 1);
		// palWritePad(GPIOC, 9, 0);
		// palWritePad(GPIOA, 9, 0);
		// palWritePad(GPIOA, 13, 1);
		// chThdSleepMilliseconds(timer);


		palWritePad(GPIOC, 7, 1);
		chThdSleepMilliseconds(timer);
		palWritePad(GPIOC, 7, 1);
		chThdSleepMilliseconds(timer);

	}
}
