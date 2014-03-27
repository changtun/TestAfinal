/*
 * File Name:  SleepService.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.service;

import java.util.Calendar;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;

import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativeDrive.PmocWakeUp;
import com.pbi.dvb.utils.LogUtils;

public class SleepService extends Service
{
    
    private static final String TAG = "SleepService";
    
    private WakeLock wakeLock;
    
    private static boolean suspendLock = false;
    
    public IBinder onBind(Intent intent)
    {
        return null;
    }
    
    public void onCreate()
    {
        super.onCreate();
        LogUtils.printLog(1, 3, TAG, "--->>>sleep service create.");
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtils.printLog(1, 3, TAG, "--->>>sleep service start.");
        
        // get the wakelock, keep the screen on.
        if (null == wakeLock)
        {
            LogUtils.printLog(1, 3, TAG, "--->>>acquire the wakeLock.");
            PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
            wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, this.getClass().getCanonicalName());
            wakeLock.acquire();
            
            suspendLock = true;
        }
        
        // Used for control the luncher page's advertising video's play.
        // If file exists, the advertising video will not play..
        // File localVideoControlFile = new File("/mnt/expand/RunDvbPvr");
        //
        // if (localVideoControlFile.exists() && localVideoControlFile.isFile())
        // {
        // localVideoControlFile.delete();
        // }
        
        NativeDrive nativeDrive = new NativeDrive(this);
        
        // get the pvr background task.
        PvrDaoImpl pvrDao = new PvrDaoImpl(this);
        PvrBean task = pvrDao.get_First_Record_Task((Integer[])null);
        
        // set the devices wake up time.
        int startTime = 0;
        
        if (null != task)
        {
            LogUtils.printLog(1, 3, TAG, "----First Record Task-Existed!!!-----");
            
            Calendar calendar = Calendar.getInstance();
            long current_Time = calendar.getTimeInMillis();
            
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);
            
            current_Time = current_Time - calendar.getTimeInMillis();
            
            long currentDate = calendar.getTimeInMillis();
            int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
            
            Long pvrStartTime = task.get_Adjust_StartTime(currentDate, current_Time, week_Of_Today);
            LogUtils.printLog(1, 3, TAG, "----PVR Start Time is------>>>>" + pvrStartTime);
            
            // current programe is recording.
            if (pvrStartTime < 0)
            {
                
                LogUtils.printLog(1, 3, TAG, "----Current Record Task-is Recording!!!-----");
                
                // get next invalid task;
                PvrBean nextTask = pvrDao.get_First_Record_Task(task.getPvrId());
                
                // current task is the only one, pvrStartTime is the max value.
                if (null == nextTask)
                {
                    pvrStartTime = task.get_Adjust_Next_StartTime(currentDate, current_Time, week_Of_Today);
                }
                // get the nextTask's start time.
                else
                {
                    pvrStartTime = nextTask.get_Adjust_StartTime(currentDate, current_Time, week_Of_Today);
                }
            }
            
            PmocWakeUp timeBean = nativeDrive.new PmocWakeUp();
            
            // adjust the time.
            startTime = (int)(pvrStartTime / 1000l);
            startTime = startTime - 120;
            LogUtils.printLog(1, 3, TAG, "----Task Start Time is------>>>>" + startTime);
            
            if (startTime < 0)
            {
                timeBean.setU32WakeUpTime(0);
            }
            else
            {
                timeBean.setU32WakeUpTime(startTime);
            }
            
            nativeDrive.DrvPmocSetWakeUp(timeBean);
            
        }
        
        // android.os.storage.IMountService mMountService = null;
        //
        // IBinder service = android.os.ServiceManager.getService("mount");
        //
        // if (service != null)
        // {
        //
        // mMountService = android.os.storage.IMountService.Stub.asInterface(service);
        //
        // try
        // {
        // File file = new File("/mnt/sda");
        //
        // File[] files = file.listFiles();
        //
        // for (File uFile : files)
        // {
        //
        // Log.e(TAG, "uFile's absoluteFilePath = " + uFile.getAbsolutePath());
        //
        // mMountService.unmountVolume(uFile.getAbsolutePath(), true, true);
        //
        // }
        //
        // String extStoragePath = Environment.getExternalStorageDirectory().toString();
        //
        // Log.e(TAG, "StorageDirectory = " + Environment.getExternalStorageDirectory().toString());
        //
        // mMountService.unmountVolume(extStoragePath, true, true);
        //
        // }
        // catch (Exception e)
        // {
        //
        // LogUtils.e(TAG, "Encounter some problems during running the mMountService.unmountVolume");
        //
        // }
        //
        // }
        // else
        // {
        //
        // LogUtils.printLog(1, 3, TAG, "Can't get mount service");
        //
        // }
        
        // release the wakelock.
        if (null != wakeLock && wakeLock.isHeld())
        {
            LogUtils.printLog(1, 3, TAG, "--->>>release the wakeLock.");
            suspendLock = false;
            wakeLock.release();
            wakeLock = null;
        }
        
        LogUtils.printLog(1, 3,TAG, "--->>>SleepService onStart finished.");
        return super.onStartCommand(intent, flags, startId);
    }
    
    @Override
    public void onDestroy()
    {
        
        super.onDestroy();
        LogUtils.printLog(1, 3,TAG, "--->>>SleepService onDestroy finished.");
    }
    
    public static boolean isSuspendlock()
    {
        return suspendLock;
    }
    
}
