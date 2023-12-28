#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "touch.h" 
//#include "timer.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "dht11.h"
#include "rtc.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"


//#include "lv_demo_widgets.h"
//#include "lv_demo_music.h"
#include "my_gui.h"
#include "includes.h"








uint8_t duty = 0;
//任务1控制块
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[1024];			//任务1的任务堆栈，大小为128字，也就是512字节

//任务2控制块
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[512];			//任务1的任务堆栈，大小为128字，也就是512字节


//任务MPU6050控制块
OS_TCB Task_MPU_TCB;

void task_mpu(void *parg);

CPU_STK task_mpu_stk[512];			//任务1的任务堆栈，大小为128字，也就是512字节

//温湿度任务控制块
OS_TCB Task_Dht11_TCB;
void task_dht11(void *parg);
CPU_STK task_dht11_stk[512];	

//RTC控制任务块
OS_TCB Task_RTC_TCB;
void task_rtc(void *parg);
CPU_STK task_rtc_stk[512];	

//任务状态控制块
OS_TCB app_task_tcb_sta;
void app_task_sta(void *parg);
CPU_STK app_task_stk_sta[512];			//任务堆栈，大小为512字，也就是1024字节



OS_FLAG_GRP				g_flag_grp;			//事件标志组的对象
OS_MUTEX 				g_lvmux;			//lvgl互斥锁
OS_MUTEX 				g_lvget_data;			//lvgl获取数据互斥锁
OS_MUTEX				g_mutex_printf;		//互斥量的对象

OS_Q					g_queue_usart1;		//消息队列的对象


#define DEBUG_PRINTF_EN	1
void dgb_printf_safe(const char *format, ...)
{
#if DEBUG_PRINTF_EN	
	OS_ERR err;
	
	va_list args;
	va_start(args, format);
	
	OSMutexPend(&g_mutex_printf,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
	vprintf(format, args);
	OSMutexPost(&g_mutex_printf,OS_OPT_POST_NONE,&err);
	
	va_end(args);
#else
	(void)0;
#endif
}


int main(void)
{ 

	OS_ERR err;
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	systick_init(); //初始化延时函数
	usart_init(115200);		//初始化串口波特率为115200
	usart3_init(9600);
	usart2_init(9600);
//	TIM3_Int_Init(999,83);
	
	
	
	LED_Init();					//初始化LED 
 
	KEY_Init(); 				//按键初始化  
//	rtc_init();
	rtc_flash_write();
	dht11_init();			//DHT11温湿度初始化

	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	
//	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//	lv_obj_set_size(switch_obj,120,60);
//	lv_obj_align(switch_obj,LV_ALIGN_CENTER,0,0);
	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);
	
	
	
	
	//创建任务1
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//任务控制块，等同于线程id
					(CPU_CHAR *)"Task1",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)task1,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)6,											 	//任务的优先级6		
					(CPU_STK *)task1_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//没有任何选项
					&err													//返回的错误码
				);
					//创建任务1
	OSTaskCreate(	(OS_TCB *)&Task2_TCB,									//任务控制块，等同于线程id
					(CPU_CHAR *)"Task2",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)task2,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)6,											 	//任务的优先级6		
					(CPU_STK *)task2_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//没有任何选项
					&err													//返回的错误码
				);

	//创建温湿度任务	
	OSTaskCreate(	(OS_TCB *)&Task_Dht11_TCB,									//任务控制块，等同于线程id
					(CPU_CHAR *)"task_dht11",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)task_dht11,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)7,											 	//任务的优先级6		
					(CPU_STK *)task_dht11_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//没有任何选项
					&err													//返回的错误码
				);
	//创建RTC任务	
	OSTaskCreate(	(OS_TCB *)&Task_RTC_TCB,									//任务控制块，等同于线程id
					(CPU_CHAR *)"task_rtc",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)task_rtc,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)7,											 	//任务的优先级6		
					(CPU_STK *)task_rtc_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//没有任何选项
					&err													//返回的错误码
				);				
										//创建任务mpu
	OSTaskCreate(	(OS_TCB *)&Task_MPU_TCB,									//任务控制块，等同于线程id
					(CPU_CHAR *)"Task_mpu",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)task_mpu,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)7,											 	//任务的优先级6		
					(CPU_STK *)task_mpu_stk,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//没有任何选项
					&err													//返回的错误码
				);
		//创建任务
	OSTaskCreate(	&app_task_tcb_sta,							//任务控制块
					"app_task_sta",								//任务的名字
					app_task_sta,								//任务函数
					0,											//传递参数
					7,											//任务的优先级7		
					app_task_stk_sta,							//任务堆栈基地址
					512/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					256,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核																
					0,											//不需要补充用户存储区
					OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,	//开启堆栈检测与清空任务堆栈
					&err										//返回的错误码
				);					


	OSStart(&err);
	OSMutexCreate(&g_lvmux,"g_lvmux",&err);	
	OSMutexCreate(&g_lvget_data,"g_lvget_data",&err);	
	OSMutexCreate(&g_mutex_printf,"g_mutex_printf",&err);
					
					
				
	while(1);

}


void task1(void *parg)
{

	
	OS_ERR err;
//	
//	OSInit(&err);
//	lv_demo_music();
	my_gui();
	dgb_printf_safe("app_task_1 is create ok\r\n");
	while(1)
	{
		OSMutexPend(&g_lvmux,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		lv_timer_handler();
		OSMutexPost(&g_lvmux,OS_OPT_POST_NONE,&err);
		delay_ms(5);
	}
}



void task2(void *parg)
{

	dgb_printf_safe("task2 create\r\n");
	char *p=NULL;
//	char buf[20];
	uint8_t heart_val = 0;
	uint8_t spo2_val = 0;
	while(1)
	{
		usart2_send("AT+HEART\r\n");
		delay_ms(1000);
		usart2_send("AT+SPO2\r\n");
		delay_ms(1000);
	
	if(g_usart2_event)
	{
		if(strstr((char *)g_usart2_buf,"+HEART"))
		{
			p = strtok((char *)g_usart2_buf,"=");
			p=strtok(NULL,"=");
			heart_val = atoi(p);
//			dgb_printf_safe("Heart Rate = %d",heart_val);
		
			
		}
		
		else if(strstr((char *)g_usart2_buf,"+SPO2"))
		{
			p = strtok((char *)g_usart2_buf,"=");
			p=strtok(NULL,"=");
			spo2_val = atoi(p);
//			dgb_printf_safe("SPO2 = %d",spo2_val);

				
		}
		
			get_heart(heart_val,spo2_val);
			g_usart2_event = 0;
			memset((void *)g_usart2_buf,0,sizeof(g_usart2_buf));
	
	}
	
	
	}

}







void task_dht11(void *parg)
{
//	OS_ERR err;
				
//	int32_t rt; 
	
	uint8_t dht11_data[5]={0};
	
	char buf[20];	
	dgb_printf_safe("app_task_dht11 is create ok\r\n");
	while(1)
	{

		//获取温湿度
		dht11_read(dht11_data);
//		dgb_printf_safe(buf,"T:%02d.%d,H:%02d.%d",dht11_data[2],dht11_data[3],dht11_data[0],dht11_data[1]);
		//printf("T:%02d.%d,H:%02d.%d\r\n",dht11_data[2],dht11_data[3],dht11_data[0],dht11_data[1]);
		sprintf(buf,"T:%02d.%d,H:%02d.%d\r\n",dht11_data[2],dht11_data[3],dht11_data[0],dht11_data[1]);
		

		get_temp(buf,dht11_data[2],dht11_data[0]);	


		delay_ms(2000);
	}

}
void task_rtc(void *parg)
{
	

	char *p=NULL;
	char buf[20]={0};
	RTC_TimeTypeDef		RTC_TimeStructure;
	RTC_DateTypeDef		RTC_DateStructure;
//	RTC_AlarmTypeDef	RTC_AlarmStructure;
	

	
	dgb_printf_safe("app_task_rtc is create ok\r\n");
	while(1)
	{
		if(g_rtc_wakeup_event)
		{
		//获取时间
		RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
		//获取日期
		RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
		
		
		//打印日期时间
		
		
//		dgb_printf_safe("date:20%02x/%02x/%02x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date);
//		dgb_printf_safe("time:%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
		sprintf(buf,"%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
		lv_get_time(buf);		
		if(RTC_DateStructure.RTC_WeekDay == 1)
		{
//			printf("星期一\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==2)
		{
//			printf("星期二\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==3)
		{
//			printf("星期三\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==4)
		{
//			printf("星期四\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==5)
		{
//			printf("星期五\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==6)
		{
//			printf("星期六\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==7)
		{
//			printf("星期日\r\n");
		}
		g_rtc_wakeup_event = 0;
	}
	if(g_rtc_alarm_a_event)
	{
		for(int i=0;i<10;i++)
		{
			PFout(9)=0;PFout(10)=1;
			delay_ms(80);
		
			PFout(10)=0;PFout(9)=1;
			delay_ms(80);
		
		}
			PFout(9)=1;PFout(10)=1;
			delay_ms(80);
		g_rtc_alarm_a_event = 0;
	}
	
	if(g_usart3_event)
	{
	
		if(strstr((const char *)g_usart3_buf,"DATE"))
		{
		p = strtok((char *)g_usart3_buf,"-");
		
		//获取年
		p=strtok(NULL,"-");
		duty = atoi(p)-2000;
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Year = duty;
		//获取月份
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Month = duty;
		//获取日期
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Date = duty;
		//获取
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_WeekDay = duty;
	
		
			
		RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
		usart3_send("date set ok\r\n");
		printf("DATE SET OK \r\n");
		
		}
		else if(strstr((const char *)g_usart3_buf,"TIME"))
		{
		p = strtok((char *)g_usart3_buf,"-");
//		printf("p1=%s\r\n",p);
		
		p=strtok(NULL,"-");
//		printf("p2=%s\r\n",p);
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
			if(duty > 12)
			{
				RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
			
			}
				else if(duty <= 12)
			{
		
				RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
			}
		RTC_TimeStructure.RTC_Hours = duty;
			
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_TimeStructure.RTC_Minutes = duty;
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_TimeStructure.RTC_Seconds = duty;
			
		
		RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
		usart3_send("time set ok\r\n");
		printf("TIME SET OK \r\n");
		
		
		}
//		else if(strstr((const char *)g_usart3_buf,"ALARM"))
//		{
//			//ALARM SET
//			p = strtok((char *)g_usart3_buf,"-");
//			
//			//时
//			p=strtok(NULL,"-");
//			duty = atoi(p);
//			duty = (duty/10)*16+duty%10;
//			if(duty > 12)
//			{
//			RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_PM;
//			}
//			else if(duty <= 12)
//			{
//				RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
//			}
//			RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = duty;
//			//分
//			p=strtok(NULL,"-");
//			duty = atoi(p);
//			duty = (duty/10)*16+duty%10;
//			RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = duty;
//			
//			p=strtok(NULL,"-");
//			duty = atoi(p);
//			duty = (duty/10)*16+duty%10;
//			RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = duty;
//			
//			RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
//			
//			rtc_alarm_set(RTC_AlarmStructure);
//			usart3_send("alarm set ok please wait\r\n");
//			printf("ALARM SET OK PLEASE WAIT \r\n");
//		
//		}
		
	}

		g_usart3_event =0;
//		memset((void *)g_usart3_buf,0,sizeof(g_usart3_buf));
		delay_ms(500);
}	
}




void task_mpu(void *parg)
{
	
	uint8_t res;

	float pitch,roll,yaw; 		//欧拉角
//	char mpu_data[32]={0};	//数据缓存


	MPU6050_Init();
	while(mpu_dmp_init())
	{
		dgb_printf_safe("MPU6050 ERROR \r\n");
		delay_ms(500);
	}

	dgb_printf_safe("task_mpu create\r\n");
	while(1)
	{
//		GPIO_ResetBits(GPIOF,GPIO_Pin_9);
//		delay_ms(500);
//		GPIO_SetBits(GPIOF,GPIO_Pin_9);
	
	
			res=mpu_dmp_get_data(&pitch,&roll,&yaw);

			if(res==0)
			{ 
				dgb_printf_safe("pitch=%f,roll=%f,yaw=%f\r\n",pitch,roll,yaw);		
				if((yaw<90)&&(yaw>-90))
				{
					GPIO_ResetBits(GPIOF,GPIO_Pin_9);
				}
				else
				{
					GPIO_ResetBits(GPIOF,GPIO_Pin_9);
				}
			}	
		

		if(KEY1==0) 
		{


			while(mpu_dmp_init())		//初始化MPU DMP
			{
				delay_ms(20);
			}
			dgb_printf_safe("MPU 6050 INIT\r\n");
	
		}
	
		delay_ms(1000);
	}



}
void  app_task_sta(void *p_arg)  
{  
	OS_ERR err;  
	
	CPU_STK_SIZE free,used;  
	
	dgb_printf_safe("app_task_sta is create ok\r\n");	
	
	delay_ms(3000);
	
	while(1)  
	{  
		OSTaskStkChk (&Task1_TCB,&free,&used,&err); 
		dgb_printf_safe("task1   stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));

		OSTaskStkChk (&Task2_TCB,&free,&used,&err); 
		dgb_printf_safe("Task2_TCB   stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));  
			  			  
		OSTaskStkChk (&Task_Dht11_TCB,&free,&used,&err); 
		dgb_printf_safe("Task_Dht11_TCB stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));  

		OSTaskStkChk (&Task_RTC_TCB,&free,&used,&err); 
		dgb_printf_safe("Task_RTC_TCB    stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free)); 
	
		OSTaskStkChk (&Task_MPU_TCB,&free,&used,&err); 
		dgb_printf_safe("Task_MPU_TCB   stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));
	
//		OSTaskStkChk (&app_task_tcb_usart1,&free,&used,&err); 
//		dgb_printf_safe("app_task_usart1   stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));	
//		
//		OSTaskStkChk (&app_task_tcb_mpu6050,&free,&used,&err); 
//		dgb_printf_safe("app_task_mpu6050  stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));			

		OSTaskStkChk (&app_task_tcb_sta,&free,&used,&err); 
		dgb_printf_safe("app_task_sta   stk[used/free:%d/%d usage:%d%%]\r\n",used,free,(used*100)/(used+free));
		
		delay_ms(3000);
	}  
}  









