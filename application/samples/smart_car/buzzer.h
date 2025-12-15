#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化无源蜂鸣器（PWM）
 */
void buzzer_init(void);

/**
 * @brief 播放测试歌曲（在 buzzer.c 中实现）
 */
void buzzer_play_song(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H__ */
