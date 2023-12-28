/*============================================================================
* 文件描述： 该文件是ANO_Tech的源文件，主要实现用户上报MPU信息API                       *
*                                                                            *
* 基本功能： (需要串口初始化）上报MPU信息到匿名四轴上位机V2.6版本（其他版本不可用）                                                *
* 作者    ： uzi                                                           *
* 时间    ： 2020.12.02                                                      *
============================================================================*/
#include "ANO_Tech.h"

void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{

	while(len--)
	{
		//发送数据
		USART_SendData(USART1,*pbuf++);
		
		//检查数据是否发送完毕
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	
	}
}

/*---------------------------------------------------------------------------
* 函数：void Uart0NimingReport(u8 fun,u8*data,u8 len)
* 描述：传送数据给匿名四轴上位机软件(V2.6版本)
* 参数：
		fun:  功能字. 0XA0~0XAF
		data: 数据缓存区,最多28字节!!
		len:  data区有效数据个数
* 返回：无
*---------------------------------------------------------------------------*/
void Uart0NimingReport(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_bytes(&send_buf[i],1);//发送数据到串口1 
}


/*---------------------------------------------------------------------------
* 函数：void MPU6050SendData(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
* 描述：发送加速度传感器数据和陀螺仪数据
* 参数：
		aacx,aacy,aacz:x,y,z三个方向上面的加速度值
		gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
* 返回：无
*---------------------------------------------------------------------------*/
void MPU6050SendData(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	Uart0NimingReport(0XA1,tbuf,12);//自定义帧,0XA1
}	

/*---------------------------------------------------------------------------
* 函数：Uart0ReportImu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
* 描述：通过串口0上报结算后的姿态数据给电脑
* 参数：
		aacx,aacy,aacz:x,y,z三个方向上面的加速度值
		gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
		roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
		pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
		yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
* 返回：无
*---------------------------------------------------------------------------*/
void Uart0ReportImu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	Uart0NimingReport(0XAF,tbuf,28);//飞控显示帧,0XAF
} 
