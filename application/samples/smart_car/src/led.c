#include "led.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "gpio.h"

/* RGB GPIO 定义 */
#define LED_R_GPIO   0
#define LED_G_GPIO   13
#define LED_B_GPIO   14

rgb_bit_t led_colors[] = {
    {1, 0, 0},   // 红   100
    {0, 1, 0},   // 绿   010
    {0, 0, 1},   // 蓝   001
    {1, 1, 0},   // 黄   110
    {1, 0, 1},   // 紫   101
    {0, 1, 1},   // 青   011
    {1, 1, 1},   // 白   111
};

void led_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();

    /* R */
    uapi_pin_set_mode(LED_R_GPIO, PIN_MODE_0);
    uapi_gpio_set_dir(LED_R_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(LED_R_GPIO, GPIO_LEVEL_LOW);

    /* G */
    uapi_pin_set_mode(LED_G_GPIO, PIN_MODE_0);
    uapi_gpio_set_dir(LED_G_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(LED_G_GPIO, GPIO_LEVEL_LOW);

    /* B */
    uapi_pin_set_mode(LED_B_GPIO, PIN_MODE_0);
    uapi_gpio_set_dir(LED_B_GPIO, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(LED_B_GPIO, GPIO_LEVEL_LOW);
}

/**
 * @brief 按 RGB 位控制 LED（0/1）
 * @param r 0 或 1
 * @param g 0 或 1
 * @param b 0 或 1
 */
void rgb_set_gpio(uint8_t r, uint8_t g, uint8_t b)
{
    uapi_gpio_set_val(LED_R_GPIO, r ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LED_G_GPIO, g ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
    uapi_gpio_set_val(LED_B_GPIO, b ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}
