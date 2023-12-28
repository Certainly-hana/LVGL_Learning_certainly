#include "sys.h"
#include "usart.h"	
 
#include "includes.h"					//ucos 使用	  


 volatile uint8_t  g_usart1_buf[128]={0};
 volatile uint32_t g_usart1_cnt=0;
volatile uint32_t		g_usart1_event=0;
 
volatile uint8_t		g_usart3_buf[32]={0};
volatile uint32_t		g_usart3_event=0;

volatile uint8_t		g_usart2_buf[32]={0};
volatile uint32_t		g_usart2_event=0;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;   
	
	//清空发送完成标志位
	USART1->SR = (uint16_t)~USART_FLAG_TC;
	return ch;
}

 
//初始化IO 串口1 
//baud:波特率
void usart_init(u32 baud)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baud;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

	
}




void usart3_init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_Initstructure;
	
	//打开PA端口硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	
	//打开串口1硬件时钟

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//配置PA9和PA10配置为复用功能

	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PA9和PA10连接到串口1的硬件
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	//配置串口1的相关参数：波特率、无校验位、数据位
   USART_InitStructure.USART_BaudRate = baud;						//波特率
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8位数据位
   USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1位停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//允许收发数据
   
   USART_Init(USART3,&USART_InitStructure);

	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);			//设置接收中断
	
	//配置串口1的中断优先级
	NVIC_Initstructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_Initstructure);
	//使能串口
	
	USART_Cmd(USART3,ENABLE);

}

//usart2 for heart rate and spo2 value
void usart2_init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_Initstructure;
	
	//打开PA端口硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	
	//打开串口1硬件时钟

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//配置PA9和PA10配置为复用功能

	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA2和PA3连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//配置串口1的相关参数：波特率、无校验位、数据位
   USART_InitStructure.USART_BaudRate = baud;						//波特率
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8位数据位
   USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1位停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//允许收发数据
   
   USART_Init(USART2,&USART_InitStructure);

	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);			//设置接收中断
	
	//配置串口1的中断优先级
	NVIC_Initstructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_Initstructure);
	//使能串口
	
	USART_Cmd(USART2,ENABLE);

}


//usart3 data send func
void usart3_send(char *str)
{
	char *p = str;
	while(*p!='\0')
	{
		USART_SendData(USART3,*p);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)== RESET);
		USART_ClearFlag(USART3,USART_FLAG_TXE);
	
		//指针偏移
		p++;
	}


}

//usart2 heart rate and spo2 data send func
void usart2_send(char *str)
{
	char *p = str;
	while(*p!='\0')
	{
		USART_SendData(USART2,*p);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET);
		USART_ClearFlag(USART2,USART_FLAG_TXE);
	
		//指针偏移
		p++;
	}


}





void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t d=0;
	
	
	//进入中断
	OSIntEnter();    

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		//接收串口数据
		d=USART_ReceiveData(USART1);	
		
		g_usart1_buf[g_usart1_cnt] = d;
		
		g_usart1_cnt++;

		if(d == '#'|| g_usart1_cnt>= sizeof(g_usart1_buf))
		{
			g_usart1_cnt=0;
			g_usart1_event =1;
		}		
		
		
		//清空串口接收中断标志位
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	} 
	

	
	//退出中断
	OSIntExit();    
} 


 

//串口3中断服务函数
void USART3_IRQHandler(void)
{
	static uint32_t i=0;
	uint8_t d;
	//进入中断
	OSIntEnter(); 
	//检测标志位
	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET)
	{
		//接收数据
		d = USART_ReceiveData(USART3);
		
		g_usart3_buf[i] = d;
		i++;

		//判断是否有结束符'#',同时检测有没有超过缓冲区大小
		if(d=='#'||i>= sizeof(g_usart3_buf))
		{
			i=0;
			g_usart3_event =1;
		
		}
		
		//通过串口1发送
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)== RESET);
		USART_ClearFlag(USART1,USART_FLAG_TXE);
//	
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	
	}
	//退出中断
	OSIntExit(); 

}

void USART2_IRQHandler(void)
{
	static uint32_t i=0;
	uint8_t d;
	//进入中断
	OSIntEnter(); 
	//检测标志位
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		//接收数据
		d = USART_ReceiveData(USART2);
		
		g_usart2_buf[i] = d;
		i++;

		//判断是否有结束符'#',同时检测有没有超过缓冲区大小
		if(d=='#'||i>= sizeof(g_usart2_buf))
		{
			i=0;
			g_usart2_event =1;
		
		}
		
		//通过串口1发送
//		USART_SendData(USART1,d);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)== RESET);
//		USART_ClearFlag(USART1,USART_FLAG_TXE);
//	
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	
	}
	//退出中断
	OSIntExit(); 

}






