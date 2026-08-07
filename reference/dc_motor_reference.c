#include "training_api.h"

/*
 * DC motor calibration reference.
 *
 * Command map:
 *   0x01 GET ON  -> move to 50 percent
 *   0x02 GET OFF -> move to 10 percent
 *   0x03 STOP    -> stop immediately
 *   0x04 CAL     -> CCW end, then CW start
 *
 * The firmware layer touches registers and exposes small FW_* functions.
 * The control layer only calls those functions and decides the sequence.
 */

#define CMD_GET_ON 0x01u
#define CMD_GET_OFF 0x02u
#define CMD_STOP 0x03u
#define CMD_CALIBRATION 0x04u

#define STATE_IDLE 0u
#define STATE_CALIBRATE_TO_END 1u
#define STATE_WAIT_CURRENT_LOW 2u
#define STATE_CALIBRATE_TO_START 3u
#define STATE_MOVE_TO_TARGET 4u
#define STATE_READY 5u

#define DIRECTION_STOP 0u
#define DIRECTION_CW 1u
#define DIRECTION_CCW 2u

#define MOTOR_DUTY 50u
#define GET_ON_PERCENT 50u
#define GET_OFF_PERCENT 10u
#define INITIAL_POSITION 1000u
#define END_CURRENT_RAW_THRESHOLD 2800u

/* ---------------- Firmware / hardware layer ---------------- */

volatile uint32_t current_position = INITIAL_POSITION;
volatile uint32_t start_point = 0u;
volatile uint32_t end_point = 0u;
volatile uint32_t target_position = 0u;
volatile uint32_t controller_state = STATE_IDLE;
volatile uint32_t motor_direction = DIRECTION_STOP;
volatile uint32_t current_raw = 0u;
volatile uint32_t calibration_complete = 0u;
volatile uint32_t hall_count = 0u;
volatile uint32_t calibration_hall_base = 0u;
volatile uint32_t calibration_current_seen_low = 0u;

static void FW_ADC_Init(void)
{
    ADC1->SQR3 = ADC_SQR3_SQ1_ADC1_IN1;
    ADC1->CR1 = 0u;
    ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_EXTSEL_TIM2_CC2 | ADC_CR2_EXTTRIG;
}

static uint32_t FW_ADC_ReadCurrent(void)
{
    if ((ADC1->SR & ADC_SR_EOC) != 0u)
    {
        current_raw = ADC1->DR;
    }

    return current_raw;
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
    motor_direction = DIRECTION_STOP;
}

static void FW_Motor_SetCCW(void)
{
    /* DIR HIGH is CCW in the trainer model. */
    GPIOA->BSRR = GPIOA_DRV_DIR_BIT | GPIOA_DRV_EN_BIT;
    TIM2->CCR1 = MOTOR_DUTY;
    motor_direction = DIRECTION_CCW;
}

static void FW_Motor_SetCW(void)
{
    /* DIR LOW is CW in the trainer model. */
    GPIOA->BSRR = (GPIOA_DRV_DIR_BIT << 16) | GPIOA_DRV_EN_BIT;
    TIM2->CCR1 = MOTOR_DUTY;
    motor_direction = DIRECTION_CW;
}

static void FW_Hall_Reset(void)
{
    HALL->COUNT = 0u;
    hall_count = 0u;
}

static uint32_t FW_Hall_ReadCount(void)
{
    return HALL->COUNT;
}

/* ---------------- Control logic layer ---------------- */

static void CL_UpdatePosition(void)
{
    uint32_t hardware_hall_count = FW_Hall_ReadCount();

    while (hall_count < hardware_hall_count)
    {
        if (motor_direction == DIRECTION_CCW)
        {
            current_position++;
        }
        else if (motor_direction == DIRECTION_CW)
        {
            current_position--;
        }

        hall_count++;
    }
}

static void CL_StartCalibration(void)
{
    FW_Motor_Stop();
    current_position = INITIAL_POSITION;
    start_point = 0u;
    end_point = 0u;
    target_position = 0u;
    calibration_complete = 0u;
    calibration_hall_base = hall_count;
    calibration_current_seen_low = 0u;
    controller_state = STATE_CALIBRATE_TO_END;
    FW_Motor_SetCCW();
}

static void CL_StartMove(uint32_t percent)
{
    uint32_t stroke = end_point - start_point;

    target_position = start_point + (stroke * percent) / 100u;
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
        controller_state = STATE_READY;
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

    (void)data;

    if (cmd == CMD_GET_ON && calibration_complete != 0u)
    {
        CL_StartMove(GET_ON_PERCENT);
    }
    else if (cmd == CMD_GET_OFF && calibration_complete != 0u)
    {
        CL_StartMove(GET_OFF_PERCENT);
    }
    else if (cmd == CMD_STOP)
    {
        FW_Motor_Stop();
        controller_state = STATE_IDLE;
    }
    else if (cmd == CMD_CALIBRATION)
    {
        CL_StartCalibration();
    }
}

static void CL_HandleCalibration(void)
{
    if (controller_state == STATE_CALIBRATE_TO_END && current_raw >= END_CURRENT_RAW_THRESHOLD)
    {
        if (hall_count > calibration_hall_base && calibration_current_seen_low != 0u)
        {
            end_point = current_position;
            FW_Motor_Stop();
            controller_state = STATE_WAIT_CURRENT_LOW;
        }
    }
    else if (controller_state == STATE_CALIBRATE_TO_END && hall_count > calibration_hall_base && current_raw < END_CURRENT_RAW_THRESHOLD)
    {
        calibration_current_seen_low = 1u;
    }
    else if (controller_state == STATE_WAIT_CURRENT_LOW && current_raw < END_CURRENT_RAW_THRESHOLD)
    {
        calibration_hall_base = hall_count;
        calibration_current_seen_low = 0u;
        controller_state = STATE_CALIBRATE_TO_START;
        FW_Motor_SetCW();
    }
    else if (controller_state == STATE_CALIBRATE_TO_START && current_raw >= END_CURRENT_RAW_THRESHOLD)
    {
        if (hall_count > calibration_hall_base && calibration_current_seen_low != 0u)
        {
            start_point = current_position;
            calibration_complete = 1u;
            FW_Motor_Stop();
            controller_state = STATE_READY;
        }
    }
    else if (controller_state == STATE_CALIBRATE_TO_START && hall_count > calibration_hall_base && current_raw < END_CURRENT_RAW_THRESHOLD)
    {
        calibration_current_seen_low = 1u;
    }
}

static void CL_HandleMove(void)
{
    if (controller_state != STATE_MOVE_TO_TARGET)
    {
        return;
    }

    if (motor_direction == DIRECTION_CCW)
    {
        if (current_position >= target_position)
        {
            FW_Motor_Stop();
            controller_state = STATE_READY;
        }
    }
    else if (motor_direction == DIRECTION_CW)
    {
        if (current_position <= target_position)
        {
            FW_Motor_Stop();
            controller_state = STATE_READY;
        }
    }
}

static void CL_PublishWatch(void)
{
    watch_u32("current_position", current_position);
    watch_u32("start_point", start_point);
    watch_u32("end_point", end_point);
    watch_u32("target_position", target_position);
    watch_u32("current_raw", current_raw);
    watch_u32("controller_state", controller_state);
    watch_u32("calibration_complete", calibration_complete);
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
    TIM2->CR1 = TIM2_CR1_CEN;

    FW_ADC_Init();
    FW_UART_Init();
    EXTI->IMR = EXTI_IMR_MR6;
    EXTI->RTSR = EXTI_RTSR_TR6;
    EXTI->FTSR = 0u;
    EXTI->PR = EXTI_PR_PR6;
    FW_Motor_Stop();
    FW_Hall_Reset();
    CL_PublishWatch();
}

void EXTI6_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_PR_PR6) != 0u)
    {
        EXTI->PR = EXTI_PR_PR6;
    }
}

void user_loop(void)
{
    current_raw = FW_ADC_ReadCurrent();
    CL_UpdatePosition();
    CL_HandleCommand();
    CL_HandleCalibration();
    CL_HandleMove();
    CL_PublishWatch();
}
