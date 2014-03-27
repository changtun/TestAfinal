
#include <stdlib.h>
#include "dvbplayer.h"
#include "log/anpdebug.h"
#include "npdvb.h"
#include "drv_avctrl.h" 


static char *dvb_chname_str = NULL;
static char *dvb_db_path = NULL;
static int dvb_bug = 1;

void dvb_debug(char *str_temp)
{
    if(dvb_bug == 0)
        return;
    DebugMessage("===   PXWANG	dvb_debug	CallBy Function:%s", str_temp);
}

int NpDvb_Debug(int flag)
{
	dvb_bug = flag;
	return 0;
}


int NpDvb_Init(void)
{
    dvb_debug(__FUNCTION__);
    DrvPlayerInit(1);

    return 0;
}

int NpDvb_term(void)
{
    dvb_debug(__FUNCTION__);
    DrvPlayerUnInit();

    return 0;
}



int NpDvb_Play_c(int f_v, int s_v, int m_v, int se_v, int p_v)
{
    DVBPlayer_Play_Param_t pParam;
    dvb_debug(__FUNCTION__);

    memset( &pParam, 0, sizeof(DVBPlayer_Play_Param_t) );
    pParam.uTunerParam.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    pParam.uTunerParam.tCable.u32Freq = f_v;
    pParam.uTunerParam.tCable.u32SymbRate = s_v;
    pParam.uTunerParam.tCable.eMod = (DVBCore_Modulation_e)m_v;
    pParam.uTunerParam.tCable.u8TunerId = 0;

    pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
    pParam.uProgParam.tServParam.u16ServId = se_v;
    pParam.uProgParam.tServParam.u16PMTPid = p_v;

    DVBPlayer_Play_Program( &pParam );
    return 0;
}


int NpDvb_Play_t(int f_v, int b_v, int se_v, int p_v)
{
    DVBPlayer_Play_Param_t pParam;
    dvb_debug(__FUNCTION__);

    memset( &pParam, 0, sizeof(DVBPlayer_Play_Param_t) );
    pParam.uTunerParam.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBT;
    pParam.uTunerParam.tTerrestrial.u32CentreFreq = f_v;
    pParam.uTunerParam.tTerrestrial.u8BandWidth = b_v;
    pParam.uTunerParam.tTerrestrial.u8TunerId = 0;

    pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
    pParam.uProgParam.tServParam.u16ServId = se_v;
    pParam.uProgParam.tServParam.u16PMTPid = p_v;

    DVBPlayer_Play_Program( &pParam );
    return 0;
}



int NpDvb_Stop(void)
{
    dvb_debug(__FUNCTION__);
    DVBPlayer_Stop();
    return 0;
}

int NpDvb_volume(int volume_v)
{
    int mute = 0;
    dvb_debug(__FUNCTION__);

    if(volume_v == 0)
    {
        mute = 1;
        DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_MUTE, &mute);
        DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_SETVOLUME, &volume_v);
    }
    else
    {
        DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_MUTE, &mute);
        DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_SETVOLUME, &volume_v);
    }
    return 0;
}


int NpDvb_Area(int x, int y, int width, int height)
{
	DRV_AVCtrl_VidWindow_t tVidOut_Window;
	S32 iRet = 0;
    dvb_debug(__FUNCTION__);
	
	tVidOut_Window.uX_Axis  = x;
	tVidOut_Window.uY_Axis  = y;
	tVidOut_Window.uWidth = width;
	tVidOut_Window.uHeight = height;
	tVidOut_Window.uBaseWidth = 1920;
	tVidOut_Window.uBaseHeight = 1080;
	iRet = DVBPlayer_Set_Attr( eDVBPLAYER_ATTR_VID_WINDWOS, (void *)&tVidOut_Window );
    return iRet;
}

int NpDvb_DbPath(char *path_str)
{
    dvb_debug(__FUNCTION__);
    NpDvb_db_str(path_str);

    return 0;
}



void NpDvb_str(char *chname_str)
{
    int ustrlen;
    dvb_debug(__FUNCTION__);

    if(dvb_chname_str != NULL)
    {
        free(dvb_chname_str);
        dvb_chname_str = NULL;
    }

    ustrlen = strlen(chname_str);
    if(ustrlen > 0)
    {
        dvb_chname_str = (char *)malloc(ustrlen + 10);
        memset(dvb_chname_str, 0, ustrlen + 10);
        if(dvb_chname_str == NULL)
        {
            return;
        }
        sprintf( dvb_chname_str, "%s", chname_str);
        DebugMessage("=========   PXWANG	CALL BY %s", dvb_chname_str);
    }

}

void NpDvb_db_str(char *path_str)
{
    int ustrlen;
    dvb_debug(__FUNCTION__);

    if(dvb_db_path != NULL)
    {
        free(dvb_db_path);
        dvb_db_path = NULL;
    }

    ustrlen = strlen(path_str);
    if(ustrlen > 0)
    {
        dvb_db_path = (char *)malloc(ustrlen + 10);
        memset(dvb_db_path, 0, ustrlen + 10);
        if(dvb_db_path == NULL)
        {
            return;
        }
        sprintf( dvb_db_path, "%s", path_str);
    }

}


