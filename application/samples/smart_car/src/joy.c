#include "motor.h"
#include "bluetooth.h"
#include "track.h"
#include "cmsis_os2.h"
#include "buzzer.h"

volatile int l_or_r = 0; // 0: left, 1: right
volatile int color_mode = 0;
volatile int color_delay_time = 50;   // 颜色切换延时，默认50

typedef struct {    // 歌曲结构体
    const uint16_t *notes;
    const uint32_t *durations;
    uint16_t length;
} music_t;

/* ===== 曲子 1：简单提示音 ===== */
static const uint16_t music_beep_notes[] = {
    NOTE_C5, NOTE_C5, REST, NOTE_C5
};

static const uint32_t music_beep_durations[] = {
    200, 200, 100, 300
};

/* ===== 曲子 2：你现在用的那首 ===== */
static const uint16_t music_demo_notes[] = {
    523, 523, 659, 659, 698, 698, 659,
    587, 587, 523, 523, 494, 494, 440, 440, 494
};

static const uint32_t music_demo_durations[] = {
    300, 300, 300, 300, 300, 300, 600,
    300, 300, 300, 300, 300, 300, 300, 300, 600
};

/* ===== 曲子 3：晴天（副歌简化版）===== */
static const uint16_t music_qingtian_notes[] = {
    659, 784, 880,
    880, 784, 659,
    587, 659,

    659, 784, 880,
    880, 784, 659,
    587, 659,

    784, 880, 1047,
    880, 784, 659,
    587, 659,
    0
};

static const uint32_t music_qingtian_durations[] = {
    300, 300, 500,
    300, 300, 500,
    300, 500,

    300, 300, 500,
    300, 300, 500,
    300, 500,

    300, 300, 600,
    300, 300, 500,
    300, 800,
    300
};
static const music_t music_library[] = {
    {
        music_beep_notes,
        music_beep_durations,
        sizeof(music_beep_notes) / sizeof(uint16_t)
    },
    {
        music_demo_notes,
        music_demo_durations,
        sizeof(music_demo_notes) / sizeof(uint16_t)
    },
    {
        music_qingtian_notes,
        music_qingtian_durations,
        sizeof(music_qingtian_notes) / sizeof(uint16_t)
    }
};

#define MUSIC_COUNT (sizeof(music_library) / sizeof(music_library[0]))  // 曲子数量

const music_t *joy_get_music(uint8_t index) // 获取指定索引的歌曲
{
    if (index >= MUSIC_COUNT) {
        return NULL;
    }
    return &music_library[index];
}

uint8_t joy_get_music_count(void)   // 获取曲库中歌曲数量
{
    return MUSIC_COUNT;
}

void remote_control_task(void)
{
    /* 速度调节 */
    if (g_ctrl_mode == 'S') {
        if (g_speed_value > 99) g_speed_value = 99;
    }

    /* 方向控制 */
    else if (g_ctrl_mode == 'L') {

        switch (g_dir_value) {

        case '0':
            set_left_speed(0);
            set_right_speed(0);
            break;

        case 'w':   // 前进
            set_left_speed(-g_speed_value);
            set_right_speed(-g_speed_value);           
            break;

        case 's':   // 后退
            set_left_speed(g_speed_value);
            set_right_speed(g_speed_value);
            break;

        case 'a':   // 左转
            set_left_speed(-g_speed_value);
            set_right_speed(g_speed_value);
            break;

        case 'd':   // 右转
            set_left_speed(g_speed_value);
            set_right_speed(-g_speed_value);
            break;

        default:
            break;
        }
    }
}

void obstacle_avoid_task(void)
{
    int left  = get_track_status(9);   // 左
    int front = get_track_status(10);  // 前
    int right = get_track_status(11);  // 右

    int speed = g_speed_value;

    /* 全通：前进 */
    if (left == 1 && front == 1 && right == 1) {
        set_left_speed(-speed);
        set_right_speed(-speed);
        return;
    }

    /* 左侧有障碍 → 右转 */
    if (left == 0) {
        set_left_speed(speed);
        set_right_speed(-speed);
        osDelay(150);
        set_left_speed(-speed);
        set_right_speed(-speed);        
        return;
    }

    /* 右侧有障碍 → 左转 */
    if (right == 0) {
        set_left_speed(-speed);
        set_right_speed(speed);
        osDelay(150);
        set_left_speed(-speed);
        set_right_speed(-speed);   
        return;
    }

    /* 正前方有障碍 */
    if (front == 0) 
    {
        /* 后退 */
        set_left_speed(speed);
        set_right_speed(speed);
        color_delay_time = 10;
        osDelay(85);
        /* 掉头 */
        if(l_or_r == 0)
        {
            set_left_speed(speed);
            set_right_speed(-speed);
            osDelay(150);
            l_or_r = 1;
        }
        else
        {
            set_left_speed(-speed);
            set_right_speed(speed);
            osDelay(150);
            l_or_r = 0;
        }
        color_delay_time = 50;
        return;
    }
}