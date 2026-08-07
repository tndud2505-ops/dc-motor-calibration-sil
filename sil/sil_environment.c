#include "sil_environment.h"

static int32_t g_position;
static int32_t g_start_point;
static int32_t g_end_point;
static dc_motor_direction_t g_direction;
static uint32_t g_pending_hall_pulses;

void sil_environment_reset(int32_t initial_position,
                           int32_t start_point,
                           int32_t end_point)
{
    g_position = initial_position;
    g_start_point = start_point;
    g_end_point = end_point;
    g_direction = DC_DIRECTION_STOP;
    g_pending_hall_pulses = 0u;
}

void sil_environment_tick(void)
{
    if (g_direction == DC_DIRECTION_CW && g_position < g_end_point)
    {
        g_position++;
        g_pending_hall_pulses++;
    }
    else if (g_direction == DC_DIRECTION_CCW && g_position > g_start_point)
    {
        g_position--;
        g_pending_hall_pulses++;
    }
}

static void sil_motor_cw(void)
{
    g_direction = DC_DIRECTION_CW;
}

static void sil_motor_ccw(void)
{
    g_direction = DC_DIRECTION_CCW;
}

static void sil_motor_stop(void)
{
    g_direction = DC_DIRECTION_STOP;
}

static uint32_t sil_stopper_active(void)
{
    if (g_direction == DC_DIRECTION_CW && g_position >= g_end_point)
    {
        return 1u;
    }
    if (g_direction == DC_DIRECTION_CCW && g_position <= g_start_point)
    {
        return 1u;
    }
    return 0u;
}

static uint32_t sil_take_hall_pulses(void)
{
    const uint32_t pulses = g_pending_hall_pulses;
    g_pending_hall_pulses = 0u;
    return pulses;
}

static const dc_motor_platform_t g_platform = {
    sil_motor_cw,
    sil_motor_ccw,
    sil_motor_stop,
    sil_stopper_active,
    sil_take_hall_pulses
};

const dc_motor_platform_t *sil_environment_platform(void)
{
    return &g_platform;
}

int32_t sil_environment_position(void)
{
    return g_position;
}
