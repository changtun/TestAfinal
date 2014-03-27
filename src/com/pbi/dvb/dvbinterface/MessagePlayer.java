package com.pbi.dvb.dvbinterface;

import java.util.ArrayList;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.pbi.dvb.R;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class MessagePlayer
{
    private Handler handler;
    private Context context;
    public MessagePlayer()
    {
        super();
    }
    
    public MessagePlayer(Handler handler,Context context)
    {
        super();
        this.handler = handler;
        this.context = context;
    }
    
    public class oneCHaudio
    {
        public int audiotype;
        public int audiopid;
        
        public String languagecode;
        
        public int Getaudiotype()
        {
            return audiotype;
        }
        
        public void Setaudiotype(int values)
        {
            audiotype = values;
        }
        
        public int Getaudiopid()
        {
            return audiopid;
        }
        
        public void Setaudiopid(int values)
        {
            audiopid = values;
        }
        
        public String Getlanguagecode()
        {
            return languagecode;
        }
        
        public void Setlanguagecode(String values)
        {
            languagecode = values;
        }
    }
    
    public class AudioInfos
    {
        public int totals;
        
        public oneCHaudio[] audiolist;
        
        public int Gettotals()
        {
            return totals;
        }
        
        public void Settotals(int value)
        {
            totals = value;
        }
        
        public oneCHaudio[] Getaudiolist()
        {
            return audiolist;
        }
        
        public void Setaudiolist(oneCHaudio[] value)
        {
            audiolist = value;
        }
        
        public void Bulidaudiolist(int number, AudioInfos list)
        {
            int ii = 0;
            System.out.println("Bulidaudiolist===========" + number + "====");
            oneCHaudio[] Infos = new oneCHaudio[number];
            list.Setaudiolist(Infos);
            for (ii = 0; ii < number; ii++)
            {
                list.audiolist[ii] = new oneCHaudio();
            }
        }
    }
    
    public void playerMsgInterrupt()
    {
        handler.sendEmptyMessage(Config.PLAYER_PROGRAM_INTERRUPT);
    }
    
    public void playerMsgTunerLock()
    {
        handler.sendEmptyMessage(Config.PLAYER_LOCK_TUNER_SUCCESS);
    }
    
    public void playerMsgPmtTimeOut()
    {
        handler.sendEmptyMessage(Config.PLAYER_PMT_TIME_OUT);
    }
    
    /* 1.av中断是指音视频祯数检测不正常发出的消息 */
    /* 2.上层处理的时候应该判断当前是否是清流或者是ca不能解扰得状态 */
    /* 如果是清流，或者当前节目是加扰的但是解扰状态，那么节目中断的提示需要正常显示 */
    /* 如果当前是加扰节目，但不是解扰状态，就不需要提示节目中断，而应该是显示ca的错误消息 */
    /* 比如说当前ca的状态是E23，正常观看的状态，这个时候就需要提示节目中断 */
    /* 比如当前的ca状态是E16或E14等，就不要再处理这个消息了，按照之前的ca的错误类型显示就可以了 */
    public void playerMsgAVinterrupt()
    {
        NativePlayer nativePlayer = NativePlayer.getInstance();
        int caMode = nativePlayer.DVBPlayerGetCaMode();
        
        // clear stream
        if (caMode == 0)
        {
            handler.sendEmptyMessage(Config.PLAYER_PMT_TIME_OUT);
        }
        
        // scrambling stream
        if (caMode == 1)
        {
            int errcode = new MessageCa().GetLastErrcode();
            if (errcode == 23)
            {
                handler.sendEmptyMessage(Config.AV_FRAME_UNNORMAL);
            }
        }
    }
    
    public void playerMsgStart()
    {
        handler.sendEmptyMessage(Config.PLAYER_START);
    }
    
    public void playerMsgAudioInfo(AudioInfos info)
    {
        Message langMsg = new Message();
        ArrayList<MessagePlayer.oneCHaudio> listLang = new ArrayList<MessagePlayer.oneCHaudio>();
        if (null != info)
        {
            int i=1;
            for (oneCHaudio oneCHaudio : info.Getaudiolist())
            {
                oneCHaudio.Setlanguagecode(context.getString(R.string.dvb_soundtrack_lang)+i);
                listLang.add(oneCHaudio);
                i++;
            }
        }
        langMsg.obj = listLang;
        langMsg.what = Config.PLAYER_LANGUAGE;
        handler.sendMessage(langMsg);
    }
    
    public void playerMsgCaMode(int mode)
    {
        Message msg = new Message();
        msg.what = Config.UPDATE_CA_MODE;
        msg.obj = mode;
        handler.sendMessage(msg);
    }

	public void playerMsgUpdatePmtInfo(int Tsid, int ServiceId, int PmtPid)
	{
	    LogUtils.printLog(1, 3, "MessagePlayer", "--->>>Update pmt pid message has comming!!!");
	    Message msg = new Message();
	    msg.what = Config.UPDATE_PMT_PID;
	    Bundle bundle =new Bundle();
	    bundle.putInt("tsId", Tsid);
	    bundle.putInt("serviceId", ServiceId);
	    bundle.putInt("pmdPid", PmtPid);
	    
	    msg.obj = bundle;
	    handler.sendMessage(msg);
	}
    
}
