#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
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

void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void SoftSPI_Init(void);
int TOUCH_GetPoint(TouchPoint* point, TouchPointDebug* pd);
#endif

