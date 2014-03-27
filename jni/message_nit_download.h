#ifndef __MESSAGE_DL__
#define __MESSAGE_DL__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


void DL_Nit_Message_CallBack(int param1 );

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_DLMessageInit
(JNIEnv *env, jobject obj, jobject MsgObj);


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_DLMessageUnint
(JNIEnv *env, jobject obj);

extern void JNI_DLNITMsg_Logout(JNIEnv *env);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif 



