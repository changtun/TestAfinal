/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : tuner.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/7/11
  Last Modified :
  Description   : jni_staple.cpp header file
  Function List :
  History       :
  1.Date        : 2012/7/11
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

#ifndef __TUNER_H__
#define __TUNER_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern bool GetDvbCroeCableDescClass(JNIEnv *env, jobject ObjCable, DVBCore_Cab_Desc_t *ptCable );
extern bool SetDvbCroeCableDescClass(JNIEnv *env, jobject ObjCable, DVBCore_Cab_Desc_t tCable );

/*
 * Class:     com_pbi_dvb_dvbinterface_Tuner
 * Method:    getTunerStrengthPercent
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerStrengthPercent
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_pbi_dvb_dvbinterface_Tuner
 * Method:    getTunerQualityPercent
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerQualityPercent
  (JNIEnv *, jobject, jint);

/*
 * Class:     com_pbi_dvb_dvbinterface_Tuner
 * Method:    getTunerCablePercent
 * Signature: (Lcom/pbi/dvb/dvbinterface/Tuner/DVBCore_Cab_Desc_t;Lcom/pbi/dvb/dvbinterface/Tuner/CablePercent;)V
 */
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerCablePercent
  (JNIEnv *, jobject, jobject, jobject);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
