/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_pbi_dvb_dvbinterface_NativeInstallation */

#ifndef _Included_com_pbi_dvb_dvbinterface_NativeInstallation
#define _Included_com_pbi_dvb_dvbinterface_NativeInstallation
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationInit
  (JNIEnv *, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationStart
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativeInstallation/Install_Cfg_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationStart
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationStop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationStop
  (JNIEnv *, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationGetSearchRet
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativeInstallation/Install_Search_Result_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationGetSearchRet
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationFreeSearchRet
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationFreeSearchRet
  (JNIEnv *, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationDeinit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationDeinit
  (JNIEnv *, jobject);

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installMessageInit
 * Signature: (Lcom/pbi/dvb/dvbinterface/DVBMessage;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installMessageInit
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
