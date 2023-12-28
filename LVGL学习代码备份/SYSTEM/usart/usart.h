#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "includes.h"

extern volatile uint8_t  g_usart1_buf[128];
extern volatile uint32_t g_usart1_event;


extern void usart_init(uint32_t baud);
extern void usart3_init(uint32_t baud);
extern void usart3_send(char *str);
extern volatile uint8_t		g_usart3_buf[32];
extern volatile uint32_t		g_usart3_event;
extern void usart2_init(uint32_t baud);

extern volatile uint8_t		g_usart2_buf[32];
extern volatile uint32_t		g_usart2_event;

extern void usart2_send(char *str);

#endif


