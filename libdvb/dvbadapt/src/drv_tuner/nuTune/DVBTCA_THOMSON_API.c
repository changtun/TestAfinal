/******************************************************************************
 *	(c)	Copyright Pro. Broadband Inc. PVware
 * 
 * File name  :DVBTCA_THOMSON_API.c 
 * Description: 
 *   
 * History :
 *	Date			   Modification								Name
 ******************************************************************************/
#include "stdio.h"
#include "string.h"
#include "pbitrace.h"
#include "drv_tuner.h"
#include "osapi.h"
#include "./include/drxk.h"
#include "./include/drx_driver.h"
#include "./include/drxk_mc.h"	
#include "./include/bsp_i2c.h"
#include "./include/drxk_tuner_tables.h"	   			/* File containing pre-configured tuners */
#include "./include/DVBTCA_THOMSON_API.h"
//#include "sys_ui_extctrl.h"
/***************************************************************************************/
//#include "pv_database.h"

#define NOTEXTCTRLCHECKTUNER	0 /*add for control if use extern check tuner by hujun 10.03.18*/
//#define TUNERDETECT	/*add for detect by hujun 10.03.18*/


/* declare demod instance */
I2CDeviceAddr_t	 	demodAddr;
DRXCommonAttr_t	 	demodCommAttr;
DRXKData_t		  	demodExtAttr;

static DRXDemodInstance_t  	demod;
static MediumType_t			ucSetMediumType = MEDIUM_TYPE_DVBS;
S32 g_tRetLock=0;//add for analog check tuner
U32 uiCountryIndex=0;

#undef TU_DBG

#ifdef TU_DBG
#define DB_TUNER_INFO(_X_) printf _X_
#define DB_TUNER_ERR(_X_) printf _X_
#else
#define DB_TUNER_INFO(_X_) pbiinfo _X_
#define DB_TUNER_ERR(_X_) pbierror _X_
#endif /* TU_DBG */

DRXStandard_t Tuner_Get_CountryAnalogFormat(void);
DRXAudStandard_t THOMSON_DrvAnalogStandDetec(void);
S32 Tuner_Get_AudioFreqByCountry(void);
/*********************************************************************************************************
 * Function 		:	  DVBTCA_THOMSON_DrvInit
 * parameters 		:	Tuner_ID				
 * Return			:	 	
 * Description 		:	
 **********************************************************************************************************/
TUNER_ErrorCode_t DVBTCA_THOMSON_DrvInit(U8 Tuner_ID)
{
	//U8	ucError=0;
	pDRXVersionList_t 	versionList = NULL;
	DRXCfgMPEGOutput_t  outputConfig;
	DRXKCfgAtvOutput_t 	atvOutputCfg;
	DRXCfg_t			config = { DRX_CFG_MPEG_OUTPUT, NULL };  

	memset(&outputConfig,0,sizeof(DRXCfgMPEGOutput_t));
	memset(&atvOutputCfg,0,sizeof(DRXKCfgAtvOutput_t));		
	demodAddr			= DRXKDefaultAddr_g;
	demodCommAttr 		= DRXKDefaultCommAttr_g;
	demodExtAttr		= DRXKData_g;
	demod				= DRXKDefaultDemod_g;

	/* connect datastructures through pointers */
	demod.myI2CDevAddr  = &demodAddr;
	demod.myCommonAttr  = &demodCommAttr;
	demod.myExtAttr	 	= &demodExtAttr;
	demod.myTuner		= &PRE_CONF_TUNER;		//TUNER_THOMSON_DHF_84000_K

	/* modify device address and ID */
	demodAddr.i2cAddr   = DRX_DEMOD_ADDR;	/* i2c address */
	demodAddr.i2cDevId  = 1;			    /* device ID   */

	/* use microcode upload (skip this part to boot from ROM) */
	demod.myCommonAttr->microcode	   	= DRXK_MC_MAIN;
	demod.myCommonAttr->verifyMicrocode = FALSE;

	if( DRXBSP_I2C_Init(Tuner_ID) != 1 )
	{
		return eTUNER_ERROR;
	}
	if( DRXBSP_HST_Init() != 1 )
	{
		return eTUNER_ERROR;
	}

	/* probe function for demod, can be called before demod is opened */
	if ( DRX_Ctrl( &demod, DRX_CTRL_PROBE_DEVICE, NULL ) != DRX_STS_OK )
	{
		DB_TUNER_ERR(("failure: no DRX-K detected\n"));
		return eTUNER_ERROR;

	}
	DB_TUNER_INFO(("success: DRX-K detected\n"));
	/* open demodulator instance */
	if ( DRX_Open( &demod ) != DRX_STS_OK )
	{
		DB_TUNER_ERR(("failure: error opening demodulator\n"));
		return eTUNER_ERROR;

	}
	DB_TUNER_INFO(("success: demodulator instance opened\n"));

	/* show version information */
	if ( DRX_Ctrl( &demod, DRX_CTRL_VERSION, &versionList ) != DRX_STS_OK )
	{
		DB_TUNER_ERR(("failure: error retrieving version information\n"));
	}
	DB_TUNER_INFO(("Version Info:\n"));
	while ( versionList != NULL )
	{
		pDRXVersion_t version = versionList->version;
		if (version)
		{
			DB_TUNER_INFO(("%s :  %s\n", version->moduleName, version->vString));
		}
		versionList = versionList->next;
	}

	/* If required, change MPEG output configuration */
	config.cfgData= &outputConfig;
	outputConfig.enableMPEGOutput = TRUE;
	outputConfig.insertRSByte = FALSE;
	outputConfig.enableParallel = TRUE;//
	outputConfig.invertDATA = FALSE;
	outputConfig.invertERR = FALSE;
	outputConfig.invertSTR = FALSE;
	outputConfig.invertVAL = FALSE;
	outputConfig.invertCLK = FALSE;
	outputConfig.staticCLK = FALSE;
	outputConfig.bitrate   = 32000000UL;
	outputConfig.widthSTR  = DRX_MPEG_STR_WIDTH_1;

	if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CFG, &config ) != 1 )
	{
		DB_TUNER_ERR(("failure: error re-configuring MPEG output\n"));
		return eTUNER_ERROR;
	}
	DB_TUNER_INFO(("success: re-configured MPEG output\n"));

	//ATV output config
	config.cfgType = (DRXCfgType_t)DRXK_CFG_ATV_OUTPUT;
	config.cfgData = &atvOutputCfg;
	/* get current settings */
	DRX_Ctrl( &demod , DRX_CTRL_GET_CFG, &config );
	/* enable CVBS output */
	atvOutputCfg.enableCVBSOutput = TRUE;
	/* enable SIF output without attenuation */
	atvOutputCfg.enableSIFOutput = TRUE;
	atvOutputCfg.sifAttenuation = DRXK_SIF_ATTENUATION_0DB;
	/* activate new settings */

	if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CFG, &config ) != 1 )
	{
		DB_TUNER_ERR(("failure: error re-configuring ATV output\n"));
		return eTUNER_ERROR;

	}
	DB_TUNER_INFO(("success: re-configured ATV output\n"));

	return eTUNER_NO_ERROR; 
}

/*********************************************************************************************************
 * Function 		:	  DRV_Tuner_ThomsonSet
 * parameters 		:	pScanInfo				
 * Return			:	 	
 * Description 		:	
 **********************************************************************************************************/
TUNER_ErrorCode_t DRV_Tuner_ThomsonSet(ts_src_info_t *pScanInfo)
{
	TUNER_ErrorCode_t 	tError=eTUNER_NO_ERROR;
	DRXChannel_t	  		channel;
	memset(&channel,0,sizeof(DRXChannel_t));
	ucSetMediumType=pScanInfo->MediumType;
	if(THOMSON_DrvModeChange(pScanInfo)!=0)
	{
		return eTUNER_ERROR;
	}
	switch(pScanInfo->MediumType)
	{
		case MEDIUM_TYPE_DVBC:
			{
				DRXLockStatus_t		tLockStatus=DRX_NEVER_LOCK;
				DRXConstellation_t		tChoseQam=DRX_CONSTELLATION_QAM64;
				U8					ucLocktime=0;
				switch(pScanInfo->u.Cable.Modulation)
				{
					case ePI_16QAM:
					{
						tChoseQam=DRX_CONSTELLATION_QAM16;
					}
					break;	
					case ePI_32QAM:
					{
						tChoseQam=DRX_CONSTELLATION_QAM32;
					}
					break;	
					case ePI_64QAM:
					{
						tChoseQam=DRX_CONSTELLATION_QAM64;
					}					
					break;	
					case ePI_128QAM:
					{
						tChoseQam=DRX_CONSTELLATION_QAM128;
					}					
					break;			
					case ePI_256QAM:
					{
						tChoseQam=DRX_CONSTELLATION_QAM256;
					}
					break;
					default:
						break;	
				}
				
				/* set channel parameters */
				channel.bandwidth		= pScanInfo->u.Cable.Bandwidth; //DRX_BANDWIDTH_8MHZ;
				DB_TUNER_INFO((" dvbc.bandwidth = %d \n",channel.bandwidth));
				channel.mirror			= DRX_MIRROR_AUTO;
				channel.hierarchy		= DRX_HIERARCHY_AUTO;
				channel.priority		= DRX_PRIORITY_HIGH;
				channel.coderate		= DRX_CODERATE_AUTO;
				channel.guard			= DRX_GUARD_AUTO;
				channel.fftmode 		= DRX_FFTMODE_AUTO;
				channel.classification 	= DRX_CLASSIFICATION_AUTO;			
				channel.constellation 	= tChoseQam;
				channel.symbolrate		= pScanInfo->u.Cable.SymbolRateSPS*1000; /* Symb/s */
				#if 0/*scj modify 11420*/
				if( FRA == (Country_t)uiCountryIndex/*(Country_t)Dialog_GetCountryIndex()*/)
				{
					channel.frequency	 = pScanInfo->u.Cable.FrequencyKHertz+125; //ferench freq switch
				}
				else
				#endif
				{
					channel.frequency	 = pScanInfo->u.Cable.FrequencyKHertz; /* kHz */
					DB_TUNER_INFO((" dvbc.frequency =%d\n",channel.frequency));
				}
				/* program channel */
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CHANNEL, &channel ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(("failure: error programming desired channel  %d \n",__LINE__));
					tError = eTUNER_ERROR;
				}
				else
				{
					//if((Search_Get_Dialog_Flag()!=1)&&(SIMGR_If_AutoDetectQamSym()!=1))/*not in search diaog or not in auto check qam and symb*/
					{ 		
						while((tLockStatus!=DRX_LOCKED)&&(++ucLocktime<10))
						{
							if(DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus )!= DRX_STS_OK)
							{
								DB_TUNER_ERR(("failure: get lock states dvbc\n"));
							}

							OS_TaskDelay(50);
						}		
						if(DRX_LOCKED==tLockStatus)
						{
							DB_TUNER_INFO(("lock  dvbc\n"));
						}
						else
						{
							DB_TUNER_INFO(("unlock  dvbc\n"));				
						}
					}	
				}
			}
			break;
		case MEDIUM_TYPE_DVBT:
			{
				DRXLockStatus_t		tLockStatus = DRX_NEVER_LOCK;
				U8					ucLocktime = 0;
				channel.frequency = pScanInfo->u.Terrestrial.FrequencyKHertz; /* kHz */
				DB_TUNER_INFO((" dvbt.frequency =%d\n",channel.frequency));

				channel.bandwidth	  = pScanInfo->u.Terrestrial.Bandwidth;	   //modify 2010.05.11 fro differ bandwidth	
				DB_TUNER_INFO((" dvbt.bandwidth = %d \n",channel.bandwidth));
	
				channel.mirror 			= DRX_MIRROR_AUTO;
				channel.constellation	= DRX_CONSTELLATION_AUTO;
				channel.hierarchy		= DRX_HIERARCHY_AUTO;
				channel.priority		= DRX_PRIORITY_HIGH;
				channel.coderate		= DRX_CODERATE_AUTO;
				channel.guard 			= DRX_GUARD_AUTO;
				channel.fftmode			= DRX_FFTMODE_AUTO;
				channel.classification 	= DRX_CLASSIFICATION_AUTO;
				/* program channel */
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CHANNEL, &channel ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(( "failure: error programming desired channel %d \n",__LINE__));
					tError=eTUNER_ERROR;
				}
                else
                {
					//if((Search_Get_Dialog_Flag()!=1)&&(SIMGR_If_AutoDetectQamSym()!=1))/*not in search diaog or not in auto check qam and symb*/
					{ 		
						while((tLockStatus!=DRX_LOCKED)&&(++ucLocktime<10))
						{
							if(DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus )!= DRX_STS_OK)
							{
								DB_TUNER_ERR(("failure: get lock states dvbt\n"));
							}

							OS_TaskDelay(50);
						}		
						if(DRX_LOCKED==tLockStatus)
						{
							DB_TUNER_INFO(("lock  dvbt\n"));
						}
						else
						{
							DB_TUNER_INFO(("unlock  dvbt\n"));				
						}
					}	
				}
			}
			break;
		case MEDIUM_TYPE_ANALOG:
			{
				g_tRetLock=0;//add for check turner
				#if  0
				DRXAudStandard_t audioStandard = DRX_AUD_STANDARD_AUTO;//new add 
				DRXAudStatus_t audioStatus;
				#endif /* #if 0 */
				/*add by SuZhou 2010.5.31 {{{*/
				
				if(channel.frequency>=474000)
				{
					channel.bandwidth =DRX_BANDWIDTH_8MHZ;
				}
				else
				{
					channel.bandwidth =DRX_BANDWIDTH_7MHZ;
				}
				/*add by SuZhou 2010.5.31 }}}*/
				
				//memset(&audioStatus,0,sizeof(DRXAudStatus_t));

				channel.mirror = DRX_MIRROR_AUTO;
				channel.constellation = DRX_CONSTELLATION_AUTO;
				channel.hierarchy = DRX_HIERARCHY_AUTO;
				channel.priority = DRX_PRIORITY_HIGH;
				channel.coderate = DRX_CODERATE_AUTO;
				channel.guard	 = DRX_GUARD_AUTO;
				channel.fftmode	 = DRX_FFTMODE_AUTO;
				channel.classification = DRX_CLASSIFICATION_AUTO;

				//new add 2010.05.11 for center freq :pScanInfo->u.Analog.FrequencyKHertz is picture freq + audioband/2

				/*changed by SuZhou 因为模拟全频改成盲扫，盲扫频率不需加1/2伴音,否则播放不出。
				至于预设的搜索，需要加1/2伴音，在Pv_simger.c L:1615添加{{{*/
				//channel.frequency = pScanInfo->u.Analog.FrequencyKHertz+Tuner_Get_AudioFreqByCountry();
				
				channel.frequency = pScanInfo->u.Analog.FrequencyKHertz;
				
				/*Changed by SuZhou}}}*/
				
				//channel.bandwidth = DRX_BANDWIDTH_8MHZ;			
				DB_TUNER_INFO(("tuner_freq:%d^^^^^^^^^^^^^^^^\n",channel.frequency));
				/* program channel */
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CHANNEL, &channel ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(("failure: error programming desired channel %d \n",__LINE__));
					tError |=eTUNER_ERROR;
				}
				if ( DRX_Ctrl( &demod, DRX_CTRL_GET_CHANNEL, &channel ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(("failure: error programming desired channel %d \n",__LINE__));
					tError |=eTUNER_ERROR;
				}
				/* (3) apply the detected frequency */
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CHANNEL, &channel ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(("failure: error programming desired channel %d \n",__LINE__));
					tError |=eTUNER_ERROR;
				}
				////////////////////////addtest
#if 1
				/* start the audio detection */
				THOMSON_DrvAnalogStandDetec();//add test 2010.05.04		   
				/* read back the detected audio standard */

#if 0			
				if ( DRX_Ctrl( &demod, DRX_CTRL_AUD_SET_STANDARD, &audioStandard)!= DRX_STS_OK )
				{
					DB_TUNER(( "failure: error DRX_CTRL_AUD_GET_STANDARD\n"));
					tError=eTUNER_ERROR;
				}
#endif

#endif

#if 0			
				if (  DRX_Ctrl( &demod, DRX_CTRL_AUD_GET_STATUS, &audioStatus)) != DRX_STS_OK )
				{
					DB_TUNER(( "failure: error DRX_CTRL_AUD_SET_STANDARD\n")) ;
					tError=eTUNER_ERROR;
				}
				DB_TUNER(( "audioStatus.stereo=%d,audioStatus.nicamStatus=%d \n",
							audioStatus.stereo,audioStatus.nicamStatus)) ;
#endif

			}		
			break;
		default:
			break;

	}
	return tError;
}

/*********************************************************************************************************
 * Function 		:	  Tuner_GetFELockThomson
 * parameters 		:					
 * Return			:	 	
 * Description 		:	
 **********************************************************************************************************/
void Tuner_GetFELockThomson(U8 * tRetLock )
{
	DRXLockStatus_t		tLockStatus=DRX_NEVER_LOCK;
	//U8  					tempstatus=0;


	switch(ucSetMediumType)
	{
		case MEDIUM_TYPE_DVBC:
		case MEDIUM_TYPE_DVBT: 
			{
				DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus );	 

				if(DRX_LOCKED==tLockStatus)
				{
					*tRetLock=1;
				}	
				else
				{
					*tRetLock=0;
				}
			}
			break;
		case MEDIUM_TYPE_ANALOG:
			{

/*change by suzhou for blind_scan{{{*/
#if	NOTEXTCTRLCHECKTUNER
//使用全扫，设为1，盲扫，设为0
/*change by suzhou for blind_scan}}}*/

				DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus );	 
				if(tLockStatus==DRX_LOCKED)
				{
					*tRetLock = 1;
				}
				else
				{
					*tRetLock = 0;
				}

				THOMSON_DrvAnalogStandDetec();//add test 2010.05.04
				///////////////////////////////		   
#else
				//*tRetLock= EXTCTRL_CheckTuner_Analog();	//同步头
				#if 0/*scj del 110420*/
				if(0==EXTCTRL_CheckTuner_Analog(&tempstatus))
				{
					g_tRetLock=tempstatus;	
				}
				#endif
				if(1==g_tRetLock)
				{
					DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus );	 
					if(tLockStatus==DRX_LOCKED)	
						*tRetLock= 1;	//再次确认同步头
					else
						*tRetLock = 0;
				}
				else
				{
					*tRetLock = 0;
				}
#endif			 			 	
			}
			break;
		default:
			break;

	}
}

/*********************************************************************************************************
 * Function 		:	  THOMSON_DrvModeChange
 * parameters 		:	uiModeType				
 * Return			:	 	
 * Description 		:	tError
 **********************************************************************************************************/
U8 THOMSON_DrvModeChange(ts_src_info_t *pScanInfo)
{
	U8 	tRet=0;
	DRXStandard_t	 		standard=DRX_STANDARD_AUTO;
	switch(pScanInfo->MediumType)
	{
		case MEDIUM_TYPE_DVBT:
			{
#if 0
				DRXKCfgDvbtSqiSpeed_t sqiSpeed = DRXK_DVBT_SQI_SPEED_MEDIUM;
				DRXCfg_t config	= { (DRXCfgType_t) DRXK_CFG_DVBT_SQI_SPEED, NULL };

				config.cfgData = &sqiSpeed;

				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_CFG, &config ) != DRX_STS_OK )
				{
					printf( "failure: error re-configuring SQI speed\n" );
				}
				printf( "success: re-configured SQI speed\n" );
#endif
				/* select desired standard */
				standard = DRX_STANDARD_DVBT;
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_STANDARD, &standard ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(( "failure: error selecting standard %s\n",
								DRX_STR_STANDARD( standard ) ));
					tRet=1;

				}
				DB_TUNER_INFO(( "success: standard %s selected\n",
							DRX_STR_STANDARD( standard ) ));
			}
			break;

		case MEDIUM_TYPE_DVBC:
			{
				standard = DRX_STANDARD_ITU_A;
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_STANDARD, &standard ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(( "failure: error selecting standard %s\n",
								DRX_STR_STANDARD( standard ) ));
					tRet=1;

				}
				else
				{
					DB_TUNER_INFO(( "success: standard %s selected\n",
								DRX_STR_STANDARD( standard ) ));
				}
			}
			break;

		case MEDIUM_TYPE_ANALOG:
			{
				//standard = pScanInfo->u.Analog.AnalogPal;
				standard =(U8)Tuner_Get_CountryAnalogFormat();
				//standard=DRX_STANDARD_PAL_SECAM_DK;// 暂时
				if ( DRX_Ctrl( &demod, DRX_CTRL_SET_STANDARD, &standard ) != DRX_STS_OK )
				{
					DB_TUNER_ERR(("failure: error selecting standard %s\n",
								DRX_STR_STANDARD( standard )));
					tRet=1;			
				}
				else
				{
					DB_TUNER_INFO(("success: standard %s selected\n",DRX_STR_STANDARD( standard )));
				}
			}
			break;
		default:
			break;

	}

	return tRet;
}
/******************************************************************************
 * Function		: Tuner_GetFEInfoThomson_Search
 * Description 	: Get signal info
 * Input	   	: ptTunerInfo
 * Output	  	:
 * Return	  	: none
 ******************************************************************************/
void Tuner_GetFEInfoThomson_Search(Tun_Info_t *ptTunerInfo)
{
	DRXLockStatus_t		tLockStatus=DRX_NEVER_LOCK;
	u16_t			 		usgStrength = 0;
	DRXSigQuality_t   		tQuality;

	memset(&tQuality,0,sizeof(DRXSigQuality_t));
	DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus );
	if(DRX_LOCKED==tLockStatus)	
	{
		DRX_Ctrl( &demod, DRX_CTRL_SIG_QUALITY, &tQuality );
		DRX_Ctrl( &demod, DRX_CTRL_SIG_STRENGTH, &usgStrength );
		ptTunerInfo->SignalQuality=tQuality.indicator;
		ptTunerInfo->SignalStrength=usgStrength;
		ptTunerInfo->LockState=1;
	}
	else
	{
		ptTunerInfo->SignalQuality=0;
		ptTunerInfo->SignalStrength=0;
		ptTunerInfo->LockState=0;
	}
}
/******************************************************************************
 * Function		: Tuner_GetFEInfoAvlink2108
 * Description 	: Get signal info
 * Input	   	: ptTunerInfo
 * Output	  	:
 * Return	  	: none
 ******************************************************************************/
void		Tuner_GetFEInfoThomson(Tun_Info_t *ptTunerInfo)
{
	DRXLockStatus_t		tLockStatus=DRX_NEVER_LOCK;
	u16_t			 		usgStrength = 0;
	DRXSigQuality_t   		tQuality;
	DRXChannel_t	  		channel;

	memset(&tQuality,0,sizeof(DRXSigQuality_t));
	memset(&channel,0,sizeof(DRXChannel_t));
	DRX_Ctrl( &demod, DRX_CTRL_LOCK_STATUS, &tLockStatus );
	if(DRX_LOCKED==tLockStatus)	
	{

		DRX_Ctrl( &demod, DRX_CTRL_SIG_QUALITY, &tQuality );
		DRX_Ctrl( &demod, DRX_CTRL_SIG_STRENGTH, &usgStrength );
		DRX_Ctrl( &demod, DRX_CTRL_GET_CHANNEL, &channel );

		ptTunerInfo->SignalQuality =tQuality.indicator;
		ptTunerInfo->SignalStrength = usgStrength;
		ptTunerInfo->FrequencyKhz = (U32)channel.frequency;
		//ptTunerInfo->SignalQuality=tQuality.indicator;
		//ptTunerInfo->SignalStrength=usgStrength;
		ptTunerInfo->BitErrorRate = 0;
		//ptTunerInfo->BitErrorRate[1] = 0;
		//ptTunerInfo->BitErrorRate[2] =0;
		ptTunerInfo->LockState=1;

		DB_TUNER_INFO(("tQuality.indicator:%d\n",tQuality.indicator));
#if 0	
		switch(ptTunerInfo->MediumType)
		{
			case MEDIUM_TYPE_DVBT:
				{

					DRX_Ctrl( &demod, DRX_CTRL_SIG_STRENGTH, &usgStrength );
					DRX_Ctrl( &demod, DRX_CTRL_SIG_QUALITY, &tQuality );

					ptTunerInfo->tTun_SignalInfo.uiQualityValue =tQuality.indicator;
					ptTunerInfo->tTun_SignalInfo.uiStrengthValue = usgStrength;
					ptTunerInfo->uiQualityPercent=;
					ptTunerInfo->uiStrengthPercent=;
					ptTunerInfo->BitErrorRate[0] = 0;
					ptTunerInfo->BitErrorRate[1] = 0;
					ptTunerInfo->BitErrorRate[2] =0;
					ptTunerInfo->Lock=1;
				}
				break;
			case MEDIUM_TYPE_DVBC:
				{

					ptTunerInfo->tTun_SignalInfo.uiQualityValue = uiQuality/100;
					ptTunerInfo->tTun_SignalInfo.uiStrengthValue = usSignallevel/1000;
					ptTunerInfo->uiQualityPercent=uiQuality/100;
					ptTunerInfo->uiStrengthPercent=usSignallevel/1000;
					ptTunerInfo->BitErrorRate[0] = 0;
					ptTunerInfo->BitErrorRate[1] = 0;
					ptTunerInfo->BitErrorRate[2] =0;
					ptTunerInfo->Lock=1;
				}

				break;
			case MEDIUM_TYPE_ANALOG:
				{
					ptTunerInfo->tTun_SignalInfo.uiQualityValue = uiQuality/100;
					ptTunerInfo->tTun_SignalInfo.uiStrengthValue = usSignallevel/1000;
					ptTunerInfo->uiQualityPercent=uiQuality/100;
					ptTunerInfo->uiStrengthPercent=usSignallevel/1000;
					ptTunerInfo->BitErrorRate[0] = 0;
					ptTunerInfo->BitErrorRate[1] = 0;
					ptTunerInfo->BitErrorRate[2] =0;
					ptTunerInfo->Lock=1;
				}

				break;
		}

#endif
	}
	else
	{
		//ptTunerInfo->tTun_SignalInfo.uiQualityValue = 0;
		//ptTunerInfo->tTun_SignalInfo.uiStrengthValue = 0;
		ptTunerInfo->SignalQuality=0;
		ptTunerInfo->SignalStrength=0;
		ptTunerInfo->BitErrorRate = 0;
		//ptTunerInfo->BitErrorRate[1] = 0;
		//ptTunerInfo->BitErrorRate[2] =0;
		ptTunerInfo->LockState=0;
	}

	DB_TUNER_INFO(("uiQualityPercent=%d   uiStrengthPercent=%d \n",ptTunerInfo->SignalQuality,ptTunerInfo->SignalStrength));

}

/******************************************************************************
 * Function		: Tuner_GetChannelThomson
 * Description 	: detect the real freq
 * Input	   	: 
 * Output	  	:
 * Return	  	: none
 ******************************************************************************/
U32 Tuner_GetChannelThomson(void)
{
	DRXChannel_t	  		channel;


	memset(&channel,0,sizeof(DRXChannel_t));
	/* program channel */
	if ( DRX_Ctrl( &demod, DRX_CTRL_GET_CHANNEL, &channel ) != DRX_STS_OK )
	{
		DB_TUNER_ERR(("failure: error programming desired channel %d \n",__LINE__));
	}
	DB_TUNER_INFO(("\n get _tuner_freq:%d^^^^^^^^^^^^^^^^\n",channel.frequency ));

	return (U32)channel.frequency;
}


/******************************************************************************
 * Function		: Tuner_GetChannelThomson
 * Description 	: detect the real freq
 * Input	   	: 
 * Output	  	:
 * Return	  	: none
 ******************************************************************************/
S32 Tuner_Get_AudioFreqByCountry(void)
{
	S32 siAudioFreq=0;

	switch(uiCountryIndex)
	{
#if 0
		case GER:
			siAudioFreq=2250;	 
			break;
		case GBR:
			siAudioFreq=2750;
			break;

		case FRA:
			siAudioFreq=2750;		  
			break;

		case ITA:
			siAudioFreq=2250;		   
			break;

		case POL:
			siAudioFreq=2750;	 

			break;

		case RUS:
			siAudioFreq=2750;		
			break;

		case ESP:
			siAudioFreq=2750;	 
			break;

		case SWE:
			siAudioFreq=2750;				 
			break;

		case UAE:
			siAudioFreq=2750;			 
			break;

		case DEN:
			siAudioFreq=2750;		 
			break;

		case NED:
			siAudioFreq=2750;			
			break;

		case FIN:
			siAudioFreq=2750;	 

			break;

		case HUN:

			siAudioFreq=2750;	   
			break;

		case NOR:
			siAudioFreq=2750;			   
			break;

		case POR:
			siAudioFreq=2750;	 
			break;
		case GRE:
			siAudioFreq=3250; //GRE暂时用作中国	  
			break;
#endif
		default:

			break;
	}
	return siAudioFreq;
}

/******************************************************************************
 * Function		: Tuner_Get_CountryAnalogFormat
 * Description 	: get the analog audio format by country
 * Input	   	: 
 * Output	  	:
 * Return	  	: none
 ******************************************************************************/
DRXStandard_t Tuner_Get_CountryAnalogFormat(void)
{

	DRXStandard_t	 	tStandard=DRX_STANDARD_AUTO;
#if 0
	Country_t		   tCurrentCounty=GER;

	// tCurrentCounty=(Country_t)Dialog_GetCountryIndex();  //DB_SYS_Config_Get(CONFIG_SYSTEMS_SETTINGS_COUNTRY);
	switch(uiCountryIndex)
	{
		case GER:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;	 
			break;
		case GBR:
			tStandard=DRX_STANDARD_PAL_SECAM_I;
			break;

		case FRA:
			tStandard=DRX_STANDARD_PAL_SECAM_L;		  
			break;

		case ITA:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;		   
			break;

		case POL:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;	 

			break;

		case RUS:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;		
			break;

		case ESP:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;	 
			break;

		case SWE:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;				 
			break;

		case UAE:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;			 
			break;

		case DEN:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;		 
			break;

		case NED:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;			
			break;

		case FIN:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;	 

			break;

		case HUN:

			tStandard=DRX_STANDARD_PAL_SECAM_BG;	   
			break;

		case NOR:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;			   
			break;

		case POR:
			tStandard=DRX_STANDARD_PAL_SECAM_BG;	 
			break;
		case GRE:
			tStandard=DRX_STANDARD_PAL_SECAM_DK; //GRE暂时用作中国	 

			break;
		default:


			break;
	}

#endif
	return tStandard;

}

/******************************************************************************
 * Function		: THOMSON_DrvAnalogStandDetec
 * Description 	: detect the analog standard
 * Input	   	: 
 * Output	  	:
 * Return	  	: DRXAudStandard_t
 ******************************************************************************/
DRXAudStandard_t THOMSON_DrvAnalogStandDetec(void)
{
	DRXAudStandard_t audioStandard = DRX_AUD_STANDARD_AUTO;//new add 
#ifdef TUNERDETECT//add for detect tuner
	U8	ucI=0;
	if ( DRX_Ctrl( &demod, DRX_CTRL_AUD_SET_STANDARD, &audioStandard)!= DRX_STS_OK )
	{
		DB_TUNER(( "failure: error DRX_CTRL_AUD_SET_STANDARD\n")) ;
	}

	audioStandard=DRX_AUD_STANDARD_NOT_READY;

	while((DRX_AUD_STANDARD_NOT_READY==audioStandard)&&(ucI++<50))
	{
		OS_TaskDelay(20);
		/* read back the detected audio standard */
		if ( DRX_Ctrl( &demod, DRX_CTRL_AUD_GET_STANDARD, &audioStandard)!= DRX_STS_OK )
		{
			pbierror("failure: error DRX_CTRL_AUD_GET_STANDARD\n");
		}
	}
	pbiinfo("\n=========The detected standard is %d==========\n",audioStandard);

#endif
	return audioStandard;

}



////////////////////////////////////
#if 0
U8 GetLockStates(void)
{ 
	U8 rData=0;
	U32 ctrlData=1;

	DRXK_Ctrl(  &demod,DRX_CTRL_I2C_BRIDGE,&ctrlData );


	rData=TUNER_CANNED_GetStatus( demod.myTuner );

	ctrlData=0;
	DRXK_Ctrl(  &demod,DRX_CTRL_I2C_BRIDGE, &ctrlData );

	return rData;
}
#endif

