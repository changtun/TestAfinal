package com.pbi.dvb.service;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.observer.EITGradeContentObserver;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.receiver.SleepBroadcastReceiver;
import com.pbi.dvb.utils.AlarmTaskUtil;

public class PvrGlobalService extends Service
{
    private static final String TAG = "PvrGlobalService";
    
    private PvrDatabaseObserver pvrObserver;
    
    private EITGradeContentObserver dvbContentObserver;
    
    private SleepBroadcastReceiver sleepBrordcast;
    
    @Override
    public IBinder onBind(Intent intent)
    {
        
        return null;
        
    }
    
    @Override
    public void onCreate()
    {
        
        super.onCreate();
        
        Log.e(TAG, "PvrGlobalService onCreate");
    }
    
    @Override
    public void onStart(Intent intent, int startId)
    {
        
        Log.e(TAG, "PvrGlobalService onStart");
        
        // register the sleep broadcast receiver
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        sleepBrordcast = new SleepBroadcastReceiver();
        registerReceiver(sleepBrordcast, filter);
        
        // register the observer to the system for monitor the database's change
        pvrObserver = new PvrDatabaseObserver(this, null);
        
        Uri uri = Uri.parse("content://" + DBPvr.PVR_AUTHORITY + "/" + DBPvr.TABLE_PVR_NAME);
        
        getContentResolver().registerContentObserver(uri, false, pvrObserver);
        
        // register the observer to settings.db
        dvbContentObserver = new EITGradeContentObserver(this, null);
        Uri dvbUri = Uri.parse("content://" + "settings" + "/system/dvb_grade");
        getContentResolver().registerContentObserver(dvbUri, true, dvbContentObserver);
        
    }
    
    @Override
    public void onDestroy()
    {
        
        super.onDestroy();
        
        Log.e(TAG, "PvrGlobalService onDestroy");
        
        if (null != dvbContentObserver)
        {
            getContentResolver().unregisterContentObserver(dvbContentObserver);
        }
        if (pvrObserver != null)
        {
            Log.e(TAG, "unregister the pvrObserver from the system");
            getContentResolver().unregisterContentObserver(pvrObserver);
        }
        else
        {
            
            Log.e(TAG, "The pvrObserver is not expected to be null here... But it was...");
            Log.e(TAG, "This may course the Observer not to be unregister from the system");
            
        }
        
        if (sleepBrordcast != null)
        {
            
            unregisterReceiver(sleepBrordcast);
            
            Log.e(TAG, "unregister the sleepBrordcast from the system");
            
        }
        else
        {
            
            Log.e(TAG, "The sleepBroadcast is not expected to be null here. But it was");
            
            Log.e(TAG, "This may course the Broadcast not to be unregister from the system");
            
        }
    }
    
    private class PvrDatabaseObserver extends ContentObserver
    {
        
        private static final String TAG = "PvrDatabaseObserver";
        
        private AlarmTaskUtil alarmUtil;
        
        private Context context = null;
        
        public PvrDatabaseObserver(Context context, Handler handler)
        {
            super(handler);
            
            Log.e(TAG, "Observer created");
            
            alarmUtil = new AlarmTaskUtil();
            
            this.context = context;
        }
        
        @Override
        public void onChange(boolean selfChange)
        {
            
            super.onChange(selfChange);
            
            Log.e(TAG, "Observer onChange()");
            
            alarmUtil.setNextTask(context, new TaskCompleteCallBack(context));
        }
        
    }
    
}
