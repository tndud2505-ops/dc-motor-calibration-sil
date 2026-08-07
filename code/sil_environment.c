#include "sil_api.h"

/* 학생 작성 영역: 함수 원형은 유지하고 TODO를 구현합니다. */

void SIL_Init(void)
{
    /* TODO: 가상 위치, 모터 방향, ADC 전류값 초기화 */
}

void SIL_MotorCW(void)
{
    /* TODO: 가상 모터를 CW 상태로 변경 */
}

void SIL_MotorCCW(void)
{
    /* TODO: 가상 모터를 CCW 상태로 변경 */
}

void SIL_MotorStop(void)
{
    /* TODO: 가상 모터를 STOP 상태로 변경 */
}

void SIL_Tick(void)
{
    /* TODO: 위치 이동, Hall 인터럽트, 전류 계산, ADC 인터럽트 구현 */
}

uint32_t SIL_ReadCurrentRaw(void)
{
    /* TODO: 가상 ADC 전류값 반환 */
    return 0u;
}

int32_t SIL_GetPhysicalPosition(void)
{
    /* TODO: 가상 모터의 물리 위치 반환 */
    return 0;
}
