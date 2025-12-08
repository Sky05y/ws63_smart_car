#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "track.h"

static void *main_task(const char *arg)
{
    UNUSED(arg);

    motor_init();          // 初始化 PWM 电机驱动
    track_init(7);
    track_init(8);
    track_init(9);
    track_init(10);
    // 设置速度
    set_left_speed(40);
    set_right_speed(40);
    while(1){
        if(get_track_status(7)==0 && get_track_status(8)==0 && get_track_status(9)==0 && get_track_status(10)==0)
        {
            printf("forward\n");
            set_left_speed(40);
            set_right_speed(40);
        }
        else if (get_track_status(7) == 1 && get_track_status(8) == 1 && get_track_status(9) == 1 && get_track_status(10) == 1)
        {
            printf("stop\n");
            set_left_speed(0);
            set_right_speed(0);
        }
        else if (get_track_status(7) == 0 && get_track_status(8) == 1 && get_track_status(9) == 0 && get_track_status(10) == 0)
        {
            printf("minor_left\n");
            set_left_speed(30);
            set_right_speed(50);
        }
        else if (get_track_status(7) == 0 && get_track_status(8) == 0 && get_track_status(9) == 1 && get_track_status(10) == 0)
        {
            printf("minor_right\n");
            set_left_speed(50);
            set_right_speed(30);
        }
        else if(get_track_status(7)==1)
        {
            printf("left\n");
            set_left_speed(15);
            set_right_speed(70);
        }
        else if(get_track_status(10)==1)
        {
            printf("right\n");
            set_left_speed(70);
            set_right_speed(15);
        }
        // osDelay(100);
    }
    return NULL;
}

static void main_entry(void)
{
    osThreadAttr_t attr = {
        .name = "MainTask",
        .stack_size = 0x2000,
        .priority = osPriorityNormal
    };

    osThreadNew((osThreadFunc_t)main_task, NULL, &attr);
}

app_run(main_entry);
