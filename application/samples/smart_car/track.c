#include "gpio.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "hal_gpio.h"
#include "osal_debug.h"
void gpio_callback_func(pin_t pin, uintptr_t param)  
{
    unused(param);  
    // osal_printk("PIN:%d interrupt success. \r\n", pin);  
}

errcode_t track_init(pin_t pin)
{
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(pin, HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
    if(uapi_gpio_register_isr_func(pin, GPIO_INTERRUPT_RISING_EDGE, gpio_callback_func) != ERRCODE_SUCC)
    {
        uapi_gpio_unregister_isr_func(pin);
        return ERRCODE_FAIL;  
    }
    return ERRCODE_SUCC;
}

void hit_init(void)
{
    track_init(9);
    track_init(10);
    track_init(11);
}

int get_track_status(pin_t pin)
{
    gpio_level_t lv = uapi_gpio_get_val(pin);
    if(lv == GPIO_LEVEL_HIGH)
        return 1;
    else
        return 0;
}