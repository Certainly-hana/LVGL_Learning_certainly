#include "maxiic.h"
#include "delay.h"
//GPIO��ʼ���Ľṹ��
static GPIO_InitTypeDef  GPIO_InitStructure;

void max_i2c_sda_mode(GPIOMode_TypeDef pin_mode)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//����Ҫ����������	
	
	if(pin_mode == GPIO_Mode_OUT)
	{
		//����PG9����Ϊ���ģʽ
		
		GPIO_InitStructure.GPIO_Mode = pin_mode;		//���ģʽ
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//���������������������������������������������չ����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�����ٶ�100MHz�������ٶ�Խ�ߣ����ľͻ�Խ��
		
	
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	//����ģʽ
	}

	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

//��ʼ��IIC
void MAX_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���������������������������������������������չ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	
	MAX_IIC_SCL=1;
	MAX_IIC_SDA=1;

}
//����IIC��ʼ�ź�
void MAX_IIC_Start(void)
{
	MAX_SDA_OUT();     //sda�����
	MAX_IIC_SDA=1;	  	  
	MAX_IIC_SCL=1;
	delay_us(4);
 	MAX_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	MAX_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void MAX_IIC_Stop(void)
{
	MAX_SDA_OUT();//sda�����
	MAX_IIC_SCL=0;
	MAX_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	MAX_IIC_SCL=1; 
	MAX_IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 MAX_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MAX_SDA_IN();      //SDA����Ϊ����  
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
	MAX_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void MAX_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MAX_SDA_OUT(); 	    
    MAX_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        MAX_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		MAX_IIC_SCL=1;
		delay_us(2); 
		MAX_IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 MAX_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MAX_SDA_IN();//SDA����Ϊ����
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
        MAX_IIC_NAck();//����nACK
    else
        MAX_IIC_Ack(); //����ACK   
    return receive;
}


void MAX_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX_IIC_Start();  

	MAX_IIC_Send_Byte(WriteAddr);	    //����д����
	MAX_IIC_Wait_Ack();
	
	for(i=0;i<dataLength;i++)
	{
		MAX_IIC_Send_Byte(data[i]);
		MAX_IIC_Wait_Ack();
	}				    	   
    MAX_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

void MAX_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX_IIC_Start();  

	MAX_IIC_Send_Byte(deviceAddr);	    //����д����
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(writeAddr);
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(deviceAddr|0X01);//�������ģʽ			   
	MAX_IIC_Wait_Ack();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = MAX_IIC_Read_Byte(1);
	}		
	data[dataLength-1] = MAX_IIC_Read_Byte(0);	
    MAX_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

void MAX_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data)
{				  	  	    																 
    MAX_IIC_Start();  
	
	MAX_IIC_Send_Byte(daddr);	   //����д����
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(addr);//���͵�ַ
	MAX_IIC_Wait_Ack();		 
	MAX_IIC_Start();  	 	   
	MAX_IIC_Send_Byte(daddr|0X01);//�������ģʽ			   
	MAX_IIC_Wait_Ack();	 
    *data = MAX_IIC_Read_Byte(0);		   
    MAX_IIC_Stop();//����һ��ֹͣ����	    
}

void MAX_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{				   	  	    																 
    MAX_IIC_Start();  
	
	MAX_IIC_Send_Byte(daddr);	    //����д����
	MAX_IIC_Wait_Ack();
	MAX_IIC_Send_Byte(addr);//���͵�ַ
	MAX_IIC_Wait_Ack();	   	 										  		   
	MAX_IIC_Send_Byte(data);     //�����ֽ�							   
	MAX_IIC_Wait_Ack();  		    	   
    MAX_IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}



























