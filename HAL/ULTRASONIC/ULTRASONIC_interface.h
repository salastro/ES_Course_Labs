#ifndef ULTRASONIC_INTERFACE_H
#define ULTRASONIC_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

#define ULTRASONIC_SENSOR_COUNT 4

typedef struct
{
    u8 TriggerPort;
    u8 TriggerPin;
    u8 EchoPort;
    u8 EchoPin;
} ULTRASONIC_t;

void ULTRASONIC_Init(ULTRASONIC_t *Sensor);
void ULTRASONIC_InitMeasurementSystem(ULTRASONIC_t *FrontSensor, ULTRASONIC_t *BackSensor,
                                      ULTRASONIC_t *LeftSensor, ULTRASONIC_t *RightSensor);
u16 ULTRASONIC_GetDistanceCm(ULTRASONIC_t *Sensor);
u16 ULTRASONIC_GetDistanceMm(ULTRASONIC_t *Sensor);

#endif