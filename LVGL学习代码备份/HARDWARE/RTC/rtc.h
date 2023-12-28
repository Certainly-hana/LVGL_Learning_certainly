#ifndef __RTC_H
#define __RTC_H

extern void rtc_init(void);

extern  OS_FLAG_GRP			g_flag_grp;					//事件标志组的对象

extern volatile uint32_t 	g_rtc_wakeup_event;
extern volatile uint32_t 	g_rtc_alarm_a_event;
extern void rtc_flash_write(void);

extern void rtc_alarm_a_init(void);
extern void rtc_alarm_set(RTC_AlarmTypeDef RTC_AlarmStructure);
#endif

