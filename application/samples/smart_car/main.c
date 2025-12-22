#include "motor.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "tcxo.h"
#include "pinctrl.h"
#include "track.h"
#include "bluetooth.h"
#include "led.h"
#include "buzzer.h"
static int l_or_r = 0; // 0: left, 1: right

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
    }
}

int map_10_99_to_neg99_99(int val)
{
    if (val < 10) val = 10;
    if (val > 99) val = 99;

    // 线性映射：把 10~99 映射到 -99~99
    return ((val - 54) * 198) / 89;   // 中点 54/55 附近为 0
}

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_bit_t;


static const rgb_bit_t led_colors[] = {
    {1, 0, 0},   // 红   100
    {0, 1, 0},   // 绿   010
    {0, 0, 1},   // 蓝   001
    {1, 1, 0},   // 黄   110
    {1, 0, 1},   // 紫   101
    {0, 1, 1},   // 青   011
    {1, 1, 1},   // 白   111
    {0, 0, 0},   // 灭   000
};

static void *led_light(const char *arg)
{
    (void)arg;

    led_init();

    uint32_t index = 0;
    uint32_t count = sizeof(led_colors) / sizeof(led_colors[0]);

    while (1) {
        rgb_set_gpio(
            led_colors[index].r,
            led_colors[index].g,
            led_colors[index].b
        );

        index++;
        if (index >= count) {
            index = 0;
        }

        osDelay(50);   // 每个颜色 500ms
    }

    return NULL;
}
static void *main_task(const char *arg)
{
    UNUSED(arg);

    // buzzer_init();
    // buzzer_play_song(); 

    // osDelay(2000);

    motor_init();          // 初始化PWM电机驱动

    errcode_t ret =  track_init(9);
    printf("track init 9 ret=%d\n", ret);
    track_init(10);
    track_init(11);
    // track_init(12);        // 初始化红外循迹传感器

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

    // 设置速度
    set_left_speed(50);
    set_right_speed(50);
    osDelay(60);
    set_left_speed(0);
    set_right_speed(0);   
    osDelay(300); 
    while (1)
    {
        usr_uart_read_data();
        /* 速度调节 */
        if (g_work_mode == 'R') {
            remote_control_task();
        }
        else if (g_work_mode == 'Y') {
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
