/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : jni_staple.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/6/27
  Last Modified :
  Description   : jni_staple.cpp header file
  Function List :
  History       :
  1.Date        : 2012/6/27
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#ifndef __JNI_STAPLE_H__
#define __JNI_STAPLE_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define DeleteLocalRef(ENV,Obj)     if( NULL != (Obj) ) \
                                        (ENV)->DeleteLocalRef((jobject)(Obj)); \
                                    (Obj) = NULL

#define DeleteGlobalRef(ENV,Obj)    if( NULL != (Obj) ) \
                                        (ENV)->DeleteGlobalRef((jobject)(Obj)); \
                                    (Obj) = NULL


extern int GetIntJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name);
extern double GetDoubleJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name);
extern char GetCharJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name);
extern jbyte GetByteJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name);
extern jstring GetStringJavaToC(JNIEnv *env, jobject obj, jclass Clazz, const char *name );

extern void SetIntCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jint value);
extern void SetCharCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jchar value);
extern void SetStringJnitoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jstring str);
extern void SetShortCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jshort value);
extern void SetLongCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jlong value);
extern void SetDoubleCtoJava(JNIEnv *env, jobject obj, jclass Clazz, const char *name, jdouble value);
extern jstring stoJstring( JNIEnv* env, const char* pat );
extern jobject NewClsToObject(JNIEnv *env, jclass cls );

extern jstring stoJstring_UNICODE( JNIEnv* Env, const char* str, jsize jlen); 
extern jstring stoJstringGB2312( JNIEnv* Env, const char* str, jsize len);  
extern jstring stoJstring_UTF16( JNIEnv* Env, const char* str, jsize len); 
extern jstring stoJstring_UTF8( JNIEnv* Env, const char* str, jsize len);  
extern jstring stoJstring_GBK( JNIEnv* Env, const char* str, jsize len);

extern int Jstringtoc_UTF8( JNIEnv* env, jstring jstr, char* str);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __JNI_STAPLE_H__ */
