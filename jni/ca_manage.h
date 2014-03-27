#ifndef __CA_MANAGE_H__
#define __CA_MANAGE_H__

//#include <uti_udrm_type.h>


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


extern bool _register_ca_msg_callback_methods(JNIEnv *env);
extern bool _logout_ca_msg_callback_methods(JNIEnv *env);
extern void _CaServiceMessage(JNIEnv *env, jobject obj, int code, int pronum, int index);
extern void _CaMailMessage(JNIEnv *env, jobject obj,int param2);
extern void _caEnhanceChanPlay(JNIEnv *env, jobject obj,int param2);
extern void _CaIppNotifyMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaIppUpdate(JNIEnv *env, jobject obj,int param2);
extern void _CaStatusMessage(JNIEnv *env, jobject obj, int type, int status);
extern void _CaOsdDisplayMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaStatusMessage(JNIEnv *env, jobject obj, int type, int status);
extern void _CaSCInfoMessage(JNIEnv *env, jobject obj, int param2);
extern void _CaPPidListMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaPPidInfoMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaUseViewMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaPPVListMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaIppvListMessage(JNIEnv *env, jobject obj,int param2);
extern void _CaFp(JNIEnv *env, jobject obj, int type, int param2);
	


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_PurchaseIpp
(JNIEnv *env, jobject obj,jobject dataobj , jstring jsPin);

//extern void _CaFp(JNIEnv *env, jobject obj,int param2);
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_CheckReminder
(JNIEnv *env, jobject obj);
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_ReadFeedDataFromMaster
(JNIEnv *env, jobject obj);
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_WriteFeedDataToSlaver
(JNIEnv *env, jobject obj);


/*得到当前的ca的errcode,对应E00~E36*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_GetLastErrcode
(JNIEnv *env, jobject obj);

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_SetReminderStatus
(JNIEnv *env,jobject obj, int status);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CA_MANAGE_H__ */


