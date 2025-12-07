#ifndef TRACK_H
#define TRACK_H

#include "gpio.h"
#include "pinctrl_porting.h"
#include "errcode.h"
#include <stdint.h>
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

void gpio_callback_func(pin_t pin, uintptr_t param);

errcode_t track_init(pin_t pin);

int get_track_status(pin_t pin);

#ifdef __cplusplus
}
#endif

#endif
