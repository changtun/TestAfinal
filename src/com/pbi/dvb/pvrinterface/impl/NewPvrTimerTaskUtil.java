package com.pbi.dvb.pvrinterface.impl;

import java.util.Calendar;
import java.util.Date;

import android.util.Log;

import com.pbi.dvb.pvrinterface.PvrTimerTaskCallBack;
import com.pbi.dvb.utils.LogUtils;

public class NewPvrTimerTaskUtil extends Thread
{
    
    private static final String TAG = "PvrTimerTaskUtil";
    
    private long milli_Delay = 0;
    
    private boolean runFlag = true;
    
    private long startTime = 0;
    
    private PvrTimerTaskCallBack callBack = null;
    
    public NewPvrTimerTaskUtil(long milli_Delay, PvrTimerTaskCallBack callBack)
    {
        
        this.milli_Delay = milli_Delay;
        
        this.callBack = callBack;
        
    }
    
    @Override
    public void run()
    {
        
        startTime = Calendar.getInstance().getTimeInMillis();
        
        if (runFlag)
        {
            
            try
            {
                
                Log.e(TAG, "before running task, the delay Start at = " + new Date().toString());
                
                while (runFlag && milli_Delay > 0)
                {
                    
                    LogUtils.d(TAG, "NewPvrTimerTaskUtil run... Delay time left is " + milli_Delay);
                    
                    Thread.sleep(1000);
                    
                    milli_Delay -= 1000;
                    
                }
                
                Log.e(TAG, "after running task, the delay end at = " + new Date().toString());
                
            }
            catch (InterruptedException e)
            {
                
                e.printStackTrace();
                
            }
        }
        
        if (runFlag)
        {
            
            Log.e(TAG, "Total delay time = " + (Calendar.getInstance().getTimeInMillis() - startTime)
                + " milli_Delay = " + milli_Delay);
            
            // handler.sendEmptyMessage(PvrBackgroundService.RECORD_FINISHED);
            callBack.pvrTaskCallBack();
            
            runFlag = false;
            
        }
        else
        {
            
            LogUtils.e(TAG, "---->>>> runFlag = false <<<<----");
            
        }
    }
    
    public boolean isRunFlag()
    {
        return runFlag;
    }
    
    public void stopTask()
    {
        
        LogUtils.e(TAG, "NewPvrTimerTaskUtil::stopTask run...");
        
        runFlag = false;
    }
}