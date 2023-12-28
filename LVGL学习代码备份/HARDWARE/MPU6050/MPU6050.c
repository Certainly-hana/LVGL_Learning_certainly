#include "mpu6050.h"

/*---------------------------------------------------------------------------
* 函数：I2C_MPU6050_Config(void)
* 描述：MPU6050引脚初始化
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void I2C_MPU6050_Config(void)
{			
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	IIC_SCL=1;
	IIC_SDA=1;
	
}

/*---------------------------------------------------------------------------
* 函数：IIC_Start(void)
* 描述：IIC主机发出开始条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void IIC_Start(void)
{
	SDA_OUT;     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  

/*---------------------------------------------------------------------------
* 函数：void IIC_Stop(void)
* 描述：IIC主机发出停止条件
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void IIC_Stop(void)
{
	SDA_OUT;//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}


/*---------------------------------------------------------------------------
* 函数：IIC_Wait_Ack(void)
* 描述：IIC主机等待从机应答
* 参数：无
* 返回：1：从机无应答；   0：从机应答
*---------------------------------------------------------------------------*/
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN;      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		//delay_us(1);	
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

/*---------------------------------------------------------------------------
* 函数：IIC_Ack(void)  
* 描述：IIC主机发出应答
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/
static void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT;
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}


/*---------------------------------------------------------------------------
* 函数：IIC_NAck(void)
* 描述：IIC主机发出非应答
* 参数：无
* 返回：无
*---------------------------------------------------------------------------*/	    
static void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT;
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     


/*---------------------------------------------------------------------------
* 函数：IIC_Send_Byte(u8 txd)
* 描述：IIC主机往从机写一个字节
* 参数：要写入的字节
* 返回：无
*---------------------------------------------------------------------------*/
static void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT; 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    


/*---------------------------------------------------------------------------
* 函数：IIC_Read_Byte(unsigned char ack)
* 描述：IIC主机从从机读一个字节
* 参数：ack=1时，发送ACK，ack=0，发送nACK 
* 返回：receive:接收到的数据
*---------------------------------------------------------------------------*/   
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN;//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



/*---------------------------------------------------------------------------
* 函数：MPU_Init(void)
* 描述：MPU6050初始化
* 参数：无
* 返回：0 成功 | 1 失败
*---------------------------------------------------------------------------*/
u8 MPU6050_Init(void)
{ 
	u8 res;
	I2C_MPU6050_Config();//初始化IIC总线
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
    delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU_Set_Rate(50);						//设置采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
//	printf("RES=%X\r\n",res);
	if(res==MPU_ADDR)//器件ID正确
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						//设置采样率为50Hz
 	}else return 1;
	return 0;
}


/*---------------------------------------------------------------------------
* 函数：MPU_Set_Gyro_Fsr(u8 fsr)
* 描述：设置MPU6050陀螺仪传感器满量程范围
* 参数：fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
* 返回：0 成功 | 非0 失败
*---------------------------------------------------------------------------*/ 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}


/*---------------------------------------------------------------------------
* 函数：MPU_Set_Gyro_Fsr(u8 fsr)
* 描述：设置MPU6050加速度传感器满量程范围
* 参数：fsr:0,±2g;1,±4g;2,±8g;3,±16g
* 返回：0 成功 | 非0 失败
*---------------------------------------------------------------------------*/ 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}


/*---------------------------------------------------------------------------
* 函数：MPU_Set_LPF(u16 lpf)
* 描述：lpf:数字低通滤波频率(Hz)
* 参数：fsr:0,±2g;1,±4g;2,±8g;3,±16g
* 返回：0 成功 | 非0 失败
*---------------------------------------------------------------------------*/
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//设置数字低通滤波器  
}


/*---------------------------------------------------------------------------
* 函数：MPU_Set_Rate(u16 rate)
* 描述：设置MPU6050的采样率(假定Fs=1KHz)
* 参数：rate:4~1000(Hz)
* 返回：0 成功 | 非0 失败
*---------------------------------------------------------------------------*/
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}


/*---------------------------------------------------------------------------
* 函数：MPU_Get_Temperature(void)
* 描述：得到温度值
* 参数：无
* 返回：温度值(扩大了100倍)
*---------------------------------------------------------------------------*/
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}

/*---------------------------------------------------------------------------
* 函数：MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
* 描述：得到陀螺仪值(原始值)
* 参数：gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
* 返回：0 成功 | 1 失败
*---------------------------------------------------------------------------*/
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}


/*---------------------------------------------------------------------------
* 函数：MPU_Get_Accelerometer(short *ax,short *ay,short *az)
* 描述：得到加速度值(原始值)
* 参数：gx,gy,gz:加速器x,y,z轴的原始读数(带符号)
* 返回：0 成功 | 1 失败
*---------------------------------------------------------------------------*/
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}


/*---------------------------------------------------------------------------
* 函数：MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
* 描述：从寄存器中写入多个数据
* 参数：addr:器件地址  reg：要读的寄存器；   buf：写入数据流；  len：写入的数据长度
* 返回：0：写入成功； 1：写入失败
*---------------------------------------------------------------------------*/
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i; 
    IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答
	for(i=0;i<len;i++)
	{
		IIC_Send_Byte(buf[i]);	//发送数据
		if(IIC_Wait_Ack())		//等待ACK
		{
			IIC_Stop();	 
			return 1;		 
		}		
	}    
    IIC_Stop();	 
	return 0;	
} 

/*---------------------------------------------------------------------------
* 函数：MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
* 描述：从寄存器中读出多个数据
* 参数：addr:器件地址  reg：要读的寄存器；   buf：读出数据流；  len：读取的数据长度
* 返回：0：读出成功； 1：读出失败
*---------------------------------------------------------------------------*/
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
 	IIC_Start(); 
	IIC_Send_Byte((addr<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答
    IIC_Start();
	IIC_Send_Byte((addr<<1)|1);//发送器件地址+读命令	
    IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
    IIC_Stop();	//产生一个停止条件 
	return 0;	
}

/*---------------------------------------------------------------------------
* 函数：MPU_Write_Byte(u8 reg,u8 data)     
* 描述：把单个数据写入到相应的寄存器中
* 参数：reg:寄存器地址 data:数据
* 返回：0：写入成功； 1：写入失败
*---------------------------------------------------------------------------*/
u8 MPU_Write_Byte(u8 reg,u8 data) 				 
{ 
    IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	if(IIC_Wait_Ack())	//等待应答
	{
		IIC_Stop();		 
		return 1;		
	}
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答 
	IIC_Send_Byte(data);//发送数据
	if(IIC_Wait_Ack())	//等待ACK
	{
		IIC_Stop();	 
		return 1;		 
	}		 
    IIC_Stop();	 
	return 0;
}

	

/*---------------------------------------------------------------------------
* 函数：MPU_Read_Byte(u8 reg)     
* 描述：把单个数据读出到相应的寄存器中
* 参数：reg:寄存器地址 
* 返回：读到的数据
*---------------------------------------------------------------------------*/
u8 MPU_Read_Byte(u8 reg)
{
	u8 res;
    IIC_Start(); 
	IIC_Send_Byte((MPU_ADDR<<1)|0);//发送器件地址+写命令	
	IIC_Wait_Ack();		//等待应答 
    IIC_Send_Byte(reg);	//写寄存器地址
    IIC_Wait_Ack();		//等待应答
    IIC_Start();
	IIC_Send_Byte((MPU_ADDR<<1)|1);//发送器件地址+读命令	
    IIC_Wait_Ack();		//等待应答 
	res=IIC_Read_Byte(0);//读取数据,发送nACK 
    IIC_Stop();			//产生一个停止条件 
	return res;		
}
