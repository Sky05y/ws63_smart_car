#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "uart.h"
#include "uart_porting.h"
#include "osal_debug.h"
#define TEST_UART_RX_BUFF_SIZE 64 /* 定义 UART 接收缓存区大小 */  
#define UART_BAUDRATE   9600
#define UART_DATA_BITS  8
#define UART_STOP_BITS  1
#define UART_PARITY_BIT 0

volatile char g_ctrl_mode = 0;     // 'L' or 'S'
volatile char g_dir_value = 0;     // w s a d 0
volatile int  g_speed_value = 80;  // 00~99 默认速度
void usr_uart_io_config(void)
{  
    /* 如下IO复用配置，也可集中在SDK中的usr_io_init函数中进行配置 */  
    uapi_pin_set_mode(S_MGPIO15, PIN_MODE_1); /* uart1 tx */  
    errcode_t tt = uapi_pin_set_mode(S_MGPIO16, PIN_MODE_1); /* uart1 rx */  
    if (tt != ERRCODE_SUCC) {
        osal_printk("pin set mode fail\r\n");
    }
    else {
        osal_printk("pin set mode success\r\n");
    }
}

unsigned char g_uart_rx_buff[TEST_UART_RX_BUFF_SIZE] = { 0 };  
uart_buffer_config_t g_uart_buffer_config = {  
    .rx_buffer = g_uart_rx_buff,  
    .rx_buffer_size = TEST_UART_RX_BUFF_SIZE  
};

errcode_t usr_uart_init_config(void)  
{  
    errcode_t errcode;  

    osal_printk("[UART] setting attr...\r\n");
    uart_attr_t attr = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    osal_printk("[UART] attr: baud=%d databits=%d stopbits=%d parity=%d\r\n",
    attr.baud_rate, attr.data_bits, attr.stop_bits, attr.parity);

    osal_printk("[UART] setting pin config...\r\n");
    uart_pin_config_t pin_config = {  
        .tx_pin = S_MGPIO15, /* uart1 tx */  
        .rx_pin = S_MGPIO16, /* uart1 rx */  
        // .cts_pin = 0xff,
        // .rts_pin = 0xff
    };

    osal_printk("[UART] pins: TX=%d RX=%d CTS=%d RTS=%d\r\n",
                pin_config.tx_pin, pin_config.rx_pin,
                pin_config.cts_pin, pin_config.rts_pin);

    errcode = uapi_uart_deinit(UART_BUS_1);

    osal_printk("[UART] deinit returned: 0x%x\r\n", errcode);

    errcode = uapi_uart_init(UART_BUS_1, &pin_config, &attr, NULL, &g_uart_buffer_config);  
    if (errcode != ERRCODE_SUCC) {
        osal_printk("uart init fail\r\n,code=0x%x\r\n", errcode);  
    }
    else {
        osal_printk("uart init success\r\n");
    }

    osal_printk("=== usr_uart_init_config end ===\r\n");

    return errcode;
}

void usr_uart_read_data(void)
{
    static char frame[5];
    static int pos = 0;

    unsigned char buf[64];
    int len = uapi_uart_read(UART_BUS_1, buf, 64, 0);
    if (len <= 0) return;

    for (int i = 0; i < len; i++) {
        char c = buf[i];

        /* 帧起始 */
        if (c == 'L' || c == 'S') {
            pos = 0;
            frame[pos++] = c;
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