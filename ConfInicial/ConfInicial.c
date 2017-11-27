
#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "Emr3V210.h"
#include "serial.h"
#include "ff.h"
//#include "LiquidCrystal.h"
#include "ascii.h"
#include "ValidarEmr3.h"
#include "ConfInicial.h"
#include "EepromVirtual.h"
#include "LectureSw.h"
#include "ButtonEmr3.h"
#include "DataSd.h"



void ConfInicial(const char data_inicial[])
{
   	
   	
   	LCDPrintRow( 0,"  Read microSD  ");
    LCDPrintRow2( 0,"                ");

    if(data_inicial[1] == ',' && data_inicial[4] == ',' && data_inicial[7] == ',' && data_inicial[9] == ',' && data_inicial[11] == ',' && data_inicial[13] == ',' && data_inicial[19] == ',' && data_inicial[25] == ',' && data_inicial[35] == ',' && data_inicial[45] == ',' && data_inicial[55] == ',')
    {
        
        
        write_eeprom(consola,(data_inicial[cons] & 0X0F));
        write_eeprom(estado1,(data_inicial[est1] & 0X0F));
   
        if(data_inicial[p1] == 'A' || data_inicial[p1] == 'a') write_eeprom(pos1,0X0A);
        else if(data_inicial[p1] == 'B' || data_inicial[p1] == 'b')write_eeprom(pos1,0X0B);
        else if(data_inicial[p1] == 'C' || data_inicial[p1] == 'c') write_eeprom(pos1,0X0C);
        else if(data_inicial[p1] == 'D' || data_inicial[p1] == 'd') write_eeprom(pos1,0X0D);
        else if(data_inicial[p1] == 'E' || data_inicial[p1] == 'e') write_eeprom(pos1,0X0E);
        else if(data_inicial[p1] == 'F' || data_inicial[p1] == 'f') write_eeprom(pos1,0X0F);
        else write_eeprom(pos1,(data_inicial[p1] & 0X0F));  
       
       
        write_eeprom(estado2,(data_inicial[est2] & 0X0F));

        if(data_inicial[p2] == 'A' || data_inicial[p2] == 'a') write_eeprom(pos2,0X0A);
        else if(data_inicial[p2] == 'B' || data_inicial[p2] == 'b') write_eeprom(pos2,0X0B);
        else if(data_inicial[p2] == 'C' || data_inicial[p2] == 'c') write_eeprom(pos2,0X0C);
        else if(data_inicial[p2] == 'D' || data_inicial[p2] == 'd') write_eeprom(pos2,0X0D);
        else if(data_inicial[p2] == 'E' || data_inicial[p2] == 'e') write_eeprom(pos2,0X0E);
        else if(data_inicial[p2] == 'F' || data_inicial[p2] == 'f') write_eeprom(pos2,0X0F);
        else write_eeprom(pos2,(data_inicial[p2] & 0X0F)); 

       

        write_eeprom(pppu,(data_inicial[p_p] & 0X0F));

       
        write_eeprom(PD,(data_inicial[p_d] & 0X0F));

       
        write_eeprom(PV,(data_inicial[p_v] & 0X0F)); 

       
        write_eeprom(ppu1_level1,(data_inicial[ppu1_1] & 0X0F));

        
        if(data_inicial[ppu2_1] == '.')
        {
            write_eeprom(ppu2_level1,(data_inicial[ppu3_1] & 0X0F));
            write_eeprom(ppu3_level1,(data_inicial[ppu4_1] & 0X0F));
            write_eeprom(ppu4_level1,(data_inicial[ppu5_1] & 0X0F));
              
        }

        else if(data_inicial[ppu3_1] == '.')
        {
            write_eeprom(ppu2_level1,(data_inicial[ppu2_1] & 0X0F));
            write_eeprom(ppu3_level1,(data_inicial[ppu4_1] & 0X0F));
            write_eeprom(ppu4_level1,(data_inicial[ppu5_1] & 0X0F));

        }

        else if(data_inicial[ppu4_1] == '.')
        {
            write_eeprom(ppu2_level1,(data_inicial[ppu2_1] & 0X0F));
            write_eeprom(ppu3_level1,(data_inicial[ppu3_1] & 0X0F));
            write_eeprom(ppu4_level1,(data_inicial[ppu5_1] & 0X0F));

        }

        else if(data_inicial[ppu5_1] == '.')
        {
            write_eeprom(ppu2_level1,(data_inicial[ppu2_1] & 0X0F));
            write_eeprom(ppu3_level1,(data_inicial[ppu3_1] & 0X0F));
            write_eeprom(ppu4_level1,(data_inicial[ppu4_1] & 0X0F));

        }

        sd_ppu(0x01);

        write_eeprom(ppu1_level2,(data_inicial[ppu1_2] & 0X0F));

        
        if(data_inicial[ppu2_2] == '.')
        {
            write_eeprom(ppu2_level2,(data_inicial[ppu3_2] & 0X0F));
            write_eeprom(ppu3_level2,(data_inicial[ppu4_2] & 0X0F));
            write_eeprom(ppu4_level2,(data_inicial[ppu5_2] & 0X0F));
              
        }

        else if(data_inicial[ppu3_2] == '.')
        {
            write_eeprom(ppu2_level2,(data_inicial[ppu2_2] & 0X0F));
            write_eeprom(ppu3_level2,(data_inicial[ppu4_2] & 0X0F));
            write_eeprom(ppu4_level2,(data_inicial[ppu5_2] & 0X0F));

        }

        else if(data_inicial[ppu4_2] == '.')
        {
            write_eeprom(ppu2_level2,(data_inicial[ppu2_2] & 0X0F));
            write_eeprom(ppu3_level2,(data_inicial[ppu3_2] & 0X0F));
            write_eeprom(ppu4_level2,(data_inicial[ppu5_2] & 0X0F));
        }

        else if(data_inicial[ppu5_2] == '.')
        {
            write_eeprom(ppu2_level2,(data_inicial[ppu2_2] & 0X0F));
            write_eeprom(ppu3_level2,(data_inicial[ppu3_2] & 0X0F));
            write_eeprom(ppu4_level2,(data_inicial[ppu4_2] & 0X0F));

        }

        sd_ppu(0x02);
        


        write_eeprom(td1_1,(data_inicial[dt1_1] & 0X0F));
        write_eeprom(td1_2,(data_inicial[dt1_2] & 0X0F));
        write_eeprom(td1_3,(data_inicial[dt1_3] & 0X0F));
        write_eeprom(td1_4,(data_inicial[dt1_4] & 0X0F));
        write_eeprom(td1_5,(data_inicial[dt1_5] & 0X0F));

        if(data_inicial[dt1_6] == '.')
        {
            write_eeprom(td1_6,(data_inicial[dt1_7] & 0X0F));
            write_eeprom(td1_7,(data_inicial[dt1_8] & 0X0F));
            write_eeprom(td1_8,(data_inicial[dt1_9] & 0X0F));
        }

        else if(data_inicial[dt1_7] == '.')
        {
            write_eeprom(td1_6,(data_inicial[dt1_6] & 0X0F));
            write_eeprom(td1_7,(data_inicial[dt1_8] & 0X0F));
            write_eeprom(td1_8,(data_inicial[dt1_9] & 0X0F));
        }

        else if(data_inicial[dt1_8] == '.')
        {
            write_eeprom(td1_6,(data_inicial[dt1_6] & 0X0F));
            write_eeprom(td1_7,(data_inicial[dt1_7] & 0X0F));
            write_eeprom(td1_8,(data_inicial[dt1_9] & 0X0F));
        }

        else if(data_inicial[dt1_9] == '.')
        {
            write_eeprom(td1_6,(data_inicial[dt1_6] & 0X0F));
            write_eeprom(td1_7,(data_inicial[dt1_7] & 0X0F));
            write_eeprom(td1_8,(data_inicial[dt1_8] & 0X0F));
        }

        sd_Tdinero(0x01);

        write_eeprom(td2_1,(data_inicial[dt2_1] & 0X0F));
        write_eeprom(td2_2,(data_inicial[dt2_2] & 0X0F));
        write_eeprom(td2_3,(data_inicial[dt2_3] & 0X0F));
        write_eeprom(td2_4,(data_inicial[dt2_4] & 0X0F));
        write_eeprom(td2_5,(data_inicial[dt2_5] & 0X0F));

        if(data_inicial[dt2_6] == '.')
        {
            write_eeprom(td2_6,(data_inicial[dt2_7] & 0X0F));
            write_eeprom(td2_7,(data_inicial[dt2_8] & 0X0F));
            write_eeprom(td2_8,(data_inicial[dt2_9] & 0X0F));
        }

        else if(data_inicial[dt2_7] == '.')
        {
            write_eeprom(td2_6,(data_inicial[dt2_6] & 0X0F));
            write_eeprom(td2_7,(data_inicial[dt2_8] & 0X0F));
            write_eeprom(td2_8,(data_inicial[dt2_9] & 0X0F));
        }

        else if(data_inicial[dt2_8] == '.')
        {
            write_eeprom(td2_6,(data_inicial[dt2_6] & 0X0F));
            write_eeprom(td2_7,(data_inicial[dt2_7] & 0X0F));
            write_eeprom(td2_8,(data_inicial[dt2_9] & 0X0F));
        }

        else if(data_inicial[dt2_9] == '.')
        {
            write_eeprom(td2_6,(data_inicial[dt2_6] & 0X0F));
            write_eeprom(td2_7,(data_inicial[dt2_7] & 0X0F));
            write_eeprom(td2_8,(data_inicial[dt2_8] & 0X0F));
        }

        sd_Tdinero(0x02);


        write_eeprom(inicio1,0x01);
        write_eeprom(inicio2,0x01);

        write_eeprom(lado1,0x01);
        write_eeprom(lado2,0x01);

        

        Status_1(read_eeprom(estado1));
        
        Status_2(read_eeprom(estado2));
    }

    else
    {
        LCDPrintRow2(0,"   Error=0006   ");
        while(true)
        {
            chThdSleepMilliseconds(5000);
        }
    }
   	

    chThdSleepMilliseconds(2000);
    
    return;
    
}


