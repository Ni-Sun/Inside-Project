#ifndef __WIFI_STA__
#define __WIFI_STA__


#include "wifiDriver.h"
#include "malloc.h"	  
//#include "beep.h"
#include "usart.h"	 


/*
���Wifiģ���Ƿ�����
*/
void checkModuleOn(void);

/*
����STA UDP ģʽ��Ȼ������ģ��
*/
void settingUDPMod(void);
/*
����·����
@param ssid ·��������
@param password ����
*/
void connectAP(const u8 * ssid,const u8 * password);

/*
��ȡ��AP�õ���IP��ַ
@param ipbuf ������������ڻ���IP��ַ�ַ���
@return 0 ��ȡ�ɹ�
				1 ��ȡʧ��
*/
u8 getCurrentIP(u8* ipbuf);

/*
����UDP������
@param ip UDP��������IP
@param prot UDP�������˿ں�
*/
void configUDPServer(const u8 * ip, const u8 * port);

/*
����UDP��Ϣ
@param msg ���͵���Ϣ����
@param msgLen ������Ϣ�ֽ���
*/
void sendUDPMsg(u8 * msg,unsigned int msgLen);


/*
���UDP����������״̬
@return 0 ��������
				1 �����쳣
*/
u8 checkUDPServerStat(void);

/**
 * ��ȡ�Ѿ����յ�����Ϣ
 * @param dataLen ��Ч���ݳ���
 */
unsigned char *getReceivedMsg(unsigned short *dataLen);


/*
STA UDP���Ӳ���
*/
void testSTA(void);

#endif
