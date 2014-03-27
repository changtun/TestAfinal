#include <stdio.h>
#include <jni.h>


#include "logit.h"
#include "jnistaple.h"
#include "ca_manage.h"

#include <ca.h>
#include <ctiapi.h>
#include <ctisys.h>
#include <systime.h>

//#include <Tr_Cas.h>


#include "pbi_coding.h"



typedef struct
{
	jclass MsgClass;
	jclass	MailClass;			
	jclass      ippClass;
	jclass      FpClass;
	jclass      OsdClass;
	jclass      ScClass;
	jclass      pListClass;
	jclass      pInfoClass;
	jclass      uViewClass;
	jclass      pSkipClass;
	jclass      ppvNumClass;
	jclass      ppvListClass;
	jclass      ippvListClass;
	jmethodID Constructor;

}CA_MESSAGE_CALLBACK_t;

CA_MESSAGE_CALLBACK_t g_tCA_Msg_Callback;

static int  dvb_runing_flag =0;//1:ca消息注册了回调函数说明当前是在电视播放中；0:当前不再播放中

static unsigned char feedbuf[256];
static unsigned char feedlen;


bool _register_ca_msg_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;
	jmethodID jMethodID = NULL;
#if 1
	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessageCa");//env->GetObjectClass(PlayerMsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG1(-1, "_register_uti_ca_msg_callback_methods error! %d.",__LINE__);
		return false;
	}
	

	g_tCA_Msg_Callback.MsgClass = (jclass)env->NewGlobalRef((jobject)jClass); 
	if( NULL == g_tCA_Msg_Callback.MsgClass )
	{
		DEBUG_MSG(-1, "_register_uti_ca_msg_callback_methods error!");
		return false;
	}

	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$maildata");
	if( NULL == jClass )
	{
		DEBUG_MSG1(-1, "_register_uti_ca_msg_callback_methods error!%d.",__LINE__);
		return false;
	}
	
	g_tCA_Msg_Callback.MailClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.MailClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ippnotifymsg");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.ippClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.ippClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	/*jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$fingerPrintMsg");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.FpClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.FpClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}*/

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$osdDisplayStatus");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.OsdClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.OsdClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$smartCardInfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.ScClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.ScClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ppidListInfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.pListClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.pListClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ppidInfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.pInfoClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.pInfoClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$userViewInfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.uViewClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.uViewClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$proSkipInfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.pSkipClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.pSkipClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ppvnumber");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.ppvNumClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.ppvNumClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ppvlistinfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.ppvListClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.ppvListClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}

	jClass= env->FindClass("com/pbi/dvb/dvbinterface/MessageCa$ippvlistinfo");
	if(jClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return false;
	}

	g_tCA_Msg_Callback.ippvListClass = (jclass)env->NewGlobalRef((jobject)jClass);  
	if(g_tCA_Msg_Callback.ippvListClass ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return false;
	}
	
//	g_tCA_Msg_Callback.Constructor = 	  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "<init>", "()V");
	DeleteLocalRef( env, jClass );
#endif
#if 0




#endif
	return true;
}

bool _logout_ca_msg_callback_methods(JNIEnv *env)
{
    DeleteGlobalRef( env, g_tCA_Msg_Callback.MsgClass );
	g_tCA_Msg_Callback.MsgClass  = NULL;
	
    DeleteGlobalRef( env, g_tCA_Msg_Callback.MailClass);
	g_tCA_Msg_Callback.MailClass = NULL;
	
    DeleteGlobalRef( env, g_tCA_Msg_Callback.ippClass);
	g_tCA_Msg_Callback.ippClass = NULL;
	
    	//DeleteGlobalRef( env, g_tCA_Msg_Callback.FpClass);
	//g_tCA_Msg_Callback.FpClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.OsdClass);
	g_tCA_Msg_Callback.OsdClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.ScClass);
	g_tCA_Msg_Callback.ScClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.pListClass);
	g_tCA_Msg_Callback.pListClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.pInfoClass);
	g_tCA_Msg_Callback.pInfoClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.uViewClass);
	g_tCA_Msg_Callback.uViewClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.pSkipClass);
	g_tCA_Msg_Callback.pSkipClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.ppvNumClass);
	g_tCA_Msg_Callback.ppvNumClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.ppvListClass);
	g_tCA_Msg_Callback.ppvListClass = NULL;
	
	DeleteGlobalRef( env, g_tCA_Msg_Callback.ippvListClass);
	g_tCA_Msg_Callback.ippvListClass = NULL;
	
	return true;
}



void _CaStatusMessage(JNIEnv *env, jobject obj, int type, int status)
{
	jclass jClass = NULL; 
	jmethodID   Method = NULL;
	Method = env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaStatusMessage", "(II)V");
	if(Method ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	DEBUG_MSG2( -1, "_CaStatusMessage type %d status %d ", type, status);
	env->CallVoidMethod( obj, Method, type, status);

}

void _CaOsdDisplayMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL; 		
	jmethodID	jmethods=NULL;

	CTI_CAOsdDisplayStatus_t  *temp =(CTI_CAOsdDisplayStatus_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaOsdDisplayMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.OsdClass, "CaOsdDisplayMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$osdDisplayStatus;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaOsdDisplayMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaOsdDisplayMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.OsdClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaOsdDisplayMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iAvailBit",temp->avail);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iLoopNum",temp->loop_num);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iLoopInterval",temp->loop_interval);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iLoopSpeed",temp->loop_speed);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iOsdLx",temp->coordinate_x_lt);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iOsdLy",temp->coordinate_y_lt);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iOsdRx",temp->coordinate_x_rt);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iOsdRy",temp->coordinate_y_rt);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iFontSize",temp->font_size);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iFontColor", (temp->font_R<<24 | temp->font_G<<16 | temp->font_B<<8 | temp->font_Z));	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.OsdClass, "iBackColor", (temp->back_R<<24 | temp->back_G<<16 | temp->back_B<<8 | temp->back_Z));

	DEBUG_MSG1(-1,T("ca _CaOsdDisplayMessage--------eCaMsg LINE %d--------------------OK"),__LINE__);

	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
}

void _CaSCInfoMessage(JNIEnv *env, jobject obj, int param2)
{
	jobject 	MsgObj=NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;
	char buff[128];

	CTI_SCInfoNotify_t  *temp =(CTI_SCInfoNotify_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaSCInfoMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaSCInfoMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$smartCardInfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaSCInfoMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaSCInfoMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.ScClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaSCInfoMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iScStatus",temp->scstatus);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iUsedStartus",temp->usedstatus);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iSysId", (temp->systemid[0]<<8 | temp->systemid[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iEcmSysId", (temp->ecmsubsysid[0]<<8 | temp->ecmsubsysid[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iEmmSysId", (temp->emmsubsysid[0]<<8 | temp->emmsubsysid[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iParentRating",temp->parentalrating);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iGroupCtrl",temp->groupcontrol);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iAntiMoveCtrl", (temp->antimovectrl[0]<<16 | temp->antimovectrl[1]<<8 | temp->antimovectrl[2]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iSCType",temp->smartcard_type);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iSCMatch",temp->smartcard_matching);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iSTBMatch",temp->stb_ird_matching);

	//sprintf(buff, "%02x%02x%02x%02x%02x%02x%02x%02x", \
		//temp->cardnumber[0],temp->cardnumber[1],temp->cardnumber[2],temp->cardnumber[3],temp->cardnumber[4],temp->cardnumber[5],temp->cardnumber[6],temp->cardnumber[7]);
	CTICA_CardNumHexToDec(temp->cardnumber, buff);
	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ScClass, "setsCardNum", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)buff, (int)strlen(buff) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ScClass, "setsCardVer", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)temp->cosversion, (int)strlen(temp->cosversion) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ScClass, "setsCardType", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)temp->cardlabel, (int)strlen(temp->cardlabel) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ScClass, "setsIsSueTime", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)temp->issuetime, (int)strlen(temp->issuetime) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ScClass, "setsExpireTime", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)temp->expiretime, (int)strlen(temp->expiretime) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );
	
	if(temp->smartcard_type == 1)
	{
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iWordPeriod", (temp->childCard.work_period[0]<<8 | temp->childCard.work_period[1]));
		SetLongCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "lMoutherUa",( temp->childCard.mother_ua[0]<<32  | temp->childCard.mother_ua[1]<<24 | temp->childCard.mother_ua[2]<<16 | temp->childCard.mother_ua[3]<<8 | temp->childCard.mother_ua[4]));
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartYear",(temp->childCard.verify_start_time[0]<<8 | temp->childCard.verify_start_time[1]));
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartMonth",temp->childCard.verify_start_time[2]);
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartDay",temp->childCard.verify_start_time[3]);
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartHour",temp->childCard.verify_start_time[4]);
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartMinute",temp->childCard.verify_start_time[5]);
		SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ScClass, "iStartSecond",temp->childCard.verify_start_time[6]); 		
	}

	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
	
}

void _CaServiceMessage(JNIEnv *env, jobject obj, int code, int pronum, int index)
{
	jclass jClass = NULL; 
	jmethodID   Method = NULL;
	Method = env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaServiceMessage", "(III)V");
	if(Method ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	DEBUG_MSG3( -1, "CA Message service pronum %d index %d ErrIdx %d ",pronum, index,  code );
	env->CallVoidMethod( obj, Method, code, pronum, index);
}

jobject getCaPPidInfo(JNIEnv *env, CTI_PPidInfo_t * pInfo)
{
	jobject 	MsgObj=NULL;
	jmethodID	jMetidTemp=NULL;
	jstring 	jstringtemp=NULL;
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.pInfoClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return NULL;
	}
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pInfoClass, "iPPid",( pInfo->id[0]<<16 | pInfo->id[1]<<8 | pInfo->id[2] ));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pInfoClass, "iUsedStatus", pInfo->usedstatus);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pInfoClass, "iCreaditDeposit", (pInfo->credit_deposit[0]<<24 | pInfo->credit_deposit[1]<<16 | pInfo->credit_deposit[2]<<8 | pInfo->credit_deposit[3] ));

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pInfoClass, "setsLabel", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)pInfo->label, (int)strlen(pInfo->label) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pInfoClass, "setsCreditDate", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)pInfo->credit_date, (int)strlen(pInfo->credit_date) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pInfoClass, "setsStartDate", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)pInfo->prog_right_start_date, (int)strlen(pInfo->prog_right_start_date) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pInfoClass, "setsEndDate", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)pInfo->prog_right_end_date, (int)strlen(pInfo->prog_right_end_date) );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );
	
	for(int i = 0; i < 8; i++)
	{
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pInfoClass, "setsiProgRight", "(II)V");	
		env->CallVoidMethod( MsgObj, jMetidTemp, i,  (pInfo->prog_right[i]<<24 | pInfo->prog_right[i+1]<<16 | pInfo->prog_right[i+2]<<8 | pInfo->prog_right[i+3] ));
	}

	return MsgObj;
}

void _CaPPidInfoMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL;
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	
	CTI_PPidInfo_t  *temp =(CTI_PPidInfo_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaPPidInfoMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$ppidInfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}

	MsgObj = getCaPPidInfo(env, temp);
	if(MsgObj == NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidInfoMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
}

void _CaPPidListMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL, jObjTemp; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	int i = 0;

	CTI_PPidNotify_t  *temp =(CTI_PPidNotify_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaPPidListMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$ppidListInfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}	
	
	DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	

	MsgObj =env->AllocObject(g_tCA_Msg_Callback.pListClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaPPidListMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	for(i = 0; i < temp->count; i++)
	{
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pListClass, "setpListInfo", "(Lcom/pbi/dvb/dvbinterface/MessageCa$ppidInfo;)V");
		jObjTemp = getCaPPidInfo(env, &(temp->info[i]));
		if(jObjTemp == NULL)
		{
			DEBUG_MSG1(-1,T("ca _CaPPidInfoMessage--------eCaMsg LINE %d--------------------"),__LINE__);
			break;
		}
		env->CallVoidMethod( MsgObj, jMetidTemp, jObjTemp );
		DeleteLocalRef(env, jObjTemp );
	}
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pListClass, "iCount", i);
	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
	
}

void _CaUseViewMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;
	CTI_UserView temp;
	if(param2 == 0)
	{
		DEBUG_MSG1(-1,T("ca _CaUseViewMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	memset(&temp, 0, sizeof(CTI_UserView));
	memcpy(((char *)&temp)+5, (char *)param2, (sizeof(temp)-5));
	

	/*CTI_UserView  *temp =(CTI_UserView *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaUseViewMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}*/
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaUseViewMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$userViewInfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaUseViewMessage--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.uViewClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaUseViewMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iType",temp.type);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iPinCode",(temp.PinCode[0]<<24 | temp.PinCode[1]<<16 | temp.PinCode[2]<8 | temp.PinCode[3]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iFlag",temp.flag);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStatus",temp.status);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartChannel", (temp.control_start_channel[0]<<8 | temp.control_start_channel[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndChannel", (temp.control_end_channel[0]<<8 | temp.control_end_channel[1]));

	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartYear",(temp.control_start_time[0]<<8 | temp.control_start_time[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartMonth",temp.control_start_time[2]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartDay",temp.control_start_time[3]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartHour",temp.control_start_time[4]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartMinute",temp.control_start_time[5]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iStartSecond",temp.control_start_time[6]); 		
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndYear",(temp.control_end_time[0]<<8 | temp.control_end_time[1]));		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndMonth", temp.control_end_time[2]); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndDay", temp.control_end_time[3]); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndHour", temp.control_end_time[4]);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndMinute", temp.control_end_time[5]);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.uViewClass, "iEndSecond", temp.control_end_time[6]);	

	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
	
}

void _CaPPVListMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL, jObjTemp = NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;

	CTI_PPVListNotify_t  *temp =(CTI_PPVListNotify_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaPPVListMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$ppvlistinfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.ppvListClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ppvListClass, "iPage", temp->page);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ppvListClass, "iProgCount", temp->prog_count);

	for(int i = 0; i < temp->prog_count; i++)
	{
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ppvListClass, "setaPPvNum", "(Lcom/pbi/dvb/dvbinterface/MessageCa$ppvnumber;)V");
		if(jMetidTemp ==NULL )
		{
			DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
			DeleteLocalRef(env, MsgObj );
			return;
		}
		jObjTemp =env->AllocObject(g_tCA_Msg_Callback.ppvNumClass);
		if(jObjTemp ==NULL )
		{
			DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
			DeleteLocalRef(env, MsgObj );
			return;
		}
		SetIntCtoJava( env, jObjTemp, g_tCA_Msg_Callback.ppvNumClass, "iStartNum", (temp->ppv[i].start_number[0]<<24 |temp->ppv[i].start_number[1]<<16 |temp->ppv[i].start_number[2]<<8 |temp->ppv[i].start_number[3] ));	
		SetIntCtoJava( env, jObjTemp, g_tCA_Msg_Callback.ppvNumClass, "iEndNum", (temp->ppv[i].end_number[0]<<24 |temp->ppv[i].end_number[1]<<16 |temp->ppv[i].end_number[2]<<8 |temp->ppv[i].end_number[3] ));	
		env->CallVoidMethod( MsgObj, jMetidTemp, jObjTemp );
		DeleteLocalRef(env, jObjTemp );
	}
	
	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
	
}

void _CaIppvListMessage(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL, jObjTemp = NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;

	CTI_IPPVListNotify_t  *temp =(CTI_IPPVListNotify_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "CaIPPVListMessage","(Lcom/pbi/dvb/dvbinterface/MessageCa$ippvlistinfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.ippvListClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ippvListClass, "iPage", temp->page);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.ippvListClass, "iProgCount", temp->prog_count);

	for(int i = 0; i < temp->prog_count; i++)
	{
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ippvListClass, "setiIppvNum", "(II)V");
		if(jMetidTemp == NULL)
		{
			DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
			DeleteLocalRef(env, MsgObj );
			return;
		}
		env->CallVoidMethod( MsgObj, jMetidTemp, i,  (temp->ippv[i].prog_number[0]<<24|temp->ippv[i].prog_number[1]<<16|temp->ippv[i].prog_number[2]<<8|temp->ippv[i].prog_number[3]));
		//DeleteLocalRef(env, jObjTemp );
	}
	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
}
void _CaMailMessage(JNIEnv *env, jobject obj,int param2)
{	
	jobject 	MsgObj=NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;
#if 1
	CTI_CABmailInfo_t  *temp =(CTI_CABmailInfo_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	if(temp->tag == 0x83)
	{
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "emailNotify","(Lcom/pbi/dvb/dvbinterface/MessageCa$maildata;)V");
	}
	else if(temp->tag == 0x84)
	{
		jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "osdNotify","(Lcom/pbi/dvb/dvbinterface/MessageCa$maildata;)V");
	}
	else
	{
		jmethods = NULL;
	}
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.MailClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.MailClass, "setbMsgTitle", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)temp->title_context, (int)temp->title_len );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	DEBUG_MSG1(-1,T("ca _CaMailMessage--------setbMsgTitle[%s]--------------------"), temp->title_context);

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.MailClass, "setbMsgData", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)temp->body_context, (int)temp->body_len );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	DEBUG_MSG1(-1,T("ca _CaMailMessage--------setbMsgData[%s]--------------------"), temp->body_context);
	
	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.MailClass, "setbMsgFrom", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)temp->from_context, (int)temp->from_len );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	DEBUG_MSG1(-1,T("ca _CaMailMessage--------setbMsgFrom[%s]--------------------"), temp->from_context);

	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "bStatus",temp->status);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "bType",temp->tag);
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgYear",(temp->send_time[0]<<8 | temp->send_time[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgMonth",temp->send_time[2]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgDay",temp->send_time[3]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgHour",temp->send_time[4]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgMinute",temp->send_time[5]);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgSecond",temp->send_time[6]); 		
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateYear",(temp->build_time[0]<<8 | temp->build_time[1]));		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateMonth", temp->build_time[2]); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateDay", temp->build_time[3]); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateHour", temp->build_time[4]);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateMinute", temp->build_time[5]);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateSecond", temp->build_time[6]);	

	DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------OK"),__LINE__);

	env->CallVoidMethod( obj,jmethods,MsgObj);
	DeleteLocalRef(env, MsgObj );
	
#else
	CAS_MSG_STRUCT	*temp =(CAS_MSG_STRUCT	*)param2;
	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "emailNotify","(Lcom/pbi/dvb/dvbinterface/MessageCa$maildata;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}

	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	//MsgObj =NewClsToObject(env,MsgClass);
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.MailClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.MailClass, "setbMsgTitle", "(Ljava/lang/String;)V");
	//jstringtemp = stoJstring_GBK( env, (const char*)(temp->bMsgTitle));
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, (char*)temp->bMsgTitle, (int)temp->wTitleLen );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp ); 		

	DeleteLocalRef( env, jstringtemp );
	
	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.MailClass, "setbMsgData", "(Ljava/lang/String;)V");
	//jstringtemp = stoJstring_GBK( env, (const char*)(temp->bMsgData));
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgData[0]*/0x14, (char*)temp->bMsgData, (int)temp->wDataLen );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );


	DeleteLocalRef( env, jstringtemp );
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "wIndex",temp->wIndex);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "bType",temp->bType);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "bClass",temp->bClass);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "bPriority",temp->bPriority);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "wPeriod",temp->wPeriod);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgYear",(temp->sMsgTime.bYear[0])*100+(temp->sMsgTime.bYear[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgMonth",temp->sMsgTime.bMonth);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgDay",temp->sMsgTime.bDay);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgHour",temp->sMsgTime.bHour);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgMinute",temp->sMsgTime.bMinute);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sMsgSecond",temp->sMsgTime.bSecond); 		
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateYear",(temp->sCreateTime.bYear[0])*100 +(temp->sCreateTime.bYear[1]));		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateMonth",temp->sCreateTime.bMonth); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateDay",temp->sCreateTime.bDay); 		
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateHour",temp->sCreateTime.bHour);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateMinute",temp->sCreateTime.bMinute);	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.MailClass, "sCreateSecond",temp->sCreateTime.bSecond);	
	
	DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------OK"),__LINE__);


	env->CallVoidMethod( obj,jmethods,MsgObj);

	DeleteLocalRef(env, MsgObj );
#endif
}
void _CaIppUpdate(JNIEnv *env, jobject obj,int param2)
{
	jobject 	ippObj=NULL; 		
	jmethodID	jMetidTemp=NULL;
	jstring 	jstringtemp=NULL;
	int *temp =(int*)param2;
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	jMetidTemp=env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "ippUpdateNotify","(I)V");
	if( NULL == jMetidTemp)
	{
		return;
	}
	DEBUG_MSG2(-1,T("ca _CaIppUpdate--------eCaMsg	LINE %d--------------------*temp%d"),__LINE__,*temp);
	env->CallVoidMethod( obj,jMetidTemp,*temp);	
}

void _CaIppNotifyMessage(JNIEnv *env, jobject obj,int param2)
{
	
	jobject 	ippObj=NULL; 		
	jmethodID	jMetidTemp=NULL,jmethos1=NULL;
	jstring 	jstringtemp=NULL;
	jclass      ippClass=NULL;
	
	#if 1
	CTI_IppvInfoNotify_t *temp = (CTI_IppvInfoNotify_t*)param2;
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppNotifyMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	
	jmethos1=      env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "ippNotify",  "(Lcom/pbi/dvb/dvbinterface/MessageCa$ippnotifymsg;)V");
	if(jmethos1  ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	ippObj =env->AllocObject(g_tCA_Msg_Callback.ippClass);
	if(ippObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iChannelId", (temp->prog_number[0]<<8 | temp->prog_number[1]) );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iStatus", temp->status );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iIppId", (temp->ppid_id[0]<<16 | temp->ppid_id[1]<<8  | temp->ppid_id[2]) );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iType_p", temp->p_type_support );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iType_t", temp->t_type_support );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iIppvNum", (temp->ippv_prog_num[0]<<24 | temp->ippv_prog_num[1]<<16  | temp->ippv_prog_num[2]<<8  | temp->ippv_prog_num[3]) );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iPrice_p", (temp->ippv_p_price[0]<<24 | temp->ippv_p_price[1]<<16  | temp->ippv_p_price[2]<<8  | temp->ippv_p_price[3]) );
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "iPrice_t", (temp->ippv_t_price[0]<<24 | temp->ippv_t_price[1]<<16  | temp->ippv_t_price[2]<<8  | temp->ippv_t_price[3]) );
	{
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ippClass, "setsIppidName", "(Ljava/lang/String;)V");
		jstringtemp = CodingToJstring( env, /*(int)temp->pbContent[0]*/CHARACTER_TYPE_DEFAULT, (char*)temp->ppid_label, (int)strlen(temp->ppid_label) );
		env->CallVoidMethod( ippObj, jMetidTemp, jstringtemp );
		DeleteLocalRef( env, jstringtemp );
	}

	env->CallVoidMethod( obj, jmethos1,ippObj);

   	DeleteLocalRef(env, ippObj );
	DEBUG_MSG1(-1,T("ca _CaIppNotifyMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
	#else
	CAS_IPPNOTIFY_STRUCT *temp =(CAS_IPPNOTIFY_STRUCT*)param2;
	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppNotifyMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	
	jmethos1=      env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "ippNotify",  "(Lcom/pbi/dvb/dvbinterface/MessageCa$ippnotifymsg;)V");
	if(jmethos1  ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaIppUpdate--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	ippObj =env->AllocObject(g_tCA_Msg_Callback.ippClass);
	if(ippObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "wChannelId",temp->wChannelId);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "bIppType",temp->bIppType);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "ulIppCharge",temp->ulIppCharge);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "ulIppUnitTime",temp->ulIppUnitTime);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "ulIppRunningNum",temp->ulIppRunningNum);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startYear",((temp->ulIppStart.bYear[0])*100)+(temp->ulIppStart.bYear[1]));
	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startMonth",temp->ulIppStart.bMonth);	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startDay",temp->ulIppStart.bDay);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startHour",temp->ulIppStart.bHour);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startMinute",temp->ulIppStart.bMinute);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "startSecond",temp->ulIppStart.bSecond);


	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endYear",(temp->ulIppEnd.bYear[0]*100)+temp->ulIppEnd.bYear[1]);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endMonth",temp->ulIppEnd.bMonth);	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endDay",temp->ulIppEnd.bDay);	
	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endHour",temp->ulIppEnd.bHour);	
	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endMinute",temp->ulIppEnd.bMinute);	
	
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "endSecond",temp->ulIppEnd.bSecond);
	SetIntCtoJava( env, ippObj, g_tCA_Msg_Callback.ippClass, "bContentLen",temp->bContentLen);

	if(temp->bContentLen>0)
	{
	
		DEBUG_MSG2(-1,T("ca _CaIppNotifyMessage--------eCaMsg  LINE %d  contentlen%d--------------------"),__LINE__,temp->bContentLen);
		{
			int im=0;
			for(im=0;im<temp->bContentLen;im++)
			{
				DEBUG_MSG1(-1,T(" %x "),(temp->pbContent)[im]);
			}

		}

		
		jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.ippClass, "setpbContent", "(Ljava/lang/String;)V");
		jstringtemp = CodingToJstring( env, /*(int)temp->pbContent[0]*/0x14, (char*)temp->pbContent, (int)temp->bContentLen );
		env->CallVoidMethod( ippObj, jMetidTemp, jstringtemp );

		DeleteLocalRef( env, jstringtemp );

	}

	env->CallVoidMethod( obj, jmethos1,ippObj);

   	DeleteLocalRef(env, ippObj );
	DEBUG_MSG1(-1,T("ca _CaIppNotifyMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
#endif
}

/*强制频道切换完成后，需要锁定当前节目，不再响应遥控器操作*/
void _caEnhanceChanPlay(JNIEnv *env, jobject obj,int param2)
{
	jobject 	MsgObj=NULL; 		
	jmethodID	jMetidTemp=NULL,jmethods=NULL;
	jstring 	jstringtemp=NULL;
	CTI_ProgSkipInfo_t skipdata;
	CTI_ProgSkipNotify_t  *temp =(CTI_ProgSkipNotify_t *)param2;	
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	skipdata = temp->skipdata;
	
	jmethods =  env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "enhancechNotify","(Lcom/pbi/dvb/dvbinterface/MessageCa$proSkipInfo;)V");
	if(jmethods ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaMailMessage--------eCaMsg  LINE %d--------------------"),__LINE__);	
	
	MsgObj =env->AllocObject(g_tCA_Msg_Callback.pSkipClass);
	if(MsgObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca CAS_MC_NOTIFY_SHORT_MESSAGE--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iProNum", (temp->prognumber[0]<<8 | temp->prognumber[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iStatus", skipdata.status);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iType", skipdata.type);
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iNetidOrFrq", (skipdata.netid_or_frq[0]<<8 | skipdata.netid_or_frq[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iTsidOrMod", (skipdata.tsid_or_mod[0]<<8 | skipdata.tsid_or_mod[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iSrvidOrSyb", (skipdata.srvid_or_symbol[0]<<8 | skipdata.srvid_or_symbol[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iEsId", (skipdata.es_id[0]<<8 | skipdata.es_id[1]));
	SetIntCtoJava( env, MsgObj, g_tCA_Msg_Callback.pSkipClass, "iMsgLen", skipdata.message_len);

	jMetidTemp = env->GetMethodID( g_tCA_Msg_Callback.pSkipClass, "setsMsg", "(Ljava/lang/String;)V");
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/CHARACTER_TYPE_DEFAULT, (char*)skipdata.message, (int)skipdata.message_len );
	env->CallVoidMethod( MsgObj, jMetidTemp, jstringtemp );
	DeleteLocalRef( env, jstringtemp );

	env->CallVoidMethod( obj, jmethods,MsgObj);
   	DeleteLocalRef(env, MsgObj );
#if 0	
	CAS_FORCECHANNEL_STRUCT  *temp =(CAS_FORCECHANNEL_STRUCT*)param2;

	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _caEnhanceChanPlay--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	jMetidTemp =    env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "enhancechNotify","(IIII)V");
	if(jMetidTemp==NULL)
	{
		DEBUG_MSG1(-1,T("ca _caEnhanceChanPlay--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	env->CallVoidMethod(obj,jMetidTemp,(int)(temp->bLockFlag),(int)(temp->wNetwrokId),(int)(temp->wTsId),(int)(temp->wServId));
#endif
}

/*返回值:-1,输入参数错误；-2:当前卡未插入；0:成功；1失败；2:输入参数错误；3:密码错误
4: 智能卡nvram错误；5:内存不足;6:产品空间不足*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_PurchaseIpp
(JNIEnv *env, jobject obj,jobject dataobj , jstring jsPin)
{
	
	jclass 			jobjCLS=NULL;
	jmethodID 		jMetid=NULL;	
	jfieldID 		jFidTemp=NULL;

	int ret =0,i =0;
	
	unsigned char ucNewPin[10];
#if 0	
	CAS_IPPPURCHASE_STRUCT ippdata;

	CAS_SCARD_STATUS state;


	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==failure ret =%d",__FUNCTION__,__LINE__,ret);
		return -1;
	}
			
	jobjCLS =env->GetObjectClass( dataobj );
	if( NULL == jobjCLS )
	{
		ret = -1;
		DEBUG_MSG2( -1,"ERROR func %s=======%d============= \n",__FUNCTION__,__LINE__);
		return -1;
	}

	
	memset( ucNewPin, 0, 10 );

	
	ret = Jstringtoc_UTF8( env, jsPin, (char *)ucNewPin );
	if( 0 != ret )
	{
		DEBUG_MSG1( -1,"ERROR PurchaseIpp=======%d============= \n",__LINE__);
		return -1;
	}	
	
	
	for(i=0;i<6;i++)
	{
		ucNewPin[i]=ucNewPin[i]-0x30;
		DEBUG_MSG1( -1,"PIN==%d============= \n",ucNewPin[i]);
	}
	

	memset(&ippdata,0,sizeof(CAS_IPPPURCHASE_STRUCT));

	ippdata.bIppType =GetIntJavaToC(env,dataobj,jobjCLS,"bIppType");
	ippdata.ulIppCharge =GetIntJavaToC(env,dataobj,jobjCLS,"ulIppCharge");
	ippdata.ulIppRunningNum=GetIntJavaToC(env,dataobj,jobjCLS,"ulIppRunningNum");
	ippdata.wChannelId=GetIntJavaToC(env,dataobj,jobjCLS,"wChannelId");
	ippdata.ulIppUnitTime=GetIntJavaToC(env,dataobj,jobjCLS,"ulIppUnitTime");
	
	ippdata.ulPurchaseNum=GetIntJavaToC(env,dataobj,jobjCLS,"ulPurchaseNum");

	DEBUG_MSG1( -1," PurchaseIpp====IPPTYPE%d\n",ippdata.bIppType);
	DEBUG_MSG1( -1," PurchaseIpp====ulIppCharge%d\n",ippdata.ulIppCharge);
	DEBUG_MSG1( -1," PurchaseIpp====ulIppRunningNum%d\n",ippdata.ulIppRunningNum);
	DEBUG_MSG1( -1," PurchaseIpp====wChannelId%d\n",ippdata.wChannelId);
	DEBUG_MSG1( -1," PurchaseIpp===ulIppUnitTime%d\n",ippdata.ulIppUnitTime);
	DEBUG_MSG1( -1," PurchaseIpp===ulPurchaseNum%d\n",ippdata.ulPurchaseNum);
	ret =MC_IppPurchase(ucNewPin,6,ippdata);
	
	DEBUG_MSG2( -1," PurchaseIpp=======%d============= ret %d\n",__LINE__,ret);
#endif	
	return ret;
	
}


void _CaFp(JNIEnv *env, jobject obj, int type, int param2)
{
	
	jmethodID jmethodtemp=NULL;	
	jstring        jstringtemp=NULL;
	char *str = (char*)param2;
	char buff[128];
	if(str ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}
	
	jmethodtemp =env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "FpNotify","(ILjava/lang/String;)V");
	if(jmethodtemp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	sprintf(buff, "%02x%02x%02x%02x%02x%02x%02x%02x", \
		str[0],str[01],str[2],str[3],str[4],str[5],str[6],str[7],str[8]);
	for(int i = 0; buff[i] != '\0'&& i<128; i++ )
	{
		if(buff[i]>='a' && buff[i]<='z')
		{
			buff[i] -= 0x20;
		}
	}
	DEBUG_MSG2(-1,T("ca _CaFp--------eCaMsg  LINE %d---[%s]-----------------"),__LINE__, buff);
	jstringtemp = CodingToJstring( env, /*(int)temp->bMsgTitle[0]*/0x14, buff, (int)strlen(buff) );
	env->CallVoidMethod(obj,jmethodtemp, type, jstringtemp);
	DeleteLocalRef( env, jstringtemp );
#if 0
	CAS_FINGERPRINT_STRUCT *temp =(CAS_FINGERPRINT_STRUCT *)param2;
	if(temp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}


	jmethodtemp =env->GetMethodID(g_tCA_Msg_Callback.MsgClass, "FpNotify","(Lcom/pbi/dvb/dvbinterface/MessageCa$fingerPrintMsg;)V");
      if(jmethodtemp ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg LINE %d--------------------"),__LINE__);
		return;
	}
	
	DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------"),__LINE__);
	
	fpObj =env->AllocObject(g_tCA_Msg_Callback.FpClass);
	if(fpObj ==NULL)
	{
		DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------"),__LINE__);
		return;
	}

	
	DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------"),__LINE__);
	SetIntCtoJava( env, fpObj, g_tCA_Msg_Callback.FpClass, "hashnumlen",temp->bHashedNumLen);
	if(temp->bHashedNumLen>0)
	{		
		jMetid = env->GetMethodID( g_tCA_Msg_Callback.FpClass, "sethashnum", "(Ljava/lang/String;)V");
		//jstringtemp = stoJstring_GBK( env, (const char*)(temp->pbHashedNum));
		
		jstringtemp = CodingToJstring( env, /*(int)temp->pbHashedNum[0]*/0x14, (char*)temp->pbHashedNum, (int)temp->bHashedNumLen );
		env->CallVoidMethod( fpObj, jMetid, jstringtemp );

		DeleteLocalRef( env,jstringtemp );
    }

	env->CallVoidMethod(obj,jmethodtemp,fpObj);

    DeleteLocalRef( env, fpObj );
	DEBUG_MSG1(-1,T("ca _CaFp--------eCaMsg  LINE %d--------------------ok"),__LINE__);
#endif
}




/*判断是否需要显示到期提醒*/
/*返回值定义: 0,不需要显示；1:需要显示；*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_CheckReminder
(JNIEnv *env, jobject obj)
{
	signed short ret =0;
	int 		i =0;
#if 0
	CAS_SCARD_STATUS state;
	CAS_TIMESTAMP   starttime;
	CAS_TIMESTAMP   entime;
	unsigned int remain;
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==failure ret =%d",__FUNCTION__,__LINE__,ret);
		return 0;
	}

	
	DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d== start",__FUNCTION__,__LINE__);
	
	for(i =0; i<320; i++)
	{
		if(dvb_runing_flag !=1)/*当前推出电视播放了*/
		{
			DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d== start  times%d",__FUNCTION__,__LINE__,i);
			return 0;
		}
		
		ret = MC_TIME_GetEntitleInfo((unsigned char *)(&starttime),(unsigned char *)(&entime),&remain,i);
		if((ret==1)||(ret==2))
		{
			DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
			continue;
		}
		
	//	DEBUG_MSG3(-1,"%s +++++++++++++++++++++LINE %d  remain %d" ,__FUNCTION__,__LINE__,remain);
		if((remain <=5)&&(remain>0))
		{
			DEBUG_MSG3(-1,"%s---REMINDER OK+++++++++++++++++++++=%d==ret",__FUNCTION__,__LINE__,ret);
			return 1;
		}
	}
	DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d== ok",__FUNCTION__,__LINE__);
	#endif
	return 0;

	

}

/*从母卡中读取喂养数据，用于喂养流程*/
/*返回值:0 成功，非0，失败*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_ReadFeedDataFromMaster
(JNIEnv *env, jobject obj)
{
	unsigned short ret =0;
#if 0	
	CAS_SCARD_STATUS state;

	DEBUG_MSG2(-1,"%s +++++++++++++++++++++LINE %d",__FUNCTION__,__LINE__);

	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==failure ret =%d",__FUNCTION__,__LINE__,ret);
		return -1;
	}

	memset(feedbuf,0,256);
	feedlen =0;
	
	ret =MC_ReadFeedDataFromMaster(feedbuf,&feedlen);
	if(ret !=0)
	{		
		DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret %d",__FUNCTION__,__LINE__,ret);
	}
	
	DEBUG_MSG3(-1,"%s  complete+++++++++++++++++++++=%s==len %d",__FUNCTION__,feedbuf,feedlen);
#endif
	return ret;

}

/*将喂养数据写入子卡*/
/*返回值:0 成功，非0，失败*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_WriteFeedDataToSlaver
(JNIEnv *env, jobject obj)
{
	
	unsigned short ret =0;

#if 0	
	CAS_SCARD_STATUS state;

	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==failure ret =%d",__FUNCTION__,__LINE__,ret);
		return -1;
	}
	
	ret =MC_WriteFeedDataToSlaver(feedbuf,(unsigned char)feedlen);
	
	if(ret !=0)
	{		
		DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret %d",__FUNCTION__,__LINE__,ret);
	}
	
	DEBUG_MSG3(-1,"%s  complete+++++++++++++++++++++=%s==len %d",__FUNCTION__,feedbuf,feedlen);
#endif
	return ret;

}

/*得到当前的ca的errcode,对应E00~E36*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_GetLastErrcode
(JNIEnv *env, jobject obj)
{
	int status =0;
#if 0
	 status =Cas_Get_Last_Err();
	 DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret %d",__FUNCTION__,__LINE__,status);
#endif
	 return status;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageCa_SetReminderStatus
(JNIEnv *env,jobject obj, int status)
{	
	dvb_runing_flag =status;
	DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret %d",__FUNCTION__,__LINE__,status);
	return 0;
}


