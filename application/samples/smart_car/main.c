#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"

static void *main_task(const char *arg)
{
    UNUSED(arg);

    motor_init();          // 初始化 PWM 电机驱动
    while(1){
    // 设置速度
    set_left_speed(30);
    set_right_speed(30);
    uapi_tcxo_delay_ms(2000);

    set_left_speed(60);
    set_right_speed(60);
    uapi_tcxo_delay_ms(2000);

    set_left_speed(100);
    set_right_speed(100);
    uapi_tcxo_delay_ms(2000);

    set_left_speed(0);
    set_right_speed(0);
    uapi_tcxo_delay_ms(2000);
    }
    return NULL;
}

static void main_entry(void)
{
    osThreadAttr_t attr = {
        .name = "MainTask",
        .stack_size = 0x1000,
        .priority = osPriorityNormal
    };

    osThreadNew((osThreadFunc_t)main_task, NULL, &attr);
}

app_run(main_entry);
