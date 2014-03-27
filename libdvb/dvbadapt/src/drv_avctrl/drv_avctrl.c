/******************************************************************************
 *	(c)	Copyright Pro. Broadband Inc. PVware
 * 
 * File name  :drv_avctrl.c 
 * Description: 
 * 
 *     
 * History :
 *	Date               Modification                                Name
 * --------------------------------------------------
 * 2010/06/08	created						zheng dong sheng
 *  --------------------------------------------------*
******************************************************************************/

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "osapi.h"
#include "pbitrace.h"
#include "drv_avctrl.h"
#include "inject_data.h" 
#include <sys/system_properties.h>
/*******************************************************/
/*               Private Defines and Macros			                   */
/*******************************************************/

#define INVAILD_PID					0x1FFF
#define SND_MIX_WEIGHT_MAX		100
#define SND_MIX_WEIGHT_MIN		0
#define AVCTRL_INFO(x)			printf x
#define AVCTRL_ERR(x)			printf x
#define AUDIO_MAX_PES_COUNT     0xFFFFFFFF
#define AUDIO_MID_PES_COUNT		0x1FFFFFFF
#define AUDIO_MIN_PES_COUNT		0

#define AV_DEV_NO   0
#define AOUT_DEV_NO AV_DEV_NO
#define VOUT_DEV_NO AV_DEV_NO

#define INJECT_PATH "/mnt/expand/Inject.mpg"
#define MAX_INJECT	1024 * 512
#define MIN_INJECT 	1024 * 256

U32 gVidPid = 0x1FFF;
U32 gAudPid = 0x1FFF;
AM_Bool_t gVOUT_Flag = AM_FALSE;

#define AM_TRY_VOID(_func) \
	do {\
	AM_ErrorCode_t _ret;\
	pbiinfo("[zshang][%s][%d]\n", __FUNCTION__, __LINE__);\
	if ((_ret=(_func))!=AM_SUCCESS){\
		pbiinfo("[zshang][%x]\n", _ret);\
		return;\
	}\
	} while(0)


/*******************************************************/
/*               Private Types						                   */
/*******************************************************/

/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/
//{{{added by LYN,2011.02.14,for Audio init
 #define SYS_PINSHARED_ADDR 0x10203000
 #define SIO_DEV_NUMBER 0
 
  #define det_vi0_tsi0_gpio 0x1a4
  #define MUTECTL_PINSHARED_OFFSET det_vi0_tsi0_gpio
  #define MUTECTL_MASK 0x3
  #define MUTECTL_BITS 0x3
  #define MUTECTL_GPIO_NUM (8 * 12 + 5)     /* GPIO12_5 */
  /* ALL chip is same for SPDIF OUT */
#define spdif_gpio 0x40
#define SPDIF_PINSHARED_OFFSET spdif_gpio
#define SPDIF_MASK 0x3
#define SPDIF_BITS 0x1

#define DACMODE0 HI_UNF_DISP_DAC_MODE_HD_PR
#define DACMODE1 HI_UNF_DISP_DAC_MODE_HD_Y
#define DACMODE2 HI_UNF_DISP_DAC_MODE_HD_PB
#define DACMODE3 HI_UNF_DISP_DAC_MODE_SVIDEO_C
#define DACMODE4 HI_UNF_DISP_DAC_MODE_SVIDEO_Y
#define DACMODE5 HI_UNF_DISP_DAC_MODE_CVBS


/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/
static DRV_AVCtrl_VIDFormat_t g_eVidFmt = AVCTRL_VID_FORMAT_BULL;
static DRV_AVCtrl_AudFormat_t g_eAudFmt = AVCTRL_AUD_FORMAT_BULL;
static DRV_AVCtrl_Vid_Stop_Mode_t g_eVidStopMode = AVCTRL_VID_STOP_MODE_BLACK;
static DRV_AVCTRL_EVENT_CALLBACK_FUN FDRV_AVCTRL_CallBack_Fun = NULL;

static U8 g_AVCtrlInitFlag = 0; /* add by zxguan use init avctrl again */
static U32 g_UserDemux = 10;
static U8* gInjectBuff = NULL;//add zshang 20130906
static U32 gInjectLen = 0;//add zshang 20130906
static U32 dal_VideoFrameCount = 0;
/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/
static void _DRV_AVCTRL_RecAudFmt( DRV_AVCtrl_AudFormat_t eAudFmt );
static void _DRV_AVCTRL_RecVidFmt( DRV_AVCtrl_VIDFormat_t eVidFmt );


static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AvPlay_Init( U32 DemuxId );
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AvPlay_UnInit( void );
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AudSnd_Init( void );
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AudSnd_UnInit( void );
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_VidOut_Init();
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_VidOut_UnInit();
static DRV_AVCtrl_ErrorCode_t   _DRV_AVCtrl_Regist_Event( void );

/*******************************************************/
/*               Functions							                   */
/*******************************************************/
/******************************************************************************
* Function :_DRV_AVCTRL_RecAudFmt
* Parameters : 
*			DRV_AVCtrl_AudFormat_t sAudFmt
* Return :
*			NULL
* Description :
*                   The function is used to record global audio format.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
*****************************************************************************/

static void _DRV_AVCTRL_RecAudFmt( DRV_AVCtrl_AudFormat_t eAudFmt )
{
	g_eAudFmt = eAudFmt;
}

/******************************************************************************
* Function :_DRV_AVCTRL_GetAudFmt
* Parameters : 
*			NULL
* Return :
*			 DRV_AVCtrl_AudFormat_t g_eAudFmt 
* Description :
*                   The function is used to get global audio format.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
*****************************************************************************/

DRV_AVCtrl_AudFormat_t _DRV_AVCTRL_GetAudFmt( void )
{
	return g_eAudFmt;
}

/******************************************************************************
* Function :_DRV_AVCTRL_RecVidFmt
* Parameters : 
*			DRV_AVCtrl_VIDFormat_t eVidFmt
* Return :
*			NULL
* Description :
*                   The function is used to record global video format.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
*****************************************************************************/

static void _DRV_AVCTRL_RecVidFmt( DRV_AVCtrl_VIDFormat_t eVidFmt )
{
	g_eVidFmt = eVidFmt;
}

/******************************************************************************
* Function :_DRV_AVCTRL_GetVidFmt
* Parameters : 
*			NULL
* Return :
*			 DRV_AVCtrl_VIDFormat_t g_eVidFmt
* Description :
*                   The function is used to get global video format.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
*****************************************************************************/

DRV_AVCtrl_VIDFormat_t _DRV_AVCTRL_GetVidFmt( void )
{
	return g_eVidFmt;
}
/******************************************************************************
* Function :_DRV_AVCTRL_AvPlay_Init
* Parameters : 
*			nothing
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to initialize av play module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14,	for His3716M adapting.
* --------------------
*****************************************************************************/

static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AvPlay_Init( U32 DemuxID )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	AM_AV_OpenPara_t para;
	char dispMode[16] = "720p";
	PBIDEBUG("AV Open");
	memset(&para, 0, sizeof(para));
	ErrCode = AM_AV_Open(AV_DEV_NO, &para);
	if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("AM_AV_Open error!");
        return AVCTRL_ERROR;
    }
    ErrCode = AM_AV_SetVPathPara (AV_DEV_NO, AM_AV_FREE_SCALE_DISABLE, AM_AV_DEINTERLACE_ENABLE, AM_AV_PPMGR_DISABLE);
	if (ErrCode != AM_SUCCESS)
    {
        pbierror("call AM_AV_SetVPathPara failed.[0x%x].\n",ErrCode);
        //return AVCTRL_ERROR;
    }
#if 1//�ڳ���720p ����DVB�»���ְױ�?��Ҫ�ӳٵȴ�AM_AV_SetVPathPara ִ�����?
    //property_get("ubootenv.var.outputmode",dispMode,"720p");
   // if(strncmp(dispMode, "720p", 4))    
    	usleep(1000*750);
#endif
#if 0
    ErrCode = DRV_AVCtrl_Vid_EnableOutput();
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("DRV_AVCtrl_Vid_EnableOutput error!");
        return AVCTRL_ERROR;
    }
    ErrCode = DRV_AVCtrl_Aud_EnableOutput();
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("DRV_AVCtrl_Aud_EnableOutput error!");
        return AVCTRL_ERROR;
    }
#else
	ErrCode = _DRV_AVCTRL_AudSnd_Init();
    if( ErrCode != AM_SUCCESS )
    {
    	pbiinfo("[%s %d]  _DRV_AVCTRL_AudSnd_Init error!",DEBUG_LOG);
    	return AVCTRL_ERROR;
    } 
    ErrCode = _DRV_AVCTRL_VidOut_Init();
    if( ErrCode != AM_SUCCESS )
    {
    	pbiinfo("[%s %d]  _DRV_AVCTRL_VidOut_Init error!",DEBUG_LOG);
    	return AVCTRL_ERROR;
    }       
#endif   
	ErrCode = DRV_AVCtrl_Regist_AllEvent();
	if( ErrCode != AM_SUCCESS )
    {
    	pbierror("call DRV_AVCtrl_Regist_AllEvent failed.[0x%x].\n",ErrCode);
    }
	PBIDEBUG("AM_AV_SetTSSource");
	ErrCode = AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_TS2);
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("AM_AV_SetTSSource error!");
        return AVCTRL_ERROR;
    }
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :_DRV_AVCTRL_AvPlay_UnInit
* Parameters : 
*			nothing
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to uninitialize avplay module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,	2011/02/14	for His3716M adapting
* --------------------
*****************************************************************************/

static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AvPlay_UnInit( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_SetVPathPara (AV_DEV_NO, AM_AV_FREE_SCALE_ENABLE, AM_AV_DEINTERLACE_DISABLE, AM_AV_PPMGR_ENABLE);
	if (ErrCode != AM_SUCCESS)
    {
        pbierror("call AM_AV_SetVPathPara failed.[0x%x].\n",ErrCode);
        //return AVCTRL_ERROR;
    }	
#if 0
	ErrCode = DRV_AVCtrl_Vid_DisableOutput();
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("DRV_AVCtrl_Vid_DisableOutput error!");
        return AVCTRL_ERROR;
    }
    ErrCode = DRV_AVCtrl_Aud_DisableOutput();
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("DRV_AVCtrl_Aud_DisableOutput error!");
        return AVCTRL_ERROR;
    }
#else	
	ErrCode = _DRV_AVCTRL_AudSnd_UnInit();
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\nsound module uninit error \n");
		return AVCTRL_ERROR;
	}

	 ErrCode = _DRV_AVCTRL_VidOut_UnInit();
	 if( ErrCode != AM_SUCCESS )
	{
		pbierror("\nvideo out module uninit error \n");
		return AVCTRL_ERROR;
	}
	ErrCode = AM_AV_Close(AV_DEV_NO);
    if( AM_SUCCESS != ErrCode )
    {
        PBIDEBUG("AM_AV_Close error!");
        return AVCTRL_ERROR;
    }	
#endif    
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :_DRV_AVCTRL_AudSnd_Init
* Parameters : 
*			nothing
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to initialize aud out module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14,	for His3716M adapting.
* --------------------
*****************************************************************************/

static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AudSnd_Init( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	AM_AOUT_OpenPara_t aout_para;
	memset(&aout_para,0,sizeof(aout_para));
	ErrCode = AM_AOUT_Open(AOUT_DEV_NO, &aout_para);
	if (ErrCode != AM_SUCCESS )
	{
	    pbierror("call AM_AOUT_Open failed.\n");
	    return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :_DRV_AVCTRL_AudSnd_DeInit
* Parameters : 
*			nothing
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to uninitialize aud out module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14	for His3716M adapting
* --------------------
*****************************************************************************/

static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_AudSnd_UnInit( void )
{

	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AOUT_Close(AOUT_DEV_NO);
	if (ErrCode != AM_SUCCESS )
	{
		pbierror("call AM_AOUT_Close failed.\n");
	    return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :_DRV_AVCTRL_VidOut_Init
* Parameters : 
*			HI_UNF_VO_DEV_MODE_E enDevMode
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to initialize video out module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14,	for His3716M adapting.
* --------------------
*****************************************************************************/
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_VidOut_Init()
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	AM_VOUT_OpenPara_t vout_para;
	memset(&vout_para,0,sizeof(vout_para));
	ErrCode = AM_VOUT_Open(VOUT_DEV_NO, &vout_para);
	if (ErrCode != AM_SUCCESS )
	{
	    pbierror("call AM_VOUT_Open failed.\n");
	    return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}    

/******************************************************************************
* Function :_DRV_AVCTRL_VidOut_UnInit
* Parameters : 
*			HI_UNF_VO_DEV_MODE_E enDevMode
* Return :
*			DRV_AVCtrl_ErrorCode_t	ErrCode
* Description :
*                   The function is used to uninitialize video out module.
* Author : 
*                   dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14	for His3716M adapting.
* --------------------
*****************************************************************************/
static DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_VidOut_UnInit()
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_VOUT_Close(VOUT_DEV_NO);
	if (ErrCode != AM_SUCCESS )
	{
		pbierror("call AM_VOUT_Close failed.\n");
	    return AVCTRL_ERROR;
	}
	 return AVCTRL_NO_ERROR;
}

static int  DRV_AVCTRL_GetSysFs_Str(const char *path, char *valstr, int size)
{
	int fd;
	fd = open(path, O_RDONLY);
	if (fd >= 0) 
	{
		memset(valstr,0,size);
		read(fd, valstr, size - 1);
		valstr[strlen(valstr)] = '\0';
		close(fd);
	}
	else
	{
		pbierror("unable to open file %s\n", path);
		valstr = '\0';
		return -1;
	};
	pbiinfo("get_sysfs_str=%s\n", valstr);
	return 0;
}


DRV_AVCtrl_ErrorCode_t _DRV_AVCTRL_VO_CreatWin(U32 x,U32 y,U32 w,U32 h)
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	char dispMode[16] = "720p";
	char disModePath[] = "/sys/class/display/mode";
	DRV_AVCTRL_GetSysFs_Str(disModePath, dispMode, 16);
//	property_get("ubootenv.var.outputmode",dispMode,"720p");
	x = 0;
	y = 0;
	if(!strncmp(dispMode, "480", 3))
	{
		w = 720;
		h = 480;
	}
	else if(!strncmp(dispMode, "576", 3))
	{
		w = 720;
		h = 576;
	}
	else if(!strncmp(dispMode, "720", 3))
	{
		w = 1280;
		h = 720;
	}
	else if(!strncmp(dispMode, "1080", 4))
	{
		w = 1920;
		h = 1080;
	}
	else
	{
		w = 1920;
		h = 1080;
	}
	ErrCode = AM_AV_SetVideoWindow (AV_DEV_NO, x, y, w, h);
	if (ErrCode != AM_SUCCESS)
    {
        pbierror("call AM_AV_SetVideoWindow failed.[0x%x].\n",ErrCode);
        return ErrCode;
    }
    return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_ErrorCode_t DRV_AV_Init( void )
{
    DRV_AVCtrl_ErrorCode_t ErrCode = AVCTRL_NO_ERROR;
   /* ErrCode = _DRV_AVCTRL_VidOut_Init();
    if( ErrCode != AVCTRL_NO_ERROR )
    {
    	pbiinfo("[%s %d]  _DRV_AVCTRL_VidOut_Init error!",DEBUG_LOG);
    	return AVCTRL_ERROR;
    }

    ErrCode = _DRV_AVCTRL_AudSnd_Init();
    if( ErrCode != AVCTRL_NO_ERROR )
    {
    	pbiinfo("[%s %d]  _DRV_AVCTRL_AudSnd_Init error!",DEBUG_LOG);
    	return AVCTRL_ERROR;
    }*/

	DRV_AVCtrl_Inject_Init(INJECT_PATH);
    PBIDEBUG("DRV_AV_Init ok");
    return AVCTRL_NO_ERROR;
}


U8 DRV_AVCtrl_InitFlag()
{
    return g_AVCtrlInitFlag;
}

U8 Drv_AVCtrl_ModeType()
{
    switch( g_UserDemux )
    {
        case 0:
        {
            return 0;
        }break;
        case 4:
        {
            return 1;
        }break;
        default:
        {
            return 0;
        }
    }

    return 0;
}

U32 *DRV_AVCtrl_GetAVHandle()
{
	return NULL;
}


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PVR_Stop()
{
    DRV_AVCtrl_Vid_Set_StopMode(AVCTRL_VID_STOP_MODE_STILL);
    DRV_AVCtrl_Vid_Stop();
	DRV_AVCtrl_Aud_Stop();	
    _DRV_AVCTRL_RecAudFmt( AVCTRL_AUD_FORMAT_BULL);
    _DRV_AVCTRL_RecVidFmt( AVCTRL_VID_FORMAT_BULL );
	usleep(100);
    DRV_AVCtrl_Vid_Set_StopMode(AVCTRL_VID_STOP_MODE_BLACK);

    return AVCTRL_NO_ERROR;
}
/******************************************************************************
* Function :DRV_AVCtrl_Init
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to initialize AV module.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN, 2011/02/14 ,for Hi3716M adapting.
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Init( U32 DemuxId )
{
	DRV_AVCtrl_ErrorCode_t ErrCode = AVCTRL_NO_ERROR;

    if( 1 == g_AVCtrlInitFlag )
    {
        pbiinfo("[%s %d], Drv AV Ctrl Init again!", DEBUG_LOG);
        return AVCTRL_NO_ERROR;
    }
      
    PBIDEBUG("DRV_AVCtrl_Init");

	ErrCode = _DRV_AVCTRL_AvPlay_Init( DemuxId );
	if( ErrCode != AVCTRL_NO_ERROR )
	{
		pbierror("\nAV module init error \n");
		return AVCTRL_ERROR;
	}

	DRV_AVCtrl_Regist_AllEvent();

	ErrCode = _DRV_AVCTRL_VO_CreatWin(0, 0, 1280, 720);
	if(ErrCode != AVCTRL_NO_ERROR)
	{
		pbierror("\nvideo out Creat window error \n");
		return AVCTRL_ERROR;
	}
	
    ErrCode = AM_AV_SetVideoDisplayMode(AV_DEV_NO, AM_AV_VIDEO_DISPLAY_FULL_SCREEN);
	if(ErrCode != AVCTRL_NO_ERROR)
	{
		pbierror("\n AM_AV_SetVideoDisplayMode error \n");
		return AVCTRL_ERROR;
	}
	    
    g_AVCtrlInitFlag = 1; 
    PBIDEBUG("DRV_AVCtrl_Init");
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_UnInit
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to uninitialize AV module.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN, 2011/02/14	for His3716M adapting
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_UnInit( void )
{
	DRV_AVCtrl_ErrorCode_t ErrCode = AVCTRL_NO_ERROR;

	PBIDEBUG("DRV_AVCtrl_UnInit");
	if( 0 == g_AVCtrlInitFlag )
	{
	    pbiinfo("[%s %d], Drv AV Ctrl UnInit again!", DEBUG_LOG );
	    return AVCTRL_NO_ERROR;
	}
	DRV_AVCtrl_Vid_Stop();
	DRV_AVCtrl_Aud_Stop();
	_DRV_AVCTRL_RecAudFmt( AVCTRL_AUD_FORMAT_BULL);
	_DRV_AVCTRL_RecVidFmt( AVCTRL_VID_FORMAT_BULL );	

	ErrCode = _DRV_AVCTRL_AvPlay_UnInit();
	if( ErrCode != AVCTRL_NO_ERROR )
	{
		pbierror("\navplay module uninit error \n");
		return AVCTRL_ERROR;
	}
    g_AVCtrlInitFlag = 0;
    PBIDEBUG("DRV_AVCtrl_UnInit Ok!!!!!!");
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Regist_CallBackFun
* Parameters : 
*                    DRV_AVCTRL_EVENT_CALLBACK_FUN FEvent_CallBack_Fun 
* Return :
*             NULL
* Description :
*                   The function is used to regist call back function.
* Author : dszheng     2010/07/16
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
void DRV_AVCtrl_Regist_CallBackFun( DRV_AVCTRL_EVENT_CALLBACK_FUN FEvent_CallBack_Fun )
{
}

char* DRV_AVCtrl_EventToStr (int event_type)
{
	switch(event_type)
	{
		case AM_AV_EVT_PLAYER_STATE_CHANGED: return  "AM_AV_EVT_PLAYER_STATE_CHANGED";
		case AM_AV_EVT_PLAYER_SPEED_CHANGED: return  "AM_AV_EVT_PLAYER_SPEED_CHANGED";
		case AM_AV_EVT_PLAYER_TIME_CHANGED: return  "AM_AV_EVT_PLAYER_TIME_CHANGED";
		case AM_AV_EVT_PLAYER_UPDATE_INFO: return  "AM_AV_EVT_PLAYER_UPDATE_INFO";
		case AM_AV_EVT_VIDEO_WINDOW_CHANGED: return  "AM_AV_EVT_VIDEO_WINDOW_CHANGED";
		case AM_AV_EVT_VIDEO_CONTRAST_CHANGED: return  "AM_AV_EVT_VIDEO_CONTRAST_CHANGED";
		case AM_AV_EVT_VIDEO_SATURATION_CHANGED: return  "AM_AV_EVT_VIDEO_SATURATION_CHANGED";
		case AM_AV_EVT_VIDEO_BRIGHTNESS_CHANGED: return  "AM_AV_EVT_VIDEO_BRIGHTNESS_CHANGED";
		case AM_AV_EVT_VIDEO_ENABLED: return  "AM_AV_EVT_VIDEO_ENABLED"; 
		case AM_AV_EVT_VIDEO_DISABLED: return  "AM_AV_EVT_VIDEO_DISABLED"; 
		case AM_AV_EVT_VIDEO_ASPECT_RATIO_CHANGED: return  "AM_AV_EVT_VIDEO_ASPECT_RATIO_CHANGED";
		case AM_AV_EVT_VIDEO_DISPLAY_MODE_CHANGED: return  "AM_AV_EVT_VIDEO_DISPLAY_MODE_CHANGED";
		case AM_AV_EVT_AV_NO_DATA: return  "AM_AV_EVT_AV_NO_DATA";
		case AM_AV_EVT_AV_DATA_RESUME: return  "AM_AV_EVT_AV_DATA_RESUME";
		case AM_AV_EVT_VIDEO_ES_END: return  "AM_AV_EVT_VIDEO_ES_END";
		case AM_AV_EVT_AUDIO_ES_END: return  "AM_AV_EVT_AUDIO_ES_END";
		case AM_AV_EVT_VIDEO_SCAMBLED: return  "AM_AV_EVT_VIDEO_SCAMBLED";
		case AM_AV_EVT_AUDIO_SCAMBLED: return  "AM_AV_EVT_AUDIO_SCAMBLED";
	}
	return "not found";
}

void DRV_AVCtrl_Event_CallBackFun(int dev_no, int event_type, void *param, void *data)
{
	pbiinfo("[zshang] dev_no[%d] event_type[%s] param[%x]", dev_no, DRV_AVCtrl_EventToStr(event_type), param);
}

/******************************************************************************
* Function :DRV_AVCtrl_Regist_AllEvent
* Parameters : 
*                    DRV_AVCTRL_EVENT_CALLBACK_FUN FEvent_CallBack_Fun 
* Return :
*             NULL
* Description :
*                   The function is used to regist av event.
* Author : zshang     2013/04/03
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
AM_ErrorCode_t DRV_AVCtrl_Regist_AllEvent( )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	int i = AM_AV_EVT_PLAYER_STATE_CHANGED;
	for(i = AM_AV_EVT_PLAYER_STATE_CHANGED; i < AM_AV_EVT_END; i++)
	{
		ErrCode |= AM_EVT_Subscribe(AV_DEV_NO, i, DRV_AVCtrl_Event_CallBackFun, NULL);
	}
	return ErrCode;
}

/******************************************************************************
* Function :DRV_AVCtrl_Clear_Mpg
* Parameters : 
*                    void  
* Return :
*             NULL
* Description :
*                   The function is used to clear video.
* Author : zshang     2013/07/31
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

void DRV_AVCtrl_Clear_Mpg()
{
	//AM_TRY_VOID(AM_AV_ClearVideoBuffer(AV_DEV_NO));	
	AM_AV_InjectPara_t para;
	AM_TRY_VOID(AM_AV_ClearVideoBuffer (AV_DEV_NO));
	memset(&para, 0, sizeof(AM_AV_InjectPara_t));
	para.pkg_fmt = PFORMAT_ES;
	AM_TRY_VOID(AM_AV_StartInject(AV_DEV_NO, &para));
	AM_TRY_VOID(AM_AV_StopInject(AV_DEV_NO));	
}

/******************************************************************************
* Function :DRV_AVCtrl_Inject_Init
* Parameters : 
*                    void  
* Return :
*             NULL
* Description :
*                   The function is used to Init.
* Author : zshang     2013/07/31
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

void DRV_AVCtrl_Inject_Init(const char *file)
{
	int iRet = 0, fsize = 0;
	FILE *fd = NULL;
	if(gInjectBuff != NULL)
		return;
	if( file != NULL )
	{
		iRet = access(file, F_OK | R_OK);
		if( 0 != iRet )
		{
			pbiinfo("[%s %d] File Not Find or File Can Not Read.\n",__FUNCTION__,__LINE__);
			goto End;
		}

		fd = fopen(file, "rb");
	    if (fd == -1)
	    {
	    	pbiinfo("[%s %d] File Can Not Open.\n",__FUNCTION__,__LINE__);
	        goto End;
	    }
		fseek(fd, 0, SEEK_END);
		fsize = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		if(fsize == 0)
		{
			pbiinfo("[%s %d] File Size is 0.\n",__FUNCTION__,__LINE__);
			fclose(fd);
			goto End;
		}
		
		gInjectBuff = (U8 *)malloc(fsize);
		if(gInjectBuff == NULL)
		{
			pbiinfo("[%s %d] gInjectBuff malloc is NULL.size[%d]\n",__FUNCTION__,__LINE__, fsize);
			fclose(fd);
		    goto End;
		}
		iRet = fread(gInjectBuff, 1, fsize, fd);
		if(iRet != fsize)
		{
			pbiinfo("[%s %d] gInjectBuff read iRet[%d] size[%d].\n",__FUNCTION__,__LINE__, iRet, fsize);
			fclose(fd);
			free(gInjectBuff);
		    goto End;
		}
		gInjectLen = fsize;
		pbiinfo("File [%s %d] gInjectBuff[%p] gInjectLen[%d].\n",__FUNCTION__,__LINE__, gInjectBuff, gInjectLen);
		fclose(fd);
		return;
	}
End:
	gInjectBuff = inject_data_buf;
	gInjectLen = inject_data_len;
	pbiinfo("Data [%s %d] gInjectBuff[%p] gInjectLen[%d].\n",__FUNCTION__,__LINE__, gInjectBuff, gInjectLen);
}


/******************************************************************************
* Function :DRV_AVCtrl_Inject_Mpg
* Parameters : 
*                    file name  
* Return :
*             NULL
* Description :
*                   The function is used to play small file stream.
* Author : zshang     2013/07/31
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

void DRV_AVCtrl_Inject_Mpg()
{
	AM_AV_InjectPara_t para;
	int iRet = 0, send = 0, icount = 0, left = 0, len = 0;
	static U8 buf[32*1024];
	DRV_AVCtrl_Vid_Stop_Mode_t stop_mode;
	pbiinfo("DRV_AVCtrl_Inject_Mpg\n");
	if(gInjectBuff == NULL || gInjectLen == 0)
		return;
		
	memset(&para, 0, sizeof(AM_AV_InjectPara_t));
	para.pkg_fmt = PFORMAT_ES;
	AM_TRY_VOID(AM_AV_StartInject(AV_DEV_NO, &para));
	AM_TRY_VOID(AM_DMX_SetSource(0, 3));
	AM_TRY_VOID(AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_HIU));
	DRV_AVCtrl_Vid_Get_StopMode(&stop_mode);
	gInjectLen = (gInjectLen > MAX_INJECT) ? MAX_INJECT : gInjectLen;
	icount = (MIN_INJECT / gInjectLen) + 1;	
	pbiinfo("DRV_AVCtrl_Inject_Mpg2: gInjectLen:[%d] icount:[%d] \n", gInjectLen, icount);
	while(icount)
	{
		len = sizeof(buf)-left;
		if(len){
			if(iRet + len > gInjectLen)
				len = gInjectLen - iRet;
			memcpy(buf+left, gInjectBuff + iRet, len);
			iRet += len;
			left += len;
			if(iRet >= gInjectLen){
				iRet = 0;
				icount--;
			}
		}
		else
		{
			pbiinfo( "DRV_AVCtrl_Inject_Mpg len 0\n");
			break;
		}
		
		if(left)
		{
			send = left;
			AM_AV_InjectData(AV_DEV_NO, AM_AV_INJECT_MULTIPLEX, buf, &send, -1);
			if(send)
			{
				left -= send;
				if(left)
					memmove(buf, buf+send, left);
			}
			else
			{
				pbiinfo( "inject 0 bytes\n");
			}
		}
		usleep(1000*20);
	}	
	AM_TRY_VOID(AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_TS2));
	AM_TRY_VOID(AM_DMX_SetSource(0, 2));	
	AM_TRY_VOID(AM_AV_DisableVideoBlackout(AV_DEV_NO));	
	AM_TRY_VOID(AM_AV_StopInject(AV_DEV_NO));	
	switch(stop_mode)
	{
		case AVCTRL_VID_STOP_MODE_BLACK:
		AM_TRY_VOID(AM_AV_EnableVideoBlackout(AV_DEV_NO));		
		break;
		case AVCTRL_VID_STOP_MODE_STILL:
		AM_TRY_VOID(AM_AV_DisableVideoBlackout(AV_DEV_NO));
		break;
	}
}


/******************************************************************************
* Function :DRV_AVCtrl_Vid_Start
* Parameters : 
*                    DRV_AVCtrl_VIDFormat_t eVidFmt
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to decode video.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t  DRV_AVCtrl_Vid_Start( DRV_AVCtrl_VIDFormat_t eVidFmt )
{

	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	pbiinfo("eVidFmt:%d AudFmt:%d [%d][%d]\n",eVidFmt, _DRV_AVCTRL_GetAudFmt(), gVidPid, gAudPid);
	switch(eVidFmt)
	{
		case AVCTRL_VID_FORMAT_MPEG2:
		eVidFmt = 0;
		break;
		case AVCTRL_VID_FORMAT_MPEG4:
		eVidFmt = 1;
		break;
		case AVCTRL_VID_FORMAT_AVS:
		eVidFmt = 7;
		break;
		case AVCTRL_VID_FORMAT_H263:
		eVidFmt = 9;
		break;
		case AVCTRL_VID_FORMAT_H264:
		eVidFmt = 2;
		break;
		case AVCTRL_VID_FORMAT_REAL8:		
		case AVCTRL_VID_FORMAT_REAL9:
		eVidFmt = 4;
		break;
		case AVCTRL_VID_FORMAT_VC1:
		eVidFmt = 6;
		break;
		case AVCTRL_VID_FORMAT_DIVX3:
		eVidFmt = 8;
		break;
		break;
	}
	ErrCode = AM_AV_StartTS(AV_DEV_NO, gVidPid, gAudPid, eVidFmt, _DRV_AVCTRL_GetAudFmt());
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\nvideo play module start video decode error %x\n",ErrCode);
		return AVCTRL_ERROR;
	}
	_DRV_AVCTRL_RecVidFmt( eVidFmt );
    PBIDEBUG("DRV_AVCtrl_Vid_Start ok");
	return AVCTRL_NO_ERROR;
}


/******************************************************************************
* Function :DRV_AVCtrl_Vid_Set_StopMode
* Parameters : 
*          DRV_AVCtrl_Vid_Stop_Mode_t mode
* Return :
*          AVCTRL_NO_ERROR
* Description :
*          The function is used to set video stop mode. still or blackscreen
* Author : yjzhao     2011/03/07
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Set_StopMode( DRV_AVCtrl_Vid_Stop_Mode_t mode )
{
	g_eVidStopMode = mode;
	return AVCTRL_NO_ERROR;
}


/******************************************************************************
* Function :DRV_AVCtrl_Vid_Get_StopMode
* Parameters : 
*          DRV_AVCtrl_Vid_Stop_Mode_t *mode
* Return :
*          AVCTRL_NO_ERROR
* Description :
*          The function is used to get current video stop mode.
* Author : yjzhao     2011/03/07
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Get_StopMode( DRV_AVCtrl_Vid_Stop_Mode_t *mode )
{
	*mode = g_eVidStopMode;
	return AVCTRL_NO_ERROR;
}


/******************************************************************************
* Function :DRV_AVCtrl_Vid_Stop
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_NO_ERROR
*             AVCTRL_ERROR
* Description :
*                    The function is used to stop video decoder.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Stop( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	DRV_AVCtrl_Vid_Stop_Mode_t stop_mode;
	DRV_AVCtrl_Vid_Get_StopMode(&stop_mode);
	switch(stop_mode)
	{
		case AVCTRL_VID_STOP_MODE_BLACK:
		ErrCode = AM_AV_EnableVideoBlackout(AV_DEV_NO);
		if( ErrCode != AM_SUCCESS )
		{
			pbierror("AM_AV_EnableVideoBlackout error %d\n",ErrCode);
			return AVCTRL_ERROR;
		}
		break;
		case AVCTRL_VID_STOP_MODE_STILL:
		ErrCode = AM_AV_DisableVideoBlackout(AV_DEV_NO);
		if( ErrCode != AM_SUCCESS )
		{
			pbierror("AM_AV_DisableVideoBlackout error %d\n",ErrCode);
			return AVCTRL_ERROR;
		}
		break;
	}
	DRV_AVCtrl_Vid_SetPID(INVAILD_PID);
	ErrCode = AM_AV_StopTS(AV_DEV_NO);
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("AM_AV_StopTSVideo error %d\n",ErrCode);
		return AVCTRL_ERROR;
	}	
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetPID
* Parameters : 
*                    U32 uiVidPID
* Return :
*             AVCTRL_NO_ERROR
*             AVCTRL_ERROR
* Description :
*                    The function is used to set video pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetPID( U32 uiVidPID )
{
	gVidPid = uiVidPID;
	return AVCTRL_NO_ERROR;
}

U32 DRV_AVCtrl_Vid_GetPID( void )
{
	return gVidPid;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_ClearPID
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set invaild video pid for clearing video pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_ClearPID( void )
{
	gVidPid = INVAILD_PID;
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetIOWindow
* Parameters : DRV_AVCtrl_VidWindow_t tVidOut_Window
*
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set io window.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14  for His3716M adapting
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetIOWindow( DRV_AVCtrl_VidWindow_t tVidOut_Window )
{
	U32 uiActual_Width;
	U32 uiActual_Height;
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	uiActual_Width = tVidOut_Window.uX_Axis + tVidOut_Window.uWidth;
	uiActual_Height = tVidOut_Window.uY_Axis + tVidOut_Window.uHeight;
	pbierror("[zshang]DRV_AVCtrl_Vid_SetIOWindow[%d][%d][%d][%d][%d][%d]\n", uiActual_Width, uiActual_Height, tVidOut_Window.uBaseWidth, tVidOut_Window.uBaseHeight, tVidOut_Window.uX_Axis, tVidOut_Window.uY_Axis);
	if( ( uiActual_Width > tVidOut_Window.uBaseWidth && tVidOut_Window.uBaseWidth > 0 ) || ( uiActual_Height > tVidOut_Window.uBaseHeight && tVidOut_Window.uBaseWidth > 0 ) )
	{
		pbierror("\n[zshang]Actual width or height should be less than base width or height.\n");
		return AVCTRL_ERROR;
	}

	ErrCode = AM_AV_SetVideoWindow (AV_DEV_NO, tVidOut_Window.uX_Axis, tVidOut_Window.uY_Axis, tVidOut_Window.uWidth, tVidOut_Window.uHeight);
	if (ErrCode != AM_SUCCESS)
    {
        pbierror("call AM_AV_SetVideoWindow failed.[0x%x].\n",ErrCode);
        return AVCTRL_ERROR;
    }
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_GetIOWindow
* Parameters : DRV_AVCtrl_VidWindow_t* sVidOut_Window 
*
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to get io window.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN, 2011/02/14	for His3716M adapting
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetIOWindow( DRV_AVCtrl_VidWindow_t* tVidOut_Window  )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_GetVideoWindow (AV_DEV_NO, &(tVidOut_Window->uX_Axis), &(tVidOut_Window->uY_Axis), &(tVidOut_Window->uWidth), &(tVidOut_Window->uHeight));
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("call AM_AV_SetVideoWindow failed.[0x%x].\n",ErrCode);
		return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetAlpha
* Parameters : 
*                    U32 uiAlpha
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set video alpha.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14 	for His3716M adapting
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAlpha( U32 uiAlpha )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetAlpha
* Parameters : 
*                    U32* uiAlpha
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to get video alpha.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN, 2011/02/14 	for His3716M adapting
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAlpha( U32* uiAlpha )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetAspectRatio
* Parameters : 
*                    DRV_AVCtrl_VidARatio_t  sVidAsRatio
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set aspect ratio.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*Modified by LYN,	2011/02/14	for His3716M adapting
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAspectRatio( DRV_AVCtrl_VidARatio_t  eVidAsRatio )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_SetVideoAspectRatio (AV_DEV_NO, eVidAsRatio);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("call AM_AV_SetVideoAspectRatio failed.[0x%x].\n",ErrCode);
		return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_GetAspectRatio
* Parameters : 
*                    DRV_AVCtrl_VidARatio_t* sVidAsRatio
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to get aspect ratio.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,	2011/02/14	for His3716M adapting
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAspectRatio( DRV_AVCtrl_VidARatio_t*  eVidAsRatio )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	AM_AV_VideoAspectRatio_t VidAsRatio;
	ErrCode = AM_AV_GetVideoAspectRatio (AV_DEV_NO, &VidAsRatio);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("call AM_AV_SetVideoAspectRatio failed.[0x%x].\n",ErrCode);
		return AVCTRL_ERROR;
	}
	*eVidAsRatio = VidAsRatio;
	return AVCTRL_NO_ERROR;
}


/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetAspectCvrs
* Parameters : 
*                    DRV_AVCtrl_VIDACvrs_t sVidAsCvrs
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   Thefunction is used to set aspect cvrs.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15 		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAspectCvrs( DRV_AVCtrl_VIDACvrs_t  eVidAsCvrs )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_GetAspectCvrs
* Parameters : 
*                    DRV_AVCtrl_VIDACvrs_t* sVidAsCvrs
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   Thefunction is used to get aspect cvrs.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15,		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAspectCvrs( DRV_AVCtrl_VIDACvrs_t*  eVidAsCvrs )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_EnableOutput
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   Thefunction is used to enable video output.
* Author : dszheng 2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_EnableOutput( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_VOUT_Enable (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("call AM_AV_EnableVideoBlackout failed.[0x%x].\n",ErrCode);
		return AVCTRL_ERROR;
	}
	gVOUT_Flag = AM_TRUE;
	return AVCTRL_NO_ERROR;
}

//add by xyfeng
AM_Bool_t DRV_AVCTR_GetVid_Status( void )
{
	return gVOUT_Flag;
}
/******************************************************************************
* Function :DRV_AVCtrl_Vid_DisableOutput
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to disable video output.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15,		for 3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_DisableOutput( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_VOUT_Disable (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("call DRV_AVCtrl_Vid_DisableOutput failed.[0x%x].\n",ErrCode);
		return AVCTRL_ERROR;
	}
	gVOUT_Flag = AM_FALSE;
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Vid_SetOutputInterface
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set video output interface.
* Author : dszheng     2010/09/07
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for 3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetOutputInterface( 
						DRV_AVCtrl_VIDOUT_INTERFACE_t eVidOut_Interface )
{
	return AVCTRL_NO_ERROR;//modified by LYN, for 3716 platform ,no output interface need be switched.
}

/******************************************************************************
* Function :DRV_AVCtrl_PCR_SetPID
* Parameters : 
*                    U32 uiPcrPID
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set pcr pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PCR_SetPID( U32 uiPcrPID )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_PCR_ClearPID
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to clear pcr pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PCR_ClearPID( void )
{
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_Start
* Parameters : 
*                    DRV_AVCtrl_AudFormat_t eAudFmt
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to start audio decode.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Start( DRV_AVCtrl_AudFormat_t eAudFmt )
{

	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	pbiinfo("eAudFmt:%d VidFmt:%d [%d][%d]\n",eAudFmt, _DRV_AVCTRL_GetVidFmt(), gVidPid, gAudPid);
	switch(eAudFmt)
	{
		case AVCTRL_AUD_FORMAT_AAC:
		eAudFmt = 2;
		break;
		case AVCTRL_AUD_FORMAT_AAC_RAW:
		eAudFmt = 19;
		break;
		case AVCTRL_AUD_FORMAT_MPEG:
		eAudFmt = 0;
		break;
		case AVCTRL_AUD_FORMAT_AC3:
		eAudFmt = 3;
		break;
		case AVCTRL_AUD_FORMAT_EAC3:
		eAudFmt = 4;
		break;
		case AVCTRL_AUD_FORMAT_PCM:
		eAudFmt = 16;
		break;
	}
	ErrCode = AM_AV_StartTS(AV_DEV_NO, gVidPid, gAudPid, _DRV_AVCTRL_GetVidFmt(), eAudFmt );
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\n video play module start video decode error [%x],eAudFmt\n",ErrCode);
		return AVCTRL_ERROR;
	}
	_DRV_AVCTRL_RecAudFmt( eAudFmt );
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_Stop
* Parameters : 
*                   NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to stop audio decoder.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Stop( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	DRV_AVCtrl_Aud_SetPID(INVAILD_PID);
	ErrCode = AM_AV_StopTS(AV_DEV_NO);
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("AM_AV_StopTSVideo error %d\n",ErrCode);
		return AVCTRL_ERROR;
	}	
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_SetPID
* Parameters : 
*                    U32 uiAudPID
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                    The function is used to set audio pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetPID( U32 uiAudPID )
{
	gAudPid = uiAudPID;
	return AVCTRL_NO_ERROR;
}

U32 DRV_AVCtrl_Aud_GetPID( void )
{
	return gAudPid;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_ClearPID
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set invaild audio pid for clearing audio pid.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_ClearPID( void )
{
	gAudPid = INVAILD_PID;
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_SetVolume
* Parameters : 
*                    Volume Value
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set volume sound.
* Author : zxguan    2012/11/27
* --------------------
* Modification History:
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_VolumeInit( void )
{
    HI_ErrorCode_t ErrCode = HI_SUCCESS;
    S32 real_vol = 0;
    HI_BOOL bMute;
    ErrCode = HI_UNF_SND_GetVolume( HI_UNF_SND_0, &real_vol );
    if( HI_SUCCESS != ErrCode )
    {
        return AVCTRL_ERROR;
    }
    ErrCode = HI_UNF_SND_SetVolume(  HI_UNF_SND_0, real_vol );
    if( ErrCode != HI_SUCCESS )
    {
    	pbierror("\nset volume error %d\n",ErrCode);
    	return AVCTRL_ERROR;
    }
    ErrCode =  HI_UNF_SND_GetMute( HI_UNF_SND_0,  &bMute ); 
    if( HI_SUCCESS != ErrCode )
    {
        return AVCTRL_ERROR;
    }
    ErrCode =  HI_UNF_SND_SetMute( HI_UNF_SND_0,  bMute ); 
    if( HI_SUCCESS != ErrCode )
    {
        return AVCTRL_ERROR;
    } 
    return AVCTRL_NO_ERROR;
}
* 
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetVolume( U32 u32Volume )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
    if( u32Volume > 100 )
    {
        u32Volume = 100;
    }
    if( u32Volume < 0 )
    {
        u32Volume = 0;
    }
	ErrCode =  AM_AOUT_SetVolume( AV_DEV_NO, u32Volume ); 
    if( AM_SUCCESS != ErrCode )
    {
        pbierror("\nset Volume error %d\n",ErrCode);
		return AVCTRL_ERROR;
    }
    return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_Mute
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set mute on analog and digital sound.
* Author : dszheng    2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15 		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Mute( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode =  AM_AOUT_SetMute( AV_DEV_NO, AM_TRUE ); 
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AOUT_SetMute error %d AM_TRUE\n",ErrCode);
		return AVCTRL_ERROR;
	}	
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_UnMute
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set unmute on analog and digital sound.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_UnMute( void )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode =  AM_AOUT_SetMute( AV_DEV_NO, AM_FALSE ); 
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AOUT_SetMute error %d AM_FALSE\n",ErrCode);
		return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

/******************************************************************************
* Function :DRV_AVCtrl_Aud_SetStereo
* Parameters : 
*                    DRV_AVCtrl_AudStereo_t tStereo
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to set audio stereo.
* Author : dszheng     2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetStereo( DRV_AVCtrl_AudStereo_t eStereo )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode =  AM_AOUT_SetOutputMode( AV_DEV_NO, eStereo ); 
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AOUT_SetMute error %d AM_FALSE\n",ErrCode);
		return AVCTRL_ERROR;
	}
	 pbiinfo("%s   mode %d   OK",__FUNCTION__,eStereo);	
	return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_AudStereo_t  DRV_AVCtrl_Aud_GetStereo(void  )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	AM_AOUT_OutputMode_t mode;
	ErrCode =  AM_AOUT_GetOutputMode(AV_DEV_NO, &mode); 
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AOUT_GetOutputMode error %d AM_FALSE\n",ErrCode);
		return -1;
	}
	
	 pbiinfo("%s   mode %d   OK",__FUNCTION__,mode);	
	return (DRV_AVCtrl_AudStereo_t)mode;
}


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_EnableOutput( void )
{
	DRV_AVCtrl_Aud_UnMute();
    PBIDEBUG("AUD Enable ok!");
    return AVCTRL_NO_ERROR;
}
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_DisableOutput( void )
{
	DRV_AVCtrl_Aud_Mute();
    return AVCTRL_NO_ERROR;
}
/******************************************************************************
* Function :DRV_AVCtrl_Set_PlayPause
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to pause av.
* Author : dszheng    2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t   DRV_AVCtrl_Set_PlayPause( void )
{
#if 1
	AM_ErrorCode_t Error=AM_SUCCESS;

	Error = AM_AV_PauseInject(AV_DEV_NO);
	if( Error != AM_SUCCESS)
	{
		pbierror("[DRV_AVCtrl_Set_PlayPause]AM_AV_PauseInject	failed");
		return Error;
	}

	return AVCTRL_NO_ERROR;

#else

AM_ErrorCode_t Error=AM_SUCCESS;


	DRV_AVCtrl_Vid_Set_StopMode( AVCTRL_VID_STOP_MODE_STILL);

	DRV_AVCtrl_Vid_Stop();
	DRV_AVCtrl_Aud_Stop();
	Error = AM_AV_StopInject(AV_DEV_NO);
	if( Error != AM_SUCCESS)
	{
		pbierror("[DRV_AVCtrl_Set_PlayPause]AM_AV_PauseInject	failed %d",Error);
		return Error;
	}


return AVCTRL_NO_ERROR;

#endif
}

/******************************************************************************
* Function :DRV_AVCtrl_Set_PlayResume
* Parameters : 
*                    NULL
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   The function is used to resume av.
* Author : dszheng    2010/06/08
* --------------------
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/
DRV_AVCtrl_ErrorCode_t   DRV_AVCtrl_Set_PlayResume( void )
{

	AM_ErrorCode_t Error=AM_SUCCESS;

	Error = AM_AV_ResumeInject(AV_DEV_NO);
	if( Error != AM_SUCCESS)
	{
		pbierror("[AM_AV_ResumeInject]AM_AV_ResumeInject	failed %d",Error);
		return Error;
	}

	return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Format(DRV_AVCtrl_VidStandard_t tFormat)
{
	return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Format(DRV_AVCtrl_VidStandard_t* pFormat)
{

	return AVCTRL_NO_ERROR;
}


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Resolution(DRV_AVCtrl_VidResolution_t resolution)
{
	return AVCTRL_NO_ERROR;
}


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Resolution(DRV_AVCtrl_VidStandard_t *pResolution)
{
	return AVCTRL_NO_ERROR;
}



/******************************************************************************
* Function :DRV_AVCtrl_Set_Brightness
* Parameters : 
*                    brightness:ͼ������
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   ����ͼ�������?
* Author : qbxu   2010/07/31
* --------------------
*note:���ȷ�ΧΪ0-100��
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Brightness(U32 brightness)
{
	return AVCTRL_NO_ERROR;
}
/******************************************************************************
* Function :DRV_AVCtrl_Set_Contrast
* Parameters : 
*                    brightness:ͼ��Աȶ�?
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   ����ͼ��ĶԱȶ�?
* Author : qbxu   2010/07/31
* --------------------
*note:�Աȶȷ�ΧΪ0-100��
* Modification History:
* Modified by LYN,		2011/02/15		for His3716M adapting.
*
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Contrast(U32 contrast)
{
	return AVCTRL_NO_ERROR;
}
/******************************************************************************
* Function :DRV_AVCtrl_Set_Saturation
* Parameters : 
*                    brightness:ͼ��ɫ��
* Return :
*             AVCTRL_ERROR
*             AVCTRL_NO_ERROR
* Description :
*                   ����ͼ���ɫ��?
* Author : qbxu   2010/07/31
* --------------------
*note:ɫ�ȷ�ΧΪ0-100��
*Midification History:
*Modified by LYN,		2011/02/15		for His3716M adapting.
* --------------------
******************************************************************************/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Saturation(U32 Saturationess)
{
	return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Status_Info(DRV_AVCTRL_STATUS_AVCOUNT_INFO_t *tCountInfo)
{

	AM_AV_VideoStatus_t VStatus;
	AM_AV_AudioStatus_t Astatus;
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_GetVideoStatus (AV_DEV_NO, &VStatus);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AV_GetVideoStatus error %d AM_FALSE\n",ErrCode);
		return AVCTRL_ERROR;
	}
	ErrCode = AM_AV_GetAudioStatus (AV_DEV_NO, &Astatus);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AV_GetAudioStatus error %d AM_FALSE\n",ErrCode);
		return AVCTRL_ERROR;
	}
	//tCountInfo->u32VidFrameCount = VStatus.frames;
	//tCountInfo->u32AuddFrameCount = Astatus.frames;
	tCountInfo->u32VidFrameCount = VStatus.vb_data;
	tCountInfo->u32AuddFrameCount = Astatus.ab_data;
	/*pbiinfo("DRV_AVCtrl_Get_Status_Info vframes[%d]\n fps[%d]\n interlaced[%d]\n src_h[%d]\n src_w[%d]\n vb_data[%d]\n	vb_free[%d]\n vb_size[%d]\n vid_fmt[%d]\n ",VStatus.frames, 
	VStatus.fps,
	VStatus.interlaced,
	VStatus.src_h,
	VStatus.src_w,
	VStatus.vb_data,
	VStatus.vb_free,
	VStatus.vb_size,
	VStatus.vid_fmt);*/
	tCountInfo->u32VidErrorFrameCount = 0;
	
	return AVCTRL_NO_ERROR;
}

U32 DRV_AVCtrl_Get_AVPlayHandle()
{
    return NULL;
}

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_StartTs( )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	pbiinfo("[%s | %d]gVidPid:[%d] gAudPid:[%d] eVidFmt[%d]eAudFmt[%d]\n", DEBUG_LOG, gVidPid, gAudPid, _DRV_AVCTRL_GetVidFmt(), _DRV_AVCTRL_GetAudFmt() );
	ErrCode = AM_AV_StartTS(AV_DEV_NO, gVidPid, gAudPid, _DRV_AVCTRL_GetVidFmt(), _DRV_AVCTRL_GetAudFmt() );
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\n video play module start video decode error [%x],eAudFmt\n",ErrCode);
		return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_StopTs( )
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	pbiinfo("[%s | %d]gVidPid:[%d] gAudPid:[%d] eVidFmt[%d]eAudFmt[%d]\n", DEBUG_LOG, gVidPid, gAudPid, _DRV_AVCTRL_GetVidFmt(), _DRV_AVCTRL_GetAudFmt() );
	ErrCode = AM_AV_StopTS(AV_DEV_NO);
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\n video play module stop video decode error [%x],eAudFmt\n",ErrCode);
		return AVCTRL_ERROR;
	}
	ErrCode = AM_AV_ClearVideoBuffer(AV_DEV_NO);
	if( ErrCode != AM_SUCCESS )
	{
		pbierror("\n video play module clear video buffer error [%x],eAudFmt\n",ErrCode);
		return AVCTRL_ERROR;
	}
	return AVCTRL_NO_ERROR;
}


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_SetVPathPara(int isDvb)
{
	AM_ErrorCode_t ErrRet=0;
	int ff = AM_AV_FREE_SCALE_ENABLE;
	int di = AM_AV_DEINTERLACE_DISABLE;
	int pp = AM_AV_PPMGR_ENABLE;
	if(isDvb) {
		ff = AM_AV_FREE_SCALE_DISABLE;
		di = AM_AV_DEINTERLACE_ENABLE;
		pp = AM_AV_PPMGR_DISABLE;
	}
	if(!g_AVCtrlInitFlag)
	{
		AM_AV_OpenPara_t para;
		PBIDEBUG("AV Open");
		memset(&para, 0, sizeof(para));
		ErrRet = AM_AV_Open(AV_DEV_NO, &para);
		if( AM_SUCCESS != ErrRet )
	    {
	        PBIDEBUG("AM_AV_Open error!");
	        return AVCTRL_ERROR;
	    }
	}
	pbiinfo("isDvb:[%d] DRV_AVCtrl_SetVPathPara:ff[%d]di[%d]pp[%d]", isDvb, ff, di, pp);
	ErrRet = AM_AV_SetVPathPara (AV_DEV_NO, ff, di, pp);
	if (ErrRet != AM_SUCCESS)
	{
		pbierror("call AM_AV_SetVPathPara failed.[0x%x].\n",ErrRet);
		//return AVCTRL_ERROR;
	}
	if(!g_AVCtrlInitFlag)
	{
		PBIDEBUG("AV Close");
		ErrRet = AM_AV_Close(AV_DEV_NO);
	    if( AM_SUCCESS != ErrRet )
	    {
	        PBIDEBUG("AM_AV_Close error!");
	        return AVCTRL_ERROR;
	    }
	}
	return ErrRet;
}


/*-----------------------------------------------------------------------------------------*/
S32 DRV_AVCtrl_TS_Stop( void )
{
	AM_ErrorCode_t ErrCode = 0;

    PBIDEBUG("DRV_AVCtrl_TS_Stop in !");
//	DRV_AVCtrl_Vid_Set_StopMode( AVCTRL_VID_STOP_MODE_BLACK);

    DRV_AVCtrl_Vid_Stop();
    DRV_AVCtrl_Aud_Stop();
    ErrCode = AM_AV_StopInject(AV_DEV_NO);
    if( AM_SUCCESS != ErrCode )
    {
        pbierror("[%s %d] AM_AV_StopInject error 0x%x.\n", DEBUG_LOG, ErrCode);
    } 
 //   ErrCode = AM_DMX_Close(0);
    if( AM_SUCCESS != ErrCode )
    {
  //      pbierror("[%s %d] AM_AV_StopInject error 0x%x.\n", DEBUG_LOG, ErrCode);
    }
    PBIDEBUG("DRV_AVCtrl_TS_Stop End!!!!!!");
	
    return 0;
}


DRV_AVCtrl_AudFormat_t DRV_AVCtrl_Vid_GetFmt(void)
{
	return _DRV_AVCTRL_GetVidFmt();
}
DRV_AVCtrl_AudFormat_t DRV_AVCtrl_Aud_GetFmt(void)
{
	return _DRV_AVCTRL_GetAudFmt();
}


AM_ErrorCode_t DRV_AVCtrl_SetTs_Source(int src)
{
	AM_ErrorCode_t ErrRet=0;
	ErrRet = AM_AV_SetTSSource(AV_DEV_NO, src);
    if( AM_SUCCESS != ErrRet )
    {
       pbierror("AM_AV_SetTSSource error!");
       return ErrRet;
    }
	return SetDemuxDVB(src);
}


S32 Amlogic_av_player_init(void)
{
	static int aaaa = 0;
	int ErrRet=0;
	
	if( 1 == aaaa )
	{
		return 0;
		pbierror("[Amlogic_av_player_init] already Amlogic_av_player_init OK,not neccessry");
	}
	aaaa++;
	ErrRet = DRV_AVCtrl_Init(0);
	if( 0 != ErrRet )
	{
		pbierror("[Amlogic_av_player_init] Amlogic_av_player_init failed");
		return ErrRet;
	}
	
	ErrRet = AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_HIU);
    if( AM_SUCCESS != ErrRet )
    {
        pbierror("AM_AV_SetTSSource error!");
       return AVCTRL_ERROR;
    }
	pbierror("[Amlogic_av_player_init] Amlogic_av_player_init success");
	return 0;
}

S32 Amlogic_Inject_stop(void)
{
	
	AM_ErrorCode_t Error;
	Error = AM_AV_StopInject(AV_DEV_NO);
	if(Error !=AM_SUCCESS)
	{
		pbierror("[Amlogic_Inject_stop] Amlogic_Inject_stop failed");	
	}
	return Error;
}

S32 Amlogic_Inject_start(void)
{
	AM_AV_InjectPara_t para;
	AM_ErrorCode_t Error=AM_SUCCESS;
		
	memset(&para,0,sizeof(para));
	para.vid_fmt = DRV_AVCtrl_Vid_GetFmt();
	para.aud_fmt = DRV_AVCtrl_Aud_GetFmt();
	para.pkg_fmt = PFORMAT_TS;
	para.vid_id  = DRV_AVCtrl_Vid_GetPID();
	para.aud_id  = DRV_AVCtrl_Aud_GetPID();
//	Error = AM_AV_SetTSSource(AV_DEV_NO, AM_AV_TS_SRC_HIU);
//	if(Error !=AM_SUCCESS)
//	{
//		pbierror("[Amlogic_Inject_start] AM_AV_SetTSSource failed %d",Error);
//		return Error;
//	}
	
	Error = AM_AV_StartInject(AV_DEV_NO, &para);
	if(Error !=AM_SUCCESS)
	{
		pbierror("[Amlogic_Inject_start] Amlogic_Inject_start failed %d",Error);
		return Error;
	}
	return AM_SUCCESS;
}


S32 Amlogic_Inject_Data(char* buffer,int*length)
{
	AM_ErrorCode_t Error=AM_SUCCESS;

	Error =	AM_AV_InjectData(AV_DEV_NO, AM_AV_INJECT_MULTIPLEX, buffer, length, 1000);
	if( Error != AM_SUCCESS)
	{
		pbierror("[Amlogic_Inject_Data]Amlogic_Inject_Data  failed[%x]",Error);
		return Error;
	}
	return AM_SUCCESS;
}
S32 Amlogic_SetTSSource(void)
{
	return DRV_AVCtrl_SetTs_Source(2);

}
U32 amlogic_Check_Video_Status(void)
{
    DRV_AVCtrl_ErrorCode_t ret = AVCTRL_ERROR;
    DRV_AVCTRL_STATUS_AVCOUNT_INFO_t tCountInfo;
    ret = DRV_AVCtrl_Get_Status_Info( &tCountInfo );
    if( AVCTRL_ERROR == ret )
    {
        return 1;
    }
    if( dal_VideoFrameCount == tCountInfo.u32VidFrameCount )
    {
        return 1;
    }
    dal_VideoFrameCount = tCountInfo.u32VidFrameCount;
    return 0;
}
U32 amlogic_Get_Pause_Video_Frame(void)
{

	DRV_AVCtrl_ErrorCode_t ret = AVCTRL_ERROR;
    DRV_AVCTRL_STATUS_AVCOUNT_INFO_t tCountInfo;
    ret = DRV_AVCtrl_Get_Status_Info( &tCountInfo );
    if( AVCTRL_ERROR == ret )
    {
	   pbierror("[Amlogic_Inject_Data]Amlogic_Inject_Data	failed");
	   return 1;
    }
    dal_VideoFrameCount = tCountInfo.u32VidFrameCount;
	return 0;
}

