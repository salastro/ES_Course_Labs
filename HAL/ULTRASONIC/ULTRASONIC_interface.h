#ifndef ULTRASONIC_INTERFACE_H
#define ULTRASONIC_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

typedef struct
{
    u8 TriggerPort;
    u8 TriggerPin;
    u8 EchoPort;
    u8 EchoPin;
} ULTRASONIC_t;

void ULTRASONIC_Init(ULTRASONIC_t *Sensor);
u16 ULTRASONIC_GetDistanceCm(ULTRASONIC_t *Sensor);
u16 ULTRASONIC_GetDistanceMm(ULTRASONIC_t *Sensor);

#endif