/*
 * File Name:  SoundtrackBean.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.domain;

import java.util.ArrayList;

import android.content.Context;

import com.pbi.dvb.R;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.dvbinterface.MessagePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_SWITCHAUDIO_t;
import com.pbi.dvb.utils.LogUtils;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author 姓名 工号
 * @version [版本号, 2013-6-17]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class SoundtrackBean
{
    private static final String TAG = "SoundtrackBean";
    
    private String soundtrack; // 0 立体声 1 左声道 2 右声道 3 混合
    
    private Context context;
    
    private NativePlayer player;
    
    private ArrayList<MessagePlayer.oneCHaudio> listLang;
    
    private int langPos;
    
    public SoundtrackBean(Context context)
    {
        super();
        this.context = context;
        this.player = NativePlayer.getInstance();
        this.langPos = 0;
    }
    
    public void setListLang(ArrayList<MessagePlayer.oneCHaudio> listLang)
    {
        this.listLang = listLang;
    }
    
    public String getCurrentLang(int serviceId,int logicalNumber)
    {
        ServiceInfoDao dao = new ServiceInfoDaoImpl(this.context);
        ServiceInfoBean service = dao.getChannelInfoByServiceId(serviceId, logicalNumber);
        String lang = null;
        if (null != listLang && listLang.size() > 0)
        {
            //init default lang.
            lang = listLang.get(0).Getlanguagecode();
            for(int i=0;i<listLang.size();i++)
            {
//                LogUtils.printLog(1, 3, TAG, "--->>> current lang is: "+listLang.get(0).Getlanguagecode());
                if(listLang.get(i).Getaudiopid() == service.getAudioPid())
                {
                    lang =  listLang.get(i).Getlanguagecode();
                }
            }
        }
        return lang;
    }
    
    public String getNextLang()
    {
        MessagePlayer.oneCHaudio chAudio;
        int aPid = 0;
        int aPtype = 0;
        String audioLang = "DEF";
        if (null != listLang)
        {
            LogUtils.printLog(1, 2, TAG, "--->>> language sorts: " + listLang.size());
            
            if (langPos < listLang.size())
            {
                chAudio = listLang.get(langPos);
                aPid = chAudio.Getaudiopid();
                aPtype = chAudio.Getaudiotype();
                audioLang = chAudio.Getlanguagecode();
                
                langPos++;
            }
            else
            {
                chAudio = listLang.get(0);
                aPid = chAudio.Getaudiopid();
                aPtype = chAudio.Getaudiotype();
                audioLang = chAudio.Getlanguagecode();
                langPos = 0;
            }
            if (listLang.size() > 1)
            {
                // serviceInfoDao.updateAudioInfo(serviceId, aPid, aPtype, audioLang);
                DVBPlayer_SWITCHAUDIO_t switchaudio_t = player.new DVBPlayer_SWITCHAUDIO_t();
                switchaudio_t.setU16AudioPid(aPid);
                switchaudio_t.setU8AudioFmt(aPtype);
                player.DVBPlayerSwitchAudio(switchaudio_t);
            }
        }
        
        return audioLang;
    }
    
    public String getPreviousLang()
    {
        MessagePlayer.oneCHaudio chAudio;
        int aPid = 0;
        int aPtype = 0;
        String audioLang = "DEF";
        if (null != listLang)
        {
            LogUtils.printLog(1, 2, TAG, "--->>> language sorts: " + listLang.size());
            
            if (langPos > 0 && langPos < listLang.size())
            {
                chAudio = listLang.get(langPos);
                aPid = chAudio.Getaudiopid();
                aPtype = chAudio.Getaudiotype();
                audioLang = chAudio.Getlanguagecode();
                
                langPos--;
            }
            else
            {
                chAudio = listLang.get(0);
                aPid = chAudio.Getaudiopid();
                aPtype = chAudio.Getaudiotype();
                audioLang = chAudio.Getlanguagecode();
                langPos = listLang.size() - 1;
            }
            if (listLang.size() > 1)
            {
                // serviceInfoDao.updateAudioInfo(serviceId, aPid, aPtype, audioLang);
                DVBPlayer_SWITCHAUDIO_t switchaudio_t = player.new DVBPlayer_SWITCHAUDIO_t();
                switchaudio_t.setU16AudioPid(aPid);
                switchaudio_t.setU8AudioFmt(aPtype);
                player.DVBPlayerSwitchAudio(switchaudio_t);
            }
        }
        return audioLang;
    }
    
    public String getCurrentSoundtrack(int serviceId, int logicalNumber)
    {
        ServiceInfoDao dao = new ServiceInfoDaoImpl(this.context);
        ServiceInfoBean service = dao.getChannelInfoByServiceId(serviceId, logicalNumber);
        String sournd = null;
        if (null == service)
        {
            LogUtils.printLog(1, 3, TAG, "--->>> not find the service,return the default soundtrack(stereo).");
            // if not find current service, return default sound track.
            return context.getString(R.string.dvb_st_soundtrack_stereo);
        }
        // 0 立体声 1 左声道 2 右声道 3 混合
        LogUtils.printLog(1, 3, TAG, "--->>> the current soundtrack is: " +service.getAudioMode());
        switch (service.getAudioMode())
        {
            case 1:
                sournd = context.getString(R.string.dvb_st_soundtrack_left);
                break;
            case 2:
                sournd = context.getString(R.string.dvb_st_soundtrack_right);
                break;
            case 3:
                sournd = context.getString(R.string.dvb_st_soundtrack_mix);
                break;
            default :
                sournd = context.getString(R.string.dvb_st_soundtrack_stereo);
                break;
        }
        return sournd;
    }
    
    public String getNextSoundtrack(String currentSoundtrack)
    {
        if (null != currentSoundtrack)
        {
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_stereo)))
            {
                player.DVBPlayerAVCtrlAudMode(1);
                return context.getString(R.string.dvb_st_soundtrack_left);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_left)))
            {
                player.DVBPlayerAVCtrlAudMode(2);
                return context.getString(R.string.dvb_st_soundtrack_right);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_right)))
            {
                player.DVBPlayerAVCtrlAudMode(3);
                return context.getString(R.string.dvb_st_soundtrack_mix);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_mix)))
            {
                player.DVBPlayerAVCtrlAudMode(0);
                return context.getString(R.string.dvb_st_soundtrack_stereo);
            }
        }
        return currentSoundtrack;
    }
    
    public String getPreviousSoundtrack(String currentSoundtrack)
    {
        if (null != currentSoundtrack)
        {
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_stereo)))
            {
                player.DVBPlayerAVCtrlAudMode(3);
                return context.getString(R.string.dvb_st_soundtrack_mix);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_left)))
            {
                player.DVBPlayerAVCtrlAudMode(0);
                return context.getString(R.string.dvb_st_soundtrack_stereo);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_right)))
            {
                player.DVBPlayerAVCtrlAudMode(1);
                return context.getString(R.string.dvb_st_soundtrack_left);
            }
            if (currentSoundtrack.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_mix)))
            {
                player.DVBPlayerAVCtrlAudMode(2);
                return context.getString(R.string.dvb_st_soundtrack_right);
            }
        }
        return currentSoundtrack;
    }
    
    public int parserSoundtrack(String text)
    {
        if (text.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_mix)))
        {
            return 3;
        }
        else if (text.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_right)))
        {
            return 2;
        }
        else if (text.equalsIgnoreCase(context.getString(R.string.dvb_st_soundtrack_left)))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    public int parserLang(String text)
    {
       int audioPid = 0;
       if(null == listLang)
       {
           LogUtils.printLog(1, 3, TAG, "--->>>list lang is null,return audio pid 0.");
           return 0;
       }
       for(int i=0;i <listLang.size();i++)
       {
           if(listLang.get(i).Getlanguagecode().equalsIgnoreCase(text))
           {
               audioPid = listLang.get(i).Getaudiopid();
           }
       }
       return audioPid;
    }
    
}
