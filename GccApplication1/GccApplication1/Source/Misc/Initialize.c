#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <hal_defs.h>
#include <ihal.h>
#include <lora_driver.h>

#include <FreeRTOSTraceDriver.h>
#include <stdio_driver.h>

#include "../Headers/config.h"
#include "../Headers/Controller.h"
#include "../Headers/loraUplinkHandler.h"
#include "../Headers/Initialize.h"


static uint16_t maxQueueSize = 10;

#define SENSOR_PRIORITY			(tskIDLE_PRIORITY + 1)
#define CONTROLLER_PRIORITY		(SENSOR_PRIORITY + 1)
#define LORA_UPLINK_PRIORITY	(configMAX_PRIORITIES - 1)

#define SENSOR_STACK_SIZE		(configMINIMAL_STACK_SIZE + 200)
#define CONTROLLER_STACK_SIZE	(configMINIMAL_STACK_SIZE + 250)
#define LORA_UPLINK_STACK_SIZE	(configMINIMAL_STACK_SIZE + 300)

SemaphoreHandle_t sensorSemaphore;

QueueHandle_t queue;
	
EventGroupHandle_t measureEventGroup;
EventGroupHandle_t dataReadyEventGroup;
EventGroupHandle_t loraReadyEventGroup;
	
Temperature_Humidity_t th_Sensor;
Sound_t sound_Sensor;
CO2_t co2_Sensor;

Servo_t servoWindow;

LoraUpLink_t lora_UpLink;
Controller_t controller_;


static void initialize()
{
	if ( sensorSemaphore == NULL ){
		sensorSemaphore = xSemaphoreCreateMutex();  
		if ( ( sensorSemaphore ) != NULL ){
			xSemaphoreGive( ( sensorSemaphore ) );
		}
	}


	measureEventGroup = xEventGroupCreate();
	dataReadyEventGroup = xEventGroupCreate();
	loraReadyEventGroup = xEventGroupCreate();
	
	queue = xQueueCreate(maxQueueSize, sizeof(lora_payload_t));
	
	th_Sensor = th_create(measureEventGroup, dataReadyEventGroup, sensorSemaphore);
	sound_Sensor = sound_create(measureEventGroup, dataReadyEventGroup, sensorSemaphore);
	co2_Sensor = co2_create(measureEventGroup, dataReadyEventGroup, sensorSemaphore);
	
	servoWindow = servo_create();
	
	lora_UpLink = lora_createUpLink(loraReadyEventGroup, queue);
	controller_ = controller_create(queue, measureEventGroup, dataReadyEventGroup, loraReadyEventGroup, sensorSemaphore,co2_Sensor, th_Sensor, sound_Sensor, servoWindow);
	
	xTaskCreate(
	_lora_uplink_task,
	"UpLink task",  
	LORA_UPLINK_STACK_SIZE,  
	lora_UpLink,  
	LORA_UPLINK_PRIORITY,  
	NULL);
	
	xTaskCreate(_controller_task,
	"Controller task",
	CONTROLLER_STACK_SIZE,
	controller_,
	CONTROLLER_PRIORITY,
	NULL);
	
	xTaskCreate(
	_co2_task, 
	"CO2 sensor", 
	SENSOR_STACK_SIZE, 
	co2_Sensor, 
	SENSOR_PRIORITY, 
	NULL);
	
	xTaskCreate(
	_th_task,
	"temperature and humidity sensor", 
	SENSOR_STACK_SIZE, 
	th_Sensor, 
	SENSOR_PRIORITY, 
	NULL);
	
	xTaskCreate(
	_sound_task,
	"sound sensor",
	SENSOR_STACK_SIZE,
	sound_Sensor,
	SENSOR_PRIORITY,
	NULL);
	
}

void startSystem()
{
	DDRA |= _BV(DDA0) | _BV(DDA7);
	
	trace_init();
	
	stdioCreate(ser_USART0);
	
	initialize();
	
	hal_create(5);
	
	lora_driver_create(LORA_USART, NULL);	
	
	printf("Program Started!!\n");
}