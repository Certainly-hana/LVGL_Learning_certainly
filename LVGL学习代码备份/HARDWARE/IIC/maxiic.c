#include "maxiic.h"
#include "delay.h"
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

//��ʼ��IIC
void max_IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOBʱ��

  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	max_IIC_SCL=1;
	max_IIC_SDA=1;
}
//����IIC��ʼ�ź�
void max_IIC_Start(void)
{
	max_SDA_OUT();     //sda�����
	max_IIC_SDA=1;	  	  
	max_IIC_SCL=1;
	delay_us(4);
 	max_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	max_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void max_IIC_Stop(void)
{
	max_SDA_OUT();//sda�����
	max_IIC_SCL=0;
	max_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	max_IIC_SCL=1; 
	max_IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 max_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	max_SDA_IN();      //SDA����Ϊ����  
	max_IIC_SDA=1;delay_us(1);	   
	max_IIC_SCL=1;delay_us(1);	 
	while(max_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			max_IIC_Stop();
			return 1;
		}
	}
	max_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void max_IIC_Ack(void)
{
	max_IIC_SCL=0;
	max_SDA_OUT();
	max_IIC_SDA=0;
	delay_us(2);
	max_IIC_SCL=1;
	delay_us(2);
	max_IIC_SCL=0;
}
//������ACKӦ��		    
void max_IIC_NAck(void)
{
	max_IIC_SCL=0;
	max_SDA_OUT();
	max_IIC_SDA=1;
	delay_us(2);
	max_IIC_SCL=1;
	delay_us(2);
	max_IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void max_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	max_SDA_OUT(); 	    
    max_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        max_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		max_IIC_SCL=1;
		delay_us(2); 
		max_IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 max_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	max_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        max_IIC_SCL=0; 
        delay_us(2);
		max_IIC_SCL=1;
        receive<<=1;
        if(max_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        max_IIC_NAck();//����nACK
    else
        max_IIC_Ack(); //����ACK   
    return receive;
}






void max_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    max_IIC_Start();  

	max_IIC_Send_Byte(WriteAddr);	    //����д����
	max_IIC_Wait_Ack();
	
	for(i=0;i<dataLength;i++)
	{
		max_IIC_Send_Byte(data[i]);
		max_IIC_Wait_Ack();
	}				    	   
    max_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

void max_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    max_IIC_Start();  

	max_IIC_Send_Byte(deviceAddr);	    //����д����
	max_IIC_Wait_Ack();
	max_IIC_Send_Byte(writeAddr);
	max_IIC_Wait_Ack();
	max_IIC_Send_Byte(deviceAddr|0X01);//�������ģʽ			   
	max_IIC_Wait_Ack();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = max_IIC_Read_Byte(1);
	}		
	data[dataLength-1] = max_IIC_Read_Byte(0);	
    max_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

void max_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data)
{				  	  	    																 
    max_IIC_Start();  
	
	max_IIC_Send_Byte(daddr);	   //����д����
	max_IIC_Wait_Ack();
	max_IIC_Send_Byte(addr);//���͵�ַ
	max_IIC_Wait_Ack();		 
	max_IIC_Start();  	 	   
	max_IIC_Send_Byte(daddr|0X01);//�������ģʽ			   
	max_IIC_Wait_Ack();	 
    *data = max_IIC_Read_Byte(0);		   
    max_IIC_Stop();//����һ��ֹͣ����	    
}

void max_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{				   	  	    																 
    max_IIC_Start();  
	
	max_IIC_Send_Byte(daddr);	    //����д����
	max_IIC_Wait_Ack();
	max_IIC_Send_Byte(addr);//���͵�ַ
	max_IIC_Wait_Ack();	   	 										  		   
	max_IIC_Send_Byte(data);     //�����ֽ�							   
	max_IIC_Wait_Ack();  		    	   
    max_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}






























