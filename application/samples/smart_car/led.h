#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void led_init(void);

/* GPIO 方式设置 RGB（0/1） */
void rgb_set_gpio(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif
