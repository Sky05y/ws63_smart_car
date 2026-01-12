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
/* 蓝牙任务 */
static void *bluetooth_task(const char *arg)
{
    (void)arg;
    while (1)
    {
        usr_uart_read_data();
        osDelay(10);
    }
    return NULL;
}
/* 蜂鸣器任务 */
static void *buzzer_task(const char *arg)
{
    (void)arg;
    uint8_t music_index = 0;
    const music_t *music;
    buzzer_init();
    while (1)
    {
        if (is_play_music)
        {
            music = joy_get_music(music_index);
            osDelay(10);
            if (music != NULL)
            {
                buzzer_play_music(
                    music->notes,
                    music->durations,
                    music->length
                );
            }
            osDelay(40);
            /* 播完切下一首 */
            music_index++;
            if (music_index >= joy_get_music_count())
            {
                music_index = 0;
            }
        }
        osDelay(90);
    }
    return NULL;
}
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
            rgb_set_gpio(1,0,0); // 红色常亮
        }
        else if(color_mode == 2)
        {
            rgb_set_gpio(0,1,0); // 绿色常亮
        }
        else if(color_mode == 3)
        {
            rgb_set_gpio(0,0,1); // 蓝色常亮
        }
        else if(color_mode == 4)
        {
            rgb_set_gpio(1,1,0); // 黄灯常亮
        }
        else if(color_mode == 5)
        {
            rgb_set_gpio(1,0,1); // 紫灯常亮
        }
        else if(color_mode == 6)
        {
            rgb_set_gpio(0,1,1); // 青灯常亮
        }
        else if(color_mode == 7)
        {
            rgb_set_gpio(1,1,1); // 白灯常亮
        }
        else if(color_mode == 8)
        {
            rgb_set_gpio(0,0,0); // 关闭LED
        }
        osDelay(40);
    }

    return NULL;
}
/* 主任务 */
static void *main_task(const char *arg)
{
    UNUSED(arg);

    motor_init();          // 初始化PWM电机驱动
    hit_init();            // 初始化避障传感器
    usr_uart_init_config(); // 初始化蓝牙串口配置
    tb6612_init();          // 初始化TB6612电机GPIO驱动
    osDelay(200);
    set_left_speed(50);     // 初始化小车，后退一小段距离
    set_right_speed(50);
    osDelay(60);
    set_left_speed(0);
    set_right_speed(0);   
    osDelay(300); 
    while (1)
    {
        if (g_work_mode == 'R')
        {
            remote_control_task();
        }
        else if (g_work_mode == 'Y')
        {
            obstacle_avoid_task();
        }
        osDelay(30);
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
        .priority = osPriorityNormal
    };
    osThreadAttr_t attr2 = {
        .name = "BuzzerTask",
        .stack_size = 0x2000,
        .priority = osPriorityNormal
    };
    osThreadAttr_t attr3 = {
        .name = "BluetoothTask",
        .stack_size = 0x2000,
        .priority = osPriorityNormal1
    };
    osThreadNew((osThreadFunc_t)main_task, NULL, &attr);
    osThreadNew((osThreadFunc_t)led_light, NULL, &attr1);
    osThreadNew((osThreadFunc_t)buzzer_task, NULL, &attr2);
    osThreadNew((osThreadFunc_t)bluetooth_task, NULL, &attr3);
}

app_run(main_entry);
