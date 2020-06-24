#pragma once

#include <stdint.h>

typedef struct Sound* Sound_t;

Sound_t sound_create();
void sound_measure(Sound_t self);
uint16_t sound_getData(Sound_t self);
