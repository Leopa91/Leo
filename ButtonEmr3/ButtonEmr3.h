#ifndef ButtonEmr3_h
#define ButtonEmr3_h

#include "ch.h"
#include "hal.h"


#define 	TECLADO_MODO		0x01
#define 	TECLADO_FIN			0x02
#define 	ESTADO_DISPLAY		0x03
#define 	LEER_DISPLAY		0x04
#define 	AUTORIZAR			0x05
#define 	ESTADO_SURTIDOR		0x06
#define 	TECLADO_INICIO		0x07
#define 	INICIO				0x08
#define 	FIN					0x09
#define 	TECLADO_SIGUIENTE   0x0A
#define 	TECLADO_ENTRADA     0x0B
#define 	TECLADO_0           0x10
#define 	TECLADO_1			0x11
#define 	TECLADO_2			0x12
#define 	TECLADO_3			0x13
#define 	TECLADO_4			0x14
#define 	TECLADO_5			0x15
#define 	TECLADO_6			0x16
#define 	TECLADO_7			0x17
#define 	TECLADO_8			0x18
#define 	TECLADO_9			0x19
#define 	TECLADO_PRESET		0X1A
#define 	TOTAL_VOLUMEN		0X1B
#define 	ENVIAR_PPU			0X1C
#define 	ENVIAR_VOLUMEN		0X1D
#define 	HORAS				0X20
#define 	FECHAS   			0X21
#define 	LEER_PPU   			0X22
#define 	LEER_PRES_VOL		0X23
#define 	PEDIR_VENTA			0X24
#define		TEST_PUNTO			0X25
#define		SALIR_CONSALA		0x26
#define 	TECLADO_PAUSA		0x27


void Imprimir_EMR3(uint8_t Data[], uint8_t P);
uint8_t che_emr3(const uint8_t Data[], uint8_t P);
void ButtonEmr(uint8_t data, uint8_t pos);
void LeerTotales(uint8_t pos);
uint64_t expo2(bool bit_data, uint32_t eleva);
void f_pf(uint32_t Y, uint32_t DP, uint8_t tipo);
void Pf_p(uint8_t dato_f[]);
uint16_t mult(uint8_t punto);


#endif /* BUTTON_EMR_A */
