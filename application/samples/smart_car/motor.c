#include "motor.h"
#include "pwm.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "tcxo.h"
#include "gpio.h"

#define PWM_LEFT_GPIO      1
#define PWM_RIGHT_GPIO     2
#define PWM_PIN_MODE1       1
#define PWM_PIN_MODE2       1

#define PWM_GROUP_ID1               1
#define PWM_GROUP_ID2               2

#define PWM_LEFT_CHANNEL   1    //左侧PWM口
#define PWM_RIGHT_CHANNEL  2    //右侧PWM口

// 固定总周期
#define PWM_TOTAL_COUNT    1000   // 可根据需要调大/调小

static pwm_config_t cfg_left;
static pwm_config_t cfg_right;

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

void motor_init(void)
{
    errcode_t ret;
    ret = uapi_pin_set_mode(PWM_LEFT_GPIO, PWM_PIN_MODE1);
    printf("set left pin mode ret=%d\n", ret);
    ret = uapi_pin_set_mode(PWM_RIGHT_GPIO, PWM_PIN_MODE2);
    printf("set right pin mode ret=%d\n", ret);
    uapi_pwm_deinit();
    ret = uapi_pwm_init();
    if (ret != ERRCODE_SUCC) {
    printf("PWM init fail %d\n", ret);
    return;
    }
    // 默认全 0%
    cfg_left.low_time  = PWM_TOTAL_COUNT-1;
    cfg_left.high_time = 1;
    cfg_left.offset_time = 0;
    cfg_left.cycles = 0;
    cfg_left.repeat = true;

    cfg_right = cfg_left;
    uapi_pwm_open(PWM_LEFT_CHANNEL, &cfg_left);
    ret = uapi_pwm_open(PWM_RIGHT_CHANNEL, &cfg_right);
    printf("PWM open ret=%d\n", ret);
    uapi_tcxo_delay_ms(100);
    printf("motor init delay done\n");
    // 必须启动

    uint8_t channel_id1 = PWM_LEFT_CHANNEL;
    uint8_t channel_id2 = PWM_RIGHT_CHANNEL;

    uapi_pwm_set_group(PWM_GROUP_ID1, &channel_id1, 1);
    uapi_pwm_set_group(PWM_GROUP_ID2, &channel_id2, 1);

    uapi_pwm_start_group(PWM_GROUP_ID1);
    uapi_pwm_start_group(PWM_GROUP_ID2);
    uapi_tcxo_delay_ms(100);
    // uapi_pwm_close(PWM_GROUP_ID1);
    // uapi_pwm_close(PWM_GROUP_ID2);

    uapi_pwm_start(PWM_LEFT_GPIO);
    uapi_pwm_start(PWM_RIGHT_GPIO);

    printf("motor init start done\n");

    // GPIO Init start


}

void set_left_speed(int8_t speed)
{
    if(speed > 0)
    {
        uapi_gpio_set_val(7, GPIO_LEVEL_HIGH);  // BIN1 -> 左轮
        uapi_gpio_set_val(8, GPIO_LEVEL_LOW);  // BIN1 -> 左轮
    }
    else
    {
        uapi_gpio_set_val(8, GPIO_LEVEL_HIGH);  // BIN1
        uapi_gpio_set_val(7, GPIO_LEVEL_LOW);  // BIN2 -> 左轮
        speed = -speed;
    }
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

    // osal_printk("set left speed: %d%% -> high=%u low=%u\n", speed, high, low);

    // uapi_pwm_open(PWM_LEFT_CHANNEL, &cfg);   // 自动覆盖旧配置
    errcode_t ret = uapi_pwm_update_duty_ratio(PWM_LEFT_CHANNEL, low, high);
    if (ret != ERRCODE_SUCC) {
    printf("PWM left update fail %d\n", ret);
    }
    uapi_pwm_start_group(PWM_GROUP_ID1);
}

void set_right_speed(int8_t speed)
{

    if (speed > 0)
    {
        uapi_gpio_set_val(3, GPIO_LEVEL_HIGH);  // AIN1 -> 右轮
        uapi_gpio_set_val(6, GPIO_LEVEL_LOW);  // AIN2
    }
    else
    {
        uapi_gpio_set_val(6, GPIO_LEVEL_HIGH);  // AIN1 -> 右轮
        uapi_gpio_set_val(3, GPIO_LEVEL_LOW);  // AIN2
        speed = -speed;
    }
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

    // printf("set right speed: %d%% -> high=%u low=%u\n", speed, high, low);

    // uapi_pwm_open(PWM_RIGHT_CHANNEL, &cfg);
    uapi_pwm_update_duty_ratio(PWM_RIGHT_CHANNEL, low, high);
    uapi_pwm_start_group(PWM_GROUP_ID2);
}

void tb6612_init(void)
{
    // 4号，5号引脚无法复用为GPIO输出模式
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(3, PIN_MODE_0);
    uapi_gpio_set_dir(3, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(3, GPIO_LEVEL_HIGH);  // AIN1 -> 右轮

    uapi_pin_set_mode(6, PIN_MODE_0);
    uapi_gpio_set_dir(6, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(6, GPIO_LEVEL_LOW);  // AIN2

    uapi_pin_set_mode(7, PIN_MODE_0);
    uapi_gpio_set_dir(7, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(7, GPIO_LEVEL_HIGH);  // BIN1 -> 左轮

    uapi_pin_set_mode(8, PIN_MODE_0);
    uapi_gpio_set_dir(8, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(8, GPIO_LEVEL_LOW);  // BIN2
}