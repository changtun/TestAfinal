/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>

#ifndef _Included_com_pbi_dvb_dvbinterface_NativeEPG
#define _Included_com_pbi_dvb_dvbinterface_NativeEPG
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_EpgMessageInit
(JNIEnv *env, jobject obj, jobject MsgObj);


/*banner条信息取当前播放event*/   
/*serviceid,tsid,ornetowrkid: 要获取事件的频道信息*/
/*fevent :传入型参数，获取的下一个事件信息，由jni获取并传递到此实例中*/  
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetPevent
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject pevent);




/*banner条信息取下一个播放event*/
/*serviceid,tsid,ornetowrkid: 要获取事件的频道信息*/
/*fevent :传入型参数，获取的当前事件信息，由jni获取并传递到此实例中*/	
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetFevent
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject fevent);



/*epg菜单使用，获取某天的某个event信息*/
/*serviceid,tsid,ornetowrkid:频道信息*/
/*eventinfo: 传出型参数，要获取的事件信息，由jni获取并传递到此实例中*/
/*curdate:	  当前天数，从0开始，0是当天，1第二天，依次类推*/
/*utctime	  传入型参数，传入当前的系统时间，
		  如果获取不到系统时间 则传递参数全部为0,当前时间以码流时间为准*/
/*number:	 要获取 的第几个事件*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventInfoByIndex
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,int curdate,int number,jobject eventinfo,jobject utctime);


/*获取当前观看节目的级别*/	
/*返回值0成功，1失败*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetRate
	(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,jobject frate);

JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetUtcTime
									(JNIEnv *env, jobject obj,jobject utctime);

JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetOneEventExtentInfo
										(JNIEnv *env, jobject obj, short serviceid,short tsid,short ornetowrkid,
										char flag,char cur_date, short number, jobject data);

/*EPG 菜单使用，获取某页的epg事件列表*/
/*serviceid,tsid,ornetowrkid:频道信息*/ 
/*number:	 要获取 事件列表的第几个开始*/
/*one_page_max :一张页面显示几个事件*/
/*total_number: 当前频道的epg 列表的总数 */
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventListInfo
										(JNIEnv *env, jobject obj, short serv_id,short ts_id,short on_id,
										int cur_date, short number, short one_page_max,jobject eventlist);

JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_GetEventListInfoByNibble
										(JNIEnv *env, jobject obj, short serv_id,short ts_id,short on_id,
										int cur_date, short number, short one_page_max,jobject eventlist,short nibble);


#ifdef __cplusplus
}
#endif
#endif


  
