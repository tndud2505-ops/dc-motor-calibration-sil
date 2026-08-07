#include "dc_motor_control.h"

static int32_t absolute_position_error(int32_t left, int32_t right)
{
    return left >= right ? left - right : right - left;
}

static void motor_stop(dc_motor_controller_t *controller)
{
    if (controller->platform->motor_stop != 0)
    {
        controller->platform->motor_stop();
    }
    controller->snapshot.direction = DC_DIRECTION_STOP;
}

static void motor_cw(dc_motor_controller_t *controller)
{
    if (controller->platform->motor_cw != 0)
    {
        controller->platform->motor_cw();
    }
    controller->snapshot.direction = DC_DIRECTION_CW;
}

static void motor_ccw(dc_motor_controller_t *controller)
{
    if (controller->platform->motor_ccw != 0)
    {
        controller->platform->motor_ccw();
    }
    controller->snapshot.direction = DC_DIRECTION_CCW;
}

static void reset_calibration_values(dc_motor_controller_t *controller)
{
    controller->snapshot.start_point = 0;
    controller->snapshot.end_point = 0;
    controller->snapshot.target_position = 0;
    controller->snapshot.calibration_complete = 0u;
}

static void start_calibration(dc_motor_controller_t *controller)
{
    motor_stop(controller);
    reset_calibration_values(controller);
    controller->snapshot.command_rejected = 0u;
    controller->snapshot.state = DC_STATE_CALIBRATE_TO_UPPER;
    motor_cw(controller);
}

static void start_position_move(dc_motor_controller_t *controller,
                                uint32_t percent)
{
    int32_t stroke;

    if (controller->snapshot.calibration_complete == 0u ||
        controller->snapshot.end_point <= controller->snapshot.start_point)
    {
        motor_stop(controller);
        controller->snapshot.command_rejected = 1u;
        controller->snapshot.state = DC_STATE_IDLE;
        return;
    }

    stroke = controller->snapshot.end_point - controller->snapshot.start_point;
    controller->snapshot.target_position =
        controller->snapshot.start_point + (stroke * (int32_t)percent) / 100;
    controller->snapshot.command_rejected = 0u;
    controller->snapshot.state = DC_STATE_MOVE_TO_TARGET;

    if (controller->snapshot.current_position < controller->snapshot.target_position)
    {
        motor_cw(controller);
    }
    else if (controller->snapshot.current_position > controller->snapshot.target_position)
    {
        motor_ccw(controller);
    }
    else
    {
        motor_stop(controller);
        controller->snapshot.state = DC_STATE_COMPLETE;
    }
}

static void handle_calibration(dc_motor_controller_t *controller)
{
    uint32_t stopper_active = 0u;

    if (controller->platform->stopper_active != 0)
    {
        stopper_active = controller->platform->stopper_active();
    }

    if (controller->snapshot.state == DC_STATE_CALIBRATE_TO_UPPER && stopper_active != 0u)
    {
        motor_stop(controller);
        controller->snapshot.end_point = controller->snapshot.current_position;
        controller->snapshot.state = DC_STATE_WAIT_CURRENT_LOW;
    }
    else if (controller->snapshot.state == DC_STATE_WAIT_CURRENT_LOW && stopper_active == 0u)
    {
        controller->snapshot.state = DC_STATE_CALIBRATE_TO_LOWER;
        motor_ccw(controller);
    }
    else if (controller->snapshot.state == DC_STATE_CALIBRATE_TO_LOWER && stopper_active != 0u)
    {
        motor_stop(controller);
        controller->snapshot.end_point -= controller->snapshot.current_position;
        controller->snapshot.start_point = 0;
        controller->snapshot.current_position = 0;
        controller->snapshot.calibration_complete = 1u;
        controller->snapshot.state = DC_STATE_READY;
    }
}

static void handle_position_move(dc_motor_controller_t *controller)
{
    if (absolute_position_error(controller->snapshot.current_position,
                                controller->snapshot.target_position) <= DC_POSITION_TOLERANCE)
    {
        motor_stop(controller);
        controller->snapshot.state = DC_STATE_COMPLETE;
        return;
    }

    if (controller->snapshot.current_position < controller->snapshot.target_position)
    {
        motor_cw(controller);
    }
    else
    {
        motor_ccw(controller);
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
    motor_stop(controller);
}

void dc_motor_on_command(dc_motor_controller_t *controller,
                         uint32_t command,
                         uint32_t data)
{
    controller->snapshot.last_command = command;
    controller->snapshot.command_rejected = 0u;

    if (command == DC_CMD_STOP)
    {
        motor_stop(controller);
        controller->snapshot.state = DC_STATE_IDLE;
        return;
    }

    if (data != 0u)
    {
        controller->snapshot.command_rejected = 1u;
        return;
    }

    if (command == DC_CMD_CALIBRATION)
    {
        start_calibration(controller);
    }
    else if (command == DC_CMD_GET_ON)
    {
        start_position_move(controller, DC_GET_ON_PERCENT);
    }
    else if (command == DC_CMD_GET_OFF)
    {
        start_position_move(controller, DC_GET_OFF_PERCENT);
    }
    else
    {
        controller->snapshot.command_rejected = 1u;
    }
}

void dc_motor_tick(dc_motor_controller_t *controller)
{
    uint32_t pulse_count;

    if (controller->platform->take_hall_pulses != 0)
    {
        pulse_count = controller->platform->take_hall_pulses();
        while (pulse_count > 0u)
        {
            /* IMS 좌표: LOWER=0, CW는 증가, CCW는 감소. */
            if (controller->snapshot.direction == DC_DIRECTION_CW)
            {
                controller->snapshot.current_position++;
            }
            else if (controller->snapshot.direction == DC_DIRECTION_CCW)
            {
                controller->snapshot.current_position--;
            }
            pulse_count--;
        }
    }

    if (controller->snapshot.state == DC_STATE_CALIBRATE_TO_UPPER ||
        controller->snapshot.state == DC_STATE_WAIT_CURRENT_LOW ||
        controller->snapshot.state == DC_STATE_CALIBRATE_TO_LOWER)
    {
        handle_calibration(controller);
    }
    else if (controller->snapshot.state == DC_STATE_MOVE_TO_TARGET)
    {
        handle_position_move(controller);
    }
}

void dc_motor_get_snapshot(const dc_motor_controller_t *controller,
                           dc_motor_snapshot_t *snapshot)
{
    *snapshot = controller->snapshot;
}
