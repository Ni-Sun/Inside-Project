#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "24cxx.h" 
#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//����������(֧��ADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001A/GT9147/FT5206��)����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved						 
//////////////////////////////////////////////////////////////////////////////////

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;
 	 			    					   
//SPIд����
//������ICд��1byte����    
//num:Ҫд�������
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 
		delay_us(1);
		TCLK=1;		//��������Ч	        
	}		 			    
} 		 
//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//��1��ʱ�ӣ����BUSY
	delay_us(1);    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//�½�����Ч  	    	   
		delay_us(1);    
 		TCLK=1;
 		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);   
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x,u16 *y)
{
	TouchPoint point;
  TouchPointDebug debug;
	TOUCH_GetPoint(&point, &debug); 		
	if(point.z == 0) return 0;
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=(u16)point.x;
	*y=(u16)point.y;
	return 1;//�����ɹ�
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//������Ȧ
}	  
//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//�а�������
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		//tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			//tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}	  
//////////////////////////////////////////////////////////////////////////	 
//������EEPROM����ĵ�ַ�����ַ,ռ��14���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define SAVE_ADDR_BASE 40
//����У׼����										    
void TP_Save_Adjdata(void)
{
	AT24CXX_Write(SAVE_ADDR_BASE,(u8*)&tp_dev.xfac,14);	//ǿ�Ʊ���&tp_dev.xfac��ַ��ʼ��14���ֽ����ݣ������浽tp_dev.touchtype
 	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+14,0X0A);		//�����д0X0A���У׼����
}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 TP_Get_Adjdata(void)
{					  
	u8 temp;
	temp=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+14);//��ȡ�����,���Ƿ�У׼���� 		 
	if(temp==0X0A)//�������Ѿ�У׼����			   
 	{ 
		AT24CXX_Read(SAVE_ADDR_BASE,(u8*)&tp_dev.xfac,14);//��ȡ֮ǰ�����У׼���� 
		if(tp_dev.touchtype)//X,Y��������Ļ�෴
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}	 
//��ʾ�ַ���
u8* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 					  
//��ʾУ׼���(��������)
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,160,y0,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,180,x1,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,180,y1,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,200,x2,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,200,y2,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,220,x3,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,220,y3,4,16);	//��ʾ��ֵ
 	LCD_ShowNum(40+56,240,fac,3,16); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}
		 
//������У׼����
//�õ��ĸ�У׼����
/*
1. 在屏幕上显示4个校准点(十字光标),分别位于屏幕的四个角
2. 引导用户依次点击这4个点进行触摸屏校准
3. 记录每个校准点的实际触摸坐标
4. 根据这些坐标计算校准参数(如xfac等)
5. 将校准参数保存到EEPROM中
*/ 

void TP_Adjust(void)
{
	
}

// void TP_Adjust(void)
// {								 
// 	u16 pos_temp[4][2];//���껺��ֵ
// 	u8  cnt=0;	
// 	u16 d1,d2;
// 	u32 tem1,tem2;
// 	double fac; 	
// 	u16 outtime=0;
//  	cnt=0;				
// 	POINT_COLOR=BLUE;
// 	BACK_COLOR =WHITE;
// 	LCD_Clear(WHITE);//����   
// 	POINT_COLOR=RED;//��ɫ 
// 	LCD_Clear(WHITE);//���� 	   
// 	POINT_COLOR=BLACK;
// 	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
// 	TP_Drow_Touch_Point(20,20,RED);//����1 
// 	tp_dev.sta=0;//���������ź� 
// 	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
// 	while(1)//�������10����û�а���,���Զ��˳�
// 	{
// 		tp_dev.scan(1);//ɨ����������
// 		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
// 		{	
// 			outtime=0;		
// 			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ�����������.
						   			   
// 			pos_temp[cnt][0]=tp_dev.x[0];
// 			pos_temp[cnt][1]=tp_dev.y[0];
// 			cnt++;	  
// 			switch(cnt)
// 			{			   
// 				case 1:						 
// 					TP_Drow_Touch_Point(20,20,WHITE);				//�����1 
// 					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//����2
// 					break;
// 				case 2:
//  					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//�����2
// 					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//����3
// 					break;
// 				case 3:
//  					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//�����3
//  					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//����4
// 					break;
// 				case 4:	 //ȫ���ĸ����Ѿ��õ�
// 	    		    //�Ա����
// 					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
// 					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
// 					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
// 					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
// 					fac=(float)d1/d2;
// 					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
// 					{
// 						cnt=0;
//  				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
//    	 					TP_Drow_Touch_Point(20,20,RED);								//����1
//  						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
//  						continue;
// 					}
// 					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
// 					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
// 					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
// 					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
// 					fac=(float)d1/d2;
// 					if(fac<0.95||fac>1.05)//���ϸ�
// 					{
// 						cnt=0;
//  				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
//    	 					TP_Drow_Touch_Point(20,20,RED);								//����1
//  						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
// 						continue;
// 					}//��ȷ��
								   
// 					//�Խ������
// 					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
// 					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
// 					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
// 					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
// 					tem1*=tem1;
// 					tem2*=tem2;
// 					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
// 					fac=(float)d1/d2;
// 					if(fac<0.95||fac>1.05)//���ϸ�
// 					{
// 						cnt=0;
//  				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
//    	 					TP_Drow_Touch_Point(20,20,RED);								//����1
//  						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
// 						continue;
// 					}//��ȷ��
// 					//������
// 					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
// 					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
// 					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
// 					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
// 					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//������Ԥ����෴��.
// 					{
// 						cnt=0;
//  				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
//    	 					TP_Drow_Touch_Point(20,20,RED);								//����1
// 						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
// 						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
// 						if(tp_dev.touchtype)//X,Y��������Ļ�෴
// 						{
// 							CMD_RDX=0X90;
// 							CMD_RDY=0XD0;	 
// 						}else				   //X,Y��������Ļ��ͬ
// 						{
// 							CMD_RDX=0XD0;
// 							CMD_RDY=0X90;	 
// 						}			    
// 						continue;
// 					}		
// 					POINT_COLOR=BLUE;
// 					LCD_Clear(WHITE);//����
// 					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//У�����
// 					delay_ms(1000);
// 					TP_Save_Adjdata();  
//  					LCD_Clear(WHITE);//����   
// 					return;//У�����				 
// 			}
// 		}
// 		delay_ms(10);
// 		outtime++;
// 		if(outtime>1000)
// 		{
// 			TP_Get_Adjdata();
// 			break;
// 	 	} 
//  	}
// }	 



// ????(???HAL??????)
#define SPI_SCK_GPIO     GPIOB
#define SPI_SCK_PIN      GPIO_Pin_13
#define SPI_MOSI_GPIO    GPIOB
#define SPI_MOSI_PIN     GPIO_Pin_15
#define SPI_MISO_GPIO    GPIOB
#define SPI_MISO_PIN     GPIO_Pin_14

// ???(??????????)
#define SPI_DELAY()  __asm__ volatile("nop;nop;nop;nop")

//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(void)
{	
	if(lcddev.id==0X5510)				//4.3����ݴ�����
	{
		if(GT9147_Init()==0)			//��GT9147
		{ 
			tp_dev.scan=GT9147_Scan;	//ɨ�躯��ָ��GT9147������ɨ��
		}else
		{
			OTT2001A_Init();
			tp_dev.scan=OTT2001A_Scan;	//ɨ�躯��ָ��OTT2001A������ɨ��
		}
		tp_dev.touchtype|=0X80;			//������ 
		tp_dev.touchtype|=lcddev.dir&0X01;//������������ 
		return 0;
	}else if(lcddev.id==0X1963)			//7����ݴ�����
	{
		FT5206_Init();
		tp_dev.scan=FT5206_Scan;		//ɨ�躯��ָ��GT9147������ɨ��		
		tp_dev.touchtype|=0X80;			//������ 
		tp_dev.touchtype|=lcddev.dir&0X01;//������������ 
		return 0;
	}else
	{
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

		//ע��,ʱ��ʹ��֮��,��GPIO�Ĳ�������Ч
		//��������֮ǰ,����ʹ��ʱ��.����ʵ���������������
   	 	/*
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PB,PF�˿�ʱ��
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 // PB1�˿�����
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOB, &GPIO_InitStructure);//B1�������
	 	GPIO_SetBits(GPIOB,GPIO_Pin_1);//����
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 // PB2�˿�����
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	 	GPIO_Init(GPIOB, &GPIO_InitStructure);//B2��������
	 	GPIO_SetBits(GPIOB,GPIO_Pin_2);//����		
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_9;				 // F9��PF11�˿�����
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF9,PF11�������
	 	GPIO_SetBits(GPIOF, GPIO_Pin_11|GPIO_Pin_9);//����
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PF10�˿�����
	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	 	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10��������
	 	GPIO_SetBits(GPIOF,GPIO_Pin_10);//����		
 */
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��	 
		AT24CXX_Init();			//��ʼ��24CXX
		if(TP_Get_Adjdata())return 0;//�Ѿ�У׼
		else			  		//δУ׼?
		{ 										    
			LCD_Clear(WHITE);	//����
			TP_Adjust();  		//��ĻУ׼  
		}			
		TP_Get_Adjdata();	
	}
	return 1; 									 
}

