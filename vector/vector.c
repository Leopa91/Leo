#include "ch.h"
#include "hal.h"
#include "vector.h"
//#include "Emr3F2.h"

uint8_t *vector (void)
{
	uint8_t din[16];
	din[0]=2;
	din[1]=2;
	din[2]=3;
	din[3]=4;
	din[4]=5;
	din[5]=6;
	din[6]=7;
	din[7]=8;
	din[8]=9;
	din[9]=10;
	din[10]=11;
	din[11]=12;
	din[12]=13;
	din[13]=14;
	din[14]=15;
	din[15]=16;



	return *din;
}
