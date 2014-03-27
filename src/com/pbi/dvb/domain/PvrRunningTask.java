package com.pbi.dvb.domain;

import java.util.Calendar;
import java.util.Date;

import android.content.Context;
import android.util.Log;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.pvrinterface.PvrTimerTaskCallBack;
import com.pbi.dvb.pvrinterface.impl.NewPvrTimerTaskUtil;
import com.pbi.dvb.pvrinterface.impl.SetNextTaskCallBack;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.utils.AlarmTaskUtil;
import com.pbi.dvb.utils.LogUtils;

public abstract class PvrRunningTask extends PvrBean
{
    
    private static final String TAG = "PvrRunningTask";
    
    private static Long lastTriggerTime = Long.MAX_VALUE;
    
    protected static PvrRunningTask runningPvrBean = null;
    
    protected static Integer pvr_Task_State;
    
    private NewPvrTimerTaskUtil timerTaskUtil;
    
    protected PvrRunningTask()
    {
    }
    
    protected abstract PvrRunningTask getPreRunInstance(PvrBean pvrBean);
    
    public static PvrRunningTask getInstance()
    {
        return runningPvrBean;
    }
    
    public static PvrRunningTask getInstance(Context context, int pvr_ID)
    {
        
        PvrBean newBean = null;
        
        // If originalPvrBean is null...
        // It means that the PVR task is first setted...
        // Or it means that the task has been started but no finish yet..
        // Or it means that the task has been started and has finished but not
        // call the stopTask()
        LogUtils.e(TAG, "---->>>> PvrRunningTask::getInstance(Context context, int pvr_ID) runs... <<<<----");
        LogUtils.e(TAG, "---->>>> The pvr_ID of task that expected to be runned is " + pvr_ID + " <<<<----");
        
        if (runningPvrBean == null)
        {
            // The task is just running... New a new one...
            
            LogUtils.e(TAG, "---->>>> The PvrRunningTask start first(I think so...) Let's new a new one...<<<<----");
            
            PvrDao pvrDao = new PvrDaoImpl(context);
            
            newBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
            
        }
        else
        {
            if (pvr_Task_State == DBPvr.PVR_TASK_INITIALIZATION)
            {
                LogUtils.e(TAG,
                    "---->>>> The state of the PvrRunningTask is DBPvr.PVR_TASK_INITIALTION... Do not need to get a new one...<<<<----");
                
                // Doing nothing here for newBean = null ;
                
            }
            else if (pvr_Task_State == DBPvr.PVR_TASK_EXECUTING)
            {
                // ����û���...ΪʲôҪ��ʼ��???�������쳣��~~~???
                
                // ����....
                // ����Ǹ���������, ��������Ӧ��ʹ����������...???What does this mean??
                
                // ����...
                // ���¼�ƹ��, ��̨Service�쳣��ֹ����ô��...��ʱ��runningPvrBean�Ǵ��ڵ�,
                // �����runningPvrBean����Ѿ�������...
                // �����Ҫ�ж�һ��runningPvrBean�Ƿ��Ѿ�������.
                // ���������ܵ�ǰ����Ҫִ��,,,����=.=���Ѿ�����һ����...������Ϊ������Ҫִ�о��ж�Ϊ����ִ��...
                // ����...��������������Ҫִ��, �����ǵ����...��ô...�϶�Ҫ�ж�Ϊ����ִ�е���!!!
                
                LogUtils.e(TAG, "---->>>> The task is being executed now... Check if the task is expired... <<<<----");
                
                // �ظ�������, �ж�һ�������Ƿ������Ҫ����.
                Calendar current = Calendar.getInstance();
                
                long currentTime = current.getTimeInMillis();
                
                // The task maybe is started yesterday..But no one change the
                // PVR_TASK_EXECUTING state to stopState ;
                // When does the lastTriggerTime set?
                // We should check if we need to minus DBPvr.TASK_TRIGGER_DELAY here...
                // Because when we calculate the trigger time, we have minus it onece...
                // if (currentTime >= (lastTriggerTime + 24 * 60 * 60 * 1000 - DBPvr.TASK_START_DELAY))
                if (currentTime >= (lastTriggerTime + 24 * 60 * 60 * 1000 - DBPvr.TASK_TRIGGER_DELAY))
                {
                    
                    LogUtils.e(TAG, "---->>>> The task was started yesterday... <<<<----");
                    
                    // ���������...��Ҫ����
                    runningPvrBean.stopTask(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
                    
                    PvrDao pvrDao = new PvrDaoImpl(context);
                    
                    newBean = (RecordPvrBean)pvrDao.get_Record_Task_By_ID(pvr_ID);
                    
                }
                else
                {
                    
                    // ����û���pvr_ID��������ִ�й�����޸�����������, �ͻ�ִ������
                    // �������ﻹ��Ҫ�ж�һ�µ�ǰ�����Ƿ��Ѿ�����...Ҳ����˵, ��ʹ��ÿ�������,����Ҳ������Ϊ����ԭ�������쳣��ֹ.
                    // ����������13:00 ~ 14:00��, ��ʱ����15:00��, �ǿ϶��������͹�����...
                    
                    LogUtils.e(TAG,
                        "---->>>> The task was not started yesterday...Check if user change the data while the task is running...Or the task has finished but system didn't set the stop state to task.. <<<<----");
                    
                    current.set(Calendar.HOUR_OF_DAY, 0);
                    current.set(Calendar.MINUTE, 0);
                    current.set(Calendar.SECOND, 0);
                    current.set(Calendar.MILLISECOND, 0);
                    
                    long currentDate = current.getTimeInMillis();
                    int week_Of_Today = current.get(Calendar.DAY_OF_WEEK);
                    currentTime = currentTime - currentDate;
                    
                    long adjustTime = runningPvrBean.get_Adjust_StartTime(currentDate, currentTime, week_Of_Today);
                    
                    Log.e(TAG, "currentTime + adjustTime = " + (currentTime + adjustTime) + " = "
                        + new Date(currentTime + adjustTime).toString() + "    ...currentTime = "
                        + new Date(currentTime).toString() + " adjustTime = " + adjustTime);
                    
                    Log.e(TAG,
                        "runningPvrBean.getPvrStartTime() + runningPvrBean.getPvrRecordLength() = "
                            + (runningPvrBean.getPvrStartTime() + runningPvrBean.getPvrRecordLength())
                            + " ....runningPvrBean.getPvrStartTime() = "
                            + new Date(runningPvrBean.getPvrStartTime()).toString() + "  recordLength = "
                            + runningPvrBean.getPvrRecordLength());
                    
                    if ((currentTime + adjustTime) == runningPvrBean.getPvrStartTime())
                    {
                        
                        LogUtils.e(TAG, "---->>>> The task is running...Can not start a new one... <<<<----");
                        
                        // ��������ִ��, ��Ӧ�ÿ���������
                        
                    }
                    else
                    {
                        
                        LogUtils.e(TAG, "---->>>> The task is expired...Let's get a new one~ <<<<----");
                        
                        // �����Ѿ�������...��ȡ��һ����~~
                        // ��ǰpvrBean�Ѿ�������, �µ�pvrID���µ�����
                        // ��ʱ����ʵ�����Ѿ�ִ������, ��һ�ε�stopTask(), �ǲ���Ҫ�ദ��һЩʲô������?
                        runningPvrBean.stopTask(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
                        
                        PvrDao pvrDao = new PvrDaoImpl(context);
                        
                        newBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
                    }
                    
                }
                
            }
            else if (pvr_Task_State == DBPvr.PVR_TASK_FINISHED)
            {
                
                LogUtils.e(TAG, "---->>>> The task has finished yet...Let's new a new one~ <<<<----");
                
                PvrDao pvrDao = new PvrDaoImpl(context);
                
                newBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
                
            }
            else if (pvr_Task_State == DBPvr.PVR_TASK_CANCEL)
            {
                // If there is a everyday task in the system...
                // After the task has finished...The user start recording manually and then stop it..
                // Then the task's state will be changed from finished to cancel
                // At this moment, if the user changed the task's data, then it can be started again..
                // If the user don't chagne the task's data, then at the second day...
                // The task will be recard as a canceled one...
                // So if the task is in cancel state, we should judge that if the task is a new day task..
                // Or if the cancel task is not in running time...(if so)
                LogUtils.e(TAG,
                    "---->>>> The task is canceled by user manually... Check if need to start a new one...<<<<----");
                
                // If user stop the task manually...Than if the task need to be executed right now... We regard it as a
                // task that need to be ignored
                LogUtils.e(TAG, "---->>>> The task has been canceled...Check if start the same one... <<<<----");
                
                PvrDao pvrDao = new PvrDaoImpl(context);
                
                PvrBean checkBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
                
                LogUtils.e(TAG, "---->>>> The checkBean is :: <<<<----");
                
                LogUtils.e(TAG, checkBean.toString());
                
                // �ظ�������, �ж�һ�������Ƿ������Ҫ����.
                Calendar current = Calendar.getInstance();
                
                long currentTime = current.getTimeInMillis();
                
                // The task maybe is started yesterday..But no one change the
                // PVR_TASK_EXECUTING state to stopState ;
                // When does the lastTriggerTime set?
                // We should check if we need to minus DBPvr.TASK_TRIGGER_DELAY here...
                // Because when we calculate the trigger time, we have minus it onece...
                // if (currentTime >= (lastTriggerTime + 24 * 60 * 60 * 1000 - DBPvr.TASK_START_DELAY))
                if (runningPvrBean.getPvrId() == pvr_ID)
                {
                    LogUtils.e(TAG, "---->>>> The originalPvrBean's id is the same as the new one...");
                    
                    // ��ʵ����Ӧ��дһ���ȽϺ���=.=''
                    // ����: originalPvrBean.compareTo(checkBean) ;
                    if (currentTime >= (lastTriggerTime + 24 * 60 * 60 * 1000))
                    {
                        
                        LogUtils.e(TAG, "---->>>> The task was canceled yesterday... <<<<----");
                        
                        newBean = checkBean;
                        
                    }
                    else if ((runningPvrBean.getPvrStartTime().longValue() != checkBean.getPvrStartTime().longValue())
                        || (runningPvrBean.getPvrRecordLength().longValue() != checkBean.getPvrRecordLength()
                            .longValue()))
                    {
                        
                        LogUtils.e(TAG,
                            "---->>>> The task's data has been changed ever... So let's new a new one~~ <<<<----");
                        
                        newBean = checkBean;
                    }
                    else
                    {
                        // Doing nothing for the task is just canceled.. And the task's data does not have any change..
                        
                        // TODO: Actually here should check if the cancel state of the task is expired...
                        // If the state is expired...Then start a new one...
                    }
                    
                }
                else
                {
                    LogUtils.e(TAG, "---->>>> The originalPvrBean's id is not the same with the new one <<<<----");
                    
                    newBean = checkBean;
                    
                }
            }
            else if (pvr_Task_State == DBPvr.PVR_TASK_SKIP)
            {
                LogUtils.e(TAG, "---->>>> The task's state is DBPvr.PVR_TASK_SKIP... Let's new a new one <<<<----");
                
                PvrDao pvrDao = new PvrDaoImpl(context);
                
                newBean = pvrDao.get_Record_Task_By_ID(pvr_ID);
            }
            else
            {
                throw new RuntimeException("---->>>> Unsupported pvr_task_state <<<<----");
            }
        }
        
        // originalPvrBean Ϊnull, ˵����������ִ��
        // originalPvrBean ��Ϊnull, ˵�����Կ�ʼ�µ�����, ����һ����ִ�е�����
        if (newBean != null)
        {
            
            switch (newBean.getPvrWakeMode())
            {
            
                case DBPvr.RECORD_WAKEUP_MODE:
                    
                    PvrRunningTask recordTask = new PvrRunningTask()
                    {
                        
                        @Override
                        public void startTask(Context context)
                        {
                        }
                        
                        @Override
                        protected PvrRunningTask getPreRunInstance(PvrBean pvrBean)
                        {
                            return null;
                        }
                        
                        @Override
                        public void stopTask(Integer stopType)
                        {
                        }
                    };
                    
                    recordTask.new RecordPvrBeanFactory().getPreRunInstance(newBean);
                    
                    break;
                
                case DBPvr.PLAY_WAKEUP_MODE:
                    
                    // PlayPvrBean.getInstance();
                    PvrRunningTask playTask = new PvrRunningTask()
                    {
                        
                        @Override
                        public void startTask(Context context)
                        {
                            
                        }
                        
                        @Override
                        protected PvrRunningTask getPreRunInstance(PvrBean pvrBean)
                        {
                            return null;
                        }
                        
                        @Override
                        public void stopTask(Integer stopType)
                        {
                            
                        }
                    };
                    
                    playTask.new PlayPvrBeanFactory().getPreRunInstance(newBean);
                    
                    break;
                
                default:
                    
                    throw new RuntimeException("Error:: The PVR wake up mode is error...");
                    
            }
            
            newBean = runningPvrBean;
            
        }
        
        // The newBean maybe null or a child of PvrRunningTask here...
        return (PvrRunningTask)newBean;
        
    }
    
    public static boolean checkTaskLegality(long setTriggerTime)
    {
        
        boolean result = true;
        
        Calendar calendar = Calendar.getInstance();
        
        long currentTime = calendar.getTimeInMillis();
        
        if ((currentTime - setTriggerTime) > DBPvr.MAX_RECORD_LENGTH)
        {
            
            LogUtils.e(TAG, "thr set trigger time is " + new Date(setTriggerTime).toString());
            
            LogUtils.e(TAG, "Now is " + new Date().toString());
            
            result = false;
            
        }
        else
        {
            
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);
            
            long currentDate = calendar.getTimeInMillis();
            currentTime = currentTime - calendar.getTimeInMillis();
            int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
            
            long value = runningPvrBean.get_Adjust_StartTime(currentDate, currentTime, week_Of_Today);
            
            if (value == Long.MAX_VALUE)
            {
                
                LogUtils.e(TAG,
                    "PvrRunningTask::checkTaskLegality  The task's start time is not smaller than today, but the trigger time seams that be illegal..");
                
                LogUtils.e(TAG, "Check if the task is a do_once_task");
                
                result = false;
            }
        }
        
        return result;
        
    }
    
    // ���һ�����ʱ��û�е���ʱ��, ��һ��һ���Ե����������ú���...
    // ����� һ���������Ѿ�������, ʱ��ŵ������ʵһ�µ�ʱ��, ��ʱ������ͻ������.
    // ����������һ���������, �������һ��ÿ�յ�����, ��ô������ʵ����������Ѿ�������, ����
    public boolean checkTaskExpired()
    {
        
        boolean result = false;
        
        Calendar calendar = Calendar.getInstance();
        
        long currentTime = calendar.getTimeInMillis();
        
        if ((currentTime - lastTriggerTime) > 365 * 24 * 60 * 60 * 1000)
        {
            
            LogUtils.e(TAG, "thr set trigger time is " + new Date(lastTriggerTime).toString() + "   now is "
                + new Date().toString());
            
            // �����־���Խ��һЩ����, ����������ǰ��������.
            // ��������, ��ǰ�����ǿ�����һ��ִ�е�...
            // ����ʱ������ڴ�DVB��ʱ�����һ�ε���(��֪����̨¼�ƻ᲻��)
            // ��˷��������, ��������ǿ�����һ�δ�DVB�����.
            // ���Ի�, ��һЩ����޸�, �ı䵱ǰ״̬���Ƶķ�ʽ��.
            
            result = true;
            
        }
        
        calendar.set(Calendar.HOUR_OF_DAY, 0);
        calendar.set(Calendar.MINUTE, 0);
        calendar.set(Calendar.SECOND, 0);
        calendar.set(Calendar.MILLISECOND, 0);
        
        long currentDate = calendar.getTimeInMillis();
        currentTime = currentTime - calendar.getTimeInMillis();
        int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
        
        long value = this.get_Adjust_StartTime(currentDate, currentTime, week_Of_Today);
        
        if (value == Long.MAX_VALUE)
        {
            
            LogUtils.e(TAG,
                "PvrRunningTask::checkTaskExpired  The task's start time is not smaller than today, but the trigger time seams that be illegal..");
            
            LogUtils.e(TAG, "Check if the task is a do_once_task");
            
            // ����о����Բ����޸�, ��Ϊ���value��ֵΪLong.MAX_VALUE�Ļ�, ��ô��serNextTask()��ʱ��,
            // dao��ᷢ�����ֵ̫����, ����˵����һ��������, ��ʱ���������ģʽ��Ϊ�ر�״̬.
            result = true;
            
        }
        
        if ((value + currentTime) != this.getPvrStartTime())
        {
            LogUtils.e(TAG,
                "---->>>> the adjust start time + currentTime should be the same as pvr start time <<<<----");
            
            return true;
        }
        
        // Error below...The task will start, then the value very possible that be bigger than 0...
        // else if (value > 0)
        // {
        // // 2013-02-26 17:31 add by linkang
        // LogUtils.e(TAG, "---->>>> the adjust start time gap should be smaller than zero... But it is not <<<<----");
        //
        // return true;
        // }
        
        return result;
    }
    
    public abstract void startTask(Context context);
    
    public abstract void stopTask(Integer stopType);
    
    /*
     * If the task is satisfy the execution condition.. But you��the system or the user want to cancel it Then run this
     * function to change the original task's state..
     */
    public void cancelTask()
    {
        
        LogUtils.e(TAG, "-------->>>> Cancel the current pre task <<<<--------");
        
        if (timerTaskUtil != null)
        {
            
            timerTaskUtil.stopTask();
            
            timerTaskUtil = null;
            
        }
        
        pvr_Task_State = DBPvr.PVR_TASK_CANCEL;
        
    }
    
    /*
     * If current task does not satisfy the execution condition... Such the task's start time is at year of 1970... Then
     * you need to skip current original task... Notice: It's not cancel the task...It's because the task should not run
     * now, should reset it!!!
     */
    public void skipCurrentOriginalTask()
    {
        
        LogUtils.e(TAG, "---->>>> PvrRunningTask::skipCurrentOriginalTask(), skipID = " + getPvrId() + "<<<<----");
        
        pvr_Task_State = DBPvr.PVR_TASK_SKIP;
        
    }
    
    public void gotoNextTask(Context context)
    {
        
        // TODO:
        // ����Ӧ����Ӷ���������ж�...����������ط�, ��һ��Ҫ�ж�=.=''
        AlarmTaskUtil alarmUtil = null;
        
        alarmUtil = new AlarmTaskUtil();
        
        LogUtils.e(TAG, "gotoNextTask(Context context)");
        
        alarmUtil.setNextTask(context, new TaskCompleteCallBack(context));
        
    }
    
    // I don't like the function below...It seems useless.
    public void gotoNextTask(Context context, int pvr_ID)
    {
        
        // TODO:
        // ����Ӧ����Ӷ���������ж�...����������ط�, ��һ��Ҫ�ж�=.=''
        AlarmTaskUtil alarmUtil = null;
        
        alarmUtil = new AlarmTaskUtil();
        
        LogUtils.e(TAG, "gotoNextTask(Context context, int pvr_ID)");
        
        alarmUtil.setNextTask(context, new SetNextTaskCallBack(context, pvr_ID));
        // alarmUtil.setNextTask(context, new SetNextTaskCallBack(context,
        // getPvrId()));
        
    }
    
    private class RecordPvrBeanFactory extends RecordPvrBean
    {
        
        private RecordPvrBeanFactory()
        {
        }
        
        @Override
        protected PvrRunningTask getPreRunInstance(PvrBean pvrbean)
        {
            
            RecordPvrBean result = null;
            
            if (pvrbean != null)
            {
                
                result = new RecordPvrBeanFactory();
                
                result.setPvrId(pvrbean.getPvrId());
                result.setPvrMode(pvrbean.getPvrMode());
                result.setPvrWakeMode(pvrbean.getPvrWakeMode());
                result.setServiceId(pvrbean.getServiceId());
                result.setLogicalNumber(pvrbean.getLogicalNumber());
                
                result.setPvrSetDate(pvrbean.getPvrSetDate());
                result.setPvrStartTime(pvrbean.getPvrStartTime());
                result.setPvrRecordLength(pvrbean.getPvrRecordLength());
                
                result.setPvrWeekDate(pvrbean.getPvrWeekDate());
                result.setPvr_pmt_pid(pvrbean.getPvr_pmt_pid());
                result.setPvr_type_tag(pvrbean.getPvr_type_tag());
                result.setPvr_is_sleep(pvrbean.getPvr_is_sleep());
                
                pvr_Task_State = DBPvr.PVR_TASK_INITIALIZATION;
                
                runningPvrBean = result;
                
            }
            else
            {
                pvr_Task_State = DBPvr.PVR_TASK_UNINITIALIZTION;
                
                throw new IllegalArgumentException(
                    "---->>>> RecordPvrBeanFactory::getPreRunInstance  The pvrBean is null while trying to get the instance...<<<<----");
            }
            
            return result;
            
        }
        
        @Override
        public void startTask(Context context)
        {
            
            LogUtils.e(TAG, "---->>>> RecordPvrBean::startTask() run <<<<----");
            
            startRecord(context);
            
        }
        
        @Override
        public void stopTask(Integer stopType)
        {
            LogUtils.e(TAG, "---->>>> RecordPvrBean::stopTask() run <<<<----");
            
            stopRecord(stopType);
        }
        
    }
    
    private class PlayPvrBeanFactory extends PlayPvrBean
    {
        
        @Override
        protected PvrRunningTask getPreRunInstance(PvrBean pvrBean)
        {
            
            PlayPvrBean result = null;
            
            if (pvrBean != null)
            {
                
                result = new PlayPvrBeanFactory();
                
                result.setPvrId(pvrBean.getPvrId());
                result.setPvrMode(pvrBean.getPvrMode());
                result.setPvrWakeMode(pvrBean.getPvrWakeMode());
                result.setServiceId(pvrBean.getServiceId());
                result.setLogicalNumber(pvrBean.getLogicalNumber());
                
                result.setPvrSetDate(pvrBean.getPvrSetDate());
                result.setPvrStartTime(pvrBean.getPvrStartTime());
                result.setPvrRecordLength(pvrBean.getPvrRecordLength());
                
                result.setPvrWeekDate(pvrBean.getPvrWeekDate());
                result.setPvr_pmt_pid(pvrBean.getPvr_pmt_pid());
                result.setPvr_type_tag(pvrBean.getPvr_type_tag());
                result.setPvr_is_sleep(pvrBean.getPvr_is_sleep());
                
                pvr_Task_State = DBPvr.PVR_TASK_INITIALIZATION;
                
                runningPvrBean = result;
                
            }
            else
            {
                pvr_Task_State = DBPvr.PVR_TASK_UNINITIALIZTION;
                
                throw new IllegalArgumentException(
                    "---->>>> PlayPvrBeanFactory::getPreRunInstance  The pvrBean is null while trying to get the instance...<<<<----");
            }
            
            return result;
            
        }
        
        @Override
        public void startTask(Context context)
        {
            LogUtils.e(TAG, "---->>>> PlayPvrBean::startTask run<<<<----");
            
            startPlay(context);
        }
        
        @Override
        public void stopTask(Integer stopType)
        {
            LogUtils.e(TAG, "---->>>> PlayPvrBean::stopTask run<<<<----");
            
            stopPlay(stopType);
        }
    }
    
    public static Long getLastTriggerTime()
    {
        return lastTriggerTime;
    }
    
    public static void setLastTriggerTime(Long lastTriggerTime)
    {
        PvrRunningTask.lastTriggerTime = lastTriggerTime;
    }
    
    protected void swapTaskState(Integer stopType)
    {
        LogUtils.e(TAG, "---->>>> PvrRunningTask:: swapTaskState() stopType = " + stopType + "<<<<----");
        
        if (stopType != null)
        {
            switch (stopType)
            {
                case DBPvr.PVR_STOP_TYPE_MANUALLY:
                    
                    pvr_Task_State = DBPvr.PVR_TASK_CANCEL;
                    
                    break;
                
                case DBPvr.PVR_STOP_TYPE_AUTOMATICALLY:
                    
                    LogUtils.e(TAG, "---->>>> Change the state of the task into FINISHED <<<<----");
                    
                    pvr_Task_State = DBPvr.PVR_TASK_FINISHED;
                    
                    break;
                
                case DBPvr.PVR_STOP_TYPE_ERROR:
                    
                    // Here may should be change to pvr_Task_State = DBPvr.PVR_TASK_SKIP
                    pvr_Task_State = DBPvr.PVR_TASK_FINISHED;
                    
                    break;
                
                default:
                    
                    throw new IllegalArgumentException("---->>>> Unsupported PVR task stop type <<<<----");
                    
            }
        }
    }
    
    public void checkStartTimeOut(final Context context)
    {
        
        if (timerTaskUtil != null)
        {
            
            timerTaskUtil.stopTask();
            
            timerTaskUtil = null;
            
        }
        
        // Check the TASK_TRIGGER_DELAY here...
        timerTaskUtil = new NewPvrTimerTaskUtil(DBPvr.TASK_TRIGGER_DELAY, new PvrTimerTaskCallBack()
        {
            
            @Override
            public void pvrTaskCallBack()
            {
                
                if (PvrRunningTask.pvr_Task_State == DBPvr.PVR_TASK_INITIALIZATION)
                {
                    
                    skipCurrentOriginalTask();
                    
                    gotoNextTask(context);
                    
                }
                
            }
        });
        
        timerTaskUtil.start();
        
    }
    
    @Override
    public String toString()
    {
        return "PvrRunningTask [getPvrId()=" + getPvrId() + ", getPvrMode()=" + getPvrMode() + ", getPvrWakeMode()="
            + getPvrWakeMode() + ", getServiceId()=" + getServiceId() + ", getLogicalNumber()=" + getLogicalNumber()
            + ", getPvrSetDate()=" + getPvrSetDate() + ", getPvrStartTime()=" + getPvrStartTime()
            + ", getPvrRecordLength()=" + getPvrRecordLength() + ", getPvrWeekDate()=" + getPvrWeekDate()
            + ", getPvr_pmt_pid()=" + getPvr_pmt_pid() + ", getPvr_is_sleep()=" + getPvr_is_sleep()
            + ", getPvr_type_tag()=" + getPvr_type_tag() + ", get_Adjust_StartTime()=" + get_Adjust_StartTime()
            + ", getServiceBean()=" + getServiceBean() + ", checkTimeExpired()=" + checkTimeExpired() + ", toString()="
            + super.toString() + ", getClass()=" + getClass() + ", hashCode()=" + hashCode() + "]";
    }
}
