#include <stdint.h>
#include <stdio.h>

#include "dc_motor_control.h"
#include "sil_environment.h"

static uint32_t run_until_state(dc_motor_controller_t *controller,
                                dc_motor_state_t expected_state)
{
    uint32_t tick;

    for (tick = 0u; tick < 2000u; tick++)
    {
        sil_environment_tick();
        dc_motor_tick(controller);
        if (controller->snapshot.state == expected_state)
        {
            return 1u;
        }
    }
    return 0u;
}

static uint32_t snapshot_matches(const dc_motor_snapshot_t *snapshot,
                                 dc_motor_state_t state,
                                 int32_t position,
                                 int32_t target)
{
    return snapshot->state == state &&
           snapshot->current_position == position &&
           snapshot->target_position == target &&
           snapshot->direction == DC_DIRECTION_STOP;
}

int main(void)
{
    dc_motor_controller_t controller;
    dc_motor_snapshot_t snapshot;
    uint32_t calibration_finished;
    uint32_t get_off_finished;
    uint32_t get_on_finished;
    uint32_t stop_finished;
    int32_t position_before_stop;
    uint32_t success;

    sil_environment_reset(50, 0, 100);
    dc_motor_controller_init(&controller,
                             sil_environment_platform(),
                             DC_INITIAL_POSITION);

    dc_motor_on_command(&controller, DC_CMD_CALIBRATION, 0u);
    calibration_finished = run_until_state(&controller, DC_STATE_READY);

    dc_motor_on_command(&controller, DC_CMD_GET_OFF, 0u);
    get_off_finished = run_until_state(&controller, DC_STATE_COMPLETE);
    dc_motor_get_snapshot(&controller, &snapshot);

    get_on_finished = 0u;
    if (snapshot_matches(&snapshot, DC_STATE_COMPLETE, 10, 10) != 0u)
    {
        dc_motor_on_command(&controller, DC_CMD_GET_ON, 0u);
        get_on_finished = run_until_state(&controller, DC_STATE_COMPLETE);
    }
    dc_motor_get_snapshot(&controller, &snapshot);

    position_before_stop = snapshot.current_position;
    dc_motor_on_command(&controller, DC_CMD_GET_OFF, 0u);
    sil_environment_tick();
    dc_motor_tick(&controller);
    dc_motor_on_command(&controller, DC_CMD_STOP, 0u);
    sil_environment_tick();
    dc_motor_tick(&controller);
    dc_motor_get_snapshot(&controller, &snapshot);

    stop_finished = snapshot.state == DC_STATE_IDLE &&
                    snapshot.direction == DC_DIRECTION_STOP &&
                    snapshot.current_position == position_before_stop - 1 &&
                    sil_environment_position() == snapshot.current_position;

    success = calibration_finished != 0u &&
              get_off_finished != 0u &&
              get_on_finished != 0u &&
              stop_finished != 0u &&
              snapshot.calibration_complete != 0u &&
              snapshot.start_point == 0 &&
              snapshot.end_point == 100 &&
              snapshot.target_position == 10 &&
              snapshot.current_position == position_before_stop - 1;

    /* 학생이 파형보다 상태 변수의 의미에 집중하도록 변수만 출력합니다. */
    printf("calibration_finished=%u\n", calibration_finished);
    printf("calibration_complete=%u\n", snapshot.calibration_complete);
    printf("start_point=%ld\n", (long)snapshot.start_point);
    printf("end_point=%ld\n", (long)snapshot.end_point);
    printf("current_position=%ld\n", (long)snapshot.current_position);
    printf("target_position=%ld\n", (long)snapshot.target_position);
    printf("state=%u\n", (unsigned int)snapshot.state);
    printf("calibration_test=%u\n", calibration_finished);
    printf("get_off_test=%u\n", get_off_finished);
    printf("get_on_test=%u\n", get_on_finished);
    printf("stop_test=%u\n", stop_finished);
    printf("success=%u\n", success);

    return success != 0u ? 0 : 1;
}
