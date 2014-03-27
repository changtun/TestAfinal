/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : dvb_socket_message.c
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/6/18
  Last Modified :
  Description   : dvb socket message 
  Function List :
  History       :
  1.Date        : 2012/6/18
    Author      : zxguan
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pvddefs.h"
#include "pbitrace.h"
#include "client.h"
#include "dvb_socket_message.h"

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
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
#define MAX_MSG_SIZE 100

#define SOCKET_MSG_INTERRUPT                1

#define SOCKET_MSG_INSTALL_COMPLETED        10
#define SOCKET_MSG_INSTALL_FAILED           11
#define SOCKET_MSG_INSTALL_PROGRESS         12
#define SOCKET_MSG_INSTALL_TP_COMPLETED     13
#define SOCKET_MSG_INSTALL_TP_FAILED        14
#if 0
#define SOCKET_MSG_CA_
#define SOCKET_MSG_CA_
#define SOCKET_MSG_CA_
#define SOCKET_MSG_CA_
#define SOCKET_MSG_CA_
#define SOCKET_MSG_CA_
#endif

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/


/*****************************************************************************
 Prototype    : Installation_Call_Back
 Description  : installation call back
 Input        : Install_Notify_Type_e eNotifyType  
                void * pData                       
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/6/18
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
void Installation_Call_Back(Install_Notify_Type_e eNotifyType, void * pData)
{
    C8 Message[MAX_MSG_SIZE];

    memset( Message, 0, MAX_MSG_SIZE );
    
    switch( eNotifyType )
    {
        case eINSTALL_COMPLETED: /* 搜索完成，返回搜索结果 */
        {
            snprintf( Message, MAX_MSG_SIZE, "%d", SOCKET_MSG_INSTALL_COMPLETED );
            
        }break;
        case eINSTALL_INTERRUPT: /* 搜索中断，返回搜索结果 */
        {
            snprintf( Message, MAX_MSG_SIZE, "%d", SOCKET_MSG_INTERRUPT );
            
        }break;    
        case eINSTALL_FAILED: /* 搜索失败 */
        {
            snprintf( Message, MAX_MSG_SIZE, "%d", SOCKET_MSG_INSTALL_FAILED );
            
        }break;
        case eINSTALL_PROGRESS: /* 显示搜索进度，及当前搜索频点 */
        {
            Install_Progress_t *pProgress = NULL;
            
            if( NULL == pData )
            {
                pbierror("pData is NULL...%d..%s.\n",__LINE__,__FUNCTION__);
                return;
            }

            pProgress = (Install_Progress_t *)pData;
            
            if( eDVBCORE_SIGNAL_SOURCE_DVBC == pProgress->puCurrTuner[0].eSignalSource )
            {
                snprintf( Message, MAX_MSG_SIZE,"%d\n%d\n%d", SOCKET_MSG_INSTALL_PROGRESS, \
                    pProgress->u32Progress,pProgress->puCurrTuner[0].tCable.u32Freq);
            }
            
        }break;            
        case eINSTALL_TP_COMPLETED: /* 完成一个频点，返回完成频点的搜索数据 */
        {
            Install_Search_Result_t *pRes = NULL;
            
            if( NULL == pData )
            {
                pbierror("pData is NULL...%d..%s.\n",__LINE__,__FUNCTION__);
                return;
            }
            
            pRes = (Install_Search_Result_t *)pData;
            
            if( eDVBCORE_SIGNAL_SOURCE_DVBC == pRes->ptServList[0].ptTpDesc[0].uTunerDesc.eSignalSource )
            {
                snprintf( Message, MAX_MSG_SIZE, "%d\n%d\n%s", \
                    SOCKET_MSG_INSTALL_TP_COMPLETED,
                    pRes->ptServList[0].ptTpDesc[0].uTunerDesc.tCable.u32Freq, \
                    pRes->ptServList[0].sServName );
            }
            
        }break;            
        case eINSTALL_TP_FAILED: /* 搜索一个频点失败 */
        {
            snprintf( Message, MAX_MSG_SIZE, "%d", SOCKET_MSG_INSTALL_TP_FAILED );
            
        }break;            
        default:
        {
            return;
            
        }break;            
    }

    Msg_Socket_Client_Send( Message, MAX_MSG_SIZE );
    
}

