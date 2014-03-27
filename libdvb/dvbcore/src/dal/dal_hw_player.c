/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dal_hw_player.c
* Description : 
* History : 
*	Date				Modification				Name
*	----------			------------			----------
*	2010/12/13			Created					zyliu
******************************************************************************/
/*-----------------------------------------------------*/
/*              Includes												*/
/*-----------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pvddefs.h"
#include "osapi.h"
#include "psi_si.h"
#include "drv_avctrl.h"

#ifdef USE_ST_PLAYREC
#include "stvmix.h"
#include "stapp_vmix.h"
#include "stapp_layer.h"
#include "playrec.h"
#include "drv_still.h"
#endif
#include "ring_buffer.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal.h"
#include "dal_core.h"
#include "dal_hw_player.h"
#include "pbitrace.h"

/*-----------------------------------------------------*/
/*              Defines												*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Global												*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Exported Types									*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Local Function Prototypes								*/
/*-----------------------------------------------------*/

#ifdef USE_ST_PLAYREC

static DAL_ERROR_t playrec_start_src_demux( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
static DAL_ERROR_t playrec_start_src_file( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
static DAL_ERROR_t playrec_stop(void);

ST_ErrorCode_t SetDemuxDVB(void);
ST_ErrorCode_t SetDemuxP2S(void);
#else
static DAL_ERROR_t amlogic_play_start_src_demux(DAL_Player_Input_t* input,DAL_Player_Output_t* output);
static DAL_ERROR_t amlogic_play_stop(void);
DAL_ERROR_t   amlogic_play_Resume( DAL_Player_Input_t* input,DAL_Player_Output_t* output );

extern S32 SetDemuxDVB(void);
extern S32 SetDemuxP2S(void);
#endif


/*-----------------------------------------------------*/
/*              External Function  Definition								*/
/*-----------------------------------------------------*/
DAL_ERROR_t DAL_hw_play_start( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
#ifdef USE_ST_PLAYREC
	DRV_Still_ErrorCode_t	err_code = STILL_NO_ERROR;
	ST_ErrorCode_t			error = ST_NO_ERROR;
#else
	S32 error = 0;
#endif
	int						iret = 0;
	dal_evt_param_t				dal_evt_param;

	if( input == NULL || output == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

#if defined(DRV_Still_Stop)
	if(input->SourceType != SourceType_TUNER  
		&& input->SourceType != SourceType_MMS
		&& input->SourceType != SourceType_FILE)
	{
		DAL_DEBUG(("Not type SourceType_TUNER or SourceType_MMS or SourceType_FILE, Stop still before play start\n"));
		err_code = DRV_Still_Stop();
		if( err_code != 0)
		{
			DAL_DEBUG(("DRV_Still_Stop failed with %d\n", err_code));
		}
	}
#endif

	if( input->SourceType == SourceType_TUNER)
	{
		error = SetDemuxDVB();
		if( error != 0)
		{
			return iret;
		}

#ifdef USE_ST_PLAYREC		
		iret = playrec_start_src_demux(input,output);
#else
		//?hisi
#endif
		if( iret != 0)
		{
			return iret;
		}
		DAL_DVB_CheckFristFrame();	 /*利用dal_core task 去检测第一帧*/	
	}
	else if( input->SourceType == SourceType_HTTP ) /* http play*/
	{
#ifdef USE_ST_PLAYREC	
	//	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
		iret = playrec_start_src_file(input,output);
#else
	//hisi?
#endif
		if( iret != 0)
		{
	//		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
			return iret;
		}
	//	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
	}
	else if(input->SourceType == SourceType_FILE)
	{
#ifdef FLV	
		PLAYREC_SET_PLAY_FLVLIST(0);
#endif
#ifdef MMS
		PLAYREC_SET_PLAY_MMS(0);
#endif
#ifdef USE_ST_PLAYREC
		iret = playrec_start_src_file(input,output);
#else
		//hisi?
#endif
		if( iret != 0)
		{
			return iret;
		}
	}
	else if((input->SourceType == SourceType_P2S)
		||(input->SourceType == SourceType_P2V))
	{
		error = SetDemuxP2S();
		if( error != 0)
		{
			DebugMessage("[DAL]SetDemuxP2S failed %d ",error);
			return iret;
		}
		DebugMessage("[DAL]SetDemuxP2S success ");
		
		DAL_P2S_PlayStart( input, output);
	#if 0	
		pbiinfo("\n[%s %d]===== ====start av================== \n\n",DEBUG_LOG);
		hisi_start_audio(0,0);
		hisi_start_video(0,0);
		pbiinfo("\n[%s %d]===== ====================== \n\n",DEBUG_LOG);
	#endif	
	}
#ifdef FLV	
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		DAL_FlvList_PlayStart( input, output);
	}
#endif
	//{{{{add by LYN,for UDP Multicast,2011-08-22
#if defined(USE_DAL_MULTICAST)
	else if(SourceType_UDP == input->SourceType)
	{
	    //change the demux configuration same with P2S stram
	    error = SetDemuxP2S();
		if( error != 0)
		{
			return iret;
		}

        //start Multicast
        DAL_Multicast_PlayStart(input,output);
		
	}
	//}}}}
#endif	
#ifdef MMS	
	else if( input->SourceType == SourceType_MMS)
	{
		DAL_MMS_PlayStart( input, output);
	}
#endif	
	else if(input->SourceType == SourceType_BUFFER)
	{
		DAL_DEBUG(("SourceType_BUFFER DAL_ERROR_NOT_SUPPORT\n"));
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if(input->SourceType == SourceType_HLS)
	{
		error = SetDemuxP2S();
		if( error != 0)
		{
			DebugMessage("[DAL]SetDemuxP2S failed %x ",error);
			dal_core_event_notify(DAL_CORE_EVENT_M3U8_START_BAD_STATUS,&dal_evt_param);
			return iret;
		}
		DebugMessage("[DAL]SetDemuxP2S success,%d ",input->SourceType);

		DAL_hls_PlayStart(input, output);
	}
	else if(input->SourceType == SourceType_TIMESHIFT)
	{
		error = SetDemuxP2S();
		if( error != 0)
		{
			DebugMessage("[DAL]timeshift SetDemuxP2S failed %x ",error);
			dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_START_BAD_STATUS,&dal_evt_param);
			return iret;
		}
		DebugMessage("[DAL]timeshift SetDemuxP2S success,%d ",input->SourceType);

		DAL_Timeshift_PlayStart(input, output);
	}
	else
	{
		DAL_DEBUG(("Unknow type DAL_ERROR_NOT_SUPPORT\n"));
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_restart( DAL_Player_Input_t* old_input, DAL_Player_Input_t* new_input, DAL_Player_Output_t* old_output, DAL_Player_Output_t* new_output)
{
#ifdef USE_ST_PLAYREC
	PLAYREC_StreamData_t			pids[PLAYREC_MAX_PIDS];
	ST_ErrorCode_t					error = ST_NO_ERROR;
#else

	S32 error = 0;
#endif	
	int								i = 0;

	if( old_input == NULL || new_input == NULL || old_output == NULL || new_output == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	if( old_input->SourceType == SourceType_TUNER)
	{
#ifdef USE_ST_PLAYREC
		for( i = 0; i < new_input->PidsNum; i++)
		{
			pids[i].Type = new_input->Pids[i].type;
			pids[i].Pid = new_input->Pids[i].pid;
		}	
		error = PLAYREC_PlayChangePids( 0, new_input->PidsNum, pids);
		if( error != ST_NO_ERROR)
		{
			return DAL_ERROR_PLAYREC_CHANGEPIDS_FAILED;
		}
#else
//?hisi interface
#endif
		//...   对output 的处理
	}
	else if( old_input->SourceType == SourceType_HTTP)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if((old_input->SourceType == SourceType_P2S)
		||(old_input->SourceType == SourceType_P2V))
	{
		DAL_P2S_PlayRestart(new_input,new_output);
	//	DAL_DEBUG(("DAL_P2S_PlayRestart send msg ok!\n"));
		DebugMessage("[dal]DAL_hw_play_restart -> DAL_P2S_PlayRestart send msg ok!");
	}
#if defined(USE_DAL_MULTICAST)
	//{{{ add by LYN, for multicast, 2011-08-22
	else if (SourceType_UDP == old_input->SourceType)
	{
	    DAL_Multicast_PlayRestart(new_input,new_output);
	   // DAL_DEBUG(("DAL_Multicast_PlayRestart send msg ok!\n"));
	   DebugMessage("[dal]DAL_hw_play_restart -> DAL_Multicast_PlayRestart send msg ok!");
	}
	//}}} add by LYN, for multicast, 2011-08-22
#endif	
#ifdef MMS
	else if( old_input->SourceType == SourceType_MMS)
	{
		DAL_MMS_PlayRestart(new_input,new_output);
	}
#endif
#ifdef FLV
	else if( old_input->SourceType == SourceType_FLV_LIST)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
#endif
	else if( old_input->SourceType == SourceType_HLS)
	{
		DAL_HLS_PlayRestart(new_input,new_output);
	}
	else if (old_input->SourceType == SourceType_BUFFER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}	
	return DAL_NO_ERROR;
}


DAL_ERROR_t DAL_hw_play_stop( DAL_Player_Input_t* input)
{
	int						iret = 0;
#ifdef USE_ST_PLAYREC
	ST_ErrorCode_t			error = ST_NO_ERROR;
#else
	S32			error = 0;
#endif
	dal_evt_param_t			dal_evt_param;

	//DAL_DEBUG(("entry\n"));
	DebugMessage("[dal]DAL_hw_play_stop -> entry!");

	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}
	
	if( input->SourceType == SourceType_TUNER 
		|| input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE)
	{	
	
#ifdef USE_ST_PLAYREC
		iret = playrec_stop();
		if( iret != 0)
		{
			return iret;
		}
#else
	iret = amlogic_play_stop();
	if( iret != 0)
	{
		DebugMessage("[dal]hisi_play_stop : error!");
		dal_core_event_notify(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,&dal_evt_param);
		return iret;
	}

#endif
	}
	else if( (input->SourceType == SourceType_P2S)
		||( input->SourceType == SourceType_P2V))
	{
		DAL_P2S_PlayStop();
		//error = SetDemuxDVB();
		
		if( error != 0)
		{
			return error;
		}
	}
#if defined(USE_DAL_MULTICAST)
	//{{{add by LYN,for multicast , 2011-08-22
	else if(SourceType_UDP == input->SourceType)
	{
	    DAL_Multicast_PlayStop();
	 //   error = SetDemuxDVB();
		if( error != 0)
		{
			return error;
		}
	}
	//}}}add by LYN,for multicast , 2011-08-22
#endif	
#ifdef FLV
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		DAL_FlvList_PlayStop();
	}
#endif	
#ifdef MMS
	else if( input->SourceType == SourceType_MMS)
	{
		DAL_MMS_PlayStop();
	}
#endif	
	else if( input->SourceType == SourceType_HLS)
	{
		 DAL_hls_PlayStop();	
	}
	else if( input->SourceType == SourceType_TIMESHIFT)
	{
		DAL_Timeshift_PlayStop();	
	}
	else if( input->SourceType == SourceType_BUFFER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	else
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_pause( DAL_Player_Input_t* input)
{
#ifdef USE_ST_PLAYREC

	ST_ErrorCode_t				error = ST_NO_ERROR;
#else
	S32 error = 0;
#endif
	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	if( input->SourceType == SourceType_TUNER 
		|| input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE)
	{
#ifdef USE_ST_PLAYREC
		error = PLAYREC_PlayPause(0);
		if( error != ST_NO_ERROR)
		{
			return DAL_ERROR_PLAYREC_PAUSE_FAILED;
		}
#endif		
	}
	else if( input->SourceType == SourceType_P2S)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if( input->SourceType == SourceType_P2V)
	{
		P2V_pause();
	}
#if defined(USE_DAL_MULTICAST)
	//{{{add by LYN , for Multicast, 2011-08-22
    else if(SourceType_UDP == input->SourceType)
	{
	    return DAL_ERROR_NOT_SUPPORT;
	}
	//}}}add by LYN , for Multicast, 2011-08-22
#endif
#ifdef FLV
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		DAL_FlvList_PlayPause();
	}
#endif	
	else if(SourceType_HLS == input->SourceType)
	{
		HLS_pause();

	}
	else
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_resume( DAL_Player_Input_t* input)
{
#ifdef USE_ST_PLAYREC
	ST_ErrorCode_t				error = ST_NO_ERROR;
#else
	S32  error = 0;
#endif

	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	if( input->SourceType == SourceType_TUNER 
		|| input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE)
	{
#ifdef USE_ST_PLAYREC
		error = PLAYREC_PlayResume(0);
		if( error != ST_NO_ERROR)
		{
			return DAL_ERROR_PLAYREC_RESUME_FAILED;
		}
#endif		
	}
	else if( input->SourceType == SourceType_P2S)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if(input->SourceType == SourceType_P2V)
	{
		P2V_resume(	input->Source.http.URL);

	}
#if defined(USE_DAL_MULTICAST)	
	//{{{ add by LYN, for multicast , 2011-08-22
	else if( SourceType_UDP == input->SourceType)
	{
	    return DAL_ERROR_NOT_SUPPORT;
	}
#endif	
	//}}} add by LYN, for multicast , 2011-08-22
#ifdef FLV	
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		DAL_FlvList_PlayResume();
	}
#endif
	else if(SourceType_HLS == input->SourceType)
	{
		HLS_resume( input->Source.http.URL);
	}
	else
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_seek( DAL_Player_Input_t* input, U32 msecond)
{
	
#ifdef USE_ST_PLAYREC
		ST_ErrorCode_t				error = ST_NO_ERROR;
#else
		S32  error = 0;
#endif

	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	if( input->SourceType == SourceType_TUNER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if( input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE)
	{
#ifdef USE_ST_PLAYREC
	
		printf(">>>>> ready to seek to %u ms\n", msecond);
		error = PLAYREC_PlaySeek( 0, msecond, PLAYREC_PLAY_SEEK_SET);
		if( error != ST_NO_ERROR)
		{
			printf("add_zyliu ##### [%s]: PLAYREC_PlaySeek return 0x%x\n",__FUNCTION__,error);
			return DAL_ERROR_PLAYREC_SEEK_FAILED;
		}
#endif
	}
	else if( input->SourceType == SourceType_P2S
		|| input->SourceType == SourceType_BUFFER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
#if defined(USE_DAL_MULTICAST)
	//{{{add by LYN , for multicast ,2011-08-22
	else if(SourceType_UDP == input->SourceType)
	{
	    return DAL_ERROR_NOT_SUPPORT;
	}
	//}}}add by LYN , for multicast ,2011-08-22
#endif	
#ifdef FLV	
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		DAL_FlvList_PlaySeek(msecond);
	}
#endif	
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_setspeed( DAL_Player_Input_t* input, DAL_PlayerSpeed_t speed)
{
#ifdef USE_ST_PLAYREC
			ST_ErrorCode_t				error = ST_NO_ERROR;
#else
			S32  error = 0;
#endif
	
	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}
	if( input->SourceType == SourceType_TUNER
		|| input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE)
	{
	
#ifdef USE_ST_PLAYREC
		error = PLAYREC_PlaySetSpeed( 0, speed);
		if( error != ST_NO_ERROR)
		{
			return DAL_ERROR_PLAYREC_SETSPEED_FAILED;
		}
#endif		
	}
	else if( input->SourceType == SourceType_P2S
			||	input->SourceType == SourceType_P2V
			|| input->SourceType == SourceType_FLV_LIST
			|| input->SourceType == SourceType_BUFFER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
#if defined(USE_DAL_MULTICAST)	
    //{{{ add by LYN , multicast , 2011-08-22
    else if (SourceType_UDP == input->SourceType)
    {
        return DAL_ERROR_NOT_SUPPORT;
    }
    //}}}add by LYN , multicast , 2011-08-22
#endif	
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_getinfo( DAL_Player_Input_t* input, DAL_Player_Info_t* player_info)
{
#ifdef USE_ST_PLAYREC
	PLAYREC_PlayStreamInfos_t	PlayStreamInfos;
	PLAYREC_PlayParams_t		PlayParams;
	PLAYREC_PlayStatus_t		PlayStatus;
	ST_ErrorCode_t				error = ST_NO_ERROR;
	S32 						speed;
	U32 						BeginOfFileTimeInMs;
	U32 						CurrentPlayTimeInMs;
	U32 						EndOfFileTimeInMs;

	printf("---------------------------------------------DAL_hw_play_getinfo entry\n");

	if( input == NULL || player_info == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}
	
	if( input->SourceType == SourceType_TUNER
		|| input->SourceType == SourceType_HTTP
		|| input->SourceType == SourceType_FILE
		||  input->SourceType == SourceType_P2S
		||  input->SourceType == SourceType_P2V
		/*|| input->SourceType == SourceType_FLV_LIST*/
		|| SourceType_UDP== input->SourceType)//add by LYN, for multicast
	{
		if( input->SourceType == SourceType_P2S || input->SourceType == SourceType_P2V|| input->SourceType == SourceType_FLV_LIST)
		{
			/*因为P2S播放是异步开启的，所以先判断下playrec是否已经开启*/
			if( DAL_P2S_PlayIsStart() == 0)
			{
				printf("---------------------------------------------return DAL_NO_ERROR\n");
				return DAL_NO_ERROR;
			}
		}

        //{{{add by LYN, for multicat , 2011-08-22
        if(SourceType_UDP == input->SourceType)
        {
            //same with the P2S function
            if( DAL_Multicast_PlayIsStart() == 0)
			{
				return DAL_NO_ERROR;
			}
        }
        //}}}add by LYN
        
		error = PLAYREC_PlayGetStreamInfos(0, &PlayStreamInfos);
		if( error != ST_NO_ERROR)
		{
			return DAL_ERROR_PLAYREC_GETSTREAMINFO_FAILED;
		}
		error = PLAYREC_PlayGetSpeed( 0, &speed);
		error = PLAYREC_PlayGetStatus( 0, &PlayStatus);
		error = PLAYREC_PlayGetParams( 0, &PlayParams);
		error = PLAYREC_PlayGetTime( 0, &BeginOfFileTimeInMs, &CurrentPlayTimeInMs, &EndOfFileTimeInMs);


		player_info->BufferingData = TRUE;
		player_info->IsBuffering = FALSE;
		player_info->nowposition = 0;
		//player_info.output
		//player_info->playing_filesize_t
		//player_info->record_filesize
		//player_info->record_filesize_t
		//player_info.states
		//player_info.input
	}
	else if( input->SourceType == SourceType_FLV_LIST)
	{
		//DAL_FlvList_PlayGetinfo();
		return DAL_ERROR_NOT_SUPPORT;
	}
	else if( input->SourceType == SourceType_BUFFER)
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
#else
// hisi ? ???
#endif	
	return DAL_NO_ERROR;
}

DAL_ERROR_t DAL_hw_play_getplaytime( DAL_Player_Input_t* input, unsigned int* p_total_time, unsigned int* p_current_time)
{
	int				iret = 0;

	
	if( SourceType_FILE == input->SourceType || SourceType_HTTP == input->SourceType)
	{
		iret = DAL_Local_PlayGetPlayTime(p_total_time,p_current_time);
		if( iret != 0)
		{
			DAL_ERROR(("DAL_Local_PlayGetPlayTime failed\n"));
			return -1;
		}		
	}
#ifdef FLV	
	else if( SourceType_FLV_LIST == input->SourceType)
	{
		iret = DAL_FlvList_PlayGetPlayTime(p_total_time,p_current_time);
		if( iret != 0)
		{
			DAL_ERROR(("DAL_FlvList_PlayGetPlayTime failed\n"));
			return -1;
		}
	}
#endif	
	else
	{
		*p_total_time = 0;
		*p_current_time = 0;
		return DAL_ERROR_NOT_SUPPORT;
	}
	return 0;
}

int DAL_Local_PlayGetPlayTime( unsigned int* total_time, unsigned int* current_time)
{
#ifdef USE_ST_PLAYREC

	ST_ErrorCode_t				err = ST_NO_ERROR;
	U32							BeginOfFileTimeInMs;
	U32							CurrentPlayTimeInMs;
	U32							EndOfFileTimeInMs;
	
	err = PLAYREC_PlayGetTime( 0, &BeginOfFileTimeInMs, &CurrentPlayTimeInMs, &EndOfFileTimeInMs);
	if( err != ST_NO_ERROR )
	{
		DAL_ERROR(("PLAYREC_PlayGetTime failed\n"));
		*total_time = 0;
		*current_time = 0;
		return -1;
	}

	*total_time = EndOfFileTimeInMs;
	*current_time = CurrentPlayTimeInMs;
#endif	
	return 0;
}

#ifdef FLV
DAL_ERROR_t DAL_hw_play_flvlist_getplaytime(unsigned int* p_total_time, unsigned int* p_current_time)
{
	int				iret = 0;

	iret = DAL_FlvList_PlayGetPlayTime(p_total_time,p_current_time);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_FlvList_PlayGetPlayTime failed\n"));
		return -1;
	}
	return 0;
}

DAL_ERROR_t DAL_hw_play_flvlist_play_prev( void)
{
	return DAL_FlvList_PlaySeekPrev();
}

DAL_ERROR_t DAL_hw_play_flvlist_play_next( void)
{
	return DAL_FlvList_PlaySeekNext();
}

DAL_ERROR_t DAL_hw_play_flvlist_play_pause_and_resume( void)
{
	return DAL_FlvList_PlayPauseAndResume();
}

#endif


DAL_ERROR_t DAL_PlayrecStartWithDemux( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
#ifdef USE_ST_PLAYREC

	return playrec_start_src_demux(input,output);
#else
	if( DAL_InjectIsPause())
	{
	//	return amlogic_play_Resume(input,output);
	return DRV_AVCtrl_Set_PlayResume();
	}
	else
	{
	   return amlogic_play_start_src_demux(input,output);
        }

#endif
}

DAL_ERROR_t DAL_PlayrecStartWithFile( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
#ifdef USE_ST_PLAYREC

	return playrec_start_src_file(input,output);
#else
//? hisi    mms & flv use
	return 0;

#endif
}

DAL_ERROR_t DAL_PlayrecStop(void)
{
#ifdef USE_ST_PLAYREC
	return playrec_stop();	
#else
	//? hisi
	return amlogic_play_stop();
#endif
}
S32 toHisiStreamType(DAL_StreamType_t type)
{ 
	switch(type)
	{
		case DAL_STREAMTYPE_MP1V: 
			return STREAM_TYPE_MPEG1_VIDEO;
		case DAL_STREAMTYPE_MP2V: 
			return STREAM_TYPE_MPEG2_VIDEO;
		case DAL_STREAMTYPE_MPEG4P2:
			return STREAM_TYPE_MPEG4P2;
		case DAL_STREAMTYPE_H264: 
			return STREAM_TYPE_H264;
		
		case DAL_STREAMTYPE_MP1A : 
			return STREAM_TYPE_MPEG1_AUDIO;
		case DAL_STREAMTYPE_MP2A :
			return STREAM_TYPE_MPEG2_AUDIO;
		case DAL_STREAMTYPE_MP4A : 
			return STREAM_TYPE_MPEG4_AAC;
		case DAL_STREAMTYPE_AC3:
			return STREAM_TYPE_AC3;
		case DAL_STREAMTYPE_AAC: 
			return STREAM_TYPE_MPEG_AAC;
		default: return -1;
	}
}

static DAL_ERROR_t amlogic_play_start_src_demux(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	U32 audio_pid = 0,video_pid = 0,audio_fmt = 0,video_fmt = 0,Error=0,i=0,
		VElementNum=0,AElementNum=0;

	
	if(input == NULL)
	{	
		DebugMessage("[dal]amlogic_play_start_src_demux ->amlogic_play_start_src_demux bad parameter!");
		return DAL_ERROR_BAD_PARAMETER;
	}
	Error = DRV_AVCtrl_Vid_Stop();
	Error |= DRV_AVCtrl_Aud_Stop();
	if(Error != 0)
	{
		DebugMessage("[dal]amlogic_play_start_src_demux ->amlogic_play_start_src_demux AV stop error!");
		return DAL_ERROR_PLAYREC_STOP_FAILED;
	}
	DRV_AVCtrl_Vid_EnableOutput();
	
	DebugMessage("[dal]amlogic_play_start_src_demux ->DRV_AVCtrl_Vid_EnableOutput AV out!");

	for( i = 0; i< input->PidsNum; i++)
	{
			if( input->Pids[i].type == DAL_STREAMTYPE_MP1V
			|| input->Pids[i].type == DAL_STREAMTYPE_MP2V
			|| input->Pids[i].type == DAL_STREAMTYPE_MPEG4P2
			|| input->Pids[i].type == DAL_STREAMTYPE_H264)
		{
			DebugMessage("[dal]amlogic_play_start_src_demux ->found a video pid[%d]: %d	type[%d]: %d\n", i,input->Pids[i].pid,i,input->Pids[i].type);
			VElementNum++;
			if( VElementNum == 1 )
			{
				video_fmt = toHisiStreamType(input->Pids[i].type);
				video_pid = input->Pids[i].pid;
				
			}
			DebugMessage("[DAL]use first video pid: %u	type: %u\n", video_pid,video_fmt);
		}
		else if( input->Pids[i].type == DAL_STREAMTYPE_AC3
			|| input->Pids[i].type == DAL_STREAMTYPE_AAC
			|| input->Pids[i].type == DAL_STREAMTYPE_MP1A 
			|| input->Pids[i].type == DAL_STREAMTYPE_MP2A
			|| input->Pids[i].type == DAL_STREAMTYPE_MP4A )
		{
		
			DebugMessage("[dal]amlogic_play_start_src_demux -> found a audio pid[%d]: %d	type[%d]: %d\n",i,input->Pids[i].pid,i,input->Pids[i].type);
			AElementNum++;
			if( AElementNum == 1 )
			{
				audio_fmt = toHisiStreamType(input->Pids[i].type);
				audio_pid= input->Pids[i].pid;
			}
			DebugMessage("[DAL]use first audio pid: %u	type: %u\n",audio_pid,audio_fmt);
		}
	}

	if((VElementNum==0)||(AElementNum==0))
	{
		DebugMessage("[dal]amlogic_play_start_src_demux ->amlogic_play_start_src_demux parse av pid error!");
		return DAL_ERROR_P2S_ParsePID_FAILED;
	}
	DebugMessage("[dal]amlogic_play_start_src_demux ->DAL TS PMT Vpid:%x Vfmt:%x Apid:%x Afmt:%x\n",video_pid, video_fmt, audio_pid, audio_fmt );
	if( audio_pid < 0x1FFF )
	{	
	
		Error = amlogic_start_PCR(audio_pid);
		Error|= amlogic_start_audio(audio_pid,audio_fmt);
		 if(Error != 0)
		 {
   		//	 DAL_ERROR(("\n[hisi_play_start_src_demux]Start_Audio ErrRet:%x\n",Error));
			DebugMessage("[dal]amlogic_play_start_src_demux ->Start_Audio ErrRet:%x\n",Error);
			 return Error;
		 }
	}
	
	if((video_pid > 0 ) && (video_pid < 0x1FFF ))
	{
		Error = amlogic_start_video(video_pid,video_fmt);
		if( Error != 0 )
		{
		//	DAL_ERROR(("\n[hisi_play_start_src_demux]Start_Video ErrRet:%x\n",Error));
			DebugMessage("[dal]amlogic_play_start_src_demux ->Start_Video ErrRet:%x\n",Error);
			return Error;
		}
	}	

	return DAL_NO_ERROR;
}

DAL_ERROR_t   amlogic_play_Resume( DAL_Player_Input_t* input,DAL_Player_Output_t* output )
{

	DAL_ERROR_t Error=DAL_NO_ERROR;
	U32 audio_pid = 0,video_pid = 0,audio_fmt = 0,video_fmt = 0,i=0,VElementNum=0,AElementNum=0;
		

	for( i = 0; i< input->PidsNum; i++)
	{
			if( input->Pids[i].type == DAL_STREAMTYPE_MP1V
			|| input->Pids[i].type == DAL_STREAMTYPE_MP2V
			|| input->Pids[i].type == DAL_STREAMTYPE_MPEG4P2
			|| input->Pids[i].type == DAL_STREAMTYPE_H264)
		{
			DebugMessage("[dal]amlogic_Play_Resume ->found a video pid[%d]: %d	type[%d]: %d\n", i,input->Pids[i].pid,i,input->Pids[i].type);
			VElementNum++;
			if( VElementNum == 1 )
			{
				video_fmt = toHisiStreamType(input->Pids[i].type);
				video_pid = input->Pids[i].pid;
				
			}
			DebugMessage("[DAL]use first video pid: %u	type: %u\n", video_pid,video_fmt);
		}
		else if( input->Pids[i].type == DAL_STREAMTYPE_AC3
			|| input->Pids[i].type == DAL_STREAMTYPE_AAC
			|| input->Pids[i].type == DAL_STREAMTYPE_MP1A 
			|| input->Pids[i].type == DAL_STREAMTYPE_MP2A
			|| input->Pids[i].type == DAL_STREAMTYPE_MP4A )
		{
		
			DebugMessage("[dal]amlogic_Play_Resume -> found a audio pid[%d]: %d	type[%d]: %d\n",i,input->Pids[i].pid,i,input->Pids[i].type);
			AElementNum++;
			if( AElementNum == 1 )
			{
				audio_fmt = toHisiStreamType(input->Pids[i].type);
				audio_pid= input->Pids[i].pid;
			}
			DebugMessage("[DAL]use first audio pid: %u	type: %u\n",audio_pid,audio_fmt);
		}
	}

	if((VElementNum==0)||(AElementNum==0))
	{
		DebugMessage("[dal]amlogic_Play_Resume ->amlogic_play_start_src_demux parse av pid error!");
		return DAL_ERROR_P2S_ParsePID_FAILED;
	}
	DebugMessage("[dal]amlogic_Play_Resume ->DAL TS PMT Vpid:%x Vfmt:%x Apid:%x Afmt:%x\n",video_pid, video_fmt, audio_pid, audio_fmt );
	if( audio_pid < 0x1FFF )
	{	
	
		Error = amlogic_start_PCR(audio_pid);
		Error|= amlogic_start_audio(audio_pid,audio_fmt);
		 if(Error != 0)
		 {
			DebugMessage("[dal]amlogic_Play_Resume ->Start_Audio ErrRet:%x\n",Error);
			 return Error;
		 }
	}
	
	if((video_pid > 0 ) && (video_pid < 0x1FFF ))
	{
		Error = amlogic_start_video(video_pid,video_fmt);
		if( Error != 0 )
		{
			DebugMessage("[dal]amlogic_Play_Resume ->Start_Video ErrRet:%x\n",Error);
			return Error;
		}
	}	

//	 DRV_AVCtrl_Set_PlayResume();
	Amlogic_Inject_start();

	return DAL_NO_ERROR;
}




static DAL_ERROR_t amlogic_play_stop(void)
{
	U32 Error = 0;

	Error =  DRV_AVCtrl_TS_Stop();
	if(Error != 0)
		return DAL_ERROR_PLAYREC_STOP_FAILED;
	else
		return DAL_NO_ERROR;
}

S32 amlogic_start_audio( U32 AudPid,U32 AudFmt)
{
	S32 ErrRet = 0;

	//AudPid = 68;
	//AudFmt = 0x03;  
	
//	AudPid = get_mpts_audio_pid();	
//	AudFmt = get_mpts_audio_FMT();
	
	switch(AudFmt)
   	{
        case STREAM_TYPE_MPEG1_AUDIO:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;

        case STREAM_TYPE_MPEG2_AUDIO:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;

        case STREAM_TYPE_MPEG4_AAC:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AAC_RAW;
        }
        break;
        case STREAM_TYPE_MPEG_AAC:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AAC;
        }
        break;

        case STREAM_TYPE_AC3:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AC3;
        }
        break;
        case STREASM_TYPE_EAC3:
        {
            AudFmt = AVCTRL_AUD_FORMAT_EAC3;
        }
        break;

        default:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;
    }

	 ErrRet = DRV_AVCtrl_Aud_SetPID(AudPid);
	 ErrRet |= DRV_AVCtrl_Aud_Start(AudFmt);
	 return ErrRet;
}

S32 amlogic_start_video( U32 VidPid,U32 VidFmt)
{
	S32 ErrRet = 0;

	//VidPid =82;
	//VidFmt = 0x1b;
	
//	VidPid = get_mpts_video_pid();
//	VidFmt = get_mpts_video_FMT();
	
	switch(VidFmt)
	{
		case STREAM_TYPE_MPEG1_VIDEO:
		case STREAM_TYPE_MPEG2_VIDEO:
		{
			VidFmt = AVCTRL_VID_FORMAT_MPEG2;
		}
		break;

		case STREAM_TYPE_MPEG4P2:
		{
			VidFmt = AVCTRL_VID_FORMAT_MPEG4;
		}
		break;

		case STREAM_TYPE_H264:
		{
			VidFmt = AVCTRL_VID_FORMAT_H264;
		}
		break;

		default:
		{
			VidFmt = AVCTRL_VID_FORMAT_MPEG2;
		}
		break;
	}
	ErrRet = DRV_AVCtrl_Vid_SetPID(VidPid);
	ErrRet |= DRV_AVCtrl_Vid_Start(VidFmt);

	return ErrRet;
}

S32 amlogic_start_PCR( U32 VidPid)
{
	S32 ErrRet = 0;	
	ErrRet = DRV_AVCtrl_PCR_SetPID( VidPid );

	return ErrRet;
}


#ifdef USE_ST_PLAYREC
static DAL_ERROR_t playrec_start_src_demux( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	PLAYREC_PlayInitParams_t		PlayInitParams;
	PLAYREC_PlayStartParams_t		PlayStartParams;
	PLAYREC_PlayTermParams_t		PlayTermParams;
	PLAYREC_PlayParams_t			PlayParams;
	ST_ErrorCode_t					error = ST_NO_ERROR;
	U32								i = 0;	
    	char                            ucSourceName[8];


	memset(&PlayInitParams, 0, sizeof(PLAYREC_PlayInitParams_t));
	if( input->SourceType == SourceType_TUNER)
	{
		/*modify zyliu tuner 的选择拿到DAL 外层*/
		sprintf(PlayInitParams.SourceName,"%d", input->TunerIndex);
	}
	else if(( input->SourceType == SourceType_P2S)
		||( input->SourceType == SourceType_P2V))
	{
		strcpy(PlayInitParams.SourceName, "0");
	}

    //{{{add by LYN , for multicast , 2011-08-22
    else if( SourceType_UDP == input->SourceType)
    {
        // same with the P2S function
        strcpy(PlayInitParams.SourceName,"0");
    }
    //}}}add by LYN , for multicast , 2011-08-22
    
	strcpy(PlayInitParams.DestinationName,"SWTS");
		
	PlayInitParams.PidsNum = input->PidsNum;
	for( i = 0; i < input->PidsNum; i++)
	{
		//printf("input->Pids[%d].type: %d   input->Pids[%d].pid: %d\n", i,input->Pids[i].type,i,input->Pids[i].type);
		if( input->Pids[i].type != PLAYREC_STREAMTYPE_INVALID && input->Pids[i].pid != STPTI_InvalidPid())
		{
			PlayInitParams.Pids[i].Type = input->Pids[i].type;
			PlayInitParams.Pids[i].Pid = input->Pids[i].pid;
		}
	}
	error = PLAYREC_PlayInit( 0, &PlayInitParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_INIT_FAILED;
	}
	
	memset(&PlayStartParams, 0, sizeof(PLAYREC_PlayStartParams_t));
	if( input->SourceType == SourceType_TUNER)
	{
		sprintf(PlayStartParams.SourceName,"%d",input->TunerIndex);
	}
	else
	{
		strcpy(PlayStartParams.SourceName, "0");
	}
	strcpy(PlayStartParams.DestinationName,"SWTS");
	
	PlayStartParams.PidsNum = input->PidsNum;
	for( i = 0; i < PlayStartParams.PidsNum; i++)
	{
		printf("input->Pids[%d].type: %d   input->Pids[%d].pid: %d\n", i,input->Pids[i].type,i,input->Pids[i].pid);
		if( input->Pids[i].type != PLAYREC_STREAMTYPE_INVALID && input->Pids[i].pid != STPTI_InvalidPid())
		{
			PlayStartParams.Pids[i].Type = input->Pids[i].type;
			PlayStartParams.Pids[i].Pid = input->Pids[i].pid;
		}
	}
	
	PlayStartParams.VideoEnableDisplay = TRUE;
	PlayStartParams.VideoLayerIdList[0] = 0;
	PlayStartParams.VideoLayerIdList[1] = 1;
	PlayStartParams.VideoLayerIdList[2] = 0xFFFFFFFF;
	PlayStartParams.VideoDecodeOnce = FALSE;
	PlayStartParams.AudioEnableOutput = TRUE;
	PlayStartParams.StartPositionInMs = 0;
	PlayStartParams.StartSpeed = PLAYREC_PLAY_SPEED_ONE;

	error = PLAYREC_PlayStart( 0, &PlayStartParams);
	if( error != ST_NO_ERROR)
	{
		memset( &PlayTermParams, 0, sizeof(PLAYREC_PlayTermParams_t));
		PLAYREC_PlayTerm( 0, &PlayTermParams);
		return DAL_ERROR_PLAYREC_START_FAILED;
	}

	error = PLAYREC_PlayGetParams( 0, &PlayParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_GetParams_FAILED;
	}
	output->vidhandle = PlayParams.VIDHandle;
	output->vpSDhandle = PlayParams.VIEWHandle[1];
	output->vpHDhandle = PlayParams.VIEWHandle[0];
	return DAL_NO_ERROR;
}

static DAL_ERROR_t playrec_start_src_file( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	PLAYREC_PlayInitParams_t		PlayInitParams;
	PLAYREC_PlayStartParams_t		PlayStartParams;
	PLAYREC_PlayTermParams_t		PlayTermParams;
	PLAYREC_PlayParams_t			PlayParams;
	ST_ErrorCode_t					error = ST_NO_ERROR;
	U32								i = 0;	

	memset(&PlayInitParams, 0, sizeof(PLAYREC_PlayInitParams_t));
	strncpy(PlayInitParams.SourceName, (char*)input->Source.http.URL, strlen((char*)input->Source.http.URL));
	strcpy(PlayInitParams.DestinationName, "SWTS");
	
	PlayInitParams.IsFilePartiallyDownloaded = FALSE;
	PlayInitParams.Metadata = NULL;
	PlayInitParams.WmdrmNDHeader = NULL;
	PlayInitParams.PidsNum = 0;

	//PlayInitParams.Pids[0].Type = PLAYREC_STREAMTYPE_MMV;
	//PlayInitParams.Pids[0].Pid = 0;
	
	error = PLAYREC_PlayInit( 0, &PlayInitParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_INIT_FAILED;
	}
	
	memset(&PlayStartParams, 0, sizeof(PLAYREC_PlayStartParams_t));
	strncpy(PlayStartParams.SourceName, PlayInitParams.SourceName, strlen(PlayInitParams.SourceName));
	strcpy(PlayStartParams.DestinationName,"SWTS");
	
	PlayStartParams.PidsNum = PlayInitParams.PidsNum;
	for( i = 0; i < PlayStartParams.PidsNum; i++)
	{
		if( PlayInitParams.Pids[i].Type!= PLAYREC_STREAMTYPE_INVALID && PlayInitParams.Pids[i].Pid != STPTI_InvalidPid())
		{
			PlayStartParams.Pids[i].Type = PlayInitParams.Pids[i].Type;
			PlayStartParams.Pids[i].Pid = PlayInitParams.Pids[i].Pid;
		}
	}
	
	PlayStartParams.VideoEnableDisplay = TRUE;
	PlayStartParams.VideoDecodeOnce    = FALSE;
	PlayStartParams.AudioEnableOutput  = TRUE;
	PlayStartParams.StartSpeed		   = PLAYREC_PLAY_SPEED_ONE;
	PlayStartParams.StartPositionInMs  = 0;
	
	PlayStartParams.VideoLayerIdList[0] = 0;
	PlayStartParams.VideoLayerIdList[1] = 1;
	PlayStartParams.VideoLayerIdList[2] = 0xFFFFFFFF;

	error = PLAYREC_PlayStart( 0, &PlayStartParams);
	if( error != ST_NO_ERROR)
	{
		memset( &PlayTermParams, 0, sizeof(PLAYREC_PlayTermParams_t));
		PLAYREC_PlayTerm( 0, &PlayTermParams);		
		return DAL_ERROR_PLAYREC_START_FAILED;
	}

	error = PLAYREC_PlayGetParams( 0, &PlayParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_GetParams_FAILED;
	}
	output->vidhandle = PlayParams.VIDHandle;
	output->vpSDhandle = PlayParams.VIEWHandle[1];
	output->vpHDhandle = PlayParams.VIEWHandle[0];
	return DAL_NO_ERROR;
}

static DAL_ERROR_t playrec_stop(void)
{
	PLAYREC_PlayStopParams_t		PlayStopParams;
	PLAYREC_PlayTermParams_t		PlayTermParams;
	ST_ErrorCode_t					error = ST_NO_ERROR;

	memset( &PlayStopParams, 0, sizeof(PLAYREC_PlayStopParams_t));
	PlayStopParams.VideoDisableDisplay = 0;
	PlayStopParams.VideoFreezeMethod.Mode = STVID_FREEZE_MODE_FORCE;
	PlayStopParams.VideoFreezeMethod.Field = STVID_FREEZE_FIELD_CURRENT;
	PlayStopParams.VideoStopMethod = STVID_STOP_WHEN_END_OF_DATA;
	PlayStopParams.AudioFadingMethod.FadeType = STAUD_FADE_SOFT_MUTE;
	PlayStopParams.AudioStopMethod = STAUD_STOP_WHEN_END_OF_DATA;
	
	error = PLAYREC_PlayStop( 0, &PlayStopParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_STOP_FAILED;
	}
	
	memset( &PlayTermParams, 0, sizeof(PLAYREC_PlayTermParams_t));
	error = PLAYREC_PlayTerm( 0, &PlayTermParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_PLAYREC_TERM_FAILED;
	}
	return DAL_NO_ERROR;
}
#endif


