#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "track.h"
#include "bluetooth.h"

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
    while(1)
    {
        set_left_speed(50);
        set_right_speed(50);
        osDelay(300);
        set_left_speed(0);
        set_right_speed(0);   
        osDelay(300);
        set_left_speed(-50);
        set_right_speed(-50);
        osDelay(300);  
    }
    // while(1){
    //     int* p = usr_uart_read_data();   // 读取UART数据
    //     // p[0] 是 X 坐标，p[1] 是 Y 坐标
    //     int x = p[0];   // 0~100
    //     int y = p[1];   // 0~100

    //     // 摇杆中心在（50,50）
    //     const int center = 50;

    //     // 范围：-50 ~ +50
    //     int dx = x - center;   // 左右
    //     int dy = y - center;   // 前后

    //     // ===============================
    //     //  1) 前后速度：根据 Y 控制基础速度
    //     // ===============================
    //     int base_speed = dy;  // -50 ~ +50

    //     // 归一化为电机速度（示例最大 70）
    //     int max_speed = 70;
    //     int forward_speed = (base_speed * max_speed) / 50;  // 映射比例
    //     // forward_speed 取值范围 -70 ~ +70

    //     // ===============================
    //     //  2) 转向控制：根据 X 调整左右速度差
    //     // ===============================
    //     int turn = dx;  // -50 ~ +50

    //     // 转向强度，也做一个比例映射
    //     int turn_strength = (turn * max_speed) / 50; // -70 ~ +70

    //     // ===============================
    //     //  3) 组合左右轮速度
    //     // ===============================
    //     int left_speed  = forward_speed - turn_strength;  
    //     int right_speed = forward_speed + turn_strength;

    //     // 限幅，防止超过电机最大速度
    //     if (left_speed > max_speed) left_speed = max_speed;
    //     if (left_speed < -max_speed) left_speed = -max_speed;
    //     if (right_speed > max_speed) right_speed = max_speed;
    //     if (right_speed < -max_speed) right_speed = -max_speed;

    //     // ===============================
    //     // 4) 摇杆在中间：速度归零
    //     // ===============================
    //     if (abs(dx) < 5 && abs(dy) < 5) {
    //         left_speed = 0;
    //         right_speed = 0;
    //     }

    //     // ===============================
    //     // 5) 输出速度
    //     // ===============================
    //     set_left_speed(left_speed);
    //     set_right_speed(right_speed);
    //     // if(get_track_status(9)==0 && get_track_status(10)==0 && get_track_status(11)==0 && get_track_status(12)==0)
    //     // {
    //     //     printf("forward\n");
    //     //     set_left_speed(40);
    //     //     set_right_speed(40);
    //     // }
    //     // else if (get_track_status(9) == 1 && get_track_status(10) == 1 && get_track_status(11) == 1 && get_track_status(12) == 1)
    //     // {
    //     //     printf("stop\n");
    //     //     set_left_speed(0);
    //     //     set_right_speed(0);
    //     // }
    //     // else if (get_track_status(9) == 0 && get_track_status(10) == 1 && get_track_status(11) == 0 && get_track_status(12) == 0)
    //     // {
    //     //     printf("minor_left\n");
    //     //     set_left_speed(30);
    //     //     set_right_speed(50);
    //     // }
    //     // else if (get_track_status(9) == 0 && get_track_status(10) == 0 && get_track_status(11) == 1 && get_track_status(12) == 0)
    //     // {
    //     //     printf("minor_right\n");
    //     //     set_left_speed(50);
    //     //     set_right_speed(30);
    //     // }
    //     // else if(get_track_status(9)==1)
    //     // {
    //     //     printf("left\n");
    //     //     set_left_speed(15);
    //     //     set_right_speed(70);
    //     // }
    //     // else if(get_track_status(12)==1)
    //     // {
    //     //     printf("right\n");
    //     //     set_left_speed(70);
    //     //     set_right_speed(15);
    //     // }
    //     osDelay(50);
    // }
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
