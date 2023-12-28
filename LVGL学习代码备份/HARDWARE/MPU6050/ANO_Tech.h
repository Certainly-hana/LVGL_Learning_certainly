/*============================================================================
* �ļ������� ���ļ���ANO_Tech��ͷ�ļ�����Ҫʵ���û��ϱ�MPU��ϢAPI                       *
*                                                                            *
* �������ܣ� (��Ҫ���ڳ�ʼ�����ϱ�MPU��Ϣ������������λ��V2.6�汾�������汾�����ã�                                                *
* ����    �� uzi                                                           *
* ʱ��    �� 2020.12.02                                                      *
============================================================================*/
#ifndef __ANO_TECH_H__
#define __ANO_TECH_H__

#include "sys.h"

void usart1_send_bytes(uint8_t *pbuf,uint32_t len);

void Uart0NimingReport(u8 fun,u8*data,u8 len);

void MPU6050SendData(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);

void Uart0ReportImu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);





#endif
