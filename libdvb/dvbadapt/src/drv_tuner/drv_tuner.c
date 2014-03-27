/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc.
*
* Module name : drv_tuner
* File name   : drv_tuner.c
* Description : Adapt layer of tuner driver.
*
* History :
*   2012-07-06 ZYJ
*       Initial Version.
*******************************************************************************/  
#include <stdio.h>
#include "pvddefs.h"
#include "pbitrace.h"
#if defined(TUNER_TYPE_HISIDEMO)
#include "hi_unf_ecs.h"
#endif
//#ifdef TUNER_TYPE_AVL63X1
#include "AVL_tuner.h"
//#endif /* TUNER_TYPE_AVL63X1 */
//#ifdef TUNER_TYPE_nuTUNE
#include "DVBTCA_THOMSON_API.h"
//#endif /* TUNER_TYPE_nuTUNE */
//#ifdef ONBOARD
#include "tmNxTypes.h"
#include "tmbslOM3971Main.h"
//#endif /* ONBOARD */
#include "drv_tuner.h"

#include "file_config.h"

/*add for 重复锁频by xyfeng 2011-01-12{{{*/
static U32 g_uiLastFreq = 123456;
static U32 g_uiLastSymRate= 6666;
static tPI_Modulation g_eModu= ePI_16QAM;
/*}}}add by xyfeng 2011-01-12*/
static U32 g_uiLastBWidth = 0;
/* BEGIN: Added by zhwu, 2013/3/21 */
#define DRV_FAILED(res)			(res>0)
#define DRV_CHECK(res)			if(DRV_FAILED(res)) { 										\
									pbierror("FUN:%s,LINE:%d,ErrCode:%d \n ",__FUNCTION__,	\
										__LINE__,res);										\
										return eTUNER_ERROR; }											
int TUN_TYPE = 1; /* 默认NUTUNE类型 */
/* END:   Added by zhwu, 2013/3/21 */
static TUNER_ErrorCode_t tuner_read(  U16 SlaAddr, U8 *RegAddr, U32 RegSize, U8 *puBuffer, U32 BuffLength )
{
	S32 res = 0;
	res = DRV_I2C_Read( SlaAddr, RegAddr, RegSize, puBuffer, BuffLength );
	DRV_CHECK(res);
	
    return eTUNER_NO_ERROR;
}

static TUNER_ErrorCode_t choose_tuner_type(void)
{
	int res = 0; 
	U8 buff[1] ={0x1};
	U8 RegAddr[1]= {0x00};

	res = tuner_read( 0xC0,/*TMDD_TDA18250_0x00_ID_byte*/RegAddr, 1, buff, 1 );
	
	if ( buff[0] == 0xD5 )
	{
	    TUN_TYPE = 0; /* NXP tuner type */
	}
	else
	{
		res = tuner_read( 0x58, RegAddr,1, buff, 1 );
		if (( res == 0 )&&(buff[0] == 0))
		{
		    TUN_TYPE = 1; /* NUT tuner type */
		}
		else
	    {
	    	TUN_TYPE = 2; /* AVL tuner type */
		}
	}
	
	return eTUNER_NO_ERROR;
}
TUNER_ErrorCode_t DRV_Tuner_Init( void )
{
    int res = 0;	
	res = choose_tuner_type();
	
	//pbiinfo("FUN:%s,LINE:%d,TUN_TYPE:%x \n ",__FUNCTION__,__LINE__,TUN_TYPE);
	switch(TUN_TYPE)
	{
	  	case 1 :
	        res = DVBTCA_THOMSON_DrvInit(0);
			if ( res == 0 )
			{
    			pbiinfo("current is nuTune tuner type \n");
		        break;
			}
			TUN_TYPE = 0;
			pbiinfo("nuTuner tuner init fail retry NXP tuner!\n");
	    case 0 :
	        res = OM3971_Open();
			if ( res == 0 )
			{
				pbiinfo("current is NXP tuner type \n");
				break;
			}
			TUN_TYPE = 2;
			pbiinfo("NXP tuner init fail retry AVL tuner!\n");
		case 2:
			res = AVL63X1_Tuner_Initialize(GetTunerBandWidth());
			if ( res == 0 )
			    pbiinfo("current is AVL tuner type \n");
		
			break;		
	    default:
	        
			break;
	}
	DRV_CHECK(res);

	return eTUNER_NO_ERROR;
}

TUNER_ErrorCode_t  DRV_Tuner_SetFrequency(U8 ucTuner_ID, ts_src_info_t *ptSrcInfo, U32 uiTimeOut )
{
	tmErrorCode_t res = 0;
	
	switch (ptSrcInfo->MediumType)
	{
		case MEDIUM_TYPE_DVBC:
		{
			pbiinfo("DRV_Tuner_SetFrequency FrequencyKHertz[%d] , SymbolRateSPS[%d], Bandwidth[%d]\n",ptSrcInfo->u.Cable.FrequencyKHertz, ptSrcInfo->u.Cable.SymbolRateSPS, ptSrcInfo->u.Cable.Bandwidth );
			if( 0xFFFFFFFF != uiTimeOut )
		    {
		        if( ( eTun_Lock == DRV_Tuner_GetLockStatus( 0 ) )
		            && ( g_uiLastFreq == ptSrcInfo->u.Cable.FrequencyKHertz ) 
		            && ( g_uiLastSymRate == ptSrcInfo->u.Cable.SymbolRateSPS ) 
		            && ( g_eModu == ptSrcInfo->u.Cable.Modulation ) 
		            && ( g_uiLastBWidth == ptSrcInfo->u.Cable.Bandwidth ))
		        {
		            pbiinfo("[xyfeng]DRV_Tuner_SetFrequency  locked(%d) \n",g_uiLastFreq );
		            return eTUNER_NO_ERROR;
		        }
		    }
		    g_uiLastFreq = ptSrcInfo->u.Cable.FrequencyKHertz; 
		    g_uiLastSymRate = ptSrcInfo->u.Cable.SymbolRateSPS;
		    g_eModu = ptSrcInfo->u.Cable.Modulation; 
		    g_uiLastBWidth = ptSrcInfo->u.Cable.Bandwidth;
	    }
		break;
		case MEDIUM_TYPE_DVBT:
		{
			pbiinfo("DRV_Tuner_SetFrequency FrequencyKHertz[%d] ,  Bandwidth[%d]\n",ptSrcInfo->u.Terrestrial.FrequencyKHertz, ptSrcInfo->u.Terrestrial.Bandwidth );
			if( 0xFFFFFFFF != uiTimeOut )
		    {
			    if( ( eTun_Lock == DRV_Tuner_GetLockStatus( 0 ) )
			        && ( g_uiLastFreq == ptSrcInfo->u.Terrestrial.FrequencyKHertz ) 
			        && ( g_uiLastBWidth == ptSrcInfo->u.Terrestrial.Bandwidth ) )
			    {
			        pbiinfo("[xyfeng]DRV_Tuner_SetFrequency  locked(%d) \n",g_uiLastFreq );
			        return eTUNER_NO_ERROR;
			    }
		    }    
		    g_uiLastFreq = ptSrcInfo->u.Terrestrial.FrequencyKHertz; 
		    g_uiLastBWidth = ptSrcInfo->u.Terrestrial.Bandwidth;
		}
		break;
		case MEDIUM_TYPE_DVBS:
		case MEDIUM_TYPE_DVBS2:
		default:
			break;
	}
	switch(TUN_TYPE)
	{
		case 0:	
			{
				UInt32 freq = 0,Symb = 0;
				freq = ptSrcInfo->u.Cable.FrequencyKHertz;
				Symb = ptSrcInfo->u.Cable.SymbolRateSPS;

				res = OM3971_SetFrequence( freq, Symb, ptSrcInfo->u.Cable.Modulation, ptSrcInfo->u.Cable.Bandwidth );
				DRV_CHECK(res);
			}
		    break;
		case 1:
			res = DRV_Tuner_ThomsonSet(ptSrcInfo);
			DRV_CHECK(res);
		    break;
		case 2:
			{
				AVL_TUNER_PARM_STRU tTunerParam;
				tTunerParam.m_uiFrequency_Hz = ptSrcInfo->u.Cable.FrequencyKHertz * 1000;
				tTunerParam.m_demod_mode = 1;//AVL_DEMOD_MODE_DVBC;
				tTunerParam.m_uiSymbolRate_Hz = ptSrcInfo->u.Cable.SymbolRateSPS * 1000;

				res = AVL63X1_Tuner_SetFrequency(0, &tTunerParam, 0);
				DRV_CHECK(res);
			}
			break;
		default:
			break;
	}

	return eTUNER_NO_ERROR;
}
TUNER_LOCKSTATUS_t  DRV_Tuner_GetLockStatus( U8 ucTuner_ID )
{
	UInt8 TS = 0;
	int res = 0;
	TUNER_LOCKSTATUS_t	tRetLock=eTun_unLock;
	
    switch ( TUN_TYPE )
    {
        case 0 :
			res = OM3971_GetLockStatus(&TS);
			if ( res != 0 )
			    return eTun_Lock_Err;
			if ( TS == 1 )
			    return eTun_Lock;
            break;
			
        case 1 :
			Tuner_GetFELockThomson( &tRetLock );
            break;
		case 2:
			{
				AVL63X1_LockCode    ErrCode = AVL_unLock;
				ErrCode = AVL63X1_Tuner_GetLockStatus(ucTuner_ID);
				if(AVL_Lock_Err == ErrCode)
				    return eTun_Lock_Err;
				if(AVL_Lock == ErrCode)
				    return eTun_Lock;
			}
			break;
        default:
            break;
    }

	return tRetLock;
}
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoStrengthPercent( U8 Tuner_ID, U32 *puiSignalPower )
{
	switch ( TUN_TYPE )
    {
        case 0 :
        {
        	tmErrorCode_t res = 0;
        	res = OM3971_GetSignalStrength(puiSignalPower);
			if ( 0 != res )
				return eTUNER_ERROR;
		}
        break;
        case 1 :
        {
        	Tun_Info_t ptTunerInfo;
        	Tuner_GetFEInfoThomson(&ptTunerInfo);
        	*puiSignalPower = ptTunerInfo.SignalStrength;
    	}
        break;
		case 2:
		{
			AVL63X1_ErrorCode   ErrCode = AVL63X1_EC_OK;
			U16                 u16Strength = 0;
			ErrCode = AVL63X1_Tuner_GetTunInfoStrengthPercent(Tuner_ID, &u16Strength);
		    if( ErrCode != AVL63X1_EC_OK )
		        return eTUNER_ERROR;
		    *puiSignalPower = u16Strength;
	    }
		break;
        default:
            break;
    }
    return eTUNER_NO_ERROR;
}
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoQualityPercent( U8 Tuner_ID, U32 *puiSignalQuality )
{	
	switch ( TUN_TYPE )
    {
        case 0 :
        {
        	tmErrorCode_t res = 0;
        	res = OM3971_GetSignalQuality(puiSignalQuality);
			if ( 0 != res )
				return eTUNER_ERROR;
		}
        break;
        case 1 :
        {
        	Tun_Info_t ptTunerInfo;
			Tuner_GetFEInfoThomson(&ptTunerInfo);
		    *puiSignalQuality = ptTunerInfo.SignalQuality;
	    }
        break;
		case 2:
		{
			AVL63X1_ErrorCode   ErrCode = AVL63X1_EC_OK;
		    ErrCode = AVL63X1_Tuner_GetTunInfoQualityPercent(Tuner_ID, puiSignalQuality);
		    if( ErrCode != AVL63X1_EC_OK )
		        return eTUNER_ERROR;
	    }
		break;
        default:
            break;
    }
    return eTUNER_NO_ERROR;
}
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoBer( U8 Tuner_ID, U32 *puiSignalBer )
{
	switch ( TUN_TYPE )
    {
        case 0 : 
        {
        	tmErrorCode_t res = 0;
        	int Integer = 0; 
			U32 Divider = 0;	
			res = OM3971_GetSignalBer( &Integer, &Divider);
			if ( 0 != res )
				return eTUNER_ERROR;
			*puiSignalBer= Integer / Divider;
		}
        break;
        case 1 :
        {
        	Tun_Info_t ptTunerInfo;
        	Tuner_GetFEInfoThomson(&ptTunerInfo);
		    *puiSignalBer = ptTunerInfo.BitErrorRate;
	    }
        break;
		case 2:
		{
			AVL63X1_ErrorCode   ErrCode = AVL63X1_EC_OK;
		    ErrCode = AVL63X1_Tuner_GetTunInfoBer(Tuner_ID, puiSignalBer);
		    if( ErrCode != AVL63X1_EC_OK )
		        return eTUNER_ERROR;
	    }
		break;
        default:
            break;
    }
    return eTUNER_NO_ERROR;
}
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoSnr( U8 Tuner_ID, U32 *puiSignalSnr )
{
	switch ( TUN_TYPE )
    {
        case 0 :
        {
        	tmErrorCode_t res = 0;
        	int Integer = 0; 
			U32 Divider = 0;	
        	res = OM3971_GetSignalSnr( &Integer, &Divider );
			if ( 0 != res )
				return eTUNER_ERROR;
			*puiSignalSnr = Integer / Divider;
		}
        break;
        case 1 :
            break;
		case 2:
		{
			AVL63X1_ErrorCode   ErrCode = AVL63X1_EC_OK;
		    ErrCode = AVL63X1_Tuner_GetTunInfoSnr(Tuner_ID, puiSignalSnr);
		    if( ErrCode != AVL63X1_EC_OK )
		        return eTUNER_ERROR;
	    }
		break;
        default:
            break;
    }
    return eTUNER_NO_ERROR;
}
void DRV_Tuner_Deinit( void )
{
	switch( TUN_TYPE )
	{
	    case 0 :
	        OM3971_Close();
	        break;
	    case 1 :
	        #
	        break;
	    case 2 :
	        AVL63X1_Tuner_DeInit();
	        break;
	    default:
	        break;
	}
}
#ifdef ONBOARD
TUNER_ErrorCode_t DVBC_Tuner_test( void )
{
	ts_src_info_t ptSrcInfo;
	TUNER_LOCKSTATUS_t TLK;
	int iI = 0;
	U32 SignalPower;
	U32 SignalQuality;
	UInt16	SignalLevel=0;
	U32  SignalBer; U32 SignalSnr;
	
	ptSrcInfo.u.Cable.FrequencyKHertz = 698000;
	ptSrcInfo.u.Cable.SymbolRateSPS = 6875;
	ptSrcInfo.u.Cable.Modulation = ePI_64QAM;
	ptSrcInfo.u.Cable.Bandwidth = ePI_BW_8MHZ;
	
	DRV_Tuner_SetFrequency(0, &ptSrcInfo, 1000);

	for ( iI = 0 ; iI < 50; iI++ )
	{
    	TLK = DRV_Tuner_GetLockStatus(0);
		if ( TLK == 1 )
			 break;//pbierror("FUN:%s,LINE:%d,TLK:0x%x\n",__FUNCTION__,__LINE__,TLK);	return eTUNER_NOT_DONE;}
		usleep(10);
	}
	
	printf("TLK %d \n",TLK);
	if ( TLK == 1 )
	{
	    DRV_Tuner_GetTunInfoStrengthPercent(0, &SignalPower);
		//OM3971_GetSignalStrength(&SignalPower);
		//tmbslHCDP_GetSignalLevel(0, &SignalLevel);
		printf("SignalPower %d \n",SignalPower);
		
		DRV_Tuner_GetTunInfoQualityPercent(0, &SignalQuality);
		printf("SignalQuality1 %d \n",SignalQuality);

		DRV_Tuner_GetTunInfoBer(0, &SignalBer);
		printf("SignalBer %d \n",SignalBer);

		DRV_Tuner_GetTunInfoSnr(0, &SignalSnr);
		printf("SignalSnr %d \n",SignalSnr);
	}


	return eTUNER_NO_ERROR;
}
#endif /* ONBOARD */


