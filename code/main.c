#include <stdio.h>

#include "sil_api.h"

static void RunTicks(uint32_t count)
{
    uint32_t tick;

    for (tick = 0u; tick < count; tick++)
    {
        uint32_t motor_was_running = Control_GetMotorDirection();

        SIL_Tick();
        Control_MainFunction();

        if (motor_was_running != DIRECTION_STOP)
        {
            printf("moving: physical_position=%d, virtual_position=%d, current_raw=%u\n",
                   (int)SIL_GetPhysicalPosition(),
                   (int)Control_GetCurrentPosition(),
                   Control_GetCurrentRaw());
        }
    }
}

static void PrintVariables(const char *command_name)
{
    printf("[%s]\n", command_name);
    printf("state=%u\n", Control_GetState());
    printf("calibration_complete=%u\n", Control_GetCalibrationComplete());
    printf("lower_point=%d\n", (int)Control_GetLowerPoint());
    printf("upper_point=%d\n", (int)Control_GetUpperPoint());
    printf("target_position=%d\n", (int)Control_GetTargetPosition());
    printf("current_position=%d\n", (int)Control_GetCurrentPosition());
    printf("physical_position=%d\n", (int)SIL_GetPhysicalPosition());
    printf("current_raw=%u\n", Control_GetCurrentRaw());
    printf("motor_direction=%u\n\n", Control_GetMotorDirection());
}

int main(void)
{
    uint32_t tick;

    SIL_Init();
    Control_Init();

    Control_Command(CMD_CALIBRATION);
    for (tick = 0u; tick < 210u; tick++)
    {
        SIL_Tick();
        printf("[CAL tick=%u] current_position=%d physical_position=%d current_raw=%u\n",
               (unsigned int)(tick + 1u),
               (int)Control_GetCurrentPosition(),
               (int)SIL_GetPhysicalPosition(),
               Control_GetCurrentRaw());
        Control_MainFunction();
    }
    PrintVariables("CALIBRATION");

    Control_Command(CMD_GET_OFF);
    RunTicks(20u);
    PrintVariables("GET OFF");

    Control_Command(CMD_GET_ON);
    RunTicks(50u);
    PrintVariables("GET ON");

    Control_Command(CMD_GET_OFF);
    RunTicks(3u);
    Control_Command(CMD_STOP);
    RunTicks(1u);
    PrintVariables("STOP");

    return 0;
}
