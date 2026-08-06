#include "training_api.h"

/*
 * DC motor CALIBRATION reference for the assignment.
 *
 * The assignment convention is fixed here:
 *   - CCW Hall pulse: current_position += 1
 *   - CW Hall pulse:  current_position -= 1
 *
 * The local trainer's DIR signal is wired so DIR LOW produces the physical
 * travel used as logical CCW. Keep that board mapping inside FW_* functions;
 * the control layer must only call those functions.
 */

#define CMD_GET_ON 0x01u
#define CMD_GET_OFF 0x02u
#define CMD_STOP 0x03u
#define CMD_CALIBRATION 0x04u

#define STATE_IDLE 0u
#define STATE_CALIBRATE_TO_END 1u
#define STATE_CALIBRATE_TO_START 2u
#define STATE_READY 3u
#define STATE_MOVE_TO_TARGET 4u
#define STATE_COMPLETE 5u
#define STATE_TIMEOUT 6u

#define DIRECTION_STOP 0u
#define DIRECTION_CW 1u
#define DIRECTION_CCW 2u

#define MOTOR_DUTY 50u
#define GET_ON_PERCENT 50u
#define GET_OFF_PERCENT 10u
#define INITIAL_POSITION 1000u
#define MOTION_TIMEOUT_TICKS 1500u
#define END_CURRENT_RAW_THRESHOLD 2880u
#define END_CURRENT_SETTLE_TICKS 25u
#define END_CURRENT_CONFIRM_TICKS 8u

/* ---------------- Firmware / hardware layer ---------------- */

volatile uint32_t current_position = INITIAL_POSITION;
volatile uint32_t start_point = 0u;
volatile uint32_t end_point = 0u;
volatile uint32_t target_position = 0u;
volatile uint32_t controller_state = STATE_IDLE;
volatile uint32_t motor_direction = DIRECTION_STOP;
volatile uint32_t motor_running = 0u;
volatile uint32_t calibration_complete = 0u;
volatile uint32_t calibration_locked = 0u;
volatile uint32_t calibration_timeout = 0u;
volatile uint32_t command_rejected = 0u;
volatile uint32_t last_command = 0u;
volatile uint32_t last_data = 0u;
volatile uint32_t hall_irq_count = 0u;
volatile uint32_t hall_count = 0u;
volatile uint32_t current_raw = 0u;
volatile uint32_t motion_ticks = 0u;
volatile uint32_t end_current_ticks = 0u;
volatile uint32_t end_current_detected = 0u;

static void FW_ADC_Init(void)
{
    ADC1->SQR3 = ADC_SQR3_SQ1_ADC1_IN1;
    ADC1->CR1 = 0u;
    ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_EXTSEL_TIM2_CC2 | ADC_CR2_EXTTRIG;
}

static void FW_UART_Init(void)
{
    USART1->BRR = 0x1D4Cu;
    USART1->CR1 = USART1_CR1_UE | USART1_CR1_RE;
}

static uint32_t FW_UART_Read(uint32_t *cmd, uint32_t *data)
{
    uint32_t frame;

    if ((USART1->SR & USART1_SR_RXNE) == 0u)
    {
        return 0u;
    }

    frame = USART1->DR;
    *cmd = (frame >> 8) & 0xFFu;
    *data = frame & 0xFFu;
    return 1u;
}

static void FW_Motor_Stop(void)
{
    TIM2->CCR1 = 0u;
    GPIOA->BSRR = GPIOA_DRV_EN_BIT << 16;
    motor_running = 0u;
    motor_direction = DIRECTION_STOP;
}

static void FW_Motor_SetCCW(void)
{
    FW_Motor_Stop();
    /* Board mapping: DIR LOW is the logical CCW direction for this task. */
    GPIOA->BSRR = GPIOA_DRV_EN_BIT | (GPIOA_DRV_DIR_BIT << 16);
    TIM2->CCR1 = MOTOR_DUTY;
    motor_direction = DIRECTION_CCW;
    motor_running = 1u;
}

static void FW_Motor_SetCW(void)
{
    FW_Motor_Stop();
    GPIOA->BSRR = GPIOA_DRV_EN_BIT | GPIOA_DRV_DIR_BIT;
    TIM2->CCR1 = MOTOR_DUTY;
    motor_direction = DIRECTION_CW;
    motor_running = 1u;
}

static void FW_Hall_Reset(void)
{
    HALL->COUNT = 0u;
    hall_count = 0u;
}

static void FW_Sensor_Service(void)
{
    if ((ADC1->SR & ADC_SR_EOC) != 0u)
    {
        current_raw = ADC1->DR;
    }

    if (motor_running == 0u)
    {
        return;
    }

    motion_ticks++;
    if (motion_ticks >= END_CURRENT_SETTLE_TICKS && current_raw >= END_CURRENT_RAW_THRESHOLD)
    {
        end_current_ticks++;
        if (end_current_ticks >= END_CURRENT_CONFIRM_TICKS)
        {
            end_current_detected = 1u;
        }
    }
    else
    {
        end_current_ticks = 0u;
    }
}

static void FW_Motor_StartForCurrentDirection(void)
{
    if (motor_direction == DIRECTION_CCW)
    {
        FW_Motor_SetCCW();
    }
    else if (motor_direction == DIRECTION_CW)
    {
        FW_Motor_SetCW();
    }
}

/* ---------------- Control logic layer ---------------- */

static void CL_ResetMotionMonitor(void)
{
    motion_ticks = 0u;
    end_current_ticks = 0u;
    end_current_detected = 0u;
}

static void CL_EnterTimeout(void)
{
    FW_Motor_Stop();
    calibration_timeout = 1u;
    controller_state = STATE_TIMEOUT;
}

static void CL_StartCalibration(void)
{
    FW_Motor_Stop();
    current_position = INITIAL_POSITION;
    start_point = 0u;
    end_point = 0u;
    target_position = 0u;
    calibration_complete = 0u;
    calibration_timeout = 0u;
    CL_ResetMotionMonitor();
    controller_state = STATE_CALIBRATE_TO_END;
    FW_Motor_SetCCW();
}

static void CL_StartMove(uint32_t percent)
{
    uint32_t stroke;

    if (calibration_complete == 0u || end_point <= start_point)
    {
        FW_Motor_Stop();
        command_rejected = 1u;
        controller_state = STATE_IDLE;
        return;
    }

    stroke = end_point - start_point;
    target_position = start_point + (stroke * percent) / 100u;
    CL_ResetMotionMonitor();
    controller_state = STATE_MOVE_TO_TARGET;

    if (current_position < target_position)
    {
        FW_Motor_SetCCW();
    }
    else if (current_position > target_position)
    {
        FW_Motor_SetCW();
    }
    else
    {
        FW_Motor_Stop();
        controller_state = STATE_COMPLETE;
    }
}

static void CL_HandleCommand(void)
{
    uint32_t cmd;
    uint32_t data;

    if (FW_UART_Read(&cmd, &data) == 0u)
    {
        return;
    }

    last_command = cmd;
    last_data = data;
    command_rejected = 0u;

    if (cmd == CMD_STOP && data == 0u)
    {
        FW_Motor_Stop();
        controller_state = STATE_IDLE;
    }
    else if (data != 0u)
    {
        command_rejected = 1u;
    }
    else if (cmd == CMD_CALIBRATION && calibration_locked == 0u)
    {
        calibration_locked = 1u;
        CL_StartCalibration();
    }
    else if (cmd == CMD_GET_ON && controller_state == STATE_READY)
    {
        CL_StartMove(GET_ON_PERCENT);
    }
    else if (cmd == CMD_GET_OFF && controller_state == STATE_READY)
    {
        CL_StartMove(GET_OFF_PERCENT);
    }
    else
    {
        command_rejected = 1u;
    }
}

static void CL_HandleCalibration(void)
{
    if (controller_state == STATE_CALIBRATE_TO_END && end_current_detected != 0u)
    {
        FW_Motor_Stop();
        end_point = current_position;
        CL_ResetMotionMonitor();
        controller_state = STATE_CALIBRATE_TO_START;
        FW_Motor_SetCW();
    }
    else if (controller_state == STATE_CALIBRATE_TO_START && end_current_detected != 0u)
    {
        FW_Motor_Stop();
        start_point = current_position;
        calibration_complete = 1u;
        CL_ResetMotionMonitor();
        controller_state = STATE_READY;
    }
    else if ((controller_state == STATE_CALIBRATE_TO_END || controller_state == STATE_CALIBRATE_TO_START) && motion_ticks >= MOTION_TIMEOUT_TICKS)
    {
        CL_EnterTimeout();
    }
}

static void CL_HandleMove(void)
{
    if (controller_state != STATE_MOVE_TO_TARGET)
    {
        return;
    }

    if (current_position == target_position)
    {
        FW_Motor_Stop();
        controller_state = STATE_COMPLETE;
    }
    else if (motion_ticks >= MOTION_TIMEOUT_TICKS)
    {
        CL_EnterTimeout();
    }
    else
    {
        FW_Motor_StartForCurrentDirection();
    }
}

static void CL_Service(void)
{
    CL_HandleCommand();
    CL_HandleCalibration();
    CL_HandleMove();
}

static void CL_PublishWatch(void)
{
    watch_u32("current_position", current_position);
    watch_u32("start_point", start_point);
    watch_u32("end_point", end_point);
    watch_u32("target_position", target_position);
    watch_u32("controller_state", controller_state);
    watch_u32("motor_direction", motor_direction);
    watch_u32("calibration_complete", calibration_complete);
    watch_u32("calibration_locked", calibration_locked);
    watch_u32("calibration_timeout", calibration_timeout);
    watch_u32("command_rejected", command_rejected);
    watch_u32("last_command", last_command);
    watch_u32("last_data", last_data);
    watch_u32("hall_count", hall_count);
    watch_u32("current_raw", current_raw);
}

void user_init(void)
{
    trainer_clock_init_72mhz();

    /* Hardware initialization belongs only to the firmware layer. */
    RCC->APB2ENR = RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN;
    RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
    GPIOA->CRL = 0x44422200u;
    GPIOA->CRH = 0x44444444u;

    TIM2->PSC = 11999u;
    TIM2->ARR = 99u;
    TIM2->CCR1 = 0u;
    TIM2->CCR2 = 30u;
    TIM2->CCER = TIM2_CCER_CC1E;

    FW_ADC_Init();
    FW_UART_Init();
    EXTI->IMR = EXTI_IMR_MR6;
    EXTI->RTSR = EXTI_RTSR_TR6;
    EXTI->FTSR = 0u;
    EXTI->PR = EXTI_PR_PR6;
    TIM2->CR1 = TIM2_CR1_CEN;
    FW_Motor_Stop();
    FW_Hall_Reset();
    CL_PublishWatch();
}

void EXTI6_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_PR_PR6) != 0u)
    {
        hall_irq_count++;
        hall_count++;

        /* Assignment rule: CCW +1, CW -1. */
        if (motor_direction == DIRECTION_CCW)
        {
            current_position++;
        }
        else if (motor_direction == DIRECTION_CW && current_position > 0u)
        {
            current_position--;
        }

        EXTI->PR = EXTI_PR_PR6;
    }
}

void user_loop(void)
{
    FW_Sensor_Service();
    CL_Service();
    CL_PublishWatch();
}
