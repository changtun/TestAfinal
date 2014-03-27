package com.pbi.dvb.receiver;

import java.util.Calendar;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.pvrinterface.impl.SetNextTaskCallBack;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.utils.AlarmTaskUtil;
import com.pbi.dvb.utils.LogUtils;

public class OnPvrTaskCompleteReceiver extends BroadcastReceiver
{
    
    private static final String TAG = "com.pbi.dvb.receiver.OnPvrTaskCompleteReceiver";
    
    @Override
    public void onReceive(Context context, Intent intent)
    {
        
        Integer pvr_ID = intent.getIntExtra(DBPvr.PVR_ID, -1);
        
        boolean skip_Task = intent.getBooleanExtra(DBPvr.IF_SKIP_TASK, false);
        
        if (pvr_ID == null || pvr_ID == -1)
        {
            
            pvr_ID = getResultExtras(false).getInt(DBPvr.PVR_ID, -1);
            
            skip_Task = getResultExtras(false).getBoolean(DBPvr.IF_SKIP_TASK, false);
            
        }
        
        LogUtils.e(TAG, "The pvr_ID = " + pvr_ID);
        LogUtils.e(TAG, "The skip flag in PvrTaskCompleteReceiver is " + skip_Task);
        
        if (pvr_ID < 0)
        {
            
            Log.w(TAG, "After the pvr record task completed, the pvr_id passed was illegal.");
            
            Log.w(TAG,
                "Please check that if the stop record command is send by the use press the stop key on remote control!!!The pvr_ID = "
                    + pvr_ID);
            
        }
        
        PvrDao pvrDao;
        
        pvrDao = new PvrDaoImpl(context);
        
        PvrBean pvrBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
        
        AlarmTaskUtil alarmUtil = new AlarmTaskUtil();
        
        if (skip_Task)
        {
            
            // 需要跳过当前条目
            if (pvrBean != null && pvrBean.getPvrMode() == DBPvr.DO_ONCE_MODE)
            {
                
                pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                
                pvrDao.update_Record_Task(pvrBean);
                
                // It does not need to set the next task..
                // Because the system will monitor the database's change.
                // When the database is changed, then it will clear all tasks and set the earliest one automatically.
                // Acutally if you set the task here will cause the task be executed twice at the same time..
                // So don't do it !!! Don't call alarmUtil.setNextTask() here.
                
            }
            else
            {
                
                Log.e("Alarm", "OnPvrTaskCompleteReceiver.onReceive: setNextTask1111111");
                
                alarmUtil.setNextTask(context, new SetNextTaskCallBack(context, pvr_ID));
            }
            
        }
        else
        {
            
            Calendar calendar = Calendar.getInstance();
            
            long currentTime = calendar.getTimeInMillis();
            
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);
            
            if (pvrBean != null)
            {
                
                calendar.add(Calendar.MILLISECOND, (int)(pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()));
                
            }
            
            long endTime = calendar.getTimeInMillis();
            
            // Notice, I think the "endTime > currentTime" should not be changed
            // to "endTime >= currentTime"..
            // For maybe exists this situation, thats the record task's length
            // is zero ;
            if (endTime > currentTime)
            {
                
                Log.e("Alarm", "OnPvrTaskCompleteReceiver.onReceive: setNextTask2222222");
                
                Log.e(TAG,
                    "here should never be invoke...If you see this message, it's very possible that you occuor some fucking error...!!But it was lovely too~~~See it as your first love!!!");
                
                alarmUtil.setNextTask(context, new TaskCompleteCallBack(context));
                
            }
            else
            {
                
                if (pvrBean != null && pvrBean.getPvrMode() == DBPvr.DO_ONCE_MODE)
                {
                    
                    pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                    
                    pvrDao.update_Record_Task(pvrBean);
                    
                }
                else
                {
                    
                    Log.e("Alarm", "OnPvrTaskCompleteReceiver.onReceive: setNextTask3333333");
                    
                    alarmUtil.setNextTask(context, new TaskCompleteCallBack(context));
                    
                }
            }
        }
    }
}
