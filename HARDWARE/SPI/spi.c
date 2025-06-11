#include "spi.h"
#include "usart.h"
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
 

//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI2的初始化


void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15上拉

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
	
	SPI2_ReadWriteByte(0xff);//启动传输	 

}   
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)
  
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	SPI_Cmd(SPI2,ENABLE); 
} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据					    
}





#include "stm32f10x_gpio.h"

// ????(???HAL??????)
#define SPI_SCK_GPIO     GPIOG
#define SPI_SCK_PIN      GPIO_Pin_13
#define SPI_MOSI_GPIO    GPIOG
#define SPI_MOSI_PIN     GPIO_Pin_15
#define SPI_MISO_GPIO    GPIOG
#define SPI_MISO_PIN     GPIO_Pin_14

// ???(??????????)
#define SPI_DELAY()  __asm__ volatile("nop;nop;nop;nop")

void SoftSPI_Init(void)
{
			GPIO_InitTypeDef GPIO_InitStruct;
    
    // 1. ????(APB2??)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

    // 2. ??SCK?MOSI?????
    GPIO_InitStruct.GPIO_Pin = SPI_SCK_PIN | SPI_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_SCK_GPIO, &GPIO_InitStruct);

    // 3. ??MISO?????
    GPIO_InitStruct.GPIO_Pin = SPI_MISO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPI_MISO_GPIO, &GPIO_InitStruct);

    // 4. ??????
    GPIO_WriteBit(SPI_SCK_GPIO, SPI_SCK_PIN, Bit_RESET); // SCK???
    GPIO_WriteBit(SPI_MOSI_GPIO, SPI_MOSI_PIN, Bit_RESET); // MOSI???
	/*
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 1. ????(APB2??)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. ??SCK?MOSI?????
    GPIO_InitStruct.GPIO_Pin = SPI_SCK_PIN | SPI_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_SCK_GPIO, &GPIO_InitStruct);

    // 3. ??MISO?????
    GPIO_InitStruct.GPIO_Pin = SPI_MISO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPI_MISO_GPIO, &GPIO_InitStruct);

    // 4. ??????
    GPIO_WriteBit(SPI_SCK_GPIO, SPI_SCK_PIN, Bit_RESET); // SCK???
    GPIO_WriteBit(SPI_MOSI_GPIO, SPI_MOSI_PIN, Bit_RESET); // MOSI???
		*/
}

// ??SPI?????(??0:CPOL=0, CPHA=0)
u8 SoftSPI_ReadWriteByte(u8 txData)
{
    u8 rxData = 0;
    int i;
    for(i=0; i<8; i++)
    {
        // ???????(??0)
        GPIO_WriteBit(SPI_MOSI_GPIO, SPI_MOSI_PIN, (txData & 0x80) ? Bit_SET : Bit_RESET);
        txData <<= 1;
        
        SPI_DELAY();
        
        // ???????
        GPIO_WriteBit(SPI_SCK_GPIO, SPI_SCK_PIN, Bit_SET);
        rxData <<= 1;
        if(GPIO_ReadInputDataBit(SPI_MISO_GPIO, SPI_MISO_PIN))
            rxData |= 0x01;
            
        GPIO_WriteBit(SPI_SCK_GPIO, SPI_SCK_PIN, Bit_RESET);
        SPI_DELAY();
    }
    return rxData;
}

uint8_t TOUCH_SPI_TransferByte(uint8_t data)
{
    return SoftSPI_ReadWriteByte(data);
}

int TOUCH_ReadCoordinate(uint8_t command)
{
    int data = 0;
	  int high;
	  int low;
    TCS = 0;
    TOUCH_SPI_TransferByte(command);
		high = (int)TOUCH_SPI_TransferByte(0x00);
    low  = (int)TOUCH_SPI_TransferByte(0x00);
    TCS = 1;

    data = ((high << 8) | low) >> 3;  // 12???,??12???,?3???
    return data;
}


int mapAAA(int data, int min, int max, int realMin, int realMax) {
	if (max == min) return -1;  // ????0
	if(data < min) return realMin;
	if(data > max) return realMax;
	return (int)((float)(data - min) * (realMax - realMin) / (max - min) + realMin);
}

int TOUCH_GetPoint(TouchPoint* point, TouchPointDebug* pd)
{
		int x = 0, y = 0, z1 = 0, z2 = 0;
	  int rxplate = 300;
	  int i;
    if (TOUCH_PEN_STATE())  // PEN ???
		{
				point->x = -1;
        point->y = -1;
        point->z = 0;
				pd->x = -1;
				pd->y = -1;
				pd->z1 = -1;
				pd->z2 = -1;
        return 0;  // ?????
		}

    #define READ_TIMES 5
    for (i = 0; i < READ_TIMES; i++) {
        x += TOUCH_ReadCoordinate(0xD0);  // ?? X
        y += TOUCH_ReadCoordinate(0x90);  // ?? Y
        z1 += TOUCH_ReadCoordinate(0xB0); // ?? Z1
        z2 += TOUCH_ReadCoordinate(0xC0); // ?? Z2
        //OS_ERR err;
        //OSTimeDly(1, OS_OPT_TIME_DLY, &err);
    }
    x /= READ_TIMES;
    y /= READ_TIMES;
    z1 /= READ_TIMES;
    z2 /= READ_TIMES;

    point->x = 240-mapAAA(x, 200, 3800, 0, 240);
    point->y = mapAAA(y, 200, 3700, 0, 320);
		pd->x = x;
		pd->y = y;
		pd->z1 = z1;
		pd->z2 = z2;
		
    if (z1 == 0) {
        point->z = 0; // ???0??
				return 0;
    } else {
        // ?????,???????????
        // rxplate ?????????,?????300O(????????)  
				if(rxplate != 0) {
						float rtouch = (float)z2 / z1 - 1.0f;
						rtouch = rtouch * x * rxplate / 4096.0f;  // 12bit ADC,????4096

						//if (rtouch < 0) rtouch = 0;
						//if (rtouch > 1023) rtouch = 1023;

						point->z = (uint16_t)rtouch;
				} else {
						point->z = (1023 - (z2 - z1));
				}
    }
		
		// ?????
		/*
		if(point->z > 600) {
			point->x = -1;
			point->y = -1;
			point->z = 0;
			pd->x = -1;
			pd->y = -1;
			pd->z1 = -1;
			pd->z2 = -1;
			
			return 0;
		}
		*/
		
		//printf("Point(%d, %d, %d)\n", point->x, point->y, point->z);

    return 1;  // ????
}


