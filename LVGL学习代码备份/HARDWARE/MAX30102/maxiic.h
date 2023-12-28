#ifndef __MAXIIC_H
#define __MAXIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO��������
#define MAX_SDA_IN()  max_i2c_sda_mode(GPIO_Mode_IN)	
#define MAX_SDA_OUT() max_i2c_sda_mode(GPIO_Mode_OUT)

//IO��������	 
#define MAX_IIC_SCL    PBout(8) //SCL
#define MAX_IIC_SDA    PBout(9) //SDA	 
#define MAX_READ_SDA   PBin(9)  //����SDA 

//IIC���в�������
void MAX_IIC_Init(void);                //��ʼ��IIC��IO��				 
void MAX_IIC_Start(void);				//����IIC��ʼ�ź�
void MAX_IIC_Stop(void);	  			//����IICֹͣ�ź�
void MAX_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 MAX_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MAX_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MAX_IIC_Ack(void);					//IIC����ACK�ź�
void MAX_IIC_NAck(void);				//IIC������ACK�ź�

void MAX_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
void MAX_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

void MAX_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
void MAX_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);

void max_i2c_sda_mode(GPIOMode_TypeDef pin_mode);
#endif
















