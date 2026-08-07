#include "training_api.h"
#include "dc_motor_control.h"

#define END_CURRENT_RAW_THRESHOLD 2800u

static dc_motor_controller_t controller;
static uint32_t current_raw;
static uint32_t last_hall_count;

/* Target-only firmware: this file may access MCU registers. */
static void target_motor_cw(void)
{
    GPIOA->BSRR = GPIOA_DRV_EN_BIT | (GPIOA_DRV_DIR_BIT << 16);
    TIM2->CCR1 = 50u;
}

static void target_motor_ccw(void)
{
    GPIOA->BSRR = GPIOA_DRV_EN_BIT | GPIOA_DRV_DIR_BIT;
    TIM2->CCR1 = 50u;
}

static void target_motor_stop(void)
{
    TIM2->CCR1 = 0u;
    GPIOA->BSRR = GPIOA_DRV_EN_BIT << 16;
}

static uint32_t target_stopper_active(void)
{
    if ((ADC1->SR & ADC_SR_EOC) != 0u)
    {
        current_raw = ADC1->DR;
    }
    return current_raw >= END_CURRENT_RAW_THRESHOLD;
}

static uint32_t target_take_hall_pulses(void)
{
    uint32_t hall_count = HALL->COUNT;
    uint32_t pulse_count = hall_count - last_hall_count;
    last_hall_count = hall_count;
    return pulse_count;
}

static const dc_motor_platform_t target_platform = {
    target_motor_cw,
    target_motor_ccw,
    target_motor_stop,
    target_stopper_active,
    target_take_hall_pulses
};

static uint32_t target_uart_read(uint32_t *command, uint32_t *data)
{
    uint32_t frame;

    if ((USART1->SR & USART1_SR_RXNE) == 0u)
    {
        return 0u;
    }

    frame = USART1->DR;
    *command = (frame >> 8) & 0xFFu;
    *data = frame & 0xFFu;
    return 1u;
}

void user_init(void)
{
    trainer_clock_init_72mhz();

    RCC->APB2ENR = RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN |
                   RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN;
    RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
    GPIOA->CRL = 0x44422200u;

    TIM2->PSC = 11999u;
    TIM2->ARR = 99u;
    TIM2->CCR1 = 0u;
    TIM2->CCR2 = 30u;
    TIM2->CCER = TIM2_CCER_CC1E;
    TIM2->CR1 = TIM2_CR1_CEN;

    ADC1->SQR3 = ADC_SQR3_SQ1_ADC1_IN1;
    ADC1->CR1 = 0u;
    ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_EXTSEL_TIM2_CC2 | ADC_CR2_EXTTRIG;

    USART1->BRR = 0x1D4Cu;
    USART1->CR1 = USART1_CR1_UE | USART1_CR1_RE;

    EXTI->IMR = EXTI_IMR_MR6;
    EXTI->RTSR = EXTI_RTSR_TR6;
    EXTI->PR = EXTI_PR_PR6;
    HALL->COUNT = 0u;
    last_hall_count = 0u;

    dc_motor_controller_init(&controller, &target_platform, DC_INITIAL_POSITION);
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
    dc_motor_snapshot_t snapshot;
    uint32_t command;
    uint32_t data;

    if (target_uart_read(&command, &data) != 0u)
    {
        dc_motor_on_command(&controller, command, data);
    }

    dc_motor_tick(&controller);
    dc_motor_get_snapshot(&controller, &snapshot);

    watch_u32("current_position", (uint32_t)snapshot.current_position);
    watch_u32("lower_point", (uint32_t)snapshot.start_point);
    watch_u32("upper_point", (uint32_t)snapshot.end_point);
    watch_u32("target_position", (uint32_t)snapshot.target_position);
    watch_u32("controller_state", (uint32_t)snapshot.state);
    watch_u32("calibration_complete", snapshot.calibration_complete);
    watch_u32("current_raw", current_raw);
}
