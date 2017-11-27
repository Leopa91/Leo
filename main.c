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

#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "Emr3V210.h"
#include "serial.h"
#include "ff.h"
#include "diskio.h"
#include "mmc_spi.h"
#include "ascii.h"
#include "EepromVirtual.h"
#include "ValidarEmr3.h"
#include "ConfInicial.h"
#include "ConfEmr3.h"
#include "LectureSw.h"
#include "ButtonEmr3.h"
#include "DataSd.h"
#include "RxWire.h"

//Configuracion del puerto serial a 8 bit, paridad ever, 1 bit stop
#define USART_CR1_9BIT_WORD     (1 << 12)   /* CR1 9 bit word */
#define USART_CR1_PARITY_SET    (1 << 10)   /* CR1 Parity bit enable */
#define USART_CR1_EVEN_PARITY   (0 << 9) /* CR1 even parity */


#define POLLING_INTERVAL                10
#define POLLING_DELAY                   10

static unsigned cnt;

static virtual_timer_t tmr;

static event_source_t inserted_event, removed_event;

static void tmrfunc(void *p) 
{
    BaseBlockDevice *bbdp = p;

    chSysLockFromISR();
    if (cnt > 0) 
    {
        if (blkIsInserted(bbdp)) 
        {
            if (--cnt == 0)
            {
                chEvtBroadcastI(&inserted_event);
            }
        }
        else
        cnt = POLLING_INTERVAL;
    }
    
    else 
    {
        if (!blkIsInserted(bbdp)) 
        {
            cnt = POLLING_INTERVAL;
            chEvtBroadcastI(&removed_event);
        }
    }
    
    chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, bbdp);
    chSysUnlockFromISR();
}

static void tmr_init(void *p) 
{

    chEvtObjectInit(&inserted_event);
    chEvtObjectInit(&removed_event);
    chSysLock();
    cnt = POLLING_INTERVAL;
    chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, p);
    chSysUnlock();
}

static const WDGConfig wdgcfg = {
    STM32_IWDG_PR_64,
    STM32_IWDG_RL(5000)};

static void rxend(UARTDriver *uartp) 
{

   	(void)uartp;
   	write_eeprom(Flag1,0x01);
}

static UARTConfig uart_cfg_2= 
{
   0,
   0,
   rxend,
   0,
   0,
   9600,
   0,
   USART_CR2_LINEN,
   0
};

static SerialConfig uart_cfg_1= 
{
	5787,
   	USART_CR1_9BIT_WORD | USART_CR1_PARITY_SET | USART_CR1_EVEN_PARITY,//0x0A00,//101000000000,
   	USART_CR2_LINEN,
  	0
};

//TAREA PARA CONTROLAR LA EMR3 
static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) 
{

    (void)arg;
    chRegSetThreadName("LECTURA");
    while (true) 
    {
        LectureSw();
    }
}

//TAREA ENCARGADA DE LAZO DE CORRIENTE O PROTOCOLO GILBARCO
static THD_WORKING_AREA(SD, 256);
static THD_FUNCTION(SD_thread_2, p) 
{

   	(void)p;
   	chRegSetThreadName("WIRE");
   	uint8_t dato_wire;
   	uint8_t i;

   	while (true) 
   	{

      	if(read_eeprom(consola) == 0x02 || read_eeprom(consola) == 0x04)
      	{
         	dato_wire = RxWire();
		}
 
        else 
        {
            chThdSleepMilliseconds(10);
        }
        
        chThdSleepMilliseconds(10);
   	}
}

//TAREA ENCARGADA DE VISUALIZAR SI EL MICRO FUNCIONAMIENTO
static THD_WORKING_AREA(LED, 128);
static THD_FUNCTION(LED_thread_3, q) 
{

   	(void)q;
   	chRegSetThreadName("LED");

   	while (true) 
   	{
      	palClearPad(GPIOE, 0);
      	chThdSleepMilliseconds(500);
      	palSetPad(GPIOE, 0);
      	chThdSleepMilliseconds(500);
        wdgReset(&WDGD1);                 
   	}
}

int main (void)
{
    halInit();
    chSysInit();
    uint8_t aux_punto;
    uint8_t recibo[20];

    static thread_t *shelltp = NULL;
   
    event_listener_t el0, el1;

    palSetPadMode(GPIOE, 0,PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOE, 0);

    palSetPadMode(GPIOE, 1,PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOE, 1);

    palSetPadMode(GPIOE, 2,PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOE, 2);

    palSetPadMode(GPIOE, 3,PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOE, 3);

    palSetPadMode(GPIOC, 0,PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOC, 0);

    uint8_t D_T1;
    uint8_t i;

    LCDInit();
    LCDBegin(16,2);
    
    //SPI

    /* Inicializa SPI */ 
    palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);// | PAL_STM32_OSPEED_HIGHEST);          // NSS
    palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // SCK
    palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // MISO
    palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // MOSI
    palSetPad(GPIOA, 4);

    chThdSleepMilliseconds(100);  

    chEvtRegister(&inserted_event, &el0, 0);
    chEvtRegister(&removed_event, &el1, 1);

    //Congi
    palSetPadMode(GPIOD, IN_1, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOD, IN_2, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOD, IN_3, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, IN_4, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, IN_5, PAL_MODE_INPUT_PULLDOWN);

    //x100
    palSetPadMode(GPIOC, IN_6, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOC, IN_7, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOD, IN_8, PAL_MODE_INPUT_PULLDOWN);

    palSetPadMode(GPIOB, BUT_1, PAL_MODE_INPUT_PULLDOWN);//+
    palSetPadMode(GPIOE, BUT_2, PAL_MODE_INPUT_PULLDOWN);//-

    palSetPadMode(GPIOA, 11, PAL_MODE_OUTPUT_PUSHPULL);//confi EMR3_1
    palSetPadMode(GPIOA, PIN_CONF_B, PAL_MODE_OUTPUT_PUSHPULL);//confi EMR3_2

    //CONFIGURACION DEL PUERTO SERIAL DE EMR3
    uartStart(&UARTD1,&uart_cfg_2);     
    
    //PINES DEL TX Y RX DEL SERIAL DE LA EMR3
    palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));

    uartStopReceive(&UARTD1);
    uartStopSend(&UARTD1);

    //Configuracion WDG
    wdgStart(&WDGD1, &wdgcfg);

    //INICIAR LA TAREA ENCARGADA DE VISUALIZAR SI EL MICRO FUNCIONAMIENTO
    chThdCreateStatic(LED, sizeof(LED),NORMALPRIO , LED_thread_3, NULL);

    //Mensaje
    LCDPrintRow(0,"SISTEMA  INSEPET");
    LCDPrintRow2(0,"www.insepet.com ");
    
    chThdSleepMilliseconds(1000);
 
    LCDPrintRow( 0," VERSION 2.2.E  ");
    LCDPrintRow2( 0,"  Emr3 to Gil   ");

    chThdSleepMilliseconds(1000);
       
    LCDPrintRow( 0,"    microSD:    ");
    LCDPrintRow2( 0,"                ");
   
    //DETECTAR SI LA SD ESTA PUESTA 
    memori();
  
    //Determinar el tipo de consola
    if(read_eeprom(consola) == 0x01 || read_eeprom(consola) == 0x02 || read_eeprom(consola) == 0x04)
    {  
      
        if(read_eeprom(consola) == 0x02 || read_eeprom(consola) == 0x04) 
        {
            palClearPad(GPIOE, 2);  // emr3 on
            palClearPad(GPIOE, 3);  // lc on
        }

        else if(read_eeprom(consola) == 0x01) 
        {
            palClearPad(GPIOE, 2);  //  emr3 on
            palSetPad(GPIOE, 3);    //  lc off
        }

        //ENCARGADO DE SABER CUANTA CABEZAS ESTAN ACTIVAS  
        LCDPrintRow( 0,"   TEST_EMR3    ");
        ValidarEmr3();      
        chThdSleepMilliseconds(1000);

        //ENCARGADO DE LEER PUNTOS DECIMALES
        LCDPrintRow( 0,"   TEST POINS   ");
        LCDPrintRow2( 0,"                ");
        while(true)
        {
            aux_punto = ConfEmr3();
            if(aux_punto == 0x01) break; 
            LCDPrintRow2(0,"   Error=0007   ");
        }
        LCDPrintRow2( 0,"    OK POINS    ");
 
        //LEER LA HORA
        ButtonEmr(HORAS,1);
        uartStartReceive(&UARTD1, 10, recibo);
        chThdSleepMilliseconds(500);
        uartStopReceive(&UARTD1);
        sd_hora(&recibo);

        chThdSleepMilliseconds(1000);

        //LEER LAFECHA
        ButtonEmr(FECHAS,1);
        uartStartReceive(&UARTD1, 11, recibo);
        chThdSleepMilliseconds(500);
        uartStopReceive(&UARTD1);
        sd_dia(&recibo);

        // LEEMOS LOS ULTIMOS TOTALES EN VOLUMEN DE CADA CABEZA EXISTENTE
        LCDPrintRow( 0," READS TOTALES  ");
        LCDPrintRow2( 0,"                ");

        LeerTotales(1);
        lcd_out [0] = ' ';
        lcd_out [1] = 'T';
        lcd_out [2] = 'O';
        lcd_out [3] = 'T';
        lcd_out [4] = '1';
        lcd_out [5] = '=';
        lcd_out [6] = ascii(read_eeprom(tv1_1));
        lcd_out [7] = ascii(read_eeprom(tv1_2));
        lcd_out [8] = ascii(read_eeprom(tv1_3));
        lcd_out [9] = ascii(read_eeprom(tv1_4));
        lcd_out [10] = ascii(read_eeprom(tv1_5));
        lcd_out [11] = ascii(read_eeprom(tv1_6));
        lcd_out [12] = '.';
        lcd_out [13] = ascii(read_eeprom(tv1_7));
        lcd_out [14] = ascii(read_eeprom(tv1_8));
        lcd_out [15] = ' ';
        
        
        if(read_eeprom(lado2) == 0x01)
        {
            LCDPrintRow( 0,lcd_out);
            LeerTotales(2);
            lcd_out [0] = ' ';
	        lcd_out [1] = 'T';
	        lcd_out [2] = 'O';
	        lcd_out [3] = 'T';
	        lcd_out [4] = '2';
	        lcd_out [5] = '=';
	        lcd_out [6] = ascii(read_eeprom(tv2_1));
	        lcd_out [7] = ascii(read_eeprom(tv2_2));
	        lcd_out [8] = ascii(read_eeprom(tv2_3));
	        lcd_out [9] = ascii(read_eeprom(tv2_4));
	        lcd_out [10] = ascii(read_eeprom(tv2_5));
	        lcd_out [11] = ascii(read_eeprom(tv2_6));
	        lcd_out [12] = '.';
	        lcd_out [13] = ascii(read_eeprom(tv2_7));
	        lcd_out [14] = ascii(read_eeprom(tv2_8));
	        lcd_out [15] = ' ';


            LCDPrintRow2( 0,lcd_out);
        } 

        else
        {
            LCDPrintRow2( 0,lcd_out);
        }

        chThdSleepMilliseconds(1000);

        //LEER EL PPU DE LAS VENTA. 
        
        read_ppu(); 

        
        
    }

   	else
   	{
              
        palSetPad(GPIOE, 2);    // emr3 off
        palSetPad(GPIOE, 3);    //lc off
        LCDPrintRow(0,"   CONSOLE 03   ");
        LCDPrintRow2(0,"    DISABLED    ");
        
        ButtonEmr(SALIR_CONSALA,1);
        data_recibo(0X07);

        chThdSleepMilliseconds(500);

        ButtonEmr(SALIR_CONSALA,2);
        data_recibo(0X07);
        
        chThdSleepMilliseconds(500);

    }
 
    //CONFIGURAR EL PUERTO SERAL PARA EL LAZO DE CORIENTE O PROTOCOLO GILBARCO
    sdStart(&SD2,&uart_cfg_1);
    //PINES DE TX Y RX DEL PUERTO SERIAL DEL LAZO DE CORRIENTE O PROTOCOLO GILBARCO
    palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

    chThdSleepMilliseconds(500);

    //INICIO DE LA TAREA PARA CONTROLAR LA EMR3 
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    //INICIO DE LA TAREA ENCARGADA DE LAZO DE CORRIENTE O PROTOCOLO GILBARCO
    chThdCreateStatic(SD, sizeof(SD),NORMALPRIO , SD_thread_2, NULL);

    write_eeprom(SD_CONF,0x01);          //BANDERA PARA SABER QUE ESTA LISTO LOS VECTORES PARA ESCRIBIR LA CONFIGURACION EN LA SD
    
    while (TRUE)
    {
        
        //ESCRIBIR EN EL ARCHIVO DE CONFIGURACION
        if (read_eeprom(SD_CONF) == 0x01)
        {
            Write();
            write_eeprom(SD_CONF,0x00);
        }

        //ESCRIBIR UNA EVENTUALIDAD EN EL WIRE.
        if (read_eeprom(SD_logwire) == 1)
        {
            log_wire();
            write_eeprom(SD_logwire,0);
        }

        //ESCRIBIR EL LOG DE LA EMR3.
        if (read_eeprom(SD_logemr3) == 1)
        {
            log_emr3();
            write_eeprom(SD_logemr3,0);
        }        

        //TRANSACIONES REALIZADA POR LA CABEZA 1
        if (read_eeprom(SD_TRANS) == 0X01)
        {
            trans();
            write_eeprom(SD_TRANS,0X00);
        }

        //DATOS DE VENTA OBTENIDA POR LA EMR3 1
        else if (read_eeprom(SD_emr3_sale1) == 0X01)
        {
            sale1_emr3();
            write_eeprom(SD_emr3_sale1,0X00);
        }

        //DATOS ENVIADO POR LAZO DE CORRIENTE 1
        else if (read_eeprom(SD_wire_sale1) == 0X01)
        {
            sale1_wire();
            write_eeprom(SD_wire_sale1,0X00);
        }

        //TRANSACIONES REALIZADA POR LA CABEZA 2        
        if (read_eeprom(SD_TRANS2) == 0X01)
        {
            trans2();
            write_eeprom(SD_TRANS2,0X00);
        }

        //DATOS DE VENTA OBTENIDA POR LA EMR3 2
        else if (read_eeprom(SD_emr3_sale2) == 0X01)
        {
            sale2_emr3();
            write_eeprom(SD_emr3_sale2,0X00);
        }

        //DATOS ENVIADO POR LAZO DE CORRIENTE 2
        else if (read_eeprom(SD_wire_sale2) == 0X01)
        {
            sale2_wire();
            write_eeprom(SD_wire_sale2,0X00);
        }
      
        //ESCRIBIR EL VALOR DEL PPU DE LA VENTA
        if(read_eeprom(venta_sd) == 0x01)
        {
            write_ppu();
            write_eeprom(venta_sd,0x00);

        }        
        
        chThdSleepMilliseconds(10);            
    }
    return;
}
