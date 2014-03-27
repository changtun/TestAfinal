#include <stdio.h>
#include <jni.h>


#include "logit.h"
#include "jnistaple.h"

#include "player_manage.h"
#include <psi_si.h>

typedef struct
{
	jclass MsgClass;
	jmethodID Constructor;
//	jclass FpClass;
//	jmethodID playerMsgInterrupt;
//	jmethodID playerMsgPmtTimeOut;
//	jmethodID playerMsgAVinterrupt;
//	jmethodID playerMsgTunerLock;
	//jmethodID playerTunerUnLock;
//	jmethodID playerMsgStart;
	
}PLAYER_MESSAGE_CALLBACK_t;

typedef struct
{
	int audiopid;
	int audiotype;
	int audiolangcode;
}audio_infos_t;
static PLAYER_MESSAGE_CALLBACK_t g_tPlayer_Msg_CallBack;
jclass 		     jCls;
jclass 		     jlistcls; 


bool _register_player_msg_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;
	jmethodID jMethodID = NULL;
	
	memset( &g_tPlayer_Msg_CallBack, 0, sizeof(PLAYER_MESSAGE_CALLBACK_t) );

	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessagePlayer$AudioInfos");
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods error1!");
		return false;
	}
	jCls = (jclass)env->NewGlobalRef((jobject)jClass);
	if(jCls == NULL)		 
	{
		DEBUG_MSG1(-1," _register_player_msg_callback_methods  is NULL ********************** line=%d.",__LINE__);
		return false;
	}

	
	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessagePlayer$oneCHaudio");
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods error2!");
		return false;
	}
	
	jlistcls = (jclass)env->NewGlobalRef((jobject)jClass);
	if(jlistcls ==NULL)
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		return false;
	}

	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessagePlayer");//env->GetObjectClass(PlayerMsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods error3!");
		return false;
	}
	
	g_tPlayer_Msg_CallBack.MsgClass = (jclass)env->NewGlobalRef((jobject)jClass);
	if( NULL == g_tPlayer_Msg_CallBack.MsgClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods Msg error4!");
		return false;
	}



//	g_tPlayer_Msg_CallBack.Constructor = env->GetMethodID(g_tPlayer_Msg_CallBack.MsgClass, "playerMsgInterrupt", "()V");

	DeleteLocalRef( env, jClass );

	DEBUG_MSG(-1,"_register_player_msg_callback_methods ok");
	return true;
}

bool _logout_player_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_tPlayer_Msg_CallBack.MsgClass );
	g_tPlayer_Msg_CallBack.MsgClass = NULL;

	return true;
}

void _playerMsgInterrupt(JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgInterrupt", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_playerMsgInterrupt end %d.",__LINE__);
}

void _playerMsgPmtTimeOut(JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgPmtTimeOut", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
}

void _playerMsgAVinterrupt(JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;
	
	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgAVinterrupt", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
}

void _playerMsgTunerLock(JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgTunerLock", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
}
void _playerMsgAudioInfos(JNIEnv *env, jobject obj,void *temp,int totals )
{
	jmethodID        Method = NULL,method1 =NULL;
	jobject 		MsgObj=NULL; 		
	int 			ii =0;
	jfieldID 		jFidTemp=NULL;
   	 jobjectArray  	jArrList=NULL;
    	jobject 		jObjListOne=NULL;	
	PMT_Component_t *temp1;  
	int				num1 =0;
	jstring jStrServName=NULL;
	PMT_Component_t   *temp2=NULL;
	int				num2 =0,sum =0,i=0;
	audio_infos_t  infos[16];
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		return ;
	}
	memset(infos,0,sizeof(audio_infos_t)*16);
	temp1=((PMT_Section_t*)temp)->ptAudio;
	num1 =((PMT_Section_t*)temp)->u16AudioNum;
	if((num1>0)&&(temp1!=NULL))
	{
		for(i=0;i<num1;i++)
		{
			infos[sum].audiopid =temp1[i].u16ElementaryPid;
			infos[sum].audiotype=temp1[i].u8StreamType;
			if(temp1[i].ptLangDesc!=NULL)
			{
				if((temp1[i].ptLangDesc->u16LangNum>0)&&(temp1[i].ptLangDesc->ptLangInfo!=NULL))
				{
					infos[sum].audiolangcode =temp1[i].ptLangDesc->ptLangInfo[0].u32LangCode;
					DEBUG_MSG2(-1,"%d -----code%x",__LINE__,infos[sum].audiolangcode);
				}
			}
			sum++;
		}
	}
	if((((PMT_Section_t*)temp)->ptComponent!=NULL)&&(((PMT_Section_t*)temp)->u16ComponentNum!=0))
	{
		temp2 =((PMT_Section_t*)temp)->ptComponent;
		num2 =((PMT_Section_t*)temp)->u16ComponentNum;
	}
	if((num2!=0 )&&(temp2!=NULL))
	{	
		int  mm =0,nn=0;
		for(mm =0;mm<num2; mm++ )
		{	
			if(temp2[mm].u8StreamType ==STREAM_TYPE_PRIVATE_PES_DATA)
			{
				if((temp2[mm].u16OtherDescNum >0)
					&&(temp2[mm].ptDesc!=NULL))
				{
					for(nn=0;nn<temp2[mm].u16OtherDescNum;nn++)
					{
						if(temp2[mm].ptDesc[nn].u8Tag ==ENHANCE_AC3_DESCRIPTOR_TAG)
						{
							if(sum<16)
							{	
								infos[sum].audiopid =temp2[mm].u16ElementaryPid;
								infos[sum].audiotype=ENHANCE_AC3_DESCRIPTOR_TAG;
								if(temp2[mm].ptLangDesc!=NULL)
								{
									if((temp2[mm].ptLangDesc->u16LangNum>0)&&(temp2[mm].ptLangDesc->ptLangInfo!=NULL))
									{
										infos[sum].audiolangcode =temp2[mm].ptLangDesc->ptLangInfo[0].u32LangCode;
										DEBUG_MSG2(-1,"_playerMsgAudioInfos======pid %x  lancode %x",infos[sum].audiopid,
											infos[sum].audiolangcode);
									}
								}
								sum++;
							}						
						}
					}
				}
			}
		}
	}
	if(sum==0)
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		return ;
	}
	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgAudioInfo", "(Lcom/pbi/dvb/dvbinterface/MessagePlayer$AudioInfos;)V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		return ;
	}
	MsgObj =env->AllocObject(jCls);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	method1 = env->GetMethodID( jCls, "Bulidaudiolist", "(ILcom/pbi/dvb/dvbinterface/MessagePlayer$AudioInfos;)V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		return ;
	}
	env->CallVoidMethod( MsgObj, method1,sum, MsgObj );
	//env->DeleteLocalRef( (jobject)method1);
	jFidTemp = env->GetFieldID( jCls,"audiolist","[Lcom/pbi/dvb/dvbinterface/MessagePlayer$oneCHaudio;");
	jArrList = (jobjectArray)env->GetObjectField( MsgObj, jFidTemp );
	if( NULL == jArrList )
	{
		DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);
		
		DeleteLocalRef( env, jCls );
		return ;
	}
	DEBUG_MSG1(-1,"Player Message Method    line=%d.",__LINE__);
	SetIntCtoJava( env, MsgObj, jCls,"totals",sum);
	DEBUG_MSG1(-1,"Player Message Method	line=%d.",__LINE__);
	for(ii=0; ii<sum; ii++ )
	{
		DEBUG_MSG1(-1,"Player Message Method	line=%d.",__LINE__);
		jObjListOne = env->GetObjectArrayElement( jArrList, ii );
		if( NULL == jObjListOne )
		{
			DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);

			DeleteLocalRef( env, jCls );
			return ;
		}
		DEBUG_MSG1(-1,"Player Message Method	line=%d.",__LINE__);
		SetIntCtoJava( env, jObjListOne, jlistcls,"audiotype",(jint)infos[ii].audiotype);
		SetIntCtoJava( env, jObjListOne, jlistcls,"audiopid",(jint)infos[ii].audiopid);
		if(infos[ii].audiolangcode!=0)
		{
			int  t1=infos[ii].audiolangcode-0x202020;
				char  t2[3] ={0,0,0};
				t2[0]=(t1&0x00ff0000)>>16;
				t2[1]=(t1&0x0000ff00)>>8;
				t2[2]= (char)(t1&0x000000ff);
				DEBUG_MSG4(-1,"t1%x --- code[0] %x code[1]%x  code[ 2] %x ",t1,t2[0],t2[1],t2[2]);
				jStrServName = stoJstring_UTF8( env, t2,3);
				if(jStrServName ==NULL)
				{
					DEBUG_MSG1(-1,"Player Message ERROR	line=%d.",__LINE__);
					break;
				}
				SetStringJnitoJava(env,jObjListOne,jlistcls,"languagecode",jStrServName);
				DeleteLocalRef( env, jStrServName );

		}
		DeleteLocalRef( env, jObjListOne );

	}
	env->CallVoidMethod( obj, Method, MsgObj );

	DeleteLocalRef( env, MsgObj );

}

void _playerMsgStart(JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgStart", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
}

void _playerCaMode(JNIEnv *env, jobject obj,int mode)
{
	jmethodID   Method = NULL;
	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgCaMode", "(I)V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, mode);
	Method = NULL;

}

void _playerMsgUpdatePmtInfo(JNIEnv *env, jobject obj, int TsId, int ServId, int PmtPid )
{
	jmethodID Method = NULL;

	Method = env->GetMethodID( g_tPlayer_Msg_CallBack.MsgClass, "playerMsgUpdatePmtInfo", "(III)V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, TsId, ServId, PmtPid );
	Method = NULL;
}
