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

int main(void)
{
    dc_motor_controller_t controller;
    dc_motor_snapshot_t snapshot;
    uint32_t calibration_finished;
    uint32_t move_finished;
    uint32_t success;

    sil_environment_reset(1000, 980, 1020);
    dc_motor_controller_init(&controller,
                             sil_environment_platform(),
                             DC_INITIAL_POSITION);

    dc_motor_on_command(&controller, DC_CMD_CALIBRATION, 0u);
    calibration_finished = run_until_state(&controller, DC_STATE_READY);

    dc_motor_on_command(&controller, DC_CMD_GET_OFF, 0u);
    move_finished = run_until_state(&controller, DC_STATE_COMPLETE);
    dc_motor_get_snapshot(&controller, &snapshot);

    success = calibration_finished != 0u &&
              move_finished != 0u &&
              snapshot.calibration_complete != 0u &&
              snapshot.start_point == 980 &&
              snapshot.end_point == 1020 &&
              snapshot.target_position == 984 &&
              snapshot.current_position == 984 &&
              sil_environment_position() == 984 &&
              snapshot.state == DC_STATE_COMPLETE;

    /* 학생이 파형보다 상태 변수의 의미에 집중하도록 변수만 출력합니다. */
    printf("calibration_finished=%u\n", calibration_finished);
    printf("calibration_complete=%u\n", snapshot.calibration_complete);
    printf("start_point=%ld\n", (long)snapshot.start_point);
    printf("end_point=%ld\n", (long)snapshot.end_point);
    printf("current_position=%ld\n", (long)snapshot.current_position);
    printf("target_position=%ld\n", (long)snapshot.target_position);
    printf("state=%u\n", (unsigned int)snapshot.state);
    printf("success=%u\n", success);

    return success != 0u ? 0 : 1;
}
