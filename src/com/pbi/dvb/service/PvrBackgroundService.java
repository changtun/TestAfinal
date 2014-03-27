package com.pbi.dvb.service;

import java.util.Calendar;
import java.util.Date;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.Button;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.aidl.OTADownloaderServiceAIDL;
import com.pbi.dvb.aidl.PvrRecordServiceAIDL;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrRunningTask;
import com.pbi.dvb.domain.RecordPvrBean;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.pvrinterface.PvrTimerTaskCallBack;
import com.pbi.dvb.pvrinterface.impl.TickCountDownUtil;
import com.pbi.dvb.receiver.OnPvrTaskCompleteReceiver;
import com.pbi.dvb.utils.KeySimulationManager;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrDialogManager;
import com.pbi.dvb.utils.StorageUtils;

public class PvrBackgroundService extends Service
{
    
    private static final String TAG = "PvrBackgroundService";
    
    private static final int HDD_SPACE_ERROR = 10;
    
    private static final int START_RECORD = 0;
    
    private static final int RECORD_FINISHED = 1;
    
    private static final int SEND_TASK_COMPLETE_BROADCAST = 2;
    
    protected static final int UPDATE_SUSPEND_OK_BUTTON = 3;
    
    protected static final int SEND_TASK_START_BROADCAST = 4;
    
    private RecordPvrBean pvrBean;
    
    private ServiceInfoBean serviceBean;
    
    private View hdd_Space_Error_Dialog;
    
    private View suspend_Check_Dialog;
    
    private Handler handler = new Handler()
    {
        
        private TickCountDownUtil suspend_Count_Down;
        
        private Button bt_Suspend_Ok;
        
        private OnPvrTaskCompleteReceiver taskCompleteReceiver;
        
        private boolean lock_OTA_Result;
        
        private boolean ota_Binder_Status;
        
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case START_RECORD:
                    Log.e(TAG, "PvrBackgroundService::handler: START_RECORD");
                    startRecord(msg.arg1);
                    break;
                case SEND_TASK_START_BROADCAST:
                    LogUtils.e(TAG, "PvrBackgroundService.handler:: SEND_TASK_START_BROADCAST run...");
                    Intent recordStartIntent = new Intent(Config.PVR_TASK_START_RECORD);
                    sendOrderedBroadcast(recordStartIntent, null, null, null, 0, null, null);
                    break;
                case RECORD_FINISHED:
                    Log.e(TAG, "RECORD_FINISHED... Now is " + new Date().toString());
                    if (pvrBean != null)
                    {
                        // pvrBean.stopTask();
                        pvrBean.stopTask(msg.arg1);
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "---->>>> When the record task is finished...The pvrBean is null... This means that the task may not be cancel... <<<<----");
                    }
                    
                    if (hdd_Space_Error_Dialog != null)
                    {
                        PvrDialogManager.getInstance().removeView(PvrBackgroundService.this, hdd_Space_Error_Dialog);
                    }
                    
                    // If the pvr is just finished or canceled in some way, then the pvr is not null...
                    // Now let's start recording in DVB and stop it... This will cause the error dialog!!!
                    if (pvrBean != null)
                    {
                        switch (pvrBean.getPvr_is_sleep())
                        {
                            case DBPvr.SUSPEND_AFTER_RECORD:
                            {
                                // ¼�ƺ���
                                suspend_Check_Dialog =
                                    View.inflate(PvrBackgroundService.this, R.layout.pvr_check_box_dialog, null);
                                
                                TextView tv_showPvrTitle =
                                    (TextView)suspend_Check_Dialog.findViewById(R.id.tv_showPvrTitle);
                                
                                tv_showPvrTitle.setText(R.string.pvr_time_suspend_notice);
                                
                                bt_Suspend_Ok = (Button)suspend_Check_Dialog.findViewById(R.id.bt_Ok);
                                
                                Button bt_Suspend_Cancel = (Button)suspend_Check_Dialog.findViewById(R.id.bt_Cancel);
                                
                                bt_Suspend_Ok.setOnFocusChangeListener(new ButtonOnFocusChangeListener());
                                bt_Suspend_Cancel.setOnFocusChangeListener(new ButtonOnFocusChangeListener());
                                
                                bt_Suspend_Ok.setTag(suspend_Check_Dialog);
                                bt_Suspend_Cancel.setTag(suspend_Check_Dialog);
                                
                                bt_Suspend_Ok.setOnClickListener(new OnClickListener()
                                {
                                    @Override
                                    public void onClick(View v)
                                    {
                                        // ȡ��ť����ʱ
                                        if (suspend_Count_Down != null)
                                        {
                                            suspend_Count_Down.setRunFlag(false);
                                            
                                            suspend_Count_Down = null;
                                        }
                                        
                                        // ����Ļ���Ƴ�Ի���?
                                        
                                        PvrDialogManager.getInstance().removeView(PvrBackgroundService.this,
                                            (View)v.getTag());
                                        
                                        // May need to be canceled.
                                        handler.sendEmptyMessage(PvrBackgroundService.SEND_TASK_COMPLETE_BROADCAST);
                                        
                                        // ������
                                        // sendSuspendKey();
                                        KeySimulationManager.sendSuspendLKey();
                                    }
                                });
                                
                                bt_Suspend_Cancel.setOnClickListener(new OnClickListener()
                                {
                                    @Override
                                    public void onClick(View v)
                                    {
                                        if (suspend_Count_Down != null)
                                        {
                                            suspend_Count_Down.setRunFlag(false);
                                            
                                            suspend_Count_Down = null;
                                        }
                                        
                                        // ����Ļ���Ƴ�Ի���?
                                        PvrDialogManager.getInstance().removeView(PvrBackgroundService.this,
                                            (View)v.getTag());
                                        
                                        // ���ͷ��͹㲥������
                                        handler.sendEmptyMessage(PvrBackgroundService.SEND_TASK_COMPLETE_BROADCAST);
                                        
                                        bt_Suspend_Ok = null;
                                    }
                                });
                                
                                PvrDialogManager.getInstance().adjustView(PvrBackgroundService.this,
                                    suspend_Check_Dialog,
                                    R.id.tv_showPvrTitle,
                                    R.id.ll_buttons);
                                
                                PvrDialogManager.getInstance().addView(PvrBackgroundService.this, suspend_Check_Dialog);
                                
                                if (suspend_Count_Down != null)
                                {
                                    
                                    suspend_Count_Down.setRunFlag(false);
                                    
                                    suspend_Count_Down = null;
                                }
                                
                                // PvrRecordService.UPDATE_OK_BUTTON
                                suspend_Count_Down =
                                    new TickCountDownUtil(handler, DBPvr.TASK_SUSPEND_DELAY, UPDATE_SUSPEND_OK_BUTTON);
                                
                                suspend_Count_Down.start();
                                
                                break;
                            }
                            
                            case DBPvr.DO_NOT_SUSPEND_AFTER_RECORD:
                            {
                                handler.sendEmptyMessage(PvrBackgroundService.SEND_TASK_COMPLETE_BROADCAST);
                                
                                break;
                            }
                            
                            default:
                            {
                                Log.e(TAG,
                                    "Illegal suspend state here. You should tell the system to suspend with a state value 0, or to set next task without suspend with a state value 1");
                                
                                // This handler message may should not be send out...
                                handler.sendEmptyMessage(PvrBackgroundService.SEND_TASK_COMPLETE_BROADCAST);
                                
                                break;
                            }
                        }
                        
                    }
                    else
                    {
                        
                        // We reguard as that if pvrBean is null, than the recored
                        // task is start by user control..
                        // That means the record task is not a timing record task...
                        // So here may just not do anything... Do not need to tell
                        // the system to set next task to AlarmManager
                        
                        // But here also means that all the record task is canceled
                        // by the version before 2013-01-07...So set next task is
                        // also illegal
                        
                        Intent broadIntent = new Intent(Config.PVR_TASK_COMPLETE);
                        
                        Bundle pvrMessage = new Bundle();
                        
                        pvrMessage.putInt(DBPvr.PVR_ID, -1);
                        
                        pvrMessage.putBoolean(DBPvr.IF_SKIP_TASK, false);
                        
                        if (taskCompleteReceiver == null)
                        {
                            
                            taskCompleteReceiver = new OnPvrTaskCompleteReceiver();
                            
                        }
                        
                        sendOrderedBroadcast(broadIntent, null, taskCompleteReceiver, null, 0, null, pvrMessage);
                        
                    }
                    
                    break;
                case SEND_TASK_COMPLETE_BROADCAST:
                    LogUtils.e(TAG, "SEND_TASK_COMPLETE_BROADCAST run...");
                    
                    // unlock the OTA status
                    // Check if the ota is in downloading status...
                    Intent service = new Intent();
                    
                    service.setAction("com.pbi.dvb.service.ota_downloader_service");
                    
                    lock_OTA_Result = false;
                    
                    ota_Binder_Status = false;
                    
                    ServiceConnection conn = new ServiceConnection()
                    {
                        private OTADownloaderServiceAIDL otaServiceBinder = null;
                        
                        @Override
                        public void onServiceConnected(ComponentName name, IBinder service)
                        {
                            
                            LogUtils.e(TAG, "SEND_TASK_COMPLETE_BROADCAST::onServiceConnected() run!!!");
                            
                            if (service == null)
                            {
                                
                                LogUtils.e(TAG, "SEND_TASK_COMPLETE_BROADCAST::onServiceConnected:: service == null");
                                
                            }
                            
                            otaServiceBinder = OTADownloaderServiceAIDL.Stub.asInterface(service);
                            
                            if (otaServiceBinder != null)
                            {
                                try
                                {
                                    LogUtils.e(TAG,
                                        "SEND_TASK_COMPLETE_BROADCAST::The otaServiceBinder is not null, try to unlock the ota by unlock_OTA() function...");
                                    
                                    ota_Binder_Status = true;
                                    
                                    // you should try to unlock ota...
                                    lock_OTA_Result =
                                        otaServiceBinder.unlock_OTA(OTADownloaderService.OTA_PVR_LOCK_MASK);
                                    
                                    unbindService(this);
                                    
                                    otaServiceBinder = null;
                                }
                                catch (RemoteException e)
                                {
                                    e.printStackTrace();
                                }
                            }
                            else
                            {
                                LogUtils.e(TAG,
                                    "SEND_TASK_COMPLETE_BROADCAST::The otaServiceBinder is null, can not unlock the ota by unlock_OTA_IP() function... This may cause ota downloading failed");
                            }
                            
                        }
                        
                        @Override
                        public void onServiceDisconnected(ComponentName name)
                        {
                        }
                        
                    };
                    
                    boolean bindFlag = bindService(service, conn, Context.BIND_AUTO_CREATE);
                    
                    if (bindFlag)
                    {
                        LogUtils.e(TAG, "Bind service for lock the ota success");
                    }
                    else
                    {
                        LogUtils.e(TAG, "Bind service for unlock the ota failed");
                    }
                    
                    sendTaskCompleteBroadcast();
                    
                    // pvrBean������Ϊ���߳�ͬ����������һЩBUG...
                    pvrBean = null;
                    
                    break;
                case UPDATE_SUSPEND_OK_BUTTON:
                    if (msg.arg1 == 0)
                    {
                        // �Զ����?
                        PvrDialogManager.getInstance().removeView(PvrBackgroundService.this, suspend_Check_Dialog);
                        
                        // sendSuspendKey();
                        KeySimulationManager.sendSuspendLKey();
                        
                        break;
                        
                    }
                    
                    if (bt_Suspend_Ok != null)
                    {
                        
                        bt_Suspend_Ok.setText(msg.arg1 + "");
                        
                    }
                    
                    break;
                case HDD_SPACE_ERROR:
                    String storage_state_error = null;
                    
                    if (hdd_Space_Error_Dialog == null)
                    {
                        
                        hdd_Space_Error_Dialog =
                            View.inflate(PvrBackgroundService.this, R.layout.pvr_check_box_with_one_button, null);
                        
                    }
                    
                    TextView tv_showTaskInvalid = null;
                    
                    Button bt_Ok = null;
                    
                    StorageUtils sUtil = new StorageUtils(PvrBackgroundService.this);
                    
                    int hdd_Error_Type = sUtil.checkHDDSpace();
                    
                    switch (hdd_Error_Type)
                    {
                        case 1:
                            // not enough spaces
                            Log.e(TAG, "not enough spaces");
                            storage_state_error = getString(R.string.hdd_notEnough);
                            storage_state_error =
                                storage_state_error + "\n\n" + getString(R.string.pvr_hdd_notEnough_notice);
                            break;
                        
                        case 2:
                            // no devices
                            Log.e(TAG, "no devices");
                            storage_state_error = getString(R.string.hdd_no);
                            storage_state_error = storage_state_error + "\n\n" + getString(R.string.pvr_hdd_no_notice);
                            break;
                    }
                    
                    tv_showTaskInvalid = (TextView)hdd_Space_Error_Dialog.findViewById(R.id.tv_showTaskInvalid);
                    tv_showTaskInvalid.setText(storage_state_error);
                    bt_Ok = (Button)hdd_Space_Error_Dialog.findViewById(R.id.bt_Ok);
                    bt_Ok.setTag(hdd_Space_Error_Dialog);
                    
                    bt_Ok.setOnClickListener(new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            View tagView = (View)v.getTag();
                            PvrDialogManager.getInstance().removeView(PvrBackgroundService.this, tagView);
                        }
                    });
                    
                    PvrDialogManager.getInstance().adjustView(PvrBackgroundService.this,
                        hdd_Space_Error_Dialog,
                        R.id.tv_showTaskInvalid,
                        R.id.ll_buttons);
                    
                    PvrDialogManager.getInstance().addView(PvrBackgroundService.this, hdd_Space_Error_Dialog);
                    
                    break;
            }
        }
        
        private void sendTaskCompleteBroadcast()
        {
            
            Calendar calendar = Calendar.getInstance();
            
            long currentTime = calendar.getTimeInMillis();
            
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);
            
            calendar.add(Calendar.MILLISECOND, (int)(pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()));
            
            long taskEndTime = calendar.getTimeInMillis();
            
            boolean skip_Task = false;
            
            if (taskEndTime > currentTime)
            {
                
                // �û��ֶ�ֹͣ��¼������, ����������û�п��ǵ����������µĴ������ֹ����
                
                Log.e(TAG, "The endTime > currentTime... End time = " + new Date(taskEndTime).toString()
                    + " currentTime = " + new Date(currentTime).toString());
                
                skip_Task = true;
                
            }
            
            Log.e(TAG, "The pvr_ID in RecordBackground = " + (pvrBean == null ? -1 : pvrBean.getPvrId()));
            Log.e(TAG, "The skip flag in pvrBackgroundService is " + skip_Task);
            
            Intent broadIntent = new Intent(Config.PVR_TASK_COMPLETE);
            
            Bundle pvrMessage = new Bundle();
            
            pvrMessage.putInt(DBPvr.PVR_ID, pvrBean == null ? -1 : pvrBean.getPvrId().intValue());
            
            pvrMessage.putBoolean(DBPvr.IF_SKIP_TASK, skip_Task);
            
            if (taskCompleteReceiver == null)
            {
                
                taskCompleteReceiver = new OnPvrTaskCompleteReceiver();
                
            }
            
            sendOrderedBroadcast(broadIntent, null, taskCompleteReceiver, null, 0, null, pvrMessage);
            
        }
        
    };
    
    private class ButtonOnFocusChangeListener implements OnFocusChangeListener
    {
        
        Resources resources = getBaseContext().getResources();
        
        ColorStateList black = resources.getColorStateList(R.drawable.black);
        
        ColorStateList white = resources.getColorStateList(R.drawable.white);
        
        @Override
        public void onFocusChange(View v, boolean hasFocus)
        {
            
            Button button = (Button)v;
            
            if (hasFocus)
            {
                
                v.setBackgroundResource(R.drawable.button_search_light);
                
                button.setTextColor(white);
                
            }
            else
            {
                
                v.setBackgroundResource(R.drawable.button_search);
                
                button.setTextColor(black);
                
            }
            
        }
    }
    
    @Override
    public IBinder onBind(Intent intent)
    {
        
        Log.e(TAG, "---->>>> PvrBackgroundService::onBind() run!!! <<<<----");
        
        IBinder mBinder = new PvrRecordBinder();
        
        return mBinder;
        
    }
    
    private class PvrRecordBinder extends PvrRecordServiceAIDL.Stub
    {
        
        @Override
        public void startRecord(int pvrID)
        {
            
            Log.e(TAG, "startRecord in binder");
            
            Message msg = handler.obtainMessage();
            
            // msg.arg1 = pvrID;
            
            msg.what = START_RECORD;
            
            handler.sendMessage(msg);
            
        }
        
        @Override
        public void stopRecord()
            throws RemoteException
        {
            
            Log.e(TAG, "stopRecord() in binder... Now is " + new Date().toString());
            
            Message msg = handler.obtainMessage();
            
            msg.what = RECORD_FINISHED;
            
            msg.arg1 = DBPvr.PVR_STOP_TYPE_MANUALLY;
            
            handler.sendMessage(msg);
        }
        
        @Override
        public int getCurrentTunerFreq()
            throws RemoteException
        {
            int tunerFreq = -1;
            
            if (null != pvrBean && null != pvrBean.getServiceBean())
            {
                int tpID = pvrBean.getServiceBean().getTpId();
                
                TpInfoDao tpDao = new TpInfoDaoImpl(PvrBackgroundService.this);
                
                TPInfoBean tpBean = tpDao.getTpInfo(tpID);
                
                tunerFreq = tpBean.getTunerFreq();
            }
            else
            {
                if (null == pvrBean)
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean");
                }
                else if (null == pvrBean.getServiceBean())
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean.getServiceBean()");
                }
            }
            
            return tunerFreq;
        }
        
        @Override
        public int getCurrentTunerSymbol()
            throws RemoteException
        {
            int tunerSymbol = -1;
            
            if (null != pvrBean && null != pvrBean.getServiceBean())
            {
                int tpID = pvrBean.getServiceBean().getTpId();
                
                TpInfoDao tpDao = new TpInfoDaoImpl(PvrBackgroundService.this);
                
                TPInfoBean tpBean = tpDao.getTpInfo(tpID);
                
                tunerSymbol = tpBean.getTunerSymbRate();
            }
            else
            {
                if (null == pvrBean)
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean");
                }
                else if (null == pvrBean.getServiceBean())
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean.getServiceBean()");
                }
            }
            
            return tunerSymbol;
        }
        
        @Override
        public int getCurrentTunerQAM()
            throws RemoteException
        {
            int tunerQAM = -1;
            
            if (null != pvrBean && null != pvrBean.getServiceBean())
            {
                int tpID = pvrBean.getServiceBean().getTpId();
                
                TpInfoDao tpDao = new TpInfoDaoImpl(PvrBackgroundService.this);
                
                TPInfoBean tpBean = tpDao.getTpInfo(tpID);
                
                tunerQAM = tpBean.getTunerEmod();
            }
            else
            {
                if (null == pvrBean)
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean");
                }
                else if (null == pvrBean.getServiceBean())
                {
                    LogUtils.e(TAG, "PvrRecordBinder.getCurrentTunerFreq():: null == pvrBean.getServiceBean()");
                }
            }
            
            return tunerQAM;
        }
        
        @Override
        public int[] getTunerInfo()
            throws RemoteException
        {
            int[] tunerInfo = null;
            
            if (null != pvrBean && null != pvrBean.getServiceBean())
            {
                tunerInfo = new int[3];
                
                int tpID = pvrBean.getServiceBean().getTpId();
                
                TpInfoDao tpDao = new TpInfoDaoImpl(PvrBackgroundService.this);
                
                TPInfoBean tpBean = tpDao.getTpInfo(tpID);
                
                tunerInfo[0] = tpBean.getTunerFreq();
                tunerInfo[1] = tpBean.getTunerSymbRate();
                tunerInfo[2] = tpBean.getTunerEmod();
            }
            
            return tunerInfo;
        }
    }
    
    @Override
    public void onCreate()
    {
        
        super.onCreate();
        
    }
    
    public void startRecord(int pvrID)
    {
        
        pvrBean = (RecordPvrBean)PvrRunningTask.getInstance();
        
        if (pvrBean == null)
        {
            
            // ��ȡ����ʧ��, ���¼��
            LogUtils.e(TAG, "-------->>>>There is not any task need to be started here...<<<<--------");
            
            // TODO: Check!!! If here need to set the alarm task to
            // AlarmManager???
            return;
            
        }
        
        serviceBean = pvrBean.getServiceBean();
        
        if (serviceBean == null)
        {
            
            Log.e(TAG, "Can not find the ServiceInfoBean..");
            Log.e(TAG, "ServiceID = " + (pvrBean.getServiceId() == null ? "null" : pvrBean.getServiceId())
                + " logicalNumber = " + (pvrBean.getLogicalNumber() == null ? "null" : pvrBean.getLogicalNumber()));
            
            // tUtil.setNextTask(this, new TaskCompleteCallBack(this));
            
            pvrBean.gotoNextTask(PvrBackgroundService.this);
            
            pvrBean = null;
            
            return;
        }
        
        pvrBean.setHdd_Error_Call_Back(new PvrTimerTaskCallBack()
        {
            
            @Override
            public void pvrTaskCallBack()
            {
                
                handler.sendEmptyMessage(HDD_SPACE_ERROR);
                
            }
        });
        
        pvrBean.setRecord_Finish_Call_Back(new PvrTimerTaskCallBack()
        {
            
            @Override
            public void pvrTaskCallBack()
            {
                
                Message msg = handler.obtainMessage();
                
                msg.arg1 = DBPvr.PVR_STOP_TYPE_AUTOMATICALLY;
                
                msg.what = RECORD_FINISHED;
                
                handler.sendMessage(msg);
                
            }
        });
        
        pvrBean.setTask_Start_Call_Back(new PvrTimerTaskCallBack()
        {
            @Override
            public void pvrTaskCallBack()
            {
                LogUtils.e(TAG, "RecordPvrBean.pvrTaskCallBack:: task_Start_Call_Back run...");
                
                Message msg = handler.obtainMessage();
                
                msg.what = SEND_TASK_START_BROADCAST;
                
                handler.sendMessage(msg);
            }
        });
        
        pvrBean.startTask(PvrBackgroundService.this);
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        
        Log.e(TAG, "PvrBackgroundService onStartCommand");
        
        return super.onStartCommand(intent, flags, startId);
        
    }
    
    @Override
    public void onDestroy()
    {
        
        super.onDestroy();
        
        if (pvrBean != null)
        {
            
            pvrBean.stopTask(DBPvr.PVR_STOP_TYPE_ERROR);
            
        }
        
        Log.e(TAG, "PvrBackgroundService onDestroy()");
    }
    
}