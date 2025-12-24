#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "bluetooth.h"
#include "led.h"
#include "buzzer.h"
#include "joy.h"
#include "track.h"

/* LED 灯任务 */
static void *led_light(const char *arg)
{
    (void)arg;

    led_init();

    uint32_t index = 0;
    uint32_t count = 6;

    while (1) 
    {
        if(color_mode == 0)
        {
            rgb_set_gpio(led_colors[index].r,led_colors[index].g,led_colors[index].b);
            index++;
            if (index >= count) 
                index = 0;
            osDelay(color_delay_time);   // 每个颜色 500ms
        }
        else if(color_mode == 1)
        {
            rgb_set_gpio(1,0,0); // 红色常亮，倒车模式
        }
    }

    return NULL;
}

/* 主任务 */
static void *main_task(const char *arg)
{
    UNUSED(arg);

    // buzzer_init();
    // buzzer_play_song(); 
    // osDelay(2000);

    motor_init();          // 初始化PWM电机驱动
    hit_init();            // 初始化避障传感器
    u_init();              // 初始化蓝牙串口
    tb6612_init();          // 初始化TB6612电机GPIO驱动
    osDelay(200);
    // 初始化小车，后退一小段距离
    set_left_speed(50);
    set_right_speed(50);
    osDelay(60);
    set_left_speed(0);
    set_right_speed(0);   
    osDelay(300); 
    while (1)
    {
        usr_uart_read_data();
        // 选择工作模式
        if (g_work_mode == 'R')
        {
            remote_control_task();
        }
        else if (g_work_mode == 'Y')
        {
            obstacle_avoid_task();
        }
        osDelay(10);
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
    osThreadAttr_t attr1 = {
        .name = "LedTask",
        .stack_size = 0x2000,
        .priority = osPriorityNormal1
    };
    osThreadNew((osThreadFunc_t)main_task, NULL, &attr);
    osThreadNew((osThreadFunc_t)led_light, NULL, &attr1);
}

app_run(main_entry);
