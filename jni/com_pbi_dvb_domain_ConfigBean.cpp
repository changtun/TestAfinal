#include <stdio.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include <com_pbi_dvb_domain_ConfigBean.h>

#include "file_config.h"
#include "jnistaple.h"

JNIEXPORT void JNICALL Java_com_pbi_dvb_domain_ConfigBean_getParams
  (JNIEnv *env, jobject clazz, jobject configObj)
{
	jclass configBean = NULL;
	CONFIG_WRITE_T tWriteInfo;

    configBean = env->GetObjectClass(configObj);
    if (NULL == configBean)
    {
    	return;
	}

      GetDVBConfig(&tWriteInfo);
      SetIntCtoJava( env,  configObj,  configBean, "bandwidth", tWriteInfo.eBandWidth);
      SetIntCtoJava( env,  configObj,  configBean, "batLimit", tWriteInfo.eBatSwitch);
      SetIntCtoJava( env,  configObj,  configBean, "encodeType", tWriteInfo.eEncode);

      DeleteLocalRef( env,  configBean);
}

JNIEXPORT void JNICALL Java_com_pbi_dvb_domain_ConfigBean_setParams
  (JNIEnv *env, jobject clazz, jobject configObj)
{
	jclass configBean = NULL;
	CONFIG_WRITE_T tWriteInfo;

	configBean = env->GetObjectClass(configObj);
	if (NULL == configBean)
	{
	    	return;
	}

	tWriteInfo.eBandWidth = (eTUNER_BAND_WIDTH)GetIntJavaToC(env,configObj, configBean, "bandwidth");
	tWriteInfo.eBatSwitch = (eSEARCH_BAT_SWITCH)GetIntJavaToC(env,configObj, configBean, "batLimit");
	tWriteInfo.eEncode = (eDEFAULT_CHARACTER_ENCODE)GetIntJavaToC(env,configObj, configBean, "encodeType");
	SetDVBConfig(tWriteInfo);

	DeleteLocalRef(env, configBean);
}
