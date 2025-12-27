#include "buzzer.h"
#include "pinctrl.h"
#include "gpio.h"
#include "pwm.h"
#include "tcxo.h"
#include "cmsis_os2.h"
#include "osal_debug.h"

/* 蜂鸣器GPIO定义 */
#define BUZZER_GPIO     12
#define BUZZER_CHANNEL  4       // PWM通道，根据实际硬件配置调整
#define PWM_GROUP_ID    0       // PWM组ID，根据实际硬件配置调整

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

    buzzer_pwm_cfg.low_time = 1000;  // 初始低电平时间
    buzzer_pwm_cfg.high_time = 0;    // 初始高电平时间（0表示关闭）
    uapi_pwm_open(BUZZER_CHANNEL, &buzzer_pwm_cfg);

    uint8_t channel_id = BUZZER_CHANNEL;
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(PWM_GROUP_ID);
}

void buzzer_play_tone(uint16_t frequency, uint32_t duration_ms)
{
    /* 频率为0时直接停止 */
    if (frequency == 0) {
        buzzer_stop();
        return;
    }

    /* 计算PWM周期（单位：微秒），避免除0错误 */
    uint32_t period_us = 1000000 / frequency;
    if (period_us == 0) {
        printf("Invalid frequency: %d Hz\n", frequency);
        return;
    }

    /* 占空比50%，高低电平时间各为周期的一半 */
    buzzer_pwm_cfg.high_time = period_us / 2;
    buzzer_pwm_cfg.low_time = period_us - buzzer_pwm_cfg.high_time;

    /* 更新PWM配置并重启PWM组 */
    /*uapi_pwm_close(BUZZER_CHANNEL);*/
    uapi_pwm_open(BUZZER_CHANNEL, &buzzer_pwm_cfg);

    uapi_pwm_start_group(PWM_GROUP_ID); // 启动整个组而非单个GPIO

    /* 播放指定时长后停止 */
    if (duration_ms > 0) {
        uapi_tcxo_delay_ms(duration_ms);
        buzzer_stop(); // 播放完成后停止发声
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
        // printf("Play note: %d Hz, duration: %d ms\n", notes[i], durations[i]);
        buzzer_play_tone(notes[i], durations[i]);
        // 音符间短暂停顿（避免连音）
        if (i < length - 1) { // 最后一个音符后不额外停顿
           uapi_tcxo_delay_ms(50);
        }
    }
}