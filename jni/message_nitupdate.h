#ifndef __MESSAGE_NITUPDATE__
#define __MESSAGE_NITUPDATE__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


void Nit_Message_CallBack(int param1 );

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_NitMessageInit
(JNIEnv *env, jobject obj, jobject MsgObj);


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageNit_nitSearchVer
(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_NitMessageUnint
(JNIEnv *env, jobject obj);

extern void JNI_NITMsg_Logout(JNIEnv *env);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif 


