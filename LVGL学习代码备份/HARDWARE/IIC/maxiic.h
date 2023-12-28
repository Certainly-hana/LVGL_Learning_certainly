#ifndef __MAXIIC_H
#define __MAXIIC_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO��������
#define max_SDA_IN()  {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=0<<0*2;}	//PB9����ģʽ
#define max_SDA_OUT() {GPIOF->MODER&=~(3<<(0*2));GPIOF->MODER|=1<<0*2;} //PB9���ģʽ
//IO��������	 
#define max_IIC_SCL    PFout(1) //SCL
#define max_IIC_SDA    PFout(0) //SDA	 
#define max_READ_SDA   PFin(0)  //����SDA 

//IIC���в�������
void max_IIC_Init(void);                //��ʼ��IIC��IO��				 
void max_IIC_Start(void);				//����IIC��ʼ�ź�
void max_IIC_Stop(void);	  			//����IICֹͣ�ź�
void max_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 max_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 max_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void max_IIC_Ack(void);					//IIC����ACK�ź�
void max_IIC_NAck(void);				//IIC������ACK�ź�

void max_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void max_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

void max_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
void max_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);

#endif
















