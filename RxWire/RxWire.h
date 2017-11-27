#ifndef RxWire_h
#define RxWire_h

#include "ch.h"
#include "hal.h"


static uint8_t recibo_wire[20];
static uint8_t chec1;
static uint8_t chec2;


void Imprimir(uint8_t *envio_wire,uint8_t P);
uint8_t verif_wire(uint8_t vec[], uint8_t tamano,uint8_t tipo);
void espe_wire(uint8_t vec[], uint8_t tamano);
void rx_especial(uint8_t A1, uint8_t A2, uint8_t A3, uint8_t pos);
void rx_reporte(uint8_t tipo, uint8_t pos);
void rx_ppu(uint8_t niv, uint8_t A1, uint8_t A2, uint8_t A3, uint8_t A4);
void venta_ppu(uint8_t niv, uint8_t pos);
void rx_venta(uint8_t pos, uint8_t A1, uint8_t A2, uint8_t A3, uint8_t A4, uint8_t A5, uint8_t A6);
uint8_t RxWire (void);



#endif /* BUTTON_EMR_A */
