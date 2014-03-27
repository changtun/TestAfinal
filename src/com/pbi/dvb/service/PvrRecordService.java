package com.pbi.dvb.service;

import java.util.Calendar;
import java.util.Date;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.aidl.OTADownloaderServiceAIDL;
import com.pbi.dvb.aidl.PvrRecordServiceAIDL;
import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.domain.PvrRunningTask;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.pvrinterface.impl.ButtonOnFocusChangeListener;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.pvrinterface.impl.TickCountDownUtil;
import com.pbi.dvb.utils.AlarmTaskUtil;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrDialogManager;

public class PvrRecordService extends Service
{
    
    private static final String TAG = "com.pbi.dvb.service.PvrRecordService";
    
    public static final int UPDATE_OK_BUTTON = 0;
    
    public static final int SHOW_TASK_START_DIALOG = 1;
    
    protected static final int START_TASK_WITHOUT_DIALOG = 2;
    
    protected static final int START_TASK = 1;
    
    private Integer pvr_ID;
    
    private Button bt_Ok;
    
    private View checkDialog;
    
    private TickCountDownUtil countDownUtil;
    
    private PvrRunningTask pvrBean;
    
    private Handler handler = new Handler()
    {
        
        @Override
        public void handleMessage(Message msg)
        {
            
            super.handleMessage(msg);
            
            switch (msg.what)
            {
            
                case UPDATE_OK_BUTTON:
                {
                    if (msg.arg1 == 0)
                    {
                        
                        if (countDownUtil != null)
                        {
                            
                            countDownUtil.setRunFlag(false);
                            
                            countDownUtil = null;
                            
                        }
                        
                        if (ota_Status_Service_Conn.getOta_Connection() != null)
                        {
                            try
                            {
                                LogUtils.e(TAG,
                                    "in the cancel button for check if to start PVR recording... unbind the service now...");
                                
                                ota_Status_Service_Conn.getOta_Connection()
                                    .unlock_OTA(OTADownloaderService.OTA_RECORD_LOCK_MASK);
                            }
                            catch (RemoteException e)
                            {
                                e.printStackTrace();
                            }
                        }
                        else
                        {
                            throw new RuntimeException(
                                "The otaServiceBinder should not be null here for you need to unlock the PVR lockMask!!!");
                        }
                        
                        PvrDialogManager.getInstance().removeView(PvrRecordService.this, checkDialog);
                        
                        switch (pvrBean.getPvrWakeMode())
                        {
                        
                            case DBPvr.RECORD_WAKEUP_MODE:
                                
                                startRecord(pvrBean);
                                
                                break;
                            
                            case DBPvr.PLAY_WAKEUP_MODE:
                                
                                startPlay();
                                
                                break;
                        }
                        
                        break;
                        
                    }
                    
                    while (Calendar.getInstance().getTimeInMillis() % 1000 >= 200
                        && Calendar.getInstance().getTimeInMillis() % 1000 <= 800)
                        continue;
                    
                    bt_Ok.setText(msg.arg1 + "");
                    
                    break;
                }
                
                case SHOW_TASK_START_DIALOG:
                {
                    LogUtils.e(TAG,
                        "service.PvrRecordService.onStartCommand:: ---->>>> In SHOW_TASK_START_DIALOG case <<<<----");
                    
                    showTaskStartDialog(msg.arg1);
                    
                    break;
                }
                
                case START_TASK_WITHOUT_DIALOG:
                {
                    LogUtils.e(TAG,
                        "service.PvrRecordService.onStartCommand:: service.PvrRecordService.handler:: START_TASK_WITHOUT_DIALOG run...");
                    
                    switch (pvrBean.getPvrWakeMode())
                    {
                        case DBPvr.RECORD_WAKEUP_MODE:
                        {
                            LogUtils.e(TAG,
                                "service.PvrRecordService.handler.START_TASK_WITHOUT_DIALOG:: DBPvr.RECORD_WAKEUP_MODE...");
                            
                            startRecord(pvrBean);
                            
                            break;
                        }
                        
                        case DBPvr.PLAY_WAKEUP_MODE:
                        {
                            LogUtils.e(TAG,
                                "service.PvrRecordService.handler.START_TASK_WITHOUT_DIALOG:: DBPvr.PLAY_WAKEUP_MODE...");
                            
                            startPlay();
                            
                            break;
                        }
                        
                        default:
                        {
                            LogUtils.e(TAG,
                                "service.PvrRecordService.handler.START_TASK_WITHOUT_DIALOG:: UNKNOWN MODE...");
                        }
                    }
                    
                    break;
                }
            }
        }
        
        private void showTaskStartDialog(int tickCount)
        {
            
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Before add checkDialog");
            
            PvrDialogManager.getInstance().adjustView(PvrRecordService.this,
                checkDialog,
                R.id.tv_showPvrTitle,
                R.id.ll_buttons);
            
            PvrDialogManager.getInstance().addView(PvrRecordService.this, checkDialog);
            
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: After add checkDialog");
            
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: before ok button start to count down...");
            
            if (countDownUtil != null)
            {
                
                countDownUtil.setRunFlag(false);
                
                countDownUtil = null;
            }
            
            countDownUtil = new TickCountDownUtil(handler, tickCount, UPDATE_OK_BUTTON);
            
            countDownUtil.start();
            
            LogUtils.e(TAG,
                "service.PvrRecordService.onStartCommand:: after ok button start to count down...   tickCount = "
                    + tickCount);
            
        }
        
    };
    
    @Override
    public IBinder onBind(Intent intent)
    {
        
        return null;
        
    }
    
    private class OTA_Service_Connection implements ServiceConnection
    {
        
        private OTADownloaderServiceAIDL ota_Connection = null;
        
        public OTADownloaderServiceAIDL getOta_Connection()
        {
            return ota_Connection;
        }
        
        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            if (service == null)
            {
                LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: onServiceConnected() run!!!");
            }
            
            ota_Connection = OTADownloaderServiceAIDL.Stub.asInterface(service);
        }
        
        @Override
        public void onServiceDisconnected(ComponentName name)
        {
        }
    }
    
    private void connect_To_OTA()
    {
        // Check if the ota is in downloading status...
        Intent ota_Service = new Intent();
        
        ota_Service.setAction("com.pbi.dvb.service.ota_downloader_service");
        
        ota_Status_Service_Conn = new OTA_Service_Connection();
        
        boolean bindFlag = bindService(ota_Service, ota_Status_Service_Conn, Context.BIND_AUTO_CREATE);
        
        if (bindFlag)
        {
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service success");
        }
        else
        {
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service failed");
        }
        
        LogUtils.e(TAG,
            "service.PvrRecordService.onStartCommand:: in while loop waiting for calling ota lock function...");
        
    }
    
    private OTA_Service_Connection ota_Status_Service_Conn = null;
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: ---->>>> onStartCommand runs begin <<<<----", true);
        
        connect_To_OTA();
        
        final Intent mIntent = intent;
        
        new Thread()
        {
            public void run()
            {
                
                synchronized (PvrRecordService.class)
                {
                    
                    LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: service onStart run... now is ("
                        + new Date().toString() + ")");
                    
                    long lastTriggerTime = 0;
                    
                    if (mIntent != null)
                    {
                        
                        pvr_ID = mIntent.getIntExtra(DBPvr.PVR_ID, -1);
                        
                        lastTriggerTime = mIntent.getLongExtra(DBPvr.PVR_TRIGGER_TIME, -1);
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: PvrRecordService::onStart() pvr_ID = " + pvr_ID);
                        
                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: set triggerTime is "
                            + new Date(lastTriggerTime).toString());
                        
                    }
                    else
                    {
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: PvrRecordService::onStart() pvr_ID = null for intent == null");
                        
                        AlarmTaskUtil util = new AlarmTaskUtil();
                        
                        util.setNextTask(PvrRecordService.this, new TaskCompleteCallBack(PvrRecordService.this));
                        
                        // ���intentΪ�յĻ�, ���˳�service, ���������һ�ν���������ͬ��pvr_ID�Ļ�, ���ᱨ���ظ�����..
                        // intentʲôʱ���Ϊ����???�������϶���Ϊ��.�������onStart()����������, intentΪ��.
                        // �����������ط�����, �Ͳ�֪����
                        
                        return;
                        
                    }
                    
                    PvrRunningTask checkTask = null;
                    
                    checkTask = PvrRunningTask.getInstance(PvrRecordService.this, pvr_ID);
                    
                    // ���PvrRecordService�쳣������, pvrBean����Ϊnull..
                    // ��ʱ��mBean����һ������ִ�е�����, ʵ��������������Ҫ������������һ���...
                    // Ȼ������pvrBeanΪnull, ����޷��ж������Ƿ���һ������, ����ֿ�����һ���µ�...��ʵ��������¼�Ƶ���һ������
                    
                    // �����, ���PvrRunningTask��Ϊ��������ִ�еĻ�, �᷵��null...
                    // ���PvrRunningTask��Ϊ������ִ��״̬�Ļ�, �ͻ᷵��һ����ִ�е�����...
                    // ֻ���ڵ���startTask()֮��, �Ż������PvrRunningTask��״̬ת��Ϊִ���е�״̬...
                    if (checkTask == null)
                    {
                        
                        // if (mBean == pvrBean) {
                        
                        // if (prePvrID == pvr_ID) {
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: When do the task, the pvrID is the same...");
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: ERROR!!! It means that the task execute twice in the same time!!!");
                        
                        // ������pvr_ID��ͬ��, ���п����´ν����Ļ�����ͬ��pvr_ID, �������������Ϊ-1�Ļ�, �ƺ��ǻ��������
                        // �����1��2��3������һ������������ʧЧ�ģ� ��4�����
                        // ��ʱ��ϵͳ��⵽1��Ҫ����, ������Ϊ��ݿ�仯��, ��ʱ��ᷢ��һ���㲥, ���ڹ㲥��������һ�������ʱ��,
                        // �ֻ��ѯһ�������б�.
                        // ��ʱ��Ҳ��1�Ѿ��ر���, ����2��3��������Ҫ�رյ�״̬...��Ϊ�㲥�����ߺ�DVB����ͬһ�����,
                        // ��ʱ��ͻ����������ͬʱ�ڴ�����µĲ���...������ĳ�ֲ������˳�������.
                        // Ҳ����˵, ��һ��, ����2��ʱ��ᷢ���������²����� ����3��ʱ��Ҳ�ᷢ���������²���
                        // ��Ϊ���л���ĳ��ԭ��, ��ʱ�����Ȼ��Ȱ�setNextTask()ִ�����,
                        // Ҳ����˵��ͬһʱ����������AlarmManager�¼�, ���Ҷ�����ִ��.(��Ϊʱ����ʱ���Ѿ�����, ִ��ʱ���Ǹ�ֵ��!)
                        // Ȼ��������֮��, �ͻῪʼִ����ݿ���¹㲥ʱ���onChange()������
                        // ��Ϊ֮ǰ��5��update����, ��2��3��updateҲ�?���и���update������
                        // �����ʱ����ܻ����߰˴ε�"��ݿ���"
                        // ������ﲻ����һ�µĻ�, Ӧ�þͻᵯ���ܶ���Ի����!!!������Ԥ�ƵĽ��...
                        
                        // ������ﲻ������һ������Ļ�, �᲻����Ϊ��һ�������Ѿ����, ����pvr_ID����, ����ĳ��ԭ��, pvr_ID����,
                        // ���û��޸������pvr_ID���ڵ������ʱ��, ��������ʼִ��...����ʱ����Ϊ�����Ѿ�Ҫ��ʼ��,
                        // ����ȴ���ظ���...���������������ж�, ��Σ�յ�....
                        
                        // ������ﲻ���������, ��ô��һ�������ڽ���ʱ, �û��޸��������б�, ��ʱ��Ի����ֻᵯ��, ����ִ��һ��...
                        
                        // ����˵, ��BUG, ������!!!
                        
                        return;
                        
                    }
                    
                    pvrBean = checkTask;
                    
                    LogUtils.e(TAG,
                        "service.PvrRecordService.onStartCommand:: Before connect to service for locking ota... pvrBean = "
                            + pvrBean);
                    // ������һ��ʱ���ڻ����ڳ�ʼ��״̬�Ļ�, ����Ϊ�����޷���ʼ...
                    // ������Ҫ�������ΪU���쳣���ڵȴ�״̬, ��������ԭ����ɵ�����, �����п�Ԥ��Ŀɻָ�����Ԥ�ϵ�֮��, ����������쳣,
                    // ��Ҫ���¿�������.
                    // pvrBean.checkStartTimeOut(PvrRecordService.this);
                    
                    checkDialog = View.inflate(PvrRecordService.this, R.layout.pvr_check_box_dialog, null);
                    
                    TextView tv_showPvrTitle = (TextView)checkDialog.findViewById(R.id.tv_showPvrTitle);
                    
                    // tv_showPvrTitle.setTextSize(18);
                    
                    Integer service_ID = null;
                    Integer logical_Number = null;
                    
                    if (pvrBean == null)
                    {
                        
                        throw new RuntimeException("Error: Retrieve pvrBean's data from database occured some problem.");
                        
                    }
                    else
                    {
                        
                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: pvrID = " + pvrBean.getPvrId()
                            + " serviceID111 = " + service_ID + " logicalNumber111 = " + logical_Number);
                        
                    }
                    
                    PvrRunningTask.setLastTriggerTime(lastTriggerTime);
                    
                    if (pvrBean.checkTaskExpired())
                    {
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: ---->>>> PvrRecordService:: The task has been expired...<<<<----");
                        
                        if (pvrBean.getPvrMode() == DBPvr.DO_ONCE_MODE)
                        {
                            pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                            
                            pvrBean.update(PvrRecordService.this);
                        }
                        
                        // GOTO the next task...Here should not cancel the task...
                        // For the task maybe is just skipped by time ajudst...It's actually need to get a right new
                        // time..
                        pvrBean.skipCurrentOriginalTask();
                        
                        pvrBean.gotoNextTask(PvrRecordService.this);
                        
                        return;
                        
                    }
                    
                    if (service_ID == null || logical_Number == null)
                    {
                        service_ID = pvrBean.getServiceId();
                        logical_Number = pvrBean.getLogicalNumber();
                        
                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: pvrID = " + pvrBean.getPvrId()
                            + " serviceID222 = " + service_ID + " logicalNumber222 = " + logical_Number);
                    }
                    
                    ServiceInfoDao serviceDao = new ServiceInfoDaoImpl(PvrRecordService.this);
                    
                    ServiceInfoBean serviceBean = serviceDao.getChannelInfoByServiceId(service_ID, logical_Number);
                    
                    if (serviceBean == null)
                    {
                        
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: Can not find the service's information, and the service_ID = "
                                + service_ID + " logical_Number = " + logical_Number);
                        
                        pvrBean.skipCurrentOriginalTask();
                        
                        // ������һ������
                        // If do not skip the task here...Than it will restart soon...
                        // But I believe it's very possible that the serviceBean will be null also...
                        // So if you do not want to skip the task here...You must make that if the task restart a
                        // explict times,
                        // than skip it...
                        pvrBean.gotoNextTask(PvrRecordService.this, pvrBean.getPvrId());
                        
                        return;
                    }
                    
                    pvrBean.setServiceBean(serviceBean);
                    
                    String channelName = serviceBean.getChannelName();
                    
                    String noticeTitle = null;
                    
                    switch (pvrBean.getPvrWakeMode().intValue())
                    {
                    
                        case DBPvr.RECORD_WAKEUP_MODE:
                            
                            noticeTitle = getString(R.string.pvr_string_record_notice);
                            
                            noticeTitle =
                                noticeTitle + " " + channelName + " ?\n\n"
                                    + getString(R.string.pvr_string_close_notice);
                            
                            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: noticeTitle = " + noticeTitle);
                            
                            tv_showPvrTitle.setText(noticeTitle);
                            
                            break;
                        
                        case DBPvr.PLAY_WAKEUP_MODE:
                            
                            noticeTitle = getString(R.string.pvr_string_play_notice);
                            noticeTitle =
                                noticeTitle + " " + channelName + " ?\n" + getString(R.string.pvr_string_close_notice);
                            
                            tv_showPvrTitle.setText(noticeTitle);
                            
                            break;
                        
                        default:
                            
                            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Unexpected wakeUpMode value: "
                                + pvrBean.getPvrWakeMode().intValue());
                            
                            break;
                    
                    }
                    
                    bt_Ok = (Button)checkDialog.findViewById(R.id.bt_Ok);
                    Button bt_Cancel = (Button)checkDialog.findViewById(R.id.bt_Cancel);
                    
                    bt_Ok.setTag(checkDialog);
                    bt_Cancel.setTag(checkDialog);
                    
                    bt_Ok.setOnFocusChangeListener(new ButtonOnFocusChangeListener(PvrRecordService.this));
                    bt_Cancel.setOnFocusChangeListener(new ButtonOnFocusChangeListener(PvrRecordService.this));
                    
                    bt_Ok.setOnClickListener(new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            
                            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Ok button click");
                            
                            View view = (View)v.getTag();
                            
                            if (countDownUtil != null)
                            {
                                
                                countDownUtil.setRunFlag(false);
                                
                                countDownUtil = null;
                                
                            }
                            
                            PvrDialogManager.getInstance().removeView(PvrRecordService.this, view);
                            
                            if (ota_Status_Service_Conn.getOta_Connection() != null)
                            {
                                try
                                {
                                    LogUtils.e(TAG,
                                        "in the cancel button for check if to start PVR recording... unbind the service now...");
                                    
                                    ota_Status_Service_Conn.getOta_Connection()
                                        .unlock_OTA(OTADownloaderService.OTA_RECORD_LOCK_MASK);
                                }
                                catch (RemoteException e)
                                {
                                    e.printStackTrace();
                                }
                            }
                            else
                            {
                                throw new RuntimeException(
                                    "The otaServiceBinder should not be null here for you need to unlock the PVR lockMask!!!");
                            }
                            
                            switch (pvrBean.getPvrWakeMode())
                            {
                            
                                case DBPvr.PLAY_WAKEUP_MODE:
                                    
                                    // pvrBean.startTask(PvrRecordService.this);
                                    startPlay();
                                    
                                    break;
                                
                                case DBPvr.RECORD_WAKEUP_MODE:
                                    
                                    startRecord(pvrBean);
                                    
                                    break;
                            
                            }
                        }
                    });
                    
                    bt_Cancel.setOnClickListener(new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            
                            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Cancel button click");
                            
                            View clickView = null;
                            
                            switch (v.getId())
                            {
                            
                                case R.id.bt_Cancel:
                                    
                                    clickView = (View)v.getTag();
                                    
                                    break;
                                
                                default:
                                    
                                    LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Illegal onClick id");
                                    
                                    // throw new RuntimeException(
                                    // "Illegal button click...This may be a system error");
                                    
                                    break;
                            
                            }
                            
                            if (countDownUtil != null)
                            {
                                
                                countDownUtil.setRunFlag(false);
                                
                                countDownUtil = null;
                                
                            }
                            
                            PvrDialogManager.getInstance().removeView(PvrRecordService.this, clickView);
                            
                            if (pvrBean == null)
                            {
                                
                                LogUtils.e(TAG,
                                    "service.PvrRecordService.onStartCommand:: The pvrBean in cancel function is null");
                                
                            }
                            else
                            {
                                
                                if (pvrBean.getPvrMode() == null)
                                {
                                    
                                    LogUtils.e(TAG,
                                        "The pvrBean in cancel function is not null...But the pvrMode is null");
                                    
                                }
                                else
                                {
                                    
                                    LogUtils.e(TAG,
                                        "service.PvrRecordService.onStartCommand:: Nothing happened in the night...Everything is ok");
                                    
                                }
                                
                            }
                            
                            if (pvrBean.getPvrMode() == DBPvr.DO_ONCE_MODE)
                            {
                                
                                pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                                
                                pvrBean.update(PvrRecordService.this);
                                
                            }
                            
                            if (ota_Status_Service_Conn.getOta_Connection() != null)
                            {
                                try
                                {
                                    LogUtils.e(TAG,
                                        "in the cancel button for check if to start PVR recording... unbind the service now...");
                                    
                                    ota_Status_Service_Conn.getOta_Connection()
                                        .unlock_OTA(OTADownloaderService.OTA_PVR_LOCK_MASK);
                                    
                                    unbindService(ota_Status_Service_Conn);
                                    
                                    ota_Status_Service_Conn = null;
                                }
                                catch (RemoteException e)
                                {
                                    e.printStackTrace();
                                }
                            }
                            else
                            {
                                throw new RuntimeException(
                                    "The otaServiceBinder should not be null here for you need to unlock the PVR lockMask!!!");
                            }
                            
                            pvrBean.cancelTask();
                            
                            pvrBean.gotoNextTask(PvrRecordService.this, pvrBean.getPvrId());
                            
                        }
                    });
                    
                    /*
                     * Calculate the time length between current time and task start time. If the task has expired
                     * today, the function will try to get the next start time of this task.
                     */
                    long triggerDelayTime = pvrBean.get_Adjust_StartTime();
                    
                    /* the time length to count down for the task start notice dialog */
                    int tickCount = 0;
                    
                    LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: ---->>>> triggerDelayTime = "
                        + triggerDelayTime + " delay = " + DBPvr.TASK_START_DELAY + " <<<<----", true);
                    
                    if (pvrBean.getPvr_type_tag() != DBPvr.TASK_EPG_START_WITHOUT_DIALOG)
                    {
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: (pvrBean.getPvr_type_tag() != DBPvr.TASK_EPG_START_WITHOUT_DIALOG)");
                        
                        if (triggerDelayTime > DBPvr.TASK_START_DELAY)
                        {
                            try
                            {
                                LogUtils.e(TAG,
                                    "service.PvrRecordService.onStartCommand:: ---->>>> total sleep time = "
                                        + (triggerDelayTime - DBPvr.TASK_START_DELAY) / 1000 + " <<<<----");
                                
                                LogUtils.e(TAG,
                                    "service.PvrRecordService.onStartCommand:: ---->>>> sleep begin <<<<----",
                                    true);
                                
                                long sleepTime = triggerDelayTime - DBPvr.TASK_START_DELAY;
                                
                                if ((triggerDelayTime - DBPvr.TASK_START_DELAY) % 1000 > 500)
                                {
                                    sleepTime += 500;
                                }
                                
                                for (int i = 0; i < sleepTime; i += 1000)
                                {
                                    Thread.sleep(1000);
                                    
                                    // Check task cancel..
                                    final PvrDao pvrDao = new PvrDaoImpl(PvrRecordService.this);
                                    
                                    PvrBean statusBean = pvrDao.get_Record_Task_By_ID(pvrBean.getPvrId());
                                    
                                    if (null == statusBean || statusBean.getPvrMode() == DBPvr.CLOSE_MODE)
                                    {
                                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: statusBean ["
                                            + (statusBean == null ? " == null" : " != null") + "]" + " pvrId = "
                                            + (statusBean == null ? "-1" : statusBean.getPvrId()) + " pvrMode = "
                                            + (statusBean == null ? "NoTask" : "CLOSE_MODE"));
                                        
                                        pvrBean.skipCurrentOriginalTask();
                                        
                                        pvrBean.gotoNextTask(PvrRecordService.this, pvrBean.getPvrId());
                                        
                                        return;
                                    }
                                }
                                
                                LogUtils.e(TAG,
                                    "service.PvrRecordService.onStartCommand:: ---->>>> sleep end <<<<----",
                                    true);
                                
                            }
                            catch (InterruptedException e)
                            {
                                
                                e.printStackTrace();
                                
                            }
                            
                            tickCount = (DBPvr.TASK_START_DELAY - DBPvr.DVB_INIT_DELAY) / 1000;
                            
                        }
                        else if (triggerDelayTime >= DBPvr.DVB_INIT_DELAY && triggerDelayTime <= DBPvr.TASK_START_DELAY)
                        {
                            
                            tickCount = (int)((triggerDelayTime - DBPvr.DVB_INIT_DELAY) / 1000);
                            
                            if ((triggerDelayTime - DBPvr.DVB_INIT_DELAY) % 1000 > 0)
                            {
                                tickCount += 1;
                            }
                            
                        }
                        // else if (triggerDelayTime > 0)
                        // {
                        //
                        // tickCount = 0;
                        //
                        // }
                        else
                        {
                            LogUtils.e(TAG,
                                "service.PvrRecordService.onStartCommand:: ---->>>> The triggerDelayTime is smaller than zero... triggerDelayTime = "
                                    + triggerDelayTime + " <<<<----",
                                true);
                            
                            tickCount = (DBPvr.TASK_START_DELAY - DBPvr.DVB_INIT_DELAY) / 1000;
                        }
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: (pvrBean.getPvr_type_tag() == DBPvr.TASK_EPG_START_WITHOUT_DIALOG)");
                    }
                    
                    int ota_Binder_Count = 0;
                    
                    while (ota_Status_Service_Conn.getOta_Connection() == null && ota_Binder_Count <= 30)
                    {
                        ota_Binder_Count++;
                        
                        try
                        {
                            Thread.sleep(1000);
                        }
                        catch (InterruptedException e)
                        {
                            e.printStackTrace();
                        }
                    }
                    
                    boolean lock_OTA_Result = false;
                    
                    if (ota_Status_Service_Conn.getOta_Connection() != null)
                    {
                        try
                        {
                            lock_OTA_Result =
                                ota_Status_Service_Conn.getOta_Connection()
                                    .try_Lock_OTA(OTADownloaderService.OTA_PVR_LOCK_MASK);
                        }
                        catch (RemoteException e)
                        {
                            e.printStackTrace();
                        }
                    }
                    
                    if (!lock_OTA_Result)
                    {
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: the lock_OTA_Result == false, so skip current pvr task...");
                        
                        pvrBean.skipCurrentOriginalTask();
                        
                        pvrBean.gotoNextTask(PvrRecordService.this, pvrBean.getPvrId());
                        
                        return;
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "the lock_OTA_Result == true, that means we lock ota successfully, so continue current pvr task...");
                    }
                    
                    // Check task cancel..
                    final PvrDao pvrDao = new PvrDaoImpl(PvrRecordService.this);
                    
                    PvrBean statusBean = pvrDao.get_Record_Task_By_ID(pvrBean.getPvrId());
                    
                    if (null == statusBean || statusBean.getPvrMode() == DBPvr.CLOSE_MODE)
                    {
                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: statusBean ["
                            + (statusBean == null ? " == null" : " != null") + "]" + " pvrId = "
                            + (statusBean == null ? "-1" : statusBean.getPvrId()) + " pvrMode = "
                            + (statusBean == null ? "NoTask" : "CLOSE_MODE"));
                        
                        pvrBean.skipCurrentOriginalTask();
                        
                        pvrBean.gotoNextTask(PvrRecordService.this, pvrBean.getPvrId());
                        
                        return;
                    }
                    
                    /*
                     * TASK_EPG_START_WITHOUT_DIALOG means it's a real time task... Do not need to be restart any
                     * more...
                     */
                    if (null != pvrBean && null != pvrBean.getPvr_type_tag()
                        && DBPvr.TASK_EPG_START_WITHOUT_DIALOG == pvrBean.getPvr_type_tag())
                    {
                        pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                        
                        pvrDao.update_Record_Task(pvrBean);
                    }
                    
                    /* TASK_EPG_START_WITHOUT_DIALOG means start the task right now... Do not need to show the dialog... */
                    if (pvrBean.getPvr_type_tag() == DBPvr.TASK_EPG_START_WITHOUT_DIALOG)
                    {
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: pvrBean.getPvr_type_tag() == DBPvr.TASK_EPG_START_WITHOUT_DIALOG");
                        
                        Message msg = handler.obtainMessage();
                        
                        msg.what = START_TASK_WITHOUT_DIALOG;
                        
                        handler.sendMessage(msg);
                    }
                    else
                    {
                        /* Start task normally... Show the dialog and begin to count down... */
                        LogUtils.e(TAG,
                            "service.PvrRecordService.onStartCommand:: pvrBean.getPvr_type_tag() != DBPvr.TASK_EPG_START_WITHOUT_DIALOG["
                                + pvrBean.getPvr_type_tag() + "]");
                        
                        Message msg = handler.obtainMessage();
                        
                        msg.what = SHOW_TASK_START_DIALOG;
                        
                        msg.arg1 = tickCount;
                        
                        handler.sendMessage(msg);
                    }
                    
                    return;
                }
            };
        }.start();
        
        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: ---->>>> onStartCommand runs end <<<<----");
        
        return super.onStartCommand(intent, flags, startId);
    }
    
    protected void startPlay()
    {
        
        if (ota_Status_Service_Conn.getOta_Connection() != null)
        {
            try
            {
                LogUtils.e(TAG,
                    "service.PvrRecordService.onStartCommand:: in the cancel button for check if to start PVR recording... unbind the service now...");
                
                ota_Status_Service_Conn.getOta_Connection().unlock_OTA(OTADownloaderService.OTA_PVR_LOCK_MASK);
            }
            catch (RemoteException e)
            {
                e.printStackTrace();
            }
        }
        else
        {
            throw new RuntimeException(
                "The otaServiceBinder should not be null here for you need to unlock the PVR lockMask!!!");
        }
        
        pvrBean.startTask(PvrRecordService.this);
        
        // pvrBean.stopTask(DBPvr.PVR_STOP_TYPE_AUTOMATICALLY);
        
    }
    
    public void startRecord(final PvrBean pvrBean)
    {
        if (null == pvrBean)
        {
            LogUtils.e(TAG,
                "service.PvrRecordService.onStartCommand:: service.PvrRecordService.startRecord:: null == pvrBean... Do not star the record task...");
            
            return;
        }
        
        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: -------->>>> Start Record <<<<--------");
        
        Intent service = new Intent();
        
        service.setAction("com.pbi.dvb.service.pvrBackgroudService");
        
        ServiceConnection conn = new ServiceConnection()
        {
            
            private boolean runFlag = false;
            
            private PvrRecordServiceAIDL pvrRecordBinder = null;
            
            @Override
            public void onServiceConnected(ComponentName name, IBinder service)
            {
                
                LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: onServiceConnected() run!!!");
                
                if (service == null)
                {
                    
                    LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: onServiceConnected:: service == null");
                    
                }
                
                pvrRecordBinder = PvrRecordServiceAIDL.Stub.asInterface(service);
                
                if (!runFlag)
                {
                    
                    if (pvrRecordBinder != null)
                    {
                        
                        try
                        {
                            
                            pvrRecordBinder.startRecord(pvrBean.getPvrId());
                            
                        }
                        catch (RemoteException e)
                        {
                            
                            e.printStackTrace();
                            
                        }
                        
                    }
                    else
                    {
                        
                        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: pvrRecordBinder is null");
                        
                    }
                    
                    runFlag = true;
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
            
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service success");
            
        }
        else
        {
            
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service failed");
            
        }
        
        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: -------->>>>start record end<<<<--------");
    }
    
    @Override
    public void onDestroy()
    {
        
        super.onDestroy();
        
        if (countDownUtil != null)
        {
            
            countDownUtil.setRunFlag(false);
            
            countDownUtil = null;
            
        }
        
        if (pvrBean != null)
        {
            
            pvrBean.skipCurrentOriginalTask();
            
            pvrBean.gotoNextTask(PvrRecordService.this);
            
        }
        
        // this may cause some unexpected bug... for PvrBackgroundService is running normally but here crashed...
        // Then the ota will recard that the PvrBackgroundService is not recording now...
        // Then it interrupted the pvr and start ota downing...
        // So just not unlock here...
        
        // if (otaServiceBinder != null)
        // {
        // try
        // {
        // otaServiceBinder.unlock_OTA(OTADownloaderService.OTA_PVR_LOCK_MASK);
        // }
        // catch (RemoteException e)
        // {
        // e.printStackTrace();
        // }
        //
        // unbindService(ota_Status_Service_Conn);
        //
        // ota_Status_Service_Conn = null;
        //
        // otaServiceBinder = null;
        // }
        
        LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: PvrRecordService onDestroy()");
    }
    
}
