/*

config.txt

organizacion del archivo: posicion1,posicion2,PuntoPPU,PuntoDinero,PuntoVol,PPUlevel1, PPUlevel2,totalDinero,totalVolumen

transactions.txt

organizacion del archivo: Fecha,hora,PPU,Posicion,ultimaVentaDinero,ultimaVentaVolumen,totalDinero,totalVolumen

*/




#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"
#include "shell.h"

#include "ff.h"

#include "usbcfg.h"

FATFS MMC_FS;
MMCDriver MMCD1;

#define POLLING_INTERVAL                10
#define POLLING_DELAY                   10


/**
 * @brief   Debounce counter.
 */
static unsigned cnt;


/**
 * @brief   Card monitor timer.
 */
static virtual_timer_t tmr;


static event_source_t inserted_event, removed_event;

/********************************************* Micro SD Struct ************************************/
static const SPIConfig hs_spicfg = { 
  NULL, 
  GPIOC, 
  5, 
  SPI_CR1_BR_2 | SPI_CR1_BR_1 //0 
};


static const SPIConfig ls_spicfg = { 
  NULL, 
  GPIOC, 
  5, 
  SPI_CR1_BR_2 | SPI_CR1_BR_1 
};

static MMCConfig mmccfg = {&SPID2, &ls_spicfg, &hs_spicfg};

/********************************************* Thread Blink LED ***********************************/
/*
 * This is a thread of the Blink
 * 
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) 
{

  (void)arg;
  chRegSetThreadName("Blink");
  


  while (true) 
  {
 
  	palSetPad(GPIOD, 15);       /* Green LED  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, 15);     /* Green LED  */
    chThdSleepMilliseconds(500);
	
  }
}

// /*
//  * Serial 2 driver configuration structure LOOP CURRENT PASIVE
//  */
// static SerialConfig sdcfg = {
//     115200,                                
//     0,
//     USART_CR2_LINEN,
//     0
// };

/**
 * @brief   Insertion monitor timer callback function.
 *
 * @param[in] p         pointer to the @p BaseBlockDevice object
 *
 * @notapi
 */
static void tmrfunc(void *p) {
  BaseBlockDevice *bbdp = p;

  chSysLockFromISR();
  if (cnt > 0) {
    if (blkIsInserted(bbdp)) {
      if (--cnt == 0) {
        chEvtBroadcastI(&inserted_event);
      }
    }
    else
      cnt = POLLING_INTERVAL;
  }
  else {
    if (!blkIsInserted(bbdp)) {
      cnt = POLLING_INTERVAL;
      chEvtBroadcastI(&removed_event);
    }
  }
  chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, bbdp);
  chSysUnlockFromISR();
}

/**
 * @brief   Polling monitor start.
 *
 * @param[in] p         pointer to an object implementing @p BaseBlockDevice
 *
 * @notapi
 */
static void tmr_init(void *p) {

  chEvtObjectInit(&inserted_event);
  chEvtObjectInit(&removed_event);
  chSysLock();
  cnt = POLLING_INTERVAL;
  chVTSetI(&tmr, MS2ST(POLLING_DELAY), tmrfunc, p);
  chSysUnlock();
}

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

static FRESULT scan_files(BaseSequentialStream *chp, char *path) {
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
  if (res == FR_OK) {
    i = strlen(path);
    for (;;) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0)
        break;
      if (fno.fname[0] == '.')
        continue;
      fn = fno.fname;
      if (fno.fattrib & AM_DIR) {
        path[i++] = '/';
        strcpy(&path[i], fn);
        res = scan_files(chp, path);
        if (res != FR_OK)
          break;
        path[--i] = 0;
      }
      else {
        chprintf(chp, "%s/%s\r\n", path, fn);
      }
    }
  }
  return res;
}

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
#define TEST_WA_SIZE    THD_WORKING_AREA_SIZE(256)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state time\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
            states[tp->p_state]);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriorityX(),
                           TestThread, chp);
  if (tp == NULL) {
    chprintf(chp, "out of memory\r\n");
    return;
  }
  chThdWait(tp);
}

static void cmd_tree(BaseSequentialStream *chp, int argc, char *argv[]) {
  FRESULT err;
  uint32_t clusters;
  FATFS *fsp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: tree\r\n");
    return;
  }
  if (!fs_ready) {
    chprintf(chp, "File System not mounted\r\n");
    return;
  }
  err = f_getfree("/", &clusters, &fsp);
  if (err != FR_OK) {
    chprintf(chp, "FS: f_getfree() failed\r\n");
    return;
  }
  chprintf(chp,
           "FS: %lu free clusters, %lu sectors per cluster, %lu bytes free\r\n",
           clusters, (uint32_t)SDC_FS.csize,
           clusters * (uint32_t)SDC_FS.csize * (uint32_t)MMCSD_BLOCK_SIZE);
  fbuff[0] = 0;
  scan_files(chp, (char *)fbuff);
}

static void cmd_read(BaseSequentialStream *chp, int argc, char *argv[]) 
{
  FRESULT err;
  uint32_t clusters;
  FATFS *fsp;
  FIL fil;       /* File object */
  char line[82]; /* Line buffer */
  FRESULT fr;    /* FatFs return code */

  (void)argv;
  
  if (argc > 0) 
  {
    chprintf(chp, "Usage: read\r\n");
    return;
  }
  
  if (!fs_ready) 
  {
    chprintf(chp, "File System not mounted\r\n");
    return;
  }
  
  err = f_getfree("/", &clusters, &fsp);
  
  if (err != FR_OK) 
  {
    chprintf(chp, "FS: f_getfree() failed\r\n");
    return;
  }

  chprintf(chp, "Abriendo Archivo....\r\n");
  /* Open a text file */
  fr = f_open(&fil, "config.txt", FA_READ);
  
  if (fr)
  { 
    chprintf(chp, "No se pudo abrir el archivo....\r\n");
    return (int)fr;

  }


  /* Read last lines and display it */
  f_gets(line, sizeof line, &fil);
  chprintf(chp, "%s \r\n", line);
  chprintf(chp, "Cerrando Archivo....\r\n");

  /* Close the file */
  f_close(&fil);

}

static void cmd_write(BaseSequentialStream *chp, int argc, char *argv[]) 
{
  FRESULT err;
  uint32_t clusters;
  FATFS *fsp;
  FIL fil;       /* File object */
  char line[82]; /* Line buffer */
  FRESULT fr;    /* FatFs return code */
  uint8_t bw;

  (void)argv;
  
  if (argc > 0) 
  {
    chprintf(chp, "Usage: read\r\n");
    return;
  }
  
  if (!fs_ready) 
  {
    chprintf(chp, "File System not mounted\r\n");
    return;
  }
  
  err = f_getfree("/", &clusters, &fsp);
  
  if (err != FR_OK) 
  {
    chprintf(chp, "FS: f_getfree() failed\r\n");
    return;
  }

  line[0] = 'C';
  line[1] = 'o';
  line[2] = 'n';
  line[3] = 'f';
  line[4] = 'i';
  line[5] = 'g';
  line[6] = ' ';
  line[7] = 'n';
  line[8] = 'e';
  line[9] = 'w';
  line[10] = '\r\n';

  chprintf(chp, "Abriendo Archivo....\r\n");
  /* Open a text file */
  fr = f_open(&fil, "config.txt", FA_WRITE);
  
  if (fr)
  { 
    chprintf(chp, "No se pudo abrir el archivo....\r\n");
    return (int)fr;

  }

  fr = f_write(&fil, &line ,strlen(line), &bw);

  /* Close the file */
  f_close(&fil);

  chprintf(chp, "Archivo Modificado....\r\n");

}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {"tree", cmd_tree},
  {"read", cmd_read},
  {"write", cmd_write},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

// MMC card insertion event
static void InsertHandler(eventid_t id)
{
  FRESULT err;

  (void)id;

  /*
   * On insertion MMC initialization and FS mount.
   */

  if (mmcConnect(&MMCD1)) 
  {
    return;
  }

  err = f_mount(&MMC_FS, "/", 1);
  
  if (err != FR_OK) 
  {
    mmcDisconnect(&MMCD1);
    return;
  }
  
  palSetPad(GPIOD, 12);
  fs_ready = TRUE;
}

// MMC card removal event
static void RemoveHandler(eventid_t id)
{
  (void) id;
  mmcDisconnect(&MMCD1);
  fs_ready = FALSE;

}



int main(void) 
{

  halInit();
  chSysInit();

  static thread_t *shelltp = NULL;
  static const evhandler_t evhndl[] = {
    InsertHandler,
    RemoveHandler
  };
  event_listener_t el0, el1;


  /* Inicializa LED */
  //palSetPadMode(GPIOE, 0, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOD, 12, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOD, 13, PAL_MODE_OUTPUT_PUSHPULL);

  // /* Inicializa Serial */
  // sdStart(&SD2, &sdcfg);
  // palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));  //TX
  // palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));  //RX

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Shell manager initialization.
   */
  shellInit();

  /* Inicializa SPI */ 
  palSetPadMode(GPIOC, 5, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);          // NSS
  // palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);          // SCK
  // palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);          // MISO
  // palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);          // MOSI

  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // SCK
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // MISO
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);            // MOSI
  palSetPad(GPIOC, 5);                                                                   // set NSS high


  /*
  * Creates the LCD thread.
  */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  
  /*
   * Initializes the SDIO drivers.
   */
    
  // initialize MMC driver
  mmcObjectInit(&MMCD1); 
  mmcStart(&MMCD1, &mmccfg);
  tmr_init(&MMCD1);

  chEvtRegister(&inserted_event, &el0, 0);
  chEvtRegister(&removed_event, &el1, 1);


  while (true) 
  {
    if (!shelltp && (SDU1.config->usbp->state == USB_ACTIVE))
    {
      shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    }

    else if (chThdTerminatedX(shelltp)) 
    {
      chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
      shelltp = NULL;           /* Triggers spawning of a new shell.        */
    }
    
    chEvtDispatch(evhndl, chEvtWaitOneTimeout(ALL_EVENTS, MS2ST(500)));


  }

}