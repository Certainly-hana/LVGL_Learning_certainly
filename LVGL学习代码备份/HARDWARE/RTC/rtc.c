#include "sys.h"
#include "includes.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"



 volatile uint32_t 	g_rtc_alarm_a_event = 0;

volatile uint32_t 	g_rtc_wakeup_event=0;

void rtc_init(void)
{

	EXTI_InitTypeDef		EXTI_InitStructure;
	NVIC_InitTypeDef		NVIC_Initstructure;
	RTC_DateTypeDef		RTC_DateStructure;
	RTC_InitTypeDef		RTC_InitStructure;
	RTC_TimeTypeDef		RTC_TimeStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	
	PWR_BackupAccessCmd(ENABLE);
	
	//使能LSE震荡时钟源
	RCC_LSEConfig(RCC_LSE_ON);
	
	//等待外部晶振就绪(官方代码要添加超时)
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)
	
	//选择LSE作为RTC的外部震荡时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//使能RTC的硬件时钟
	RCC_RTCCLKCmd(ENABLE);
	
	//等待所有寄存器就绪
	RTC_WaitForSynchro();
	
	/* 
	ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)/(uwSynchPrediv + 1)
	
				 = 32768/128/256=1
	*/
	
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;		//异步分频
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;			//同步分频
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);

	
	RTC_DateStructure.RTC_Year = 0x23;					//年份配置
	RTC_DateStructure.RTC_Month = RTC_Month_August;
	RTC_DateStructure.RTC_Date = 0x29;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday ;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 18h 53mn 00s AM */
	RTC_TimeStructure.RTC_H12  	  = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x15;
	RTC_TimeStructure.RTC_Minutes = 0x27;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
	
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认是0
	RTC_SetWakeUpCounter(0);

	//清除RTC唤醒中断标志（一秒唤醒一次来获取时间，需要先清空标志位）
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	
	//使能RTC唤醒中断（WUT，wake up timer）
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能唤醒功能（配置完毕打开唤醒功能）
	RTC_WakeUpCmd(ENABLE);
//	
//	RTC_ClearFlag(RTC_FLAG_WUTF);
	
//配置中断属性
	
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//使能RTC唤醒中断
	NVIC_Initstructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Initstructure);

	
}


void rtc_flash_write(void)
{
	EXTI_InitTypeDef		EXTI_InitStructure;
	NVIC_InitTypeDef		NVIC_Initstructure;

	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x32F2)
	{
		//rtc初始化
		rtc_init();
	
		
		//设置备份寄存器
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
		
		
	}
	else{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	
	PWR_BackupAccessCmd(ENABLE);
	
	//使能LSE震荡时钟源
	RCC_LSEConfig(RCC_LSE_ON);
	
	//等待外部晶振就绪(官方代码要添加超时)
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET)
	
	//选择LSE作为RTC的外部震荡时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//使能RTC的硬件时钟
	RCC_RTCCLKCmd(ENABLE);
	
	//等待所有寄存器就绪
	RTC_WaitForSynchro();

	//清除RTC唤醒中断标志（一秒唤醒一次来获取时间，需要先清空标志位）
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断（WUT，wake up timer）
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能唤醒功能（配置完毕打开唤醒功能）
	RTC_WakeUpCmd(ENABLE);
	
	
//配置中断属性
	
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//使能RTC唤醒中断
	NVIC_Initstructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Initstructure);

	
	
	}

}




void alarm_init(void)
{
	EXTI_InitTypeDef		EXTI_InitStructure;
	RTC_AlarmTypeDef		RTC_AlarmStructure;
	NVIC_InitTypeDef		NVIC_Initstructure;
	//配置闹钟前需要关闭闹钟
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x15;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x28;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x10;
	
#if 0	//每天生效
//	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
//	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;	//屏蔽日期和时间
#endif

#if 0	//指定某一天生效
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x29;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;		//不屏蔽
#endif
	
#if 1	//指定某一星期生效
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Monday;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;		//不屏蔽
#endif
	
	
	//配置闹钟A
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	//打开闹钟
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	

	/* EXTI configuration */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable the RTC Alarm Interrupt */
	NVIC_Initstructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Initstructure);
	

	
	/* Set alarm A sub seconds and enable SubSec Alarm : generate 8 interrupts per Second */
	RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);
	
	/* Enable alarm A interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);



}

void rtc_alarm_set(RTC_AlarmTypeDef RTC_AlarmStructure)
{
		RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
		RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
		RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
}



void RTC_WKUP_IRQHandler(void)
{

	OSIntEnter();	
	//检测标志位
	if(RTC_GetITStatus(RTC_IT_WUT)==SET)
	{
//		printf("RTC_IRQHandler\r\n");
		g_rtc_wakeup_event = 1;
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
	
	OSIntExit();
	
	

}




void RTC_Alarm_IRQHandler(void)
{
	//检测标志位
	if(RTC_GetITStatus(RTC_IT_ALRA)==SET)
	{
//		printf("RTC_IRQHandler\r\n");
		
		g_rtc_alarm_a_event = 1;
		//清空标志位
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
	
	
	
	

}

