/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : client.c
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/6/18
  Last Modified :
  Description   : socket client, service in java
  Function List :
  History       :
  1.Date        : 2012/6/18
    Author      : zxguan
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pvddefs.h"
#include "pbitrace.h"
#include "errno.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
S32 g_Socket_fd = -1;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define SERV_PORT 9527
#define MAXLINE 100
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

/*****************************************************************************
 Prototype    : _Get_Socket_Handle
 Description  : 获取 socket 句柄
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
static S32 _Get_Socket_Handle()
{
    return g_Socket_fd;
}

/*****************************************************************************
 Prototype    : _Set_Socket_Handle
 Description  : 设置 socket 句柄
 Input        : S32 value  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
static void _Set_Socket_Handle(S32 value)
{
    g_Socket_fd = value;
}

/*****************************************************************************
 Prototype    : Msg_Socket_Client_Init
 Description  : 初始化 socket client 与服务端建立连接
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 Msg_Socket_Client_Init()
{
    S32 sockfd = -1;
    S32 ret = 0;
    struct sockaddr_in servaddr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( 0 > sockfd )
    {
        pbierror("[Msg socket]socket error!..%d..%s.\n",__LINE__,__FUNCTION__);
        return -1;
    }

    bzero( &servaddr, sizeof(servaddr) );

    servaddr.sin_family = AF_INET;
    ret = inet_pton( AF_INET, ".", &servaddr.sin_addr );
    if( -1 == ret )
    {
        pbierror("[Msg socket]inet pron error!..%d..%s.\n",__LINE__,__FUNCTION__);
        return -1;
    }

    //servaddr.sin_addr.s_addr = inet_addr("10.10.90.87");
    servaddr.sin_port = htons(SERV_PORT);

    ret = connect( sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr) );
    if( 0 > ret )
    {
        pbierror("[Msg socket]connect error!..%d..%s.\n",__LINE__,__FUNCTION__);
        pbierror("ret :%d..%s..\n\n",ret,strerror(errno));
        return -1;
    }

    _Set_Socket_Handle( sockfd );
    
    return 0;
}

/*****************************************************************************
 Prototype    : Msg_Socket_Client_Send
 Description  : client 发送数据
 Input        : C8 *Msg  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 Msg_Socket_Client_Send(C8 *Msg, U32 size)
{
    S32 sockfd = -1;
    S32 ret = 0;

    if( NULL == Msg )
    {
        return -1;
    }
    
    sockfd = _Get_Socket_Handle();
    if( -1 == sockfd )
    {
        return -1;
    }
    
    pbiinfo("[Msg socket] msg:%s..%d..%s.\n",Msg,__LINE__,__FUNCTION__);
    ret = write( sockfd, Msg, size );
    if( -1 == ret )
    {
        pbierror("[Msg socket] write error!..%d..%s.\n",__LINE__,__FUNCTION__);
        pbierror("ret :%d..%s..\n\n",ret,strerror(errno));
    }

    return 0;
}

/*****************************************************************************
 Prototype    : Msg_Socket_Client_Destroy
 Description  : 销毁socket 句柄（断开连接）
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 Msg_Socket_Client_Destroy()
{
    S32 sockfd = -1;
    
    sockfd = _Get_Socket_Handle();
    if( -1 != sockfd )
    {
        close(sockfd);
    }
    return 0;
}




