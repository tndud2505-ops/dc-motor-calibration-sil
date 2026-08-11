#include "sil_api.h"

/* 학생 작성 영역: 함수 원형은 유지하고 TODO를 구현합니다. */
#define PHYSICAL_POSITION_LOWER 0
#define PHYSICAL_POSITION_START 50
#define PHYSICAL_POSITION_UPPER 100

#define CURRENT_RAW_STOP 0
#define CURRENT_RAW_NORMAL 1500
#define CURRENT_RAW_STALL 3200

static int32_t physical_position;
static uint32_t motor_direction;
static uint32_t current_raw;

void SIL_Init(void)
{
    /* TODO: 가상 위치, 모터 방향, ADC 전류값 초기화 */
    physical_position = PHYSICAL_POSITION_START;
    motor_direction = DIRECTION_STOP;
    current_raw = 0u;
}

void SIL_MotorCW(void)
{
    /* TODO: 가상 모터를 CW 상태로 변경 */
    motor_direction = DIRECTION_CW;
}

void SIL_MotorCCW(void)
{
    /* TODO: 가상 모터를 CCW 상태로 변경 */
    motor_direction = DIRECTION_CCW;
}

void SIL_MotorStop(void)
{
    /* TODO: 가상 모터를 STOP 상태로 변경 */
    motor_direction = DIRECTION_STOP;
}

void SIL_Tick(void)
{
    /* TODO: 위치 이동, Hall 인터럽트, 전류 계산, ADC 인터럽트 구현 */
    if (motor_direction == DIRECTION_CW &&
        physical_position < PHYSICAL_POSITION_UPPER) {
        physical_position++;
        EXTI6_IRQHandler();
    }
    else if (motor_direction == DIRECTION_CCW &&
        physical_position > PHYSICAL_POSITION_LOWER) {
        physical_position--;
        EXTI6_IRQHandler();
    }

    if (motor_direction == DIRECTION_STOP) {
        current_raw = CURRENT_RAW_STOP;
    }
    else if (physical_position == PHYSICAL_POSITION_UPPER ||
        physical_position == PHYSICAL_POSITION_LOWER) {
        current_raw = CURRENT_RAW_STALL;
    }
    else {
        current_raw = CURRENT_RAW_NORMAL;
    }

    ADC1_2_IRQHandler();
}

uint32_t SIL_ReadCurrentRaw(void)
{
    /* TODO: 가상 ADC 전류값 반환 */
    return current_raw;
}

int32_t SIL_GetPhysicalPosition(void)
{
    /* TODO: 가상 모터의 물리 위치 반환 */
    return physical_position;
}
