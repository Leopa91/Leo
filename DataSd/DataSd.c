#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "Emr3V210.h"
#include "serial.h"
#include "ff.h"
#include "ffconf.h"
#include "ascii.h"
#include "EepromVirtual.h"
#include "LectureSw.h"
#include "ButtonEmr3.h"
#include "DataSd.h"
#include "RxWire.h"

uint32_t FILE_FIN = 620;

/*===========================================================================*/
/* FatFs related.                                                            */
/*===========================================================================*/
FATFS MMC_FS;
MMCDriver MMCD1;

/*===========================================================================*/
/* FatFs related.                                                            */
/*===========================================================================*/

/**
* @brief FS object.
*/
static FATFS SDC_FS;

/* FS mounted and ready.*/
static bool fs_ready = FALSE;

/* Generic large buffer.*/
static uint8_t fbuff[1024];

static const SPIConfig hs_spicfg = 
{ 
    NULL, 
    GPIOA, 
    4, 
    SPI_CR1_BR_2 | SPI_CR1_BR_1 //0 
};


static const SPIConfig ls_spicfg = 
{ 
    NULL, 
    GPIOA, 
    4, 
    SPI_CR1_BR_2 | SPI_CR1_BR_1 
};

static MMCConfig mmccfg = {&SPID1, &ls_spicfg, &hs_spicfg};

static FRESULT scan_files(BaseSequentialStream *chp, char *path) 
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
    #if _USE_LFN
    fno.lfname = 0;
    fno.lfsize = 0;
    #endif
    res = f_opendir(&dir, path);
    if (res == FR_OK) 
    {
        i = strlen(path);
        
        for (;;) 
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
            break;
            
            if (fno.fname[0] == '.')
            continue;
            fn = fno.fname;
            
            if (fno.fattrib & AM_DIR) 
            {
                path[i++] = '/';
                strcpy(&path[i], fn);
                res = scan_files(chp, path);
            
                if (res != FR_OK)
                break;
                path[--i] = 0;
            }
        
            else 
            {

            }
        }
    }
    
    return res;
}

void borrar_sd1(void)
{
    FRESULT fr2;

    fr2 = f_unlink("transh.txt");

    fr2 = f_unlink("s1emr3h.txt");

    fr2 = f_unlink("s1wireh.txt");

    fr2 = f_rename("trans.txt","transh.txt");

    fr2 = f_rename("s1emr3.txt","s1emr3h.txt");
   
    fr2 = f_rename("s1wire.txt","s1wireh.txt");

    ventas_1 = false;
    return;
}

void borrar_sd2(void)
{
    FRESULT fr2;

    fr2 = f_unlink("trans2h.txt");

    fr2 = f_unlink("s2emr3h.txt");
   
    fr2 = f_unlink("s2wireh.txt");

    fr2 = f_rename("trans2.txt","trans2h.txt");
 
    fr2 = f_rename("s2emr3.txt","s2emr3h.txt");
   
    fr2 = f_rename("s2wire.txt","s2wireh.txt");

    ventas_2 = false;
    return;
}

void borrar_log(void)
{
    FRESULT fr2;
    fr2 = f_unlink("lwire.txt");
    return;
}

void memori(void)
{
    // initialize MMC driver
    mmcObjectInit(&MMCD1); 
    mmcStart(&MMCD1, &mmccfg);
    //tmr_init(&MMCD1);

    FRESULT err;

    if (mmcConnect(&MMCD1)) 
    {
        LCDPrintRow2(0,"   Error=0001   ");
        while(true)
        {
            chThdSleepMilliseconds(5000);
        }
    }
   
      
    err = f_mount(&MMC_FS, "/", 1); 

    if (err != FR_OK) 
    {
        mmcDisconnect(&MMCD1);

        LCDPrintRow2(0,"   Error=0002   ");
        
        while(true)
        {
            chThdSleepMilliseconds(5000);
        }
        
    }
    
    fs_ready = TRUE;
    
    LCDPrintRow2( 0,"   ok microSD   ");

    chThdSleepMilliseconds(1000);

    //LEER LA INFORMACION DE LA SD 

    Read();
    return;
}

void Read(void)
{
    FRESULT err;
    uint32_t clusters;
    FATFS *fsp;
    FIL fil;       /* File object */
    char line_in[66]; /* Line buffer */
    FRESULT fr;    /* FatFs return code */
    uint8_t i;
    FILINFO fno;

    err = f_getfree("/", &clusters, &fsp);

    if (err != FR_OK) 
    {
        LCDPrintRow2(0,"   Error=0003   ");

        while(true)
        {
            chThdSleepMilliseconds(5000);
        }
        return;
    }

    /* Open a text file */

    fr = f_stat("config.txt", &fno);

    switch (fr) 
    {

        case FR_OK:
            //existe

            fr = f_open(&fil, "config.txt", FA_READ);

            if (fr)
            { 
                
                LCDPrintRow2(0,"   Error=0005   ");
                while(true)
                {
                    chThdSleepMilliseconds(5000);
                }
                return (int)fr;
            }
          
            f_gets(line_in, sizeof line_in, &fil);

            f_close(&fil);

            ConfInicial(line_in);            

            /* Close the file */
         
        break;

        case FR_NO_FILE:

            LCDPrintRow2(0,"   Error=0004   ");

            while(true)
            {
                chThdSleepMilliseconds(5000);
            }
            
        break;
    }

    
    return;
}

void Write(void)
{

	FATFS *fsp;

    //FRESULT err;
    FIL fil;       /* File object */
    char line[25]; /* Line buffer */
    FRESULT fr;    /* FatFs return code */
    uint8_t bw;


    // /* Open a text file */
    fr = f_open(&fil, "config.txt", FA_WRITE); 

    if (fr)
    { 
        return (int)fr;
    }

    fr = f_write(&fil, &Status1 ,5, &bw);
    fr = f_write(&fil, &Status2 ,3, &bw);
    fr = f_write(&fil, &PPU_POINT ,2, &bw);
    fr = f_write(&fil, &MON_POINT ,2, &bw);
    fr = f_write(&fil, &VOL_POINT ,2, &bw);
    fr = f_write(&fil, &PPU_LEVEL1 ,6, &bw);
    fr = f_write(&fil, &PPU_LEVEL2 ,6, &bw);
    fr = f_write(&fil, &T_DINERO1 ,10, &bw);
    fr = f_write(&fil, &T_VOL1 ,10, &bw);
    fr = f_write(&fil, &T_DINERO2 ,10, &bw);
    fr = f_write(&fil, &T_VOL2 ,10, &bw);
    fr = f_write(&fil, &ENTER_INIC,2, &bw);

   	// /* Close the file */
   	f_close(&fil);

   	return;
}

void trans(void)
{

   	uint32_t clusters2;
   	FATFS *fsp2;
   	FRESULT err2;
   	FIL fil2;       /* File object */
   	char line[25]; /* Line buffer */
   	FRESULT fr2;    /* FatFs return code */
   	uint8_t bw2;
   	FILINFO fno;
    uint32_t T;

   	fr2 = f_stat("trans.txt", &fno);

   	switch (fr2) 
   	{

      	case FR_OK:
    		//existe

    		/*
    		ESCRRITURA EN LA SD DE LAS TRANSACIONES
    		*/

    		fr2 = f_open(&fil2, "trans.txt", FA_READ|FA_WRITE);

    		if (fr2)
    		{ 
                
    			return (int)fr2;
    		}

            T = f_size(&fil2);

            if(T > FILE_FIN)
            {
                ventas_1 = true;
            }

            fr2 = f_lseek(&fil2,T);

    		//while(f_gets(line, sizeof(line), &fil2));

           
    		fr2 = f_write(&fil2, &FECHA ,11, &bw2);
    		fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA1 ,6, &bw2);
    		fr2 = f_write(&fil2, &DINERO1 ,8, &bw2);
    		fr2 = f_write(&fil2, &VOL1 ,8, &bw2);
    		fr2 = f_write(&fil2, &T_DINERO1 ,10, &bw2);
    		fr2 = f_write(&fil2, &T_VOL1 ,10, &bw2);
    		fr2 = f_write(&fil2, &PRESET1 ,9, &bw2);
    		fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

           

    		/* Close the file */
    		f_close(&fil2);
   		break;

		case FR_NO_FILE:

    		fr2 = f_open(&fil2, "trans.txt", FA_CREATE_NEW | FA_WRITE);

    		if (fr2)
    		{ 

    		 	return (int)fr2;
    		}

            fr2 = f_lseek(&fil2,0);

    		fr2 = f_write(&fil2, &FECHA ,11, &bw2);
    		fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA1 ,6, &bw2);
    		fr2 = f_write(&fil2, &DINERO1 ,8, &bw2);
    		fr2 = f_write(&fil2, &VOL1 ,8, &bw2);
    		fr2 = f_write(&fil2, &T_DINERO1 ,10, &bw2);
    		fr2 = f_write(&fil2, &T_VOL1 ,10, &bw2);
    		fr2 = f_write(&fil2, &PRESET1 ,9, &bw2);
    		fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            

    		/* Close the file */
    		f_close(&fil2);
		break;

	}

   return;
}

void trans2(void)
{

	uint32_t clusters2;
	FATFS *fsp2;

	FRESULT err2;
	FIL fil2;       /* File object */
	char line[25]; /* Line buffer */
	FRESULT fr2;    /* FatFs return code */
	uint8_t bw2;
	FILINFO fno;
    uint32_t T;

   	fr2 = f_stat("trans2.txt", &fno);

   	switch (fr2) 
   	{

		case FR_OK:
			//existe

			/*
			ESCRRITURA EN LA SD DE LAS TRANSACIONES
			*/

			fr2 = f_open(&fil2, "trans2.txt", FA_READ|FA_WRITE);

			if (fr2)
			{ 
			return (int)fr2;
			}

            T = f_size(&fil2);

            if(T > FILE_FIN)
            {
                ventas_2 = true;
                
            }

            fr2 = f_lseek(&fil2,T);

			//while(f_gets(line, sizeof(line), &fil2));

			fr2 = f_write(&fil2, &FECHA ,11, &bw2);
			fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA2 ,6, &bw2);
			fr2 = f_write(&fil2, &DINERO2 ,8, &bw2);
			fr2 = f_write(&fil2, &VOL2 ,8, &bw2);
			fr2 = f_write(&fil2, &T_DINERO2 ,10, &bw2);
			fr2 = f_write(&fil2, &T_VOL2 ,10, &bw2);
			fr2 = f_write(&fil2, &PRESET2,9, &bw2);
			fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

			/* Close the file */
			f_close(&fil2);

		break;

      	case FR_NO_FILE:

			fr2 = f_open(&fil2, "trans2.txt", FA_CREATE_NEW | FA_WRITE);

			if (fr2)
			{ 
				return (int)fr2;
			}

            fr2 = f_lseek(&fil2,0);

			fr2 = f_write(&fil2, &FECHA ,11, &bw2);
			fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA2 ,6, &bw2);
			fr2 = f_write(&fil2, &DINERO2 ,8, &bw2);
			fr2 = f_write(&fil2, &VOL2 ,8, &bw2);
			fr2 = f_write(&fil2, &T_DINERO2 ,10, &bw2);
			fr2 = f_write(&fil2, &T_VOL2 ,10, &bw2);
			fr2 = f_write(&fil2, &PRESET2,9, &bw2);
			fr2 = f_write(&fil2, &ENTER_INIC ,1, &bw2);

			/* Close the file */
			f_close(&fil2);
		break;
   	}

   return;
}

void sale1_emr3(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("s1emr3.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "s1emr3.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx ,strlen(Data_Tx), &bw2);
            fr2 = f_write(&fil2, &Data_Rx ,strlen(Data_Rx), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "s1emr3.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx ,strlen(Data_Tx), &bw2);
            fr2 = f_write(&fil2, &Data_Rx ,strlen(Data_Rx), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    if(ventas_1 && read_eeprom(consola) == 0x01)
    {
        borrar_sd1();
        ventas_1 = false;
    }

    return;
}

void sale2_emr3(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("s2emr3.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "s2emr3.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx2 ,strlen(Data_Tx2), &bw2);
            fr2 = f_write(&fil2, &Data_Rx2 ,strlen(Data_Rx2), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "s2emr3.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx2 ,strlen(Data_Tx2), &bw2);
            fr2 = f_write(&fil2, &Data_Rx2 ,strlen(Data_Rx2), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    if(ventas_2 && read_eeprom(consola) == 0x01)
    {
        borrar_sd2();
        ventas_2 = false;
    }

    return;
}

void emr3_sale1(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3)
{
    uint8_t i;
    uint8_t j=3;
    uint8_t k=3;

    while(true)
    {
        if(read_eeprom(SD_emr3_sale1) == 0) break;
        chThdSleepMilliseconds(10);
    }

    if(dta_emr3 == 1)
    {
        
        Data_Tx[0]='T';
        Data_Tx[1]='x';
        Data_Tx[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Tx[j] = ascii(((DataTx[i] >> 4) & 0X0F));
            j++;
            Data_Tx[j] = ascii((DataTx[i] & 0X0F));
            j++;
            Data_Tx[j] = ',';
            j++;
        }
        Data_Tx[j]=0x00;
       
    }

    else if(dta_emr3 == 2)
    {
        

        Data_Rx[0]='R';
        Data_Rx[1]='x';
        Data_Rx[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Rx[k] = ascii(((DataTx[i] >> 4) & 0X0F));
            k++;
            Data_Rx[k] = ascii((DataTx[i] & 0X0F));
            k++;
            Data_Rx[k] = ',';
            k++;
        }
        Data_Rx[k]=0x00;        
        
        write_eeprom(SD_emr3_sale1,0x01);
    }
    
    return;
}

void emr3_sale2(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3)
{
    uint8_t i;
    uint8_t j=3;
    uint8_t k=3;

    while(true)
    {
        if(read_eeprom(SD_emr3_sale2) == 0) break;
        chThdSleepMilliseconds(10);
    }

    if(dta_emr3 == 1)
    {
        
        Data_Tx2[0]='T';
        Data_Tx2[1]='x';
        Data_Tx2[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Tx2[j] = ascii(((DataTx[i] >> 4) & 0X0F));
            j++;
            Data_Tx2[j] = ascii((DataTx[i] & 0X0F));
            j++;
            Data_Tx2[j] = ',';
            j++;
        }
        Data_Tx2[j]=0x00;
       
    }

    else if(dta_emr3 == 2)
    {
        

        Data_Rx2[0]='R';
        Data_Rx2[1]='x';
        Data_Rx2[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Rx2[k] = ascii(((DataTx[i] >> 4) & 0X0F));
            k++;
            Data_Rx2[k] = ascii((DataTx[i] & 0X0F));
            k++;
            Data_Rx2[k] = ',';
            k++;
        }
        Data_Rx2[k]=0x00;        
        
        write_eeprom(SD_emr3_sale2,0x01);
    }
    
    return;
}

void sale1_wire(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("s1wire.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "s1wire.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx3 ,strlen(Data_Tx3), &bw2);
            fr2 = f_write(&fil2, &Data_Rx3 ,strlen(Data_Rx3), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "s1wire.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx3 ,strlen(Data_Tx3), &bw2);
            fr2 = f_write(&fil2, &Data_Rx3 ,strlen(Data_Rx3), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    if(ventas_1)
    {
        borrar_sd1();
        ventas_1 = false;
    }

    return;
}

void sale2_wire(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("s2wire.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "s2wire.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx4 ,strlen(Data_Tx4), &bw2);
            fr2 = f_write(&fil2, &Data_Rx4 ,strlen(Data_Rx4), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "s2wire.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &Data_Tx4 ,strlen(Data_Tx4), &bw2);
            fr2 = f_write(&fil2, &Data_Rx4 ,strlen(Data_Rx4), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    if(ventas_2)
    {
        borrar_sd2();
        ventas_2 = false;
    }

    return;
}

void wire_sale1(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3)
{
    uint8_t i;
    uint8_t j=3;
    uint8_t k=3;

    if(dta_emr3 == 1)
    {
        
        Data_Tx3[0]='4';
        Data_Tx3[1]=ascii(read_eeprom(pos1));
        Data_Tx3[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Tx3[j] = ascii(((DataTx[i] >> 4) & 0X0F));
            j++;
            Data_Tx3[j] = ascii((DataTx[i] & 0X0F));
            j++;
            Data_Tx3[j] = ',';
            j++;
        }
        Data_Tx3[j]=0x00; 
        return;      
    }

    else if(dta_emr3 == 2)
    {
        
        Data_Rx3[0]='5';
        Data_Rx3[1]=ascii(read_eeprom(pos1));
        Data_Rx3[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Rx3[k] = ascii(((DataTx[i] >> 4) & 0X0F));
            k++;
            Data_Rx3[k] = ascii((DataTx[i] & 0X0F));
            k++;
            Data_Rx3[k] = ',';
            k++;
        }
        Data_Rx3[k]=0x00;        
    }
    
    return;
}

void wire_sale2(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3)
{
    uint8_t i;
    uint8_t j=3;
    uint8_t k=3;

    if(dta_emr3 == 1)
    {
        
        Data_Tx4[0]='4';
        Data_Tx4[1]=ascii(read_eeprom(pos2));
        Data_Tx4[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Tx4[j] = ascii(((DataTx[i] >> 4) & 0X0F));
            j++;
            Data_Tx4[j] = ascii((DataTx[i] & 0X0F));
            j++;
            Data_Tx4[j] = ',';
            j++;
        }
        Data_Tx4[j]=0x00;       
    }

    else if(dta_emr3 == 2)
    {
     
        Data_Rx4[0]='5';
        Data_Rx4[1]=ascii(read_eeprom(pos2));
        Data_Rx4[2]=':';

        for(i=0;i<Tx;i++)
        {
            Data_Rx4[k] = ascii(((DataTx[i] >> 4) & 0X0F));
            k++;
            Data_Rx4[k] = ascii((DataTx[i] & 0X0F));
            k++;
            Data_Rx4[k] = ',';
            k++;
        }
        Data_Rx4[k]=0x00;
    }
    
    return;
}

void log_wire(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("lwire.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "lwire.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &data_log ,strlen(data_log), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "lwire.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &data_log ,strlen(data_log), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    return;
}

void wire_ascci(uint8_t *DataRx, uint8_t Rx)
{
    uint8_t i;
    uint8_t j;

    for(i = 0; i < Rx; i++)
    {
        data_log[j] = ascii(((DataRx[i] >> 4) & 0X0F));
        j++;
        data_log[j] = ascii((DataRx[i] & 0X0F));
        j++;
        data_log[j] =',';
        j++;
    }

    data_log[j] = 0x00;

    write_eeprom(SD_logwire,1);

    return;

}

void log_emr3(void)
{

    uint32_t clusters2;
    FATFS *fsp2;

    FRESULT err2;
    FIL fil2;       /* File object */
    char line[50]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;

    fr2 = f_stat("lemr3.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:

            //existe

            /*
                ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */

            fr2 = f_open(&fil2, "lemr3.txt", FA_READ|FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,f_size(&fil2));
            //while(f_gets(line, sizeof(line), &fil2));

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &log_Tx ,strlen(log_Tx), &bw2);
            fr2 = f_write(&fil2, &log_Rx ,strlen(log_Rx), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);

            /* Close the file */
            f_close(&fil2);

        break;

        case FR_NO_FILE:

            fr2 = f_open(&fil2, "lemr3.txt", FA_CREATE_NEW | FA_WRITE);

            if (fr2)
            { 
                return (int)fr2;
            }

            fr2 = f_lseek(&fil2,0);

            fr2 = f_write(&fil2, &FECHA ,11, &bw2);
            fr2 = f_write(&fil2, &HORA ,9, &bw2);
            fr2 = f_write(&fil2, &log_Tx ,strlen(log_Tx), &bw2);
            fr2 = f_write(&fil2, &log_Rx ,strlen(log_Rx), &bw2);
            fr2 = f_write(&fil2, &ENTER_INIC ,2, &bw2);
            /* Close the file */
            f_close(&fil2);
      
        break;

    }

    return;
}

void emr3_log(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3)
{
    uint8_t i;
    uint8_t j=3;
    uint8_t k=3;

    while(true)
    {
        if(read_eeprom(SD_logemr3) == 0) break;
        chThdSleepMilliseconds(10);
    }

    if(dta_emr3 == 1)
    {
        
        log_Tx[0]='T';
        log_Tx[1]='x';
        log_Tx[2]=':';

        for(i=0;i<Tx;i++)
        {
            log_Tx[j] = ascii(((DataTx[i] >> 4) & 0X0F));
            j++;
            log_Tx[j] = ascii((DataTx[i] & 0X0F));
            j++;
            log_Tx[j] = ',';
            j++;
        }
        log_Tx[j]=0x00;
       
    }

    else if(dta_emr3 == 2)
    {
        

        log_Rx[0]='R';
        log_Rx[1]='x';
        log_Rx[2]=':';

        for(i=0;i<Tx;i++)
        {
            log_Rx[k] = ascii(((DataTx[i] >> 4) & 0X0F));
            k++;
            log_Rx[k] = ascii((DataTx[i] & 0X0F));
            k++;
            log_Rx[k] = ',';
            k++;
        }
        log_Rx[k]=0x00;        
        
        write_eeprom(SD_logemr3,0x01);
    }
    
    return;
}

void read_ppu(void)
{

    uint32_t clusters2;
    FATFS *fsp2;
    FRESULT err2;
    FIL fil2;       /* File object */
    char line[25]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;
    uint32_t T;

    fr2 = f_stat("ppu.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:
            //existe

            /*
            ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */            
            
            fr2 = f_open(&fil2, "ppu.txt", FA_READ);

            if (fr2)
            { 
                
                return (int)fr2;
            }

            f_gets(line, sizeof(line), &fil2);
            
            /* Close the file */
            f_close(&fil2);

            lect_ppu(&line,0);
            

        break;

        case FR_NO_FILE:

            
            lect_ppu(&line,1);           
             

        break;
    }
   return;
}

void write_ppu(void)
{

    uint32_t clusters2;
    FATFS *fsp2;
    FRESULT err2;
    FIL fil2;       /* File object */
    char line[25]; /* Line buffer */
    FRESULT fr2;    /* FatFs return code */
    uint8_t bw2;
    FILINFO fno;
    uint32_t T;

    fr2 = f_stat("ppu.txt", &fno);

    switch (fr2) 
    {

        case FR_OK:
            //existe

            /*
            ESCRRITURA EN LA SD DE LAS TRANSACIONES
            */            

            
            fr2 = f_open(&fil2, "ppu.txt", FA_WRITE);

            if (fr2)
            { 
                    
                return (int)fr2;
            }

            fr2 = f_write(&fil2, &LEVEL1_PPU ,2, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA1 ,6, &bw2);
            fr2 = f_write(&fil2, &LEVEL2_PPU ,2, &bw2);
            fr2 = f_write(&fil2, &PPU_VENTA2 ,6, &bw2);                
                
            /* Close the file */
            f_close(&fil2);
                   
            
        break;

        case FR_NO_FILE:

            
                fr2 = f_open(&fil2, "ppu.txt", FA_CREATE_NEW | FA_WRITE);

                if (fr2)
                { 
                    
                    return (int)fr2;
                }

                fr2 = f_write(&fil2, &LEVEL1_PPU ,2, &bw2);
                fr2 = f_write(&fil2, &PPU_VENTA1 ,6, &bw2);
                fr2 = f_write(&fil2, &LEVEL2_PPU ,2, &bw2);
                fr2 = f_write(&fil2, &PPU_VENTA2 ,6, &bw2);                
                
                /* Close the file */
                f_close(&fil2);
        break;
    }
   return;
}

void sd_preset(uint8_t tipo, uint8_t pos)
{
	
	if(pos == 1)
	{


		PRESET1[8] = ascii(tipo);

		if(tipo == 0)
		{
		    PRESET1[0] = ascii(9);
		    PRESET1[1] = ascii(0);
		    PRESET1[2] = ascii(0);

		    switch(dato_emr3[PD])
		    {
		        case 0:
		        PRESET1[3] = ascii(0);
		        PRESET1[4] = ascii(0);
		        PRESET1[5] = ascii(0);
		        PRESET1[6] = '.';
		        break;

		        case 1:
		        PRESET1[3] = ascii(0);
		        PRESET1[4] = ascii(0);
		        PRESET1[5] = '.';
		        PRESET1[6] = ascii(0);
		        break;

		        case 2:
		        PRESET1[3] = ascii(0);
		        PRESET1[4] = '.';
		        PRESET1[5] = ascii(0);
		        PRESET1[6] = ascii(0);
		        break;

		        case 3:
		        PRESET1[3] = '.';
		        PRESET1[4] = ascii(0);
		        PRESET1[5] = ascii(0);
		        PRESET1[6] = ascii(0);
		        break;
		    }
		}

		else if(tipo == 1)
		{
		    PRESET1[0] = ascii(read_eeprom(preset1_1));
		    PRESET1[1] = ascii(read_eeprom(preset1_2));
		    PRESET1[2] = ascii(read_eeprom(preset1_3));

		    switch(read_eeprom(PV))
		    {
		        case 0:
    		        PRESET1[3] = ascii(read_eeprom(preset1_4));
    		        PRESET1[4] = ascii(read_eeprom(preset1_5));
    		        PRESET1[5] = ascii(read_eeprom(preset1_6));
    		        PRESET1[6] = '.';
		        break;

		        case 1:
    		        PRESET1[3] = ascii(read_eeprom(preset1_4));
    		        PRESET1[4] = ascii(read_eeprom(preset1_5));
    		        PRESET1[5] = '.';
    		        PRESET1[6] = ascii(read_eeprom(preset1_6));
		        break;

		        case 2:
    		        PRESET1[3] = ascii(read_eeprom(preset1_4));
    		        PRESET1[4] = '.';
    		        PRESET1[5] = ascii(read_eeprom(preset1_5));
    		        PRESET1[6] = ascii(read_eeprom(preset1_6));
		        break;

		        case 3:
    		        PRESET1[3] = '.';
    		        PRESET1[4] = ascii(read_eeprom(preset1_4));
    		        PRESET1[5] = ascii(read_eeprom(preset1_5));
    		        PRESET1[6] = ascii(read_eeprom(preset1_6));
		        break;
		    }
		}

	    else if(tipo == 2)
	    {

		    PRESET1[0] = ascii(read_eeprom(preset1_1));
            PRESET1[1] = ascii(read_eeprom(preset1_2));
            PRESET1[2] = ascii(read_eeprom(preset1_3));

		    switch(read_eeprom(PD))
		     {
                case 0:
                    PRESET1[3] = ascii(read_eeprom(preset1_4));
                    PRESET1[4] = ascii(read_eeprom(preset1_5));
                    PRESET1[5] = ascii(read_eeprom(preset1_6));
                    PRESET1[6] = '.';
                break;

                case 1:
                    PRESET1[3] = ascii(read_eeprom(preset1_4));
                    PRESET1[4] = ascii(read_eeprom(preset1_5));
                    PRESET1[5] = '.';
                    PRESET1[6] = ascii(read_eeprom(preset1_6));
                break;

                case 2:
                    PRESET1[3] = ascii(read_eeprom(preset1_4));
                    PRESET1[4] = '.';
                    PRESET1[5] = ascii(read_eeprom(preset1_5));
                    PRESET1[6] = ascii(read_eeprom(preset1_6));
                break;

                case 3:
                    PRESET1[3] = '.';
                    PRESET1[4] = ascii(read_eeprom(preset1_4));
                    PRESET1[5] = ascii(read_eeprom(preset1_5));
                    PRESET1[6] = ascii(read_eeprom(preset1_6));
                break;
            }
		}
	}

    else if(pos == 2)
    {


        PRESET2[8] = ascii(tipo);

        if(tipo == 0)
        {
            PRESET2[0] = ascii(9);
            PRESET2[1] = ascii(0);
            PRESET2[2] = ascii(0);

            switch(dato_emr3[PD])
            {
                case 0:
                PRESET2[3] = ascii(0);
                PRESET2[4] = ascii(0);
                PRESET2[5] = ascii(0);
                PRESET2[6] = '.';
                break;

                case 1:
                PRESET2[3] = ascii(0);
                PRESET2[4] = ascii(0);
                PRESET2[5] = '.';
                PRESET2[6] = ascii(0);
                break;

                case 2:
                PRESET2[3] = ascii(0);
                PRESET2[4] = '.';
                PRESET2[5] = ascii(0);
                PRESET2[6] = ascii(0);
                break;

                case 3:
                PRESET2[3] = '.';
                PRESET2[4] = ascii(0);
                PRESET2[5] = ascii(0);
                PRESET2[6] = ascii(0);
                break;
            }
        }

        else if(tipo == 1)
        {
            PRESET2[0] = ascii(read_eeprom(preset2_1));
            PRESET2[1] = ascii(read_eeprom(preset2_2));
            PRESET2[2] = ascii(read_eeprom(preset2_3));

            switch(read_eeprom(PV))
            {
                case 0:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = ascii(read_eeprom(preset2_5));
                    PRESET2[5] = ascii(read_eeprom(preset2_6));
                    PRESET2[6] = '.';
                break;

                case 1:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = ascii(read_eeprom(preset2_5));
                    PRESET2[5] = '.';
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;

                case 2:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = '.';
                    PRESET2[5] = ascii(read_eeprom(preset2_5));
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;

                case 3:
                    PRESET2[3] = '.';
                    PRESET2[4] = ascii(read_eeprom(preset2_4));
                    PRESET2[5] = ascii(read_eeprom(preset2_5));
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;
            }
        }

        else if(tipo == 2)
        {

            PRESET2[0] = ascii(read_eeprom(preset2_1));
            PRESET2[1] = ascii(read_eeprom(preset2_2));
            PRESET2[2] = ascii(read_eeprom(preset2_3));

            switch(read_eeprom(PD))
             {
                case 0:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = ascii(read_eeprom(preset2_5));
                    PRESET2[5] = ascii(read_eeprom(preset2_6));
                    PRESET2[6] = '.';
                break;

                case 1:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = ascii(read_eeprom(preset2_5));
                    PRESET2[5] = '.';
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;

                case 2:
                    PRESET2[3] = ascii(read_eeprom(preset2_4));
                    PRESET2[4] = '.';
                    PRESET2[5] = ascii(read_eeprom(preset2_5));
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;

                case 3:
                    PRESET2[3] = '.';
                    PRESET2[4] = ascii(read_eeprom(preset2_4));
                    PRESET2[5] = ascii(read_eeprom(preset2_5));
                    PRESET2[6] = ascii(read_eeprom(preset2_6));
                break;
            }
        }
    }
}

void sd_volumen(uint8_t pos)
{
	if(pos == 1)
	{

		switch(read_eeprom(PV))
	    {
	        case 0:

	            VOL1[0] = ascii(read_eeprom(v1_1));
	            VOL1[1] = ascii(read_eeprom(v2_1));
	            VOL1[2] = ascii(read_eeprom(v3_1));
	            VOL1[3] = ascii(read_eeprom(v4_1));
	            VOL1[4] = ascii(read_eeprom(v5_1));
	            VOL1[5] = ascii(read_eeprom(v6_1));
	            VOL1[6] = '.';

	        break;

	        case 1:

	            VOL1[0] = ascii(read_eeprom(v1_1));
                VOL1[1] = ascii(read_eeprom(v2_1));
                VOL1[2] = ascii(read_eeprom(v3_1));
                VOL1[3] = ascii(read_eeprom(v4_1));
                VOL1[4] = ascii(read_eeprom(v5_1));
                VOL1[5] = '.';
                VOL1[6] = ascii(read_eeprom(v6_1));

	        break;

	        case 2:

	            VOL1[0] = ascii(read_eeprom(v1_1));
                VOL1[1] = ascii(read_eeprom(v2_1));
                VOL1[2] = ascii(read_eeprom(v3_1));
                VOL1[3] = ascii(read_eeprom(v4_1));
                VOL1[4] = '.';
                VOL1[5] = ascii(read_eeprom(v5_1));
                VOL1[6] = ascii(read_eeprom(v6_1));
	        break;

	        case 3:

	            VOL1[0] = ascii(read_eeprom(v1_1));
                VOL1[1] = ascii(read_eeprom(v2_1));
                VOL1[2] = ascii(read_eeprom(v3_1));
                VOL1[3] = '.';
                VOL1[4] = ascii(read_eeprom(v4_1));
                VOL1[5] = ascii(read_eeprom(v5_1));
                VOL1[6] = ascii(read_eeprom(v6_1));
	        break;
	    }
    }
	  
    else if(pos == 2)
    {

        switch(read_eeprom(PV))
        {
            case 0:

                VOL1[0] = ascii(read_eeprom(v1_2));
                VOL1[1] = ascii(read_eeprom(v2_2));
                VOL1[2] = ascii(read_eeprom(v3_2));
                VOL1[3] = ascii(read_eeprom(v4_2));
                VOL1[4] = ascii(read_eeprom(v5_2));
                VOL1[5] = ascii(read_eeprom(v6_2));
                VOL1[6] = '.';

            break;

            case 1:

                VOL1[0] = ascii(read_eeprom(v1_2));
                VOL1[1] = ascii(read_eeprom(v2_2));
                VOL1[2] = ascii(read_eeprom(v3_2));
                VOL1[3] = ascii(read_eeprom(v4_2));
                VOL1[4] = ascii(read_eeprom(v5_2));
                VOL1[5] = '.';
                VOL1[6] = ascii(read_eeprom(v6_2));

            break;

            case 2:

                VOL1[0] = ascii(read_eeprom(v1_2));
                VOL1[1] = ascii(read_eeprom(v2_2));
                VOL1[2] = ascii(read_eeprom(v3_2));
                VOL1[3] = ascii(read_eeprom(v4_2));
                VOL1[4] = '.';
                VOL1[5] = ascii(read_eeprom(v5_2));
                VOL1[6] = ascii(read_eeprom(v6_2));

            break;

            case 3:
            
                VOL1[0] = ascii(read_eeprom(v1_2));
                VOL1[1] = ascii(read_eeprom(v2_2));
                VOL1[2] = ascii(read_eeprom(v3_2));
                VOL1[3] = '.';
                VOL1[4] = ascii(read_eeprom(v4_2));
                VOL1[5] = ascii(read_eeprom(v5_2));
                VOL1[6] = ascii(read_eeprom(v6_2));

            break;
        }
    }
    
}
    
void sd_dinero(uint8_t pos)
{

    if(pos == 1)
    {
        switch(read_eeprom(PD))  
        {
            case 0:

                DINERO1[0] = ascii(read_eeprom(d1_1));
                DINERO1[1] = ascii(read_eeprom(d2_1));
                DINERO1[2] = ascii(read_eeprom(d3_1));
                DINERO1[3] = ascii(read_eeprom(d4_1));   
                DINERO1[4] = ascii(read_eeprom(d5_1));
                DINERO1[5] = ascii(read_eeprom(d6_1)); 
                DINERO1[6] = '.';

            break;

            case 1:

                DINERO1[0] = ascii(read_eeprom(d1_1));
                DINERO1[1] = ascii(read_eeprom(d2_1));
                DINERO1[2] = ascii(read_eeprom(d3_1));
                DINERO1[3] = ascii(read_eeprom(d4_1));   
                DINERO1[4] = ascii(read_eeprom(d5_1));
                DINERO1[5] = '.'; 
                DINERO1[6] = ascii(read_eeprom(d6_1)); 

            break;

            case 2:

                DINERO1[0] = ascii(read_eeprom(d1_1));
                DINERO1[1] = ascii(read_eeprom(d2_1));
                DINERO1[2] = ascii(read_eeprom(d3_1));
                DINERO1[3] = ascii(read_eeprom(d4_1));   
                DINERO1[4] = '.';
                DINERO1[5] = ascii(read_eeprom(d5_1));
                DINERO1[6] = ascii(read_eeprom(d6_1)); 

            break;

            case 3:
                       
                DINERO1[0] = ascii(read_eeprom(d1_1));
                DINERO1[1] = ascii(read_eeprom(d2_1));
                DINERO1[2] = ascii(read_eeprom(d3_1));
                DINERO1[3] = '.';  
                DINERO1[4] = ascii(read_eeprom(d4_1)); 
                DINERO1[5] = ascii(read_eeprom(d5_1));
                DINERO1[6] = ascii(read_eeprom(d6_1));   

            break;
        }
        return;
    }

    else if(pos == 2)
    {
        switch(read_eeprom(PD))  
        {
            case 0:

                DINERO2[0] = ascii(read_eeprom(d1_2));
                DINERO2[1] = ascii(read_eeprom(d2_2));
                DINERO2[2] = ascii(read_eeprom(d3_2));
                DINERO2[3] = ascii(read_eeprom(d4_2));   
                DINERO2[4] = ascii(read_eeprom(d5_2));
                DINERO2[5] = ascii(read_eeprom(d6_2)); 
                DINERO2[6] = '.';

            break;

            case 1:

                DINERO2[0] = ascii(read_eeprom(d1_2));
                DINERO2[1] = ascii(read_eeprom(d2_2));
                DINERO2[2] = ascii(read_eeprom(d3_2));
                DINERO2[3] = ascii(read_eeprom(d4_2));   
                DINERO2[4] = ascii(read_eeprom(d5_2));
                DINERO2[5] = '.'; 
                DINERO2[6] = ascii(read_eeprom(d6_2)); 

            break;

            case 2:

                DINERO2[0] = ascii(read_eeprom(d1_2));
                DINERO2[1] = ascii(read_eeprom(d2_2));
                DINERO2[2] = ascii(read_eeprom(d3_2));
                DINERO2[3] = ascii(read_eeprom(d4_2));   
                DINERO2[4] = '.';
                DINERO2[5] = ascii(read_eeprom(d5_2));
                DINERO2[6] = ascii(read_eeprom(d6_2)); 

            break;

            case 3:
                       
                DINERO2[0] = ascii(read_eeprom(d1_2));
                DINERO2[1] = ascii(read_eeprom(d2_2));
                DINERO2[2] = ascii(read_eeprom(d3_2));
                DINERO2[3] = '.';  
                DINERO2[4] = ascii(read_eeprom(d4_2)); 
                DINERO2[5] = ascii(read_eeprom(d5_2));
                DINERO2[6] = ascii(read_eeprom(d6_2));   

            break;
        }
        return;
    }
    return;
}

void sd_Tdinero(uint8_t pos)
{
    
    if(pos == 1)
    {

        T_DINERO1[0] = ascii(read_eeprom(td1_1));
        T_DINERO1[1] = ascii(read_eeprom(td1_2));
        T_DINERO1[2] = ascii(read_eeprom(td1_3));
        T_DINERO1[3] = ascii(read_eeprom(td1_4));
        T_DINERO1[4] = ascii(read_eeprom(td1_5));

        switch(read_eeprom(PD))
        {
            case 0:

                T_DINERO1[5] = ascii(read_eeprom(td1_6));
                T_DINERO1[6] = ascii(read_eeprom(td1_7));
                T_DINERO1[7] = ascii(read_eeprom(td1_8));
                T_DINERO1[8] = '.';

            break;

            case 1:

                T_DINERO1[5] = ascii(read_eeprom(td1_6));
                T_DINERO1[6] = ascii(read_eeprom(td1_7));
                T_DINERO1[7] = '.';
                T_DINERO1[8] = ascii(read_eeprom(td1_8));

            break;

            case 2:

                T_DINERO1[5] = ascii(read_eeprom(td1_6));
                T_DINERO1[6] = '.';
                T_DINERO1[7] = ascii(read_eeprom(td1_7));
                T_DINERO1[8] = ascii(read_eeprom(td1_8));

            break;

            case 3:

                T_DINERO1[5] = '.';
                T_DINERO1[6] = ascii(read_eeprom(td1_6));
                T_DINERO1[7] = ascii(read_eeprom(td1_7));
                T_DINERO1[8] = ascii(read_eeprom(td1_8));

            break;

        }

        return;
    }
    else if(pos == 2)
    {

        T_DINERO2[0] = ascii(read_eeprom(td2_1));
        T_DINERO2[1] = ascii(read_eeprom(td2_2));
        T_DINERO2[2] = ascii(read_eeprom(td2_3));
        T_DINERO2[3] = ascii(read_eeprom(td2_4));
        T_DINERO2[4] = ascii(read_eeprom(td2_5));

        switch(read_eeprom(PD))
        {
            case 0:

                T_DINERO2[5] = ascii(read_eeprom(td2_6));
                T_DINERO2[6] = ascii(read_eeprom(td2_7));
                T_DINERO2[7] = ascii(read_eeprom(td2_8));
                T_DINERO2[8] = '.';

            break;

            case 1:

                T_DINERO2[5] = ascii(read_eeprom(td2_6));
                T_DINERO2[6] = ascii(read_eeprom(td2_7));
                T_DINERO2[7] = '.';
                T_DINERO2[8] = ascii(read_eeprom(td2_8));

            break;

            case 2:

                T_DINERO2[5] = ascii(read_eeprom(td2_6));
                T_DINERO2[6] = '.';
                T_DINERO2[7] = ascii(read_eeprom(td2_7));
                T_DINERO2[8] = ascii(read_eeprom(td2_8));

            break;

            case 3:

                T_DINERO2[5] = '.';
                T_DINERO2[6] = ascii(read_eeprom(td2_6));
                T_DINERO2[7] = ascii(read_eeprom(td2_7));
                T_DINERO2[8] = ascii(read_eeprom(td2_8));

            break;
        }

        return;
    }

    return;

}

void sd_Tvolumen(uint8_t pos)
{
	
    if(pos == 1)
	{
		T_VOL1[0] = ascii(read_eeprom(tv1_1));
        T_VOL1[1] = ascii(read_eeprom(tv1_2));
        T_VOL1[2] = ascii(read_eeprom(tv1_3));
        T_VOL1[3] = ascii(read_eeprom(tv1_4));
        T_VOL1[4] = ascii(read_eeprom(tv1_5));
        T_VOL1[5] = ascii(read_eeprom(tv1_6));
        T_VOL1[6] = '.';
        T_VOL1[7] = ascii(read_eeprom(tv1_7));
        T_VOL1[8] = ascii(read_eeprom(tv1_8));

        return;
	}

    else if(pos == 2)
    {
        T_VOL2[0] = ascii(read_eeprom(tv2_1));
        T_VOL2[1] = ascii(read_eeprom(tv2_2));
        T_VOL2[2] = ascii(read_eeprom(tv2_3));
        T_VOL2[3] = ascii(read_eeprom(tv2_4));
        T_VOL2[4] = ascii(read_eeprom(tv2_5));
        T_VOL2[5] = ascii(read_eeprom(tv2_6));
        T_VOL2[6] = '.';
        T_VOL2[7] = ascii(read_eeprom(tv2_7));
        T_VOL2[8] = ascii(read_eeprom(tv2_8));

        return;
    }
}

void sd_ppu(uint8_t niv)
{
    if(niv == 1)
    {
        switch(read_eeprom(pppu))
        {
            case 0: 
                PPU_LEVEL1[0] = ascii(read_eeprom(ppu1_level1));
                PPU_LEVEL1[1] = ascii(read_eeprom(ppu2_level1));
                PPU_LEVEL1[2] = ascii(read_eeprom(ppu3_level1));
                PPU_LEVEL1[3] = ascii(read_eeprom(ppu4_level1));
                PPU_LEVEL1[4] = '.';
                
            break;

            case 1: 
                PPU_LEVEL1[0] = ascii(read_eeprom(ppu1_level1));
                PPU_LEVEL1[1] = ascii(read_eeprom(ppu2_level1));
                PPU_LEVEL1[2] = ascii(read_eeprom(ppu3_level1));
                PPU_LEVEL1[3] = '.';
                PPU_LEVEL1[4] = ascii(read_eeprom(ppu4_level1));
                
            break;

            case 2: 
                PPU_LEVEL1[0] = ascii(read_eeprom(ppu1_level1));
                PPU_LEVEL1[1] = ascii(read_eeprom(ppu2_level1));
                PPU_LEVEL1[2] = '.';
                PPU_LEVEL1[3] = ascii(read_eeprom(ppu3_level1));
                PPU_LEVEL1[4] = ascii(read_eeprom(ppu4_level1));
                
            break;

            case 3: 
                PPU_LEVEL1[0] = ascii(read_eeprom(ppu1_level1));
                PPU_LEVEL1[1] = '.';
                PPU_LEVEL1[2] = ascii(read_eeprom(ppu2_level1));
                PPU_LEVEL1[3] = ascii(read_eeprom(ppu3_level1));
                PPU_LEVEL1[4] = ascii(read_eeprom(ppu4_level1));
                
            break;
        }

        return;
    }

    else if(niv == 2)
    {
        switch(read_eeprom(pppu))
        {
            case 0: 
                PPU_LEVEL2[0] = ascii(read_eeprom(ppu1_level2));
                PPU_LEVEL2[1] = ascii(read_eeprom(ppu2_level2));
                PPU_LEVEL2[2] = ascii(read_eeprom(ppu3_level2));
                PPU_LEVEL2[3] = ascii(read_eeprom(ppu4_level2));
                PPU_LEVEL2[4] = '.';
                
            break;

            case 1: 
                PPU_LEVEL2[0] = ascii(read_eeprom(ppu1_level2));
                PPU_LEVEL2[1] = ascii(read_eeprom(ppu2_level2));
                PPU_LEVEL2[2] = ascii(read_eeprom(ppu3_level2));
                PPU_LEVEL2[3] = '.';
                PPU_LEVEL2[4] = ascii(read_eeprom(ppu4_level2));
                
            break;

            case 2: 
                PPU_LEVEL2[0] = ascii(read_eeprom(ppu1_level2));
                PPU_LEVEL2[1] = ascii(read_eeprom(ppu2_level2));
                PPU_LEVEL2[2] = '.';
                PPU_LEVEL2[3] = ascii(read_eeprom(ppu3_level2));
                PPU_LEVEL2[4] = ascii(read_eeprom(ppu4_level2));
                
            break;

            case 3: 
                PPU_LEVEL2[0] = ascii(read_eeprom(ppu1_level2));
                PPU_LEVEL2[1] = '.';
                PPU_LEVEL2[2] = ascii(read_eeprom(ppu2_level2));
                PPU_LEVEL2[3] = ascii(read_eeprom(ppu3_level2));
                PPU_LEVEL2[4] = ascii(read_eeprom(ppu4_level2));
                
            break;
        }

        return;
    }

    return;
}

void sd_venta(uint8_t pos)
{
    if(pos == 1)
    {

        switch(read_eeprom(pppu))
        {
            case 0: 

                LEVEL1_PPU[0] = ascii((read_eeprom(level_1) & 0x0F));
                LEVEL1_PPU[1] = ';';

                PPU_VENTA1[0] = ascii(read_eeprom(ppu1_venta1));
                PPU_VENTA1[1] = ascii(read_eeprom(ppu2_venta1));
                PPU_VENTA1[2] = ascii(read_eeprom(ppu3_venta1));
                PPU_VENTA1[3] = ascii(read_eeprom(ppu4_venta1));
                PPU_VENTA1[4] = '.';
                
            break;

            case 1: 

                LEVEL1_PPU[0] = ascii((read_eeprom(level_1) & 0x0F));
                LEVEL1_PPU[1] = ';';

                PPU_VENTA1[0] = ascii(read_eeprom(ppu1_venta1));
                PPU_VENTA1[1] = ascii(read_eeprom(ppu2_venta1));
                PPU_VENTA1[2] = ascii(read_eeprom(ppu3_venta1));
                PPU_VENTA1[3] = '.';
                PPU_VENTA1[4] = ascii(read_eeprom(ppu4_venta1));
                
            break;

            case 2: 

                LEVEL1_PPU[0] = ascii((read_eeprom(level_1) & 0x0F));
                LEVEL1_PPU[1] = ';';

                PPU_VENTA1[0] = ascii(read_eeprom(ppu1_venta1));
                PPU_VENTA1[1] = ascii(read_eeprom(ppu2_venta1));
                PPU_VENTA1[2] = '.';
                PPU_VENTA1[3] = ascii(read_eeprom(ppu3_venta1));
                PPU_VENTA1[4] = ascii(read_eeprom(ppu4_venta1));
                
            break;

            case 3: 

                LEVEL1_PPU[0] = ascii((read_eeprom(level_1) & 0x0F));
                LEVEL1_PPU[1] = ';';

                PPU_VENTA1[0] = ascii(read_eeprom(ppu1_venta1));
                PPU_VENTA1[1] = '.';
                PPU_VENTA1[2] = ascii(read_eeprom(ppu2_venta1));
                PPU_VENTA1[3] = ascii(read_eeprom(ppu3_venta1));
                PPU_VENTA1[4] = ascii(read_eeprom(ppu4_venta1));
                
            break;
        }

        return;
    }

    else if(pos == 2)
    {
        switch(read_eeprom(pppu))
        {
            case 0: 

                LEVEL2_PPU[0] = ascii((read_eeprom(level_2) & 0x0F));
                LEVEL2_PPU[1] = ';';

                PPU_VENTA2[0] = ascii(read_eeprom(ppu1_venta2));
                PPU_VENTA2[1] = ascii(read_eeprom(ppu2_venta2));
                PPU_VENTA2[2] = ascii(read_eeprom(ppu3_venta2));
                PPU_VENTA2[3] = ascii(read_eeprom(ppu4_venta2));
                PPU_VENTA2[4] = '.';
                
            break;

            case 1: 

                LEVEL2_PPU[0] = ascii((read_eeprom(level_2) & 0x0F));
                LEVEL2_PPU[1] = ';';

                PPU_VENTA2[0] = ascii(read_eeprom(ppu1_venta2));
                PPU_VENTA2[1] = ascii(read_eeprom(ppu2_venta2));
                PPU_VENTA2[2] = ascii(read_eeprom(ppu3_venta2));
                PPU_VENTA2[3] = '.';
                PPU_VENTA2[4] = ascii(read_eeprom(ppu4_venta2));
                
            break;

            case 2: 

                LEVEL2_PPU[0] = ascii((read_eeprom(level_2) & 0x0F));
                LEVEL2_PPU[1] = ';';

                PPU_VENTA2[0] = ascii(read_eeprom(ppu1_venta2));
                PPU_VENTA2[1] = ascii(read_eeprom(ppu2_venta2));
                PPU_VENTA2[2] = '.';
                PPU_VENTA2[3] = ascii(read_eeprom(ppu3_venta2));
                PPU_VENTA2[4] = ascii(read_eeprom(ppu4_venta2));
                
            break;

            case 3: 

                LEVEL2_PPU[0] = ascii((read_eeprom(level_2) & 0x0F));
                LEVEL2_PPU[1] = ';';

                PPU_VENTA2[0] = ascii(read_eeprom(ppu1_venta2));
                PPU_VENTA2[1] = '.';
                PPU_VENTA2[2] = ascii(read_eeprom(ppu2_venta2));
                PPU_VENTA2[3] = ascii(read_eeprom(ppu3_venta2));
                PPU_VENTA2[4] = ascii(read_eeprom(ppu4_venta2));
                
            break;
        }

        return;
    }

    return;
}

void sd_hora(uint8_t *recibo)
{

    HORA[0] = ascii(recibo[5] / 10);
    HORA[1] = ascii(recibo[5] % 10);

    HORA[3] = ascii(recibo[6] / 10);
    HORA[4] = ascii(recibo[6] % 10);

    HORA[6] = ascii(recibo[7] / 10);
    HORA[7] = ascii(recibo[7] % 10);

    return;
}

void sd_dia(uint8_t *recibo)
{
	
    FECHA[0] = ascii(recibo[8] / 10);
    FECHA[1] = ascii(recibo[8] % 10);

    FECHA[3] = ascii(recibo[7] / 10);
    FECHA[4] = ascii(recibo[7] % 10);

    FECHA[8] = ascii(recibo[6] / 10);
    FECHA[9] = ascii(recibo[6] % 10);

    return;
}

void sd_punto(void)
{
    PPU_POINT[0]=ascii(read_eeprom(pppu));
    MON_POINT[0]=ascii(read_eeprom(PD));
    VOL_POINT[0]=ascii(read_eeprom(PV));
    return;
}

void Status_1(uint8_t est)
{
    Status1[0]=ascii(read_eeprom(consola));
    Status1[2]=ascii(est);
    Status1[3]=ascii(read_eeprom(pos1));
    return;
}

void Status_2(uint8_t est)
{
    Status2[0]=ascii(est);
    Status2[1]=ascii(read_eeprom(pos2));
    return;
}
