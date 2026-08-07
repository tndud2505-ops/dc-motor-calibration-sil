#ifndef DC_MOTOR_CONTROL_H
#define DC_MOTOR_CONTROL_H

#include <stdint.h>

#define DC_CMD_GET_ON 0x01u
#define DC_CMD_GET_OFF 0x02u
#define DC_CMD_STOP 0x03u
#define DC_CMD_CALIBRATION 0x04u

#define DC_INITIAL_POSITION 1000
#define DC_GET_ON_PERCENT 50u
#define DC_GET_OFF_PERCENT 10u
#define DC_POSITION_TOLERANCE 0

typedef enum
{
    DC_STATE_IDLE = 0,
    DC_STATE_CALIBRATE_TO_END,
    DC_STATE_CALIBRATE_TO_START,
    DC_STATE_READY,
    DC_STATE_MOVE_TO_TARGET,
    DC_STATE_COMPLETE
} dc_motor_state_t;

typedef enum
{
    DC_DIRECTION_STOP = 0,
    DC_DIRECTION_CW,
    DC_DIRECTION_CCW
} dc_motor_direction_t;

typedef struct
{
    void (*motor_cw)(void);
    void (*motor_ccw)(void);
    void (*motor_stop)(void);
    uint32_t (*stopper_active)(void);
    uint32_t (*take_hall_pulses)(void);
} dc_motor_platform_t;

typedef struct
{
    int32_t current_position;
    int32_t start_point;
    int32_t end_point;
    int32_t target_position;
    dc_motor_state_t state;
    dc_motor_direction_t direction;
    uint32_t calibration_complete;
    uint32_t command_rejected;
    uint32_t last_command;
} dc_motor_snapshot_t;

typedef struct
{
    dc_motor_snapshot_t snapshot;
    const dc_motor_platform_t *platform;
} dc_motor_controller_t;

void dc_motor_controller_init(dc_motor_controller_t *controller,
                              const dc_motor_platform_t *platform,
                              int32_t initial_position);
void dc_motor_on_command(dc_motor_controller_t *controller,
                         uint32_t command,
                         uint32_t data);
void dc_motor_tick(dc_motor_controller_t *controller);
void dc_motor_get_snapshot(const dc_motor_controller_t *controller,
                           dc_motor_snapshot_t *snapshot);

#endif
