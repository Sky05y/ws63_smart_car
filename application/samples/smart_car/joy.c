#include "motor.h"
#include "bluetooth.h"
#include "track.h"
#include "cmsis_os2.h"

static int l_or_r = 0; // 0: left, 1: right
volatile int color_mode = 0;
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
        color_mode = 1;
        set_left_speed(speed);
        set_right_speed(speed);
        osDelay(85);
        color_mode = 0;
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
    }
}