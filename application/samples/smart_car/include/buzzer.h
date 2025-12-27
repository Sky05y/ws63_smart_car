#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <stdint.h>
#include <stddef.h>  // 增加NULL指针定义所需的头文件

// 添加音乐音符频率定义（中音区）
#define NOTE_C4  262   // 哆
#define NOTE_D4  294   // 来
#define NOTE_E4  330   // 咪
#define NOTE_F4  349   // 发
#define NOTE_G4  392   // 索
#define NOTE_A4  440   // 拉
#define NOTE_B4  494   // 西
#define NOTE_C5  523   // 高音哆
#define REST     0     // 休止符

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化蜂鸣器（配置GPIO和PWM）
 */
void buzzer_init(void);

/**
 * @brief 播放指定频率的音调
 * @param frequency 音调频率（Hz），0表示停止发声
 * @param duration_ms 播放时长（ms），0表示持续播放
 */
void buzzer_play_tone(uint16_t frequency, uint32_t duration_ms);

/**
 * @brief 播放单个音符（封装tone函数，语义更友好）
 * @param note 音符频率（可使用NOTE_xxx宏），REST表示休止符
 * @param duration_ms 播放时长（ms）
 */
void buzzer_play_note(uint16_t note, uint32_t duration_ms);

/**
 * @brief 播放一段旋律
 * @param notes 音符数组（频率值，可使用NOTE_xxx/REST宏）
 * @param durations 时长数组（对应每个音符的播放时长，ms）
 * @param length 音符/时长数组的元素个数
 */
void buzzer_play_music(const uint16_t *notes, const uint32_t *durations, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // __BUZZER_H__