package com.pbi.dvb.domain;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.net.Uri;
import android.os.RemoteException;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.os.ServiceManager;
import android.view.IWindowManager;

import com.pbi.dvb.SplashActivity;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Rec_Status_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.service.PvrRecordService;
import com.pbi.dvb.utils.LogUtils;

public abstract class PlayPvrBean extends PvrRunningTask
{
    
    protected static final String TAG = "PlayPvrBean";
    
    protected PlayPvrBean()
    {
    }
    
    public void startPlay(final Context context)
    {
        
        NativePlayer nPlayer = NativePlayer.getInstance();
        
        LogUtils.e(TAG, "In PlayPvrBean::startPlay");
        
        Log.e(TAG, "zxguan========Pvr========DVBPlayerInit====================\n");
        
        nPlayer.DVBPlayerInit(null);
        
        // TODO:
        PVR_Rec_Status_t recFile = nPlayer.new PVR_Rec_Status_t();
        nPlayer.DVBPlayerPvrRecGetStatus(recFile);
        int rec_status = recFile.getEnState();
        
        if (rec_status == 2 || rec_status == 3)
        {
            
            nPlayer.DVBPlayerPvrRecStop();
            
        }
        
        LogUtils.e(TAG, "---->>>> PlayPvrBean::startPlay <<<<----");
        
        final int keyCode = KeyEvent.KEYCODE_HOME;
        
        SharedPreferences sp = null;
        
        Editor editor = null;
        
        final ServiceInfoBean serviceBean = getServiceBean();
        
        if (serviceBean == null)
        {
            
            return;
            
        }
        
        synchronized (SplashActivity.class)
        {
            
            switch (serviceBean.getServiceType())
            {
            
                case Config.SERVICE_TYPE_RADIO:
                    
                    sp = context.getSharedPreferences(Config.DVB_LASTPLAY_SERVICE, Context.MODE_PRIVATE);
                    
                    editor = sp.edit();
                    
                    editor.putInt("radio_serviceId", getServiceId());
                    editor.putInt("radio_logicalNumber", getLogicalNumber());
                    editor.putString("radio_serviceName", serviceBean.getChannelName());
                    editor.putInt("radio_servicePosition", serviceBean.getChannelPosition());
                    
                    break;
                
                case Config.SERVICE_TYPE_TV:
                    
                    sp = context.getSharedPreferences(Config.DVB_LASTPLAY_SERVICE, Context.MODE_PRIVATE);
                    
                    editor = sp.edit();
                    
                    editor.putInt("tv_serviceId", getServiceId());
                    editor.putInt("tv_logicalNumber", getLogicalNumber());
                    editor.putString("tv_serviceName", serviceBean.getChannelName());
                    editor.putInt("tv_servicePosition", serviceBean.getChannelPosition());
                    
                    editor.apply();
                    editor.commit();
                    
                    break;
            }
        }
        
        Thread startDelay = new Thread()
        {
            public void run()
            {
                try
                {
                    Thread.sleep(100);
                    
                    long now = SystemClock.uptimeMillis();
                    
                    KeyEvent keyDown = new KeyEvent(now, now, KeyEvent.ACTION_DOWN, keyCode, 0);
                    
                    IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager.getService("window"));
                    wm.injectKeyEvent(keyDown, false);
                    
                    KeyEvent keyUp = new KeyEvent(now, now, KeyEvent.ACTION_UP, keyCode, 0);
                    wm.injectKeyEvent(keyUp, false);
                    
                    Thread.sleep(1000);
                    
                    LogUtils.e(TAG, "start_play_channel id = " + getServiceId() + " number = " + getLogicalNumber());
                    
                    Intent intent = new Intent(Intent.ACTION_MAIN);
                    
                    intent.addCategory(Intent.CATEGORY_LAUNCHER);
                    
                    ComponentName cName = new ComponentName("com.pbi.dvb", "com.pbi.dvb.SplashActivity");
                    
                    intent.setComponent(cName);
                    
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    
                    switch (serviceBean.getServiceType())
                    {
                    
                        case Config.SERVICE_TYPE_RADIO:
                            
                            intent.setData(Uri.parse("dvb:" + Config.DVB_RADIO_PROGRAMME));
                            
                            break;
                        
                        case Config.SERVICE_TYPE_TV:
                            
                            intent.setData(Uri.parse("dvb:" + Config.DVB_TV_PROGRAMME));
                            
                            break;
                    
                    }
                    
                    LogUtils.e(TAG, "Before startActivity");
                    
                    context.startActivity(intent);
                    
                    pvr_Task_State = DBPvr.PVR_TASK_EXECUTING;
                    
                    stopPlay(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
                    
                    LogUtils.e(TAG, "After startActivity");
                    
                    // if (getPvrMode() == DBPvr.DO_ONCE_MODE)
                    // {
                    //
                    // setPvrMode(DBPvr.CLOSE_MODE);
                    //
                    // update(context);
                    //
                    // stopTask();
                    //
                    // }
                    //
                    // gotoNextTask(context, getPvrId());
                    
                }
                catch (InterruptedException e)
                {
                    
                    e.printStackTrace();
                    
                }
                catch (RemoteException e)
                {
                    
                    e.printStackTrace();
                    
                }
            }
        };
        
        startDelay.start();
        
    }
    
    public void stopPlay(Integer stopType)
    {
        
        LogUtils.e(TAG, "---->>>> PlayPvrBean::stopPlay stopType = " + stopType + "<<<<----");
        
        swapTaskState(stopType);
        
    }

    @Override
    public String toString()
    {
        return "PlayPvrBean [checkTaskExpired()=" + checkTaskExpired() + ", toString()=" + super.toString()
            + ", getPvrId()=" + getPvrId() + ", getPvrMode()=" + getPvrMode() + ", getPvrWakeMode()="
            + getPvrWakeMode() + ", getServiceId()=" + getServiceId() + ", getLogicalNumber()=" + getLogicalNumber()
            + ", getPvrSetDate()=" + getPvrSetDate() + ", getPvrStartTime()=" + getPvrStartTime()
            + ", getPvrRecordLength()=" + getPvrRecordLength() + ", getPvrWeekDate()=" + getPvrWeekDate()
            + ", getPvr_pmt_pid()=" + getPvr_pmt_pid() + ", getPvr_is_sleep()=" + getPvr_is_sleep()
            + ", getPvr_type_tag()=" + getPvr_type_tag() + ", get_Adjust_StartTime()=" + get_Adjust_StartTime()
            + ", getServiceBean()=" + getServiceBean() + ", checkTimeExpired()=" + checkTimeExpired() + ", getClass()="
            + getClass() + ", hashCode()=" + hashCode() + "]";
    }
}