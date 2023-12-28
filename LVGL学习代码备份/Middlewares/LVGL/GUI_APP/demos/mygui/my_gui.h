#ifndef _MY_GUI_H
#define _MY_GUI_H
#include "sys.h"

void my_gui(void);
void get_temp(char *temp_value,uint8_t temp,uint8_t humi);

void lv_get_time(char *rtc_time);

void set_chart(uint16_t y,uint16_t humi_y);
void get_heart(uint8_t heart_value,uint8_t spo2_value);

#endif // _MY_GUI_H

