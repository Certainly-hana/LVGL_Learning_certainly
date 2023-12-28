/*============================================================================
* 文件描述： 该文件是ANO_Tech的头文件，主要实现用户上报MPU信息API                       *
*                                                                            *
* 基本功能： (需要串口初始化）上报MPU信息到匿名四轴上位机V2.6版本（其他版本不可用）                                                *
* 作者    ： uzi                                                           *
* 时间    ： 2020.12.02                                                      *
============================================================================*/
#ifndef __ANO_TECH_H__
#define __ANO_TECH_H__

#include "sys.h"

void usart1_send_bytes(uint8_t *pbuf,uint32_t len);

void Uart0NimingReport(u8 fun,u8*data,u8 len);

void MPU6050SendData(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);

void Uart0ReportImu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);





#endif
