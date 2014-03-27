#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "hi_unf_hdmi.h"
#include "hi_unf_disp.h"

#include "drv_hdmi.h"


typedef struct hiHDMI_ARGS_S
{
    HI_UNF_HDMI_ID_E  enHdmi;
    HI_UNF_ENC_FMT_E  enWantFmt;
    HI_UNF_SAMPLE_RATE_E AudioFreq;
    HI_U32            AduioBitPerSample;
    HI_U32            AudioChannelNum;
    HI_UNF_SND_INTERFACE_E AudioInputType;
    HI_U32            ForceHDMIFlag;
}HDMI_ARGS_S;

typedef void (*User_HDMI_CallBack)(HI_UNF_HDMI_EVENT_TYPE_E event, HI_VOID *pPrivateData);

static HDMI_ARGS_S g_stHdmiArgs;
HI_U32 g_HDCPFlag         = HI_FALSE;
HI_U32 g_AudioFreq        = HI_UNF_SAMPLE_RATE_48K;
HI_U32 g_AduioBitPerSample= 16;
HI_U32 g_AudioChannelNum  = 2;
HI_U32 g_AudioInputType   = HI_UNF_SND_INTERFACE_I2S;//HI_UNF_SND_INTERFACE_SPDIF;
//HI_U32 g_Audiotype        = 0;
//HI_U32 g_ForceDVIFlag     = HI_FALSE;
HI_U32 g_HDMI_Bebug       = HI_FALSE;
HI_U32 g_HDMIUserCallbackFlag = HI_FALSE;
HI_U32 g_HDMIForceMode    = HI_UNF_HDMI_FORCE_HDMI;//HI_UNF_HDMI_FORCE_NULL;

User_HDMI_CallBack pfnHdmiUserCallback = NULL;



void HDMI_HotPlug_Proc(HI_VOID *pPrivateData)
{
    HI_S32          ret = HI_SUCCESS;
    HDMI_ARGS_S     *pArgs  = (HDMI_ARGS_S*)pPrivateData;
    HI_HANDLE       hHdmi   =  pArgs->enHdmi;
    HI_UNF_HDMI_ATTR_S             stHdmiAttr;
    HI_UNF_HDMI_INFOFRAME_S        stInfoFrame;
    HI_UNF_HDMI_SINK_CAPABILITY_S  stSinkCap;
    HI_UNF_ENC_FMT_E               enTargetFmt;
    HI_UNF_HDMI_COLORSPACE_E       enColorimetry;
    HI_UNF_HDMI_VIDEO_MODE_E       enVidOutMode;
    HI_UNF_ASPECT_RATIO_E          enAspectRate;
    
    HI_UNF_HDMI_AVI_INFOFRAME_VER2_S *pstAVIInfoframe;
    HI_UNF_HDMI_AUD_INFOFRAME_VER1_S *pstAUDInfoframe;
    static HI_U8 u8FirstTimeSetting = HI_TRUE;
   
    printf("\n --- Get HDMI event: HOTPLUG. --- \n"); 

    ret = HI_UNF_HDMI_GetSinkCapability(hHdmi, &stSinkCap); 
    //HDMI_PrintSinkCap(&stSinkCap);
    if (HI_FALSE == stSinkCap.bConnected)
    {
        printf("No Connect\n");
        return;
    }
    
    ret = HI_UNF_HDMI_GetAttr(hHdmi, &stHdmiAttr);
    HI_UNF_DISP_GetFormat(HI_UNF_DISP_HD0, &enTargetFmt); /* set display's format to targetFormat */
    stHdmiAttr.enVideoFmt = enTargetFmt;

    if (HI_TRUE == stSinkCap.bSupportYCbCr)
    {
        enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_YCBCR444;
    }
    else
    {
        enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_RGB444;
    }

    //enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_RGB444;printf("force to RGB444\n");
    //enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_YCBCR444;printf("force to YCBCR444\n");
    //enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_YCBCR422;printf("force to YCBCR422\n");
    
    enColorimetry = HDMI_COLORIMETRY_ITU709;
    enAspectRate  = HI_UNF_ASPECT_RATIO_16TO9;
    
    if (HI_TRUE == stSinkCap.bSupportHdmi)
    {
        /* New function to set AVI Infoframe */
        //printf("*** SetInfoFrame for AVI Infoframe\n");        
        memset(&stInfoFrame, 0, sizeof(stInfoFrame));
        HI_UNF_HDMI_GetInfoFrame(hHdmi, HI_INFOFRAME_TYPE_AVI, &stInfoFrame);
        stInfoFrame.enInfoFrameType = HI_INFOFRAME_TYPE_AVI;
        pstAVIInfoframe = (HI_UNF_HDMI_AVI_INFOFRAME_VER2_S *)&(stInfoFrame.unInforUnit.stAVIInfoFrame);

        if(HI_UNF_ENC_FMT_1080P_60 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080P_60;
        }
        else if(HI_UNF_ENC_FMT_1080P_50 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080P_50;
        }
        else if(HI_UNF_ENC_FMT_1080P_30 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080P_30;
        }
        else if(HI_UNF_ENC_FMT_1080P_25 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080P_25;
        }
        else if(HI_UNF_ENC_FMT_1080P_24 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080P_24;
        }
        else if(HI_UNF_ENC_FMT_1080i_60 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080i_60;
        }
        else if(HI_UNF_ENC_FMT_1080i_50 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_1080i_50;
        }
        else if(HI_UNF_ENC_FMT_720P_60 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_720P_60;
        }
        else if(HI_UNF_ENC_FMT_720P_50 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_720P_50;
        }
        else if(HI_UNF_ENC_FMT_576P_50 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_576P_50;
            enColorimetry = HDMI_COLORIMETRY_ITU601;
            enAspectRate  = HI_UNF_ASPECT_RATIO_4TO3; //标准输出
            if (pstAVIInfoframe->enAspectRatio == HI_UNF_ASPECT_RATIO_16TO9)
            {
                /* 热插拔以前采用的是576p_50 16:9的模式，不需要强制改变 */
                enAspectRate = HI_UNF_ASPECT_RATIO_16TO9;
            }
        }
        else if(HI_UNF_ENC_FMT_480P_60 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_480P_60;
            enColorimetry = HDMI_COLORIMETRY_ITU601;
            enAspectRate  = HI_UNF_ASPECT_RATIO_4TO3; //标准输出
            if (pstAVIInfoframe->enAspectRatio == HI_UNF_ASPECT_RATIO_16TO9)
            {
                /* 热插拔以前采用的是576p_50 16:9的模式，不需要强制改变 */
                enAspectRate = HI_UNF_ASPECT_RATIO_16TO9;
            }
        }
        else if(HI_UNF_ENC_FMT_PAL == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_PAL;
            enColorimetry = HDMI_COLORIMETRY_ITU601;
            enAspectRate  = HI_UNF_ASPECT_RATIO_4TO3; //标准输出
            if (pstAVIInfoframe->enAspectRatio == HI_UNF_ASPECT_RATIO_16TO9)
            {
                /* 热插拔以前采用的是576p_50 16:9的模式，不需要强制改变 */
                enAspectRate = HI_UNF_ASPECT_RATIO_16TO9;
            }
        }
        else if(HI_UNF_ENC_FMT_NTSC == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_NTSC;
            enColorimetry = HDMI_COLORIMETRY_ITU601;
            enAspectRate  = HI_UNF_ASPECT_RATIO_4TO3; //标准输出
            if (pstAVIInfoframe->enAspectRatio == HI_UNF_ASPECT_RATIO_16TO9)
            {
                /* 热插拔以前采用的是576p_50 16:9的模式，不需要强制改变 */
                enAspectRate = HI_UNF_ASPECT_RATIO_16TO9;
            }
        }
        else if(HI_UNF_ENC_FMT_861D_640X480_60 == enTargetFmt)
        {
            pstAVIInfoframe->enTimingMode = HI_UNF_ENC_FMT_861D_640X480_60;
            enColorimetry = HDMI_COLORIMETRY_ITU601;
            enAspectRate  = HI_UNF_ASPECT_RATIO_4TO3;
        }
        else
        {
            printf("***Error: Unsupport Video timing:0x%x\n***", enTargetFmt);
        }
        /* Deal with xvYCC MODE */
        if (HI_TRUE == stHdmiAttr.bxvYCCMode)
        {
            if (HDMI_COLORIMETRY_ITU601 == enColorimetry)
            {
                enColorimetry = HDMI_COLORIMETRY_XVYCC_601;
            }
            else if (HDMI_COLORIMETRY_ITU709 == enColorimetry)
            {
                enColorimetry = HDMI_COLORIMETRY_XVYCC_709;
            }
        }
        pstAVIInfoframe->enOutputType            = enVidOutMode;
        pstAVIInfoframe->bActive_Infor_Present   = HI_TRUE;
        pstAVIInfoframe->enBarInfo               = HDMI_BAR_INFO_NOT_VALID;
        pstAVIInfoframe->enScanInfo              = HDMI_SCAN_INFO_NO_DATA;//HDMI_SCAN_INFO_OVERSCANNED;
        pstAVIInfoframe->enColorimetry           = enColorimetry;
        pstAVIInfoframe->enAspectRatio           = enAspectRate;
        pstAVIInfoframe->enActiveAspectRatio     = enAspectRate;
        pstAVIInfoframe->enPictureScaling        = HDMI_PICTURE_NON_UNIFORM_SCALING;
        pstAVIInfoframe->enRGBQuantization       = HDMI_RGB_QUANTIZATION_DEFAULT_RANGE;
        pstAVIInfoframe->bIsITContent            = HI_FALSE;
        pstAVIInfoframe->u32PixelRepetition      = HI_FALSE;
        pstAVIInfoframe->u32LineNEndofTopBar     = 0;  /* We can determine it in hi_unf_hdmi.c */
        pstAVIInfoframe->u32LineNStartofBotBar   = 0;  /* We can determine it in hi_unf_hdmi.c */
        pstAVIInfoframe->u32PixelNEndofLeftBar   = 0;  /* We can determine it in hi_unf_hdmi.c */
        pstAVIInfoframe->u32PixelNStartofRightBar= 0;  /* We can determine it in hi_unf_hdmi.c */
        ret = HI_UNF_HDMI_SetInfoFrame(hHdmi, &stInfoFrame);
        
        /* New function to set Audio Infoframe */
        /* HDMI requires the CT, SS and SF fields to be set to 0 ("Refer to Stream Header") 
           as these items are carried in the audio stream.*/
        //printf("*** SetInfoFrame for AUDIO Infoframe\n");
        memset(&stInfoFrame, 0, sizeof(stInfoFrame));
        stInfoFrame.enInfoFrameType = HI_INFOFRAME_TYPE_AUDIO;
        pstAUDInfoframe = (HI_UNF_HDMI_AUD_INFOFRAME_VER1_S *)&(stInfoFrame.unInforUnit.stAUDInfoFrame);
        pstAUDInfoframe->u32ChannelCount      = g_stHdmiArgs.AudioChannelNum; //maybe 2 or 8;
        pstAUDInfoframe->enCodingType         = HDMI_AUDIO_CODING_REFER_STREAM_HEAD;
        pstAUDInfoframe->u32SampleSize        = HI_UNF_HDMI_DEFAULT_SETTING;
        //pstAUDInfoframe->u32SamplingFrequency = HI_UNF_HDMI_DEFAULT_SETTING;
        if(g_stHdmiArgs.AudioInputType == HI_UNF_SND_INTERFACE_I2S)
        {
            pstAUDInfoframe->u32SamplingFrequency = HI_UNF_HDMI_DEFAULT_SETTING;
        }
        else
        {
            HI_U32 Audio_Freq = 0;
            
            HI_UNF_SND_GetSampleRate(HI_UNF_SND_0, &Audio_Freq);
            //printf("SPDIF Audio_Freq:%d\n", Audio_Freq);
            pstAUDInfoframe->u32SamplingFrequency = Audio_Freq;
        }
        pstAUDInfoframe->u32ChannelAlloc      = 0;
        pstAUDInfoframe->u32LevelShift        = 0;
        pstAUDInfoframe->u32DownmixInhibit    = HI_FALSE;
        ret = HI_UNF_HDMI_SetInfoFrame(hHdmi, &stInfoFrame);

        stHdmiAttr.bEnableHdmi         = HI_TRUE;
        stHdmiAttr.bEnableAviInfoFrame = HI_TRUE;
        stHdmiAttr.bEnableAudInfoFrame = HI_TRUE;
        
        /* Default we set audio below parm */
        stHdmiAttr.bEnableVideo       = HI_TRUE;
        stHdmiAttr.bEnableAudio       = HI_TRUE;
        stHdmiAttr.bIsMultiChannel    = HI_FALSE;
        stHdmiAttr.enSampleRate       = g_stHdmiArgs.AudioFreq;
        stHdmiAttr.enBitDepth         = g_stHdmiArgs.AduioBitPerSample;
        stHdmiAttr.enSoundIntf        = g_stHdmiArgs.AudioInputType;
        stHdmiAttr.u8I2SCtlVbit       = HI_FALSE;
    }
    else
    {
        stHdmiAttr.bEnableHdmi         = HI_FALSE;
        stHdmiAttr.bEnableVideo        = HI_TRUE;
        stHdmiAttr.bEnableAudio        = HI_FALSE;
        stHdmiAttr.bEnableAviInfoFrame = HI_FALSE;
        stHdmiAttr.bEnableAudInfoFrame = HI_FALSE;      
        printf("DVI use output format:RGB444 \n");
        enVidOutMode = HI_UNF_HDMI_VIDEO_MODE_RGB444;
    }
    
    //stHdmiAttr.bEnableHdmi        = HI_TRUE;//HI_FALSE;//HI_TRUE;  // HI_FALSE means DVI 

    /* Default we set video below parm */
    stHdmiAttr.enVideoFmt         = enTargetFmt;
    stHdmiAttr.enVidOutMode       = enVidOutMode;
    stHdmiAttr.enDeepColorMode    = HI_UNF_HDMI_DEEP_COLOR_OFF;
    /* Other inforframe Flag */
    stHdmiAttr.bEnableSpdInfoFrame = HI_FALSE;
    stHdmiAttr.bEnableMpegInfoFrame= HI_FALSE;
    stHdmiAttr.bDebugFlag          = g_HDMI_Bebug;
    
    if (u8FirstTimeSetting == HI_TRUE)
    {
        u8FirstTimeSetting = HI_FALSE;
        if (g_HDCPFlag == HI_TRUE)
        {
            stHdmiAttr.bHDCPEnable = HI_TRUE;//Enable HDCP
        }
        else
        {
            stHdmiAttr.bHDCPEnable= HI_FALSE;
        }
    }
    else
    {
        //HDCP Enable use default setting!!
    }
    
    //HDMI_PrintAttr(&stHdmiAttr);
    ret = HI_UNF_HDMI_SetAttr(hHdmi, &stHdmiAttr);
    /* HI_UNF_HDMI_SetAttr must before HI_UNF_HDMI_Start! */
    ret = HI_UNF_HDMI_Start(hHdmi);
    
    return;
}

HI_VOID HDMI_UnPlug_Proc(HI_VOID *pPrivateData)
{
    HDMI_ARGS_S     *pArgs  = (HDMI_ARGS_S*)pPrivateData;
    HI_HANDLE       hHdmi   =  pArgs->enHdmi;
    
    printf("\n --- Get HDMI event: UnPlug. --- \n");
    HI_UNF_HDMI_Stop(hHdmi);
    return;
}

HI_VOID HDMI_HdcpFail_Proc(HI_VOID *pPrivateData)
{
    printf("\n --- Get HDMI event: HDCP_FAIL. --- \n");
    return;
}

HI_VOID HDMI_HdcpSuccess_Proc(HI_VOID *pPrivateData)
{
    printf("\n --- Get HDMI event: HDCP_SUCCESS. --- \n");
    return;
}


HI_VOID HDMI_Event_Proc(HI_UNF_HDMI_EVENT_TYPE_E event, HI_VOID *pPrivateData)
{
    
    switch ( event )
    {
        case HI_UNF_HDMI_EVENT_HOTPLUG:
            HDMI_HotPlug_Proc(pPrivateData);
            break;
        case HI_UNF_HDMI_EVENT_NO_PLUG:
            HDMI_UnPlug_Proc(pPrivateData);
            break;
        case HI_UNF_HDMI_EVENT_EDID_FAIL:
            break;
        case HI_UNF_HDMI_EVENT_HDCP_FAIL:
            HDMI_HdcpFail_Proc(pPrivateData);
            break;
        case HI_UNF_HDMI_EVENT_HDCP_SUCCESS:
            HDMI_HdcpSuccess_Proc(pPrivateData);
            break;
        default:
            break;
    }
    /* Private Usage */
    if ((g_HDMIUserCallbackFlag == HI_TRUE) && (pfnHdmiUserCallback != NULL))
    {
        pfnHdmiUserCallback(event, NULL);
    }
    
    return;
}

/*
hotplug:0x10203030 0x2 or 0x10203060 0x3
cec：0x1020303c 0x2 or 0x1020305c 0x3
scl&sda:0x10203044 0x1
*/
HI_VOID HDMI_PinConfig(HI_VOID)
{
    HI_SYS_WriteRegister(0x10203030, 0x2);
    HI_SYS_WriteRegister(0x1020303c, 0x2);
    HI_SYS_WriteRegister(0x10203044, 0x1);
}

HI_S32 HIADP_HDMI_SetAdecAttr(HI_UNF_SND_INTERFACE_E enInterface, HI_UNF_SAMPLE_RATE_E enRate)
{
    if (enRate)
    {
        g_AudioFreq = enRate;
    }

    if (enInterface)
    {
        g_AudioInputType = enInterface;
    }
    return 0;
}

HI_S32 HIADP_HDMI_Init(HI_UNF_HDMI_ID_E enHDMIId, HI_UNF_ENC_FMT_E enWantFmt)
{
    HI_U32 Ret = HI_FAILURE;
    HI_UNF_HDMI_INIT_PARA_S stHdmiInitParam;
    
    HDMI_PinConfig();
    
    g_stHdmiArgs.enHdmi       = enHDMIId;
    g_stHdmiArgs.enWantFmt    = enWantFmt;
    if(g_AudioFreq == HI_UNF_SAMPLE_RATE_UNKNOWN)
    {
        g_stHdmiArgs.AudioFreq         = HI_UNF_SAMPLE_RATE_48K;
        g_stHdmiArgs.AduioBitPerSample = 16;
        g_stHdmiArgs.AudioChannelNum   = 2;
        g_stHdmiArgs.AudioInputType    = HI_UNF_SND_INTERFACE_I2S;//HI_UNF_SND_INTERFACE_SPDIF;
        g_stHdmiArgs.ForceHDMIFlag     = HI_FALSE;
    }
    else
    {
        g_stHdmiArgs.AudioFreq         = g_AudioFreq;
        g_stHdmiArgs.AduioBitPerSample = g_AduioBitPerSample;
        g_stHdmiArgs.AudioChannelNum   = g_AudioChannelNum;
        g_stHdmiArgs.AudioInputType    = g_AudioInputType;
        g_stHdmiArgs.ForceHDMIFlag     = HI_FALSE;
    }

    stHdmiInitParam.pfnHdmiEventCallback = HDMI_Event_Proc;
    stHdmiInitParam.pCallBackArgs        = &g_stHdmiArgs;
    stHdmiInitParam.enForceMode          = g_HDMIForceMode;//HI_UNF_HDMI_FORCE_NULL;
    printf("HDMI Init Mode:%d\n", stHdmiInitParam.enForceMode);
    Ret = HI_UNF_HDMI_Init(&stHdmiInitParam);
    if (HI_SUCCESS != Ret)
    {
        printf("HI_UNF_HDMI_Init failed:%#x\n",Ret);
        return HI_FAILURE;
    }

    Ret = HI_UNF_HDMI_Open(enHDMIId);
    if (Ret != HI_SUCCESS)
    {
        printf("HI_UNF_HDMI_Open failed:%#x\n",Ret);
        HI_UNF_HDMI_DeInit();
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


HI_S32 HIADP_HDMI_DeInit(HI_UNF_HDMI_ID_E enHDMIId)
{
    HI_UNF_HDMI_Stop(enHDMIId);
    HI_UNF_HDMI_Close(enHDMIId);
    HI_UNF_HDMI_DeInit();

    return HI_SUCCESS;
}


