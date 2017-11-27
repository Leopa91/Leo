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
#include "ConfEmr3.h"
#include "EepromVirtual.h"
#include "LectureSw.h"
#include "ButtonEmr3.h"
#include "DataSd.h"
#include "RxWire.h"

extern uint64_t float_data;

/***********************************************************************
FUNCION DONDE SE TOMA EL PPU DE LA VENTA ACTUAL, ESTE SE GURADA EN LA SD
CON EL ARCHIVO LLAMADO PPU.TXT; SI EL ARCHIVO NO SE ENCUETRA EL TORAMA 
EL PPU DEL NIVEL 1 QUE ESTA GUARDADO EN EL ARCHIVO CONFIG.TXT. 
***********************************************************************/
void lect_ppu(uint8_t *data_ppu, uint8_t act)
{
    
    if(act == 0x00)                                                 //DETERMINA SI EL ARCHIVO EXISTE
    {
        
        if((data_ppu[0] & 0X0F) == 0x04 || (data_ppu[0] & 0X0F) == 0x05)
        {
            
            write_eeprom(level_1,(0xF0 | (data_ppu[0] & 0X0F)));

            write_eeprom(ppu1_venta1,(data_ppu[2] & 0X0F));
        
            if(data_ppu[3] == '.')
            {
                write_eeprom(ppu2_venta1,(data_ppu[4] & 0X0F));
                write_eeprom(ppu3_venta1,(data_ppu[5] & 0X0F));
                write_eeprom(ppu4_venta1,(data_ppu[6] & 0X0F));          
            }

            else if(data_ppu[4] == '.')
            {
                write_eeprom(ppu2_venta1,(data_ppu[3] & 0X0F));
                write_eeprom(ppu3_venta1,(data_ppu[5] & 0X0F));
                write_eeprom(ppu4_venta1,(data_ppu[6] & 0X0F));          
            }

            else if(data_ppu[5] == '.')
            {
                write_eeprom(ppu2_venta1,(data_ppu[3] & 0X0F));
                write_eeprom(ppu3_venta1,(data_ppu[4] & 0X0F));
                write_eeprom(ppu4_venta1,(data_ppu[6] & 0X0F));          
            }

            else if(data_ppu[6] == '.')
            {
                write_eeprom(ppu2_venta1,(data_ppu[3] & 0X0F));
                write_eeprom(ppu3_venta1,(data_ppu[4] & 0X0F));
                write_eeprom(ppu4_venta1,(data_ppu[5] & 0X0F));          
            }

            sd_venta(1);
        }
        
        if((data_ppu[8] & 0X0F) == 0x04 || (data_ppu[8] & 0X0F) == 0x05)
        {
            write_eeprom(level_2,(0xF0 | (data_ppu[8] & 0X0F)));

            write_eeprom(ppu1_venta2,(data_ppu[10] & 0X0F));
            
            if(data_ppu[11] == '.')
            {
                write_eeprom(ppu2_venta2,(data_ppu[12] & 0X0F));
                write_eeprom(ppu3_venta2,(data_ppu[13] & 0X0F));
                write_eeprom(ppu4_venta2,(data_ppu[14] & 0X0F));          
            }

            else if(data_ppu[12] == '.')
            {
                write_eeprom(ppu2_venta2,(data_ppu[11] & 0X0F));
                write_eeprom(ppu3_venta2,(data_ppu[13] & 0X0F));
                write_eeprom(ppu4_venta2,(data_ppu[14] & 0X0F));          
            }

            else if(data_ppu[13] == '.')
            {
                write_eeprom(ppu2_venta2,(data_ppu[11] & 0X0F));
                write_eeprom(ppu3_venta2,(data_ppu[12] & 0X0F));
                write_eeprom(ppu4_venta2,(data_ppu[14] & 0X0F));          
            }

            else if(data_ppu[14] == '.')
            {
                write_eeprom(ppu2_venta2,(data_ppu[11] & 0X0F));
                write_eeprom(ppu3_venta2,(data_ppu[12] & 0X0F));
                write_eeprom(ppu4_venta2,(data_ppu[13] & 0X0F));          
            }

            sd_venta(2);
        }      
    }

    else if(act == 0x01)
    {
        write_eeprom(level_1,0xF4);
        write_eeprom(ppu1_venta1,ppu1_level1);
        write_eeprom(ppu2_venta1,ppu2_level1);
        write_eeprom(ppu3_venta1,ppu3_level1);
        write_eeprom(ppu4_venta1,ppu4_level1);

        sd_venta(1);

        write_eeprom(level_2,0xF4);
        write_eeprom(ppu1_venta2,ppu1_level1);
        write_eeprom(ppu2_venta2,ppu2_level1);
        write_eeprom(ppu3_venta2,ppu3_level1);
        write_eeprom(ppu4_venta2,ppu4_level1);
        
        sd_venta(2);
    }
       
}

void data_recibo(uint8_t P)
{

	uint8_t recibo[20];

   	uartStartReceive(&UARTD1, P, recibo);
   	chThdSleepMilliseconds(500);
   	uartStopReceive(&UARTD1);
   	aux_pasos = recibo[4];

    if(read_eeprom(log) == 1) emr3_log(&recibo,P,2);
  	//if(read_eeprom(log) == 1)   log_EMR3(recibo,P,2);
   	return;
}

void desconexion(uint8_t pos)
{
    
    uint8_t aux_punto;

    if(pos == 0x01)
    {
        if(read_eeprom(Flag1) == 0X01)
        {

            if(read_eeprom(lado1) == 0X00)
            {

                while(true)
                {
                    aux_punto = ConfEmr3();
                    if(aux_punto == 0x01) break;
                    LCDPrintRow2(0,"   Error=0007   "); 
                }

                if(read_eeprom(estado1) == 0X09 )
                {

                    
                    write_eeprom(pasos1,0x0A);

                }

                else
                {
                    
                    write_eeprom(pasos1,0x00);
                    write_eeprom(autor1,0X00);
                    write_eeprom(estado1,0X06);
                }

                chThdSleepMilliseconds(2000);
            }

            else if (read_eeprom(inicio1) == 0X01)
            {

                write_eeprom(inicio1,0X00);

                if(read_eeprom(estado1) == 0X09)
                {                     
                    write_eeprom(pasos1,0x0A);
                }
            }

            write_eeprom(lado1,0x01);
            write_eeprom(Flag1,0x00);
            write_eeprom(CA_Flag1,0x00);
        }

        else if (read_eeprom(CA_Flag1) == 0X02)
        {
            write_eeprom(lado1,0X00);
            write_eeprom(lado2,0x00);
        }

        else
        {
            write_eeprom(CA_Flag1,(read_eeprom(CA_Flag1) + 0x01));
        }
    }

    else if(pos == 0x02)
    {
        if(read_eeprom(Flag1) == 0X01)
        {

            if(read_eeprom(lado2) == 0X00)
            {

                if(read_eeprom(estado2) == 0X09 )
                {
                
                    write_eeprom(pasos2,0x0A);
                    
                }

                else
                {
                    
                    write_eeprom(pasos2,0x00);
                    write_eeprom(autor2,0X00);
                    write_eeprom(estado2,0X06);
                }

                chThdSleepMilliseconds(2000);
            }

            else if (read_eeprom(inicio2) == 0X01)
            {

                write_eeprom(inicio2,0X00);

                if(read_eeprom(estado2) == 0X09)
                {
                                          
                    write_eeprom(pasos2,0x0A);
                }
            }

            write_eeprom(lado2,0x01);
            write_eeprom(Flag1,0x00);
            write_eeprom(CB_Flag1,0x00);
        }

        else if (read_eeprom(CB_Flag1) == 0X02)
        {
            write_eeprom(lado2,0x00);
        }

        else
        {
            write_eeprom(CB_Flag1,(read_eeprom(CB_Flag1) + 0x01));
        }
    }
    
}

void LectureSw(void)
{
   
    uint16_t DIREC;    
    uint8_t B1;
    uint8_t B2;
    uint8_t B3;
    uint8_t B4;
    uint8_t B5;
    uint8_t B6;
    uint8_t B7;
    uint8_t B8;
    uint8_t B9; 
    uint16_t RES;
    uint8_t i;
    uint8_t j;
    uint8_t envio[20];
    uint8_t recibo[25];
    uint8_t recibo_parcial[25];
    uint16_t ppu;
    uint32_t valor_float;
    uint32_t valor;
    uint64_t dinero;
    uint64_t total_dinero;
    uint16_t punto;
    uint16_t punto_aux;
    uint16_t venta_cero;
    uint32_t cdin_real;
    uint32_t din_real;
    uint32_t vol_real;
    uint64_t total_vol;
    uint64_t total_vol2;

          
    DIREC = (palReadPad(GPIOB,IN_4)*8) + (palReadPad(GPIOD,IN_3)*4) + (palReadPad(GPIOD,IN_2)*2) + (palReadPad(GPIOD,IN_1)*1);

    if(palReadPad(GPIOB,IN_5) == 1 && DIREC == 0X00)
    {
        LCDInit();
        LCDBegin(16,2);   
    }

    switch(DIREC)
    {
        
        case 0X00://ACTUALIZADOR

            if( read_eeprom(consola) == 2 || read_eeprom(consola) == 1 || read_eeprom(consola) == 4)
            {

                if(ACTU != 0X00)
                {
                    write_eeprom(pasos1,0);
                    write_eeprom(pasos2,0);  
                    ACTU=0;
                    
                }

                ////////////////////////////////////////////////////////////////////////
                //ESTADOS
                ///////////////////////////////////////////////////////////////////////////
            
                else if(ACTU == 0X00)
                {
                    //DISPLAY PRINCIPAL
                    if(read_eeprom(pasos1) != 0X00)
                    {
                        lcd_out [0] = ascii(read_eeprom(consola));
                        lcd_out [1] = ' ';
                        lcd_out [2] = ' ';
                        lcd_out [3] = ' ';
                        lcd_out [4] = ascii(read_eeprom(pasos1));
                        lcd_out [5] = ascii(read_eeprom(pasos2));
                        lcd_out [6] = ' ';
                        lcd_out [7] = ascii(read_eeprom(log));
                        lcd_out [8] = ' ';
                        
                        if( read_eeprom(lado1) == 0x01)
                        {
                            lcd_out [9] = ascii(read_eeprom(estado1));
                            lcd_out [10]= ascii(read_eeprom(pos1));
                            lcd_out [11]= ascii(read_eeprom(preset_1));
                        }

                        else
                        {
                            lcd_out [9]='O';
                            lcd_out [10]='F';
                            lcd_out [11]='F';
                        }

                        if(read_eeprom(lado2)  == 0x01)
                        {
                            lcd_out [13]= ascii(read_eeprom(estado2));
                            lcd_out [14]= ascii(read_eeprom(pos2));
                            lcd_out [15]= ascii(read_eeprom(preset_2));
                        }  

                        else
                        {
                            lcd_out [13]='O';
                            lcd_out [14]='F';
                            lcd_out [15]='F';
                        }

                        lcd_out [12]=' ';
                        LCDPrintRow( 0,lcd_out);
                    }
                    else
                    {
                        //CURRENCY:0000000;
                        lcd_out [0]='C';
                        lcd_out [1]='U';
                        lcd_out [2]='R';
                        lcd_out [3]='R';
                        lcd_out [4]='E';
                        lcd_out [5]='N';
                        lcd_out [6]='C';
                        lcd_out [7]='Y';
                        lcd_out [8]=':';

                        switch(read_eeprom(PD))  
                        {
                            case 0:

                                lcd_out [9] = ascii(read_eeprom(d1_1));
                                lcd_out [10] = ascii(read_eeprom(d2_1));
                                lcd_out [11] = ascii(read_eeprom(d3_1));
                                lcd_out [12] = ascii(read_eeprom(d4_1));   
                                lcd_out [13] = ascii(read_eeprom(d5_1));
                                lcd_out [14] = ascii(read_eeprom(d6_1)); 
                                lcd_out [15] = ' ';

                            break;

                            case 1:

                                lcd_out [9] = ascii(read_eeprom(d1_1));
                                lcd_out [10] = ascii(read_eeprom(d2_1));
                                lcd_out [11] = ascii(read_eeprom(d3_1));
                                lcd_out [12] = ascii(read_eeprom(d4_1));   
                                lcd_out [13] = ascii(read_eeprom(d5_1));
                                lcd_out [14] = '.'; 
                                lcd_out [15] = ascii(read_eeprom(d6_1));

                            break;

                            case 2:

                                lcd_out [9] = ascii(read_eeprom(d1_1));
                                lcd_out [10] = ascii(read_eeprom(d2_1));
                                lcd_out [11] = ascii(read_eeprom(d3_1));
                                lcd_out [12] = ascii(read_eeprom(d4_1));   
                                lcd_out [13] = '.';
                                lcd_out [14] = ascii(read_eeprom(d5_1)); 
                                lcd_out [15] = ascii(read_eeprom(d6_1)); 

                            break;

                            case 3:
                                       
                                lcd_out [9] = ascii(read_eeprom(d1_1));
                                lcd_out [10] = ascii(read_eeprom(d2_1));
                                lcd_out [11] = ascii(read_eeprom(d3_1));
                                lcd_out [12] = '.';
                                lcd_out [13] = ascii(read_eeprom(d4_1));   
                                lcd_out [14] = ascii(read_eeprom(d5_1)); 
                                lcd_out [15] = ascii(read_eeprom(d6_1));  

                            break;
                        }
                        
                        LCDPrintRow( 0,lcd_out);
                    }

                    lcd_out [0]=' ';
                    lcd_out [1]=' ';
                    lcd_out [2]='P';
                    lcd_out [3]='R';
                    lcd_out [4]='I';
                    lcd_out [5]='C';
                    lcd_out [6]='E';
                    lcd_out [7]=':';

                    lcd_out [8]= ascii(read_eeprom(ppu1_level1));

                    if(read_eeprom(pppu) == 0X03)
                    {

                        lcd_out [9] = '.';
                        lcd_out [10] = ascii(read_eeprom(ppu2_level1));
                        lcd_out [11] = ascii(read_eeprom(ppu3_level1));
                        lcd_out [12] = ascii(read_eeprom(ppu4_level1));
                    }

                    else if(read_eeprom(pppu) == 0X02)
                    {

                        lcd_out [9] = ascii(read_eeprom(ppu2_level1));
                        lcd_out [10] = '.';
                        lcd_out [11] = ascii(read_eeprom(ppu3_level1));
                        lcd_out [12] = ascii(read_eeprom(ppu4_level1));
                    }

                    else if(read_eeprom(pppu) == 0X01)
                    {

                        lcd_out [9] = ascii(read_eeprom(ppu2_level1));
                        lcd_out [10] = ascii(read_eeprom(ppu3_level1));
                        lcd_out [11] = '.';
                        lcd_out [12] = ascii(read_eeprom(ppu4_level1));
                    }

                    else if(read_eeprom(pppu) == 0X00)
                    {

                        lcd_out [9] = ascii(read_eeprom(ppu2_level1));
                        lcd_out [10] = ascii(read_eeprom(ppu3_level1));
                        lcd_out [11] = ascii(read_eeprom(ppu4_level1));
                        lcd_out [12] = '.';
                    }

                    lcd_out [13]= ' ';
                    lcd_out [14]= ' ';
                    lcd_out [15]= ' ';
                    
                    LCDPrintRow2( 0,lcd_out);
               
                    if(read_eeprom(lado1) == 0x00) chThdSleepMilliseconds(2000);

                    if(read_eeprom(turno_aux) == 0x01) TURNO = 0X01;

                    if (TURNO == 0X01)
                    {
                        
                        if(read_eeprom(stop1) == 0x01 && read_eeprom(pausa1) == 0x01)
                        {
                            ButtonEmr(TECLADO_PAUSA,TURNO);
                            data_recibo(7);
                            write_eeprom(pasos1,0x0A);
                            write_eeprom(stop1,0x00);
                        }

                        else if(read_eeprom(pausa1) == 0x01 && read_eeprom(estado1) == 0X0C)
                        {

                            ButtonEmr(ESTADO_DISPLAY,TURNO);
                            uartStartReceive(&UARTD1, 8, recibo);
                            chThdSleepMilliseconds(500);
                            uartStopReceive(&UARTD1);
                            
                            if(recibo[0X04] == 0X08)
                            {
                                if(recibo[0X05] != 0X02) 
                                {
                                    write_eeprom(pasos1,0x0B);
                                    write_eeprom(pausa1,0x00); 
                                    
                                    if(recibo[0X05] == 0x04)
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                    }                                                           
                                }                            
                            }                            

                            TURNO = 0X02;
                            return;
                        }

                        else if(read_eeprom(pausa1) == 0x01 && read_eeprom(estado1) == 0X09)
                        {
                            ButtonEmr(INICIO,TURNO);
                            data_recibo(7);
                            write_eeprom(pasos1,0x0A);
                            write_eeprom(pausa1,0x00);
                        }
                                                
                        write_eeprom(Flag1,0X00);

                        //ESTADO DISPLAY
                        ButtonEmr(ESTADO_DISPLAY,TURNO);
                        uartStartReceive(&UARTD1, 8, recibo);
                        chThdSleepMilliseconds(500);
                        uartStopReceive(&UARTD1);

                        if(read_eeprom(log) == 1) emr3_log(&recibo,8,2);
                        
                        desconexion(TURNO);
                        
                        if(read_eeprom(lado1) == 0X01)
                        {          

                            //ESTADO DISPLAY
                            ButtonEmr(ESTADO_DISPLAY,TURNO);
                            uartStartReceive(&UARTD1, 8, recibo);
                            chThdSleepMilliseconds(500);
                            uartStopReceive(&UARTD1);

                            if((read_eeprom(estado1) == 0X06 || read_eeprom(estado1) == 0X08) && read_eeprom(pasos1) == 0X00)
                            {
                                
                                punto = mult(read_eeprom(pppu));
                                valor = ((read_eeprom(ppu1_level1)*1000) + (read_eeprom(ppu2_level1)*100) + (read_eeprom(ppu3_level1)*10) + (read_eeprom(ppu4_level1)*1));
                                
                                valor_float=valor%punto;
                                valor=valor/punto;
                                f_pf(valor,valor_float,1);
                                
                                ButtonEmr(ENVIAR_PPU,TURNO);
                                data_recibo(7); 
                                
                            }

                            if(read_eeprom(pasos1) == 0X00)
                            {
                                
                                if(recibo[0X04] == 0X08)
                                {

                                    if(recibo[0X05] == 0X00 && (read_eeprom(estado1) == 0X06 || read_eeprom(estado1) == 0X07))
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                        write_eeprom(estado1,0X06);
                                        write_eeprom(pasos1,0x00);                                             
                                    }

                                    else if(recibo[0X05] == 0X02)
                                    {
                                        
                                        if(read_eeprom(consola) == 0x01 && read_eeprom(estado1) == 0X06)
                                        {
                                            venta_ppu(0xF4,TURNO);
                                            write_eeprom(estado1,0X09);
                                            write_eeprom(pasos1,0x02);                                                
                                        }

                                        else if(read_eeprom(estado1) == 0X06)
                                        {
                                            write_eeprom(estado1,0X07);
                                            write_eeprom(pasos1,0x00);
                                        }
                  
                                        else if (read_eeprom(estado1) == 0X08)
                                        {
                                            write_eeprom(estado1,0X09);
                                            write_eeprom(pasos1,0x02);                                               
                                        }
                                    }

                                    else if(recibo[0X05] == 0X03 && read_eeprom(estado1) == 0X06)
                                    {
                                        ButtonEmr(LEER_DISPLAY,1);
                                        uartStartReceive(&UARTD1, 16, recibo_parcial);
                                        chThdSleepMilliseconds(500);
                                        uartStopReceive(&UARTD1);

                                        if(recibo_parcial[4] == 0x6B)
                                        {
                                            if(recibo_parcial[5] != 0X01)
                                            {
                                                ButtonEmr(TECLADO_MODO,TURNO);
                                                data_recibo(7);
                                            }
                                        }
                                    }

                                    else if(recibo[0X05] == 0X03 && read_eeprom(estado1) == 0X08)
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);

                                        punto = mult(read_eeprom(pppu));
                                        valor = ((read_eeprom(ppu1_level1)*1000) + (read_eeprom(ppu2_level1)*100) + (read_eeprom(ppu3_level1)*10) + (read_eeprom(ppu4_level1)*1));
                                        
                                        valor_float=valor%punto;
                                        valor=valor/punto;
                                        f_pf(valor,valor_float,1);
                                        
                                        ButtonEmr(ENVIAR_PPU,TURNO);
                                        data_recibo(7); 
                                    }

                                    else if(recibo[0X05] == 0X00 && read_eeprom(estado1) == 0X08)
                                    {
                                        
                                        ButtonEmr(LEER_DISPLAY,1);                           
                                        uartStartReceive(&UARTD1, 16, recibo_parcial);
                                        chThdSleepMilliseconds(500);
                                        uartStopReceive(&UARTD1);

                                        if(recibo_parcial[4] == 0x6B)
                                        {
                                            if(recibo_parcial[5] != 0X01)
                                            {
                                                ButtonEmr(TECLADO_MODO,TURNO);
                                                data_recibo(7);
                                            }
                                        }
                                    }
                                }
                            }

                            else if(read_eeprom(pasos1) == 0X0A)
                            {
                                if(recibo[0X04] == 0X08)
                                {
                                    if(recibo[0X05] != 0X02) 
                                    {
                                        write_eeprom(pasos1,0x0B); 
                                        
                                        if(recibo[0X05] == 0x04)
                                        {
                                            ButtonEmr(TECLADO_FIN,TURNO);
                                            data_recibo(7);
                                        }                                                           

                                    }

                                    else 
                                    { 
                                        write_eeprom(pasos1,0x08);
                                    }    
                                }
                                
                                else 
                                {                                     
                                    write_eeprom(pasos1,0x0A);
                                } 
                            } 
                              
                            if(read_eeprom(pasos1) == 0X02)
                            {
                                ButtonEmr(TECLADO_FIN,TURNO);
                                data_recibo(7); 

                                write_eeprom(venta_sd,0x01);

                                //diner
                                write_eeprom(real_din16,0);                                        
                                write_eeprom(real_din15,0);
                                write_eeprom(real_din14,0);                                
                                write_eeprom(real_din13,0);                                
                                write_eeprom(real_din12,0);                                
                                write_eeprom(real_din11,0);

                                //volumen
                                write_eeprom(real_vol16,0);                          
                                write_eeprom(real_vol15,0);                                
                                write_eeprom(real_vol14,0);                                
                                write_eeprom(real_vol13,0);                                
                                write_eeprom(real_vol12,0);                                
                                write_eeprom(real_vol11,0); 

                                write_eeprom(pasos1,0x03);                                                                                          

                                B9 = read_eeprom(level);
                                write_eeprom(level,(B9 & 0X0F));
                            }

                            //Enviar el PPU
                            else if (read_eeprom(pasos1) == 0X03)
                            {
                                punto = mult(read_eeprom(pppu));
                                valor = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));
                                valor_float = valor%punto;
                                valor = valor/punto;

                                f_pf(valor,valor_float,1);

                                ButtonEmr(ENVIAR_PPU,TURNO);
                                data_recibo(7);
                                
                                if(aux_pasos != 0x00)
                                {
                                    write_eeprom(pasos1,0x03);
                                    if(aux_pasos == 0x02)
                                    {
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(autor1,0x00);
                                        write_eeprom(estado1,0X06);
                                        write_eeprom(preset_1,0X00);
                                    }
                                }
            
                                else if(aux_pasos == 0x00)
                                {
                                    
                                    write_eeprom(pasos1,0x04);
                                    ButtonEmr(TECLADO_INICIO,TURNO);
                                    data_recibo(7);
                                    chThdSleepMilliseconds(300);
                                }                 
                            }

                            //enviar de preset
                            else if (read_eeprom(pasos1) == 0X04)
                            {                          
                                //volumen
                                if(read_eeprom(preset_1) == 1)
                                {                           
                                    punto=mult(read_eeprom(PV));
                                    valor=((read_eeprom(preset1_1)*100000)+(read_eeprom(preset1_2)*10000)+(read_eeprom(preset1_3)*1000)+(read_eeprom(preset1_4)*100)+(read_eeprom(preset1_5)*10)+(read_eeprom(preset1_6)*1));
                                    valor_float=valor%punto;
                                    valor=valor/punto;
                                    f_pf(valor,valor_float,2);
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(1,TURNO);
                                }

                                //dinero
                                else if(read_eeprom(preset_1) == 2)
                                {
                                    ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));
                                    dinero = ((read_eeprom(preset1_1)*100000)+(read_eeprom(preset1_2)*10000)+(read_eeprom(preset1_3)*1000)+(read_eeprom(preset1_4)*100)+(read_eeprom(preset1_5)*10)+(read_eeprom(preset1_6)*1));
                                   
                                    if(read_eeprom(PD) == read_eeprom(pppu))
                                    {
                                        punto=mult(read_eeprom(PV));
                                        valor = dinero/ppu;
                                        valor_float = ((dinero*punto)/ppu)-(valor*punto);
                                    }

                                    else if (read_eeprom(PD) < read_eeprom(pppu))
                                    {
                                      
                                        punto_aux=mult(read_eeprom(pppu) - read_eeprom(PD));
                                        valor = (dinero * punto_aux)/ppu;
                                        punto=mult(read_eeprom(PV));
                                        valor_float = ((dinero*punto*punto_aux)/ppu)-(valor*punto);
                                    }

                                    else if (read_eeprom(PD) > read_eeprom(pppu))
                                    {
                                      
                                        punto_aux = mult(read_eeprom(pppu) - read_eeprom(PD));
                                        valor = dinero/(ppu*punto_aux);
                                        punto=mult(read_eeprom(PV));
                                        valor_float = ((dinero*punto)/(ppu*punto_aux))-(valor*punto);                                      
                                    }

                                    f_pf(valor,valor_float,2);
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(2,TURNO);
                                }

                                //full
                                else if(read_eeprom(preset_1) == 0) 
                                {

                                    if(read_eeprom(consola) == 0x04)
                                    {
                                        write_eeprom(ieee_1,0X00);
                                        write_eeprom(ieee_2,0X00);
                                        write_eeprom(ieee_3,0XC8);
                                        write_eeprom(ieee_4,0X42);
                                    }

                                    else
                                    {
                                        write_eeprom(ieee_1,0X00);
                                        write_eeprom(ieee_2,0X00);
                                        write_eeprom(ieee_3,0X7A);
                                        write_eeprom(ieee_4,0X43); 
                                    }
                                    
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(0,TURNO);
                                    
                                }

                                if(aux_pasos != 0x00)
                                {
                                    write_eeprom(pasos1,0x04);
                                }
                                else if(aux_pasos == 0x00)
                                {
                                    write_eeprom(pasos1,0x05);
                                }
                                
                            }
                        
                            //AUTORIZACION
                            else if(read_eeprom(pasos1) == 0X05)
                            {
                                        
                                if(read_eeprom(estado1) == 0x09)
                                {
                                                                          
                                    ButtonEmr(AUTORIZAR,TURNO);
                                    data_recibo(7);
                                    write_eeprom(autor1,0x00);
                                    write_eeprom(estado1,0X09);
                                    write_eeprom(preset_1,0X00);                                    
                                    write_eeprom(pasos1,0x07);
                                    Status_1(0X09);
                                    write_eeprom(SD_CONF,0x01);
                                }
                                else
                                {
                                    write_eeprom(pasos1,0x00);
                                }
                            }

                            //LECTURA DE FECHA                            
                            else if (read_eeprom(pasos1) == 0X07) 
                            {
                                write_eeprom(pasos1,0x08);
                                ButtonEmr(FECHAS,TURNO);
                                uartStartReceive(&UARTD1, 11, recibo);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo,11,2);

                                if(recibo[4] == 0x64)
                                {
                                   	sd_dia(&recibo);
                                }
                            }

                            //REAL TIMER
                            else if (read_eeprom(pasos1) == 0X08) 
                            {
                                
                                write_eeprom(pasos1,0x0A);
                                ButtonEmr(LEER_DISPLAY,TURNO);
                                uartStartReceive(&UARTD1, 16, recibo_parcial);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1); 

                                if(read_eeprom(log) == 1) emr3_log(&recibo_parcial,16,2);  

                                for(i=0;i<14;i++)
                                {
                                    recibo[i]=0x00;
                                }

                                j=0X05;

                                for(i=5;i<14;i++)
                                {
                                    if(recibo_parcial[i] != 0X2E && recibo_parcial[i] != 0X2C)
                                    {
                                        recibo[j] = recibo_parcial[i];
                                        j++;
                                    } 
                                }

                                B8=0X0F & recibo[13];
                                B7=0X0F & recibo[12];
                                B6=0X0F & recibo[11];
                                B5=0X0F & recibo[10];
                                B4=0X0F & recibo[9];
                                B3=0X0F & recibo[8];
                                B2=0X0F & recibo[7];
                                B1=0X0F & recibo[6];

                                if(recibo[0x05] == 0x01)
                                {
                                    din_real = ((B1 * 100000) + (B2 * 10000) + (B3 * 1000) + (B4 * 100) + (B5 * 10) + B6);
                                    ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                    if(din_real == 0) vol_real = 0;
                                    else
                                    {
                                        vol_real = ((din_real)/ppu);
                                    }

                                    //diner
                                    write_eeprom(real_din16,(din_real%10));                                        
                                    din_real = din_real/10;
                                    write_eeprom(real_din15,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din14,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din13,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din12,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din11,(din_real%10));

                                    //volumen
                                    write_eeprom(real_vol16,(vol_real%10));                                    
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol15,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol14,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol13,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol12,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol11,(vol_real%10));                                   
                                }

                                else if(recibo[0x05] == 0x00)
                                {
                                    ButtonEmr(TECLADO_MODO,TURNO);
                                    data_recibo(7);                                        
                                    vol_real = ((B1*100000) + (B2*10000) + (B3*1000) + (B4*100) + (B5*10) + B6);
                                    ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                    if(vol_real == 0) din_real = 0;
                                    else
                                    {
                                        din_real = (vol_real * ppu ) / ((mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) / mult(read_eeprom(PD)));
                                    }

                                    //diner
                                    write_eeprom(real_din16,(din_real%10));                                        
                                    din_real = din_real/10;
                                    write_eeprom(real_din15,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din14,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din13,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din12,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din11,(din_real%10));

                                    //volumen
                                    write_eeprom(real_vol16,(vol_real%10));                                    
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol15,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol14,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol13,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol12,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol11,(vol_real%10));                                   
                                }

                                else
                                {
                                    ButtonEmr(TECLADO_MODO,TURNO);
                                    data_recibo(7);                                    
                                }     
                            }

                            //Volumen
                            else if (read_eeprom(pasos1) == 0X0B)
                            {
                                
                                //hora                                   
                                ButtonEmr(HORAS,TURNO);
                                uartStartReceive(&UARTD1, 10, recibo);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo,10,2);
                                           
                                if(recibo[4] ==  0X69)
                                {
                                   sd_hora(&recibo);
                                }

                                ButtonEmr(PEDIR_VENTA,TURNO);
                                uartStartReceive(&UARTD1, 23, recibo_parcial);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo_parcial,15,2);

                                float_data = 0x00;

                                if(recibo_parcial[4] == 0x67 && recibo_parcial[14] == 0X7E)
                                {   
                                    
                                    emr3_sale1(&recibo_parcial,15,2);

                                    Pf_p(recibo_parcial);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                          
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[15] == 0X7E)
                                {
                                    
                                    emr3_sale1(&recibo_parcial,16,2);

                                    for(i=0;i<15;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<15;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[16] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,17,2);

                                    for(i=0;i<16;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<16;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[17] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,18,2);

                                    for(i=0;i<17;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<17;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[18] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,19,2);

                                    for(i=0;i<18;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<18;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[19] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,20,2);

                                    for(i=0;i<19;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<19;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[20] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,21,2);

                                    for(i=0;i<20;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<20;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[21] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,22,2);

                                    for(i=0;i<21;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<21;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[22] == 0X7E)
                                {

                                    emr3_sale1(&recibo_parcial,23,2);

                                    for(i=0; i<22; i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5; i<22; i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_1,0);
                                        write_eeprom(v5_1,0);
                                        write_eeprom(v4_1,0);
                                        write_eeprom(v3_1,0);
                                        write_eeprom(v2_1,0);
                                        write_eeprom(v1_1,0);
                                        write_eeprom(d6_1,0);
                                        write_eeprom(d5_1,0);
                                        write_eeprom(d4_1,0);
                                        write_eeprom(d3_1,0);
                                        write_eeprom(d2_1,0);
                                        write_eeprom(d1_1,0);                                        
                                        write_eeprom(estado1,0x06);
                                        Status_1(0X06);
                                        write_eeprom(pasos1,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos1,0x0C);                                        
                                        cdin_real=float_data;
                                        write_eeprom(v6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_1,(cdin_real%10));
                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta1)*1000) + (read_eeprom(ppu2_venta1)*100) + (read_eeprom(ppu3_venta1)*10) + (read_eeprom(ppu4_venta1)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_1,(cdin_real%10));
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_1,(cdin_real%10));
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td1_1) * 10000000) + (read_eeprom(td1_2) * 1000000) + (read_eeprom(td1_3) * 100000) + (read_eeprom(td1_4) * 10000) + (read_eeprom(td1_5) * 1000) + (read_eeprom(td1_6) * 100) + (read_eeprom(td1_7) * 10) + (read_eeprom(td1_8) * 1);
                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td1_8,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_7,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_6,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_5,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_4,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_3,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_2,(total_dinero%10));
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td1_1,(total_dinero%10));
                                        sd_Tdinero(TURNO);
                                                                           
                                    }
                                }

                                else
                                {
                                    
                                    write_eeprom(pasos1,0x0B);
                                }
                            }       

                            //TOTAL DE VOLUMEN   
                            else if (read_eeprom(pasos1) == 0X0C)
                            { 
                                
                                LeerTotales(TURNO);                                                                

                                if(read_eeprom(consola) == 0x01)
                                {
                                    Status_1(0x06);
                                } 

                                write_eeprom(SD_CONF,0x01);
                                write_eeprom(SD_TRANS,0x01);
                                
                            
                                if(read_eeprom(consola) == 1)
                                {
                                    write_eeprom(estado1,0X06);
                                }

                                else 
                                {
                                    write_eeprom(estado1,0X0B);
                                }

                                write_eeprom(pasos1,0x00);
                            }  
                        }
                    
                        TURNO=0X02;
                    }

                    else if (TURNO == 0X02)
                    {
                        
                        if(read_eeprom(stop2) == 0x01 && read_eeprom(pausa2) == 0x01)
                        {
                            ButtonEmr(TECLADO_PAUSA,TURNO);
                            data_recibo(7);
                            write_eeprom(stop2,0x00);
                        }

                        else if(read_eeprom(pausa2) == 0x01 && read_eeprom(estado2) == 0X0C)
                        {

                            ButtonEmr(ESTADO_DISPLAY,TURNO);
                            uartStartReceive(&UARTD1, 8, recibo);
                            chThdSleepMilliseconds(500);
                            uartStopReceive(&UARTD1);
                            
                            if(recibo[0X04] == 0X08)
                            {
                                if(recibo[0X05] != 0X02) 
                                {
                                    write_eeprom(pasos2,0x0B);
                                    write_eeprom(pausa2,0x00); 
                                    
                                    if(recibo[0X05] == 0x04)
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                    }                                                           
                                }                            
                            }                            

                            TURNO = 0X01;
                            return;
                        }

                        else if(read_eeprom(pausa2) == 0x01 && read_eeprom(estado2) == 0X09)
                        {
                            ButtonEmr(INICIO,TURNO);
                            data_recibo(7);
                            write_eeprom(pasos2,0x0A);
                            write_eeprom(pausa2,0x00);
                        }

                                                
                        write_eeprom(Flag1,0X00);

                        //ESTADO DISPLAY
                        ButtonEmr(ESTADO_DISPLAY,TURNO);
                        uartStartReceive(&UARTD1, 8, recibo);
                        chThdSleepMilliseconds(500);
                        uartStopReceive(&UARTD1);

                        if(read_eeprom(log) == 1) emr3_log(&recibo,8,2);
                        
                        desconexion(TURNO);
                        
                        if(read_eeprom(lado2) == 0X01)
                        {          

                            //ESTADO DISPLAY
                            ButtonEmr(ESTADO_DISPLAY,TURNO);
                            uartStartReceive(&UARTD1, 8, recibo);
                            chThdSleepMilliseconds(500);
                            uartStopReceive(&UARTD1);

                            if((read_eeprom(estado2) == 0X06 || read_eeprom(estado2) == 0X08) && read_eeprom(pasos2) == 0X00)
                            {
                                
                                punto = mult(read_eeprom(pppu));
                                valor = ((read_eeprom(ppu1_level1)*1000) + (read_eeprom(ppu2_level1)*100) + (read_eeprom(ppu3_level1)*10) + (read_eeprom(ppu4_level1)*1));
                                
                                valor_float=valor%punto;
                                valor=valor/punto;
                                f_pf(valor,valor_float,1);
                                
                                ButtonEmr(ENVIAR_PPU,TURNO);
                                data_recibo(7); 
                                
                            }

                            if(read_eeprom(pasos2) == 0X00)
                            {

                                
                                if(recibo[0X04] == 0X08)
                                {

                                    if(recibo[0X05] == 0X00 && (read_eeprom(estado2) == 0X06 || read_eeprom(estado2) == 0X07))
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                        write_eeprom(estado2,0X06);
                                        write_eeprom(pasos2,0x00);                                             
                                    }

                                    else if(recibo[0X05] == 0X02)
                                    {
                                        
                                        if(read_eeprom(consola) == 0x01 && read_eeprom(estado2) == 0X06)
                                        {
                                            venta_ppu(0xF4,TURNO);
                                            write_eeprom(estado2,0X09);
                                            write_eeprom(pasos2,0x02);                                                
                                        }

                                        else if(read_eeprom(estado2) == 0X06)
                                        {
                                            write_eeprom(estado2,0X07);
                                            write_eeprom(pasos2,0x00);
                                        }
                  
                                        else if (read_eeprom(estado2) == 0X08)
                                        {
                                            write_eeprom(estado2,0X09);
                                            write_eeprom(pasos2,0x02);                                               
                                        }
                                    }

                                    else if(recibo[0X05] == 0X03 && read_eeprom(estado2) == 0X06)
                                    {
                                        ButtonEmr(LEER_DISPLAY,TURNO);
                                        uartStartReceive(&UARTD1, 16, recibo_parcial);
                                        chThdSleepMilliseconds(500);
                                        uartStopReceive(&UARTD1);

                                        if(recibo_parcial[4] == 0x6B)
                                        {
                                            if(recibo_parcial[5] != 0X01)
                                            {
                                                ButtonEmr(TECLADO_MODO,TURNO);
                                                data_recibo(7);
                                            }
                                        }
                                    }

                                    else if(recibo[0X05] == 0X03 && read_eeprom(estado2) == 0X08)
                                    {
                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);

                                        punto = mult(read_eeprom(pppu));
                                        valor = ((read_eeprom(ppu1_level1)*1000) + (read_eeprom(ppu2_level1)*100) + (read_eeprom(ppu3_level1)*10) + (read_eeprom(ppu4_level1)*1));
                                        
                                        valor_float=valor%punto;
                                        valor=valor/punto;
                                        f_pf(valor,valor_float,1);
                                        
                                        ButtonEmr(ENVIAR_PPU,TURNO);
                                        data_recibo(7); 
                                    }

                                    else if(recibo[0X05] == 0X00 && read_eeprom(estado2) == 0X08)
                                    {
                                        
                                        ButtonEmr(LEER_DISPLAY,TURNO);                           
                                        uartStartReceive(&UARTD1, 16, recibo_parcial);
                                        chThdSleepMilliseconds(500);
                                        uartStopReceive(&UARTD1);

                                        if(recibo_parcial[4] == 0x6B)
                                        {
                                            if(recibo_parcial[5] != 0X01)
                                            {
                                                ButtonEmr(TECLADO_MODO,TURNO);
                                                data_recibo(7);
                                            }
                                        }
                                    }
                                }
                            }

                            else if(read_eeprom(pasos2) == 0X0A)
                            {

                                if(recibo[0X04] == 0X08)
                                {

                                    if(recibo[0X05] != 0X02) 
                                    {
                                        write_eeprom(pasos2,0x0B); 

                                        if(recibo[0X05] == 0x04)
                                        {
                                            ButtonEmr(TECLADO_FIN,TURNO);
                                            data_recibo(7);
                                        }                                                           
                                    }

                                    else 
                                    { 
                                        write_eeprom(pasos2,0x08);
                                    }    
                                }
                                
                                else 
                                {                                     
                                    write_eeprom(pasos2,0x0A);
                                } 
                            } 
                              
                            if(read_eeprom(pasos2) == 0X02)
                            {
                                ButtonEmr(TECLADO_FIN,TURNO);
                                data_recibo(7); 

                                write_eeprom(venta_sd,0x01);
                                //diner
                                write_eeprom(real_din26,0);                                        
                                write_eeprom(real_din25,0);
                                write_eeprom(real_din24,0);                                
                                write_eeprom(real_din23,0);                                
                                write_eeprom(real_din22,0);                                
                                write_eeprom(real_din21,0);

                                //volumen
                                write_eeprom(real_vol26,0);                          
                                write_eeprom(real_vol25,0);                                
                                write_eeprom(real_vol24,0);                                
                                write_eeprom(real_vol23,0);                                
                                write_eeprom(real_vol22,0);                                
                                write_eeprom(real_vol21,0); 

                                write_eeprom(pasos2,0x03);

                                B9 = read_eeprom(level);
                                write_eeprom(level,(B9 & 0XF0));                                                                                          
                            }

                            //Enviar el PPU
                            else if (read_eeprom(pasos2) == 0X03)
                            {
                                punto = mult(read_eeprom(pppu));
                                valor = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));
                                valor_float = valor%punto;
                                valor = valor/punto;

                                f_pf(valor,valor_float,1);

                                ButtonEmr(ENVIAR_PPU,TURNO);
                                data_recibo(7);
                                
                                if(aux_pasos != 0x00)
                                {
                                    write_eeprom(pasos2,0x03);
                                    if(aux_pasos == 0x02)
                                    {
                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(autor2,0x00);
                                        write_eeprom(estado2,0X06);
                                        write_eeprom(preset_2,0X00);
                                    }
                                }
            
                                else if(aux_pasos == 0x00)
                                {
                                    sd_venta(TURNO);
                                    write_eeprom(pasos2,0x04);
                                    ButtonEmr(TECLADO_INICIO,TURNO);
                                    data_recibo(7);
                                    chThdSleepMilliseconds(300);
                                }                 
                            }

                            //enviar de preset
                            else if (read_eeprom(pasos2) == 0X04)
                            {                          
                                //volumen
                                if(read_eeprom(preset_2) == 1)
                                {                           
                                    punto=mult(read_eeprom(PV));
                                    valor=((read_eeprom(preset2_1)*100000)+(read_eeprom(preset2_2)*10000)+(read_eeprom(preset2_3)*1000)+(read_eeprom(preset2_4)*100)+(read_eeprom(preset2_5)*10)+(read_eeprom(preset2_6)*1));
                                    valor_float=valor%punto;
                                    valor=valor/punto;
                                    f_pf(valor,valor_float,2);
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(1,TURNO);
                                }

                                //dinero
                                else if(read_eeprom(preset_2) == 2)
                                {
                                    ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));
                                    dinero = ((read_eeprom(preset2_1)*100000)+(read_eeprom(preset2_2)*10000)+(read_eeprom(preset2_3)*1000)+(read_eeprom(preset2_4)*100)+(read_eeprom(preset2_5)*10)+(read_eeprom(preset2_6)*1));
                                   
                                    if(read_eeprom(PD) == read_eeprom(pppu))
                                    {
                                        punto=mult(read_eeprom(PV));
                                        valor = dinero/ppu;
                                        valor_float = ((dinero*punto)/ppu)-(valor*punto);
                                    }

                                    else if (read_eeprom(PD) < read_eeprom(pppu))
                                    {
                                      
                                        punto_aux=mult(read_eeprom(pppu) - read_eeprom(PD));
                                        valor = (dinero * punto_aux)/ppu;
                                        punto=mult(read_eeprom(PV));
                                        valor_float = ((dinero*punto*punto_aux)/ppu)-(valor*punto);
                                    }

                                    else if (read_eeprom(PD) > read_eeprom(pppu))
                                    {
                                      
                                        punto_aux = mult(read_eeprom(pppu) - read_eeprom(PD));
                                        valor = dinero/(ppu*punto_aux);
                                        punto=mult(read_eeprom(PV));
                                        valor_float = ((dinero*punto)/(ppu*punto_aux))-(valor*punto);                                      
                                    }

                                    f_pf(valor,valor_float,2);
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(2,TURNO);
                                }

                                //full
                                else if(read_eeprom(preset_2) == 0) 
                                {

                                    write_eeprom(ieee_1,0X00);
                                    write_eeprom(ieee_2,0X00);
                                    write_eeprom(ieee_3,0X7A);
                                    write_eeprom(ieee_4,0X43);
                                    ButtonEmr(ENVIAR_VOLUMEN,TURNO);
                                    data_recibo(7);
                                    sd_preset(0,TURNO);
                                    
                                }

                                if(aux_pasos != 0x00)
                                {
                                    write_eeprom(pasos2,0x04);
                                }
                                else if(aux_pasos == 0x00)
                                {
                                    write_eeprom(pasos2,0x05);
                                }
                                
                            }
                        
                            //AUTORIZACION
                            else if(read_eeprom(pasos2) == 0X05)
                            {
                                        
                                if(read_eeprom(estado2) == 0x09)
                                {
                                                                          
                                    ButtonEmr(AUTORIZAR,TURNO);
                                    data_recibo(7);
                                    write_eeprom(autor2,0x00);
                                    write_eeprom(estado2,0X09);
                                    write_eeprom(preset_2,0X00);                                    
                                    write_eeprom(pasos2,0x07);
                                    Status_2(0X09);
                                    write_eeprom(SD_CONF,0x01);
                                }
                                else
                                {
                                    write_eeprom(pasos2,0x00);
                                }
                            }

                            //LECTURA DE FECHA                            
                            else if (read_eeprom(pasos2) == 0X07) 
                            {
                                write_eeprom(pasos2,0x08);
                                ButtonEmr(FECHAS,TURNO);
                                uartStartReceive(&UARTD1, 11, recibo);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo,11,2);

                                if(recibo[4] == 0x64)
                                {
                                    sd_dia(&recibo);
                                }
                            }

                            //REAL TIMER
                            else if (read_eeprom(pasos2) == 0X08) 
                            {
                                
                                write_eeprom(pasos2,0x0A);
                                ButtonEmr(LEER_DISPLAY,TURNO);
                                uartStartReceive(&UARTD1, 16, recibo_parcial);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1); 

                                if(read_eeprom(log) == 1) emr3_log(&recibo_parcial,16,2);  

                                for(i=0;i<14;i++)
                                {
                                    recibo[i]=0x00;
                                }

                                j=0X05;

                                for(i=5;i<14;i++)
                                {
                                    if(recibo_parcial[i] != 0X2E && recibo_parcial[i] != 0X2C)
                                    {
                                        recibo[j] = recibo_parcial[i];
                                        j++;
                                    } 
                                }

                                B8=0X0F & recibo[13];
                                B7=0X0F & recibo[12];
                                B6=0X0F & recibo[11];
                                B5=0X0F & recibo[10];
                                B4=0X0F & recibo[9];
                                B3=0X0F & recibo[8];
                                B2=0X0F & recibo[7];
                                B1=0X0F & recibo[6];

                                if(recibo[0x05] == 0x01)
                                {
                                    
                                    din_real = ((B1 * 100000) + (B2 * 10000) + (B3 * 1000) + (B4 * 100) + (B5 * 10) + B6);
                                    ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                    if(din_real == 0) vol_real = 0;
                                    else
                                    {
                                        vol_real = ((din_real)/ppu);
                                    }

                                    //diner
                                    write_eeprom(real_din26,(din_real%10));                                        
                                    din_real = din_real/10;
                                    write_eeprom(real_din25,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din24,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din23,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din22,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din21,(din_real%10));

                                    //volumen
                                    write_eeprom(real_vol26,(vol_real%10));                                    
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol25,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol24,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol23,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol22,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol21,(vol_real%10));                                   
                                }

                                else if(recibo[0x05] == 0x00)
                                {
                                    ButtonEmr(TECLADO_MODO,TURNO);
                                    data_recibo(7);                                        
                                    vol_real = ((B1*100000) + (B2*10000) + (B3*1000) + (B4*100) + (B5*10) + B6);
                                    ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                    if(vol_real == 0) din_real = 0;
                                    else
                                    {
                                        din_real = (vol_real * ppu ) / ((mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) / mult(read_eeprom(PD)));
                                    }

                                    //diner
                                    write_eeprom(real_din26,(din_real%10));                                        
                                    din_real = din_real/10;
                                    write_eeprom(real_din25,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din24,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din23,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din22,(din_real%10));
                                    din_real = din_real/10;
                                    write_eeprom(real_din21,(din_real%10));

                                    //volumen
                                    write_eeprom(real_vol26,(vol_real%10));                                    
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol25,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol24,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol23,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol22,(vol_real%10));
                                    vol_real = vol_real/10;
                                    write_eeprom(real_vol21,(vol_real%10));                                   
                                }

                                else
                                {
                                    ButtonEmr(TECLADO_MODO,TURNO);
                                    data_recibo(7);                                    
                                }     
                            }

                            //Volumen
                            else if (read_eeprom(pasos2) == 0X0B)
                            {
                                
                                //hora                                   
                                ButtonEmr(HORAS,TURNO);
                                uartStartReceive(&UARTD1, 10, recibo);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo,10,2);
                                           
                                if(recibo[4] ==  0X69)
                                {
                                   sd_hora(&recibo);
                                }

                                ButtonEmr(PEDIR_VENTA,TURNO);
                                uartStartReceive(&UARTD1, 23, recibo_parcial);
                                chThdSleepMilliseconds(500);
                                uartStopReceive(&UARTD1);

                                if(read_eeprom(log) == 1) emr3_log(&recibo_parcial,15,2);

                                float_data = 0x00;

                                

                                if(recibo_parcial[4] == 0x67 && recibo_parcial[14] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,15,2);

                                    Pf_p(recibo_parcial);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[15] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,16,2);

                                    for(i=0;i<15;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<15;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[16] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,17,2);

                                    for(i=0;i<16;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<16;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[17] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,18,2);

                                    for(i=0;i<17;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<17;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[18] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,19,2);

                                    for(i=0;i<18;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<18;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[19] == 0X7E)
                                {   

                                    emr3_sale2(&recibo_parcial,20,2);
                                    
                                    for(i=0;i<19;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<19;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[20] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,21,2);

                                    for(i=0;i<20;i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5; i<20; i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[21] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,22,2);

                                    for(i=0; i<21; i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5; i<21; i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                               
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else if(recibo_parcial[4] == 0x67 && recibo_parcial[22] == 0X7E)
                                {   
                                    
                                    emr3_sale2(&recibo_parcial,23,2);

                                    for(i=0; i<22; i++)
                                    {
                                        recibo[i]=0x00;
                                    }

                                    j=0X05;

                                    for(i=5;i<22;i++)
                                    {

                                        recibo[j] = recibo_parcial[i];                                        

                                        if(recibo_parcial[i] == 0X7D)
                                        {
                                            i++;
                                            recibo[j] = 0X70 | (recibo_parcial[i] & 0X0F);
                                        } 

                                        j++;                                                                              
                                    }

                                    Pf_p(recibo);

                                    if(float_data == 0x00)
                                    {
                                        LeerTotales(TURNO);                                 
                                        write_eeprom(v6_2,0);
                                        write_eeprom(v5_2,0);
                                        write_eeprom(v4_2,0);
                                        write_eeprom(v3_2,0);
                                        write_eeprom(v2_2,0);
                                        write_eeprom(v1_2,0);
                                        write_eeprom(d6_2,0);
                                        write_eeprom(d5_2,0);
                                        write_eeprom(d4_2,0);
                                        write_eeprom(d3_2,0);
                                        write_eeprom(d2_2,0);
                                        write_eeprom(d1_2,0);   

                                        write_eeprom(estado2,0x06);
                                        Status_2(0X06);

                                        write_eeprom(pasos2,0x00);
                                        write_eeprom(SD_CONF,0x01);                                                                                        

                                        ButtonEmr(TECLADO_FIN,TURNO);
                                        data_recibo(7);
                                                                                
                                    }

                                    else
                                    {                                        
                                        write_eeprom(pasos2,0x0C);                                        

                                        cdin_real=float_data;
                                        write_eeprom(v6_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v5_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v4_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v3_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v2_2,(cdin_real%10));

                                        cdin_real = cdin_real/10;
                                        write_eeprom(v1_2,(cdin_real%10));

                                        sd_volumen(TURNO);
                                        
                                        ppu = ((read_eeprom(ppu1_venta2)*1000) + (read_eeprom(ppu2_venta2)*100) + (read_eeprom(ppu3_venta2)*10) + (read_eeprom(ppu4_venta2)*1));

                                        if(read_eeprom(PV) == 0)
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (mult(read_eeprom(PD)))));
                                        }
                                        else
                                        {
                                            dinero = ((float_data * ppu ) / ( (mult(read_eeprom(PV)) * mult(read_eeprom(pppu))) /  (10000)));

                                            for(i=0; i<(4-read_eeprom(PD)); i++)
                                            {
                                                cdin_real = dinero % 10;
                                                dinero = dinero / 10; 
            
                                                if(cdin_real > 4) dinero++; 
                                            }
                                        }

                                        cdin_real=dinero;
                                        write_eeprom(d6_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d5_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d4_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d3_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d2_2,(cdin_real%10));
                                        
                                        cdin_real = cdin_real/10;
                                        write_eeprom(d1_2,(cdin_real%10));
                                        
                                        sd_dinero(TURNO);

                                        float_data = 0;
                                        total_dinero = (read_eeprom(td2_1) * 10000000) + (read_eeprom(td2_2) * 1000000) + (read_eeprom(td2_3) * 100000) + (read_eeprom(td2_4) * 10000) + (read_eeprom(td2_5) * 1000) + (read_eeprom(td2_6) * 100) + (read_eeprom(td2_7) * 10) + (read_eeprom(td2_8) * 1);

                                        total_dinero = total_dinero + dinero;
                                        write_eeprom(td2_8,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_7,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_6,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_5,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_4,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_3,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_2,(total_dinero%10));
                                        
                                        total_dinero = total_dinero/10;
                                        write_eeprom(td2_1,(total_dinero%10));
                                        
                                        sd_Tdinero(TURNO);                                    
                                    }
                                }

                                else
                                {
                                    
                                    write_eeprom(pasos2,0x0B);
                                }
                            }       

                            //TOTAL DE VOLUMEN   
                            else if (read_eeprom(pasos2) == 0X0C)
                            { 
                                
                                LeerTotales(TURNO);                                                                

                                if(read_eeprom(consola) == 0x01)
                                {
                                    Status_2(0x06);
                                } 

                                write_eeprom(SD_CONF,0x01);
                                write_eeprom(SD_TRANS2,0x01);
                                
                            
                                if(read_eeprom(consola) == 1)
                                {
                                    write_eeprom(estado2,0X06);
                                }

                                else 
                                {
                                    write_eeprom(estado2,0X0B);
                                }

                                write_eeprom(pasos2,0x00);
                            }  
                        }
                    
                        TURNO=0X01;
                    }
                }
            }

            else 
            {
                LCDPrintRow(0,"   CONSOLE 03   ");
                LCDPrintRow2( 0,"    DISABLED    ");
                chThdSleepMilliseconds(10);
            }
        
        break;

        case 0X01://POSICION LLENADO_1

            write_eeprom(lado1,0x00);
            write_eeprom(lado2,0x00);
            ACTU=0X01;

            LCDPrintRow( 0,"   ADDRESS 1    ");

            if((ACTU == 0X00 || ACTU == 0X01) && read_eeprom(estado1) == 0X06 && (read_eeprom(estado2) == 0X06 || read_eeprom(turno_aux) == 1))
            {
                ACTU=0X01;


                if(palReadPad(GPIOB,IN_5) == 0)
                {

                    if(palReadPad(GPIOB,BUT_1) == 1) sel_pos=1;
                    else if (palReadPad(GPIOE,BUT_2) == 1) sel_pos=2;


                    if (sel_pos == 0)
                    {
                        POS1_A = read_eeprom(pos1);
                    }

                    else if (sel_pos == 1)
                    {

                        if ( (!palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6))) 
                        {
                            POS1_A=0X01;

                        }

                        else if ( (palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 1) ) 
                        {
                            POS1_A=0X02;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X03;
                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X04;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X05;
                        }  

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X06;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X07;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X08;
                        }
                    }

                    else if (sel_pos == 2)
                    {

                        if ( (palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                        {
                            POS1_A=0X09;

                        }

                        else if ( (palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 1) ) 
                        {
                            POS1_A=0X0A;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X0B;
                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X0C;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X0D;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {  
                            POS1_A=0X0E;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X0F;
                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS1_A=0X00;
                        }
                    }

                    if(POS1_A == read_eeprom(pos2))  
                    {
                        if(POS1_A == 0X0F) POS1_A=0X00;
                        else POS1_A++;

                    }

                    
                    lcd_out [0]=' ';
                    lcd_out [1]=' ';
                    lcd_out [2]=' ';
                    lcd_out [3]=' ';
                    lcd_out [4]=' ';
                    lcd_out [5]=' ';
                    lcd_out [6]=' ';
                    lcd_out [7]='0'; 
                    lcd_out [8]=ascii(POS1_A);
                    lcd_out [9]=' ';
                    lcd_out [10]=' ';
                    lcd_out [11]=' ';
                    lcd_out [12]=' ';
                    lcd_out [13]=' ';
                    lcd_out [14]=' ';
                    lcd_out [15]=' ';
                    
                    LCDPrintRow2( 0,lcd_out);
                }

                else if (palReadPad(GPIOB,IN_5) == 1 && read_eeprom(pos1) != POS1_A)
                {
                    write_eeprom(pos1,POS1_A);
                    
                    Status_1(0x06);
                   
                    write_eeprom(SD_CONF,0x01);

                    
                    LCDPrintRow2( 0,"  SAVE ADDRESS  ");

                    chThdSleepMilliseconds(500);
                    
                    sel_pos=0;
                }   

                else if (palReadPad(GPIOB,IN_5) == 1 &&  read_eeprom(pos1) ==  POS1_A)
                {

                    LCDPrintRow2( 0,"   ADDRESS OK   ");

                    chThdSleepMilliseconds(500);
                }
            }

            else 
            {
                LCDPrintRow2( 0,"    DISABLED    ");
            }

            chThdSleepMilliseconds(200);

        break;

        case 0X02://POSICION LLENADO_2

            write_eeprom(lado1,0x00);
            write_eeprom(lado2,0x00);
            ACTU=0X01;

            LCDPrintRow( 0,"   ADDRESS 2    ");

            if((ACTU == 0X00 || ACTU == 0X01) && read_eeprom(estado1) == 0X06 && (read_eeprom(estado2) == 0X06 || read_eeprom(turno_aux) == 1))
            {

                ACTU=0X01;

                if(palReadPad(GPIOB,IN_5) == 0)
                {

                    if(palReadPad(GPIOB,BUT_1) == 1) sel_pos=1;
                    else if (palReadPad(GPIOE,BUT_2) == 1) sel_pos=2;

                    if(sel_pos == 0)
                    {
                        POS2_A = read_eeprom(pos2);
                    }

                    else if (sel_pos==1)
                    {

                        if ( (palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                        {
                            POS2_A=0X01;

                        }

                        else if ( (palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 1) ) 
                        {
                            POS2_A=0X02;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X03;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X04;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X05;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X06;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X07;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X08;

                        }

                    }

                    else if (sel_pos == 2)
                    {

                        if ( (palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                        {
                            POS2_A=0X09;

                        }

                        else if ( (palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 1) ) 
                        {
                            POS2_A=0X0A;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X0B;

                        }

                        else if ( (!palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X0C;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X0D;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (!palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X0E;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (!palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X0F;

                        }

                        else if ( (palReadPad(GPIOD,IN_8)) && (palReadPad(GPIOC,IN_7)) && (palReadPad(GPIOC,IN_6)) ) 
                        {
                            POS2_A=0X00;

                        }

                    }

                    if(POS2_A == read_eeprom(pos1))  
                    {
                        if(POS2_A == 0X0F) POS2_A=0X00;
                        
                        else POS2_A++;

                    }

                    

                    lcd_out [0]=' ';
                    lcd_out [1]=' ';
                    lcd_out [2]=' ';
                    lcd_out [3]=' ';
                    lcd_out [4]=' ';
                    lcd_out [5]=' ';
                    lcd_out [6]=' ';
                    lcd_out [7]='0'; 
                    lcd_out [8]=ascii(POS2_A);
                    lcd_out [9]=' ';
                    lcd_out [10]=' ';
                    lcd_out [11]=' ';
                    lcd_out [12]=' ';
                    lcd_out [13]=' ';
                    lcd_out [14]=' ';
                    lcd_out [15]=' ';
                    
                    LCDPrintRow2( 0,lcd_out);

                }

                else if (palReadPad(GPIOB,IN_5) == 1 && read_eeprom(pos2) != POS2_A)
                {
                    write_eeprom(pos2,POS2_A);
                    
                    Status_2(0x06);
                   
                    write_eeprom(SD_CONF,0x01);
                    
                    LCDPrintRow2( 0,"  SAVE ADDRESS  ");

                    chThdSleepMilliseconds(500);
                    
                    sel_pos=0;

                }

                else if (palReadPad(GPIOB,IN_5) == 1 && read_eeprom(pos2) ==  POS2_A)
                {

                    LCDPrintRow2( 0,"   ADDRESS OK   ");

                    write_eeprom(pos2,POS2_A);
                    chThdSleepMilliseconds(500);
                    sel_pos=0;

                }
            }

            else 
            {
                LCDPrintRow2( 0,"    DISABLED    ");
            }
            
            chThdSleepMilliseconds(200);

        break;

        case 0X03://CONSOLA

            write_eeprom(lado1,0x00);
            write_eeprom(lado2,0x00);
            ACTU=0X01;

            LCDPrintRow( 0,"    CONSOLE     ");

            if((ACTU == 0X00 || ACTU == 0X01) && read_eeprom(estado1) == 0X06 && (read_eeprom(estado2) == 0X06 || read_eeprom(turno_aux) == 1))
            {
                ACTU=0X01;

                if(palReadPad(GPIOB,IN_5) == 0)
                {

                    if ( (palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==0) && (palReadPad(GPIOC,IN_6) == 1 )) 
                    {
                        cons_emr3=1; //SIN LAZO DE CORRIENTE
                        lcd_out [8]='1';
                    }

                    else if ( (palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==1) && (palReadPad(GPIOC,IN_6) == 0 )) 
                    {
                        cons_emr3=2; //CONSOLA
                        lcd_out [8]='2';
                    }

                    else if ((palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==1) && (palReadPad(GPIOC,IN_6) == 1 )) 
                    {
                        cons_emr3=3; //FUERA TOTAL DE CONSOLA
                        lcd_out [8]='3';
                    }

                    else if ((palReadPad(GPIOD,IN_8)==1) && (palReadPad(GPIOC,IN_7)==0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                    {
                        cons_emr3=4; //FUERA TOTAL DE CONSOLA
                        lcd_out [8]='4';
                    }

                    else
                    {
                        lcd_out [8]= ascii(read_eeprom(consola));
                    }

                    lcd_out [0]=' ';
                    lcd_out [1]=' ';
                    lcd_out [2]=' ';
                    lcd_out [3]=' ';
                    lcd_out [4]=' ';
                    lcd_out [5]=' ';
                    lcd_out [6]=' ';
                    lcd_out [7]='0';
                    lcd_out [9]=' ';
                    lcd_out [10]=' ';
                    lcd_out [11]=' ';
                    lcd_out [12]=' ';
                    lcd_out [13]=' ';
                    lcd_out [14]=' ';
                    lcd_out [15]=' ';
                    
                    LCDPrintRow2(0,lcd_out);
                }

                else if (palReadPad(GPIOB,IN_5) == 1 && read_eeprom(consola) != cons_emr3)
                {

                    
                    
                    LCDPrintRow2( 0,"  SAVE CONSOLE  ");

                    write_eeprom(consola,cons_emr3);
                    Status_1(0x06);

                    if(cons_emr3 == 3)
                    {
                       
                        //7E01FF442500977E
                        envio[0]=0X7E;
                        envio[1]=0X01;
                        envio[2]=0XFF;
                        envio[3]=0X44;
                        envio[4]=0X25;
                        envio[5]=0X00;
                        envio[6]=0X97;
                        envio[7]=0X7E;

                        uartStartSend(&UARTD1, 8, envio);
                        chThdSleepMilliseconds(100);
                        uartStopSend(&UARTD1);

                        chThdSleepMilliseconds(500);

                        //7E01FF442500977E
                        envio[0]=0X7E;
                        envio[1]=0X02;
                        envio[2]=0XFF;
                        envio[3]=0X44;
                        envio[4]=0X25;
                        envio[5]=0X00;
                        envio[6]=0X96;
                        envio[7]=0X7E;

                        uartStartSend(&UARTD1, 8, envio);
                        chThdSleepMilliseconds(100);
                        uartStopSend(&UARTD1);
                        chThdSleepMilliseconds(500);

                    }

                    else if  (cons_emr3 == 1 || cons_emr3 == 2 || cons_emr3 == 4)
                    {

                        
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

                        chThdSleepMilliseconds(500);

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

                        chThdSleepMilliseconds(500);
                    }

                    write_eeprom(SD_CONF,0x01);
                }

                else if (palReadPad(GPIOB,IN_5) == 1 && read_eeprom(consola) == cons_emr3)
                {
                    LCDPrintRow2( 0,"   CONSOLE OK   ");
                }
            }

            else 
            {
                LCDPrintRow2( 0,"    DISABLED    ");
            }

            chThdSleepMilliseconds(200);

        break;

        case 0X04://PPU

            write_eeprom(lado1,0x00);
            write_eeprom(lado2,0x00);
            ACTU=0X01;

            LCDPrintRow( 0,"     PRICE      ");

            if(read_eeprom(consola) == 0X01 && read_eeprom(estado1) == 0X06 && (read_eeprom(estado2) == 0X06 || read_eeprom(turno_aux) == 1))
            {

                if ((palReadPad(GPIOD,IN_8) == 0) && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                {

                    if(palReadPad(GPIOB,BUT_1) == 1)
                    {
                        if (read_eeprom(ppu4_level1) < 0X09)
                        {
                            write_eeprom(ppu4_level1,read_eeprom(ppu4_level1) + 1);  
                        } 
                    }

                    else if (palReadPad(GPIOE,BUT_2) == 1)
                    {
                        if (read_eeprom(ppu4_level1) > 0X00) 
                        {
                            write_eeprom(ppu4_level1,read_eeprom(ppu4_level1) - 1); 
                        }
                    }
                }

                else if ( palReadPad(GPIOD,IN_8)==0 && (palReadPad(GPIOC,IN_7) == 0) && (palReadPad(GPIOC,IN_6) == 1 )) 
                {

                    if(palReadPad(GPIOB,BUT_1) == 1)
                    {
                        if (read_eeprom(ppu3_level1) < 0X09)
                        {
                            write_eeprom(ppu3_level1,read_eeprom(ppu3_level1) + 1); 
                        } 
                    }

                    else if (palReadPad(GPIOE,BUT_2) == 1)
                    {
                        if (read_eeprom(ppu3_level1) > 0X00) 
                        {
                            write_eeprom(ppu3_level1,read_eeprom(ppu3_level1) - 1);
                        }
                    }

                }

                else if ((palReadPad(GPIOD,IN_8)==0) && (palReadPad(GPIOC,IN_7)==1) && (palReadPad(GPIOC,IN_6) == 0)) 
                {

                    if(palReadPad(GPIOB,BUT_1) == 1)
                    {
                        if (read_eeprom(ppu2_level1) < 0X09)
                        {
                            write_eeprom(ppu2_level1,read_eeprom(ppu2_level1) + 1);  
                        } 
                    }

                    else if (palReadPad(GPIOE,BUT_2) == 1)
                    {
                        if (read_eeprom(ppu2_level1) > 0X00) 
                        {
                            write_eeprom(ppu2_level1,read_eeprom(ppu2_level1) - 1);
                        }
                    }
                }

                else if ((palReadPad(GPIOD,IN_8)==1) && (palReadPad(GPIOC,IN_7)==0) && (palReadPad(GPIOC,IN_6) == 0 )) 
                {

                    if(palReadPad(GPIOB,BUT_1) == 1)
                    {
                        if (read_eeprom(ppu1_level1) < 0X09)
                        {
                            write_eeprom(ppu1_level1,read_eeprom(ppu1_level1) + 1);  
                        } 
                    }

                    else if (palReadPad(GPIOE,BUT_2) == 1)
                    {
                        if (read_eeprom(ppu1_level1) > 0X00) 
                        {
                            write_eeprom(ppu1_level1,read_eeprom(ppu1_level1) - 1);
                        }
                    }
                }

                

                lcd_out [0]=' ';
                lcd_out [1]=' ';
                lcd_out [2]=' ';
                lcd_out [3]=' ';
                lcd_out [4]=' ';
                lcd_out [5]= ascii(read_eeprom(ppu1_level1));

                if(read_eeprom(pppu) == 0X03)
                {

                    lcd_out [6] = '.';
                    lcd_out [7] = ascii(read_eeprom(ppu2_level1));
                    lcd_out [8] = ascii(read_eeprom(ppu3_level1));
                    lcd_out [9] = ascii(read_eeprom(ppu4_level1));
                }

                else if(read_eeprom(pppu) == 0X02)
                {

                    lcd_out [6] = ascii(read_eeprom(ppu2_level1));
                    lcd_out [7] = '.';
                    lcd_out [8] = ascii(read_eeprom(ppu3_level1));
                    lcd_out [9] = ascii(read_eeprom(ppu4_level1));
                }

                else if(read_eeprom(pppu) == 0X01)
                {

                    lcd_out [6] = ascii(read_eeprom(ppu2_level1));
                    lcd_out [7] = ascii(read_eeprom(ppu3_level1));
                    lcd_out [8] = '.';
                    lcd_out [9] = ascii(read_eeprom(ppu4_level1));
                }

                else if(read_eeprom(pppu) == 0X00)
                {

                    lcd_out [6] = ascii(read_eeprom(ppu2_level1));
                    lcd_out [7] = ascii(read_eeprom(ppu3_level1));
                    lcd_out [8] = ascii(read_eeprom(ppu4_level1));
                    lcd_out [9] = '.';
                }


                lcd_out [10]=' ';
                lcd_out [11]=' ';
                lcd_out [12]=' ';
                lcd_out [13]=' ';
                lcd_out [14]=' ';
                lcd_out [15]=' ';

                LCDPrintRow2(0,lcd_out);
            }
            
            else 
            {
                LCDPrintRow2(0,"    DISABLED    ");
            }
             
            chThdSleepMilliseconds(200);

        break;

        case 0X05://LOG

            write_eeprom(lado1,0x00);
            write_eeprom(lado2,0x00);
            ACTU=0X01;

            LCDPrintRow( 0,"      LOG       ");

                           

                if(palReadPad(GPIOB,IN_5) == 1 && read_eeprom(log) == 0)
                {
                     
                    //borrar_log();
                    write_eeprom(log,0x01);

                    //fr2 = f_unlink("logwire.txt");


                    LCDPrintRow2( 0,"       ON       ");
                    chThdSleepMilliseconds(300);

                }

                else if( read_eeprom(log) == 0x00)
                {

                    LCDPrintRow2( 0,"      OFF       ");
                    chThdSleepMilliseconds(300);    
                }

                else if( read_eeprom(log) == 1)
                {
                     
                    LCDPrintRow2( 0,"       ON       ");
                    chThdSleepMilliseconds(300);

                }



                chThdSleepMilliseconds(100);
            

        break;

        default:

            LCDPrintRow( 0,"    INSEPET     ");
            LCDPrintRow2( 0,"    DISABLED    ");
            chThdSleepMilliseconds(200);

        break;
    }
   
   return;    
}