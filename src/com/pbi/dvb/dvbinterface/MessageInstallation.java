package com.pbi.dvb.dvbinterface;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.pbi.dvb.global.Config;

public class MessageInstallation
{
    private static final String TAG = "MessageInstallaion";
    
    private Handler handler;
    
    public MessageInstallation()
    {
        super();
    }
    
    public MessageInstallation(Handler handler)
    {
        super();
        this.handler = handler;
    }
    
    /**
     * <��ʾ�ź��ж�>
     * 
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installInterrupt()
    {
        handler.sendEmptyMessage(Config.SIGNAL_INTERRUPT);
    }
    
    /**
     * <�������ֵ>
     * 
     * @param Progress
     * @param Freq
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installProgress(int Progress, int Freq)
    {
        Log.i(TAG, "---------------------Progress---->>>>" + Progress);
        Message message = new Message();
        message.what = Config.UPDATE_SEARCH_PROGRESS;
        Bundle bundle = new Bundle();
        bundle.putInt("progress", Progress);
        bundle.putInt("frequency", Freq);
        message.obj = bundle;
        handler.sendMessage(message);
    }
    
    /**
     * <��Ƶ���������>
     * 
     * @param Freq
     * @param Name
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installTpCompleted(int freq, int SymbRate, int Mod, String name, char serviceType)
    {
        Log.i(TAG, "------------------>>>Program Name--------------->>>" + freq);
        
        Bundle bundle = new Bundle();
        bundle.putInt("frequency", freq);
        bundle.putInt("symbRate", SymbRate);
        bundle.putInt("mod", Mod);
        bundle.putString("name", name);
        bundle.putChar("serviceType", serviceType);
        
        Message message = new Message();
        message.obj = bundle;
        message.what = Config.SIGLE_TP_SEARCH_COMPLETE;
        handler.sendMessage(message);
    }
    
    /**
     * <��Ƶ������ʧ��>
     * 
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installTpFailed()
    {
        Log.i(TAG, "-----------Tp Search Failed!!-------------");
        handler.sendEmptyMessage(Config.SIGLE_TP_SEARCH_FAILED);
    }
    
    /**
     * <������ɻ�ȡ�������>
     * 
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installCompleted()
    {
        Log.i(TAG, "------------------>>>DVB Message Search Finished!!!--------------->>>");
        Message message = new Message();
        message.what = Config.AUTO_SEARCH_COMPLETE;
        handler.removeMessages(Config.AUTO_SEARCH_COMPLETE);
        handler.sendMessage(message);
    }
    
    /**
     * <����ʧ��>
     * 
     * @see [�ࡢ��#��������#��Ա]
     */
    public void installFailed()
    {
        handler.sendEmptyMessage(Config.AUTO_SEARCH_FAILED);
    }
    
}