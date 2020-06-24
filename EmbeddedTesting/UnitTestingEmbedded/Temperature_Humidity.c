#include "../UnitTestingEmbedded/Drivers/hih8120.h"

#include "config.h"
#include "Temperature_Humidity.h"

#include <stdio.h>
#include <stdlib.h>


typedef struct Temperature_Humidity {

	uint16_t temperature;
	uint16_t humidity;

}Temperature_Humidity;


void th_initDriver() {
	//hih8120DriverReturnCode_t rc = hih8120Create();

	if (HIH8120_OK != hih8120Create()) {
		//puts("There is not enough HEAP memory to create the driver.");
	}
	else {
		//puts("Temp and Hum driver initialized correctly.");
	}
}

Temperature_Humidity_t th_create() {

	th_initDriver();

	Temperature_Humidity_t self = malloc(sizeof(Temperature_Humidity));

	if (self == NULL) {
		return NULL;
	}


	self->humidity = -1;
	self->temperature = -274;

	return self;
}


uint16_t th_getHum(Temperature_Humidity_t self) {
	if (self->humidity == -1)
	{
		//puts("The returned temperature value is invalid.");
		return self->humidity;
	}
	else
	{
		return self->humidity;
	}
}

uint16_t th_getTemp(Temperature_Humidity_t self) {
	if (self->temperature == -274)
	{
		//puts("The returned temperature value is invalid.");
		return self->temperature;
	}
	else
	{
		return self->temperature;
	}
}




