#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"

static void *main_task(const char *arg)
{
    UNUSED(arg);

    motor_init();          // 初始化 PWM 电机驱动
    // 设置速度
    set_left_speed(40);
    set_right_speed(40);
    
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
