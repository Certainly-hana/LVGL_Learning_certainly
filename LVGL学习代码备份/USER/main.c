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
//����1���ƿ�
OS_TCB Task1_TCB;

void task1(void *parg);

CPU_STK task1_stk[1024];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�

//����2���ƿ�
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[512];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�


//����MPU6050���ƿ�
OS_TCB Task_MPU_TCB;

void task_mpu(void *parg);

CPU_STK task_mpu_stk[512];			//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�

//��ʪ��������ƿ�
OS_TCB Task_Dht11_TCB;
void task_dht11(void *parg);
CPU_STK task_dht11_stk[512];	

//RTC���������
OS_TCB Task_RTC_TCB;
void task_rtc(void *parg);
CPU_STK task_rtc_stk[512];	

//����״̬���ƿ�
OS_TCB app_task_tcb_sta;
void app_task_sta(void *parg);
CPU_STK app_task_stk_sta[512];			//�����ջ����СΪ512�֣�Ҳ����1024�ֽ�



OS_FLAG_GRP				g_flag_grp;			//�¼���־��Ķ���
OS_MUTEX 				g_lvmux;			//lvgl������
OS_MUTEX 				g_lvget_data;			//lvgl��ȡ���ݻ�����
OS_MUTEX				g_mutex_printf;		//�������Ķ���

OS_Q					g_queue_usart1;		//��Ϣ���еĶ���


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
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	systick_init(); //��ʼ����ʱ����
	usart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(9600);
	usart2_init(9600);
//	TIM3_Int_Init(999,83);
	
	
	
	LED_Init();					//��ʼ��LED 
 
	KEY_Init(); 				//������ʼ��  
//	rtc_init();
	rtc_flash_write();
	dht11_init();			//DHT11��ʪ�ȳ�ʼ��

	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	
//	lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//	lv_obj_set_size(switch_obj,120,60);
//	lv_obj_align(switch_obj,LV_ALIGN_CENTER,0,0);
	//OS��ʼ�������ǵ�һ�����еĺ���,��ʼ�����ֵ�ȫ�ֱ����������ж�Ƕ�׼����������ȼ����洢��
	OSInit(&err);
	
	
	
	
	//��������1
	OSTaskCreate(	(OS_TCB *)&Task1_TCB,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"Task1",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)task1,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)6,											 	//��������ȼ�6		
					(CPU_STK *)task1_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					//��������1
	OSTaskCreate(	(OS_TCB *)&Task2_TCB,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"Task2",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)task2,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)6,											 	//��������ȼ�6		
					(CPU_STK *)task2_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//û���κ�ѡ��
					&err													//���صĴ�����
				);

	//������ʪ������	
	OSTaskCreate(	(OS_TCB *)&Task_Dht11_TCB,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"task_dht11",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)task_dht11,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)7,											 	//��������ȼ�6		
					(CPU_STK *)task_dht11_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
	//����RTC����	
	OSTaskCreate(	(OS_TCB *)&Task_RTC_TCB,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"task_rtc",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)task_rtc,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)7,											 	//��������ȼ�6		
					(CPU_STK *)task_rtc_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//û���κ�ѡ��
					&err													//���صĴ�����
				);				
										//��������mpu
	OSTaskCreate(	(OS_TCB *)&Task_MPU_TCB,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"Task_mpu",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)task_mpu,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)7,											 	//��������ȼ�6		
					(CPU_STK *)task_mpu_stk,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
		//��������
	OSTaskCreate(	&app_task_tcb_sta,							//������ƿ�
					"app_task_sta",								//���������
					app_task_sta,								//������
					0,											//���ݲ���
					7,											//��������ȼ�7		
					app_task_stk_sta,							//�����ջ����ַ
					512/10,										//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					256,										//�����ջ��С			
					0,											//��ֹ������Ϣ����
					0,											//Ĭ������ռʽ�ں�																
					0,											//����Ҫ�����û��洢��
					OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,	//������ջ�������������ջ
					&err										//���صĴ�����
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

		//��ȡ��ʪ��
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
		//��ȡʱ��
		RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
		//��ȡ����
		RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
		
		
		//��ӡ����ʱ��
		
		
//		dgb_printf_safe("date:20%02x/%02x/%02x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date);
//		dgb_printf_safe("time:%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
		sprintf(buf,"%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
		lv_get_time(buf);		
		if(RTC_DateStructure.RTC_WeekDay == 1)
		{
//			printf("����һ\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==2)
		{
//			printf("���ڶ�\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==3)
		{
//			printf("������\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==4)
		{
//			printf("������\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==5)
		{
//			printf("������\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==6)
		{
//			printf("������\r\n");
		}else if(RTC_DateStructure.RTC_WeekDay ==7)
		{
//			printf("������\r\n");
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
		
		//��ȡ��
		p=strtok(NULL,"-");
		duty = atoi(p)-2000;
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Year = duty;
		//��ȡ�·�
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Month = duty;
		//��ȡ����
		p=strtok(NULL,"-");
		duty = atoi(p);
		duty = (duty/10)*16+duty%10;
		RTC_DateStructure.RTC_Date = duty;
		//��ȡ
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
//			//ʱ
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
//			//��
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

	float pitch,roll,yaw; 		//ŷ����
//	char mpu_data[32]={0};	//���ݻ���


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


			while(mpu_dmp_init())		//��ʼ��MPU DMP
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









