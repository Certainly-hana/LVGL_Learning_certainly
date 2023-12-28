#ifndef __MAXIIC_H
#define __MAXIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO方向设置
#define MAX_SDA_IN()  max_i2c_sda_mode(GPIO_Mode_IN)	
#define MAX_SDA_OUT() max_i2c_sda_mode(GPIO_Mode_OUT)

//IO操作函数	 
#define MAX_IIC_SCL    PBout(8) //SCL
#define MAX_IIC_SDA    PBout(9) //SDA	 
#define MAX_READ_SDA   PBin(9)  //输入SDA 

//IIC所有操作函数
void MAX_IIC_Init(void);                //初始化IIC的IO口				 
void MAX_IIC_Start(void);				//发送IIC开始信号
void MAX_IIC_Stop(void);	  			//发送IIC停止信号
void MAX_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 MAX_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MAX_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MAX_IIC_Ack(void);					//IIC发送ACK信号
void MAX_IIC_NAck(void);				//IIC不发送ACK信号

void MAX_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
void MAX_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

void MAX_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
void MAX_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);

void max_i2c_sda_mode(GPIOMode_TypeDef pin_mode);
#endif
















