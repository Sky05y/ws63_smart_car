#include "motor.h"
#include "pwm.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "tcxo.h"

#define PWM_LEFT_GPIO      1
#define PWM_RIGHT_GPIO     2
#define PWM_PIN_MODE       1

#define PWM_LEFT_CHANNEL   1    //左侧PWM口
#define PWM_RIGHT_CHANNEL  2    //右侧PWM口

// 固定总周期
#define PWM_TOTAL_COUNT    1000   // 可根据需要调大/调小

static pwm_config_t cfg_left;
static pwm_config_t cfg_right;

void motor_init(void)
{
    uapi_pin_set_mode(PWM_LEFT_GPIO, PWM_PIN_MODE);
    uapi_pin_set_mode(PWM_RIGHT_GPIO, PWM_PIN_MODE);

    // uapi_pwm_deinit();
    uapi_pwm_init();
    // 默认全 0%
    cfg_left.low_time  = PWM_TOTAL_COUNT;
    cfg_left.high_time = 0;
    cfg_left.offset_time = 0;
    cfg_left.cycles = 0;
    cfg_left.repeat = true;

    cfg_right = cfg_left;

    uapi_pwm_open(PWM_LEFT_CHANNEL, &cfg_left);
    uapi_pwm_open(PWM_RIGHT_CHANNEL, &cfg_right);

    // 必须启动
    uapi_pwm_start(PWM_LEFT_CHANNEL);
    uapi_pwm_start(PWM_RIGHT_CHANNEL);

    osal_printk("motor init OK\n");
}

void set_left_speed(uint8_t speed)
{
    if (speed > 100) speed = 100;

    uint32_t high = speed * 10;         // high + low = 1000
    uint32_t low  = PWM_TOTAL_COUNT - high;

    pwm_config_t cfg = {
        .low_time = low,
        .high_time = high,
        .offset_time = 0,
        .cycles = 0,
        .repeat = true
    };

    printf("set left speed: %d%% -> high=%u low=%u\n", speed, high, low);

    uapi_pwm_open(PWM_LEFT_CHANNEL, &cfg);   // 自动覆盖旧配置
    uapi_pwm_start(PWM_LEFT_CHANNEL);        // 必须重新 start
}

void set_right_speed(uint8_t speed)
{
    if (speed > 100) speed = 100;

    uint32_t high = speed * 10;
    uint32_t low  = PWM_TOTAL_COUNT - high;

    pwm_config_t cfg = {
        .low_time = low,
        .high_time = high,
        .offset_time = 0,
        .cycles = 0,
        .repeat = true
    };

    printf("set right speed: %d%% -> high=%u low=%u\n", speed, high, low);

    uapi_pwm_open(PWM_RIGHT_CHANNEL, &cfg);
    uapi_pwm_start(PWM_RIGHT_CHANNEL);
}