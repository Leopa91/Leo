#ifndef LEER_TOTALES_h
#define LEER_TOTALES_h

#include "ch.h"
#include "hal.h"

static char Status1[5]={'2',',','6','1',','};
static char Status2[3]={'6','2',','};

static char PPU_POINT[2]={'3',','};
static char MON_POINT[2]={'3',','};
static char VOL_POINT[2]={'3',','};

static char PPU_LEVEL1[6]={'1','.','2','3','4',','};
static char PPU_LEVEL2[6]={'1','.','2','3','4',','};

static char PPU_VENTA1[6]={'1','.','2','3','4',','};
static char PPU_VENTA2[6]={'1','.','2','3','4',','};

static char LEVEL1_PPU[2]={'4',';'};
static char LEVEL2_PPU[2]={'4',';'};

static char T_DINERO1[10]={'0','0','0','0','1','.','2','3','4',','};
static char T_VOL1[10]={'0','0','0','0','1','.','2','3','4',','};

static char T_DINERO2[10]={'0','0','0','0','1','.','2','3','4',','};
static char T_VOL2[10]={'0','0','0','0','1','.','2','3','4',','};

static char FECHA[11]={'2','2','/','0','3','/','2','0','1','7',','};
static char FECHA2[11]={'2','2','/','0','3','/','2','0','1','7',','};

static char HORA[9]={'1','2',':','3','0',':','0','0',','};
static char HORA2[9]={'1','2',':','3','0',':','0','0',','};

static char DINERO1[8]={'0','0','1','.','2','3','4',','};
static char VOL1[8]={'0','0','1','.','2','3','4',','};

static char DINERO2[8]={'0','0','1','.','2','3','4',','};
static char VOL2[8]={'0','0','1','.','2','3','4',','};

static char PRESET1[9]={'0','0','1','.','2','3','4',',','1'};
static char PRESET2[9]={'0','0','4','.','3','2','1',',','2'};

static uint8_t ENTER_INIC[2]={'\r','\n'};

static bool ventas_1;
static bool ventas_emr1;

static bool ventas_2;
static bool ventas_emr2;

static char Data_Tx[60];
static char Data_Rx[60];

static char Data_Tx2[60];
static char Data_Rx2[60];

static char Data_Tx3[150];
static char Data_Rx3[150];

static char Data_Tx4[150];
static char Data_Rx4[150];

static char data_log[60];

static char log_Tx[60];
static char log_Rx[60];

void borrar_sd1(void);
void borrar_sd2(void);
void borrar_log(void);
void memori(void);
void Read(void);
void Write(void);
void trans(void);
void trans2(void);
void sale1_emr3(void);
void sale2_emr3(void);
void emr3_sale1(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3);
void emr3_sale2(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3);
void sale1_wire(void);
void sale2_wire(void);
void wire_sale1(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3);
void wire_sale2(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3);
void log_wire(void);
void wire_ascci(uint8_t *DataRx, uint8_t Rx); 
void log_emr3(void);
void emr3_log(uint8_t *DataTx,uint8_t Tx, uint8_t dta_emr3);
void read_ppu(void);
void write_ppu(void);
void sd_preset(uint8_t tipo, uint8_t pos);
void sd_volumen(uint8_t pos);
void sd_dinero(uint8_t pos);
void sd_Tdinero(uint8_t pos);
void sd_Tvolumen(uint8_t pos);
void sd_ppu(uint8_t niv);
void sd_venta(uint8_t pos);
void sd_hora(uint8_t *recibo);
void sd_dia(uint8_t *recibo);
void sd_punto(void);
void Status_1(uint8_t est);
void Status_2(uint8_t est);





#endif /* BUTTON_EMR_A */
