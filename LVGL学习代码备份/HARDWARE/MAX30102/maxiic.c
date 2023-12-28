#include "maxiic.h"
#include "delay.h"
//GPIO初始化的结构体
static GPIO_InitTypeDef  GPIO_InitStructure;

void max_i2c_sda_mode(GPIOMode_TypeDef pin_mode)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//第9根引脚
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//不需要上下拉电阻	
	
	if(pin_mode == GPIO_Mode_OUT)
	{
		//配置PG9引脚为输出模式
		
		GPIO_InitStructure.GPIO_Mode = pin_mode;		//输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加驱动能力就是增加输出电流，还有吸收灌电流
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//工作速度100MHz，工作速度越高，功耗就会越高
		
	
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	//输入模式
	}

	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

//初始化IIC
void MAX_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //推挽输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出，增加驱动能力就是增加输出电流，还有吸收灌电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	
	MAX_IIC_SCL=1;
	MAX_IIC_SDA=1;

}
//产生IIC起始信号
void MAX_IIC_Start(void)
{
	MAX_SDA_OUT();     //sda线输出
	MAX_IIC_SDA=1;	  	  
	MAX_IIC_SCL=1;
	delay_us(4);
 	MAX_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	MAX_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void MAX_IIC_Stop(void)
{
	MAX_SDA_OUT();//sda线输出
	MAX_IIC_SCL=0;
	MAX_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	MAX_IIC_SCL=1; 
	MAX_IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MAX_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MAX_SDA_IN();      //SDA设置为输入  
	MAX_IIC_SDA=1;delay_us(1);	   
	MAX_IIC_SCL=1;delay_us(1);	 
	while(MAX_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MAX_IIC_Stop();
			return 1;
		}
	}
	MAX_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void MAX_IIC_Ack(void)
{
	MAX_IIC_SCL=0;
	MAX_SDA_OUT();
	MAX_IIC_SDA=0;
	delay_us(2);
	MAX_IIC_SCL=1;
	delay_us(2);
	MAX_IIC_SCL=0;
}
//不产生ACK应答		    
void MAX_IIC_NAck(void)
{
	MAX_IIC_SCL=0;
	MAX_SDA_OUT();
	MAX_IIC_SDA=1;
	delay_us(2);
	MAX_IIC_SCL=1;
	delay_us(2);
	MAX_IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MAX_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MAX_SDA_OUT(); 	    
    MAX_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        MAX_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		MAX_IIC_SCL=1;
		delay_us(2); 
		MAX_IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MAX_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MAX_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        MAX_IIC_SCL=0; 
        delay_us(2);
		MAX_IIC_SCL=1;
        receive<<=1;
        if(MAX_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        MAX_IIC_NAck();//发送nACK
    else
        MAX_IIC_Ack(); //发送ACK   
    return receive;
}


void MAX_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX_IIC_Start();  

	MAX_IIC_Send_Byte(WriteAddr);	    //发送写命令
	MAX_IIC_Wait_Ack();
	
	for(i=0;i<dataLength;i++)
	{
		MAX_IIC_Send_Byte(data[i]);
		MAX_IIC_Wait_Ack();
	}				    	   
    MAX_IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

void MAX_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX_IIC_Start();  

	MAX_IIC_Send_Byte(deviceAddr);	    //发送写命令
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(writeAddr);
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(deviceAddr|0X01);//进入接收模式			   
	MAX_IIC_Wait_Ack();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = MAX_IIC_Read_Byte(1);
	}		
	data[dataLength-1] = MAX_IIC_Read_Byte(0);	
    MAX_IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

void MAX_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data)
{				  	  	    																 
    MAX_IIC_Start();  
	
	MAX_IIC_Send_Byte(daddr);	   //发送写命令
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(addr);//发送地址
	MAX_IIC_Wait_Ack();		 
	MAX_IIC_Start();  	 	   
	MAX_IIC_Send_Byte(daddr|0X01);//进入接收模式			   
	MAX_IIC_Wait_Ack();	 
    *data = MAX_IIC_Read_Byte(0);		   
    MAX_IIC_Stop();//产生一个停止条件	    
}

void MAX_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{				   	  	    																 
    MAX_IIC_Start();  
	
	MAX_IIC_Send_Byte(daddr);	    //发送写命令
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(addr);//发送地址
	MAX_IIC_Wait_Ack();	   	 										  		   
	MAX_IIC_Send_Byte(data);     //发送字节							   
	MAX_IIC_Wait_Ack();  		    	   
    MAX_IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}



























