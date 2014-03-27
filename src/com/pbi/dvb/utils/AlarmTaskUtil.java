package com.pbi.dvb.utils;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.pvrinterface.PvrSetTaskCallback;
import com.pbi.dvb.pvrinterface.impl.SetNextTaskCallBack;

public class AlarmTaskUtil
{
    
    private static final String TAG = "com.pbi.dvb.utils.AlarmTaskUtil";
    
    private AlarmManager am = null;
    
    public void setNextTask(Context context, PvrSetTaskCallback callback)
    {
        
        if (am == null)
        {
            
            am = (AlarmManager)context.getSystemService(Context.ALARM_SERVICE);
            
        }
        
        clearTask(context);
        
        setTask(context, callback);
        
    }
    
    private PvrBean getFirstAlarmTask(Context context, PvrSetTaskCallback callback)
    {
        
        LogUtils.e(TAG, "-------->>>> getFirstAlarmTask() begin... <<<<--------", true);
        
        boolean adjust_Trigger_Time = false;
        
        PvrBean pvrBean = callback.get_First_Record_Task();
        
        // 如果这里为空, 说明数据库中只有一条记录, 并且该条记录由于用户手动终止而被跳过了.
        // 或者是因为数据库里没有任何记录...
        // 所以说, 如果adjust_Trigger_Time为真的话, 说明数据库里最多只有一条记录...如果不是的话,
        // 说明是一个程序的BUG...
        
        // 以上注释目前也许已经不正确...因为目前代码存在跳过多条记录的可能.
        if (pvrBean == null)
        {
            
            // If pvrBean is null...Maybe the database has not any tasks..
            // Or there are several tasks, but they all be skipped...
            // Maybe the database has just one task...
            // Maybe the database has just two tasks...
            // Maybe the database has more than two tasks..
            // But they all be skipped...
            adjust_Trigger_Time = true;
            
        }
        
        if (adjust_Trigger_Time)
        {
            
            LogUtils.e(TAG, "getFirstAlarmTask()::pvrBean == null");
            
            PvrDao pvrDao = new PvrDaoImpl(context);
            
            int unclosed_Task_Num = pvrDao.get_Count_Of_Unclose_Task();
            
            LogUtils.e(TAG, "-------->>>> unclosed_Task_Num = " + unclosed_Task_Num + " <<<<--------");
            
            if (unclosed_Task_Num > 0)
            {
                
                pvrBean = pvrDao.get_First_Record_Task((Integer[])null);
                
                if (pvrBean == null)
                {
                    
                    LogUtils.e(TAG,
                        "There is one unclosed task , but when try to get it via pvrDao.get_First_Record_Task(null), the result is null...Maybe the task is an expired DO_ONCE_TASK");
                    
                    return null;
                    
                }
                else
                {
                    
                    LogUtils.e(TAG, "-------->>>> The pvrBean we get is :: " + pvrBean.toString());
                    
                }
                
            }
            else
            {
                
                LogUtils.e(TAG, "There is no task now!!! unclosed_Task_Num = " + unclosed_Task_Num);
                
                return null;
                
            }
            
        }
        else
        {
            
            LogUtils.e(TAG, "pvrBean != null");
            
            LogUtils.e(TAG, "pvrBean = " + pvrBean.toString());
            
        }
        
        // 调整颤抖吧, 愚蠢的人类!!!
        
        LogUtils.i(TAG, "-------->>>>getFirstAlarmTask() end...Now is " + new Date().toString() + " <<<<--------");
        
        return pvrBean;
        
    }
    
    private void setTask(Context context, PvrSetTaskCallback callBack)
    {
        
        synchronized (AlarmTaskUtil.class)
        {
            
            LogUtils.e(TAG, "---->>>> AlarmTaskUtil::setTask begin <<<<----", true);
            
            if (am == null)
            {
                
                LogUtils.e(TAG,
                    "-------->>>> In AlarmTaskUtil:setTask The AlarmManager is null...This will cause the task not set into the system... <<<<--------");
                
                return;
                
            }
            
            Map<Integer, Integer> skipRecord = new HashMap<Integer, Integer>();
            
            List<Integer> skipList = new ArrayList<Integer>();
            
            Integer[] skipIDs = null;
            
            if (callBack != null && callBack.getSkipID() != null)
            {
                
                Integer[] callBackSkip = callBack.getSkipID();
                
                skipIDs = callBackSkip;
                
                for (int i = 0; i < callBackSkip.length; i++)
                {
                    
                    if (callBackSkip[i] != null && callBackSkip[i] != -1)
                    {
                        
                        if (!skipList.contains(callBackSkip[i]))
                        {
                            
                            skipList.add(callBackSkip[i]);
                            
                        }
                        
                        if (skipRecord.containsKey(callBackSkip[i]))
                        {
                            
                            int skipCount = skipRecord.get(callBackSkip[i]);
                            
                            skipCount++;
                            
                            skipRecord.remove(callBackSkip[i]);
                            
                            skipRecord.put(callBackSkip[i], skipCount);
                            
                        }
                        else
                        {
                            
                            skipRecord.put(callBackSkip[i], 1);
                            
                        }
                        
                    }
                    else
                    {
                        
                        LogUtils.e(TAG, "-------->>>> callBackSkip[" + i + "] == null ...<<<<--------");
                        
                    }
                    
                }
                
            }
            
            PvrBean pvrBean = null;
            
            Calendar midnightTime = Calendar.getInstance();
            
            long current_Time = midnightTime.getTimeInMillis();
            int week_Of_Today = midnightTime.get(Calendar.DAY_OF_WEEK);
            long current_Date = 0;
            
            midnightTime.set(Calendar.HOUR_OF_DAY, 0);
            midnightTime.set(Calendar.MINUTE, 0);
            midnightTime.set(Calendar.SECOND, 0);
            midnightTime.set(Calendar.MILLISECOND, 0);
            
            current_Time = current_Time - midnightTime.getTimeInMillis();
            current_Date = midnightTime.getTimeInMillis();
            
            long triggerAtMillis = 0;
            
            PvrDao pvrDao = new PvrDaoImpl(context);
            
            // Here may occur some problems...
            // For database maybe contains some expired task...But when you call
            // this funciton ...
            // These tasks will be recarded as normal task...Not be skipped..
            // Then when checking if need to reset the skipList, I will skip all
            // tasks more times than expected...
            // That will cause one earliest task be set many times at almost the
            // same time...
            // So you need to filter all tasks in the databases to change the
            // expired task into close mode...
            
            pvrDao.filter_All_Task();
            
            int max_Record_Items = pvrDao.get_Count_Of_Unclose_Task();
            
            for (int i = 0; i < DBPvr.MAX_TASK_ITEMS; i++)
            {
                
                pvrBean = getFirstAlarmTask(context, new SetNextTaskCallBack(context, skipIDs));
                
                LogUtils.e(TAG, "pvrBean.getPvrID() = " + (pvrBean == null ? null : pvrBean.getPvrId()));
                
                for (int j = 0; skipIDs != null && j < skipIDs.length; j++)
                {
                    
                    LogUtils.e(TAG, "skipIDs[" + j + "] = " + skipIDs[j]);
                    
                }
                
                if (pvrBean == null)
                {
                    
                    LogUtils.e(TAG, "-------->>>>There is not any pvr(reversation) task....<<<<--------");
                    
                    return;
                    
                }
                
                if (skipRecord.containsKey(pvrBean.getPvrId() == null ? -1 : pvrBean.getPvrId()))
                {
                    
                    int skipCount = skipRecord.get(pvrBean.getPvrId());
                    
                    LogUtils.e(TAG, "PvrID = " + pvrBean.getPvrId() + "   skipCount = " + skipCount);
                    
                    triggerAtMillis =
                        pvrBean.get_Adjust_Next_StartTime(current_Date, current_Time, week_Of_Today, skipCount);
                    
                }
                else
                {
                    
                    LogUtils.e(TAG, "---->>>> Nothing has been skipped <<<< ----");
                    
                    triggerAtMillis = pvrBean.get_Adjust_StartTime(current_Date, current_Time, week_Of_Today);
                    
                }
                
                triggerAtMillis = current_Date + current_Time + triggerAtMillis - DBPvr.TASK_TRIGGER_DELAY;
                
                Intent task_Intent = null;
                
                task_Intent = new Intent("com.pbi.dvb.service.pvrRecordDialog");
                
                task_Intent.putExtra(DBPvr.PVR_ID, pvrBean.getPvrId() == null ? -1 : pvrBean.getPvrId().intValue());
                
                task_Intent.putExtra(DBPvr.PVR_SERVICE_ID, pvrBean.getServiceId() == null ? -1 : pvrBean.getServiceId()
                    .intValue());
                
                task_Intent.putExtra(DBPvr.PVR_LOGICAL_NUMBER, pvrBean.getLogicalNumber() == null ? -1
                    : pvrBean.getLogicalNumber().intValue());
                
                task_Intent.putExtra(DBPvr.PVR_TRIGGER_TIME, triggerAtMillis);
                
                LogUtils.e(TAG,
                    "-------->>>> The pvrBean[" + i + "]'s triggerAtMillis = " + new Date(triggerAtMillis).toString()
                        + " skipCount = " + skipRecord.get(pvrBean.getPvrId()),
                    true);
                
                PendingIntent first_Task =
                    PendingIntent.getService(context, i, task_Intent, PendingIntent.FLAG_UPDATE_CURRENT);
                
                am.set(AlarmManager.RTC_WAKEUP, triggerAtMillis, first_Task);
                
                if (skipList != null && !skipList.contains(pvrBean.getPvrId()))
                {
                    
                    skipList.add(pvrBean.getPvrId());
                    
                }
                
                if (skipRecord.containsKey(pvrBean.getPvrId()))
                {
                    
                    LogUtils.e(TAG, "---->>>> The skipList is ever being clear... <<<<----");
                    
                    int skipCount = skipRecord.get(pvrBean.getPvrId());
                    
                    skipCount++;
                    
                    skipRecord.remove(pvrBean.getPvrId());
                    
                    skipRecord.put(pvrBean.getPvrId(), skipCount);
                    
                }
                else
                {
                    
                    skipRecord.put(pvrBean.getPvrId(), 1);
                    
                }
                
                if (skipList != null)
                {
                    
                    if (skipIDs != null)
                    {
                        
                        skipIDs = skipList.toArray(skipIDs);
                        
                    }
                    else
                    {
                        
                        skipIDs = (Integer[])skipList.toArray(new Integer[skipList.size()]);
                        
                    }
                    
                    if (skipList.size() == max_Record_Items)
                    {
                        
                        skipList.clear();
                        
                    }
                }
                
            }
            
            LogUtils.e(TAG, "---->>>> AlarmTaskUtil::setTask end <<<<----", true);
            
        }
        
    }
    
    private void clearTask(Context context)
    {
        
        LogUtils.e(TAG, "---->>>> AlarmTaskU::clearTask begin <<<<----", true);
        
        // Intent first_Task_Intent = null;
        //
        // first_Task_Intent = new
        // Intent("com.pbi.dvb.service.pvrRecordDialog");
        //
        // PendingIntent first_Task = PendingIntent.getService(context, 0,
        // first_Task_Intent, PendingIntent.FLAG_UPDATE_CURRENT);
        //
        // am.cancel(first_Task);
        //
        // Intent second_Task_Intent = null;
        //
        // second_Task_Intent = new
        // Intent("com.pbi.dvb.service.pvrRecordDialog");
        //
        // PendingIntent second_Task = PendingIntent.getService(context, 1,
        // second_Task_Intent, PendingIntent.FLAG_UPDATE_CURRENT);
        //
        // am.cancel(second_Task);
        
        Intent task_Intent = null;
        
        task_Intent = new Intent("com.pbi.dvb.service.pvrRecordDialog");
        
        for (int i = 0; i < DBPvr.MAX_RECORD_ITEMS; i++)
        {
            
            PendingIntent task_PI =
                PendingIntent.getService(context, i, task_Intent, PendingIntent.FLAG_UPDATE_CURRENT);
            
            am.cancel(task_PI);
            
        }
        
        LogUtils.e(TAG, "---->>>> AlarmTaskU::clearTask end <<<<----", true);
        
    }
}
