#ifndef CAR_CONFIG_H
#define CAR_CONFIG_H

#include "../../SERVICES/STD_TYPES.h"

// Motor speed configuration
#define CAR_MAX_SPEED 100U
#define CAR_MIN_SPEED 0U

// Acceleration/Deceleration configuration
#define CAR_ACCELERATION_STEP 5U    // Speed change per iteration
#define CAR_ACCELERATION_DELAY_MS 5U  // Delay between speed changes

// Obstacle detection configuration
#define CAR_OBSTACLE_DISTANCE_CM 10U
#define CAR_ULTRASONIC_DEBOUNCE_MAX 5U
#define CAR_ULTRASONIC_DEBOUNCE_THRESHOLD 3U

// Obstacle state definitions
#define CAR_OBSTACLE_STATE_CLEAR 0U
#define CAR_OBSTACLE_STATE_DETECTED 1U

#endif
