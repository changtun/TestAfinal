package com.pbi.dvb.dvbinterface;

import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;

public class NativePlayer
{
    private volatile static NativePlayer player;
    
    private NativePlayer()
    {
    }
    
    public static NativePlayer getInstance()
    {
        if (null == player)
        {
            synchronized (NativePlayer.class)
            {
                if (null == player)
                {
                    player = new NativePlayer();
                }
            }
        }
        return player;
    }
    
    public native int DVBPlayerInit(DVBPlayer_Config_t pConfig);
    
    public native int DVBPlayerDeinit();
    
    // public native int DVBPlayerSetAttr( DVBPlayer_Attr_e Attr/*, void pParam */);
    // public native int DVBPlayerGetAttr( DVBPlayer_Attr_e Attr/*, void pParam */);
    public native int DVBPlayerPlayProgram(DVBPlayer_Play_Param_t pParam);
    
    public native int DVBPlayerSetStopMode(int StopMode); /* 0 : STILL Other : Black */
    
    public native int DVBPlayerStop();
    
    public native int DVBPlayerGetInfo(DVBPlayer_Info_t pInfo);
    
    public native int DVBPlayerMessageInit(MessagePlayer MsgPlayer);
    
    public native int DVBPlayerSwitchAudio(DVBPlayer_SWITCHAUDIO_t param);
    
    public native int DVBPlayerAVCtrlSetVolume(int Volume);
    
    public native int DVBPlayerAVCtrlAudMute(int isMute);

	public native int DVBPlayerAVCtrlSetVPathPara(int isDvb);
    
    public native int DVBPlayerAVCtrlAudMode(int mode);
    
    public native int DVBPlayerAVCtrlGetAudMode();
    
    public native int DVBPlayerAVCtrlGetWindows(DRV_AVCtrl_VidWindow_t tVidWindows);
    
    public native int DVBPlayerAVCtrlSetWindows(DRV_AVCtrl_VidWindow_t tVidWindows, int isCheak);
    
    public native int DVBPlayerGetCaMode();/* ����ֵ0��ʾ������1�Ǽ��Ž�Ŀ */
    
    public native int DVBPlayerPvrMessageInit(MessagePVR MsgPVR);
    
    public native int DVBPlayerPvrMessageUnInit();
    
    // public native int DVBPlayerPvrInit();
    // public native int DVBPlayerPvrDeInit();
    
    public native int DVBPlayerPvrRecStart(DvbPVR_Rec_t tPvrRecInfo);
    
    public native int DVBPlayerPvrRecStop();
    
    public native int DVBPlayerPvrRecGetStatus(PVR_Rec_Status_t tRecStatus);
    
    public native int DVBPlayerPvrRemoveFile(String sFileName);
    
    public native int DVBPlayerPvrPlayStart(DvbPVR_Play_t tPvrPlayInfo);
    
    public native int DVBPlayerPvrPlayStop();
    
    public native int DVBPlayerPvrPlayPause();
    
    public native int DVBPlayerPvrPlayResume();
    
    public native int DVBPlayerPvrPlaySpeedCtrl(int Speed);
    
    public native int DVBPlayerPvrPlayGetFileInfo(PVR_Play_File_Info_t tPvrFileInfo);
    
    public native int DVBPlayerPvrPlayGetStatus(PVR_Play_Status_t tPvrStatus);
    
    public class DRV_AVCtrl_VidWindow_t
    {
        int uX_Axis;
        
        int uY_Axis;
        
        int uWidth;
        
        int uHeight;
        
        public int getWinX()
        {
            return uX_Axis;
        }
        
        public void setWinX(int value)
        {
            uX_Axis = value;
        }
        
        public int getWinY()
        {
            return uY_Axis;
        }
        
        public void setWinY(int value)
        {
            uY_Axis = value;
        }
        
        public int getWinWidth()
        {
            return uWidth;
        }
        
        public void setWinWidth(int value)
        {
            uWidth = value;
        }
        
        public int getWinHeight()
        {
            return uHeight;
        }
        
        public void setWinHeight(int value)
        {
            uHeight = value;
        }
    }
    
    public class DVBPlayer_Config_t
    {
        int eVideoSystem;
        
        int eAspectRatio;
        
        int eSyncType;
        
        int eParentCtrlFlag;
        
        int u8ParentRating;
        
        DVBPlayer_VidOutWindow_t tVidOutWin;
        
        public int geteVideoSystem()
        {
            return eVideoSystem;
        }
        
        public void seteVideoSystem(int eVideoSystem)
        {
            this.eVideoSystem = eVideoSystem;
        }
        
        public int geteAspectRatio()
        {
            return eAspectRatio;
        }
        
        public void seteAspectRatio(int eAspectRatio)
        {
            this.eAspectRatio = eAspectRatio;
        }
        
        public int geteSyncType()
        {
            return eSyncType;
        }
        
        public void seteSyncType(int eSyncType)
        {
            this.eSyncType = eSyncType;
        }
        
        public int geteParentCtrlFlag()
        {
            return eParentCtrlFlag;
        }
        
        public void seteParentCtrlFlag(int eParentCtrlFlag)
        {
            this.eParentCtrlFlag = eParentCtrlFlag;
        }
        
        public int getU8ParentRating()
        {
            return u8ParentRating;
        }
        
        public void setU8ParentRating(int u8ParentRating)
        {
            this.u8ParentRating = u8ParentRating;
        }
        
        public DVBPlayer_VidOutWindow_t gettVidOutWin()
        {
            return tVidOutWin;
        }
        
        public void settVidOutWin(DVBPlayer_VidOutWindow_t tVidOutWin)
        {
            this.tVidOutWin = tVidOutWin;
        }
        
    }
    
    public class DVBPlayer_Serv_Param_t
    {
        int eServLockFlag;
        
        int u16ServId;
        
        int u16PMTPid;
        
        int u16NetId;
        
        int u16TS_Id;
        
        int u16LogicNum;
        
        int u16AudioPID;
        
        public int geteServLockFlag()
        {
            return eServLockFlag;
        }
        
        public void seteServLockFlag(int eServLockFlag)
        {
            this.eServLockFlag = eServLockFlag;
        }
        
        public int getU16ServId()
        {
            return u16ServId;
        }
        
        public void setU16ServId(int u16ServId)
        {
            this.u16ServId = u16ServId;
        }
        
        public int getU16PMTPid()
        {
            return u16PMTPid;
        }
        
        public void setU16PMTPid(int u16pmtPid)
        {
            u16PMTPid = u16pmtPid;
        }
        
        public int getU16LogicNum()
        {
            return u16LogicNum;
        }
        
        public void setU16LogicNum(int u16LogicNum)
        {
            this.u16LogicNum = u16LogicNum;
        }
        
        public int getU16TsId()
        {
            return u16TS_Id;
        }
        
        public void setU16TsId(int u16TsId)
        {
            this.u16TS_Id = u16TsId;
        }
        
        public int getU16NetId()
        {
            return u16NetId;
        }
        
        public void setU16NetId(int u16NetId)
        {
            this.u16NetId = u16NetId;
        }
        
        public int getu16AudioPID()
        {
            return u16AudioPID;
        }
        
        public void setu16AudioPID(int u16Pid)
        {
            this.u16AudioPID = u16Pid;
        }
        
    }
    
    public class DVBPlayer_AVPid_t
    {
        int u16AudioPid;
        
        int u16VideoPid;
        
        int u16PCRPid;
        
        byte u8AudioFmt;
        
        byte u8VideoFmt;
        
        public int getU16AudioPid()
        {
            return u16AudioPid;
        }
        
        public void setU16AudioPid(int u16AudioPid)
        {
            this.u16AudioPid = u16AudioPid;
        }
        
        public int getU16VideoPid()
        {
            return u16VideoPid;
        }
        
        public void setU16VideoPid(int u16VideoPid)
        {
            this.u16VideoPid = u16VideoPid;
        }
        
        public int getU16PCRPid()
        {
            return u16PCRPid;
        }
        
        public void setU16PCRPid(int u16pcrPid)
        {
            u16PCRPid = u16pcrPid;
        }
        
        public byte getU8AudioFmt()
        {
            return u8AudioFmt;
        }
        
        public void setU8AudioFmt(byte u8AudioFmt)
        {
            this.u8AudioFmt = u8AudioFmt;
        }
        
        public byte getU8VideoFmt()
        {
            return u8VideoFmt;
        }
        
        public void setU8VideoFmt(byte u8VideoFmt)
        {
            this.u8VideoFmt = u8VideoFmt;
        }
    }
    
    public class DVBPlayer_SWITCHAUDIO_t
    {
        int u16AudioPid;
        
        int u8AudeoFmt;
        
        public int getU16AudioPid()
        {
            return u16AudioPid;
        }
        
        public void setU16AudioPid(int u16Pid)
        {
            this.u16AudioPid = u16Pid;
        }
        
        public int getU8AudioFmt()
        {
            return u8AudeoFmt;
        }
        
        public void setU8AudioFmt(int u8Fmt)
        {
            this.u8AudeoFmt = u8Fmt;
        }
        
    }
    
    public class DVBPlayer_Play_Param_t
    {
        int eSignalSource;
        
        DVBCore_Cab_Desc_t tCable;
        
        int eProgType; /* 0 ServParam, 1 AVPids */
        
        DVBPlayer_Serv_Param_t tServParam;
        
        DVBPlayer_AVPid_t tAVPids;
        
        public int geteSignalSource()
        {
            return eSignalSource;
        }
        
        public void seteSignalSource(int eSignalSource)
        {
            this.eSignalSource = eSignalSource;
        }
        
        public DVBCore_Cab_Desc_t gettCable()
        {
            return tCable;
        }
        
        public void settCable(DVBCore_Cab_Desc_t tCable)
        {
            this.tCable = tCable;
        }
        
        public int geteProgType()
        {
            return eProgType;
        }
        
        public void seteProgType(int eProgType)
        {
            this.eProgType = eProgType;
        }
        
        public DVBPlayer_Serv_Param_t gettServParam()
        {
            return tServParam;
        }
        
        public void settServParam(DVBPlayer_Serv_Param_t tServParam)
        {
            this.tServParam = tServParam;
        }
        
        public DVBPlayer_AVPid_t gettAVPids()
        {
            return tAVPids;
        }
        
        public void settAVPids(DVBPlayer_AVPid_t tAVPids)
        {
            this.tAVPids = tAVPids;
        }
        
    }
    
    public class DVBPlayer_VidOutWindow_t
    {
        int eZoomMode;
        
        int s32PositionX;
        
        int s32PositionY;
        
        int u32WinWidth;
        
        int u32WinHight;
        
        public int geteZoomMode()
        {
            return eZoomMode;
        }
        
        public void seteZoomMode(int eZoomMode)
        {
            this.eZoomMode = eZoomMode;
        }
        
        public int getS32PositionX()
        {
            return s32PositionX;
        }
        
        public void setS32PositionX(int s32PositionX)
        {
            this.s32PositionX = s32PositionX;
        }
        
        public int getS32PositionY()
        {
            return s32PositionY;
        }
        
        public void setS32PositionY(int s32PositionY)
        {
            this.s32PositionY = s32PositionY;
        }
        
        public int getU32WinWidth()
        {
            return u32WinWidth;
        }
        
        public void setU32WinWidth(int u32WinWidth)
        {
            this.u32WinWidth = u32WinWidth;
        }
        
        public int getU32WinHight()
        {
            return u32WinHight;
        }
        
        public void setU32WinHight(int u32WinHight)
        {
            this.u32WinHight = u32WinHight;
        }
        
    }
    
    public class DVBPlayer_Info_t
    {
        int eRunningStatus;
        
        int eTunerStatus;
        
        int eProgType;
        
        int u16ServiceId;
        
        int u16PMTPid;
        
        int ePMTStatus;
        
        int u16VideoPid;
        
        int u16AudioPid;
        
        int u16PCRPid;
        
        public int getU16PCRPid()
        {
            return u16PCRPid;
        }
        
        public void setU16PCRPid(int u16pcrPid)
        {
            u16PCRPid = u16pcrPid;
        }
        
        public int geteRunningStatus()
        {
            return eRunningStatus;
        }
        
        public void seteRunningStatus(int eRunningStatus)
        {
            this.eRunningStatus = eRunningStatus;
        }
        
        public int geteTunerStatus()
        {
            return eTunerStatus;
        }
        
        public void seteTunerStatus(int eTunerStatus)
        {
            this.eTunerStatus = eTunerStatus;
        }
        
        public int geteProgType()
        {
            return eProgType;
        }
        
        public void seteProgType(int eProgType)
        {
            this.eProgType = eProgType;
        }
        
        public int getU16ServiceId()
        {
            return u16ServiceId;
        }
        
        public void setU16ServiceId(int u16ServiceId)
        {
            this.u16ServiceId = u16ServiceId;
        }
        
        public int getU16PMTPid()
        {
            return u16PMTPid;
        }
        
        public void setU16PMTPid(int u16pmtPid)
        {
            u16PMTPid = u16pmtPid;
        }
        
        public int getePMTStatus()
        {
            return ePMTStatus;
        }
        
        public void setePMTStatus(int ePMTStatus)
        {
            this.ePMTStatus = ePMTStatus;
        }
        
        public int getU16VideoPid()
        {
            return u16VideoPid;
        }
        
        public void setU16VideoPid(int u16VideoPid)
        {
            this.u16VideoPid = u16VideoPid;
        }
        
        public int getU16AudioPid()
        {
            return u16AudioPid;
        }
        
        public void setU16AudioPid(int u16AudioPid)
        {
            this.u16AudioPid = u16AudioPid;
        }
        
    }
    
    public class PVR_Play_Status_t
    {
        int eState;
        
        int eSpeed;
        
        double u64CurPlayPos;
        
        int u32CurPlayFrame;
        
        int u32CurPlayTimeInMs;
        
        /**
         * ��ȡ eState
         * 
         * @return ���� eState
         */
        public int geteState()
        {
            return eState;
        }
        
        /**
         * ���� eState
         * 
         * @param ��eState���и�ֵ
         */
        public void seteState(int eState)
        {
            this.eState = eState;
        }
        
        /**
         * ��ȡ eSpeed
         * 
         * @return ���� eSpeed
         */
        public int geteSpeed()
        {
            return eSpeed;
        }
        
        /**
         * ���� eSpeed
         * 
         * @param ��eSpeed���и�ֵ
         */
        public void seteSpeed(int eSpeed)
        {
            this.eSpeed = eSpeed;
        }
        
        /**
         * ��ȡ u64CurPlayPos
         * 
         * @return ���� u64CurPlayPos
         */
        public double getU64CurPlayPos()
        {
            return u64CurPlayPos;
        }
        
        /**
         * ���� u64CurPlayPos
         * 
         * @param ��u64CurPlayPos���и�ֵ
         */
        public void setU64CurPlayPos(double u64CurPlayPos)
        {
            this.u64CurPlayPos = u64CurPlayPos;
        }
        
        /**
         * ��ȡ u32CurPlayFrame
         * 
         * @return ���� u32CurPlayFrame
         */
        public int getU32CurPlayFrame()
        {
            return u32CurPlayFrame;
        }
        
        /**
         * ���� u32CurPlayFrame
         * 
         * @param ��u32CurPlayFrame���и�ֵ
         */
        public void setU32CurPlayFrame(int u32CurPlayFrame)
        {
            this.u32CurPlayFrame = u32CurPlayFrame;
        }
        
        /**
         * ��ȡ u32CurPlayTimeInMs
         * 
         * @return ���� u32CurPlayTimeInMs
         */
        public int getU32CurPlayTimeInMs()
        {
            return u32CurPlayTimeInMs;
        }
        
        /**
         * ���� u32CurPlayTimeInMs
         * 
         * @param ��u32CurPlayTimeInMs���и�ֵ
         */
        public void setU32CurPlayTimeInMs(int u32CurPlayTimeInMs)
        {
            this.u32CurPlayTimeInMs = u32CurPlayTimeInMs;
        }
        
    }
    
    public class PVR_Play_File_Info_t
    {
        public int eIndxType;
        
        public int u32FrameNum;
        
        public int u32StartTimeInMs;
        
        public int u32EndTimeInMs;
        
        public double u64ValidSizeInByte;
        
        /**
         * ��ȡ eIndxType
         * 
         * @return ���� eIndxType
         */
        public int geteIndxType()
        {
            return eIndxType;
        }
        
        /**
         * ���� eIndxType
         * 
         * @param ��eIndxType���и�ֵ
         */
        public void seteIndxType(int eIndxType)
        {
            this.eIndxType = eIndxType;
        }
        
        /**
         * ��ȡ u32FrameNum
         * 
         * @return ���� u32FrameNum
         */
        public int getU32FrameNum()
        {
            return u32FrameNum;
        }
        
        /**
         * ���� u32FrameNum
         * 
         * @param ��u32FrameNum���и�ֵ
         */
        public void setU32FrameNum(int u32FrameNum)
        {
            this.u32FrameNum = u32FrameNum;
        }
        
        /**
         * ��ȡ u32StartTimeInMs
         * 
         * @return ���� u32StartTimeInMs
         */
        public int getU32StartTimeInMs()
        {
            return u32StartTimeInMs;
        }
        
        /**
         * ���� u32StartTimeInMs
         * 
         * @param ��u32StartTimeInMs���и�ֵ
         */
        public void setU32StartTimeInMs(int u32StartTimeInMs)
        {
            this.u32StartTimeInMs = u32StartTimeInMs;
        }
        
        /**
         * ��ȡ u32EndTimeInMs
         * 
         * @return ���� u32EndTimeInMs
         */
        public int getU32EndTimeInMs()
        {
            return u32EndTimeInMs;
        }
        
        /**
         * ���� u32EndTimeInMs
         * 
         * @param ��u32EndTimeInMs���и�ֵ
         */
        public void setU32EndTimeInMs(int u32EndTimeInMs)
        {
            this.u32EndTimeInMs = u32EndTimeInMs;
        }
        
        /**
         * ��ȡ u64ValidSizeInByte
         * 
         * @return ���� u64ValidSizeInByte
         */
        public double getU64ValidSizeInByte()
        {
            return u64ValidSizeInByte;
        }
        
        /**
         * ���� u64ValidSizeInByte
         * 
         * @param ��u64ValidSizeInByte���и�ֵ
         */
        public void setU64ValidSizeInByte(double u64ValidSizeInByte)
        {
            this.u64ValidSizeInByte = u64ValidSizeInByte;
        }
        
    }
    
    public class PVR_Time_Rec_t
    {
        int u32AudioNum;
        
        int[] u32AudioPid;
        
        int[] u32AudioFmt;
        
        int u32VideoPid;
        
        int u32VideoFmt;
    }
    
    public class DvbPVR_Rec_t
    {
        public int u8RecFlag; /* 0 : BackRec 1: TimeRec */
        
        public int u16PmtPid;
        
        public int u16ServId;
        
        public String sName; /* MaxLength 128 */
        
        public double u64MaxFileLength; /* 0 : MAX */
        
        public int u16LogicNum;
        
        /* BackRec == u8RecFlag */
        public int eTunerType;
        
        public DVBCore_Cab_Desc_t tCableInfo;
        /* TimeRec == u8RecFlag */
        // PVR_Time_Rec_t tTimeRec;
    }
    
    public class DvbPVR_Play_t
    {
        public int PvrPlayerType;/* 0 : DVB ����ģʽ 1 : PVR ����ģʽ */
        
        public String sName;
    }
    
    public class PVR_Rec_Status_t
    {
        // 0 δ��ʼ�� 1 ��ʼ�� 2 ¼���� 3 ��ͣ�� 4 ����ֹͣ 5 �Ѿ�ֹͣ 6 ��Ч��״ֵ̬
        public int enState;
        
        public int u32Reserved;
        
        public double u64CurWritePos;
        
        public int u32CurWriteFrame;
        
        public int u32CurTimeInMs;
        
        public int u32StartTimeInMs;
        
        public int u32EndTimeInMs;
        
        public int u32BufSize;
        
        public int u32UsedSize;
        
        /**
         * ��ȡ enState
         * 
         * @return ���� enState
         */
        public int getEnState()
        {
            return enState;
        }
        
        /**
         * ���� enState
         * 
         * @param ��enState���и�ֵ
         */
        public void setEnState(int enState)
        {
            this.enState = enState;
        }
        
        /**
         * ��ȡ u32Reserved
         * 
         * @return ���� u32Reserved
         */
        public int getU32Reserved()
        {
            return u32Reserved;
        }
        
        /**
         * ���� u32Reserved
         * 
         * @param ��u32Reserved���и�ֵ
         */
        public void setU32Reserved(int u32Reserved)
        {
            this.u32Reserved = u32Reserved;
        }
        
        /**
         * ��ȡ u64CurWritePos
         * 
         * @return ���� u64CurWritePos
         */
        public double getU64CurWritePos()
        {
            return u64CurWritePos;
        }
        
        /**
         * ���� u64CurWritePos
         * 
         * @param ��u64CurWritePos���и�ֵ
         */
        public void setU64CurWritePos(double u64CurWritePos)
        {
            this.u64CurWritePos = u64CurWritePos;
        }
        
        /**
         * ��ȡ u32CurWriteFrame
         * 
         * @return ���� u32CurWriteFrame
         */
        public int getU32CurWriteFrame()
        {
            return u32CurWriteFrame;
        }
        
        /**
         * ���� u32CurWriteFrame
         * 
         * @param ��u32CurWriteFrame���и�ֵ
         */
        public void setU32CurWriteFrame(int u32CurWriteFrame)
        {
            this.u32CurWriteFrame = u32CurWriteFrame;
        }
        
        /**
         * ��ȡ u32CurTimeInMs
         * 
         * @return ���� u32CurTimeInMs
         */
        public int getU32CurTimeInMs()
        {
            return u32CurTimeInMs;
        }
        
        /**
         * ���� u32CurTimeInMs
         * 
         * @param ��u32CurTimeInMs���и�ֵ
         */
        public void setU32CurTimeInMs(int u32CurTimeInMs)
        {
            this.u32CurTimeInMs = u32CurTimeInMs;
        }
        
        /**
         * ��ȡ u32StartTimeInMs
         * 
         * @return ���� u32StartTimeInMs
         */
        public int getU32StartTimeInMs()
        {
            return u32StartTimeInMs;
        }
        
        /**
         * ���� u32StartTimeInMs
         * 
         * @param ��u32StartTimeInMs���и�ֵ
         */
        public void setU32StartTimeInMs(int u32StartTimeInMs)
        {
            this.u32StartTimeInMs = u32StartTimeInMs;
        }
        
        /**
         * ��ȡ u32EndTimeInMs
         * 
         * @return ���� u32EndTimeInMs
         */
        public int getU32EndTimeInMs()
        {
            return u32EndTimeInMs;
        }
        
        /**
         * ���� u32EndTimeInMs
         * 
         * @param ��u32EndTimeInMs���и�ֵ
         */
        public void setU32EndTimeInMs(int u32EndTimeInMs)
        {
            this.u32EndTimeInMs = u32EndTimeInMs;
        }
        
        /**
         * ��ȡ u32BufSize
         * 
         * @return ���� u32BufSize
         */
        public int getU32BufSize()
        {
            return u32BufSize;
        }
        
        /**
         * ���� u32BufSize
         * 
         * @param ��u32BufSize���и�ֵ
         */
        public void setU32BufSize(int u32BufSize)
        {
            this.u32BufSize = u32BufSize;
        }
        
        /**
         * ��ȡ u32UsedSize
         * 
         * @return ���� u32UsedSize
         */
        public int getU32UsedSize()
        {
            return u32UsedSize;
        }
        
        /**
         * ���� u32UsedSize
         * 
         * @param ��u32UsedSize���и�ֵ
         */
        public void setU32UsedSize(int u32UsedSize)
        {
            this.u32UsedSize = u32UsedSize;
        }
        
    }
}
