#include "wifiSta.h"
#include <string.h>

//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
const u8 *portnum = "1600";

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
const u8 *wifista_ssid = "danbu";            //·����SSID��
const u8 *wifista_password = "danbu1996";    //��������

const u8 *wifista_server_ip = "39.108.176.84"; // UDP ������

/*
���Wifiģ���Ƿ�����
*/
void checkModuleOn(void) {
    //���WIFIģ���Ƿ�����
    printf("Check wifi module...\r\n");
//    delay_ms(1000);
//    delay_ms(1000);
//    delay_ms(1000);
    while (atk_8266_send_cmd("AT", "OK", 20)) {
        atk_8266_quit_trans();//�˳�͸��
        atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200);  //�ر�͸��ģʽ
        printf("δ��⵽Wifiģ��!!!\r\n");

        printf("��������Wifiģ��...\r\n");

//        BEEP = 1;
//        delay_ms(500);
//        BEEP = 0;
//        delay_ms(500);
//        BEEP = 1;
//        delay_ms(1000);
//        BEEP = 0;
    }
    while (atk_8266_send_cmd("ATE0", "OK", 20));//�رջ���
    printf("Wifi module OK\r\n");
}

/*
����STA UDP ģʽ��Ȼ������ģ��
*/
void settingUDPMod(void) {
    // UDP ����
    atk_8266_send_cmd("AT+CWMODE=1", "OK", 50);        //����WIFI STAģʽ
    atk_8266_send_cmd("AT+RST", "OK", 20);        //DHCP�������ر�(��APģʽ��Ч)
    printf("Setting STA UDP mod...\r\n");
    //��ʱ3S�ȴ������ɹ�
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
}

/*
����·����
@param ssid ·��������
@param password ����
*/
void connectAP(const u8 *ssid, const u8 *password) {
    u8 *p;
    //p = mymalloc(SRAMIN, 32);                            //����32�ֽ��ڴ�
    printf("Connect to AP %s\r\n", ssid);

    //�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!!
    sprintf((char *) p, "AT+CWJAP=\"%s\",\"%s\"", ssid, password);//�������߲���:ssid,����
    while (atk_8266_send_cmd(p, "WIFI GOT IP", 300));                    //����Ŀ��·����,���һ��IP


    delay_ms(1000); // ��ʱʹģ����Ч
    delay_ms(1000);
    delay_ms(1000);
    printf("AP connected!\r\n");

    //myfree(SRAMIN, p); // �ͷ��ڴ�
}

/*
��ȡ��AP�õ���IP��ַ
@param ipbuf ������������ڻ���IP��ַ�ַ���
@return 0 ��ȡ�ɹ�
				1 ��ȡʧ��
*/
u8 getCurrentIP(u8 *ipbuf) {
    u8 *p, *p1;
    p = mymalloc(SRAMIN, 32);                            //����32�ֽ��ڴ�
    // ��ȡIP
    if (atk_8266_send_cmd("AT+CIFSR", "OK", 50)) { //��ȡWAN IP��ַʧ��
        printf("Get IP address from AP failed!\r\n");
        return 1;
    }

    p = atk_8266_check_cmd("\"");
    p1 = (u8 *) strstr((const char *) (p + 1), "\"");
    *p1 = 0;
    sprintf((char *) ipbuf, "%s", p + 1);
    myfree(SRAMIN, p); // �ͷ��ڴ�
    return (u8) 0;
}


/*
����UDP������
@param ip UDP��������IP
@param prot UDP�������˿ں�
*/
void configUDPServer(const u8 *ip, const u8 *port) {
    u8 *p;
    p = mymalloc(SRAMIN, 32);                            //����32�ֽ��ڴ�

    printf("Configuration UDP server...\t\n");
    atk_8266_send_cmd("AT+CIPMUX=0", "OK", 20);  //������ģʽ
    delay_ms(200);
    sprintf((char *) p, "AT+CIPSTART=\"UDP\",\"%s\",%s", ip, port);    //����Ŀ��UDP������
    printf("%s\r\n", p);
    while (atk_8266_send_cmd(p, "OK", 500));
    myfree(SRAMIN, p); // �ͷ��ڴ�
}

/*
����UDP��Ϣ
@param msg ���͵���Ϣ����
@param msgLen ������Ϣ�ֽ���
*/
void sendUDPMsg(u8 *msg, unsigned int msgLen) {
//    unsigned int i = 0;
    u8 *p;
    p = mymalloc(SRAMIN, 32);                            //����32�ֽ��ڴ�

//    printf("Send msg length:%d, content:\r\n", msgLen);
//    for (i = 0; i < msgLen; ++i) {
//        if (i % 4 == 0) {
//            printf("\r\n");
//        }
//        printf("%02x  ", *(msg + i));
//    }
//    printf("\r\n");


    sprintf((char *) p, "AT+CIPSEND=%d", msgLen);
    atk_8266_send_cmd(p, "OK", 200);  //����ָ�����ȵ�����
    delay_ms(200);
    atk_8266_send_data(msg, msgLen, "OK", 200);  //����ָ�����ȵ�����
    myfree(SRAMIN, p); // �ͷ��ڴ�

}


/*
���UDP����������״̬
@return 0 ��������
				1 �����쳣
*/
u8 checkUDPServerStat(void) {
    u8 res;
    //�õ�����״̬
    if (atk_8266_consta_check() == '+') res = 0;
    else res = 1;
    USART3_RX_STA = 0;
    return res;
}


/**
 * ��ȡ�Ѿ����յ�����Ϣ
 * @param dataLen ��Ч���ݳ���
 */
unsigned char *getReceivedMsg(unsigned short *dataLen) {
    unsigned short dataStartIndex = 0;
    unsigned char *msgBuffer = NULL;
//    unsigned short i = 0;
    if (USART3_RX_STA & 0X8000) {    //���յ�һ��������
        *dataLen = USART3_RX_STA & 0X7FFF;    //�õ����ν��յ������ݳ���
//        printf("data len:%d\r\n", *dataLen);
//
//        for (i = 0; i < *dataLen; i++) {
//            if(i % 4 == 0) printf("\r\n");
//            printf("%02x  ", USART3_RX_BUF[i]);
//        }
//        printf("\r\n");

        // Ѱ�ҷָ���Ч���ݵĿ�ʼ����
        for (dataStartIndex = 0; dataStartIndex < *dataLen; ++dataStartIndex) {
            if (USART3_RX_BUF[dataStartIndex] == ':') {
                dataStartIndex++;
//                printf("find data index:%d\r\n", dataStartIndex);
                *dataLen = *dataLen - dataStartIndex;
                break;
            }
        }
        if (dataStartIndex == (USART3_RX_STA & 0X7FFF)) dataStartIndex = 0;
        msgBuffer = (unsigned char *) mymalloc(SRAMIN, *dataLen);
        memcpy(msgBuffer, USART3_RX_BUF + dataStartIndex, *dataLen);
    } else {
        msgBuffer = NULL;
        dataLen = 0;
    }
    USART3_RX_STA = 0; // �������3����
    return msgBuffer;
}

void testSTA(void) {
    short t = 999;
    unsigned char *msg;
    unsigned short len, i;                          //����32�ֽ��ڴ�

    // -------------------- ����ģʽ ---------------------
    //���WIFIģ���Ƿ�����
    checkModuleOn();

    // UDP ����
    settingUDPMod();

    // ����·����
    connectAP(wifista_ssid, wifista_password);

    // ����UDO������
    configUDPServer(wifista_server_ip, portnum);
    // ------------------- ����ģʽ���� -------------------

    USART3_RX_STA = 0; // �������3����
    while (1) {

        if (t == 500) {
            sendUDPMsg("Test message from STM32F1", 25);
        }
        t++;
        if (t == 1000) {//����10����û���յ��κ�����,��������ǲ��ǻ�����.
            if (checkUDPServerStat() == 0)printf("����״̬OK\r\n");  //����״̬
            else printf("����ʧ��\r\n");
            t = 0;
        }
        delay_ms(10);

        // ��ȡ�յ�����Ϣ
        msg = getReceivedMsg(&len);
        if (msg != NULL && len > 0) {
            printf("dataLen: %d\r\n", len);
            for (i = 0; i < len; ++i) {
                printf("%02x ", msg[i]);
            }
            printf("\r\n");
            myfree(SRAMIN, msg);
            msg = NULL;
        }
    }
}
