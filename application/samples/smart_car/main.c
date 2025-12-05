#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"

#define TASKS_TEST_TASK_STACK_SIZE    0x1000
#define TASKS_TEST_TASK_PRIO          (osPriority_t)(17)
#define TASKS_TEST_DURATION_MS        1000

#define PWM_GPIO                   2
#define PWM_PIN_MODE               1
#define PWM_CHANNEL                2
#define PWM_GROUP_ID               0
#define TEST_MAX_TIMES             5
#define DALAY_MS                   100
#define TEST_TCXO_DELAY_1000MS     1000
#define PWM_TASK_STACK_SIZE        0x1000
#define PWM_TASK_PRIO              (osPriority_t)(17)

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *tasks_test_task(const char *arg)
{
    unused(arg);

    // PWM 配置结构体：
    // low_ns  = 50ns（低电平时长）
    // high_ns = 200ns（高电平时长）
    // 这会产生 4MHz，80% 占空比的PWM波形
    pwm_config_t cfg_no_repeat = {
        50,     //低电平
        200,    //高电平
        0,
        0,
        true
    };

    
    uapi_pin_set_mode(PWM_GPIO, PWM_PIN_MODE);   // 设置 PWM 对应的 GPIO 为 PWM 复用功能
    uapi_pwm_deinit();  // 反初始化 PWM（确保干净状态）
    uapi_pwm_init();    // 初始化 PWM 模块

    while(1){
    uapi_pwm_open(PWM_CHANNEL, &cfg_no_repeat); // 打开 PWM 通道并配置波形

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);   // 延时等待 1 秒（等待 PWM 稳定）
    uapi_pwm_unregister_interrupt(PWM_CHANNEL); // 取消旧的 PWM 中断回调（防止重复注册）
    uapi_pwm_register_interrupt(PWM_CHANNEL, pwm_sample_callback);  // 注册新的 PWM 完成回调函数

    uint8_t channel_id = PWM_CHANNEL;    // 要加入 PWM 组的通道编号
    // 将通道加入 PWM 组（组控制可以同时启动多个 PWM）
    uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    // 启动 PWM 组（内部包含 1 个通道）
    uapi_pwm_start_group(PWM_GROUP_ID);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);   // 输出 1 秒 PWM 波形
    }

    uapi_pwm_close(PWM_GROUP_ID);   // 关闭 PWM 组（停止输出）

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);   // 再延时 1 秒
    uapi_pwm_deinit();  // 反初始化 PWM（关闭 PWM 模块）

    return NULL;
}

static void tasks_test_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "TasksTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASKS_TEST_TASK_STACK_SIZE;
    attr.priority = TASKS_TEST_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)tasks_test_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

/* Run the tasks_test_entry. */
app_run(tasks_test_entry);
