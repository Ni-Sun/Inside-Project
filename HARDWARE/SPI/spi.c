#include "spi.h"
#include "usart.h"
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
 

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C							  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��


void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);  //PB13/14/15����

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
	SPI2_ReadWriteByte(0xff);//��������	 

}   
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M)
  
void SPI2_SetSpeed(u8 SpeedSet)
{
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	SPI_Cmd(SPI2,ENABLE); 
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
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


