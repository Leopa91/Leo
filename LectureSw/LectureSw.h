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

#ifndef LectureSw_h
#define LectureSw_h

#include "ch.h"
#include "hal.h"

static uint8_t ACTU;
static uint8_t ACTU2;

static uint8_t PASOS;
static uint8_t PASOS2;

static uint8_t TURNO = 0x01;

static uint8_t aux_pasos;
static uint8_t P_PPU;
static uint16_t P_D;
static uint16_t P_V;
static uint8_t cons_emr3;
static uint8_t POS1_A;
static uint8_t POS2_A;
static uint8_t sel_pos;

void lect_ppu(uint8_t *data_ppu, uint8_t act); 
uint8_t lectura_punto(void);
void data_recibo(uint8_t P);
void desconexion(uint8_t pos);
void LectureSw(void);


#endif /* LECTURA_SW */
