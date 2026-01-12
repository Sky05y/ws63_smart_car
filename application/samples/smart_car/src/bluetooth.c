#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "uart.h"
#include "uart_porting.h"
#include "osal_debug.h"
#include "joy.h"
#define TEST_UART_RX_BUFF_SIZE 64 /* 定义 UART 接收缓存区大小 */  
#define UART_BAUDRATE   9600
#define UART_DATA_BITS  8
#define UART_STOP_BITS  1
#define UART_PARITY_BIT 0

volatile char g_ctrl_mode = 0;     // 'L' or 'S'
volatile char g_dir_value = 0;     // w s a d 0
volatile int  g_speed_value = 80;  // 00~99 默认速度
volatile char g_work_mode = 'R';   // 'R' 遥控器模式，'Y' 避障模式
volatile bool is_play_music = false;

unsigned char g_uart_rx_buff[TEST_UART_RX_BUFF_SIZE] = { 0 };  
uart_buffer_config_t g_uart_buffer_config = {  
    .rx_buffer = g_uart_rx_buff,  
    .rx_buffer_size = TEST_UART_RX_BUFF_SIZE  
};

void usr_uart_init_config(void)  
{
    uapi_pin_set_mode(S_MGPIO15, PIN_MODE_1); /* uart1 tx */  
    uapi_pin_set_mode(S_MGPIO16, PIN_MODE_1); /* uart1 rx */  
    errcode_t errcode;  
    uart_attr_t attr = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };
    uart_pin_config_t pin_config = {  
        .tx_pin = S_MGPIO15, /* uart1 tx */  
        .rx_pin = S_MGPIO16, /* uart1 rx */  
    };
    uapi_uart_deinit(UART_BUS_1);
    uapi_uart_init(UART_BUS_1, &pin_config, &attr, NULL, &g_uart_buffer_config);  

    return;
}

void usr_uart_read_data(void)
{
    static char frame[5];
    static int pos = 0;

    unsigned char buf[64];
    int len = uapi_uart_read(UART_BUS_1, buf, 64, 0);
    if (len <= 0) return;

    for (int i = 0; i < len; i++) 
    {
        char c = buf[i];
        
        /* 帧起始 */
        if (c == 'L' || c == 'S') {
            pos = 0;
            frame[pos++] = c;
            continue;
        }
        /* 模式切换 */
        else if (c == 'R' || c == 'Y') {
            g_work_mode = c;
            pos = 0;
            continue;
        }
        else if (c == 'U'){
            printf("Receive Music Start Command\r\n");
            is_play_music = true;
            pos = 0;
            continue;
        }
        else if (c == 'T'){
            is_play_music = false;
            pos = 0;
            continue;
        }
        else if (c >= '0' && c <= '8') {
            // LED 颜色模式设置
            color_mode = c - '0';
            pos = 0;
            continue;
        }
        if (pos > 0) {
            frame[pos++] = c;
        }
        /* Lx* */
        if (pos == 3 && frame[0] == 'L' && frame[2] == '*') {
            g_ctrl_mode = 'L';
            g_dir_value = frame[1];
            pos = 0;
        }

        /* Sxx* */
        else if (pos == 4 && frame[0] == 'S' && frame[3] == '*') {
            if (frame[1] >= '0' && frame[1] <= '9' &&
                frame[2] >= '0' && frame[2] <= '9') {

                g_ctrl_mode = 'S';
                g_speed_value =
                    (frame[1] - '0') * 10 + (frame[2] - '0');
            }
            pos = 0;
        }

        /* 防止异常 */
        if (pos >= 5) {
            pos = 0;
        }
    }
}

int usr_uart_write_data(unsigned int size, char* buff)  
{  
    unsigned char tx_buff[10] = { 0 };  
    if (memcpy_s(tx_buff, 10, buff, size) != EOK) {  
        return ERRCODE_FAIL;  
    }  
    int ret = uapi_uart_write(UART_BUS_1, tx_buff, size, 0);  
    if(ret == -1) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}