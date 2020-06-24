#include "mh_z19.h"

#include "../Headers/CO2.h"
#include "../Headers/config.h"

#include <stdio.h>
#include <stdlib.h>


typedef struct CO2{
	uint16_t lastCO2ppm;
	
	EventGroupHandle_t measureEventGroup;
	EventGroupHandle_t dataReadyEventGroup;	
	SemaphoreHandle_t semaphore;
}CO2;

EventBits_t waitCO2_MEASUR_BIT;


static void co2_initDriver(){
	mh_z19_create(ser_USART3, NULL);

}

CO2_t co2_create(EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore){
	co2_initDriver();
	
	CO2_t self = malloc(sizeof(CO2));
	
	if (self == NULL){
		return NULL;
	}
	
	self->lastCO2ppm = 0;
	self->measureEventGroup = measureEventGroup;
	self->dataReadyEventGroup = dataReadyEventGroup;
	self->semaphore = semaphore;
	
	return self;
}


uint16_t co2_getData(CO2_t self){
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

static void co2_measureData(){
	int r = mh_z19_take_meassuring();
		if(r != MHZ19_OK){
		//puts("CO2 sensor: not ok");
	}
	else{
		//puts
	}
}

static void co2_setData(CO2_t self){
	mh_z19_get_co2_ppm(&self->lastCO2ppm);
	printf("CO2 is: %d\n", co2_getData(self));
}

void _co2_task(void *pvParameters){
	
	CO2_t self = (CO2_t)pvParameters;
	
	while(1){
		
		waitCO2_MEASUR_BIT = xEventGroupWaitBits(self->measureEventGroup,
		CO2_MEASURE_BIT,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		if ((waitCO2_MEASUR_BIT & (CO2_MEASURE_BIT)) == (CO2_MEASURE_BIT))
		{
			xSemaphoreTake(self->semaphore, portMAX_DELAY);
			
			co2_measureData();
			
			vTaskDelay(oneSecond);
			
			co2_setData(self);

			xEventGroupSetBits(self->dataReadyEventGroup, CO2_READY_BIT);
			
			xSemaphoreGive(self->semaphore);
		}

	}
}

