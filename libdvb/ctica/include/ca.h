/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc.
*
* Module name : CA
* File name   : ca.h
* Description : This file include definitions and API prototypes of CA module.
*
* History :
*   2012-07-03 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __CA_H_
#define __CA_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <dvbplayer.h>
//#include "Tr_Cas.h"

/*
typedef enum
{
    eCA_PostChanChangeState = 0,
    eCA_PostCATTable,
    eCA_PostPMTTable
} DVBPlayer_Event_e;

typedef struct 
{
    U16 wChannelId;
    U8 bState;
}PostChanChange_t;

typedef struct
{
    DVBPlayer_Event_e      eEventType;
    U32                         u32EventLen;
    void *                      pEventData;
}DVBPlayer_Event_t;
*/

//typedef void (*STB_CAMsg_Notify )(int  eCaMsg, unsigned short int wDemux, int ulParam,int param2);
typedef void (*CACORE_CAMsg_Notify )(U32	uEventType, U32 u32EventLen, unsigned char* pEventData, unsigned char* pPrivateData);
int CA_Init(void);
S32 CA_CallBack( DVBPlayer_Event_t tEvent );
void CTICA_RegMessage_Notify_CallBack(CACORE_CAMsg_Notify cb);
void CTICA_CardNumHexToDec(unsigned char *pHex, char *pDec);
//void CA_RegMessage_Notify_CallBack(STB_CAMsg_Notify cb);
//CAS_CUTTENT_STATE Cas_Get_Last_Err(void);
void CTICA_QueryControl( unsigned int qTyep, void* param, unsigned int arg1 );
int CTISC_GetScStatus();
//void CA_PostChanChangeState(PostChanChange_t *pCaPost);

#ifdef __cplusplus
}
#endif

#endif /* __CA_H_ */

