/*
 * File Name:  SleepBroadcastReceiver.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;

import com.pbi.dvb.service.SleepService;
import com.pbi.dvb.utils.LogUtils;

public class SleepBroadcastReceiver extends BroadcastReceiver
{
    
    private static final String TAG = "SleepBroadcastReceiver";
    
    private WakeLock wakeLock = null;
    
    public void onReceive(Context context, Intent intent)
    {
        LogUtils.printLog(1, 3, TAG, "--->>>sleep receiver receive the sleep broadcast.");
        exitSuspand(context);
        
        Intent service = new Intent(context, SleepService.class);
        service.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        context.startService(service);
        
        int totalWait = 0;
        while (true)
        {
            if (SleepService.isSuspendlock())
            {
                break;
            }
            
            totalWait += 100;
            if (totalWait >= 4000)
            {
                break;
            }
            
            try
            {
                Thread.sleep(100);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }
            
            LogUtils.i(TAG, "totalWait = " + totalWait);
        }
        
        resumeSuspand();
    }
    
    private void resumeSuspand()
    {
        LogUtils.e(TAG, ">>> resume_suspend begin <<<");
        if (null != wakeLock && wakeLock.isHeld())
        {
            wakeLock.release();
            wakeLock = null;
        }
        LogUtils.e(TAG, ">>> resume_suspend end <<<");
    }
    
    private void exitSuspand(Context context)
    {
        LogUtils.e(TAG, ">>> exit suspend begin <<<");
        PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);
        wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, this.getClass().getCanonicalName());
        wakeLock.acquire();
        LogUtils.e(TAG, ">>> exit suspend end <<<");
        
    }
    
}
