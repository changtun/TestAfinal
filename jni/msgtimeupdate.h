#ifndef __MSGTIMEUPDATE__
#define __MSGTIMEUPDATE__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


void Time_Message_CallBack( );

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativetime_TimeUpdateInit
(JNIEnv *env, jobject obj, jobject MsgObj);



JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativetime_TimeMessageUnint
(JNIEnv *env, jobject obj);

extern void JNI_TimeMsg_Logout(JNIEnv *env);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif 



