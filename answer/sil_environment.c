#include "sil_api.h"

#define PHYSICAL_LOWER_POSITION  0
#define PHYSICAL_UPPER_POSITION  100
#define PHYSICAL_INITIAL_POSITION 50

#define CURRENT_STOP_RAW    0u
#define CURRENT_NORMAL_RAW  1500u
#define CURRENT_STALL_RAW   3200u

static int32_t physical_position;
static uint32_t sil_motor_direction;
static uint32_t sil_current_raw;
static uint32_t physical_move_count;

void SIL_Init(void)
{
    physical_position = PHYSICAL_INITIAL_POSITION;
    sil_motor_direction = DIRECTION_STOP;
    sil_current_raw = CURRENT_STOP_RAW;
    physical_move_count = 0u;
}

void SIL_MotorCW(void)
{
    sil_motor_direction = DIRECTION_CW;
}

void SIL_MotorCCW(void)
{
    sil_motor_direction = DIRECTION_CCW;
}

void SIL_MotorStop(void)
{
    sil_motor_direction = DIRECTION_STOP;
}

void SIL_Tick(void)
{
    int32_t previous_position = physical_position;

    if (sil_motor_direction == DIRECTION_CW &&
        physical_position < PHYSICAL_UPPER_POSITION)
    {
        physical_position++;
    }
    else if (sil_motor_direction == DIRECTION_CCW &&
             physical_position > PHYSICAL_LOWER_POSITION)
    {
        physical_position--;
    }

    if (physical_position != previous_position)
    {
        physical_move_count++;
        if (physical_move_count >= 2u)
        {
            physical_move_count = 0u;
            EXTI6_IRQHandler();
        }
    }

    if (sil_motor_direction == DIRECTION_STOP)
    {
        sil_current_raw = CURRENT_STOP_RAW;
    }
    else if (physical_position == PHYSICAL_LOWER_POSITION ||
             physical_position == PHYSICAL_UPPER_POSITION)
    {
        sil_current_raw = CURRENT_STALL_RAW;
    }
    else
    {
        sil_current_raw = CURRENT_NORMAL_RAW;
    }

    ADC1_2_IRQHandler();
}

uint32_t SIL_ReadCurrentRaw(void)
{
    return sil_current_raw;
}

int32_t SIL_GetPhysicalPosition(void)
{
    return physical_position;
}
