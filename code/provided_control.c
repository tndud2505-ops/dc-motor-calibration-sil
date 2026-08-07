#include "sil_api.h"

#define INITIAL_POSITION       1000
#define STALL_CURRENT_RAW      2800u
#define GET_ON_PERCENT         50
#define GET_OFF_PERCENT        10

static uint32_t controller_state;
static uint32_t motor_direction;
static uint32_t control_current_raw;
static uint32_t calibration_complete;
static int32_t current_position;
static int32_t lower_point;
static int32_t upper_point;
static int32_t target_position;
static int32_t calibration_stroke;

static void Motor_Stop(void)
{
    SIL_MotorStop();
    motor_direction = DIRECTION_STOP;
}

static void Motor_CW(void)
{
    SIL_MotorCW();
    motor_direction = DIRECTION_CW;
}

static void Motor_CCW(void)
{
    SIL_MotorCCW();
    motor_direction = DIRECTION_CCW;
}

static void MoveToPercent(int32_t percent)
{
    target_position = lower_point + ((upper_point - lower_point) * percent) / 100;
    controller_state = STATE_MOVE_TO_TARGET;

    if (current_position < target_position)
    {
        Motor_CW();
    }
    else if (current_position > target_position)
    {
        Motor_CCW();
    }
    else
    {
        Motor_Stop();
        controller_state = STATE_READY;
    }
}

void Control_Init(void)
{
    controller_state = STATE_IDLE;
    motor_direction = DIRECTION_STOP;
    control_current_raw = 0u;
    calibration_complete = 0u;
    current_position = INITIAL_POSITION;
    lower_point = 0;
    upper_point = 0;
    target_position = 0;
    calibration_stroke = 0;
    Motor_Stop();
}

void Control_Command(uint32_t command)
{
    if (command == CMD_CALIBRATION)
    {
        calibration_complete = 0u;
        current_position = INITIAL_POSITION;
        lower_point = 0;
        upper_point = 0;
        calibration_stroke = 0;
        controller_state = STATE_CALIBRATE_TO_UPPER;
        Motor_CW();
    }
    else if (command == CMD_GET_ON && calibration_complete != 0u)
    {
        MoveToPercent(GET_ON_PERCENT);
    }
    else if (command == CMD_GET_OFF && calibration_complete != 0u)
    {
        MoveToPercent(GET_OFF_PERCENT);
    }
    else if (command == CMD_STOP)
    {
        Motor_Stop();
        controller_state = STATE_IDLE;
    }
}

void Control_MainFunction(void)
{
    if (controller_state == STATE_CALIBRATE_TO_UPPER && control_current_raw >= STALL_CURRENT_RAW)
    {
        Motor_Stop();
        controller_state = STATE_WAIT_CURRENT_LOW;
    }
    else if (controller_state == STATE_WAIT_CURRENT_LOW && control_current_raw < STALL_CURRENT_RAW)
    {
        calibration_stroke = 0;
        controller_state = STATE_CALIBRATE_TO_LOWER;
        Motor_CCW();
    }
    else if (controller_state == STATE_CALIBRATE_TO_LOWER && control_current_raw >= STALL_CURRENT_RAW)
    {
        lower_point = 0;
        upper_point = calibration_stroke;
        current_position = lower_point;
        calibration_complete = 1u;
        Motor_Stop();
        controller_state = STATE_READY;
    }
}

void ADC1_2_IRQHandler(void)
{
    control_current_raw = SIL_ReadCurrentRaw();
}

void EXTI6_IRQHandler(void)
{
    if (controller_state == STATE_CALIBRATE_TO_LOWER)
    {
        calibration_stroke++;
    }
    else if (controller_state == STATE_MOVE_TO_TARGET)
    {
        current_position += (motor_direction == DIRECTION_CW) ? 1 : -1;

        if ((motor_direction == DIRECTION_CW && current_position >= target_position) ||
            (motor_direction == DIRECTION_CCW && current_position <= target_position))
        {
            Motor_Stop();
            controller_state = STATE_READY;
        }
    }
}

uint32_t Control_GetState(void) { return controller_state; }
uint32_t Control_GetCalibrationComplete(void) { return calibration_complete; }
int32_t Control_GetCurrentPosition(void) { return current_position; }
int32_t Control_GetLowerPoint(void) { return lower_point; }
int32_t Control_GetUpperPoint(void) { return upper_point; }
int32_t Control_GetTargetPosition(void) { return target_position; }
uint32_t Control_GetCurrentRaw(void) { return control_current_raw; }
uint32_t Control_GetMotorDirection(void) { return motor_direction; }
