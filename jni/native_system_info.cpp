#include <stdio.h>
#include <jni.h>

#include "logit.h"
#include "jnistaple.h"
#include "native_system_info.h"
#include "system_info.h"


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeSystemInfo_SoftWareInit
  (JNIEnv *env, jobject obj, jstring jSoftWare)
{
	char cSoftWare[100];

	memset( cSoftWare, 0, 100 ); 
	Jstringtoc_UTF8(env, jSoftWare, cSoftWare);
	setSoftWareID(cSoftWare);

	return 0;
}
