#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "common_def.h"
#include "uart.h"
#include "errcode.h"

/* UART 接口引脚定义（根据 bluetooth.c） */
#define BLUETOOTH_UART_TX_PIN   7
#define BLUETOOTH_UART_RX_PIN   8

/* UART 缓冲区大小 */
#define TEST_UART_RX_BUFF_SIZE  0x1

#ifdef __cplusplus
extern "C" {
#endif

extern volatile char g_ctrl_mode;
extern volatile char g_dir_value;
extern volatile int  g_speed_value;

/* IO 配置函数 */
void usr_uart_io_config(void);

/* UART 初始化 */
errcode_t usr_uart_init_config(void);

/* UART 读写接口 */
char usr_uart_read_data(void);
int usr_uart_write_data(unsigned int size, char* buff);

/* UART 全局接收缓冲区声明（与 .c 内部一致） */
extern unsigned char g_uart_rx_buff[TEST_UART_RX_BUFF_SIZE];
extern uart_buffer_config_t g_uart_buffer_config;

#ifdef __cplusplus
}
#endif

#endif /* BLUETOOTH_H */
