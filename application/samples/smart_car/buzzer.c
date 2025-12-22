#include "motor.h"
#include "pwm.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "tcxo.h"
#include "gpio.h"
#include "cmsis_os2.h"
#define PWM_GPIO        12
#define PWM_MODE        1
#define PWM_CHANNEL     4
#define PWM_GROUP_ID    3

#define PWM_TOTAL_COUNT 1000

static pwm_config_t cfg;

/* ================= 音符频率表 =================
 * 用 PWM 周期近似频率（不需要非常准）
 * 数值越小，频率越高，声音越尖
 */
#define NOTE_DO   800
#define NOTE_RE   700
#define NOTE_MI   600
#define NOTE_FA   550
#define NOTE_SO   500
#define NOTE_LA   450
#define NOTE_SI   400
#define NOTE_HDO  350

#define NOTE_REST 0

/* ================= 内部函数 ================= */

/* 设置音符 */
static void buzzer_set_note(uint32_t period)
{
    if (period == NOTE_REST) {
        uapi_pwm_stop(PWM_GPIO);
        return;
    }

    cfg.high_time = period / 2;
    cfg.low_time  = period / 2;
    cfg.offset_time = 0;
    cfg.cycles = 0;
    cfg.repeat = true;

    uapi_pwm_update_duty_ratio(PWM_CHANNEL,
                               cfg.low_time,
                               cfg.high_time);

    uapi_pwm_start_group(PWM_GROUP_ID);
}

/* ================= 初始化 ================= */

void buzzer_init(void)
{
    errcode_t ret;

    ret = uapi_pin_set_mode(PWM_GPIO, PWM_MODE);
    printf("set buzzer pin mode ret=%d\n", ret);

    uapi_pwm_deinit();
    ret = uapi_pwm_init();
    if (ret != ERRCODE_SUCC) {
        printf("PWM init fail %d\n", ret);
        return;
    }
    else {
        printf("buzzer PWM init success\n");
    }

    cfg.low_time  = 500;
    cfg.high_time = 500;
    cfg.offset_time = 0;
    cfg.cycles = 0;
    cfg.repeat = true;

    ret = uapi_pwm_open(PWM_CHANNEL, &cfg);
    printf("Buzzer open ret=%d\n", ret);

    uint8_t channel_id = PWM_CHANNEL;
    ret = uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    printf("Buzzer set group ret=%d\n", ret);
    ret = uapi_pwm_start_group(PWM_GROUP_ID);
    printf("Buzzer start group ret=%d\n", ret);

    printf("buzzer init done\n");
}

/* ================= 一首测试歌曲 =================
 * 歌名：小星星（前半段）
 * Do Do So So La La So
 * Fa Fa Mi Mi Re Re Do
 */
void buzzer_play_song(void)
{
    const uint16_t song[] = {
        NOTE_DO, NOTE_DO, NOTE_SO, NOTE_SO,
        NOTE_LA, NOTE_LA, NOTE_SO, NOTE_REST,
        NOTE_FA, NOTE_FA, NOTE_MI, NOTE_MI,
        NOTE_RE, NOTE_RE, NOTE_DO
    };

    const uint16_t duration[] = {
        300,300,300,300,
        300,300,600,200,
        300,300,300,300,
        300,300,600
    };

    int len = sizeof(song) / sizeof(song[0]);

    for (int i = 0; i < len; i++) {
        buzzer_set_note(song[i]);
        osDelay(duration[i]);
        uapi_pwm_stop(PWM_GPIO);
        osDelay(50);
    }
}
