#include "sil_api.h"

/* 학생 작성 영역: 함수 원형은 유지하고 TODO를 구현합니다. */
static int32_t physical_position;   //가상 모터의 실제 기계 위치 0~100 (current_position과 다름)
static uint32_t motor_direction;    //모터 현재 상태
static uint32_t sil_current_raw;    //가상 ADC가 읽을 전류 원시값

#define PHYSICAL_UPPER_POSITION 100
#define PHYSICAL_LOWER_POSITION 0

void SIL_Init(void)
{
    /* TODO: 가상 위치, 모터 방향, ADC 전류값 초기화 */
    physical_position = 50; //초기 위치
    motor_direction = DIRECTION_STOP;
    sil_current_raw = 0u;
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
    if (motor_direction == DIRECTION_CW && physical_position < PHYSICAL_UPPER_POSITION) {
        physical_position++;
        EXTI6_IRQHandler();
    }

    else if (motor_direction == DIRECTION_CCW && physical_position > PHYSICAL_LOWER_POSITION) {
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
    /* TODO: 가상 ADC 전류값 반환 */
    return sil_current_raw;
}

int32_t SIL_GetPhysicalPosition(void)
{
    /* TODO: 가상 모터의 물리 위치 반환 */
    return physical_position;
}
