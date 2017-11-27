#ifndef eepromvirtual_h
#define eepromvirtual_h

#include "ch.h"
#include "hal.h"

    
#define estado1           	0
#define pos1              	1

#define estado2           	2
#define pos2              	3

#define pppu              	4

#define ppu1_level1       	5
#define ppu2_level1       	6
#define ppu3_level1       	7
#define ppu4_level1       	8

#define ppu1_level2       	9
#define ppu2_level2       	10
#define ppu3_level2       	11
#define ppu4_level2       	12

#define level             	13

#define PV                	14

#define v1_1              	15
#define v2_1              	16
#define v3_1              	17
#define v4_1              	18
#define v5_1              	19
#define v6_1              	20

#define v1_2              	21
#define v2_2              	22
#define v3_2             	23
#define v4_2              	24
#define v5_2              	25
#define v6_2              	26

#define PD                	27

#define d1_1              	28
#define d2_1              	29
#define d3_1              	30
#define d4_1              	31                    
#define d5_1              	32
#define d6_1              	33

#define d1_2              	34
#define d2_2              	35
#define d3_2              	36
#define d4_2              	37
#define d5_2              	38
#define d6_2              	39

#define preset_1          	40
#define preset_2          	41

#define preset1_1         	42
#define preset1_2         	43
#define preset1_3         	44
#define preset1_4         	45
#define preset1_5         	46
#define preset1_6         	47

#define preset2_1         	48
#define preset2_2         	49
#define preset2_3         	50
#define preset2_4         	51
#define preset2_5         	52
#define preset2_6         	53

#define tv1_1             	54
#define tv1_2             	55
#define tv1_3             	56
#define tv1_4             	57
#define tv1_5             	58
#define tv1_6             	59
#define tv1_7             	60
#define tv1_8             	61

#define tv2_1             	62
#define tv2_2             	63
#define tv2_3             	64
#define tv2_4             	65
#define tv2_5             	66
#define tv2_6             	67
#define tv2_7             	68
#define tv2_8             	69

#define td1_1             	70
#define td1_2             	71
#define td1_3             	72
#define td1_4             	73
#define td1_5             	74
#define td1_6             	75
#define td1_7             	76
#define td1_8             	77

#define td2_1             	78
#define td2_2             	79
#define td2_3             	80
#define td2_4             	81
#define td2_5             	82
#define td2_6             	83
#define td2_7             	84
#define td2_8             	85

#define lado1             	86
#define lado2             	87

#define log               	88
#define consola           	89

#define autor1            	90
#define autor2            	91

#define Flag1             	92

#define inicio1           	93
#define inicio2           	94

#define real_din11        	95
#define real_din12       	96
#define real_din13        	97
#define real_din14        	98
#define real_din15        	99
#define real_din16        	100

#define real_din21        	101
#define real_din22        	102
#define real_din23        	103
#define real_din24        	104
#define real_din25        	105
#define real_din26        	106

#define grade_ip1         	107
#define real_vol11        	108
#define real_vol12        	109
#define real_vol13        	110
#define real_vol14        	111
#define real_vol15        	112
#define real_vol16        	113

#define grade_ip2         	114
#define real_vol21        	115
#define real_vol22        	116
#define real_vol23        	117
#define real_vol24        	118
#define real_vol25        	119
#define real_vol26        	120

#define ieee_1            	121
#define ieee_2            	122
#define ieee_3            	123
#define ieee_4            	124

#define stop1			  	125
#define stop2			  	126

#define turno_aux			127
#define CA_Flag1		  	128
#define CB_Flag1		  	129
#define SD_CONF				130
#define SD_TRANS 			131
#define SD_TRANS2			132
#define SD_logemr3			133
#define SD_logwire			134

#define pasos1				135
#define pasos2				136

#define SD_emr3_sale1		137
#define SD_emr3_sale2		138

#define SD_wire_sale1		139
#define SD_wire_sale2		140

#define ppu1_venta1       	141
#define ppu2_venta1       	142
#define ppu3_venta1       	143
#define ppu4_venta1       	144

#define ppu1_venta2       	145
#define ppu2_venta2       	146
#define ppu3_venta2       	147
#define ppu4_venta2       	148
#define venta_sd			149

#define level_1            	150
#define level_2            	151

#define pausa1            	152
#define pausa2            	153



static uint8_t dato_emr3[155];

uint8_t read_eeprom(uint8_t num);
void write_eeprom(uint8_t num,uint8_t inf);



#endif /* BUTTON_EMR_A */
