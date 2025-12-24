#ifndef __CONTROL_TASK_H__
#define __CONTROL_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern volatile char g_ctrl_mode;     // 控制模式：'S' / 'L'
extern volatile int  g_speed_value;   // 速度值
extern volatile char g_dir_value;     // 方向值：w s a d 0
extern volatile int color_mode;       // 颜色/状态指示模式
extern volatile int color_delay_time;        // 颜色切换延时
/**
 * @brief 蓝牙遥控任务
 */
void remote_control_task(void);

/**
 * @brief 避障任务
 */
void obstacle_avoid_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __CONTROL_TASK_H__ */
