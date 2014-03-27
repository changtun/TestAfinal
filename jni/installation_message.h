/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : installation_message.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/7/16
  Last Modified :
  Description   : installation_message.cpp header file
  Function List :
  History       :
  1.Date        : 2012/7/16
    Author      : zxguan
    Modification: Created file

******************************************************************************/

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

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#ifndef __INSTALLATION_MESSAGE_H__
#define __INSTALLATION_MESSAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern void Installation_Call_Back( Install_Notify_Type_e eNotifyType, void * pData);
extern void JNI_Installation_Logout(JNIEnv *env);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __INSTALLATION_MESSAGE_H__ */
