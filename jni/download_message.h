#ifndef __MESSAGE_DOWNLOADMSG__
#define __MESSAGE_DOWNLOADMSG__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


void Download_Call_Back( int eNotifyType, int pData );

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadMessageInit
(JNIEnv *env, jobject obj, jobject InMsgObj);

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadMessageUnInit
(JNIEnv *env, jobject obj);

void JNI_Download_Logout(JNIEnv *env);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif 



