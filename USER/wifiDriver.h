#ifndef __WIFI_ESP8266_DIRVER__
#define __WIFI_ESP8266_DIRVER__
#include "usart3.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

/*
��ͷ�ļ�����һϵ����ESP8266 wifiģ���йصĺ�������������wifiģ�鹤��
*/



/*
���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
@param mode 0,������USART3_RX_STA;
						1,����USART3_RX_STA;
*/
void atk_8266_at_response(u8 mode);

/*
ATK-ESP8266���������,�����յ���Ӧ��
@param str:�ڴ���Ӧ����
@return 0,û�еõ��ڴ���Ӧ����
				����,�ڴ�Ӧ������λ��(str��λ��)
*/
u8* atk_8266_check_cmd(u8 *str);



/*
��ATK-ESP8266��������
@param cmd:���͵������ַ���
@param ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
@param waittime:�ȴ�ʱ��(��λ:10ms)
@return 0,���ͳɹ�(�õ����ڴ���Ӧ����)
        1,����ʧ��
*/
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

/*
��ATK-ESP8266����ָ������
@param data:���͵�����(����Ҫ��ӻس���)
@param ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
@param waittime:�ȴ�ʱ��(��λ:10ms)
@return 0,���ͳɹ�(�õ����ڴ���Ӧ����)
*/
u8 atk_8266_send_data(u8 *data, unsigned int dataLen,u8 *ack, u16 waittime);


/*
ATK-ESP8266�˳�͸��ģʽ
@return 0,�˳��ɹ�;
       1,�˳�ʧ��
*/
u8 atk_8266_quit_trans(void);

/*
��ȡATK-ESP8266ģ���AP+STA����״̬
@return 0��δ����;1,���ӳɹ�
*/
u8 atk_8266_apsta_check(void);
/*
��ȡATK-ESP8266ģ�������״̬
@return 0,δ����;1,���ӳɹ�.
*/
u8 atk_8266_consta_check(void);

#endif
