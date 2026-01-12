#include "buzzer.h"
#include "pinctrl.h"
#include "gpio.h"
#include "pwm.h"
#include "tcxo.h"
#include "cmsis_os2.h"
#include "osal_debug.h"

/* 蜂鸣器GPIO定义 */
#define BUZZER_GPIO     12
#define BUZZER_CHANNEL  4       // PWM通道
#define PWM_GROUP_ID    0       // PWM组ID

/* PWM配置参数 */
static pwm_config_t buzzer_pwm_cfg = {
    .low_time = 0,
    .high_time = 0,
    .offset_time = 0,
    .cycles = 0,
    .repeat = true
};

/**
 * @brief 停止蜂鸣器发声（占空比设为0）
 */
static void buzzer_stop(void)
{
    buzzer_pwm_cfg.high_time = 0;
    buzzer_pwm_cfg.low_time = 1000;
    uapi_pwm_close(BUZZER_CHANNEL);
    uapi_pwm_open(BUZZER_CHANNEL, &buzzer_pwm_cfg);
    uapi_pwm_start_group(PWM_GROUP_ID);
}

void buzzer_init(void)
{
    uapi_pin_init();
    uapi_pin_set_mode(BUZZER_GPIO, PIN_MODE_1);
    uapi_pwm_deinit();
    uapi_pwm_init();

    buzzer_pwm_cfg.low_time = 1000;
    buzzer_pwm_cfg.high_time = 0;
    uapi_pwm_open(BUZZER_CHANNEL, &buzzer_pwm_cfg);

    uint8_t channel_id = BUZZER_CHANNEL;
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);   
    uapi_pwm_start_group(PWM_GROUP_ID);
}

void buzzer_play_tone(uint16_t frequency, uint32_t duration_ms)
{
    if (frequency == 0) {
        buzzer_stop();
        return;
    }
    uint32_t period_us = 1000000 / frequency;
    if (period_us == 0) {
        printf("Invalid frequency: %d Hz\n", frequency);
        return;
    }
    buzzer_pwm_cfg.high_time = period_us / 2;
    buzzer_pwm_cfg.low_time = period_us - buzzer_pwm_cfg.high_time;

    uapi_pwm_open(BUZZER_CHANNEL, &buzzer_pwm_cfg);
    uapi_pwm_start_group(PWM_GROUP_ID);

    if (duration_ms > 0) {
        uapi_tcxo_delay_ms(duration_ms);
        buzzer_stop();
    }
}

/**
 * 播放一段旋律
 * @param notes 音符数组（频率）
 * @param durations 时长数组（毫秒）
 * @param length 音符数量
 */
void buzzer_play_music(const uint16_t *notes, const uint32_t *durations, uint16_t length)
{
    if (notes == NULL || durations == NULL || length == 0) {
        printf("Invalid music params!\n");
        return;
    }

    for (uint16_t i = 0; i < length; i++) {
        buzzer_play_tone(notes[i], durations[i]);
        if (i < length - 1) {
           uapi_tcxo_delay_ms(50);
        }
    }
}