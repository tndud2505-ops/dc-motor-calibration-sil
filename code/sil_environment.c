#include "sil_api.h" //[cite: 9]

#define PHYSICAL_LOWER_POSITION 0    //
#define PHYSICAL_UPPER_POSITION 100  //[cite: 1]

/* 가상 환경에서 보관할 상태 변수들 */
static int32_t physical_position; //
static uint32_t motor_direction;  //[cite: 3]
static uint32_t sil_current_raw;  //[cite: 3]

void SIL_Init(void)
{
    physical_position = 50;           //
    motor_direction = DIRECTION_STOP; //[cite: 3]
    sil_current_raw = 0u;             //
}

void SIL_MotorCW(void)
{
    motor_direction = DIRECTION_CW;   //[cite: 3]
}

void SIL_MotorCCW(void)
{
    motor_direction = DIRECTION_CCW;  //[cite: 3]
}

void SIL_MotorStop(void)
{
    motor_direction = DIRECTION_STOP; //[cite: 3]
}

void SIL_Tick(void)
{
    /* 1. 모터 위치 갱신 및 Hall 인터럽트 발생 */
    if (motor_direction == DIRECTION_CW && physical_position < PHYSICAL_UPPER_POSITION) //[cite: 3]
    {
        physical_position++; //
        EXTI6_IRQHandler();  //[cite: 1, 3]
    }
    else if (motor_direction == DIRECTION_CCW && physical_position > PHYSICAL_LOWER_POSITION) //[cite: 3]
    {
        physical_position--; //[cite: 1, 3]
        EXTI6_IRQHandler();  //[cite: 1, 3]
    }

    /* 2. 전류값 결정 */
    if (motor_direction == DIRECTION_STOP) //[cite: 3]
    {
        sil_current_raw = 0u;       //[cite: 1, 3]
    }
    else if (physical_position == PHYSICAL_LOWER_POSITION || physical_position == PHYSICAL_UPPER_POSITION) //[cite: 3]
    {
        sil_current_raw = 3200u;    //[cite: 1, 3]
    }
    else
    {
        sil_current_raw = 1500u;    //[cite: 1, 3]
    }

    /* 3. ADC 인터럽트 발생 */
    ADC1_2_IRQHandler(); //[cite: 1, 3]
}

uint32_t SIL_ReadCurrentRaw(void)
{
    return sil_current_raw; //[cite: 3]
}

int32_t SIL_GetPhysicalPosition(void)
{
    return physical_position; //[cite: 3]
}