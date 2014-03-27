#ifndef __CA_MESSAGE_H__
#define __CA_MESSAGE_H__

//#include <uti_udrm_type.h>
//#include <Tr_Cas.h>
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_CaMessageInit
(JNIEnv *env, jobject obj, jobject CaMsgObj);

extern void JNI_CaMsg_Logout(JNIEnv *env);
//extern void CA_Message_CallBack(int eCaMsg, unsigned short wDemux,  int ulParam,int param2   );
extern void CA_Message_CallBack(U32	uEventType, U32 u32EventLen, unsigned char* pEventData, unsigned char* pPrivateData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CA_MESSAGE_H__ */

