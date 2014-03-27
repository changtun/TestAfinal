#include <stdio.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include "logit.h"

#include <native_epg.h>
#include "jnistaple.h"
#include <epg_common.h>
#include <epg.h>
#include <systime.h>
#include "pbi_coding.h"




/*banner条信息取当前播放event*/
/*serviceid,tsid,ornetowrkid: 要获取事件的频道信息*/
/*pevent :传入型参数，获取的下一个事件信息，由jni获取并传递到此实例中*/
/*返回值0;成功；-1:失败*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetPevent
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject pevent)
{
  
      
	jclass peventCls=NULL;
	jmethodID jMetidTemp=NULL;
	jstring jeventname=NULL;

	  

	time_td_t	tStart_Time;
	time_td_t tend_time;
	char cEvent_Name[257];
	int ratevalue =0;

	EVENT*  ptemp =NULL;
	  
	peventCls = env->GetObjectClass( pevent );
	if( NULL == peventCls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		return -1;
	}

	  /* 取当前事件*/
	 ptemp =  App_Get_Pf_Event(serviceid,tsid,ornetowrkid,0);
	 if(ptemp == NULL)
	 {
	//	 DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		DeleteLocalRef( env, peventCls );
		return -1;
	 }

	  /*取当前事件*/
		
	 
	  /*转换时间 格式 */

	memset(cEvent_Name,0,257);

	App_Get_Event_Start_Time(&tStart_Time,ptemp);
	App_Get_Event_End_Time(  &tend_time,  ptemp);
	  

	  
	SetShortCtoJava( env, pevent, peventCls, "event_id", (jshort)(ptemp->event_id));	  
	SetShortCtoJava( env, pevent, peventCls, "start_year",(jshort)(tStart_Time.year));
	SetShortCtoJava( env, pevent, peventCls, "start_month",(jshort)(tStart_Time.month));
	SetShortCtoJava( env, pevent, peventCls, "start_date",(jshort)(tStart_Time.date));
	SetShortCtoJava( env, pevent, peventCls, "start_hour",(jshort)(tStart_Time.hour));
	SetShortCtoJava( env, pevent, peventCls, "start_minute",(jshort)(tStart_Time.minute));	  
	SetShortCtoJava( env, pevent, peventCls, "start_second",(jshort)(tStart_Time.second));



	SetShortCtoJava( env, pevent, peventCls, "end_year",(jshort)(tend_time.year));
	SetShortCtoJava( env, pevent, peventCls, "end_month",(jshort)(tend_time.month));
	SetShortCtoJava( env, pevent, peventCls, "end_date",(jshort)(tend_time.date));
	SetShortCtoJava( env, pevent, peventCls, "end_hour",(jshort)(tend_time.hour));
	SetShortCtoJava( env, pevent, peventCls, "end_minute",(jshort)(tend_time.minute)); 	
	SetShortCtoJava( env, pevent, peventCls, "end_second",(jshort)(tend_time.second));

	SetShortCtoJava( env, pevent, peventCls, "content_nibble_level",(jshort)(ptemp->content_nibble_level));

	if(ptemp->parental_rating!=NULL)
	{
		ratevalue =ptemp->parental_rating->rate;
	}

	
	SetIntCtoJava( env, pevent, peventCls, "rate",(jint)(ratevalue));
#if 0
	DEBUG_MSG1(-1,"EventId %d..",ptemp->event_id);
	DEBUG_MSG6(-1,"PPPPPP+++ starttime[%d%d%d][%d:%d:%d]", \
		tStart_Time.year,tStart_Time.month,tStart_Time.date, \
		tStart_Time.hour,tend_time.minute,tend_time.second );
#endif	
//	DEBUG_MSG1(-1,"PPPPPP+++ -eventid%x",ptemp->event_id);

	SetShortCtoJava( env, pevent, peventCls, "content_nibble_level",(jshort)(ptemp->content_nibble_level));

	if(ptemp->short_epg!=NULL)
	{
		SetCharCtoJava(env, pevent, peventCls,"name_length",(jchar)(ptemp->short_epg->name_length));

		if(ptemp->short_epg->name_length >0)
		{
			if(ptemp->short_epg->event_name!=NULL)
			{
				memcpy( cEvent_Name, ptemp->short_epg->event_name,ptemp->short_epg->name_length);
				cEvent_Name[ 256] = '\0';
				
				jMetidTemp = env->GetMethodID( peventCls, "seteventname", "(Ljava/lang/String;)V");
				//jeventname = stoJstring_GBK( env, (const char*)cEvent_Name);
				jeventname = CodingToJstring( env, (int)cEvent_Name[0], cEvent_Name, ptemp->short_epg->name_length );
				//DEBUG_MSG1(-1,"+++++++++++++++++++++=== NAME [%s]",cEvent_Name);
				env->CallVoidMethod( pevent, jMetidTemp, jeventname );
				
			}
		}
	}

    DeleteLocalRef( env, peventCls );
	DeleteLocalRef( env, jeventname );

	return 0;

}


/*banner条信息取下一个播放event*/
/*serviceid,tsid,ornetowrkid: 要获取事件的频道信息*/
/*fevent :传入型参数，获取的当前事件信息，由jni获取并传递到此实例中*/
/*返回值0;成功；-1:失败*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetFevent
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject fevent)
{

	  jclass peventCls=NULL;
	  jmethodID jMetidTemp=NULL;
	  jstring jeventname=NULL;

	  

	  time_td_t	tStart_Time;
	  time_td_t tend_time;
	  char cEvent_Name[257];
	  int ratevalue =0;

	  EVENT*  ptemp =NULL;
	  
	  peventCls = env->GetObjectClass( fevent );
	  if( NULL == peventCls )
	  {
		  DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		 return -1;
	  }

	  /* 取当前事件*/
	  ptemp =  App_Get_Pf_Event(serviceid,tsid,ornetowrkid,1);
	   if(ptemp == NULL)
	   {
	//	   DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
			DeleteLocalRef( env, peventCls );
			return -1;
	   }

	  /*取当前事件*/
		
	 
	  /*转换时间 格式 */

	  memset(cEvent_Name,0,257);

	App_Get_Event_Start_Time(&tStart_Time,ptemp);
	App_Get_Event_End_Time(  &tend_time,  ptemp);
	  

	  
  	
  	SetShortCtoJava( env, fevent, peventCls, "event_id", (jshort)(ptemp->event_id));
  
 	SetShortCtoJava( env, fevent, peventCls, "start_year",(jshort)(tStart_Time.year));
 	SetShortCtoJava( env, fevent, peventCls, "start_month",(jshort)(tStart_Time.month));
 	SetShortCtoJava( env, fevent, peventCls, "start_date",(jshort)(tStart_Time.date));
  	SetShortCtoJava( env, fevent, peventCls, "start_hour",(jshort)(tStart_Time.hour));
  	SetShortCtoJava( env, fevent, peventCls, "start_minute",(jshort)(tStart_Time.minute));	  
  	SetShortCtoJava( env, fevent, peventCls, "start_second",(jshort)(tStart_Time.second));



	SetShortCtoJava( env, fevent, peventCls, "end_year",(jshort)(tend_time.year));
	SetShortCtoJava( env, fevent, peventCls, "end_month",(jshort)(tend_time.month));
	SetShortCtoJava( env, fevent, peventCls, "end_date",(jshort)(tend_time.date));
	SetShortCtoJava( env, fevent, peventCls,"end_hour",(jshort)(tend_time.hour));
	SetShortCtoJava( env, fevent, peventCls, "end_minute",(jshort)(tend_time.minute)); 	
	SetShortCtoJava( env, fevent, peventCls, "end_second",(jshort)(tend_time.second));


	SetShortCtoJava( env, fevent, peventCls, "content_nibble_level",(jshort)(ptemp->content_nibble_level));
	

	if(ptemp->parental_rating!=NULL)
	{
		ratevalue =ptemp->parental_rating->rate;
	}

	
	SetIntCtoJava( env, fevent, peventCls, "rate",(jint)(ratevalue));

	
//	DEBUG_MSG3(-1,"FFFFF+++++++++++++++++++++=== starttime[%d%d%d]",tStart_Time.year,tStart_Time.month,tStart_Time.date);
	//DEBUG_MSG3(-1,"FFFFFF+++++++++++++++++++++=== endtime[%d%d%d]",tend_time.year,tend_time.month,tend_time.date);

	SetShortCtoJava( env, fevent, peventCls, "content_nibble_level",(jshort)(ptemp->content_nibble_level));

	if(ptemp->short_epg!=NULL)
	{
		SetCharCtoJava(env, fevent, peventCls,"name_length",(jchar)(ptemp->short_epg->name_length));

		if(ptemp->short_epg->name_length >0)
		{
			if(ptemp->short_epg->event_name!=NULL)
			{
				memcpy( cEvent_Name, ptemp->short_epg->event_name,ptemp->short_epg->name_length);
				cEvent_Name[ 256] = '\0';
				
				jMetidTemp = env->GetMethodID( peventCls, "seteventname", "(Ljava/lang/String;)V");
				//jeventname = stoJstring_GBK( env, (const char*)cEvent_Name);
				jeventname = CodingToJstring( env, (int)cEvent_Name[0], cEvent_Name, ptemp->short_epg->name_length );			
				env->CallVoidMethod( fevent, jMetidTemp, jeventname );
				//DEBUG_MSG1(-1,"+++++++++++++++++++++=== NAME [%s]",cEvent_Name);
				
			}
		}


	}

    DeleteLocalRef( env, peventCls );
	DeleteLocalRef( env, jeventname );  
	return 0;


}

/*epg菜单使用，获取某天的某个event信息*/
/*serviceid,tsid,ornetowrkid:频道信息*/
/*eventinfo: 传出型参数，要获取的事件信息，由jni获取并传递到此实例中*/
/*curdate:	  当前天数，从0开始，0是当天，1第二天，依次类推*/
/*utctime	这个参数暂时不用，使用的时候传入空值即可。(  传入型参数，传入当前的系统时间，
		  如果获取不到系统时间 则传递参数全部为0,当前时间以码流时间为准)*/
/*number:	 要获取 的第几个事件，从1开始*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventInfoByIndex
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,int curdate,int number,jobject eventinfo,jobject utctime)
{
	
	jclass peventCls=NULL;
	jmethodID jMetidTemp=NULL;
	jstring jeventname=NULL;
	

	time_td_t tStart_Time;
	time_td_t tend_time;
	char cEvent_Name[257];
	int ratevalue =0;

	EVENT*	ptemp =NULL;
	

//	DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);

	
	peventCls = env->GetObjectClass( eventinfo );
	if( NULL == peventCls )
	{
		DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EVENT;
	}


	if((curdate>6)||(curdate <0))
	{
		DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EVENT;

	}

	
	
	/* 取当前事件*/
	ptemp = App_Get_Event_Info(serviceid,tsid,ornetowrkid,curdate,number);
	if(ptemp == NULL)
	{
	//	DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EVENT;
	}
	
	
	/*取当前事件*/
	  
	
//	DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);
	  memset(cEvent_Name,0,257);

	  App_Get_Event_Start_Time(&tStart_Time,ptemp);
	  App_Get_Event_End_Time(  &tend_time,  ptemp);
	  

//	  DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);
	  
	  SetShortCtoJava( env, eventinfo, peventCls, "event_id", (jshort)(ptemp->event_id));
#if 0
	  DEBUG_MSG(-1,"ZXGUAN===============");	
	  DEBUG_MSG1(-1,"EvenvId %d.",ptemp->event_id);
#endif	  
	  SetShortCtoJava( env, eventinfo, peventCls, "start_year",(jshort)(tStart_Time.year));
	  SetShortCtoJava( env, eventinfo, peventCls, "start_month",(jshort)(tStart_Time.month));
	  SetShortCtoJava( env, eventinfo, peventCls, "start_date",(jshort)(tStart_Time.date));
	  SetShortCtoJava( env, eventinfo, peventCls, "start_hour",(jshort)(tStart_Time.hour));
	  SetShortCtoJava( env, eventinfo, peventCls, "start_minute",(jshort)(tStart_Time.minute));	  
	  SetShortCtoJava( env, eventinfo, peventCls, "start_second",(jshort)(tStart_Time.second));
#if 0
	 DEBUG_MSG6(-1,"%d.%d.%d--%d:%d:%d", \
	 	tStart_Time.year, tStart_Time.month, tStart_Time.date, \
	 	tStart_Time.hour, tStart_Time.minute, tStart_Time.second );
#endif
	SetShortCtoJava( env, eventinfo, peventCls,"end_year",(jshort)(tend_time.year));
	SetShortCtoJava( env, eventinfo, peventCls, "end_month",(jshort)(tend_time.month));
	SetShortCtoJava( env, eventinfo, peventCls, "end_date",(jshort)(tend_time.date));
	SetShortCtoJava( env, eventinfo, peventCls, "end_hour",(jshort)(tend_time.hour));
	SetShortCtoJava( env, eventinfo, peventCls, "end_minute",(jshort)(tend_time.minute)); 	
	SetShortCtoJava( env, eventinfo, peventCls, "end_second",(jshort)(tend_time.second));

	SetShortCtoJava( env, eventinfo, peventCls, "content_nibble_level",(jshort)(((ptemp->content_nibble_level)>>8)&0x00ff));

	SetIntCtoJava( env, eventinfo, peventCls, "content_nibble1", (jint)(ptemp->content_nibble_level >> 12 & 0xF) );
	SetIntCtoJava( env, eventinfo, peventCls, "content_nibble2", (jint) (ptemp->content_nibble_level >> 8 & 0xF) );
	SetIntCtoJava( env, eventinfo, peventCls, "user_nibble", (jint) (ptemp->content_nibble_level & 0xFF) );

	if(ptemp->parental_rating!=NULL)
	{
		ratevalue =ptemp->parental_rating->rate;
	}

	
	SetIntCtoJava( env, eventinfo, peventCls, "rate",(jint)(ratevalue));

	
//	DEBUG_MSG1(-1,"GetEventInfoByIndex+++++++++++++++++++++=%d==",__LINE__);

	if(ptemp->short_epg!=NULL)
	{
		SetCharCtoJava(env, eventinfo, peventCls,"name_length",(jchar)(ptemp->short_epg->name_length));

		if(ptemp->short_epg->name_length >0)
		{
			if(ptemp->short_epg->event_name!=NULL)
			{
				memcpy( cEvent_Name, ptemp->short_epg->event_name,ptemp->short_epg->name_length);
				cEvent_Name[256] = '\0';
				jMetidTemp = env->GetMethodID( peventCls, "seteventname", "(Ljava/lang/String;)V");
				//jeventname = stoJstring_GBK( env, (const char*)cEvent_Name);
				jeventname = CodingToJstring( env, (int)cEvent_Name[0], cEvent_Name, ptemp->short_epg->name_length );
				env->CallVoidMethod( eventinfo, jMetidTemp, jeventname );
				
			}
		}


	}


	//DEBUG_MSG3(-1," schdule+++++++++++++++++++++=== starttime[%d%d%d]",tStart_Time.year,tStart_Time.month,tStart_Time.date);
	//DEBUG_MSG3(-1,"schdule +++++++++++++++++++++=== endtime[%d%d%d]",tend_time.year,tend_time.month,tend_time.date);
//	DEBUG_MSG2(-1,"GetEventInfoByIndex---schdule   OK  LINE %d+++++++++++++++++++++=== name %s",__LINE__,cEvent_Name);


OUT_EVENT:

    DeleteLocalRef( env, peventCls );
	DeleteLocalRef( env, jeventname );
 //  DEBUG_MSG1(-1,"GetEventInfoByIndex +++++++++++++++++++++=%d==",__LINE__);

}

/*java上层通过这个接口当前的系统时间，这个时间是从码流中获取的*/
/*可以通过当前时间和事件的开始结束时间计算得出当前的节目播放进度*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetUtcTime
									(JNIEnv *env, jobject obj,jobject utctime)
{
	
	jclass uepgtimeCls=NULL;


	
	time_td_t tUTC_Time;


	
	uepgtimeCls = env->GetObjectClass( utctime );
	if( NULL == uepgtimeCls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EPGTIME;
	}
	

	tUTC_Time =Get_Local_Td_time();

	
	SetShortCtoJava( env, utctime, uepgtimeCls,"utc_year",(jshort)(tUTC_Time.year));
	SetShortCtoJava( env, utctime, uepgtimeCls, "utc_month",(jshort)(tUTC_Time.month));
	SetShortCtoJava( env, utctime, uepgtimeCls, "utc_date",(jshort)(tUTC_Time.date));
	SetShortCtoJava( env, utctime, uepgtimeCls, "utc_hour",(jshort)(tUTC_Time.hour));
	SetShortCtoJava( env, utctime, uepgtimeCls, "utc_minute",(jshort)(tUTC_Time.minute)); 	
	SetShortCtoJava( env, utctime, uepgtimeCls, "utc_second",(jshort)(tUTC_Time.second));
	
	DEBUG_MSG4(-1,"+++++++++++++++++++++=%d==  %d %d %d \n",__LINE__,tUTC_Time.year,tUTC_Time.month,tUTC_Time.date);
	DEBUG_MSG4(-1,"+++++++++++++++++++++=%d==  %d %d %d \n",__LINE__,tUTC_Time.hour,tUTC_Time.minute,tUTC_Time.second);

OUT_EPGTIME:

	DeleteLocalRef( env, uepgtimeCls );

}

/*获取当前观看节目的级别*/
/*返回值0成功，1失败*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetRate
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject frate)
{

	
	jclass uepgRATECls=NULL;

	char rate =0;

	
	uepgRATECls = env->GetObjectClass( frate );
	if( NULL == uepgRATECls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_RATE;
	}

	rate = App_Get_Service_Rating(serviceid,tsid,ornetowrkid);
	if(rate ==0)
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
	}

	SetIntCtoJava(env,frate,uepgRATECls,"rate",rate);
    

	

OUT_RATE:

	DeleteLocalRef( env, uepgRATECls );
}

JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetOneEventExtentInfo
										(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,
										char flag,char cur_date, short number, jobject data)
{
	
	jclass uinfoCls=NULL;
	
	jmethodID jMetidTemp=NULL;
	jstring jextendinfo=NULL;


	char infobuf[1024] ;
	unsigned short buflen =1024;
	
	uinfoCls = env->GetObjectClass( data );
	if( NULL == uinfoCls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EXINFO;
	}

	memset(infobuf,0,1024);
	
	App_Get_event_Info_For_Dialog(serviceid,tsid,ornetowrkid,0,cur_date,number,(char *)infobuf,&buflen);	
	if(buflen ==0)
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		goto OUT_EXINFO;
	}
	
		
	jMetidTemp = env->GetMethodID( uinfoCls, "setextentinfo", "(Ljava/lang/String;)V");
	jextendinfo = CodingToJstring( env, (int)infobuf[0], infobuf, buflen );
	//jextendinfo = stoJstring_GBK( env, (const char*)infobuf, );
	env->CallVoidMethod( data, jMetidTemp, jextendinfo );


OUT_EXINFO:

	DeleteLocalRef( env, uinfoCls );
}


#define MAX_EVENT_NUM (400)

int  EventlistBuilClass(JNIEnv *env, jobject Objevent, EVENT tIev)
{
	jclass eventCls=NULL;
	jfieldID jFieldTemp=NULL;
	jobject jObjTemp=NULL;
	jclass jClsTemp=NULL;
	
	int ret = TRUE;
	
	time_td_t tStart_Time;
	time_td_t tend_time;
	char 	cEvent_Name[257];

	
	jmethodID jMetidTemp=NULL;
	jstring jeventname=NULL;
	
	int ratevalue =0;
	
	eventCls = env->GetObjectClass( Objevent );
	if( NULL == eventCls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		ret = -1;
		goto OUT_BUILD_CLASS;
	}

	
//	DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
	  memset(cEvent_Name,0,257);

	  App_Get_Event_Start_Time(&tStart_Time,&tIev);
	  App_Get_Event_End_Time(  &tend_time,  &tIev);


	
	  SetShortCtoJava( env, Objevent, eventCls, "event_id", (jshort)(tIev.event_id));
	  
	  SetShortCtoJava( env, Objevent, eventCls, "start_year",(jshort)(tStart_Time.year));
	  SetShortCtoJava( env, Objevent, eventCls, "start_month",(jshort)(tStart_Time.month));
	  SetShortCtoJava( env, Objevent, eventCls, "start_date",(jshort)(tStart_Time.date));
	  SetShortCtoJava( env, Objevent, eventCls, "start_hour",(jshort)(tStart_Time.hour));
	  SetShortCtoJava( env, Objevent, eventCls, "start_minute",(jshort)(tStart_Time.minute));	  
	  SetShortCtoJava( env, Objevent, eventCls, "start_second",(jshort)(tStart_Time.second));



	SetShortCtoJava( env, Objevent, eventCls,"end_year",(jshort)(tend_time.year));
	SetShortCtoJava( env, Objevent, eventCls, "end_month",(jshort)(tend_time.month));
	SetShortCtoJava( env, Objevent, eventCls, "end_date",(jshort)(tend_time.date));
	SetShortCtoJava( env, Objevent, eventCls, "end_hour",(jshort)(tend_time.hour));
	SetShortCtoJava( env, Objevent, eventCls, "end_minute",(jshort)(tend_time.minute)); 	
	SetShortCtoJava( env, Objevent, eventCls, "end_second",(jshort)(tend_time.second));


	 SetShortCtoJava( env, Objevent, eventCls, "content_nibble_level", (jshort)(tIev.content_nibble_level));
	
	if(tIev.parental_rating!=NULL)
	{
		ratevalue =tIev.parental_rating->rate;
	}

	
	SetIntCtoJava( env, Objevent, eventCls, "rate",(jint)(ratevalue));
	
//	DEBUG_MSG1(-1, "EventId %d ", tIev.event_id);
//	DEBUG_MSG4(-1,"LINE %d ++++ +++++++++++++++++=== starttime[%d%d%d]",__LINE__,tStart_Time.year,tStart_Time.month,tStart_Time.date);
//	DEBUG_MSG4(-1,"LINE %d +++++++++++++++++++++=== endtime[%d%d%d]",__LINE__,tend_time.year,tend_time.month,tend_time.date);

	if(tIev.short_epg!=NULL)
	{
		SetCharCtoJava(env, Objevent, eventCls,"name_length",(jchar)(tIev.short_epg->name_length));

		if(tIev.short_epg->name_length >0)
		{
			if(tIev.short_epg->event_name!=NULL)
			{
				memcpy( cEvent_Name, tIev.short_epg->event_name,tIev.short_epg->name_length);
				cEvent_Name[ 256] = '\0';
				
				jMetidTemp = env->GetMethodID( eventCls, "seteventname", "(Ljava/lang/String;)V");
			//	jeventname = stoJstring_GBK( env, (const char*)cEvent_Name);
				jeventname = CodingToJstring( env, (int)cEvent_Name[0], cEvent_Name, tIev.short_epg->name_length );
				env->CallVoidMethod( Objevent, jMetidTemp, jeventname );
				
			}
		}
	}
	ret =0;

	
	OUT_BUILD_CLASS:

	DeleteLocalRef( env, eventCls );
	
	return ret;
}


/*EPG 菜单使用，获取某页的epg事件列表*/
/*serviceid,tsid,ornetowrkid:频道信息*/
/*number:	 要获取 事件列表的第几个开始*/
/*one_page_max :一张页面显示几个事件，暂时不使用，先保留*/
/*peventlist: 当前频道的epg 事件信息，包括实际总数以及相应的 事件信息*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventListInfo
										(JNIEnv *env, jobject obj, short serv_id,short ts_id,short on_id,
										int cur_date, short number, short one_page_max,jobject eventlist)
{
	
	jclass ueventCls=NULL;
	jclass utotalCls=NULL;

	
	jmethodID jMetidTemp=NULL;
	jmethodID jMetidTotal=NULL;
	
	jmethodID jMetid=NULL;
	jstring jeventname=NULL;

	jobjectArray  	jArrList=NULL;
	jfieldID 	jFidTemp=NULL;
	jobject 	jObjListOne=NULL;

	
	EVENT	plist[MAX_EVENT_NUM] ;
	int ii,ret =0;
	int uiTemp = 0;
	unsigned short total;

	jsize size1;
	
	//DEBUG_MSG1(-1,"LIST +++++++++++++++++++++=%d==",__LINE__);
	
	ueventCls = env->GetObjectClass( eventlist );
	if( NULL == ueventCls )
	{
		//DEBUG_MSG1(-1,"ERROR +++++++++++++++++++++=%d==",__LINE__);
		goto OUT_LIST_RET;
	}	



	memset(plist,0,sizeof(EVENT)*MAX_EVENT_NUM );
	
	ret = App_Get_Event_List_Info(serv_id,ts_id,on_id,cur_date,number,MAX_EVENT_NUM,plist,&total);
	if(ret !=0)
	{
		//DEBUG_MSG1(-1,"ERROR +++++++++++++++++++++=%d==",__LINE__);
		goto OUT_LIST_RET;
	}
	
	jMetid = env->GetMethodID( ueventCls, "BulidArrPointer", "(SLcom/pbi/dvb/dvbinterface/NativeEpg$EPGOnePage;)V");
	if( NULL == jMetid )
	{
		goto OUT_LIST_RET;
	}
	env->CallVoidMethod( eventlist, jMetid,total, eventlist );
	//env->DeleteLocalRef( (jobject)jMetid);
	
	
	jMetid = env->GetMethodID( ueventCls, "BulidEpglist", "(SLcom/pbi/dvb/dvbinterface/NativeEpg$EPGOnePage;)V");
	if( NULL == jMetid )
	{
		goto OUT_LIST_RET;
	}
	env->CallVoidMethod( eventlist, jMetid,total, eventlist );
	//env->DeleteLocalRef( (jobject)jMetid);

	jFidTemp = env->GetFieldID( ueventCls,"epgList","[Lcom/pbi/dvb/dvbinterface/NativeEpg$EpgEventInfo;");
	jArrList = (jobjectArray)env->GetObjectField( eventlist, jFidTemp );
	if( NULL == jArrList )
	{
		DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);
		goto OUT_LIST_RET;
	}

	
	
	//DEBUG_MSG2(-1,"LIST +++++++++++++++++++++=%d== totalnum %d   \n",__LINE__,total);

	SetShortCtoJava( env, eventlist, ueventCls,"totalnum",(jshort)total);

	
	
	for( ii = 0; (ii<MAX_EVENT_NUM)&&(ii < total); ii++ )
	{
		//DEBUG_MSG4( -1, "ArrList is ===%d== ii %d  number %d  one_page_max %d \n",__LINE__,ii,number,one_page_max);
		jObjListOne = env->GetObjectArrayElement( jArrList, ii );
		if( NULL == jObjListOne )
		{
			DEBUG_MSG1( 1,"ERROR =======%d============= \n",__LINE__);
			goto OUT_LIST_RET;
		}

		
		
		DEBUG_MSG2( 1," event id %x   =======nibble%x============= \n",plist[ii].event_id,plist[ii].content_nibble_level);
		
		//DEBUG_MSG1( 1," uiTemp=======%d============= \n",ii);
		
		ret = EventlistBuilClass( env, jObjListOne,plist[ii] );
		if( 0 != ret )  
		{   
			DEBUG_MSG1( 1,"ERROR =======%d============= \n",__LINE__);
			goto OUT_LIST_RET;
		}
		DeleteLocalRef( env, jObjListOne );
	}
	
OUT_LIST_RET:

	DeleteLocalRef( env, ueventCls );
}



JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventListInfoByNibble
										(JNIEnv *env, jobject obj, short serv_id,short ts_id,short on_id,
										int cur_date, short number, short one_page_max,jobject eventlist,short nibble)
{
	
	jclass ueventCls=NULL;
	jclass utotalCls=NULL;

	
	jmethodID jMetidTemp=NULL;
	jmethodID jMetidTotal=NULL;
	
	jmethodID jMetid=NULL;
	jstring jeventname=NULL;

	jobjectArray  	jArrList=NULL;
	jfieldID 	jFidTemp=NULL;
	jobject 	jObjListOne=NULL;

	
	EVENT	plist[MAX_EVENT_NUM] ;
	int ii,ret =0;
	int uiTemp = 0;
	unsigned short total;

	jsize size1;
	
	//DEBUG_MSG1(-1,"LIST +++++++++++++++++++++=%d==",__LINE__);
	
	ueventCls = env->GetObjectClass( eventlist );
	if( NULL == ueventCls )
	{
		//DEBUG_MSG1(-1,"ERROR +++++++++++++++++++++=%d==",__LINE__);
		goto OUT_LIST_RET;
	}	



	memset(plist,0,sizeof(EVENT)*MAX_EVENT_NUM );
	
	ret = App_Get_Event_List_InfoByNibble(serv_id,ts_id,on_id,cur_date,number,MAX_EVENT_NUM,plist,&total,nibble);
	if(ret !=0)
	{
		//DEBUG_MSG1(-1,"ERROR +++++++++++++++++++++=%d==",__LINE__);
		goto OUT_LIST_RET;
	}
	
	jMetid = env->GetMethodID( ueventCls, "BulidArrPointer", "(SLcom/pbi/dvb/dvbinterface/NativeEpg$EPGOnePage;)V");
	if( NULL == jMetid )
	{
		goto OUT_LIST_RET;
	}
	env->CallVoidMethod( eventlist, jMetid,total, eventlist );
	//env->DeleteLocalRef( (jobject)jMetid);
	
	
	jMetid = env->GetMethodID( ueventCls, "BulidEpglist", "(SLcom/pbi/dvb/dvbinterface/NativeEpg$EPGOnePage;)V");
	if( NULL == jMetid )
	{
		goto OUT_LIST_RET;
	}
	env->CallVoidMethod( eventlist, jMetid,total, eventlist );
	//env->DeleteLocalRef( (jobject)jMetid);

	jFidTemp = env->GetFieldID( ueventCls,"epgList","[Lcom/pbi/dvb/dvbinterface/NativeEpg$EpgEventInfo;");
	jArrList = (jobjectArray)env->GetObjectField( eventlist, jFidTemp );
	if( NULL == jArrList )
	{
		DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);
		goto OUT_LIST_RET;
	}

	
	
	//DEBUG_MSG2(-1,"LIST +++++++++++++++++++++=%d== totalnum %d   \n",__LINE__,total);

	SetShortCtoJava( env, eventlist, ueventCls,"totalnum",(jshort)total);

	
	
	for( ii = 0; (ii<MAX_EVENT_NUM)&&(ii < total); ii++ )
	{
		//DEBUG_MSG4( -1, "ArrList is ===%d== ii %d  number %d  one_page_max %d \n",__LINE__,ii,number,one_page_max);
		jObjListOne = env->GetObjectArrayElement( jArrList, ii );
		if( NULL == jObjListOne )
		{
			DEBUG_MSG1( 1,"ERROR =======%d============= \n",__LINE__);
			goto OUT_LIST_RET;
		}


		
		DEBUG_MSG2( 1," event id %x   =======nibble%x============= \n",plist[ii].event_id,plist[ii].content_nibble_level);
		
		//DEBUG_MSG1( 1," uiTemp=======%d============= \n",ii);
		
		ret = EventlistBuilClass( env, jObjListOne,plist[ii] );
		if( 0 != ret )
		{
			DEBUG_MSG1( 1,"ERROR =======%d============= \n",__LINE__);
			goto OUT_LIST_RET;
		}
		DeleteLocalRef( env, jObjListOne );
	}
	
OUT_LIST_RET:

	DeleteLocalRef( env, ueventCls );




}


