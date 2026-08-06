#ifndef DC_MOTOR_SIL_ENVIRONMENT_H
#define DC_MOTOR_SIL_ENVIRONMENT_H

#include <stdint.h>

#include "dc_motor_control.h"

void sil_environment_reset(int32_t initial_position,
                           int32_t start_point,
                           int32_t end_point);
void sil_environment_tick(void);
const dc_motor_platform_t *sil_environment_platform(void);
int32_t sil_environment_position(void);

#endif
