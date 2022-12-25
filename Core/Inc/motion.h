#ifndef _MOTION_H_
#define _MOTION_H_

#ifdef __cplusplus
extern "C"
{
#endif

// Axis array index values. Must start with 0 and be continuous.
#define N_AXIS 3 // Number of axes
#define X_AXIS 0 // Axis indexing value.
#define Y_AXIS 1
#define Z_AXIS 2

    extern int32_t sys_position[N_AXIS];
    extern int32_t sys_probe_position[N_AXIS];

/* Includes ------------------------------------------------------------------*/
#include "robot.h"

#ifdef __cplusplus
}
#endif

#endif /* _MOTION_H_ */