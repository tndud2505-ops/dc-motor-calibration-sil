/*
 * Student submission template.
 *
 * Copy this file to submissions/<student-id>/control_logic.c. The public
 * reference implementation is common/dc_motor_control.c; this file exposes the
 * same SIL contract with the decision-making functions left for students.
 * Do not write MCU registers here.
 */

#include "dc_motor_control.h"

static void stop_motor(const dc_motor_platform_t *platform)
{
    if (platform != 0 && platform->motor_stop != 0)
    {
        platform->motor_stop();
    }
}

void dc_motor_controller_init(dc_motor_controller_t *controller,
                              const dc_motor_platform_t *platform,
                              int32_t initial_position)
{
    controller->platform = platform;
    controller->snapshot.current_position = initial_position;
    controller->snapshot.start_point = 0;
    controller->snapshot.end_point = 0;
    controller->snapshot.target_position = 0;
    controller->snapshot.state = DC_STATE_IDLE;
    controller->snapshot.direction = DC_DIRECTION_STOP;
    controller->snapshot.calibration_complete = 0u;
    controller->snapshot.command_rejected = 0u;
    controller->snapshot.last_command = 0u;
    stop_motor(platform);
}

void dc_motor_on_command(dc_motor_controller_t *controller,
                         uint32_t command,
                         uint32_t data)
{
    /* TODO: implement STOP, CALIBRATION, GET_ON, and GET_OFF. */
    (void)controller;
    (void)command;
    (void)data;
}

void dc_motor_tick(dc_motor_controller_t *controller)
{
    /* TODO: consume Hall pulses, update position, and advance the state machine. */
    (void)controller;
}

void dc_motor_get_snapshot(const dc_motor_controller_t *controller,
                           dc_motor_snapshot_t *snapshot)
{
    *snapshot = controller->snapshot;
}
