#ifndef _PRINTF_H_
#define _PRINTF_H_
#define UART_BUFF_SIZE 100

extern void ino_print(char *s);
extern char __buff[UART_BUFF_SIZE];

#define ino_printf(...) snprintf(__buff,UART_BUFF_SIZE, ##__VA_ARGS__);ino_print(__buff);

/** macro: PRINTF_LEVEL
 *    range: 0-3
 *    0: Disable printf
 *    1: Only APP_INF messages and printf function calls
 *    2: Only APP_ERR messages
 *    3: APP_ERR messages and APP_DBG messages
 */
#define PRINTF_LEVEL                        3
#define APP_PRINTF_INFO(M, ...)             ino_printf("[INF] " M "\r\n", ##__VA_ARGS__)
#define APP_PRINTF_ERROR(M, ...)            ino_printf("[ERROR] %s:%d: " M "\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define APP_PRINTF_DEBUG(M, ...)            ino_printf("[DBG] %s:%s:%d: " M "\r\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#if   PRINTF_LEVEL == 1
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 2
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG(M,...)                      /* nothing */
#elif PRINTF_LEVEL == 3
#define APP_INF                             APP_PRINTF_INFO
#define APP_ERR                             APP_PRINTF_ERROR
#define APP_DBG                             APP_PRINTF_DEBUG
#else
#define APP_INF(M,...)                      /* nothing */
#define APP_ERR(M,...)                      /* nothing */
#define APP_DBG(M,...)                      /* nothing */
#define printf(M,...)                       /* nothing */
#endif
#define APP_DBG_H2(x)                       APP_DBG(#x ": 0x%02x", (x))
#define APP_DBG_H8(x)                       APP_DBG(#x ": 0x%08x", (x))
#define APP_DBG_D(x)                        APP_DBG(#x ": %d"    , (x))
#define APP_DBG_U(x)                        APP_DBG(#x ": %u"    , (x))
#define APP_DBG_F8(x)                       APP_DBG(#x ": 0x%08f", (x))
#define APP_DBG_F2(x)                       APP_DBG(#x ": 0x%02f", (x))
#define APP_DBG_S(x)                        APP_DBG(#x ": %s"    , (x))
#define INPUT_YES                           1
#define INPUT_NO                            0

#endif
