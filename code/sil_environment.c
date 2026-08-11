#include "sil_api.h"

#define PHYSICAL_LOWER_POSITION  0
#define PHYSICAL_UPPER_POSITION  100

static int32_t physical_position;
static uint32_t motor_direction;
static uint32_t sil_current_raw;

void SIL_Init(void)
{
    physical_position = 50;
    motor_direction = DIRECTION_STOP;
    sil_current_raw = 0u;
}

void SIL_MotorCW(void)
{
    motor_direction = DIRECTION_CW;
}

void SIL_MotorCCW(void)
{
    motor_direction = DIRECTION_CCW;
}

void SIL_MotorStop(void)
{
    motor_direction = DIRECTION_STOP;
}

void SIL_Tick(void)
{
    if (motor_direction == DIRECTION_CW &&
        physical_position < PHYSICAL_UPPER_POSITION)
    {
        physical_position++;
        EXTI6_IRQHandler();
    }
    else if (motor_direction == DIRECTION_CCW &&
             physical_position > PHYSICAL_LOWER_POSITION)
    {
        physical_position--;
        EXTI6_IRQHandler();
    }

    if (motor_direction == DIRECTION_STOP)
    {
        sil_current_raw = 0u;
    }
    else if (physical_position == PHYSICAL_LOWER_POSITION ||
             physical_position == PHYSICAL_UPPER_POSITION)
    {
        sil_current_raw = 3200u;
    }
    else
    {
        sil_current_raw = 1500u;
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