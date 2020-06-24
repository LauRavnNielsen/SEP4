#include "../Headers/Sound.h"
#include "../Headers/config.h"

#include <stdio.h>
#include <stdlib.h>

EventBits_t waitSoundBit;

typedef struct Sound {
	uint16_t dBMeasured;
	EventGroupHandle_t measureEventGroup;
	EventGroupHandle_t dataReadyEventGroup;
	SemaphoreHandle_t semaphore;
}Sound;

Sound_t sound_create(EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore) {
	Sound_t self = malloc(sizeof(Sound));

	if (self == NULL){
		return NULL;
	}
	self->dBMeasured = -1;
	self->measureEventGroup = measureEventGroup;
	self->dataReadyEventGroup = dataReadyEventGroup;
	self->semaphore = semaphore;
	//puts("Sound driver initialized correctly.");
	
	return self;
}

static void sound_measure(Sound_t self) {
	self->dBMeasured = (rand() % (150 - 0 + 1) + 0);
	printf("Sound: %d\n", self->dBMeasured);
}

uint16_t sound_getData(Sound_t self) {
	return self->dBMeasured;
}

void _sound_task(void* pvParameters) {
	Sound_t self = (Sound_t)pvParameters;

	while (1){
		
		waitSoundBit = xEventGroupWaitBits(self->measureEventGroup, 
		SOUND_MEASURE_BIT, 
		pdTRUE, 
		pdTRUE, 
		portMAX_DELAY);
		
		if ((waitSoundBit & (SOUND_MEASURE_BIT)) == (SOUND_MEASURE_BIT))
		{
			xSemaphoreTake(self->semaphore, portMAX_DELAY);
			
			sound_measure(self);

			xEventGroupSetBits(self->dataReadyEventGroup, SOUND_READY_BIT);
								
			xSemaphoreGive(self->semaphore);
		}
		else
		{
			//puts("timed out waiting for SOUND_MEASURE_BIT to be set"); 
		}		

	}
}