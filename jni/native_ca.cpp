#include <stdio.h>
#include <jni.h>
#include <ca.h>

#include "logit.h"
#include "jnistaple.h"
#include "pbi_coding.h"
#include "native_ca.h"
#include "ctisys.h"
#include "ctisc.h"
#include "ca.h"


#define MAX_IPP_PROUDCT_NUM   (20)
#define MAX_IPP_RECORD_NUM   (100)
#define MAX_ENTITLE_NUM    (320)
#define MAX_WALLET_NUM     (1)
#define MAX_ONE_ENTITLE_PAGE   (32)
#if 0
typedef struct entitle_s
{
	
	CAS_TIMESTAMP  startime;//[320];
	CAS_TIMESTAMP  endtime;//[320];
	unsigned int remain;
	unsigned short index;
	int  estatus;
	/* Return value, 0: Valid entitle; 1: Input parameters error; 2: No entitle info;
						 3: Valid entitle, but limited; 4: Entitle closed; 5: Entitle paused;  */
	
}
trcas_entitl_t;
#endif

void pinCodeToArrty(unsigned char * arrty, int pinCode, int pinLen)
{
	if(arrty == NULL || pinLen != 4)
		return;
	arrty[0] = (pinCode%10000)/1000;
	arrty[1] = (pinCode%1000)/100;
	arrty[2] = (pinCode%100)/10;
	arrty[3] = pinCode%10;
}

JNIEXPORT jint  JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_SendMessageToCA
(JNIEnv *env, jobject obj, jint type,jobject objData, jint arg1)
{
	void *param = NULL;
	jclass jObjCls =NULL;
	CTI_PinCode pinCode; 
	CTI_IPPV IPPV;
	CTI_PPID_Info PPID_Info;
	CTI_PPV_List PPV_List;
	CTI_Parental Parental;
	CTI_UserView UserView;
	unsigned char PinCode[4];
	int temp = 0;
	switch(type)
	{
		case CA_CLIENT_CHANGE_PIN_QUERY:
		case CA_CLIENT_CHECK_PIN_QUERY:
		{			
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "oldCode");
			pinCodeToArrty(pinCode.OLD_PinCode, temp, 4);
			temp = GetIntJavaToC(env, objData, jObjCls, "newCode");			
			pinCodeToArrty(pinCode.NEW_PinCode, temp, 4);
			param = &pinCode;
		}
		break;		
		case CA_CLIENT_IPPV_QUERY:
		{
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "iFLag");
			IPPV.IPPV_FLag = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iProgramNo");
			IPPV.IPPV_ProgramNo = temp;
			temp = GetIntJavaToC(env, objData, jObjCls, "pinCode");
			pinCodeToArrty(IPPV.PinCode, temp, 4);
			param = &IPPV;
		}
		break;
		case CA_CLIENT_PPID_INFO_QUERY:
		{
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "iPPID");
			PPID_Info.PPID = temp;
			temp = GetIntJavaToC(env, objData, jObjCls, "iPinCode");
			pinCodeToArrty(PPID_Info.PinCode, temp, 4);
			param = &PPID_Info;
		}
		break;
		case CA_CLIENT_PPV_LIST_INFO_QUERY:
		{
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "iPPID");
			PPV_List.PPID_ID = temp;
			temp = GetIntJavaToC(env, objData, jObjCls, "iType");
			PPV_List.List_Type = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iPage");
			PPV_List.List_Page = temp & 0xFF;
			param = &PPV_List;
		}
		break;
		case CA_CLIENT_SET_PARENTAL_QUERY:
		{
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "iParentalRate");
			Parental.Parental_rate = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iPinCode");
			pinCodeToArrty(Parental.PinCode, temp, 4);
			param = &Parental;
		}
		break;
		case CA_CLIENT_USER_VIEW_QUERY:
		{
			jObjCls = env->GetObjectClass( objData );
			if( NULL == jObjCls )
			{
				DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
				return -1;
			}
			temp = GetIntJavaToC(env, objData, jObjCls, "iType");
			UserView.type = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iPinCode");
			pinCodeToArrty(UserView.PinCode, temp, 4);
			temp = GetIntJavaToC(env, objData, jObjCls, "iFLag");
			UserView.flag = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStatus");
			UserView.status = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartCh");
			UserView.control_start_channel[0] = (temp>>8) & 0xFF;
			UserView.control_start_channel[1] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndCh");
			UserView.control_end_channel[0] = (temp>>8) & 0xFF;
			UserView.control_end_channel[1] = temp & 0xFF;

			temp = GetIntJavaToC(env, objData, jObjCls, "iStartYear");
			UserView.control_start_time[0] = (temp>>8) & 0xFF;
			UserView.control_start_time[1] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartMonth");
			UserView.control_start_time[2] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartDay");
			UserView.control_start_time[3] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartHour");
			UserView.control_start_time[4] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartMinute");
			UserView.control_start_time[5] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iStartSecond");
			UserView.control_start_time[6] = temp & 0xFF;
			UserView.control_start_time[7] = 0;

			temp = GetIntJavaToC(env, objData, jObjCls, "iEndYear");
			UserView.control_end_time[0] = (temp>>8) & 0xFF;
			UserView.control_end_time[1] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndMonth");
			UserView.control_end_time[2] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndDay");
			UserView.control_end_time[3] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndHour");
			UserView.control_end_time[4] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndMinute");
			UserView.control_end_time[5] = temp & 0xFF;
			temp = GetIntJavaToC(env, objData, jObjCls, "iEndSecond");
			UserView.control_end_time[6] = temp & 0xFF;
			UserView.control_end_time[7] = 0;
			param = &UserView;
		}
		break;
		case CA_CLIENT_PARENTAL_CHANGE_QUERY:
		{			
			pinCodeToArrty(PinCode, arg1, 4);
			param = &PinCode;
		}
		break;
		case CA_CLIENT_ANTI_RECORD_QUERY:
		case CA_CLIENT_PPID_QUERY:
		case CA_CLIENT_SC_INFO_QUERY:
		{
			param = NULL;
		}
		break;
	}
	CTICA_QueryControl(type, param,(int)arg1);
	return 0;
}


/*CA ��Ϣ�˵�*/
/*����ֵ:-1,ȡֵ���󣬿��Զ�����ʾ*/
/*����ֵ:-2,û�в������ܿ�������ca��汾�ſ��Զ���*/
JNIEXPORT jint  JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetCaData
(JNIEnv *env, jobject obj, jobject objData)
{
	jclass jObjCls =NULL;
	jfieldID jFidTemp =NULL;
	jmethodID jMetidTemp =NULL;
	jstring jstringtemp1 =NULL,jstringtemp2 =NULL,jstring3 =NULL;

    
	int  ret = 0;
	char scnum[40];
	char scver[40];
	unsigned char  *mcellver =NULL;
	unsigned char  rate; 
#if 0
	CAS_SCARD_STATUS state;

	DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);

	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}
	
	jObjCls = env->GetObjectClass( objData );
	if( NULL == jObjCls )
	{
		DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		return -1;
	}


	
	mcellver = MC_GetRevisionString();	
	if(mcellver!=NULL)
	{
		
		jMetidTemp = env->GetMethodID( jObjCls, "setCAcellVersion", "(Ljava/lang/String;)V");
		//jstring3 = stoJstring_GBK( env, (const char*)mcellver);
		jstring3 = CodingToJstring( env, 0x13, (char*)mcellver, strlen((const char*)mcellver));
		env->CallVoidMethod( objData, jMetidTemp, jstring3 );

		DeleteLocalRef(env, jstring3);
	}
	
	
	memset(scnum,0,40);
	memset(scver,0,40);
	
	ret = MC_GetScSerialNumber(scnum,40);
	if(ret !=0)
	{
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		DeleteLocalRef( env, jObjCls );		
		return -1;
	}

	jMetidTemp = env->GetMethodID( jObjCls, "setSCnumber", "(Ljava/lang/String;)V");
	//jstringtemp1 = stoJstring_GBK( env, (const char*)scnum);
	jstringtemp1 = CodingToJstring( env, 0x13, (char*)scnum, strlen(scnum));
	env->CallVoidMethod( objData, jMetidTemp, jstringtemp1 );

	DeleteLocalRef(env, jstringtemp1);

	ret = MC_GetSmartCardVersion(scver,40);
	if(ret !=0)
	{
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		DeleteLocalRef( env, jObjCls );
		return -1;
	}
	
	jMetidTemp = env->GetMethodID( jObjCls, "setSCversion", "(Ljava/lang/String;)V");
	//jstringtemp2 = stoJstring_GBK( env, (const char*)scver);
	jstringtemp2 = CodingToJstring( env, 0x13, (char*)scver, strlen(scver));
	env->CallVoidMethod( objData, jMetidTemp, jstringtemp2 );

	DeleteLocalRef(env, jstringtemp2);
	
	ret = MC_SCARD_GetRating(&rate);
	if(ret !=0)
	{
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);

		DeleteLocalRef( env, jObjCls );		
		return -1;
	}
	SetIntCtoJava( env, objData, jObjCls, "rate",rate);

	DEBUG_MSG4(-1,"scnum[%s] \n  scver[%s] \n  cellver[0x%x] \n rate[%d] ",scnum,scver,mcellver,rate);
#endif
	return 0;	
		
		
}

/*IPP ��Ѽ�¼����ӦIPPV��IPPT*/
/*index: 0~99*/
/*����ֵ:-1,ȡֵ���󣬶����ʧ��*/
/*����ֵ:-2,û�в������ܿ����޷���ȡ���*/
/*����ֵ:0 �ɹ�*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetIPPRecord
(JNIEnv *env, jobject obj, jobject objData)
{
	
	jclass 			jObjCls =NULL,jlistcls=NULL;
	jfieldID 		jFidTemp=NULL;
	jmethodID 		jMetidTemp=NULL;
	jobjectArray  	jArrList=NULL;
	jobject 		jObjListOne=NULL;	
	
	jstring 		jstring1=NULL;
#if 0	
	CAS_SCARD_STATUS state;

	int  ret = 0;	
	int  index =0,ii;
	unsigned short ulExchbYear =0; 
	CAS_IPPRECORD_STRUCT psRecordData[MAX_IPP_RECORD_NUM];
	
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== index %d",__FUNCTION__,__LINE__,index);

	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}
	

	
	jObjCls = env->GetObjectClass( objData );
	if( NULL == jObjCls )
	{
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		return -1;
	}

	memset(psRecordData,0,sizeof(CAS_IPPRECORD_STRUCT)*MAX_IPP_RECORD_NUM);
	
	for(index =0; index<MAX_IPP_RECORD_NUM; index++)
	{
		ret =MC_GetIppRecord(&psRecordData[index],index);
		if(ret!=0)
		{
			DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
			break;
		}
	}
	
	if(index >0)
	{
		
        ret =0;
	//	DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		jMetidTemp = env->GetMethodID( jObjCls, "BulidArrP", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppRecordList;)V");
		env->CallVoidMethod( objData, jMetidTemp,index, objData );
		//env->DeleteLocalRef( (jobject)jMetidTemp);
		
	//	DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		jMetidTemp = env->GetMethodID( jObjCls, "BulidRlist", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppRecordList;)V");
		env->CallVoidMethod( objData, jMetidTemp,index, objData );
		//env->DeleteLocalRef( (jobject)jMetidTemp);
	//	DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);

		

		
		jFidTemp = env->GetFieldID( jObjCls,"RList","[Lcom/pbi/dvb/dvbinterface/NativeCA$caIPPRecord;");
		jArrList = (jobjectArray)env->GetObjectField( objData, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);

			DeleteLocalRef( env, jObjCls );
			return -1;
		}
		
		
		for(ii =0;ii<index; ii++)
		{
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);

				DeleteLocalRef( env, jObjCls );
				return -1;
			}

			
			jlistcls =env->GetObjectClass( jObjListOne );
			if( NULL == jlistcls )
			{
				DeleteLocalRef( env, jObjCls );
				DeleteLocalRef( env, jObjListOne );
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
				return -1;
			}
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "bStateFlag",psRecordData[ii].bStateFlag);
			
			ulExchbYear = (psRecordData[ii].ulExchTime.bYear[0])*100 + psRecordData[ii].ulExchTime.bYear[1];
			SetShortCtoJava(env, jObjListOne, jlistcls,"ulExchbYear",ulExchbYear);
			
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "ulExchbMonth",psRecordData[ii].ulExchTime.bMonth);
			SetCharCtoJava( env, jObjListOne, jlistcls, "ulExchbDay",psRecordData[ii].ulExchTime.bDay);
			SetCharCtoJava( env, jObjListOne, jlistcls, "ulExchbHour",psRecordData[ii].ulExchTime.bHour);
			SetCharCtoJava( env, jObjListOne, jlistcls, "ulExchbMinute",psRecordData[ii].ulExchTime.bMinute);
			SetCharCtoJava( env, jObjListOne, jlistcls, "ulExchbSecond",psRecordData[ii].ulExchTime.bSecond);
			
			
			SetIntCtoJava( env, jObjListOne, jlistcls, "ulExchRunningNum",psRecordData[ii].ulExchRunningNum);
			SetIntCtoJava( env, jObjListOne, jlistcls, "ulExchValue",psRecordData[ii].ulExchValue);
			SetCharCtoJava( env, jObjListOne, jlistcls, "bContentLen",psRecordData[ii].bContentLen);
			
			
			
			jMetidTemp = env->GetMethodID( jlistcls, "setpbContent", "(Ljava/lang/String;)V");
			//jstring1 = stoJstring_GBK( env, (const char*)psRecordData[ii].pbContent);
			jstring1 = CodingToJstring( env, /*(int)psRecordData[ii].pbContent[0]*/0x13, (char*)psRecordData[ii].pbContent, psRecordData[ii].bContentLen);
			env->CallVoidMethod( jObjListOne, jMetidTemp, jstring1 );
			
			
			DEBUG_MSG3(-1,"runningnuber [%x] value[%x]  ---------- StateFlag%d",psRecordData[ii].ulExchRunningNum,
						psRecordData[ii].ulExchValue,
						psRecordData[ii].bStateFlag);

			DeleteLocalRef( env, jlistcls );
			DeleteLocalRef( env, jObjListOne );
			DeleteLocalRef( env, jstring1 );
		}		
		
		SetShortCtoJava( env, objData, jObjCls,"totalnum",(jshort)index);

		DeleteLocalRef( env, jObjCls );
		return 0;

	}
	else
	{
		DeleteLocalRef( env, jObjCls );
		return -1;
	}
#endif	
return 0;
}



/*IPP ��Ȩ��Ϣ*/
/*index: 0~19*/
/*����ֵ:-1,ȡֵ���󣬶����ʧ��*/
/*����ֵ:-2,û�в������ܿ����޷���ȡ���*/
/*����ֵ:0 �ɹ�*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetIPPProduct
(JNIEnv *env, jobject obj, jobject objData)
{
	
	jclass jObjCls=NULL,jlistcls=NULL;
	jfieldID jFidTemp=NULL;
	jmethodID jMetid=NULL;
	jobjectArray  	jArrList=NULL;
	jobject 	jObjListOne=NULL;

	
	int	 ret = 0;
#if 0	
	int  index =0,ii;
	unsigned short ubYear =0; 
	CAS_IPPPRODUCT_STRUCT psProduct[MAX_IPP_PROUDCT_NUM];
	
	CAS_SCARD_STATUS state;
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== index %d",__FUNCTION__,__LINE__,index);

	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	
	jObjCls =env->GetObjectClass( objData );
	if( NULL == jObjCls )
	{		
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}

	memset(psProduct,0,sizeof(CAS_IPPPRODUCT_STRUCT)*MAX_IPP_PROUDCT_NUM);
	
	for(index =0; index<MAX_IPP_PROUDCT_NUM; index++)
	{
	
		ret =MC_GetIppProduct(&psProduct[index],index);
		if(ret!=0)
		{
			DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==RET %d",__FUNCTION__,__LINE__,ret);
			break;
		}		
		DEBUG_MSG3(-1,"index %d=======type[%d] ulRunningNum[%d]+++++++++++++++++++++",index,psProduct[index].bProductType,psProduct[index].ulRunningNum);
	}

	if(index >0)
	{
		
        ret =0;
		
		jMetid = env->GetMethodID( jObjCls, "BulidArrIPP", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppProductList;)V");
		env->CallVoidMethod( objData, jMetid,index, objData );
		//env->DeleteLocalRef( (jobject)jMetid);
		
		
		jMetid = env->GetMethodID( jObjCls, "BulidIPPlist", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppProductList;)V");
		env->CallVoidMethod( objData, jMetid,index, objData );
		//env->DeleteLocalRef( (jobject)jMetid);

		

		
		jFidTemp = env->GetFieldID( jObjCls,"ProList","[Lcom/pbi/dvb/dvbinterface/NativeCA$caIppProduct;");
		DEBUG_MSG1( -1,"okokok =======%d============= \n",__LINE__);               
		jArrList = (jobjectArray)env->GetObjectField( objData, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);

			DeleteLocalRef( env, jObjCls );
			return -1;
		}

		for(ii =0;ii<index; ii++)
		{

			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);

				DeleteLocalRef( env, jObjCls );
				return -1;
			}

			
			jlistcls =env->GetObjectClass( jObjListOne );
			if( NULL == jlistcls )
			{
				DeleteLocalRef( env, jObjListOne );
				DeleteLocalRef( env, jObjCls );
				return -1;
			}

			SetCharCtoJava( env, jObjListOne, jlistcls, "prodcuttype",psProduct[ii].bProductType);	
			SetIntCtoJava( env, jObjListOne, jlistcls, "ulRunningNum",psProduct[ii].ulRunningNum);
			
			ubYear = (psProduct[ii].ulStartTime.bYear[0]*100)+psProduct[ii].ulStartTime.bYear[1];
			SetShortCtoJava( env, jObjListOne, jlistcls, "startYear",ubYear);
			
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMonth",psProduct[ii].ulStartTime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startDay",psProduct[ii].ulStartTime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startHour",psProduct[ii].ulStartTime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMinute",psProduct[ii].ulStartTime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startSecond",psProduct[ii].ulStartTime.bSecond);
		
		
			ubYear = (psProduct[ii].ulEndTime.bYear[0]*100)+psProduct[ii].ulEndTime.bYear[1];	
			SetShortCtoJava( env, jObjListOne, jlistcls, "endYear",ubYear);
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMonth",psProduct[ii].ulEndTime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endDay",psProduct[ii].ulEndTime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endHour",psProduct[ii].ulEndTime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMinute",psProduct[ii].ulEndTime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endSecond",psProduct[ii].ulEndTime.bSecond);	

			DeleteLocalRef( env, jlistcls );
			DeleteLocalRef( env, jObjListOne );
			DEBUG_MSG1( -1,"okokok =======%d============= \n",__LINE__);
		}
		SetShortCtoJava( env, objData, jObjCls,"totalnum",(jshort)index);

		return 0;
		
	}
	else
	{
		DeleteLocalRef( env, jObjCls );		
		return -1;
	}
	
#endif
return 0;

}


/*IPP ��Ȩ��Ϣ*/
/*INDEX: 0~319*/
/*����ֵ:-1,ȡֵ���󣬶����ʧ��*/
/*����ֵ:-2,û�в������ܿ����޷���ȡ���*/
/*����ֵ:0 �ɹ�*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetEntitle
(JNIEnv *env, jobject obj, jobject objdata)
{
	
	jclass 		jObjCls=NULL,jlistcls=NULL;
	jfieldID 	jFidTemp=NULL;
	jmethodID 	jMetid=NULL;	
	jstring 	jstring1=NULL,jstring2=NULL;
	
	jobjectArray  	jArrList=NULL;
	jobject 		jObjListOne=NULL;

#if 0	
	int	 ret = 0,index = 0,ii = 0;
	trcas_entitl_t entitle[MAX_ENTITLE_NUM];
	unsigned short ulYear =0; 
	CAS_SCARD_STATUS state;

	
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== index %d",__FUNCTION__,__LINE__,index);	
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	
	
	jObjCls =env->GetObjectClass( objdata );
	if( NULL == jObjCls )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}
	
	memset(entitle,0,MAX_ENTITLE_NUM*sizeof(trcas_entitl_t));
	
	for(index =0; index<320; index++)
	{
		entitle[index].index =index;
		
		ret = MC_TIME_GetEntitleInfo((unsigned char *)(&(entitle[index].startime)),(unsigned char *)(&(entitle[index].endtime)),&(entitle[index].remain),index);
		if((ret==1)||(ret==2))
		{
			DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
			//break;
		}
		entitle[index].estatus =ret;	
		DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret",__FUNCTION__,__LINE__,ret);
		
	}

	
	if(index>0)
	{
		ret = 0;
		
		jMetid = env->GetMethodID( jObjCls, "BulidArrE", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEntitleList;)V");
		env->CallVoidMethod( objdata, jMetid,index, objdata );
		
		jMetid = env->GetMethodID( jObjCls, "BulidElist", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEntitleList;)V");
		env->CallVoidMethod( objdata, jMetid,index, objdata );
		
		jFidTemp = env->GetFieldID( jObjCls,"EList","[Lcom/pbi/dvb/dvbinterface/NativeCA$caEntitle;");
		jArrList = (jobjectArray)env->GetObjectField( objdata, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);

			DeleteLocalRef( env, jObjCls );
			return -1;
		}

		DEBUG_MSG1( -1, "entitle total ===%d==",index);
		for(ii =0;ii<index; ii++)
		{		
		
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);				
				DeleteLocalRef( env, jObjCls );
				return -1;
			}
		
			jlistcls =env->GetObjectClass( jObjListOne );
			if( NULL == jlistcls )
			{

				DeleteLocalRef( env, jObjCls );
				DeleteLocalRef( env, jObjListOne );				
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
				return -1;
			}

			ulYear =(entitle[ii].startime.bYear[0])*100 +entitle[ii].startime.bYear[1];
			SetShortCtoJava( env, jObjListOne, jlistcls, "startYear",ulYear);	
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMonth",entitle[ii].startime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startDay",entitle[ii].startime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startHour",entitle[ii].startime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMinute",entitle[ii].startime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startSecond",entitle[ii].startime.bSecond);
		
		//	DEBUG_MSG3( 1,"TEST  %d=======%d===c:-%c-========== \n",__LINE__,entitle[ii].startime.bMonth,entitle[ii].startime.bMonth);
			ulYear = (entitle[ii].endtime.bYear[0]*100)+entitle[ii].endtime.bYear[1];	
			SetShortCtoJava( env, jObjListOne, jlistcls, "endYear",ulYear);
			
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMonth",entitle[ii].endtime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endDay",entitle[ii].endtime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endHour",entitle[ii].endtime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMinute",entitle[ii].endtime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endSecond",entitle[ii].endtime.bSecond);	

			SetIntCtoJava( env, jObjListOne, jlistcls, "pulRemain",entitle[ii].remain);	
			SetIntCtoJava( env, jObjListOne, jlistcls, "index",entitle[ii].index);
			SetIntCtoJava( env, jObjListOne, jlistcls, "status",entitle[ii].estatus);

			DeleteLocalRef( env, jlistcls );
			DeleteLocalRef( env, jObjListOne );
			DeleteLocalRef( env, jstring2 );
			DeleteLocalRef( env, jstring1 );
		}
		SetShortCtoJava( env, objdata, jObjCls,"totalnum",(jshort)index);

		DeleteLocalRef( env, jObjCls );
		return 0;
		
	}
	else
	{
		DeleteLocalRef( env, jObjCls );
		return -1;
	}
	#endif
return 0;

}



/*����ҳ��ֵ��ȡ������Ȩ��Ϣ*/
/*INDEX��Χ: 0~319*/
/*����ֵ:-1,ȡֵ���󣬶����ʧ��*/
/*����ֵ:-2,û�в������ܿ����޷���ȡ���*/
/*����ֵ:0 �ɹ�*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetEntitleByPage
(JNIEnv *env, jobject obj, jobject objdata,int pagenum,int getnumber)
{
	jclass 		jObjCls=NULL,jlistcls=NULL;
	jfieldID 	jFidTemp=NULL;
	jmethodID 	jMetid=NULL;	
	jobjectArray  	jArrList=NULL;
	jobject 		jObjListOne=NULL;

	int	 ret = 0,index,ii;
#if 0	

	trcas_entitl_t entitle[MAX_ONE_ENTITLE_PAGE];
	unsigned short ulYear =0; 
	CAS_SCARD_STATUS state;
	int  realget=0;
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== pagenum %d",__FUNCTION__,__LINE__,pagenum);	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}
	jObjCls =env->GetObjectClass( objdata );
	if( NULL == jObjCls )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}
	memset(entitle,0,MAX_ONE_ENTITLE_PAGE*sizeof(trcas_entitl_t));
	for(index = pagenum*getnumber; index < (pagenum+1)*(getnumber); index++)
	{
		DEBUG_MSG2( -1," =======%d============= index %d\n",__LINE__,index);
		if(index >=320)
			break;
		entitle[realget].index =index;
		ret = MC_TIME_GetEntitleInfo((unsigned char *)(&(entitle[realget].startime)),(unsigned char *)(&(entitle[realget].endtime)),&(entitle[realget].remain),index);
		if((ret==1)||(ret==2))
		{
			DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
			//break;
		}
		entitle[realget].estatus =ret;	
		DEBUG_MSG3(-1,"%s+++++++++++++++++++++=%d==ret%d",__FUNCTION__,__LINE__,ret);
		realget++;
	}
	if(realget>0)
	{
		ret = 0;
		jMetid = env->GetMethodID( jObjCls, "BulidArrE", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEntitleList;)V");
		env->CallVoidMethod( objdata, jMetid,realget, objdata );
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		//env->DeleteLocalRef( (jobject)jMetid);
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		jMetid = env->GetMethodID( jObjCls, "BulidElist", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEntitleList;)V");
		env->CallVoidMethod( objdata, jMetid,realget, objdata );
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		//env->DeleteLocalRef( (jobject)jMetid);
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		jFidTemp = env->GetFieldID( jObjCls,"EList","[Lcom/pbi/dvb/dvbinterface/NativeCA$caEntitle;");
		DEBUG_MSG2(-1,"%s+++++++++++++++++++++=%d==",__FUNCTION__,__LINE__);
		jArrList = (jobjectArray)env->GetObjectField( objdata, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);

			DeleteLocalRef( env, jObjCls );
			return -1;
		}
		DEBUG_MSG1( -1, "entitle total ===%d==",index);
		for(ii =0;ii<realget; ii++)
		{		
			if(entitle[ii].index>=320)
				break;
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);				

				DeleteLocalRef( env, jObjCls );
				return -1;
			}
			jlistcls =env->GetObjectClass( jObjListOne );
			if( NULL == jlistcls )
			{

				DeleteLocalRef( env, jObjCls );
				DeleteLocalRef( env, jObjListOne );				

				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
				return -1;
			}
			ulYear =(entitle[ii].startime.bYear[0])*100 +entitle[ii].startime.bYear[1];
			SetShortCtoJava( env, jObjListOne, jlistcls, "startYear",ulYear);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMonth",entitle[ii].startime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startDay",entitle[ii].startime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startHour",entitle[ii].startime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startMinute",entitle[ii].startime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "startSecond",entitle[ii].startime.bSecond);
			//DEBUG_MSG3( 1,"TEST  %d=======%d===c:-%c-========== \n",__LINE__,entitle[ii].startime.bMonth,entitle[ii].startime.bMonth);
			ulYear = (entitle[ii].endtime.bYear[0]*100)+entitle[ii].endtime.bYear[1];	
			SetShortCtoJava( env, jObjListOne, jlistcls, "endYear",ulYear);
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMonth",entitle[ii].endtime.bMonth);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endDay",entitle[ii].endtime.bDay);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endHour",entitle[ii].endtime.bHour);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endMinute",entitle[ii].endtime.bMinute);	
			SetCharCtoJava( env, jObjListOne, jlistcls, "endSecond",entitle[ii].endtime.bSecond);	
			DEBUG_MSG1( -1," =======%d============= \n",__LINE__);
			SetIntCtoJava( env, jObjListOne, jlistcls, "pulRemain",entitle[ii].remain);	
			SetIntCtoJava( env, jObjListOne, jlistcls, "index",entitle[ii].index);
			SetIntCtoJava( env, jObjListOne, jlistcls, "status",entitle[ii].estatus);

			DeleteLocalRef( env, jlistcls );
			DeleteLocalRef( env, jObjListOne );
			DEBUG_MSG1( -1," =======%d============= \n",__LINE__);
		}
		SetShortCtoJava( env, objdata, jObjCls,"totalnum",(jshort)realget);

		DeleteLocalRef( env, jObjCls );
		DEBUG_MSG2( -1," =======%d============= realget%d\n",__LINE__,realget);
		return 0;
	}
	else
	{		
		DeleteLocalRef( env, jObjCls );
		return -1;
	}
#endif
return 0;

}
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetEntitleTotals
(JNIEnv *env, jobject obj)
{
		return MAX_ENTITLE_NUM;
}
/*Ǯ����Ϣ*/
/*����ֵ:-1,ȡֵ���󣬶����ʧ��*/
/*����ֵ:-2,û�в������ܿ����޷���ȡ���*/
/*����ֵ:0 �ɹ�*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_GetEpuse
(JNIEnv *env, jobject obj, jobject data)
{
	
	jclass 			jobjCLS=NULL,jlistcls=NULL;
	jobjectArray  	jArrList=NULL;
	jobject 		jObjListOne=NULL;
	jmethodID 		jMetid=NULL;	
	jfieldID 		jFidTemp=NULL;

	
	int	 ret = 0,index =0,ii;

#if 0	
	CAS_EPURSEINFO_STRUCT psEPurseInfo[MAX_WALLET_NUM];
	
	CAS_SCARD_STATUS state;
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	
	jobjCLS =env->GetObjectClass( data );
	if( NULL == jobjCLS )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}
	
	memset(psEPurseInfo,0,sizeof(CAS_EPURSEINFO_STRUCT)*MAX_WALLET_NUM);
	
	for(index =0; index<MAX_WALLET_NUM; index++)
	{
		DEBUG_MSG2( -1,"ERROR =======%d============= index%d\n",__LINE__,index);
		ret = MC_GetEPurseState(&psEPurseInfo[index], index);
		if( ret !=0 )
		{
			break;
		}
	}
	if(index>0)
	{
		ret = 0;
		
		jMetid = env->GetMethodID( jobjCLS, "BulidArrE", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEpuseList;)V");
		env->CallVoidMethod( data, jMetid,index, data );
		//env->DeleteLocalRef( (jobject)jMetid);
		
		
		jMetid = env->GetMethodID( jobjCLS, "BulidElist", "(SLcom/pbi/dvb/dvbinterface/NativeCA$IppEpuseList;)V");
		env->CallVoidMethod( data, jMetid,index, data );
		//env->DeleteLocalRef( (jobject)jMetid);

		

		
		jFidTemp = env->GetFieldID( jobjCLS,"EPUList","[Lcom/pbi/dvb/dvbinterface/NativeCA$caEpuseInfo;");
		jArrList = (jobjectArray)env->GetObjectField( data, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);

			DeleteLocalRef( env, jobjCLS );		
			return -1;
		}

		for(ii =0;ii<index; ii++)
		{
			
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);

				DeleteLocalRef( env, jobjCLS );
				return -1;
			}
		

			jlistcls =env->GetObjectClass( jObjListOne );
			if( NULL == jlistcls )
			{
				ret = -1;
				DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);

				DeleteLocalRef( env, jobjCLS );
				DeleteLocalRef( env, jObjListOne);
				return -1;
			}
			
			SetIntCtoJava( env, jObjListOne, jlistcls, "ulCashValue",psEPurseInfo[ii].ulCashValue);
			SetIntCtoJava( env, jObjListOne, jlistcls, "ulCreditValue",psEPurseInfo[ii].ulCreditValue); 
			SetIntCtoJava( env, jObjListOne, jlistcls, "wRecordIndex",psEPurseInfo[ii].wRecordIndex);
			
			
			DEBUG_MSG3(-1,"wRecordIndex[%d] ulCreditValue[%d]  ulCashValue[%d]",psEPurseInfo[ii].wRecordIndex,psEPurseInfo[ii].ulCreditValue,psEPurseInfo[ii].ulCashValue);


			DeleteLocalRef( env, jlistcls );
			DeleteLocalRef( env, jObjListOne );
		}
		
		SetShortCtoJava( env, data, jobjCLS,"totalnum",(jshort)index);
		
		DeleteLocalRef( env, jobjCLS );
		return 0;

	}
	else
	{

		DeleteLocalRef( env, jobjCLS );
		return -1;
	}
#endif	
return 0;

}

/*��ȡ��ǰ�ۿ�����*/
//0 �ɹ�  -2: δ�������ܿ�������ֵʧ��
/*level :0~10 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_SetRating
(JNIEnv *env, jobject obj,jstring jsPin,jint llen ,jint lLevel)
{

	int	 ret = 0;
	unsigned char  result =0;

	unsigned char *cPin = NULL;
	int ilen = 0,i;

#if 0	

	CAS_SCARD_STATUS state;
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	ilen = env->GetStringUTFLength(jsPin);
	cPin = (unsigned char *)malloc( sizeof(char) * (ilen + 1) );
	if( NULL == cPin )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==  %d ",__FUNCTION__,__LINE__,ilen);
	
	ret = Jstringtoc_UTF8( env, jsPin, (char *)cPin );
	if( 0 != ret )
	{
		if( NULL != cPin )
		{
			free(cPin);
		}
		return -1;
	}
	for(i =0;i<ilen;i++)
	{
		cPin[i] =cPin[i]-0x30;
	}
		
	ret = MC_SCARD_SetRating( cPin, ilen, lLevel );
	if(ret!=0)
	{
		if( NULL != cPin )
		{
			free(cPin);
		}
		return -1;
	}
	
	DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==  ok",__FUNCTION__,__LINE__);
	return 0;
#endif
return 0;

}

/*�޸����ܿ�����*/
//0 �ɹ�  -2: δ�������ܿ�������ֵʧ��
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_ChangePin
(JNIEnv *env, jobject obj, jstring jsOldPin, jstring jsNewPin,jint pinlen)
{

	int ret = 0,i=0;
	unsigned char ucOldPin[10];
	unsigned char ucNewPin[10];
#if 0

	CAS_SCARD_STATUS state;
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	memset( ucOldPin, 0, 10 );
	memset( ucNewPin, 0, 10 );

	ret = Jstringtoc_UTF8( env, jsOldPin, (char *)ucOldPin );
	if( 0 != ret )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}
	
	ret = Jstringtoc_UTF8( env, jsNewPin, (char *)ucNewPin );
	if( 0 != ret )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}	
	
	
	for(i=0;i<pinlen;i++)
	{
		ucOldPin[i]=ucOldPin[i]-0x30;
		ucNewPin[i]=ucNewPin[i]-0x30;
	}
	
	
	ret = MC_SCARD_ChangePin( ( unsigned char *)ucOldPin, ucNewPin,pinlen );
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== ret =%d",__FUNCTION__,__LINE__,ret);
	
	return ret;
#endif
return 0;

}



JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_caParentalCtrlUnlock
(JNIEnv *env, jobject obj,jstring jsPin,jint plen )
{
	
	int	 ret = 0,i=0;
	int iLen = 0;
	unsigned char ucPin[10];
#if 0
	CAS_SCARD_STATUS state;
	
	ret= MC_GetScStatus(&state);
	if(state !=0)
	{		
		DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==card out ret =%d",__FUNCTION__,__LINE__,ret);
		return -2;
	}

	memset( ucPin, 0, 10 );

	
	ret = Jstringtoc_UTF8( env, jsPin, (char *)ucPin );
	if( 0 != ret )
	{
		DEBUG_MSG1( -1,"ERROR =======%d============= \n",__LINE__);
		return -1;
	}

	
	for(i=0;i<plen;i++)
	{
		ucPin[i]=ucPin[i]-0x30;
		DEBUG_MSG1( -1,"pin[%d] ===== \n",ucPin[i]);
	}

	ret =MC_SCARD_ParentalCtrlUnlock(ucPin,plen);
	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d== ret %d",__FUNCTION__,__LINE__,ret);
	if(ret !=0)
	{
		return -1;
	}
#endif	
    return 0;	
}

/*��ȡ���ܿ�״̬,0:���γ���1;�����룬2:������*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_CaGetScStatus
(JNIEnv *env, jobject obj)
{
	return CTISC_GetScStatus();
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_factoryResetTest
  (JNIEnv *env, jobject obj)
{
	int  ret = 0;

	ret= CTISC_GetScStatus();
	if(ret ==0)
	{
		return 1;
	}
	else if (ret ==1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}



