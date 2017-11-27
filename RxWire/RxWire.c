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



void Imprimir(uint8_t *envio_wire,uint8_t P)
{
    uint8_t i;

    chThdSleepMilliseconds(5);    

    for(i=0;i<P;i++)
    {
        sdPut(&SD2,envio_wire[i]);
        //chThdSleepMilliseconds(2);
    }

    return;
}

uint8_t verif_wire(uint8_t vec[], uint8_t tamano,uint8_t tipo)
{
    
    uint16_t valor_sum;
    uint8_t i;
    uint8_t comp;
    
    valor_sum = 0;
    
    for(i=0; i<(tamano-2); i++)
    {
        valor_sum = valor_sum + (vec[i] & 0X0F); 
    }

    valor_sum = valor_sum & 0xFF;

    valor_sum = (0xFF + 0x01) - valor_sum;
    valor_sum = valor_sum & 0X0F;


    if(tipo == 0x01)
    {
        comp = vec[tamano-2] & 0X0F;
        if(valor_sum == comp) return 1;
        else return 0;  
    }

    else if(tipo == 0x00)
    {
        return valor_sum;
    }
    
    return;
}

void espe_wire(uint8_t vec[], uint8_t tamano)
{
    uint16_t valor_sum;
    uint8_t valor_aux;
    uint16_t valor_enviar;

    uint8_t i = 0x01;
    uint8_t comp;
    uint8_t aux_ch;
    
    valor_sum = 0;
    
    for(i=1; i<tamano; i++)
    {

        if(vec[i] >= 0xC0)
        {
            aux_ch = (vec[i] & 0X0F);
            aux_ch = ((aux_ch + 0x09) << 4);
        }
        else
        {
           aux_ch = ((vec[i] & 0X0F) << 4); 
        }
        
        i++;

        if(vec[i] >= 0xC0)
        {
            valor_sum = valor_sum + (aux_ch | ((vec[i] & 0X0F) + 0x09)); 
        }
        else
        {
            valor_sum = valor_sum + (aux_ch | (vec[i] & 0X0F)); 
           
        }

    }
    
    //SUMA HEX, MOD 100, COMPLEMENTO A 2
    
    valor_sum = valor_sum & 0XFF;
    valor_sum = (0xFF + 0x01) - valor_sum;
    
    valor_aux = ((valor_sum >> 4) & 0X0F);

    if(valor_aux >= 0x0A)
    {
       chec1 = (0XC0 | (valor_aux - 0X09));             
    }
    else chec1 = (0XB0 | valor_aux); 

    valor_aux = (valor_sum & 0X0F);

    if(valor_aux >= 0x0A)
    {
        chec2 =  (0XC0 | (valor_aux - 0X09));             
    }
    else chec2 = (0XB0 | valor_aux);

    
    return;
}

void rx_especial(uint8_t A1, uint8_t A2, uint8_t A3, uint8_t pos)
{
    
    uint8_t envio_wire[50];
    uint8_t P;
    uint8_t DATO;
    

    if(pos == 0x01)
    {

        if (read_eeprom(pos1) >= 0X0A) DATO=0XC0 | (read_eeprom(pos1) - 0X09);
        else DATO=0XB0 | read_eeprom(pos1);
    }
    else if (pos == 0x02)
    {

        if (read_eeprom(pos2) >= 0X0A) DATO=0XC0 | (read_eeprom(pos2) - 0X09);
        else DATO=0XB0 | read_eeprom(pos2);
    }
    else return;

    if(A1 == 0XEE && A2 == 0XE0 && A3 == 0XE0) //puntos
    {

        //BA B0 B7 C1 B0 B0 C5 B0 B0 B1 B0 B0 B1 B0 B3 B0 B4 B0 B4 B0 B1 B0 B0 B2 C5 8D 8A

        envio_wire[0]=0XBA;
        envio_wire[1]=0XB0;
        envio_wire[2]=0XB7;
        envio_wire[3]=DATO;
        envio_wire[4]=0XB0;
        envio_wire[5]=0XB0;
        envio_wire[6]=0XC5;
        
        envio_wire[7]=0XB0;
        envio_wire[8]=0XB0;
        //CAMBIOS
        
        //TIPO DE UNIDAD
        envio_wire[9]=0XB1;
        envio_wire[10]=0XB0;
        //CONVERSION
        envio_wire[11]=0XB0;
        envio_wire[12]=0XB1;
        //PUNTO DECIMAL DINERO
        envio_wire[13]=0XB0;
        envio_wire[14]=0XB0 | (read_eeprom(PD) + 0X01);
        //PUNTO DECIMAL VOLUMEN
        envio_wire[15]=0XB0;
        envio_wire[16]=0XB0 | (read_eeprom(PV) + 0X01);
        //PUNTO PPU
        envio_wire[17]=0XB0;
        envio_wire[18]=0XB0 | (read_eeprom(pppu) + 0X01);
        //DISPLAY
        if(read_eeprom(consola) == 0x04)
        {
            envio_wire[19]=0XB0;
            envio_wire[20]=0XB0;
        }

        else
        {
            envio_wire[19]=0XB0;
            envio_wire[20]=0XB1;
        }
        
        //ARRANQUE
        envio_wire[21]=0XB0;
        envio_wire[22]=0XB0;
        
        //chesuk
        chThdSleepMilliseconds(1);   
        espe_wire(envio_wire,23);
        envio_wire[23] = chec1;
        envio_wire[24] = chec2;
         

        envio_wire[25]=0X8D;
        envio_wire[26]=0X8A;

        P=27;

        Imprimir(&envio_wire,P);

        return;
    }

    else if (A1 == 0XE1 && A2 == 0XE0 && A3 == 0XE0) // version
    {

        envio_wire[0]=0XBA;

        //B0 C2
        envio_wire[1]=0XB0;
        envio_wire[2]=0XC2;

        //C1 B0
        envio_wire[3]=DATO;
        envio_wire[4]=0XB0;

        //B0 B1
        envio_wire[5]=0XB0;
        envio_wire[6]=0XB1;

        //B0 B0
        envio_wire[7]=0XB0;
        envio_wire[8]=0XB0;

        //B7 B0
        envio_wire[9]=0XB7;
        envio_wire[10]=0XB0;

        //B9 B9
        envio_wire[11]=0XB9;
        envio_wire[12]=0XB9;

        //B1 B2
        envio_wire[13]=0XB1;
        envio_wire[14]=0XB2;

        //B0 B5
        envio_wire[15]=0XB0;
        envio_wire[16]=0XB5;

        //B0 B8
        envio_wire[17]=0XB0;
        envio_wire[18]=0XB8;

        //B0 B0
        envio_wire[19]=0XB0;
        envio_wire[20]=0XB0;

        //B0 B0
        envio_wire[21]=0XB0;
        envio_wire[22]=0XB0;

        //B4 B2
        envio_wire[23]=0XB4;
        envio_wire[24]=0XB2;

        //B4 B6
        envio_wire[25]=0XB4;
        envio_wire[26]=0XB6;

        //B0 B0
        envio_wire[27]=0XB0;
        envio_wire[28]=0XB0;

        //B0 B7
        envio_wire[29]=0XB0;
        envio_wire[30]=0XB7;

        chThdSleepMilliseconds(1);
        espe_wire(envio_wire,31);
        envio_wire[31] = chec1;
        envio_wire[32] = chec2;

        
        envio_wire[33]=0X8D;
        envio_wire[34]=0X8A;

        P=35;
  

        Imprimir(&envio_wire,P);

        

        return;

    }

    else if (A1 == 0XE0 && A2 == 0XE1 && A3 == 0XE0)// manija arriba
    {

        
        envio_wire[0]=0XBA;

        envio_wire[1]=0XB0;
        envio_wire[2]=0XB3;

        envio_wire[3]=DATO;
        envio_wire[4]=0XB0;

        envio_wire[5]=0XB1;
        envio_wire[6]=0XB0;

        envio_wire[7]=0XB0;
        envio_wire[8]=0XB0;

        envio_wire[9]=0XB1;
        envio_wire[10]=0XB1;

        envio_wire[11]=0XB1;
        envio_wire[12]=0XB1;

        envio_wire[13]=0XB1;
        envio_wire[14]=0XB1;

        espe_wire(envio_wire,15);
        envio_wire[15] = chec1;
        envio_wire[16] = chec2;


        envio_wire[17]=0X8D;
        envio_wire[18]=0X8A;

        P=19;

        
        Imprimir(&envio_wire,P);

        return;
    }

    else 
    {
       
        envio_wire[0]=0XBA;

        envio_wire[1]=0XB0;
        envio_wire[2]=0XB0;

        envio_wire[3]=DATO;
        envio_wire[4] = 0XB0 | (A3 & 0x0F) ;

        envio_wire[5] = 0XB0 | (A2 & 0x0F) ;
        envio_wire[6] = 0XB0 | (A1 & 0x0F) ;

        envio_wire[7]=0XB0;
        envio_wire[8]=0XB0;

        espe_wire(envio_wire,9);
        envio_wire[9] = chec1;
        envio_wire[10] = chec2;

        
        //envio_wire[9]=0XC3;
        // envio_wire[10]=0XC1;

                      
        envio_wire[11]=0X8D;
        envio_wire[12]=0X8A;

        P=13;

        Imprimir(&envio_wire,P);

    }
    
    return;
}

void rx_reporte(uint8_t tipo, uint8_t pos)
{
    
    uint8_t envio_wire[50];
    uint8_t recibo_wire[20];
    uint8_t P;

    if(pos == 0x01)
    {
        if (tipo == 0X01)
        {

            envio_wire[0]=0XFF;
            envio_wire[1]=0XF3;
            envio_wire[2]=0XF8;
            envio_wire[3]=0XEB;
            
            if(read_eeprom(pos1) == 0) 
            {
                envio_wire[4]=0XEF; 
            }
            else 
            {
                envio_wire[4]=0XE0 | (read_eeprom(pos1)-1);
            }
            
            //envio_wire[4]=0XE0;
            
            envio_wire[5]=0XE0;
            envio_wire[6]=0XE0;
            envio_wire[7]=0XE0;
            envio_wire[8]=0XF6;
            envio_wire[9]=0XE0;
            envio_wire[10] = read_eeprom(level_1);
            envio_wire[11] = 0XF7;
            
            //PPU1_WIRE
            envio_wire[12]=0XE0 | read_eeprom(ppu4_venta1);
            envio_wire[13]=0XE0 | read_eeprom(ppu3_venta1);
            envio_wire[14]=0XE0 | read_eeprom(ppu2_venta1);
            envio_wire[15]=0XE0 | read_eeprom(ppu1_venta1);
            
            //VOLUMEN1
            envio_wire[16]=0XF9;
            envio_wire[17]=0XE0 | read_eeprom(v6_1);
            envio_wire[18]=0XE0 | read_eeprom(v5_1);
            envio_wire[19]=0XE0 | read_eeprom(v4_1);
            envio_wire[20]=0XE0 | read_eeprom(v3_1);
            envio_wire[21]=0XE0 | read_eeprom(v2_1);
            envio_wire[22]=0XE0 | read_eeprom(v1_1);
            
            //DINERO1
            envio_wire[23]=0XFA;

            if(read_eeprom(consola) == 0x04)
            {
                envio_wire[24]=0XE0;
                envio_wire[25]=0XE0 | read_eeprom(d6_1);
                envio_wire[26]=0XE0 | read_eeprom(d5_1);
                envio_wire[27]=0XE0 | read_eeprom(d4_1);
                envio_wire[28]=0XE0 | read_eeprom(d3_1);
                envio_wire[29]=0XE0 | read_eeprom(d2_1);

            }
            else 
            {
                envio_wire[24]=0XE0 | read_eeprom(d6_1);
                envio_wire[25]=0XE0 | read_eeprom(d5_1);
                envio_wire[26]=0XE0 | read_eeprom(d4_1);
                envio_wire[27]=0XE0 | read_eeprom(d3_1);
                envio_wire[28]=0XE0 | read_eeprom(d2_1);
                envio_wire[29]=0XE0 | read_eeprom(d1_1);
            }
            

            envio_wire[30] = 0XFB;
            envio_wire[31] = (0XE0 | verif_wire(envio_wire,0X21,0x00));
            envio_wire[32] = 0XF0;
            P=33;

            

            Imprimir(&envio_wire,P);

            wire_sale1(&envio_wire,P,1);

            // if(read_eeprom(log) == 0x01)
            // {
            //     recibo_wire[0]=(0X40 | read_eeprom(pos1));
            //     log_Wire(envio_wire,33,recibo_wire,1);
            // }
        }

        //ENVIO DE LOS TOTALES DE LA CABEZA 1
        else if (tipo == 0X02)
        {
            
            envio_wire[0]=0XFF;
            envio_wire[1]=0XF6;
            envio_wire[2]=0XE0;
            envio_wire[3]=0XF9;
            //TOTAL VOLUME
            envio_wire[4]=0XE0 | read_eeprom(tv1_8);
            envio_wire[5]=0XE0 | read_eeprom(tv1_7);
            envio_wire[6]=0XE0 | read_eeprom(tv1_6);
            envio_wire[7]=0XE0 | read_eeprom(tv1_5);
            envio_wire[8]=0XE0 | read_eeprom(tv1_4);
            envio_wire[9]=0XE0 | read_eeprom(tv1_3);
            envio_wire[10]=0XE0 | read_eeprom(tv1_2);
            envio_wire[11]=0XE0 | read_eeprom(tv1_1);
            envio_wire[12]=0XFA;
            //MONAY
            envio_wire[13]=0XE0 | read_eeprom(td1_8);
            envio_wire[14]=0XE0 | read_eeprom(td1_7);
            envio_wire[15]=0XE0 | read_eeprom(td1_6);
            envio_wire[16]=0XE0 | read_eeprom(td1_5);
            envio_wire[17]=0XE0 | read_eeprom(td1_4);
            envio_wire[18]=0XE0 | read_eeprom(td1_3);
            envio_wire[19]=0XE0 | read_eeprom(td1_2);
            envio_wire[20]=0XE0 | read_eeprom(td1_1);
            //PPU1
            envio_wire[21]=0XF4;
            envio_wire[22]=0XE0 | read_eeprom(ppu4_level1);
            envio_wire[23]=0XE0 | read_eeprom(ppu3_level1);
            envio_wire[24]=0XE0 | read_eeprom(ppu2_level1);
            envio_wire[25]=0XE0 | read_eeprom(ppu1_level1);
            //LEVE 2
            envio_wire[26]=0XF5;
            envio_wire[27]=0XE0 | read_eeprom(ppu4_level2);
            envio_wire[28]=0XE0 | read_eeprom(ppu3_level2);
            envio_wire[29]=0XE0 | read_eeprom(ppu2_level2);
            envio_wire[30]=0XE0 | read_eeprom(ppu1_level2);
            //
            envio_wire[31] = 0XFB;
            
            envio_wire[32] = (0XE0 | verif_wire(envio_wire,0X22,0x00));
            
            envio_wire[33] = 0XF0;

            P=34;

            wire_sale1(&envio_wire,P,2);

            write_eeprom(SD_wire_sale1,0x01);

            Imprimir(&envio_wire,P);

            
              /*

                 envio_wire[31]=0XF6;
                 envio_wire[32]=0XE1;
                 envio_wire[33]=0XF9;
                 //VOLUMEN2
                 envio_wire[34]=0XE0;
                 envio_wire[35]=0XE0 | dato_emr3[tv1_7];
                 envio_wire[36]=0XE0 | dato_emr3[tv1_6];
                 envio_wire[37]=0XE0 | dato_emr3[tv1_5];
                 envio_wire[38]=0XE0 | dato_emr3[tv1_4];
                 envio_wire[39]=0XE0 | dato_emr3[tv1_3];
                 envio_wire[40]=0XE0 | dato_emr3[tv1_2];
                 envio_wire[41]=0XE0 | dato_emr3[tv1_1];
                 envio_wire[42]=0XFA;
                 //MONAY
                 envio_wire[43]=0XE0 | dato_emr3[td1_8];
                 envio_wire[44]=0XE0 | dato_emr3[td1_7];
                 envio_wire[45]=0XE0 | dato_emr3[td1_6];
                 envio_wire[46]=0XE0 | dato_emr3[td1_5];
                 envio_wire[47]=0XE0 | dato_emr3[td1_4];
                 envio_wire[48]=0XE0 | dato_emr3[td1_3];
                 envio_wire[49]=0XE0 | dato_emr3[td1_2];
                 envio_wire[50]=0XE0 | dato_emr3[td1_1];
                 //PPU1
                 envio_wire[51]=0XF4;
                 envio_wire[52]=0XE0 | dato_emr3[ppu4_level1];
                 envio_wire[53]=0XE0 | dato_emr3[ppu3_level1];
                 envio_wire[54]=0XE0 | dato_emr3[ppu2_level1];
                 envio_wire[55]=0XE0 | dato_emr3[ppu1_level1];
                 //LEVE 2
                 envio_wire[56]=0XF5;
                 envio_wire[57]=0XE0 | dato_emr3[ppu4_level2];
                 envio_wire[58]=0XE0 | dato_emr3[ppu3_level2];
                 envio_wire[59]=0XE0 | dato_emr3[ppu2_level2];
                 envio_wire[60]=0XE0 | dato_emr3[ppu1_level2];

                 envio_wire[61]=0XF6;
                 envio_wire[62]=0XE2;
                 envio_wire[63]=0XF9;
                 //VOLUMEN2
                 envio_wire[64]=0XE0;
                 envio_wire[65]=0XE0 | dato_emr3[tv1_7];
                 envio_wire[66]=0XE0 | dato_emr3[tv1_6];
                 envio_wire[67]=0XE0 | dato_emr3[tv1_5];
                 envio_wire[68]=0XE0 | dato_emr3[tv1_4];
                 envio_wire[69]=0XE0 | dato_emr3[tv1_3];
                 envio_wire[70]=0XE0 | dato_emr3[tv1_2];
                 envio_wire[71]=0XE0 | dato_emr3[tv1_1];
                 envio_wire[72]=0XFA;
                 //MONAY
                 envio_wire[73]=0XE0 | dato_emr3[td1_8];
                 envio_wire[74]=0XE0 | dato_emr3[td1_7];
                 envio_wire[75]=0XE0 | dato_emr3[td1_6];
                 envio_wire[76]=0XE0 | dato_emr3[td1_5];
                 envio_wire[77]=0XE0 | dato_emr3[td1_4];
                 envio_wire[78]=0XE0 | dato_emr3[td1_3];
                 envio_wire[79]=0XE0 | dato_emr3[td1_2];
                 envio_wire[80]=0XE0 | dato_emr3[td1_1];
                 //PPU
                 envio_wire[81]=0XF4;
                 envio_wire[82]=0XE0 | dato_emr3[ppu4_level1];
                 envio_wire[83]=0XE0 | dato_emr3[ppu3_level1];
                 envio_wire[84]=0XE0 | dato_emr3[ppu2_level1];
                 envio_wire[85]=0XE0 | dato_emr3[ppu1_level1];
                 //LEVE 28
                 envio_wire[86]=0XF5;
                 envio_wire[87]=0XE0 | dato_emr3[ppu4_level2];
                 envio_wire[88]=0XE0 | dato_emr3[ppu3_level2];
                 envio_wire[89]=0XE0 | dato_emr3[ppu2_level2];
                 envio_wire[90]=0XE0 | dato_emr3[ppu1_level2];

                 envio_wire[91]=0XFB;
                 DATO=0;

                 for(i=0;i<92;i++)
                 {
                    DATO=DATO+(envio_wire[i] & 0X0F);
                 }

                 CM2= DATO;
                 CM2=~CM2;
                 CM2++;
                 CM2=CM2 & 0X0F;
                 envio_wire[92]=0XE0+CM2;
                 envio_wire[93]=0XF0;

                 P=94;
                 Imprimir(envio_wire,P);
              */


            // if(read_eeprom(log) == 0x01)
            // {
            //     recibo_wire[0] = (0X50 |read_eeprom(pos1));

            //     log_Wire(envio_wire,P,recibo_wire,1);
            // }
        }
    }

    else if(pos == 0x02)
    {
        
        if (tipo == 0X01)
        {

            envio_wire[0]=0XFF;
            envio_wire[1]=0XF3;
            envio_wire[2]=0XF8;
            envio_wire[3]=0XEB;
            
            if(read_eeprom(pos2) == 0) 
            {
                envio_wire[4]=0XEF; 
            }
            else 
            {
                envio_wire[4]=0XE0 | (read_eeprom(pos2) - 1);
            }
            
            //envio_wire[4]=0XE0;
            
            envio_wire[5]=0XE0;
            envio_wire[6]=0XE0;
            envio_wire[7]=0XE0;
            envio_wire[8]=0XF6;
            envio_wire[9]=0XE0;
            envio_wire[10]=read_eeprom(level_2);
            envio_wire[11]=0XF7;
            
            //PPU1_WIRE
            envio_wire[12]=0XE0 | read_eeprom(ppu4_venta2);
            envio_wire[13]=0XE0 | read_eeprom(ppu3_venta2);
            envio_wire[14]=0XE0 | read_eeprom(ppu2_venta2);
            envio_wire[15]=0XE0 | read_eeprom(ppu1_venta2);
            
            //VOLUMEN1
            envio_wire[16]=0XF9;
            envio_wire[17]=0XE0 | read_eeprom(v6_2);
            envio_wire[18]=0XE0 | read_eeprom(v5_2);
            envio_wire[19]=0XE0 | read_eeprom(v4_2);
            envio_wire[20]=0XE0 | read_eeprom(v3_2);
            envio_wire[21]=0XE0 | read_eeprom(v2_2);
            envio_wire[22]=0XE0 | read_eeprom(v1_2);
            
            //DINERO1
            envio_wire[23]=0XFA;

            if(read_eeprom(consola) == 0x04)
            {
                envio_wire[24]=0XE0;
                envio_wire[25]=0XE0 | read_eeprom(d6_2);
                envio_wire[26]=0XE0 | read_eeprom(d5_2);
                envio_wire[27]=0XE0 | read_eeprom(d4_2);
                envio_wire[28]=0XE0 | read_eeprom(d3_2);
                envio_wire[29]=0XE0 | read_eeprom(d2_2);

            }
            else 
            {
                envio_wire[24]=0XE0 | read_eeprom(d6_2);
                envio_wire[25]=0XE0 | read_eeprom(d5_2);
                envio_wire[26]=0XE0 | read_eeprom(d4_2);
                envio_wire[27]=0XE0 | read_eeprom(d3_2);
                envio_wire[28]=0XE0 | read_eeprom(d2_2);
                envio_wire[29]=0XE0 | read_eeprom(d1_2);
            }

            envio_wire[30] = 0XFB;
            envio_wire[31] = (0XE0 | verif_wire(envio_wire,0X21,0x00));
            envio_wire[32] = 0XF0;
            P=33;

            wire_sale2(&envio_wire,P,1);

            Imprimir(&envio_wire,P);
            
        }

        //ENVIO DE LOS TOTALES DE LA CABEZA 1
        else if (tipo == 0X02)
        {
            envio_wire[0]=0XFF;
            envio_wire[1]=0XF6;
            envio_wire[2]=0XE0;
            envio_wire[3]=0XF9;
            //TOTAL VOLUME
            envio_wire[4]=0XE0 | read_eeprom(tv2_8);
            envio_wire[5]=0XE0 | read_eeprom(tv2_7);
            envio_wire[6]=0XE0 | read_eeprom(tv2_6);
            envio_wire[7]=0XE0 | read_eeprom(tv2_5);
            envio_wire[8]=0XE0 | read_eeprom(tv2_4);
            envio_wire[9]=0XE0 | read_eeprom(tv2_3);
            envio_wire[10]=0XE0 | read_eeprom(tv2_2);
            envio_wire[11]=0XE0 | read_eeprom(tv2_1);
            envio_wire[12]=0XFA;
            //MONAY
            envio_wire[13]=0XE0 | read_eeprom(td2_8);
            envio_wire[14]=0XE0 | read_eeprom(td2_7);
            envio_wire[15]=0XE0 | read_eeprom(td2_6);
            envio_wire[16]=0XE0 | read_eeprom(td2_5);
            envio_wire[17]=0XE0 | read_eeprom(td2_4);
            envio_wire[18]=0XE0 | read_eeprom(td2_3);
            envio_wire[19]=0XE0 | read_eeprom(td2_2);
            envio_wire[20]=0XE0 | read_eeprom(td2_1);
            //PPU1
            envio_wire[21]=0XF4;
            envio_wire[22]=0XE0 | read_eeprom(ppu4_level1);
            envio_wire[23]=0XE0 | read_eeprom(ppu3_level1);
            envio_wire[24]=0XE0 | read_eeprom(ppu2_level1);
            envio_wire[25]=0XE0 | read_eeprom(ppu1_level1);
            //LEVE 2
            envio_wire[26]=0XF5;
            envio_wire[27]=0XE0 | read_eeprom(ppu4_level2);
            envio_wire[28]=0XE0 | read_eeprom(ppu3_level2);
            envio_wire[29]=0XE0 | read_eeprom(ppu2_level2);
            envio_wire[30]=0XE0 | read_eeprom(ppu1_level2);
            //
            envio_wire[31] = 0XFB;
            
            envio_wire[32] = (0XE0 | verif_wire(envio_wire,34,0x00));
            
            envio_wire[33] = 0XF0;

            P=34;

            wire_sale2(&envio_wire,P,2);

            write_eeprom(SD_wire_sale2,0x01);

            Imprimir(&envio_wire,P);

            

              /*

                 envio_wire[31]=0XF6;
                 envio_wire[32]=0XE1;
                 envio_wire[33]=0XF9;
                 //VOLUMEN2
                 envio_wire[34]=0XE0;
                 envio_wire[35]=0XE0 | dato_emr3[tv1_7];
                 envio_wire[36]=0XE0 | dato_emr3[tv1_6];
                 envio_wire[37]=0XE0 | dato_emr3[tv1_5];
                 envio_wire[38]=0XE0 | dato_emr3[tv1_4];
                 envio_wire[39]=0XE0 | dato_emr3[tv1_3];
                 envio_wire[40]=0XE0 | dato_emr3[tv1_2];
                 envio_wire[41]=0XE0 | dato_emr3[tv1_1];
                 envio_wire[42]=0XFA;
                 //MONAY
                 envio_wire[43]=0XE0 | dato_emr3[td1_8];
                 envio_wire[44]=0XE0 | dato_emr3[td1_7];
                 envio_wire[45]=0XE0 | dato_emr3[td1_6];
                 envio_wire[46]=0XE0 | dato_emr3[td1_5];
                 envio_wire[47]=0XE0 | dato_emr3[td1_4];
                 envio_wire[48]=0XE0 | dato_emr3[td1_3];
                 envio_wire[49]=0XE0 | dato_emr3[td1_2];
                 envio_wire[50]=0XE0 | dato_emr3[td1_1];
                 //PPU1
                 envio_wire[51]=0XF4;
                 envio_wire[52]=0XE0 | dato_emr3[ppu4_level1];
                 envio_wire[53]=0XE0 | dato_emr3[ppu3_level1];
                 envio_wire[54]=0XE0 | dato_emr3[ppu2_level1];
                 envio_wire[55]=0XE0 | dato_emr3[ppu1_level1];
                 //LEVE 2
                 envio_wire[56]=0XF5;
                 envio_wire[57]=0XE0 | dato_emr3[ppu4_level2];
                 envio_wire[58]=0XE0 | dato_emr3[ppu3_level2];
                 envio_wire[59]=0XE0 | dato_emr3[ppu2_level2];
                 envio_wire[60]=0XE0 | dato_emr3[ppu1_level2];

                 envio_wire[61]=0XF6;
                 envio_wire[62]=0XE2;
                 envio_wire[63]=0XF9;
                 //VOLUMEN2
                 envio_wire[64]=0XE0;
                 envio_wire[65]=0XE0 | dato_emr3[tv1_7];
                 envio_wire[66]=0XE0 | dato_emr3[tv1_6];
                 envio_wire[67]=0XE0 | dato_emr3[tv1_5];
                 envio_wire[68]=0XE0 | dato_emr3[tv1_4];
                 envio_wire[69]=0XE0 | dato_emr3[tv1_3];
                 envio_wire[70]=0XE0 | dato_emr3[tv1_2];
                 envio_wire[71]=0XE0 | dato_emr3[tv1_1];
                 envio_wire[72]=0XFA;
                 //MONAY
                 envio_wire[73]=0XE0 | dato_emr3[td1_8];
                 envio_wire[74]=0XE0 | dato_emr3[td1_7];
                 envio_wire[75]=0XE0 | dato_emr3[td1_6];
                 envio_wire[76]=0XE0 | dato_emr3[td1_5];
                 envio_wire[77]=0XE0 | dato_emr3[td1_4];
                 envio_wire[78]=0XE0 | dato_emr3[td1_3];
                 envio_wire[79]=0XE0 | dato_emr3[td1_2];
                 envio_wire[80]=0XE0 | dato_emr3[td1_1];
                 //PPU
                 envio_wire[81]=0XF4;
                 envio_wire[82]=0XE0 | dato_emr3[ppu4_level1];
                 envio_wire[83]=0XE0 | dato_emr3[ppu3_level1];
                 envio_wire[84]=0XE0 | dato_emr3[ppu2_level1];
                 envio_wire[85]=0XE0 | dato_emr3[ppu1_level1];
                 //LEVE 28
                 envio_wire[86]=0XF5;
                 envio_wire[87]=0XE0 | dato_emr3[ppu4_level2];
                 envio_wire[88]=0XE0 | dato_emr3[ppu3_level2];
                 envio_wire[89]=0XE0 | dato_emr3[ppu2_level2];
                 envio_wire[90]=0XE0 | dato_emr3[ppu1_level2];

                 envio_wire[91]=0XFB;
                 DATO=0;

                 for(i=0;i<92;i++)
                 {
                    DATO=DATO+(envio_wire[i] & 0X0F);
                 }

                 CM2= DATO;
                 CM2=~CM2;
                 CM2++;
                 CM2=CM2 & 0X0F;
                 envio_wire[92]=0XE0+CM2;
                 envio_wire[93]=0XF0;

                 P=94;
                 Imprimir(envio_wire,P);
              */


            // if(read_eeprom(log) == 0x01)
            // {
            //     recibo_wire[0] = (0X50 |read_eeprom(pos2));

            //     log_Wire(envio_wire,P,recibo_wire,1);
            // }
        }
    }
}

void rx_ppu(uint8_t niv, uint8_t A1, uint8_t A2, uint8_t A3, uint8_t A4)
{
    
    if(niv == 0XF4)
    {
        write_eeprom(ppu4_level1,(A4 & 0X0F));
        write_eeprom(ppu3_level1,(A3 & 0X0F));
        write_eeprom(ppu2_level1,(A2 & 0X0F));
        write_eeprom(ppu1_level1,(A1 & 0X0F));
        sd_ppu(0x01);
    }

    else if(niv == 0XF5)
    {
        write_eeprom(ppu4_level2,(A4 & 0X0F));
        write_eeprom(ppu3_level2,(A3 & 0X0F));
        write_eeprom(ppu2_level2,(A2 & 0X0F));
        write_eeprom(ppu1_level2,(A1 & 0X0F));
        sd_ppu(0x02);
    }
    return;
}

void venta_ppu(uint8_t niv, uint8_t pos)
{
    
    if(niv == 0XF4)
    {

        if(read_eeprom(estado1) != 0x09 && pos == 0x01)
        {
            write_eeprom(ppu4_venta1,read_eeprom(ppu4_level1));
            write_eeprom(ppu3_venta1,read_eeprom(ppu3_level1));
            write_eeprom(ppu2_venta1,read_eeprom(ppu2_level1));
            write_eeprom(ppu1_venta1,read_eeprom(ppu1_level1));
            write_eeprom(level_1,niv);
            sd_venta(pos);
        }

        else if(read_eeprom(estado2) != 0x09 && pos == 0x02)
        {
            write_eeprom(ppu4_venta2,read_eeprom(ppu4_level1));
            write_eeprom(ppu3_venta2,read_eeprom(ppu3_level1));
            write_eeprom(ppu2_venta2,read_eeprom(ppu2_level1));
            write_eeprom(ppu1_venta2,read_eeprom(ppu1_level1));
            write_eeprom(level_2,niv);
            sd_venta(pos);
        }
    }

    else if(niv == 0XF5)
    {
        
        if(read_eeprom(estado1) != 0x09 && pos == 0x01)
        {
            write_eeprom(ppu4_venta1,read_eeprom(ppu4_level2));
            write_eeprom(ppu3_venta1,read_eeprom(ppu3_level2));
            write_eeprom(ppu2_venta1,read_eeprom(ppu2_level2));
            write_eeprom(ppu1_venta1,read_eeprom(ppu1_level2));
            write_eeprom(level_1,niv);
            sd_venta(pos);
        }

        else if(read_eeprom(estado2) != 0x09 && pos == 0x02)
        {
            write_eeprom(ppu4_venta2,read_eeprom(ppu4_level2));
            write_eeprom(ppu3_venta2,read_eeprom(ppu3_level2));
            write_eeprom(ppu2_venta2,read_eeprom(ppu2_level2));
            write_eeprom(ppu1_venta2,read_eeprom(ppu1_level2));
            write_eeprom(level_2,niv);
            sd_venta(pos);
        }
    }
    return;
}

void rx_venta(uint8_t pos, uint8_t A1, uint8_t A2, uint8_t A3, uint8_t A4, uint8_t A5, uint8_t A6)
{
   
    if(pos == 0x01)
    {
        
        write_eeprom(preset1_1,A6);
        write_eeprom(preset1_2,A5);
        write_eeprom(preset1_3,A4);
        write_eeprom(preset1_4,A3);
        write_eeprom(preset1_5,A2);
        write_eeprom(preset1_6,A1);
        
    }

    else if(pos == 0x02)
    {
        write_eeprom(preset2_1,A6);
        write_eeprom(preset2_2,A5);
        write_eeprom(preset2_3,A4);
        write_eeprom(preset2_4,A3);
        write_eeprom(preset2_5,A2);
        write_eeprom(preset2_6,A1);
    }
}

uint8_t RxWire (void)
{
   
    uint8_t i=0;
    uint8_t A1=0;
    uint8_t A2=0;
    uint8_t A3=0;
    uint8_t A4=0;
    uint8_t A5=0;
    uint8_t A6=0;
    uint8_t DATO=0X00;
    uint8_t envio_wire[100];
    uint8_t val_che;
    recibo_wire[0] = 0x00;

    //recibo_wire[0]=sdGet(&SD2);
    recibo_wire[0] = sdGetTimeout(&SD2,500);

        
    chThdSleepMilliseconds(2);
    

    if(read_eeprom(lado1) == 0X01)
    {

        //ESTADO
        if (recibo_wire[0] == read_eeprom(pos1)) 
        {
            DATO = (read_eeprom(estado1) << 4);
            DATO = DATO | read_eeprom(pos1);
         
            sdPut(&SD2,DATO);

            
            chThdSleepMilliseconds(5);
            
            recibo_wire[1] = sdGetTimeout(&SD2,5); 
                 
            return;
        }

        //AUTORIZACION
        else if (recibo_wire[0] == (0X10 | read_eeprom(pos1))) 
        {

            if(read_eeprom(estado1) == 0X0C)
            {
                write_eeprom(estado1,0x09);
                write_eeprom(pasos1,0X0A);
                write_eeprom(pausa1,0x01);
            }
         
            else
            {

                if(read_eeprom(estado1) == 0X07)
                {
                    write_eeprom(pasos1,0X02);
                    write_eeprom(estado1,0x09);                
                    write_eeprom(autor1,0x01);
                    write_eeprom(pasos1,0X00);
                }

                else if(read_eeprom(estado1) == 0X06)
                {
                    write_eeprom(estado1,0x08);
                    write_eeprom(autor1,0x01);
                    write_eeprom(pasos1,0X00);                    
                }                

                if(((read_eeprom(level)) & 0xF0) == 0x00)
                {
                    venta_ppu(0XF4,0x01);
                }
            }
                                   
            return;
        }

        //DATA
        else if (recibo_wire[0]  == (0x20 | read_eeprom(pos1)))
        {

            i=0;
            DATO = (0XD0 | read_eeprom(pos1));

            //chThdSleepMilliseconds(2);

            sdPut(&SD2,DATO);

            while(true)
            {
                //DATO = sdGet(&SD2); 
                DATO = sdGetTimeout(&SD2,500);
                recibo_wire[i] = DATO;
                i++;


                if (DATO == 0XF0 || DATO == read_eeprom(pos1) || DATO == read_eeprom(pos2) || DATO == (0x20 | read_eeprom(pos1)) || i == 20) break;

            } 

            if(DATO == read_eeprom(pos1) || DATO == read_eeprom(pos2) ||  DATO == (0x20 | read_eeprom(pos1))) return; 

            //Comando espaciales
            if(recibo_wire[3] == 0XFE) 
            {
                rx_especial(recibo_wire[4], recibo_wire[5], recibo_wire[6], 0x01);
                return;
            }

            //PPU
            else if(recibo_wire[3] == 0XF4 || recibo_wire[3] == 0XF5) 
            {
                
                recibo_wire[0] = 0X30;
                val_che = verif_wire(recibo_wire, i,0x01);

                if(val_che == 1)
                {
                    if(i == 7) return;
                        
                    else
                    {
                        rx_ppu(recibo_wire[3], recibo_wire[10], recibo_wire[9], recibo_wire[8], recibo_wire[7]);
                        write_eeprom(SD_CONF,0x01);
                        return;                       
                    }
                }
                
                return;
                
            }

            //Volumen
            else if(recibo_wire[3] == 0XF1)
            {
                if(read_eeprom(estado1) != 0x09)
                {
                    recibo_wire[0] = 0x30;
                    val_che = verif_wire(recibo_wire, i,0x01);

                                            
                    if(val_che == 1)
                    {
                        
                        DATO = read_eeprom(level);
                        write_eeprom(level,(DATO | 0x10));
                        write_eeprom(preset_1,0x01);

                        A1 = recibo_wire[8] & 0X0F;
                        A2 = recibo_wire[9] & 0X0F;
                        A3 = recibo_wire[10] & 0X0F;
                        A4 = recibo_wire[11] & 0X0F;
                        A5 = recibo_wire[12] & 0X0F;
                        A6 = 0X00;

                        //EXTRA YA QUE UNOS CONTROLADORES MANDA 6 DIGITO, PERO EN REALIDAD SON 5 SIMEPRE PARA VOLUMEN ASI SEA DE 6 DIGITO

                        if(read_eeprom(PV) == 0X03)
                        {
                
                            A6=A5;
                            A5=A4;
                            A4=A3;
                            A3=A2;
                            A2=A1;
                            A1=0;
                        }

                        venta_ppu(recibo_wire[4],0x01);

                        rx_venta(0x01,A1,A2,A3,A4,A5,A6);
                        return;
                    }
                }
                return;
            }

            //dinero
            else if(recibo_wire[3] == 0XF2) 
            {
                if(read_eeprom(estado1) != 0x09)
                {
                    recibo_wire[0] = 0x30;
                    val_che = verif_wire(recibo_wire, i,0x01);

                                           
                    if(val_che == 1)
                    {

                        DATO = read_eeprom(level);
                        write_eeprom(level,(DATO | 0x10));
                        write_eeprom(preset_1,0X02);
                       
                        if (i == 17)
                        {
                           
                           A1 = recibo_wire[8] & 0X0F;
                           A2 = recibo_wire[9] & 0X0F;
                           A3 = recibo_wire[10] & 0X0F;
                           A4 = recibo_wire[11] & 0X0F;
                           A5 = recibo_wire[12] & 0X0F;
                           A6 = recibo_wire[13] & 0X0F;

                           venta_ppu(recibo_wire[4],0x01);
                           rx_venta(0x01,A1,A2,A3,A4,A5,A6);
                           return;
                        }

                        else if (i == 15)
                        {
                            
                            A1=recibo_wire[6] & 0X0F;
                            A2=recibo_wire[7] & 0X0F;
                            A3=recibo_wire[8] & 0X0F;
                            A4=recibo_wire[9] & 0X0F;
                            A5=recibo_wire[10] & 0X0F;
                            A6=recibo_wire[11] & 0X0F;

                            venta_ppu(recibo_wire[4],0x01);
                            rx_venta(0x01,A1,A2,A3,A4,A5,A6);
                            return;
                        }

                        else 
                        {
                            
                            if(read_eeprom(PD) == 0X03)
                            {
                                A2 = recibo_wire[6] & 0X0F;
                                A3 = recibo_wire[7] & 0X0F;
                                A4 = recibo_wire[8] & 0X0F;
                                A5 = recibo_wire[9] & 0X0F;
                                A6 = recibo_wire[10] & 0X0F;
                                A1 = 0x00;

                                venta_ppu(recibo_wire[4],0x01);
                                rx_venta(0x01,A1,A2,A3,A4,A5,A6);
                                return;
                            }

                            else
                            {
                                A1 = recibo_wire[6] & 0X0F;
                                A2 = recibo_wire[7] & 0X0F;
                                A3 = recibo_wire[8] & 0X0F;
                                A4 = recibo_wire[9] & 0X0F;
                                A5 = recibo_wire[10] & 0X0F;
                                A6 = 0x00; 

                                venta_ppu(recibo_wire[4],0x01);
                                rx_venta(0x01,A1,A2,A3,A4,A5,A6);
                                return;
                            }                            
                        }
                    }
                }
                return;
            }
           
            return;
        }

        //STOP
        else if ((recibo_wire[0] == (0X30 |  read_eeprom(pos1))  || recibo_wire[0] == 0XFC))
        {
            
            if(recibo_wire[0] == 0XFC)
            {
                if(read_eeprom(estado2) == 0x09 && read_eeprom(stop2) == 0X00)
                {
                    write_eeprom(stop2,0X01);
                    write_eeprom(estado2,0X0C);
                    write_eeprom(pausa2,0x01);               
                }

                else 
                {
                    write_eeprom(estado2,0x06);
                    write_eeprom(autor2,0x00);
                    write_eeprom(stop2,0x00);
                    write_eeprom(preset_2,0x00);
                    write_eeprom(pasos2,0X00);
                }
            }

            if(read_eeprom(estado1) == 0x09 && read_eeprom(stop1) == 0X00)
            {
                write_eeprom(stop1,0X01);
                write_eeprom(estado1,0X0C);
                write_eeprom(pausa1,0x01);                
            }

            else if(read_eeprom(estado1) == 0x06 || read_eeprom(estado1) == 0x07 || read_eeprom(estado1) == 0x08)
            {
                write_eeprom(estado1,0x06);
                write_eeprom(autor1,0x00);
                write_eeprom(stop1,0x00);
                write_eeprom(preset_1,0x00);
                write_eeprom(pasos1,0X00);   
                write_eeprom(pausa1,0x00);             
            }

            
            return;
        }

        //ENVIAR EL PRESET
        else if (recibo_wire[0] == (0X40 | read_eeprom(pos1)))
        {
             
            if(read_eeprom(estado1) == 0X0B) 
            {
                write_eeprom(estado1,0X06);
                write_eeprom(preset_1,0x00);
            }

            rx_reporte(0x01,0x01);
                      
            Status_1(0x06);
            write_eeprom(SD_CONF,0x01);

            return;
        }

        //ENVIAR TOTALES
        else if (recibo_wire[0] == (0X50 | read_eeprom(pos1)))
        {
            rx_reporte(0x02,0x01);
            return;
        }

        //REAL TIMER DINERO
        else if ((recibo_wire[0] == (0X60 | read_eeprom(pos1)) && read_eeprom(estado1) == 0x09))
        {
         
            if(read_eeprom(consola) == 0x04)
            {
                envio_wire[0] = 0xE0; 
                envio_wire[1] = (0xE0 | read_eeprom(real_din16));
                envio_wire[2] = (0xE0 | read_eeprom(real_din15));
                envio_wire[3] = (0xE0 | read_eeprom(real_din14));
                envio_wire[4] = (0xE0 | read_eeprom(real_din13));
                envio_wire[5] = (0xE0 | read_eeprom(real_din12));
            }
            else 
            {
                envio_wire[0] = (0xE0 | read_eeprom(real_din16));
                envio_wire[1] = (0xE0 | read_eeprom(real_din15));
                envio_wire[2] = (0xE0 | read_eeprom(real_din14));
                envio_wire[3] = (0xE0 | read_eeprom(real_din13));
                envio_wire[4] = (0xE0 | read_eeprom(real_din12));
                envio_wire[5] = (0xE0 | read_eeprom(real_din11));
            }
            

            Imprimir(&envio_wire,6); 

                    
            
            return;
        }

        //TIEMPO REAL DEL VOL
        else if (recibo_wire[0] == (0X70 | read_eeprom(pos1)) && read_eeprom(estado1) == 0x09) 
        {
         
            envio_wire[0] = 0xE1;
            envio_wire[1] = 0xE0 | read_eeprom(real_vol16);
            envio_wire[2] = 0xE0 | read_eeprom(real_vol15);
            envio_wire[3] = 0xE0 | read_eeprom(real_vol14);
            envio_wire[4] = 0xE0 | read_eeprom(real_vol13);
            envio_wire[5] = 0xE0 | read_eeprom(real_vol12);
            envio_wire[6] = 0xE0 | read_eeprom(real_vol11);

            Imprimir(&envio_wire,7);
            
            return;
        }

    } 

    if(read_eeprom(lado2) == 0X01)
    {

        //ESTADO
        if (recibo_wire[0] == read_eeprom(pos2)) 
        {
            DATO = (read_eeprom(estado2) << 4);
            DATO = DATO | read_eeprom(pos2);
         
            sdPut(&SD2,DATO);
            
            chThdSleepMilliseconds(5);
            
            recibo_wire[1] = sdGetTimeout(&SD2,5);                    
                 
            return;
        }

        //AUTORIZACION
        else if (recibo_wire[0] == (0X10 | read_eeprom(pos2))) 
        {
         
            if(read_eeprom(estado2) == 0X0C)
            {
                write_eeprom(estado2,0x09);
                write_eeprom(pasos2,0X0A);
                write_eeprom(pausa2,0x01);
            }
         
            else
            {

                if(read_eeprom(estado2) == 0X07)
                {
                    write_eeprom(pasos2,0X02);
                    write_eeprom(estado2,0x09);                
                    write_eeprom(autor2,0x01);
                    write_eeprom(pasos2,0X00);
                }

                else if(read_eeprom(estado2) == 0X06)
                {
                    write_eeprom(estado2,0x08);
                    write_eeprom(autor2,0x01);
                    write_eeprom(pasos2,0X00);                    
                }                

                if(((read_eeprom(level)) & 0x0F) == 0x00)
                {
                    venta_ppu(0XF4,0x02);
                }
            }

            return;
        }

        //DATA
        else if (recibo_wire[0]  == (0x20 | read_eeprom(pos2)))
        {

            i=0;
            DATO = (0XD0 | read_eeprom(pos2));

            //chThdSleepMilliseconds(2);

            sdPut(&SD2,DATO);

            while(true)
            {
                //DATO = sdGet(&SD2); 
                DATO = sdGetTimeout(&SD2,500);
                recibo_wire[i] = DATO;
                i++;

                if (DATO == 0XF0 || DATO == read_eeprom(pos1) || DATO == read_eeprom(pos2) || DATO == (0x20 | read_eeprom(pos2)) || i == 20) break;

            } 

            if(DATO == read_eeprom(pos1) || DATO == read_eeprom(pos2) ||  DATO == (0x20 | read_eeprom(pos2))) return; 

            //Comando espaciales
            if(recibo_wire[3] == 0XFE) 
            {
                rx_especial(recibo_wire[4], recibo_wire[5], recibo_wire[6], 0x02);
                return;
            }

            //PPU
            else if(recibo_wire[3] == 0XF4 || recibo_wire[3] == 0XF5) 
            {
                
                recibo_wire[0] = 0X30;
                val_che = verif_wire(recibo_wire, i,0x01);

                if(val_che == 1)
                {
                    if(i == 7) return;
                        
                    else
                    {
                        rx_ppu(recibo_wire[3], recibo_wire[10], recibo_wire[9], recibo_wire[8], recibo_wire[7]);
                        write_eeprom(SD_CONF,0x01);
                        return;                       
                    }
                }
                
                return;                
            }

            //Volumen
            else if(recibo_wire[3] == 0XF1)
            {
                if(read_eeprom(estado2) != 0x09)
                {
                    recibo_wire[0] = 0x30;
                    val_che = verif_wire(recibo_wire, i,0x01);
                                            
                    if(val_che == 1)
                    {
                        
                        DATO = read_eeprom(level);
                        write_eeprom(level,(DATO | 0x01));

                        write_eeprom(preset_2,0x01);

                        A1 = recibo_wire[8] & 0X0F;
                        A2 = recibo_wire[9] & 0X0F;
                        A3 = recibo_wire[10] & 0X0F;
                        A4 = recibo_wire[11] & 0X0F;
                        A5 = recibo_wire[12] & 0X0F;
                        A6 = 0X00;

                        //EXTRA YA QUE UNOS CONTROLADORES MANDA 6 DIGITO, PERO EN REALIDAD SON 5 SIMEPRE PARA VOLUMEN ASI SEA DE 6 DIGITO

                        if(read_eeprom(PV) == 0X03)
                        {
                
                            A6=A5;
                            A5=A4;
                            A4=A3;
                            A3=A2;
                            A2=A1;
                            A1=0;
                        }

                        venta_ppu(recibo_wire[4],0x02);
                        rx_venta(0x02,A1,A2,A3,A4,A5,A6);
                        return;
                    }
                }
                return;
            }

            //dinero
            else if(recibo_wire[3] == 0XF2) 
            {
                if(read_eeprom(estado2) != 0x09)
                {
                    recibo_wire[0] = 0x30;
                    val_che = verif_wire(recibo_wire, i,0x01);
                                          
                    if(val_che == 1)
                    {

                        DATO = read_eeprom(level);
                        write_eeprom(level,(DATO | 0x01));
                        write_eeprom(preset_2,0X02);
                        
                        if (i == 17)
                        {
                           
                            A1 = recibo_wire[8] & 0X0F;
                            A2 = recibo_wire[9] & 0X0F;
                            A3 = recibo_wire[10] & 0X0F;
                            A4 = recibo_wire[11] & 0X0F;
                            A5 = recibo_wire[12] & 0X0F;
                            A6 = recibo_wire[13] & 0X0F;

                            venta_ppu(recibo_wire[4],0x02);
                            rx_venta(0x02,A1,A2,A3,A4,A5,A6);
                            return;
                        }

                        else if (i == 15)
                        {
                            
                            A1=recibo_wire[6] & 0X0F;
                            A2=recibo_wire[7] & 0X0F;
                            A3=recibo_wire[8] & 0X0F;
                            A4=recibo_wire[9] & 0X0F;
                            A5=recibo_wire[10] & 0X0F;
                            A6=recibo_wire[11] & 0X0F;

                            venta_ppu(recibo_wire[4],0x02);
                            rx_venta(0x02,A1,A2,A3,A4,A5,A6);
                            return;
                        }

                        else 
                        {
                            
                            if(read_eeprom(PD) == 0X03)
                            {
                                A2 = recibo_wire[6] & 0X0F;
                                A3 = recibo_wire[7] & 0X0F;
                                A4 = recibo_wire[8] & 0X0F;
                                A5 = recibo_wire[9] & 0X0F;
                                A6 = recibo_wire[10] & 0X0F;
                                A1 = 0x00;

                                venta_ppu(recibo_wire[4],0x02);
                                rx_venta(0x02,A1,A2,A3,A4,A5,A6);
                                return;
                            }

                            else
                            {
                                A1 = recibo_wire[6] & 0X0F;
                                A2 = recibo_wire[7] & 0X0F;
                                A3 = recibo_wire[8] & 0X0F;
                                A4 = recibo_wire[9] & 0X0F;
                                A5 = recibo_wire[10] & 0X0F;
                                A6 = 0x00; 

                                venta_ppu(recibo_wire[4],0x02);
                                rx_venta(0x02,A1,A2,A3,A4,A5,A6);
                                return;
                            }                            
                        }
                    }
                }
                return;
            }
           
            return;
        }

        //STOP
        else if ((recibo_wire[0] == (0X30 |  read_eeprom(pos2))  || recibo_wire[0] == 0XFC ))
        {
            if(recibo_wire[0] == 0XFC )
            {

                if(read_eeprom(estado1) == 0x09 && read_eeprom(stop1) == 0X00)
                {
                    write_eeprom(stop1,0X01);
                    write_eeprom(estado1,0X0C);
                    write_eeprom(pausa1,0x01);                
                }

                else if(read_eeprom(estado1) == 0x06 || read_eeprom(estado1) == 0x07 || read_eeprom(estado1) == 0x08)
                {
                    write_eeprom(estado1,0x06);
                    write_eeprom(autor1,0x00);
                    write_eeprom(stop1,0x00);
                    write_eeprom(preset_1,0x00);
                    write_eeprom(pasos1,0X00);   
                    write_eeprom(pausa1,0x00);             
                }
            }

            if(read_eeprom(estado2) == 0x09 && read_eeprom(stop2) == 0X00)
            {
                write_eeprom(stop2,0X01);
                write_eeprom(estado2,0X0C);
                write_eeprom(pausa2,0x01);               
            }

            else 
            {
                write_eeprom(estado2,0x06);
                write_eeprom(autor2,0x00);
                write_eeprom(stop2,0x00);
                write_eeprom(preset_2,0x00);
                write_eeprom(pasos2,0X00);
            }
            
            return;
        }

        //ENVIAR EL PRESET
        else if (recibo_wire[0] == (0X40 | read_eeprom(pos2)))
        {
             
            if(read_eeprom(estado2) == 0X0B) 
            {
                write_eeprom(estado2,0X06);
                write_eeprom(preset_2,0x00);
            }

            rx_reporte(0x01,0x02);
                      
            Status_2(0x06);
            write_eeprom(SD_CONF,0x01);

            return;
        }

        //ENVIAR TOTALES
        else if (recibo_wire[0] == (0X50 | read_eeprom(pos2)))
        {
            rx_reporte(0x02,0x02);
            return;
        }

        //REAL TIMER DINERO
        else if ((recibo_wire[0] == (0X60 | read_eeprom(pos2)) && read_eeprom(estado2) == 0x09))
        {
                
            if(read_eeprom(consola) == 0x04)
            {
                envio_wire[0] = 0xE0; 
                envio_wire[1] = (0xE0 | read_eeprom(real_din26));
                envio_wire[2] = (0xE0 | read_eeprom(real_din25));
                envio_wire[3] = (0xE0 | read_eeprom(real_din24));
                envio_wire[4] = (0xE0 | read_eeprom(real_din23));
                envio_wire[5] = (0xE0 | read_eeprom(real_din22));
            }
            else 
            {
                envio_wire[0] = (0xE0 | read_eeprom(real_din26));
                envio_wire[1] = (0xE0 | read_eeprom(real_din25));
                envio_wire[2] = (0xE0 | read_eeprom(real_din24));
                envio_wire[3] = (0xE0 | read_eeprom(real_din23));
                envio_wire[4] = (0xE0 | read_eeprom(real_din22));
                envio_wire[5] = (0xE0 | read_eeprom(real_din21));
            }     
            

            Imprimir(&envio_wire,6);                     
            
            return;
        }

        //TIEMPO REAL DEL VOL
        else if (recibo_wire[0] == (0X70 | read_eeprom(pos2)) && read_eeprom(estado2) == 0x09) 
        {
         
            envio_wire[0] = 0xE1;
            envio_wire[1] = 0xE0 | read_eeprom(real_vol26);
            envio_wire[2] = 0xE0 | read_eeprom(real_vol25);
            envio_wire[3] = 0xE0 | read_eeprom(real_vol24);
            envio_wire[4] = 0xE0 | read_eeprom(real_vol23);
            envio_wire[5] = 0xE0 | read_eeprom(real_vol22);
            envio_wire[6] = 0xE0 | read_eeprom(real_vol21);

            Imprimir(&envio_wire,7);
            
            return;
        }

    } 
 
    return;
}




