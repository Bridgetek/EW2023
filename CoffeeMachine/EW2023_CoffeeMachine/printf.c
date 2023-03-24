#include "printf.h"

char __buff[UART_BUFF_SIZE];
// Put this in .ino: void ino_print(char *s){Serial.print(s);}
// include: printf.h
// usage:
//     printf()
//     APP_DBG
//     APP_INF
//     APP_ERR