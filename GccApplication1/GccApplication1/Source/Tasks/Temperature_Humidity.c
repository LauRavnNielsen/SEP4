#include <hih8120.h>

#include "../Headers/config.h"
#include "../Headers/Temperature_Humidity.h"

#include <stdio.h>
#include <stdlib.h>

EventBits_t waitTHBits;

typedef struct Temperature_Humidity{
	EventGroupHandle_t measureDataEventGroup;
	EventGroupHandle_t dataReadyEventGroup;
	SemaphoreHandle_t semaphore;
	
	uint16_t temperature;
	uint16_t humidity;
	
}Temperature_Humidity;


static void th_initDriver(){
		if (HIH8120_OK != hih8120Create()){
			//puts("There is not enough HEAP memory to create the driver.");
		}
		else{
			//puts("Temp and Hum driver initialized correctly.");
		}
}

Temperature_Humidity_t th_create(EventGroupHandle_t measureDataEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore) {
	
	th_initDriver();
	
	Temperature_Humidity_t self = malloc(sizeof(Temperature_Humidity));
	
	if (self == NULL){
		return NULL;
	}
	
	self->measureDataEventGroup = measureDataEventGroup;
	self->dataReadyEventGroup = dataReadyEventGroup;
	self->semaphore = semaphore;

	self->humidity = -1;
	self->temperature = -274;
	
	return self;
}


uint16_t th_getHum(Temperature_Humidity_t self) {
	if (self->humidity == (uint16_t)-1)
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
	if (self->temperature == (uint16_t)-274)
	{
		//puts("The returned temperature value is invalid.");
		return self->temperature;
	}
	else
	{
		return self->temperature;
	}
}

static void th_wakeup(){
		hih8120DriverReturnCode_t rc = hih8120Wakeup();

		while (rc != HIH8120_OK){
			
			if (rc == HIH8120_DRIVER_NOT_CREATED) {
				//puts("The driver was not created, attempting to create driver.");
				th_initDriver();
							
				vTaskDelay(oneSecond);
				rc = hih8120Wakeup();
			}
			else if (rc == HIH8120_TWI_BUSY) {
				//puts("The two wire/I2C interface is busy, trying again.");
				vTaskDelay(oneSecond);
				rc = hih8120Wakeup();
			}
		}
		
		//puts("The temperature and humidity driver has been awoken.");
}

static void th_measure(){
	hih8120DriverReturnCode_t rc = hih8120Meassure();

	while (rc != HIH8120_OK){
		if (rc == HIH8120_DRIVER_NOT_CREATED){
			//puts("The driver was not created, attempting to create driver.");
			th_initDriver();

			vTaskDelay(oneSecond);
			rc = hih8120Meassure();
		}
		else if (rc == HIH8120_TWI_BUSY) {
			//puts("The two wire/I2C interface is busy, trying again.");
			
			vTaskDelay(oneSecond);
			rc = hih8120Meassure();
		}
	}

	//puts("Temperature and Humidity measured.");
}

static void th_waitTillReady(){
	int isReady = hih8120IsReady();
	
	while (!isReady){
		//puts("the measured temperature and humidity values were not ready, waiting before trying again");
		vTaskDelay(oneSecond);
		
		isReady = hih8120IsReady();
	}	
	
}

static void th_setData(Temperature_Humidity_t self){
	self->humidity = (int)hih8120GetHumidity();
	self->temperature = (int)hih8120GetTemperature();
	
	printf("Hum: %d Temp: %d\n", th_getHum(self), th_getTemp(self) );
}


void _th_task(void *pvParameters){
		Temperature_Humidity_t self = (Temperature_Humidity_t)pvParameters;

		while (1) {
			waitTHBits = xEventGroupWaitBits(self->measureDataEventGroup, 
			TEMP_HUM_MEASURE_BIT, 
			pdTRUE, 
			pdTRUE, 
			portMAX_DELAY);
			
			if ((waitTHBits & (TEMP_HUM_MEASURE_BIT)) == (TEMP_HUM_MEASURE_BIT)) {
				
				xSemaphoreTake(self->semaphore, portMAX_DELAY);
				
				th_wakeup();

				vTaskDelay(oneSecond);
				
				th_measure();

				vTaskDelay(oneSecond);
				
				th_waitTillReady();
				
				th_setData(self);
					
				xEventGroupSetBits(self->dataReadyEventGroup, TEMP_HUM_READY_BIT);	
				
				xSemaphoreGive(self->semaphore);
				
			}
			else{
				//puts("timed out waiting for TEMP_HUM_MEASURE BIT to be set");
			}

		}			
}

