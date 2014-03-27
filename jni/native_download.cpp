#include <stdio.h>
#include <jni.h>

#include "logit.h"
#include "jnistaple.h"
#include "native_download.h"
#include "dvb_download.h"


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadStart
(JNIEnv *env, jobject obj)
{

	Dvb_DownLoad_Start( );
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadGetStatus
(JNIEnv *env, jobject obj)
{
	int satus = 0;
	satus = Dvb_DownLoad_GetStatus( );

	return satus;
}