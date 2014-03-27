#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <nativehelper/jni.h>

#include "logit.h"
#include "jnistaple.h"


#define GBK_TYPE "GBK"
#define UTF8_TYPE	"UTF-8"
#define UTF16_TYPE	"UTF-16"
#define UNICODE_TYPE "unicode"

jobject NewClsToObject(JNIEnv *env, jclass cls )
{
	jobject jObj = NULL;
	jmethodID jMtdid = NULL;
	
	jMtdid = env->GetMethodID( cls, "<init>", "()V" );
	jObj = env->NewObject( cls, jMtdid );
	jMtdid = NULL;
	return jObj;
}


int GetIntJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name)
{
	jfieldID jFidTemp = NULL;
	int ret = 0;
	
	jFidTemp = env->GetFieldID( Clazz, name, "I" );
	ret = env->GetIntField( obj, jFidTemp );
	jFidTemp = NULL;
	return ret;
}

double GetDoubleJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name)
{
	jfieldID jFidTemp = NULL;
	double ret = 0;
	
	jFidTemp = env->GetFieldID( Clazz, name, "D" );
	ret = env->GetDoubleField( obj, jFidTemp );
	jFidTemp = NULL;
	return ret;
}

char GetCharJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name)
{
	jfieldID jFidTemp = NULL;
	char ret;
	
	jFidTemp = env->GetFieldID( Clazz, name, "C" );
	if( NULL == jFidTemp )
	{
		DEBUG_MSG( -1,"char java to c field ID  error!)))))))))))))))))))))))))s");
	}
	ret = env->GetCharField( obj, jFidTemp );
	jFidTemp = NULL;
	return ret;
}

jbyte GetByteJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name)
{
	jfieldID jFidTemp = NULL;;
	jbyte ret = 0;
	
	jFidTemp = env->GetFieldID( Clazz, name, "B" );
	ret = env->GetByteField( obj, jFidTemp );
	jFidTemp = NULL;
	return ret;
}

jstring GetStringJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name )
{
	jfieldID jFidTemp = NULL;;
	jstring ret = 0;
	
	jFidTemp = env->GetFieldID( Clazz, name, "Ljava/lang/String;" );
	ret = (jstring)env->GetObjectField( obj, jFidTemp );

	jFidTemp = NULL;
	return ret;
}

void SetIntCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jint value)
{
	jfieldID jFidTemp = NULL;;
	
	jFidTemp = env->GetFieldID( Clazz, name, "I" );
	env->SetIntField( obj, jFidTemp, value );
	jFidTemp = NULL;
}

void SetLongCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jlong value)
{
	jfieldID jFidTemp = NULL;;
	jFidTemp = env->GetFieldID( Clazz, name, "J" );
	env->SetLongField( obj, jFidTemp, value );
	jFidTemp = NULL;
}

void SetDoubleCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jdouble value)
{
	jfieldID jFidTemp = NULL;;
	jFidTemp = env->GetFieldID( Clazz, name, "D" );
	env->SetDoubleField( obj, jFidTemp, value );
	jFidTemp = NULL;
}

void SetShortCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jshort value)
{
	jfieldID jFidTemp = NULL;;
	jFidTemp = env->GetFieldID( Clazz, name, "S" );
	env->SetShortField( obj, jFidTemp, value );
	jFidTemp = NULL;
}
void SetCharCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jchar value)
{
	jfieldID jFidTemp = NULL;;

	jFidTemp = env->GetFieldID( Clazz, name, "C" );
	env->SetCharField( obj, jFidTemp, value );
	jFidTemp = NULL;
}


void SetStringJnitoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jstring str)
{
	jfieldID jFidTemp = NULL;;

	jFidTemp = env->GetFieldID( Clazz, name, "Ljava/lang/String;");
	env->SetObjectField( obj, jFidTemp, str );
	jFidTemp = NULL;
}

jstring stoJstring( JNIEnv* Env, const char* str)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=   strlen(str);

	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF(GBK_TYPE);
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);

	env->SetByteArrayRegion(barr,0,len,(jbyte*)str);
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );

	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );
	
	return rtn;
} 
jstring stoJstringGB2312( JNIEnv* Env, const char* str, jsize jLen)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=   jLen;
	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF("gb2312");
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);
	env->SetByteArrayRegion(barr,0,len,(jbyte*)str);
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );
	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );
	return rtn;
} 
jstring stoJstring_UNICODE( JNIEnv* Env, const char* str, jsize jlen)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=   jlen;
	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF(UNICODE_TYPE);
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);
	env->SetByteArrayRegion(barr,0,len,(jbyte*)str);
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );
	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );
	return rtn;
} 

jstring stoJstring_UTF16( JNIEnv* Env, const char* str, jsize jlen)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=   jlen;

	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF(UTF16_TYPE);
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);

	env->SetByteArrayRegion( barr, 0, len, (jbyte*)str );
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );

	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );

	return rtn;
} 

jstring stoJstring_UTF8( JNIEnv* Env, const char* str, jsize jLen)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=  jLen;

	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF(UTF8_TYPE);
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);

	env->SetByteArrayRegion( barr, 0, len, (jbyte*)str );
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );

	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );

	return rtn;
} 

jstring stoJstring_GBK( JNIEnv* Env, const char* str, jsize jLen)  
{
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len		=   jLen;

	jclass   clsstring  =   env->FindClass("java/lang/String");
	jstring   strencode =   env->NewStringUTF(GBK_TYPE);
	jmethodID   mid   	=   env->GetMethodID( clsstring, "<init>", "([BLjava/lang/String;)V" );
	jbyteArray   barr   =   env->NewByteArray(len);

	env->SetByteArrayRegion(barr,0,len,(jbyte*)str);
	rtn = (jstring)env->NewObject( clsstring, mid, barr, strencode );

	DeleteLocalRef( env, clsstring );
	DeleteLocalRef( env, strencode );
	DeleteLocalRef( env, barr );

	return rtn;
} 
int Jstringtoc_UTF8( JNIEnv* env, jstring jstr, char* str)
{
	jboolean isCopy;
	char* str1;
	if( NULL == str )
	{
		return -1;
	}
	str1=(char*)env->GetStringUTFChars( jstr, &isCopy);
	DEBUG_MSG2( -1,"Jstringtoc_UTF8!)))))))))))))))))))))))))%d %d",str1[0],str1[1]);
	memcpy(str,str1,strlen(str1));
	env->ReleaseStringUTFChars( jstr, str1);
	DEBUG_MSG2( -1,"11111 Jstringtoc_UTF8!)))))))))))))))))))))))))%d %d",str[0],str[1]);
	return 0;
}


