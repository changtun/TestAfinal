/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ������άͨѶ���Ӽ������޹�˾

 ******************************************************************************
  �� �� ��   : system_info.c
  �� �� ��   : ����
  ��    ��   : zxguan
  ��������   : 2013��5��21��
  ����޸�   :
  ��������   : system info
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��5��21��
    ��    ��   : zxguan
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <linux/sockios.h>
#include <net/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h> 
#include <net/if_arp.h> 

#include "pbitrace.h"
#include "system_info.h"
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define MAX_SOFT_WARE_ID 100

typedef struct
{
    char cSoftWareID[MAX_SOFT_WARE_ID];
}SYSTEM_INFO_M;

SYSTEM_INFO_M m_SystemInfo;

void setSoftWareID(char *pSoftWareID)
{
    int len = 0;
    if( NULL != pSoftWareID )
    {
        len = MAX_SOFT_WARE_ID > strlen(pSoftWareID) ? strlen(pSoftWareID): MAX_SOFT_WARE_ID;

        memset( m_SystemInfo.cSoftWareID, 0, MAX_SOFT_WARE_ID );
        memcpy( m_SystemInfo.cSoftWareID, pSoftWareID, len);
        pbiinfo("[%s %d] SoftWareID:%s.\n", DEBUG_LOG, m_SystemInfo.cSoftWareID);
    }
}

int getSoftWareID(char *pSoftWareId)
{
    if( NULL == pSoftWareId )
    {
        return -1;
    }

    memcpy( pSoftWareId, m_SystemInfo.cSoftWareID, MAX_SOFT_WARE_ID );

    return 0;
}

/*****************************************************************************
 �� �� ��  : GetLocalMac
 ��������  : ��ȡ����MAC��ַ
 �������  :char* mac              
 �������  : char* mac
 �� �� ֵ  :  	void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��5��21��
    ��    ��   : dwzhang
    �޸�����   : �����ɺ���

*****************************************************************************/
void GetLocalMac(char* mac)
{
	int sock_mac;
	struct ifreq ifr_mac;
	

	sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
	if( sock_mac == -1)
	{
		pbiinfo("create socket falise...mac\n");
		return ;
	}
	memset(&ifr_mac,0,sizeof(ifr_mac));
	strncpy(ifr_mac.ifr_name, "eth0", sizeof(ifr_mac.ifr_name)-1);
	if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
	{
		pbiinfo("mac ioctl error\n");
		return ;
	}

	//int a[6];
	//sprintf( a[0],"%x",ifr_mac.ifr_hwaddr.sa_data[0]);
	memset(mac,0,12);
	sprintf(mac,"%02x%02x%02x%02x%02x%02x",
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);
	pbiinfo("local mac:%s \n",mac);
	close( sock_mac );
	//return mac_addr;
}

