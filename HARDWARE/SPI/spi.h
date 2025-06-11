#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
typedef struct {
    int x;
    int y;
	  int z;
} TouchPoint;

typedef struct {
		int x;
		int y;
		int z1;
		int z2;
} TouchPointDebug;

#define PEN  		PDin(11)  	//PF10 INT
#define TCS  		PEout(5)  	//PF11  CS 

#define TOUCH_PEN_STATE() PEN==1

void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
void SoftSPI_Init(void);
int TOUCH_GetPoint(TouchPoint* point, TouchPointDebug* pd);
#endif

