/******************************************************************************

                  版权所有 (C), 2001-2011, 北京加维通讯电子技术有限公司

 ******************************************************************************
  文 件 名   : system_info.c
  版 本 号   : 初稿
  作    者   : zxguan
  生成日期   : 2013年5月21日
  最近修改   :
  功能描述   : system info
  函数列表   :
  修改历史   :
  1.日    期   : 2013年5月21日
    作    者   : zxguan
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
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
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
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
 函 数 名  : GetLocalMac
 功能描述  : 获取本机MAC地址
 输入参数  :char* mac              
 输出参数  : char* mac
 返 回 值  :  	void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年5月21日
    作    者   : dwzhang
    修改内容   : 新生成函数

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

