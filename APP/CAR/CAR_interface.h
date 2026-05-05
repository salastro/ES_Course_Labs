#ifndef CAR_INTERFACE_H
#define CAR_INTERFACE_H

#include "CAR_config.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../HAL/DC_MOTOR/DC_MOTOR_interface.h"
#include "../HAL/ULTRASONIC/ULTRASONIC_interface.h"

void Car_Accelerate(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor, u8 *CurrentSpeed, const u8 *MotorSpeed);
void ULTRASONIC_Debounce(ULTRASONIC_t *Sensor, u16 *DistanceCm, u8 *DebounceCounter);

#endif