

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>


#include "pvddefs.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal_timeshift.h"

#include "dal.h"
#include "nphls.h"


#include "log/anpdebug.h"

#define DeleteLocalRef(ENV,Obj)     if( NULL != (Obj) ) \
                                        (*ENV)->DeleteLocalRef(ENV, (jobject)(Obj)); \
                                    (Obj) = NULL

#define DeleteGlobalRef(ENV,Obj)    if( NULL != (Obj) ) \
                                        (*ENV)->DeleteGlobalRef(ENV, (jobject)(Obj)); \
                                    (Obj) = NULL




static nphls_Notify_pfn g_hls_cb;
static jobject g_HlsObj = NULL;
static JavaVM* g_HlsVM = NULL;



static int hls_bug = 1;
void hls_debug(char *str_temp)
{
    if(hls_bug == 0)
        return;
    DebugMessage("===   PXWANG	hls_debug	CallBy Function:-------------------------------------------%s", str_temp);
}

int NpHLS_Debug(int flag)
{
	hls_bug = flag;
	return 0;
}


void _nphls_result_cb( U32 event, void* param)
{
	hls_debug(__FUNCTION__);
	NpHLS_CallBack(event);
	/*
	switch (event)
	{
		case DAL_PLAY_EVENT_PLAY_START:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_START,"DAL_PLAY_EVENT_PLAY_START");
		}
		break;
		case DAL_PLAY_EVENT_NEW_PROGRESS:
		{
			g_hls_cb(DAL_PLAY_EVENT_NEW_PROGRESS,"DAL_PLAY_EVENT_NEW_PROGRESS");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_END:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_END,"DAL_PLAY_EVENT_PLAY_END");
		}
		break;
		case DAL_PLAY_EVENT_FLVLIST_END:
		{
			g_hls_cb(DAL_PLAY_EVENT_FLVLIST_END,"DAL_PLAY_EVENT_FLVLIST_END");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_START_FAILED:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_START_FAILED,"DAL_PLAY_EVENT_PLAY_START_FAILED");
		}
		break;
		case DAL_PLAY_EVENT_BUFFERING:
		{
			g_hls_cb(DAL_PLAY_EVENT_BUFFERING,"DAL_PLAY_EVENT_BUFFERING");
		}
		break;
		case DAL_PLAY_EVENT_DATA_INTERRUPT:
		{
			g_hls_cb(DAL_PLAY_EVENT_DATA_INTERRUPT,"DAL_PLAY_EVENT_DATA_INTERRUPT");
		}
		break;
		case DAL_PLAY_EVENT_DATA_RESUME:
		{
			g_hls_cb(DAL_PLAY_EVENT_DATA_RESUME,"DAL_PLAY_EVENT_DATA_RESUME");
		}
		break;
		case DAL_PLAY_EVENT_RESTART_SAME_URL:
		{
			g_hls_cb(DAL_PLAY_EVENT_RESTART_SAME_URL,"DAL_PLAY_EVENT_RESTART_SAME_URL");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_WILL_STOP:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_WILL_STOP,"DAL_PLAY_EVENT_PLAY_WILL_STOP");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_PAUSE_OK:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_PAUSE_OK,"DAL_PLAY_EVENT_PLAY_PAUSE_OK");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_RESUME_OK:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_RESUME_OK,"DAL_PLAY_EVENT_PLAY_RESUME_OK");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_SEEK_OK:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_SEEK_OK,"DAL_PLAY_EVENT_PLAY_SEEK_OK");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START,"DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE,"DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE");
		}
		break;
		case DAL_PLAY_EVENT_FIRST_FRAME:
		{
			g_hls_cb(DAL_PLAY_EVENT_FIRST_FRAME,"DAL_PLAY_EVENT_FIRST_FRAME");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_STOP_OK:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAY_STOP_OK,"DAL_PLAY_EVENT_PLAY_STOP_OK");
		}
		break;
		case DAL_PLAY_EVENT_MMS_PLAY_START_FAILED:
		{
			g_hls_cb(DAL_PLAY_EVENT_MMS_PLAY_START_FAILED,"DAL_PLAY_EVENT_MMS_PLAY_START_FAILED");
		}
		break;
		case DAL_PLAY_EVENT_PLAYREC_START_OK:
		{
			g_hls_cb(DAL_PLAY_EVENT_PLAYREC_START_OK,"DAL_PLAY_EVENT_PLAYREC_START_OK");
		}
		break;
		case DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED:
		{
			g_hls_cb(DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED,"DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED");
		}
		break;
		case DAL_PLAY_EVENT_P2V_RESUME_FAILED:
		{
			g_hls_cb(DAL_PLAY_EVENT_P2V_RESUME_FAILED,"DAL_PLAY_EVENT_P2V_RESUME_FAILED");
		}
		break;
		default:
		break;
	}*/
}


int NpHLS_Init(void)
{
    int iret = 0;
	hls_debug(__FUNCTION__);
    iret = DAL_Init();
    if( iret != 0)
    {
        DebugMessage("[DAL]:DAL_Init failed\n");
        return DAL_ERROR_BAD_PARAMETER;
    }
	INIT_TIMESHIFT();
	DAL_Set_PlayEventCallback(1,_nphls_result_cb);
    return 0;
}

int NpHls_Register(nphls_Notify_pfn result_cb)
{
	hls_debug(__FUNCTION__);
    g_hls_cb = result_cb;
    return 11;
}



int NpHLS_Term(void)
{
    int iret = 0;
	hls_debug(__FUNCTION__);
    iret = DAL_Term();
    if( iret != 0)
    {
        DebugMessage("[DAL]:NpHLS_Term failed\n");
        return DAL_ERROR_BAD_PARAMETER;
    }
    return 0;
}

int NpHLS_Start(int play_type,char *Play_url)
{
    int Error = 0;
    DAL_Player_Input_t  	DAL_Input;
    DAL_PlayerInstance_t    player = 1;
    DAL_Player_Output_t    	output;

	hls_debug(__FUNCTION__);
	if(play_type == SourceType_TIMESHIFT)
	{
		Current_timeshift_t xs;
		INIT_TIMESHIFT();
		memset(&xs,0,sizeof(Current_timeshift_t));
		strcpy(xs.http_address,Play_url);
		DebugMessage("[DAL][dal_http_start]:NpHLS_Start with %s\n", xs.http_address);
		xs.time_info=60;
		TIMESHIFT_ADD_ONE(xs);
		
		
	}
	
    memset(&output, 0, sizeof(DAL_Player_Output_t));
    memset(&DAL_Input.Source.http.URL, 0, 255);
    strncpy(DAL_Input.Source.http.URL, Play_url, strlen(Play_url));
    DAL_Input.SourceType = play_type;
    Error = DAL_Stream_Start(player, &DAL_Input, &output);
	DebugMessage("[DAL][dal_http_start]:DAL_Stream_Start with %s\n");
    if( Error != 0)
    {
        DebugMessage("[DAL][dal_http_start]:DAL_Stream_Start failed with %d\n", Error);
        return Error;
    }
    return 0;
}

int NpHLS_List_Add(char *Play_url)
{
	Current_timeshift_t xs;

	memset(&xs,0,sizeof(Current_timeshift_t));
	strcpy(xs.http_address,Play_url);
	DebugMessage("[DAL][dal_http_start]:NpHLS_Start with %s\n", xs.http_address);
	xs.time_info=60;
	TIMESHIFT_ADD_ONE(xs);
	return 0;
}







int NpHLS_Seek_Next(void)
{
    int ret = 0;
	DAL_Player_Input_t 		DAL_Input;
	DAL_PlayerInstance_t    player=1;
	DAL_Player_Output_t    output;

	hls_debug(__FUNCTION__);

	memset(&output,0,sizeof(DAL_Player_Output_t));
	memset(&DAL_Input.Source.http.URL,0,255);

	
    ret = DAL_Stream_Seek_next(player,&DAL_Input,&output);
    if( ret != 0)
        DebugMessage("[dal_http_stop]:DAL_Stream_Seek_next failed with %d\n", ret);
    else
        DebugMessage("[dal_http_stop] OK\n");
    return ret;
}


int NpHLS_Stop(void)
{
    int ret = 0;
    DAL_PlayerInstance_t m_player = 1;

	hls_debug(__FUNCTION__);
    ret = DAL_Stream_Stop(m_player);
    if( ret != 0)
        DebugMessage("[dal_http_stop]:DAL_Stream_Stop failed with %d\n", ret);
    else
        DebugMessage("[dal_http_stop] OK\n");
    return ret;
}

int NpHLS_Register_Callback(JNIEnv *env, jobject objData)
{
	jclass jClass = NULL;
	DebugMessage("NpHLS_Register_Callback Start!");
	(*env)->GetJavaVM(env, &g_HlsVM);
	
	DeleteGlobalRef(env, g_HlsObj); 	
	g_HlsObj = (*env)->NewGlobalRef(env, objData);
	DebugMessage("NpHLS_Register_Callback OK!");
	//NpHLS_CallBack(0xFF, "test");
	return 1;
}
void NpHLS_CallBack(int type)
{
	JNIEnv *	env = NULL;
	jclass 		jClass = NULL; 
	jobject 	MsgObj = NULL; 		
	jmethodID	jMetidTemp = NULL;
	jstring 	jStr = NULL;
	DebugMessage("NpHLS_CallBack Start!");
	if( NULL == g_HlsVM || NULL == g_HlsObj)
	{
		DebugMessage("NpHLS_CallBack Parm error!");
		return;
	}
	
	if ((*g_HlsVM)->AttachCurrentThread(g_HlsVM, &env, NULL) != JNI_OK || NULL == env)
	{
		DebugMessage("NpHLS_CallBack env error!");
		return;
	}

	jClass = (*env)->GetObjectClass( env, g_HlsObj);
	if( NULL == jClass )
	{
		DebugMessage("[zshang][%s|%d][jClass not found]",__FUNCTION__, __LINE__ );
		return;
	}	
	jMetidTemp = (*env)->GetMethodID(env,  jClass, "HlsCallBackToJs", "(I)V");	
	(*env)->CallVoidMethod( env, g_HlsObj, jMetidTemp, (jint)type);
	DebugMessage("NpHLS_CallBack OK!");
	
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_nppvware_NpHls_HlsRegisterCallback
(JNIEnv *env, jobject obj, jobject objData)
{	
	DebugMessage("HlsRegisterCallback Start!");
	return (jint)NpHLS_Register_Callback(env, objData);
}







