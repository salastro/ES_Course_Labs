#ifndef IR_INTERFACE_H
#define IR_INTERFACE_H

#include "../../SERVICES/STD_TYPES.h"

/* Sensor output polarity */
#define IR_ACTIVE_LOW 0U
#define IR_ACTIVE_HIGH 1U

/* Surface classification for line tracking. */
#define IR_DARK_SURFACE 0U
#define IR_LIGHT_SURFACE 1U

/* Default obstacle module polarity: LM393 output is typically active-low. */
#define IR_DEFAULT_ACTIVE_STATE IR_ACTIVE_LOW

/* Default track surface: black tape / dark line. */
#define IR_TRACK_SURFACE IR_DARK_SURFACE

typedef struct
{
    u8 Port;
    u8 Pin;
} IR_t;

void IR_Init(IR_t *Sensor);
u8 IR_GetRawValue(IR_t *Sensor);
u8 IR_IsDarkSurface(IR_t *Sensor);
u8 IR_IsLightSurface(IR_t *Sensor);
u8 IR_IsTrackDetected(IR_t *Sensor);
u8 IR_IsObstacleDetected(IR_t *Sensor);

#endif