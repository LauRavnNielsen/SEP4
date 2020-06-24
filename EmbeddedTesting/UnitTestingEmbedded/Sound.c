
#include "Sound.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct Sound {
	uint16_t dBMeasured;

}Sound;

Sound_t sound_create() {
	Sound_t self = malloc(sizeof(Sound));

	if (self == NULL) {
		return NULL;
	}
	self->dBMeasured = -1;

	//puts("Sound driver initialized correctly.");

	return self;
}

void sound_measure(Sound_t self) {
	self->dBMeasured = (rand() % (150 - 0 + 1) + 0);
	printf("Sound: %d\n", self->dBMeasured);
}

uint16_t sound_getData(Sound_t self) {
	return self->dBMeasured;
}

