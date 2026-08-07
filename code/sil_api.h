#ifndef SIL_API_H
#define SIL_API_H

#include <stdint.h>

#define CMD_GET_ON       0x01u
#define CMD_GET_OFF      0x02u
#define CMD_STOP         0x03u
#define CMD_CALIBRATION  0x04u

#define DIRECTION_STOP  0u
#define DIRECTION_CW    1u
#define DIRECTION_CCW   2u

#define STATE_IDLE                0u
#define STATE_CALIBRATE_TO_UPPER  1u
#define STATE_WAIT_CURRENT_LOW    2u
#define STATE_CALIBRATE_TO_LOWER  3u
#define STATE_MOVE_TO_TARGET      4u
#define STATE_READY               5u

/* 제공 제어 코드 API */
void Control_Init(void);
void Control_Command(uint32_t command);
void Control_MainFunction(void);
void ADC1_2_IRQHandler(void);
void EXTI6_IRQHandler(void);

uint32_t Control_GetState(void);
uint32_t Control_GetCalibrationComplete(void);
int32_t Control_GetCurrentPosition(void);
int32_t Control_GetLowerPoint(void);
int32_t Control_GetUpperPoint(void);
int32_t Control_GetTargetPosition(void);
uint32_t Control_GetCurrentRaw(void);
uint32_t Control_GetMotorDirection(void);

/* 학생이 sil_environment.c에서 구현할 SIL API */
void SIL_Init(void);
void SIL_Tick(void);
void SIL_MotorCW(void);
void SIL_MotorCCW(void);
void SIL_MotorStop(void);
uint32_t SIL_ReadCurrentRaw(void);
int32_t SIL_GetPhysicalPosition(void);

#endif
