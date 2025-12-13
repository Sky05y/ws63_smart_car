#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "track.h"
#include "bluetooth.h"
int map_10_99_to_neg99_99(int val)
{
    if (val < 10) val = 10;
    if (val > 99) val = 99;

    // 线性映射：把 10~99 映射到 -99~99
    return ((val - 54) * 198) / 89;   // 中点 54/55 附近为 0
}
static void *main_task(const char *arg)
{
    UNUSED(arg);

    motor_init();          // 初始化PWM电机驱动
    track_init(9);
    track_init(10);
    track_init(11);
    track_init(12);        // 初始化红外循迹传感器

    usr_uart_io_config();     // 配置UART引脚
    usr_uart_init_config();   // 初始化UART

    // 4号，5号引脚无法复用为GPIO输出模式
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(3, PIN_MODE_0);
    uapi_gpio_set_dir(3, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(3, GPIO_LEVEL_HIGH);  // AIN1 -> 右轮

    uapi_pin_set_mode(6, PIN_MODE_0);
    uapi_gpio_set_dir(6, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(6, GPIO_LEVEL_LOW);  // AIN2

    uapi_pin_set_mode(7, PIN_MODE_0);
    uapi_gpio_set_dir(7, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(7, GPIO_LEVEL_HIGH);  // BIN1 -> 左轮

    uapi_pin_set_mode(8, PIN_MODE_0);
    uapi_gpio_set_dir(8, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(8, GPIO_LEVEL_LOW);  // BIN2
    osDelay(200);

    // uapi_pin_set_mode(5, HAL_PIO_FUNC_GPIO);
    // uapi_gpio_set_dir(5, GPIO_DIRECTION_OUTPUT);        
    // uapi_gpio_set_val(5, GPIO_LEVEL_LOW);  // BIN2

    // uapi_pin_set_mode(7, HAL_PIO_FUNC_GPIO);
    // uapi_gpio_set_dir(7, GPIO_DIRECTION_OUTPUT);        
    // uapi_gpio_set_val(7, GPIO_LEVEL_HIGH);   // AIN2


    // uapi_pin_set_mode(8, HAL_PIO_FUNC_GPIO);
    // uapi_gpio_set_dir(8, GPIO_DIRECTION_OUTPUT);        
    // uapi_gpio_set_val(8, GPIO_LEVEL_LOW);  // AIN1 -> 右轮

    // 设置速度
    set_left_speed(50);
    set_right_speed(50);
    osDelay(300);
    set_left_speed(0);
    set_right_speed(0);   
    osDelay(300); 
    while(1){
        char p = usr_uart_read_data();   // 读取UART数据
        printf("Received: %c\n", p);
        if (p == '0') 
        {
            set_left_speed(0);
            set_right_speed(0);
            osDelay(10);
        }
        else if (p == 'w')
        {
            set_left_speed(-80);
            set_right_speed(-80);
            osDelay(10);
        }
        else if (p == 's')
        {
            set_left_speed(80);
            set_right_speed(80);
            osDelay(10);
        }
        else if (p == 'a')
        {
            set_left_speed(-75);
            set_right_speed(75);
            osDelay(10);
        }
        else if (p == 'd')
        {
            set_left_speed(75);
            set_right_speed(-75);
            osDelay(10);
        }


        // osDelay(10);
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
