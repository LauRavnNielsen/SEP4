

#include "CO2.h"
#include "../UnitTestingEmbedded/Drivers/serial.h"
#include "../UnitTestingEmbedded/Drivers/mh_z19.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct CO2 {
	uint16_t lastCO2ppm;

}CO2;


//void co2_initDriver() {
//mh_z19_create(ser_USART3, NULL);
//}

CO2_t co2_create() {
	//co2_initDriver();

	CO2_t self = malloc(sizeof(CO2));

	if (self == NULL) {
		return NULL;
	}

	self->lastCO2ppm = 0;


	return self;
}


uint16_t co2_getData(CO2_t self) {
	if (self->lastCO2ppm == 0)
	{
		//puts("CO2 measurement not saved correctly");
		return self->lastCO2ppm;
	}
	else
	{
		return self->lastCO2ppm;
	}

}

void co2_measureData() {
	int r = mh_z19_take_meassuring();
	if (r != MHZ19_OK) {
		//puts("CO2 sensor: not ok");
	}
	else {
		//puts
	}
}

void co2_setData(CO2_t self) {
	mh_z19_get_co2_ppm(&self->lastCO2ppm);
	printf("CO2 is: %d\n", co2_getData(self));
}


