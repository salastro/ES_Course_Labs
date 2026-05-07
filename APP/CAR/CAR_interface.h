#ifndef CAR_INTERFACE_H
#define CAR_INTERFACE_H

#include "CAR_config.h"
#include "../MCAL/GPIO/GPIO_interface.h"
#include "../HAL/DC_MOTOR/DC_MOTOR_interface.h"
#include "../HAL/ULTRASONIC/ULTRASONIC_interface.h"

void Car_Accelerate(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor, u8 *CurrentSpeed, const u8 *MotorSpeed);
void Car_InitAutonomousMode(DCMOTOR_t *LeftMotor, DCMOTOR_t *RightMotor,
                            u8 *CurrentSpeed, const u8 *MaxSpeed, const u8 *MinSpeed,
                            u8 *ObstacleState,
                            ULTRASONIC_t *FrontSensor, ULTRASONIC_t *BackSensor,
                            ULTRASONIC_t *LeftSensor, ULTRASONIC_t *RightSensor,
                            void (*LEDCallback)(u8));
void ULTRASONIC_Debounce(ULTRASONIC_t *Sensor, u16 *DistanceCm, u8 *DebounceCounter);

#endif