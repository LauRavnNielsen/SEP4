#include "../Headers/config.h"
#include "../Headers/Controller.h"
#include "../Headers/DataPacketHandler.h"

#include <lora_driver.h>

#include <stdint.h>
#include <stdlib.h>

DataPacketHandler_t package;
lora_payload_t payload;
EventBits_t loraBitWait;
EventBits_t readyBitsWait;


typedef struct Controller {
	QueueHandle_t queue;
	
	EventGroupHandle_t measureEventGroup;
	EventGroupHandle_t dataReadyEventGroup;
	EventGroupHandle_t loraReadtEventGroup;
	
	SemaphoreHandle_t sensorSemaphore;
	
	CO2_t CO2_sensor;
	Temperature_Humidity_t Temp_sensor;
	Sound_t Sound_sensor;
	
	Servo_t servo;
		
}Controller;


Controller_t controller_create(QueueHandle_t queue, EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, EventGroupHandle_t loraReadyEventGroup, SemaphoreHandle_t sensorSemaphore, CO2_t CO2_sensor, Temperature_Humidity_t Temp_sensor, Sound_t Sound_sensor, Servo_t servo){
	
	
	Controller_t self = malloc(sizeof(Controller));
	
	if (self == NULL){
		return NULL;
	}
	
	
	self->queue = queue;
	
	self->measureEventGroup = measureEventGroup;
	self->dataReadyEventGroup = dataReadyEventGroup;
	self->loraReadtEventGroup = loraReadyEventGroup;
		
	self->sensorSemaphore = sensorSemaphore;
	
	self->CO2_sensor = CO2_sensor;
	self->Temp_sensor = Temp_sensor;
	self->Sound_sensor = Sound_sensor;
	
	self->servo = servo;

	return self;
}

static void servoController(Controller_t self){
	
	uint16_t CO2Data = co2_getData(self->CO2_sensor);
	
	if (CO2Data < 650 ){
		servo_closeWindow(self->servo);
	}
	else if (CO2Data > 650 && CO2Data < 1000){
		servo_halfOpenWindow(self->servo);
	}
	else{
		servo_openWindow(self->servo);
	}
	
}

static void dataPackageController(Controller_t self){
	if (package == NULL){
		package = dph_create();
	}
	
		
	dph_setHumidity(th_getHum(self->Temp_sensor), package);
	dph_setTemperature(th_getTemp(self->Temp_sensor), package);
	dph_setDb(sound_getData(self->Sound_sensor), package);
	dph_setCO2(co2_getData(self->CO2_sensor), package);
		
	payload = dph_assemble(package);
}


void _controller_task(void *pvParameters){
	
	Controller_t self = (Controller_t)pvParameters;
	
	
	loraBitWait = xEventGroupWaitBits(self->loraReadtEventGroup,
	LORA_CONNECTED_BIT,
	pdTRUE,
	pdFALSE,
	portMAX_DELAY);
	
		while (1){
		
		if ((loraBitWait & (LORA_CONNECTED_BIT)) == (LORA_CONNECTED_BIT))
		{
		
		xEventGroupSetBits(self->measureEventGroup, ALL_MEASURE_BITS);		
		
		readyBitsWait = xEventGroupWaitBits(self->dataReadyEventGroup,
				ALL_READY_BITS,
				pdTRUE,
				pdTRUE,
				portMAX_DELAY);
		
		if ((readyBitsWait & (ALL_READY_BITS)) == (ALL_READY_BITS)){
							
			
			xSemaphoreTake(self->sensorSemaphore, portMAX_DELAY);
			
			dataPackageController(self);
			
			servoController(self);

			xSemaphoreGive(self->sensorSemaphore);
			
			xQueueSend(self->queue, &payload, portMAX_DELAY);
			
			//wait before telling sensors to measure again.
			vTaskDelay(oneMinute);
		}
		
		}
	}
}