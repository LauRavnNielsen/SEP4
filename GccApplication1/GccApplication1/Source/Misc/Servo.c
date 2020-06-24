#include "../Source/Headers/Servo.h"
#include  "rcServo.h"
#include "../Headers/config.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct Servo{
	int isOpen;
	int isClosed;
	int isHalfOpen;
}Servo;

Servo_t servo_create(){
	rcServoCreate();
	
	Servo_t self = malloc(sizeof(Servo));
	
	if (self == NULL){
		return NULL;
	}
	
	self->isOpen = 0;
	self->isClosed = 0;
	self->isHalfOpen = 0;
	
	
	return self;
}

void servo_openWindow(Servo_t self)
{
	if (!self->isOpen){
	
		rcServoSet(0, 100);
		self->isOpen = 1;
		self->isClosed = 0;
		self->isHalfOpen = 0;
	}
}

void servo_closeWindow(Servo_t self)
{
	if (!self->isClosed){
		
		rcServoSet(0, -100);
		self->isClosed = 1;
		self->isOpen = 0;
		self->isHalfOpen = 0;
	}	
	
}

void servo_halfOpenWindow(Servo_t self)
{
	if (!self->isHalfOpen){
			
		rcServoSet(0, 0);
		self->isHalfOpen = 1;
		self->isClosed = 0;
		self->isOpen = 0;
	}
	
}

