#pragma once

typedef struct Servo* Servo_t;

Servo_t servo_create();
void servo_openWindow(Servo_t self);
void servo_closeWindow(Servo_t self);
void servo_halfOpenWindow(Servo_t self);
