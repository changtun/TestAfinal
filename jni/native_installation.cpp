#include <stdio.h>
#include <nativehelper/jni.h>
#include <drv_api.h>
#include <dvbcore.h>
#include <installation.h>

#include "logit.h"
#include "native_installation.h"

#include "jnistaple.h"
#include "tuner.h"
#include "pbi_coding.h"
#include "installation_message.h"

bool TimeOutClass(JNIEnv *env, jobject ObjTimeOut, Install_Timeout_t *ptTimeout)
{
	jclass TimeOutCls=NULL;
	bool ret = TRUE;
	
	TimeOutCls = env->GetObjectClass( ObjTimeOut );
	if( NULL == TimeOutCls || NULL == ptTimeout )
	{
		ret = FALSE;
		goto OUT_TIME_OUT_CLASS;
	}

	ptTimeout->u32TunerTimeout 		= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32TunerTimeout");
	ptTimeout->u32NITActualTimeout 	= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32NITActualTimeout");
	ptTimeout->u32NITOtherTimeout	= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32NITOtherTimeout");
	ptTimeout->u32BATTimeout 		= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32BATTimeout");
	ptTimeout->u32PATTimeout 		= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32PATTimeout");
	ptTimeout->u32PMTTimeout 		= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32PMTTimeout");
	ptTimeout->u32SDTActualTimeout 	= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32SDTActualTimeout");
	ptTimeout->u32SDTOtherTimeout 	= GetIntJavaToC( env, ObjTimeOut, TimeOutCls, "u32SDTOtherTimeout");

	DEBUG_MSG1( -1, T("TunerTimeout %d____________________________"),ptTimeout->u32TunerTimeout);
	DEBUG_MSG1( -1, T("NITActualTimeout %d____________________________"),ptTimeout->u32NITActualTimeout);
	DEBUG_MSG1( -1, T("NITOtherTimeout %d____________________________"),ptTimeout->u32NITOtherTimeout);
	DEBUG_MSG1( -1, T("BATTimeout %d____________________________"),ptTimeout->u32BATTimeout);
	DEBUG_MSG1( -1, T("PATTimeout %d____________________________"),ptTimeout->u32PATTimeout);
	DEBUG_MSG1( -1, T("PMTTimeout %d____________________________"),ptTimeout->u32PMTTimeout);
	DEBUG_MSG1( -1, T("SDTActualTimeout %d____________________________"),ptTimeout->u32SDTActualTimeout);
	DEBUG_MSG1( -1, T("SDTOtherTimeout %d____________________________"),ptTimeout->u32SDTOtherTimeout);

OUT_TIME_OUT_CLASS:
	
	DeleteLocalRef(env, TimeOutCls);

	return ret;
}



bool InstallTpClass(JNIEnv *env, jobject ObjInstallTp, Install_Tp_t tItp)
{
	jclass InstallTpCls=NULL;
	jfieldID jFieldTemp=NULL;
	jobject jObjTemp=NULL;
	jclass jClsTemp=NULL;
	
	bool ret = TRUE;
	
	InstallTpCls = env->GetObjectClass( ObjInstallTp );
	if( NULL == InstallTpCls )
	{
		DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
		ret = FALSE;
		goto OUT_INSTALL_TP_CLASS;
	}

	SetIntCtoJava( env, ObjInstallTp, InstallTpCls, "u16NetId", tItp.u16NetId );
	SetIntCtoJava( env, ObjInstallTp, InstallTpCls, "u16TS_Id", tItp.u16TS_Id );
	SetIntCtoJava( env, ObjInstallTp, InstallTpCls, "u16ON_Id", tItp.u16ON_Id );
	SetIntCtoJava( env, ObjInstallTp, InstallTpCls, "eSignalSource", tItp.uTunerDesc.eSignalSource );

	switch( tItp.uTunerDesc.eSignalSource )
	{
		case eDVBCORE_SIGNAL_SOURCE_DVBC:
		{
			jFieldTemp = env->GetFieldID( InstallTpCls,"uTunerDesc","Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
			if( NULL == jFieldTemp )
			{
				ret = FALSE;
				goto OUT_INSTALL_TP_CLASS;
			}

			jObjTemp = env->GetObjectField( ObjInstallTp, jFieldTemp );
			
			ret = SetDvbCroeCableDescClass( env, jObjTemp, tItp.uTunerDesc.tCable );
			if( FALSE == ret )
			{
				DEBUG_MSG(-1,T("NativeInstallation_installationStart SetDvbCroeCableDescClass error!"));
				goto OUT_INSTALL_TP_CLASS;
			}
		}break;
		default:
		{
			DEBUG_MSG1(-1,"++++++++++++default+++++++++=%d==",__LINE__);
		}
	}
/*
	jFieldTemp = env->GetFieldID( InstallTpCls,"uTunerDesc","Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
	if( NULL == jFieldTemp )
	{
		ret = FALSE;
		goto OUT_INSTALL_TP_CLASS;
	}

	env->SetObjectField( ObjInstallTp,jFieldTemp, jObjTemp );
*/

OUT_INSTALL_TP_CLASS:	
	DeleteLocalRef(env, InstallTpCls);
	DeleteLocalRef(env, jObjTemp);

	return ret;
}

bool InstallServClass(JNIEnv *env, jobject ObjInstallServ, Install_Serv_t tIserv)
{
	jclass InstallServCls=NULL;
	jstring jStrServName=NULL;
	jfieldID jFidTemp=NULL;
	jobject jObjTemp=NULL;
	
	bool ret = TRUE;
	int NameLen = 0;
	int iRet = 0;
	int cmp = 0;
	char *pOutName = NULL;
	char name[20];
	
	InstallServCls = env->GetObjectClass( ObjInstallServ );
	if( NULL == InstallServCls )
	{
		ret = FALSE;
		goto OUT_INSTALL_SERV_CLASS;
	}

	DEBUG_MSG1(-1,"u8ServType %x.\n",tIserv.u8ServType);
	SetIntCtoJava( env, ObjInstallServ, InstallServCls, "u16ServId", (jchar)tIserv.u16ServId );
	SetIntCtoJava( env, ObjInstallServ, InstallServCls, "u16LogicalNum", (jchar)tIserv.u16LogicalNum );
	SetIntCtoJava( env, ObjInstallServ, InstallServCls, "u16PMTPid", (jchar)tIserv.u16PMTPid );
	SetCharCtoJava( env, ObjInstallServ, InstallServCls, "u8ServType", (jchar)tIserv.u8ServType );
	SetCharCtoJava( env, ObjInstallServ, InstallServCls, "u8FreeCA_Mode", (jchar)tIserv.u8FreeCA_Mode );

	DEBUG_MSG2(-1,"ServNameType 0x%x. LogicalNum 0x%x.\n",tIserv.uServNameType,tIserv.u16LogicalNum);

	if( NULL == tIserv.sServName )
	{
		cmp = 1;
	}
	else
	{
		cmp = strncmp( tIserv.sServName, DEFAULT_SERVICE_NAME, tIserv.u32ServNameLen );
	}
	if( 0 != cmp )
	{
		jStrServName = CodingToJstring( env, tIserv.uServNameType, tIserv.sServName, tIserv.u32ServNameLen-1 );
	}
	else
	{
		memset( name, 0, 20 );
		sprintf( name, "SID:%d", tIserv.u16ServId );
		tIserv.uServNameType = CHARACTER_TYPE_UTF8;
		jStrServName = CodingToJstring( env, tIserv.uServNameType, name, strlen(name) );
	}

	SetStringJnitoJava( env, ObjInstallServ, InstallServCls, "sServName", jStrServName );


	jFidTemp = env->GetFieldID( InstallServCls,"ptTpDesc","Lcom/pbi/dvb/dvbinterface/NativeInstallation$Install_Tp_t;");
	jObjTemp = env->GetObjectField( ObjInstallServ, jFidTemp );
	ret = InstallTpClass( env, jObjTemp, tIserv.ptTpDesc[0] );	
	if( TRUE != ret )
	{
		
	}

OUT_INSTALL_SERV_CLASS:

	DeleteLocalRef(env, jObjTemp);
	DeleteLocalRef(env, jStrServName);
	DeleteLocalRef(env, InstallServCls);
	
	return ret;
}


/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationInit
 * Signature: ()I
 */
 JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationInit
  (JNIEnv *env, jobject obj)
{
	jint ret = 0;

	DEBUG_MSG(-1,T("Installation Installation Init!")); 

/*
	if( FALSE == register_message_callback_methods(env))
	{
		DEBUG_MSG(-1,"register message error!============================");
	}
*/	
	ret = Installation_Init();
	if( 0 != ret )
	{
		DEBUG_MSG(-1,T("Installation Installation Init error!"));
	}
	else
	{
		DEBUG_MSG(-1,T("Installation Installation Init success!"));
	}
	
	return ret;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationStart
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativeInstallation/Install_Cfg_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationStart
  (JNIEnv *env, jobject obj, jobject objCfg)
{	
	jclass    	Clazz_Cfg=NULL;
	jobject    	obj_Temp=NULL;
	jobjectArray obj_Arry_Temp=NULL;
	jfieldID 	jFidTemp=NULL;
	bool ret = TRUE;

	int iTunerType = 0;
	unsigned int ij = 0;
	Install_Cfg_t		cCfg;
	Install_Timeout_t	tTimeout;

	Clazz_Cfg = env->GetObjectClass( objCfg );
	if( NULL == Clazz_Cfg )
	{
		DEBUG_MSG(-1,T("NativeInstallation_installationStart GetObjectClass error 0!"));
		ret = FALSE;
		goto OUT_INSTALLATION_START;
	}

	memset( &cCfg, 0, sizeof(Install_Cfg_t) );
	memset( &tTimeout, 0, sizeof(Install_Timeout_t) );

	cCfg.eSearchType	= (Install_Search_Type_e)GetIntJavaToC( env, objCfg, Clazz_Cfg, "eSearchType");
	cCfg.u32MainTpNum 	= GetIntJavaToC( env, objCfg, Clazz_Cfg, "u32MainTpNum");
	cCfg.u32NormalTpNum = GetIntJavaToC( env, objCfg, Clazz_Cfg, "u32NormalTpNum");
	cCfg.u8SearchLimitFlag = (char)GetIntJavaToC( env, objCfg, Clazz_Cfg, "u8SearchLimitFlag" );
	iTunerType = GetIntJavaToC( env, objCfg, Clazz_Cfg, "eSignalSource");
	
	DEBUG_MSG3( -1, T("search type %d, mantp num %d. normaltp num %d.======================="), \
		cCfg.eSearchType, cCfg.u32MainTpNum, cCfg.u32NormalTpNum );


	jFidTemp = env->GetFieldID( Clazz_Cfg,"ptTimeout","Lcom/pbi/dvb/dvbinterface/NativeInstallation$Install_Timeout_t;");
	if( NULL == jFidTemp )
	{
		DEBUG_MSG(-1,T("NativeInstallation_installationStart Install_Timeout_t error!"));
		ret = FALSE;
		goto OUT_INSTALLATION_START;
	}
	
	obj_Temp = env->GetObjectField( objCfg, jFidTemp );
	ret = TimeOutClass( env, obj_Temp, &tTimeout );
	if( FALSE == ret )
	{
		DEBUG_MSG(-1,T("NativeInstallation_installationStart TimeOutClass error!"));
		goto OUT_INSTALLATION_START;
	}
	
	DeleteLocalRef(env, obj_Temp);
	
	if( 0 < cCfg.u32MainTpNum )
	{
		switch( iTunerType )
		{
			case eDVBCORE_SIGNAL_SOURCE_DVBC:
			{
				
				jFidTemp = env->GetFieldID( Clazz_Cfg,"puMainTpDescList","[Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
				if( NULL == jFidTemp )
				{
					DEBUG_MSG(-1,T("NativeInstallation_installationStart DVBCore_Cab_Desc_t error!"));
					ret = FALSE;
					goto OUT_INSTALLATION_START;
				}
				
				obj_Arry_Temp = (jobjectArray)env->GetObjectField( objCfg, jFidTemp );
				if( NULL == obj_Arry_Temp )
				{
					DEBUG_MSG(-1,"obj temp is Null =-----------------------------------------");
					ret = FALSE;
					goto OUT_INSTALLATION_START;
				}
				
				cCfg.puMainTpDescList = (DVBCore_Tuner_Desc_u*)malloc( sizeof(DVBCore_Tuner_Desc_u) * cCfg.u32MainTpNum );
				if( NULL == cCfg.puMainTpDescList )
				{
					DEBUG_MSG(-1,T("NativeInstallation_installationStart puMainTpDescList is NULL!"));
					ret = FALSE;
					goto OUT_INSTALLATION_START;
				}
				memset( cCfg.puMainTpDescList, 0, sizeof(sizeof(DVBCore_Tuner_Desc_u) * cCfg.u32MainTpNum) );
				
				for( ij = 0; ij < cCfg.u32MainTpNum; ij++ )
				{
					obj_Temp = env->GetObjectArrayElement( obj_Arry_Temp, ij );
					
					ret = GetDvbCroeCableDescClass( env, obj_Temp, (DVBCore_Cab_Desc_t *)&cCfg.puMainTpDescList[ij] );
					if( FALSE == ret )
					{

						DEBUG_MSG(-1,T("NativeInstallation_installationStart DvbCroeCableDescClass error!"));
						goto OUT_INSTALLATION_START;
					}
					DeleteLocalRef(env, obj_Temp);
				}
			}break;
			default:
			{
				
			}
		}
	}
	else
	{
		DEBUG_MSG(-1,"----------------main tp list null----------------------");
		cCfg.puMainTpDescList = NULL;
	}

	DEBUG_MSG1(-1,"Normal tp num  %d................\n", cCfg.u32NormalTpNum );
	if( cCfg.u32NormalTpNum > 0 )
	{
		switch( iTunerType )
		{
			case eDVBCORE_SIGNAL_SOURCE_DVBC:
			{
				jFidTemp = env->GetFieldID( Clazz_Cfg,"puNormalTpDescList","[Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
				if( NULL == jFidTemp )
				{
					DEBUG_MSG(-1,T("NativeInstallation_installationStart DVBCore_Cab_Desc_t error!"));
					ret = FALSE;
					goto OUT_INSTALLATION_START;
				}
				
				obj_Arry_Temp = (jobjectArray)env->GetObjectField( objCfg, jFidTemp );

				cCfg.puNormalTpDescList = (DVBCore_Tuner_Desc_u*)malloc(sizeof(DVBCore_Tuner_Desc_u) * cCfg.u32NormalTpNum);
				if( NULL == cCfg.puNormalTpDescList )
				{
					DEBUG_MSG(-1,T("NativeInstallation_installationStart puNormalTpDescList is NULL!"));
					ret = FALSE;
					goto OUT_INSTALLATION_START;
				}
				memset( cCfg.puNormalTpDescList, 0, sizeof(sizeof(DVBCore_Tuner_Desc_u) * cCfg.u32NormalTpNum) );
				
				for( ij = 0; ij < cCfg.u32NormalTpNum; ij++ )
				{
					obj_Temp = env->GetObjectArrayElement( obj_Arry_Temp, ij );
					ret = GetDvbCroeCableDescClass( env, obj_Temp, (DVBCore_Cab_Desc_t *)&cCfg.puNormalTpDescList[ij] );
					if( FALSE == ret )
					{
						DEBUG_MSG(-1,T("Java_com_pbi_dvb_message_NativeInstallation_installationStart DvbCroeCableDescClass error!"));
						//env->DeleteLocalRef(obj_Temp);
						goto OUT_INSTALLATION_START;
					}
					DeleteLocalRef(env, obj_Temp);

				}
			}break;
			default:
			{
				
			}
		}	
	}
	else
	{
		DEBUG_MSG(-1,"----------------Normal tp list null----------------------");
		cCfg.puNormalTpDescList = NULL;
	}

	cCfg.pfnNotifyFunc = Installation_Call_Back;
	DEBUG_MSG(-1,T("##########################################################Installation_Start!"));

	Installation_Start( cCfg );


OUT_INSTALLATION_START:
	if( NULL != cCfg.puMainTpDescList )
	{	
		free(cCfg.puMainTpDescList);
	}

	if( NULL != cCfg.puNormalTpDescList)
	{
		free(cCfg.puNormalTpDescList);
	}
	DEBUG_MSG(-1,T("###########################end###############################Installation_Start!"));
	
	DeleteLocalRef(env, Clazz_Cfg);
	DeleteLocalRef(env, obj_Temp);
	DeleteLocalRef(env, obj_Arry_Temp);
	
	return ( (ret == TRUE)? 0:1 );
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationStop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationStop
  (JNIEnv *env, jobject obj)
{
	jint ret = 0;
	 	
	ret = Installation_Stop();
	if( 0 != ret )
	{
		DEBUG_MSG(-1,T("Installation Installation Stop error!"));
	}
	else
	{
		DEBUG_MSG(-1,T("Installation Installation Stop success!"));
	}
	return ret;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationGetSearchRet
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativeInstallation/Install_Search_Result_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationGetSearchRet
  (JNIEnv *env, jobject obj, jobject objIsr)
{
	jclass    	Clazz=NULL;
	jint		jRet = 0;
	jboolean  	jbRet = 0;
	jfieldID 	jFidTemp=NULL;
	jobject 	jObjListOne=NULL;
	jobjectArray  	jArrList=NULL;

	jmethodID jMetid=NULL;
	
	unsigned int ii = 0;
	bool	ret = TRUE;
	Install_Search_Result_t tIsr;
		
	Clazz = env->GetObjectClass( objIsr );
	if( NULL == Clazz )
	{
		DEBUG_MSG(-1,T("NativeInstallation_installationGetSearchRet GetObjectClass error !"));
		ret = FALSE;
		goto OUT_INSTALLATION_GET_SEARCH_RET;
	}	

	memset( &tIsr, 0, sizeof(Install_Search_Result_t) );
	
	jRet = Installation_Get_SearchRet( &tIsr );
	if( 0 != jRet )
	{
		DEBUG_MSG(-1,T("NativeInstallation_installationGetSearchRet Installation_Get_SearchRet error !"));
		ret = FALSE;
		goto OUT_INSTALLATION_GET_SEARCH_RET;
	}

	SetIntCtoJava( env, objIsr, Clazz, "u32TpNum", tIsr.u32TpNum );
	SetIntCtoJava( env, objIsr, Clazz, "u32ServNum", tIsr.u32ServNum );


	jMetid = env->GetMethodID( Clazz, "BulidTpServList", "(Lcom/pbi/dvb/dvbinterface/NativeInstallation$Install_Search_Result_t;)V");
	env->CallVoidMethod( objIsr, jMetid, objIsr );
	
	if( 0 < tIsr.u32TpNum )
	{
		jFidTemp = env->GetFieldID( Clazz,"ptTpList","[Lcom/pbi/dvb/dvbinterface/NativeInstallation$Install_Tp_t;");

		jArrList = (jobjectArray)env->GetObjectField( objIsr, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);
			ret = FALSE;
			goto OUT_INSTALLATION_GET_SEARCH_RET;
		}
		
		for( ii = 0; ii < tIsr.u32TpNum; ii++ )
		{
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );
			if( NULL == jObjListOne )
			{
				DEBUG_MSG1( 1,"=======%d=============",__LINE__);
				ret = FALSE;
				goto OUT_INSTALLATION_GET_SEARCH_RET;
			}
			
			jbRet = InstallTpClass( env, jObjListOne, tIsr.ptTpList[ii] );
			if( TRUE != jbRet )
			{
				ret = FALSE;
				goto OUT_INSTALLATION_GET_SEARCH_RET;
			}
			DeleteLocalRef( env, jObjListOne );
		}
	}

	if( 0 < tIsr.u32ServNum )
	{
		jFidTemp = env->GetFieldID( Clazz,"ptServList","[Lcom/pbi/dvb/dvbinterface/NativeInstallation$Install_Serv_t;");

		jArrList = (jobjectArray)env->GetObjectField( objIsr, jFidTemp );
		if( NULL == jArrList )
		{
			DEBUG_MSG1( -1, "ArrList is NULL===%d==",__LINE__);
			ret = FALSE;
			goto OUT_INSTALLATION_GET_SEARCH_RET;
		}
		
		for( ii = 0; ii < tIsr.u32ServNum; ii++ )
		{
			jObjListOne = env->GetObjectArrayElement( jArrList, ii );

			jbRet = InstallServClass( env, jObjListOne, tIsr.ptServList[ii] );
			if( TRUE != jbRet )
			{
				DEBUG_MSG1(-1,"+++++++++++++++++++++=%d==",__LINE__);
				ret = FALSE;
				goto OUT_INSTALLATION_GET_SEARCH_RET;
			}
			DeleteLocalRef( env, jObjListOne );
	}
	}
OUT_INSTALLATION_GET_SEARCH_RET:

	DeleteLocalRef(env, Clazz);
	
	return ( (ret == TRUE)?0:1 );
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationFreeSearchRet
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationFreeSearchRet
  (JNIEnv *env, jobject obj)
{
	jint ret = 0;
	 	
	Installation_Free_SearchRet();
	DEBUG_MSG(-1,T("Installation Installation Free SearchRet!"));
	return ret;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativeInstallation
 * Method:    installationDeinit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installationDeinit
  (JNIEnv *env, jobject obj)
{
	jint ret = 0;
	 	
	ret = Installation_Deinit();
	if( 0 != ret )
	{
		DEBUG_MSG(-1,T("Installation Installation Deinit error!"));
	}
	else
	{
		DEBUG_MSG(-1,T("Installation Installation Deinit success!"));
	}

	JNI_Installation_Logout(env);
	return ret;	
}
