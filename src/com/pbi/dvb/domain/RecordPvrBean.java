package com.pbi.dvb.domain;

import java.io.File;
import java.io.IOException;
import java.util.Calendar;
import java.util.Date;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

import com.pbi.dvb.SplashActivity;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.MountInfoBean;
import com.pbi.dvb.domain.PvrRunningTask;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.DvbPVR_Rec_t;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Rec_Status_t;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.pvrinterface.PvrTimerTaskCallBack;
import com.pbi.dvb.pvrinterface.impl.NewPvrTimerTaskUtil;
import com.pbi.dvb.utils.KeySimulationManager;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.StorageUtils;

public abstract class RecordPvrBean extends PvrRunningTask
{
    
    private static final String TAG = "RecordPvrBean";
    
    private NativePlayer nPlayer = null;
    
    private NewPvrTimerTaskUtil timerTaskUtil = null;
    
    private PvrTimerTaskCallBack hdd_Error_Call_Back = null;
    
    private PvrTimerTaskCallBack record_Finish_Call_Back = null;
    
    private PvrTimerTaskCallBack task_Start_Call_Back = null;
    
    private NativeDrive nativeDrive;
    
    protected RecordPvrBean()
    {
        
    }
    
    protected void startRecord(final Context context)
    {
        nativeDrive = new NativeDrive(context);
        nativeDrive.pvwareDRVInit();
        nativeDrive.CAInit();
        
        LogUtils.e(TAG, "---->>>> RecordPvrBean::startRecord() run 111<<<<----");
        
        // TODO:Check!!! When start to record something...We need to change the
        // reocrd status to Running state...But I don't know wheather here will
        // make some BUG...
        if (getServiceBean() == null)
        {
            
            Log.e(TAG, "Can not find the ServiceInfoBean..");
            
            Log.e(TAG, "ServiceID = " + (getServiceId() == null ? "null" : getServiceId()) + " logicalNumber = "
                + (getLogicalNumber() == null ? "null" : getLogicalNumber()));
            
            stopRecord(DBPvr.PVR_STOP_TYPE_ERROR);
            
            return;
        }
        
        final Calendar mStartTime = Calendar.getInstance();
        
        mStartTime.set(Calendar.HOUR_OF_DAY, 0);
        mStartTime.set(Calendar.MINUTE, 0);
        mStartTime.set(Calendar.SECOND, 0);
        mStartTime.set(Calendar.MILLISECOND, 0);
        mStartTime.add(Calendar.MILLISECOND, getPvrStartTime() == null ? 0 : getPvrStartTime().intValue());
        
        LogUtils.e(TAG, "getPvrStartTime() = " + getPvrStartTime());
        
        Thread startDelay = new Thread()
        {
            
            public void run()
            {
                
                final StorageUtils sUtil = new StorageUtils(context);
                
                if (sUtil.checkHDDSpace() != 0)
                {
                    
                    Log.e(TAG, "sUtil.checkHDDSpace() != 0 ;  checkHDDSpace() = " + sUtil.checkHDDSpace());
                    
                    if (hdd_Error_Call_Back != null)
                    {
                        
                        hdd_Error_Call_Back.pvrTaskCallBack();
                        
                    }
                    
                    Calendar midNight = Calendar.getInstance();
                    
                    midNight.set(Calendar.HOUR_OF_DAY, 0);
                    midNight.set(Calendar.MINUTE, 0);
                    midNight.set(Calendar.SECOND, 0);
                    midNight.set(Calendar.MILLISECOND, 0);
                    
                    long midNightTime = midNight.getTimeInMillis();
                    
                    while (sUtil.checkHDDSpace() != 0)
                    {
                        
                        try
                        {
                            
                            Thread.sleep(1000);
                            
                        }
                        catch (InterruptedException e)
                        {
                            
                            e.printStackTrace();
                            
                        }
                        
                        if ((Calendar.getInstance().getTimeInMillis() - midNightTime) > (getPvrStartTime() + getPvrRecordLength()))
                        {
                            
                            // ���̿ռ䲻��ȴ�ʱ, ���whileѭ��
                            Log.e(TAG,
                                "The time wait for inserting an space enough disk is time out...Jump out of the while loop");
                            
                            break;
                        }
                        
                    }
                    
                }
                else
                {
                    
                    Log.e(TAG, "-------->>>>The storage state is OK...<<<<--------");
                    Log.e(TAG, "-------->>>>tUtil.checkHDDSpace() == 0--------");
                    
                }
                
                Integer logical_Number = getLogicalNumber();
                
                Calendar currentTime = Calendar.getInstance();
                
                long waitTime = mStartTime.getTimeInMillis() - currentTime.getTimeInMillis();
                
                if ((waitTime > 0) || (waitTime < 0 && (-1 * waitTime < getPvrRecordLength())))
                {
                    
                    Log.e(TAG, "a mei you guo qi de RecordTask run!!!");
                    
                    int year = mStartTime.get(Calendar.YEAR);
                    int month = mStartTime.get(Calendar.MONTH);
                    int day = mStartTime.get(Calendar.DAY_OF_MONTH);
                    int hour = mStartTime.get(Calendar.HOUR_OF_DAY);
                    int minute = mStartTime.get(Calendar.MINUTE);
                    int second = mStartTime.get(Calendar.SECOND);
                    
                    month = month + 1;
                    
                    String diskPath = null;
                    
                    MountInfoBean mountInfo = sUtil.getMobileHDDInfo();
                    
                    if (mountInfo != null)
                    {
                        
                        diskPath = mountInfo.getPath();
                        
                    }
                    
                    diskPath = diskPath + "/pvr";
                    
                    File file = new File(diskPath);
                    
                    if (file.exists() && !file.isDirectory())
                    {
                        // File ����һ���ļ���
                        boolean delFileFlag = file.delete();
                        
                        Log.e(TAG, "delete " + file.getAbsolutePath() + "   " + file.getName()
                            + (delFileFlag == true ? "success" : "failure"));
                        
                    }
                    else if (!file.exists())
                    {
                        
                        file.mkdir();
                        
                    }
                    
                    String tsFileName =
                        diskPath + "/[" + getServiceBean().getChannelName() + "][" + year + "-"
                            + (month < 10 ? "0" + month : month) + "-" + (day < 10 ? "0" + day : day) + "_"
                            + (hour < 10 ? "0" + hour : hour) + "-" + (minute < 10 ? "0" + minute : minute) + "-"
                            + (second < 10 ? "0" + second : second) + "].ts";
                    
                    if (nPlayer == null)
                    {
                        
                        nPlayer = NativePlayer.getInstance();
                        
                    }
                    else
                    {
                        // nPlayer.DVBPlayerPvrRecStop();
                        // nPlayer.DVBPlayerPvrDeInit();
                    }
                    Log.e(TAG, "zxguan========Pvr========DVBPlayerInit====================\n");
                    nPlayer.DVBPlayerInit(null);
                    
                    Log.e(TAG, "RecordPvrBean::nPlayer = " + (nPlayer == null ? "null" : "!kong"));
                    
                    // TODO:
                    PVR_Rec_Status_t recFile = nPlayer.new PVR_Rec_Status_t();
                    nPlayer.DVBPlayerPvrRecGetStatus(recFile);
                    int rec_status = recFile.getEnState();
                    
                    // The DVB is in recording status...
                    if (rec_status == 2 || rec_status == 3)
                    {
                        
                        nPlayer.DVBPlayerPvrRecStop();
                        
                    }
                    
                    // NativeDrive nativeDrive = new NativeDrive(context);
                    // nativeDrive.pvwareDRVInit();
                    
                    // load player
                    // NativePlayer dvbPlayer = new NativePlayer();
                    
                    // nPlayer.DVBPlayerPvrInit();
                    
                    int tpID = getServiceBean().getTpId();
                    TpInfoDao tpDao = new TpInfoDaoImpl(context);
                    
                    TPInfoBean tpBean = tpDao.getTpInfo(tpID);
                    
                    DvbPVR_Rec_t tPvrRecInfo = nPlayer.new DvbPVR_Rec_t();
                    
                    tPvrRecInfo.u8RecFlag = 0;
                    tPvrRecInfo.u16PmtPid = getServiceBean().getPmtPid();
                    tPvrRecInfo.u16ServId = getServiceBean().getServiceId();
                    // tPvrRecInfo.sName = "stest.ts";
                    tPvrRecInfo.sName = tsFileName;
                    // tPvrRecInfo.u64MaxFileLength =
                    // (sUtil.getMobileHDDInfo().getFreeSize() - 10 * 1024 *
                    // 1024);
                    tPvrRecInfo.u64MaxFileLength = (sUtil.getMobileHDDInfo().getFreeSize() - 50);
                    tPvrRecInfo.u16LogicNum = logical_Number == null ? 0 : logical_Number.intValue();
                    tPvrRecInfo.eTunerType = tpBean.getTunerType();
                    
                    Log.e(TAG, "tsFileName = " + tPvrRecInfo.sName);
                    
                    Tuner tuner = new Tuner();
                    DVBCore_Cab_Desc_t tCableInfo = tuner.new DVBCore_Cab_Desc_t();
                    
                    tCableInfo.seteFEC_Inner(tpBean.getEfecInner());
                    tCableInfo.seteFEC_Outer(tpBean.getEfecOuter());
                    tCableInfo.seteMod(tpBean.getTunerEmod());
                    tCableInfo.setU32Freq(tpBean.getTunerFreq());
                    tCableInfo.setU32SymbRate(tpBean.getTunerSymbRate());
                    tCableInfo.setU8TunerId(tpBean.getTunerId());
                    
                    tPvrRecInfo.tCableInfo = tCableInfo;
                    
                    waitTime = mStartTime.getTimeInMillis() - Calendar.getInstance().getTimeInMillis();
                    
                    // if (waitTime >= 5000)
                    if (waitTime >= DBPvr.DVB_INIT_DELAY)
                    {
                        try
                        {
                            
                            Thread.sleep(waitTime - DBPvr.DVB_INIT_DELAY);
                            
                        }
                        catch (InterruptedException e)
                        {
                            
                            e.printStackTrace();
                            
                        }
                    }
                    
                    // if (waitTime > DBPvr.DO_TASK_DELAY) {
                    //
                    // Thread.sleep(DBPvr.DO_TASK_DELAY);
                    //
                    // } else if (waitTime > 0) {
                    //
                    // Thread.sleep(waitTime);
                    //
                    // }
                    
                    synchronized (SplashActivity.class)
                    {
                        
                        SharedPreferences recSP = context.getSharedPreferences("dvb_rec", Context.MODE_PRIVATE);
                        
                        Editor recEditor = recSP.edit();
                        recEditor.putInt("rec_serviceId", getServiceId());
                        recEditor.putInt("rec_logicalNumber", getLogicalNumber());
                        recEditor.putString("rec_serviceName", getServiceBean().getChannelName());
                        recEditor.putInt("rec_servicePosition", getServiceBean().getChannelPosition());
                        
                        Log.e(TAG, "run sp for dvb_rec.xml... ");
                        Log.e(TAG, "serviceID = " + getServiceId() + " logicalNumber = " + getLogicalNumber()
                            + "  serviceName = " + getServiceBean().getChannelName());
                        
                        recEditor.commit();
                        
                        File localVideoControlFile = new File("/mnt/expand/RunDvbPvr");
                        
                        int count = 0;
                        
                        while (!localVideoControlFile.exists())
                        {
                            count++;
                            
                            try
                            {
                                boolean createFlag = localVideoControlFile.createNewFile();
                                
                                if (createFlag)
                                {
                                    LogUtils.e(TAG,
                                        "---->>>> create the Launch main page video control file success... <<<<----");
                                }
                                
                                if (count > 10)
                                {
                                    break;
                                }
                            }
                            catch (IOException e)
                            {
                                e.printStackTrace();
                            }
                        }
                        
                        /* The type of TASK_EPG_START_WITHOUT_DIALOG means that the task is started at EPG page... */
                        if (getPvr_type_tag() != DBPvr.TASK_EPG_START_WITHOUT_DIALOG)
                        {
                            KeySimulationManager.sendHomeKey();
                        }
                        
                        Log.e(TAG, "Start record now !!!! Now is " + new Date().toString());
                        
                        nPlayer.DVBPlayerPvrRecStart(tPvrRecInfo);
                        
                        if (null != task_Start_Call_Back)
                        {
                            task_Start_Call_Back.pvrTaskCallBack();
                        }
                        
                        // Change the PvrRunningTask's status into running state
                        pvr_Task_State = DBPvr.PVR_TASK_EXECUTING;
                        
                        // waitTime = mStartTime.getTimeInMillis()
                        // + runningPvrBean.getPvrRecordLength()
                        // - Calendar.getInstance().getTimeInMillis();
                        //
                        // if (waitTime > 0) {
                        //
                        // Log.e(TAG,
                        // "real record length = "
                        // + waitTime
                        // + " setRecordLength = "
                        // + runningPvrBean
                        // .getPvrRecordLength());
                        //
                        // try {
                        //
                        // Thread.sleep(waitTime);
                        //
                        // stopRecord();
                        //
                        // } catch (InterruptedException e) {
                        //
                        // e.printStackTrace();
                        //
                        // }
                        //
                        // }
                        
                        // Here will run quickly as soon as the
                        // nPlayer.DVBPlayerPvrRecStart() run...
                        // It means that if "Start record now" run at 13:00, the
                        // finished run at 13:00 or 13:01..
                        LogUtils.e(TAG, "-------->>>>nPlayer.DVBPlayerPvrRecStart() finished<<<<-------- Now is "
                            + new Date().toString());
                        
                    }
                    
                    return;
                    
                }
            }
            
        };
        
        startDelay.start();
        
        // TODO:
        // long waitTime = mStartTime.getTimeInMillis()
        // + runningPvrBean.getPvrRecordLength()
        // - Calendar.getInstance().getTimeInMillis();
        //
        // Log.e(TAG, "real record length = " + waitTime + " setRecordLength = "
        // + runningPvrBean.getPvrRecordLength());
        
        long waitTime = mStartTime.getTimeInMillis() + getPvrRecordLength() - Calendar.getInstance().getTimeInMillis();
        
        Log.e(TAG, "real record length = " + waitTime + " setRecordLength = " + getPvrRecordLength());
        
        if (timerTaskUtil != null)
        {
            
            timerTaskUtil.stopTask();
            
            timerTaskUtil = null;
            
        }
        
        if (waitTime > 0)
        {
            
            // timerTaskUtil = new PvrTimerTaskUtil();
            //
            // timerTaskUtil.start(new ReocrdTimerTask(handler), waitTime);
            timerTaskUtil = new NewPvrTimerTaskUtil(waitTime, new PvrTimerTaskCallBack()
            {
                
                @Override
                public void pvrTaskCallBack()
                {
                    
                    LogUtils.e(TAG,
                        "---->>>> RecordPvrBean::pvrTaskCallBack() Task time record callBack runned!!! <<<<----");
                    
                    stopRecord(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
                    
                    if (record_Finish_Call_Back != null)
                    {
                        
                        LogUtils.e(TAG, "---->>>> Task finished callBack is runned <<<<----");
                        
                        record_Finish_Call_Back.pvrTaskCallBack();
                        
                    }
                    
                }
            });
            
            timerTaskUtil.start();
            
        }
        else
        {
            
            Log.e(TAG, "A time out record task...recordLength(waitTime) = " + waitTime);
            
            stopRecord(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
        }
        
    }
    
    protected void stopRecord(Integer stopType)
    {
        
        LogUtils.e(TAG, "---->>>> RecordPvrBean::stopRecord() STOP_RECORD... <<<<----", true);
        
        LogUtils.e(TAG, "---->>>> stopType = " + stopType + "<<<<----");
        
        if (timerTaskUtil != null)
        {
            
            timerTaskUtil.stopTask();
            
            timerTaskUtil = null;
            
        }
        
        if (nPlayer == null)
        {
            
            LogUtils.e(TAG,
                "RecordPvrBean::stopRecord() The nPlayer is null... Please check if the record task is started by user and not started by system.",
                true);
            
            nPlayer = NativePlayer.getInstance();
            
        }
        
        PVR_Rec_Status_t recFile = nPlayer.new PVR_Rec_Status_t();
        
        nPlayer.DVBPlayerPvrRecGetStatus(recFile);
        
        int rec_status = recFile.getEnState();
        
        Log.e(TAG, "rec_status = " + rec_status);
        
        // 2 �� 3 �ǿ�gtSong�Ĵ��볭������, ���岻̫������ʲô��˼.�Ǵ��������¼�Ƶ���˼��.
        if (rec_status == 2 || rec_status == 3)
        {
            
            Log.e(TAG, "********nPlayer.DVBPlayerPvrRecStop() runs!!!!!!");
            
            nPlayer.DVBPlayerPvrRecStop();
            
            // nPlayer.DVBPlayerPvrDeInit();
            
        }
        else
        {
            
            Log.e(TAG, "When running the stopRecord() function, the hardware seems that not in recording mode...");
            
        }
        
        // runningPvrBean = null;
        
        swapTaskState(stopType);
        
    }
    
    public void setHdd_Error_Call_Back(PvrTimerTaskCallBack hdd_Error_Call_Back)
    {
        this.hdd_Error_Call_Back = hdd_Error_Call_Back;
    }
    
    public void setRecord_Finish_Call_Back(PvrTimerTaskCallBack record_Finish_Call_Back)
    {
        this.record_Finish_Call_Back = record_Finish_Call_Back;
    }
    
    public void setTask_Start_Call_Back(PvrTimerTaskCallBack task_Start_Call_Back)
    {
        this.task_Start_Call_Back = task_Start_Call_Back;
    }
    
    @Override
    public String toString()
    {
        return "RecordPvrBean [checkTaskExpired()=" + checkTaskExpired() + ", toString()=" + super.toString()
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
