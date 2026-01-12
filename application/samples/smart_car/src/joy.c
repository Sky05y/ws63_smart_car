#include "motor.h"
#include "bluetooth.h"
#include "track.h"
#include "cmsis_os2.h"
#include "buzzer.h"

volatile int l_or_r = 0; // 0: left, 1: right
volatile int color_mode = 0;
volatile int color_delay_time = 50;   // 颜色切换延时，默认50

typedef struct {
    const uint16_t *notes;
    const uint32_t *durations;
    uint16_t length;
} music_t;

static const uint16_t jingle_bell_part1_notes[] = {
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_G5,
    REST
};
static const uint32_t jingle_bell_part1_durations[] = {
    200, 200, 400,
    200, 200, 400,
    200, 400,
    100
};

static const uint16_t jingle_bell_part2_notes[] = {
    NOTE_C5, NOTE_D5, NOTE_E5,
    NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
    NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
    REST
};
static const uint32_t jingle_bell_part2_durations[] = {
    200, 200, 400,
    200, 200, 200, 200,
    200, 200, 200, 200, 400,
    100
};

static const uint16_t jingle_bell_part3_notes[] = {
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5,
    REST
};
static const uint32_t jingle_bell_part3_durations[] = {
    200, 200, 400,
    200, 200, 400,
    200, 200, 200, 200, 400,
    100
};

static const uint16_t jingle_bell_part4_notes[] = {
    NOTE_G5, NOTE_G5, NOTE_G5, NOTE_G5,
    NOTE_G5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
    NOTE_C5, NOTE_D5, NOTE_E5,
    REST
};
static const uint32_t jingle_bell_part4_durations[] = {
    200, 200, 200, 200,
    200, 200, 200, 200, 400,
    200, 200, 200, 200, 400,
    200, 200, 400,
    100
};

static const uint16_t jingle_bell_part5_notes[] = {
    NOTE_E5, NOTE_G5, NOTE_F5, NOTE_D5, NOTE_C5,
    REST
};
static const uint32_t jingle_bell_part5_durations[] = {
    200, 200, 200, 200, 800,
    100
};
static const music_t music_library[] = {
    {
        jingle_bell_part1_notes,
        jingle_bell_part1_durations,
        sizeof(jingle_bell_part1_notes) / sizeof(uint16_t)
    },
    {
        jingle_bell_part2_notes,
        jingle_bell_part2_durations,
        sizeof(jingle_bell_part2_notes) / sizeof(uint16_t)
    },
    {
        jingle_bell_part3_notes,
        jingle_bell_part3_durations,
        sizeof(jingle_bell_part3_notes) / sizeof(uint16_t)
    },
    {
        jingle_bell_part4_notes,
        jingle_bell_part4_durations,
        sizeof(jingle_bell_part4_notes) / sizeof(uint16_t)
    },
    {
        jingle_bell_part5_notes,
        jingle_bell_part5_durations,
        sizeof(jingle_bell_part5_notes) / sizeof(uint16_t)
    }
};

#define MUSIC_COUNT (sizeof(music_library) / sizeof(music_library[0]))

const music_t *joy_get_music(uint8_t index)
{
    if (index >= MUSIC_COUNT) {
        return NULL;
    }
    return &music_library[index];
}

uint8_t joy_get_music_count(void)
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
        color_delay_time = 40;
        return;
    }
}