package com.pbi.dvb.service;

import java.io.File;
import java.io.IOException;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.RecoverySystem;
import android.os.RemoteException;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.R;
import com.pbi.dvb.aidl.OTADownloaderServiceAIDL;
import com.pbi.dvb.dvbinterface.MessageDownload;
import com.pbi.dvb.dvbinterface.MessageNitDownload;
import com.pbi.dvb.dvbinterface.NativeDownload;
import com.pbi.dvb.dvbinterface.Nativenit;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.pvrinterface.impl.ButtonOnFocusChangeListener;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.utils.AlarmTaskUtil;
import com.pbi.dvb.utils.KeySimulationManager;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.view.SelfControlDialog;

public class OTADownloaderService extends Service
{
    private static final String TAG = "OTADownloaderService";
    
    public static final int OTA_IP_LOCK_MASK = 0x01;
    
    public static final int OTA_PVR_LOCK_MASK = 0x02;
    
    public static final int OTA_RECORD_LOCK_MASK = 0x04;
    
    public static final int OTA_DOWNLOAD_LOCK_MASK = 0x08;
    
    public static final int OTA_RECEIVE_UPDATE_NOTICE = 0;
    
    public static final int OTA_START_DOWNLOAD = 1;
    
    public static final int OTA_DOWNLOAD_FINISHED = 2;
    
    public static final int OTA_UPDATE_DOWNLOAD_PROGRESS = 3;
    
    public static final int OTA_DOWNLOAD_ERROR = 4;
    
    public static final int OTA_UPDATE_MESSAGE_BY_WAKEUP_FROM_CHECK_PVR = 5;
    
    private Nativenit nitNative;
    
    private NativeDownload nativeDownload;
    
    private int ota_Status = 0;
    
    private OTADownloaderBinder ota_Downloader_Binder;
    
    private int requestForOTADownload = 0;
    
    private Handler ota_Handler = new Handler()
    {
        private OTA_Update_Progress_Dialog ota_Update_Progress_Dialog;
        
        private OTA_Check_To_Start_Download_Dialog ota_Check_Start_Dialog;
        
        private OTA_Downloader_Finish_Message_Dialog download_Finish_Notice_Dialog;
        
        private OTA_Update_Error_Dialog ota_Update_Error_Dialog;
        
        public void dispatchMessage(android.os.Message msg)
        {
            super.dispatchMessage(msg);
            
            switch (msg.what)
            {
                case OTA_RECEIVE_UPDATE_NOTICE:
                {
                    LogUtils.e(TAG, "OTA_RECEIVE_UPDATE_NOTICE:: msg.arg1 == " + msg.arg1);
                    
                    if (msg.arg1 == 1)
                    {
                        LogUtils.e(TAG,
                            "!!!!!!receive the message to start download update package from pvr lockMask check thread...");
                        LogUtils.e(TAG,
                            "receive the message to start download update package from pvr lockMask check thread");
                        LogUtils.e(TAG,
                            "receive the message to start download update package from pvr lockMask check thread");
                        LogUtils.e(TAG,
                            "receive the message to start download update package from pvr lockMask check thread");
                        LogUtils.e(TAG,
                            "receive the message to start download update package from pvr lockMask check thread");
                        LogUtils.e(TAG,
                            "!!!!!!receive the message to start download update package from pvr lockMask check thread...");
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "!!!!!!receive the message to start download update package from server from JNI");
                        LogUtils.e(TAG, "receive the message to start download update package from server from JNI");
                        LogUtils.e(TAG, "receive the message to start download update package from server from JNI");
                        LogUtils.e(TAG, "receive the message to start download update package from server from JNI");
                        LogUtils.e(TAG, "receive the message to start download update package from server from JNI");
                        LogUtils.e(TAG,
                            "!!!!!!receive the message to start download update package from server from JNI");
                    }
                    
                    // 尝试给OTA加锁， 如果加鎖成功則可以彈出OTA下載提示對話框， 否則檢查加鎖失敗的原因
                    // 如果是因爲錄製導致的加鎖失敗， 則註冊廣播接收者等待錄製完畢後談框
                    boolean checkOTA_Status = try_Lock_OTA(OTA_DOWNLOAD_LOCK_MASK);
                    
                    LogUtils.e(TAG, "checkOTA_Status111 == " + checkOTA_Status);
                    
                    if (checkOTA_Status)
                    {
                        LogUtils.e(TAG, "lock ota success!!!");
                        
                        // 加鎖成功, 彈出OTA下載確認提示框
                        if (ota_Check_Start_Dialog == null)
                        {
                            ota_Check_Start_Dialog = new OTA_Check_To_Start_Download_Dialog(OTADownloaderService.this);
                            
                            OnClickListener ok_Click_Listener = new OnClickListener()
                            {
                                
                                @Override
                                public void onClick(View v)
                                {
                                    LogUtils.e(TAG, "User press ok button to run ota download...");
                                    
                                    ota_Check_Start_Dialog.dismiss();
                                    
                                    ota_Handler.sendEmptyMessage(OTA_START_DOWNLOAD);
                                }
                            };
                            
                            OnClickListener cancel_Click_Listener = new OnClickListener()
                            {
                                
                                @Override
                                public void onClick(View v)
                                {
                                    ota_Check_Start_Dialog.dismiss();
                                    
                                    // record the skipped ID of the version to SP
                                    
                                    // reset the OTA_DOWNLOAD_LOCK_MASK to free status
                                    unlock_OTA(OTA_DOWNLOAD_LOCK_MASK);
                                    
                                    AlarmTaskUtil alarmUtil = new AlarmTaskUtil();
                                    
                                    Log.e("Alarm", "OnBootupPvrReceiver.onReceive: setNextTask");
                                    
                                    alarmUtil.setNextTask(OTADownloaderService.this, new TaskCompleteCallBack(
                                        OTADownloaderService.this));
                                    
                                }
                            };
                            
                            ota_Check_Start_Dialog.init_OK_Button(R.id.bt_Ok,
                                ok_Click_Listener,
                                new ButtonOnFocusChangeListener(OTADownloaderService.this));
                            
                            ota_Check_Start_Dialog.init_Cancel_Button(R.id.bt_Cancel,
                                cancel_Click_Listener,
                                new ButtonOnFocusChangeListener(OTADownloaderService.this));
                        }
                        
                        ota_Check_Start_Dialog.show();
                        
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "lock ota failed...  start ota lockMask check... But actually we do nothing else here... just show a log...");
                        
                        // If lock failed, we should check all the possible lock way
                        // to check whether the lock has been time out
                        // If the pvr record is started, but the STB has not any sdcard...Then the pvr will wait for the
                        // user to insert a sdcard... This may be continued for a long long time...
                        // So we had better not check the status, because this need to do some other control...
                    }
                    
                    break;
                }
                
                case OTA_START_DOWNLOAD:
                {
                    LogUtils.e(TAG, "Send a simulated home key event to system...");
                    
                    new Thread()
                    {
                        public void run()
                        {
                            KeySimulationManager.sendHomeKey();
                        }
                    }.start();
                    
                    if (nativeDownload != null)
                    {
                        nativeDownload.downloadMessageUnInit();
                        
                        nativeDownload = null;
                    }
                    
                    nativeDownload = new NativeDownload();
                    
                    MessageDownload downloadMsg = new MessageDownload(ota_Handler, OTADownloaderService.this);
                    
                    // If the JNI level do not release the downloadMsg, here may cause memory leak...
                    nativeDownload.downloadMessageInit(downloadMsg);
                    
                    nativeDownload.downloadStart();
                    
                    if (ota_Update_Progress_Dialog == null)
                    {
                        ota_Update_Progress_Dialog = new OTA_Update_Progress_Dialog(OTADownloaderService.this);
                    }
                    
                    LogUtils.e(TAG, "Show the OTA update progress dialog");
                    
                    ota_Update_Progress_Dialog.show();
                    
                    // PvrDialogManager.getInstance().addView(OTADownloaderService.this,
                    // ota_Update_Progress_Dialog,
                    // ota_Update_Progress_Dialog.getDialogParams());
                    
                    break;
                }
                
                case OTA_UPDATE_DOWNLOAD_PROGRESS:
                {
                    // 更新OTA下载进度条的进度显示
                    if (ota_Update_Progress_Dialog != null)
                    {
                        ota_Update_Progress_Dialog.setProgressVaue(msg.arg1);
                    }
                    
                    break;
                }
                
                case OTA_DOWNLOAD_FINISHED:
                {
                    // close the OTA activity
                    Intent finishOTAActivityIntent = new Intent(Config.CLOSE_OTA_ACTIVITY);
                    
                    sendBroadcast(finishOTAActivityIntent);
                    
                    // show a dialog to tell user that the update has finished
                    // and ask the user whether to execute the udpate right now
                    
                    if (download_Finish_Notice_Dialog == null)
                    {
                        download_Finish_Notice_Dialog =
                            new OTA_Downloader_Finish_Message_Dialog(OTADownloaderService.this);
                        
                        OnClickListener updateSystemButtonClick = new OnClickListener()
                        {
                            
                            @Override
                            public void onClick(View v)
                            {
                                ota_Update_Progress_Dialog.dismiss();
                                
                                download_Finish_Notice_Dialog.dismiss();
                                
                                // unregister all the JNI callbacks
                                unregister_All_Jni_Callbacks();
                                
                                unlock_OTA(OTA_DOWNLOAD_LOCK_MASK);
                                
                                // Send the update broadcast for HISI platform...
                                // Intent intent = new Intent("android.intent.action.MASTER_CLEAR");
                                //
                                // intent.putExtra("mount_point", "/mnt/sdcard");
                                //
                                // sendBroadcast(intent);
                                
                                // Send the update broadcast for Amlogic platform...
                                // String updateFilePath = "mnt/sdcard/update.zip";
                                // String updateFilePath = "/data/data/com.pbi.dvb/update.zip";
                                String updateFilePath = "/cache/update.zip";
                                
                                LogUtils.e(TAG, "OTADownloaderService::updateFilePath = " + updateFilePath);
                                
                                final File updateFile = new File(updateFilePath);
                                
                                if (updateFile.exists())
                                {
                                    try
                                    {
                                        // RebootUtils.rebootInstallPackage(SystemUpdateDialog.this,new
                                        // File(localsavePath+"/update.zip"));
                                        // The reboot call is blocking, so we need to do it on another thread.
                                        new Thread("Reboot")
                                        {
                                            
                                            @Override
                                            public void run()
                                            {
                                                try
                                                {
                                                    RecoverySystem.installPackage(OTADownloaderService.this, updateFile);
                                                }
                                                catch (IOException e)
                                                {
                                                    Log.e(TAG, "Can't perform rebootInstallPackage", e);
                                                }
                                            }
                                            
                                        }.start();
                                    }
                                    catch (Exception e)
                                    {
                                        Log.e(TAG, e.getMessage().toString());
                                    }
                                    
                                }
                                else
                                {
                                    LogUtils.e(TAG, "the udpate.zip is not exists...");
                                }
                                
                            }
                        };
                        
                        OnClickListener cancel_Update_Task_Listener = new OnClickListener()
                        {
                            
                            @Override
                            public void onClick(View v)
                            {
                                ota_Update_Progress_Dialog.dismiss();
                                
                                download_Finish_Notice_Dialog.dismiss();
                                
                                // Unregister all the JNI callbacks
                                unregister_All_Jni_Callbacks();
                                
                                unlock_OTA(OTA_DOWNLOAD_LOCK_MASK);
                                
                                AlarmTaskUtil alarmUtil = new AlarmTaskUtil();
                                
                                Log.e("Alarm", "OnBootupPvrReceiver.onReceive: setNextTask");
                                
                                alarmUtil.setNextTask(OTADownloaderService.this, new TaskCompleteCallBack(
                                    OTADownloaderService.this));
                            }
                            
                        };
                        
                        download_Finish_Notice_Dialog.init_OK_Button(R.id.bt_Ok,
                            updateSystemButtonClick,
                            new ButtonOnFocusChangeListener(OTADownloaderService.this));
                        
                        download_Finish_Notice_Dialog.init_Cancel_Button(R.id.bt_Cancel,
                            cancel_Update_Task_Listener,
                            new ButtonOnFocusChangeListener(OTADownloaderService.this));
                    }
                    
                    download_Finish_Notice_Dialog.show();
                    
                    break;
                }
                
                case OTA_DOWNLOAD_ERROR:
                {
                    // close the OTA activity
                    Intent finishOTAActivityIntent = new Intent(Config.CLOSE_OTA_ACTIVITY);
                    
                    sendBroadcast(finishOTAActivityIntent);
                    
                    if (ota_Update_Error_Dialog == null)
                    {
                        ota_Update_Error_Dialog = new OTA_Update_Error_Dialog(OTADownloaderService.this);
                        
                        OnClickListener ota_Error_Button_Click = new OnClickListener()
                        {
                            
                            @Override
                            public void onClick(View v)
                            {
                                LogUtils.e(TAG, "ota_Error_Dialog::bt_Ok::onClick() run...");
                                
                                if (ota_Update_Progress_Dialog != null)
                                {
                                    ota_Update_Progress_Dialog.dismiss();
                                }
                                
                                ota_Update_Error_Dialog.dismiss();
                                
                                unlock_OTA(OTA_DOWNLOAD_LOCK_MASK);
                                
                                AlarmTaskUtil alarmUtil = new AlarmTaskUtil();
                                
                                Log.e("Alarm", "OnBootupPvrReceiver.onReceive: setNextTask");
                                
                                alarmUtil.setNextTask(OTADownloaderService.this, new TaskCompleteCallBack(
                                    OTADownloaderService.this));
                            }
                        };
                        
                        ota_Update_Error_Dialog.init_Mid_Button(R.id.bt_Ok,
                            ota_Error_Button_Click,
                            new ButtonOnFocusChangeListener(OTADownloaderService.this));
                    }
                    
                    ota_Update_Error_Dialog.show();
                    
                    break;
                }
                
            }
        }
        
    };
    
    private class OTA_Check_To_Start_Download_Dialog extends SelfControlDialog
    {
        private TextView tv_showDialogText;
        
        // Use for show a dialog to tell user that there is a new update package can be downloaded...
        public OTA_Check_To_Start_Download_Dialog(Context context)
        {
            super(context, R.layout.checkbox_dialog);
            
            if (mainDialog != null)
            {
                tv_showDialogText = (TextView)mainDialog.findViewById(R.id.tv_showDialogText);
                
                tv_showDialogText.setText(R.string.ota_receive_update_message_notice);
            }
            
            dialogParams = null;
            
            dialogParams = new WindowManager.LayoutParams();
            
            dialogParams.height = WindowManager.LayoutParams.MATCH_PARENT;
            dialogParams.width = WindowManager.LayoutParams.MATCH_PARENT;
            dialogParams.flags = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
            dialogParams.format = PixelFormat.TRANSLUCENT;
            dialogParams.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
            dialogParams.setTitle("OTA_Check_To_Start_Download_Dialog");
        }
        
        @Override
        public void adjustDialogView()
        {
            // Do not need to do anything... For the use of weight in layout
            
            LogUtils.e(TAG, "OTA_Check_To_Start_Download_Dialog::adjustDialogView() do nothing here...");
            
            return;
        }
    }
    
    private class OTA_Update_Error_Dialog extends SelfControlDialog
    {
        // Use for show a ota download error message dialog
        public OTA_Update_Error_Dialog(Context context)
        {
            super(context, R.layout.pvr_check_box_with_one_button);
            
            if (mainDialog != null)
            {
                TextView tv_showTaskInvalid = (TextView)mainDialog.findViewById(R.id.tv_showTaskInvalid);
                
                tv_showTaskInvalid.setText(R.string.ota_error_notice);
            }
        }
        
        @Override
        public void adjustDialogView()
        {
            // Do not need to do anything... For the use of weight in layout
            return;
        }
    }
    
    private class OTA_Update_Progress_Dialog extends SelfControlDialog
    {
        private ProgressBar progressBar = null;
        
        private TextView progressValueShowInTextView;
        
        public WindowManager.LayoutParams getDialogParams()
        {
            return dialogParams;
        }
        
        public OTA_Update_Progress_Dialog(Context context)
        {
            super(context, R.layout.update_progress);
            
            if (mainDialog != null)
            {
                progressBar = (ProgressBar)mainDialog.findViewById(R.id.pb_ShowOTAUpdateProgress);
                
                progressValueShowInTextView = (TextView)mainDialog.findViewById(R.id.tv_ShowUpdateProgress);
                
                progressBar.setMax(100);
                
                progressValueShowInTextView.setText("0%");
            }
            
            dialogParams = null;
            
            dialogParams = new WindowManager.LayoutParams();
            
            dialogParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
            dialogParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
            dialogParams.flags =
                WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                    | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE;
            dialogParams.format = PixelFormat.TRANSLUCENT;
            dialogParams.gravity = Gravity.RIGHT | Gravity.TOP;
            dialogParams.alpha = 0.5f;
            dialogParams.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
            dialogParams.setTitle("Toast11111111111");
        }
        
        @Override
        public void adjustDialogView()
        {
            RelativeLayout rl_progressBar = (RelativeLayout)mainDialog.findViewById(R.id.rl_progressBar);
            
            int fontSize = (int)progressValueShowInTextView.getTextSize();
            
            progressBar.getLayoutParams().height = fontSize + 5;
            
            WindowManager mWM = (WindowManager)getSystemService(Context.WINDOW_SERVICE);
            
            Display disPlay = mWM.getDefaultDisplay();
            
            DisplayMetrics outMetrics = new DisplayMetrics();
            
            disPlay.getMetrics(outMetrics);
            
            int disWidth = outMetrics.widthPixels;
            
            LogUtils.e(TAG, "before adjust disWidth = " + disWidth);
            
            disWidth = disWidth * 1 / 10;
            
            rl_progressBar.getLayoutParams().width = 3 * (fontSize + 5);
        }
        
        public void setProgressVaue(int progress_Value)
        {
            if (progressBar != null)
            {
                String update_Progress_Value = progress_Value + "%";
                
                progressBar.setProgress(progress_Value);
                
                progressValueShowInTextView.setText(update_Progress_Value);
            }
        }
    }
    
    private class OTA_Downloader_Finish_Message_Dialog extends SelfControlDialog
    {
        
        // Use for getthing the desicition from the user to check whether do update right now...
        public OTA_Downloader_Finish_Message_Dialog(Context context)
        {
            super(context, R.layout.checkbox_dialog);
            
            if (mainDialog != null)
            {
                TextView tv_showDialogText = (TextView)mainDialog.findViewById(R.id.tv_showDialogText);
                
                // String ota_download_finished = getResources().getString(R.string.ota_download_finished);
                
                tv_showDialogText.setText(R.string.ota_download_finished);
            }
        }
        
        @Override
        public void adjustDialogView()
        {
            return;
        }
    }
    
    private void unregister_All_Jni_Callbacks()
    {
        if (nitNative != null)
        {
            nitNative.DLMessageUnint();
            
            nitNative = null;
        }
        
        if (nativeDownload != null)
        {
            nativeDownload.downloadMessageUnInit();
            
            nativeDownload = null;
        }
    }
    
    private synchronized boolean try_Lock_OTA(int lockMask)
    {
        if (lockMask != OTA_IP_LOCK_MASK && lockMask != OTA_PVR_LOCK_MASK && lockMask != OTA_RECORD_LOCK_MASK
            && lockMask != OTA_DOWNLOAD_LOCK_MASK)
        {
            // 传递过来的掩码是错误的, 直接返回加锁失败标志
            LogUtils.e(TAG, "The lockMask passed in is error...");
            
            return false;
        }
        
        boolean result = false;
        
        int maskResult = 0;
        
        switch (lockMask)
        {
            case OTA_IP_LOCK_MASK:
            {
                maskResult = (ota_Status & OTA_DOWNLOAD_LOCK_MASK);
                
                break;
            }
            
            case OTA_PVR_LOCK_MASK:
            {
                maskResult = (ota_Status & OTA_DOWNLOAD_LOCK_MASK);// | (ota_Status & OTA_RECORD_LOCK_MASK);
                
                break;
            }
            
            case OTA_RECORD_LOCK_MASK:
            {
                maskResult = (ota_Status & OTA_DOWNLOAD_LOCK_MASK) | (ota_Status & OTA_PVR_LOCK_MASK);
                
                break;
            }
            
            case OTA_DOWNLOAD_LOCK_MASK:
            {
                maskResult =
                    (ota_Status & OTA_DOWNLOAD_LOCK_MASK) | (ota_Status & OTA_IP_LOCK_MASK)
                        | (ota_Status & OTA_PVR_LOCK_MASK) | (ota_Status & OTA_RECORD_LOCK_MASK);
                
                requestForOTADownload++;
                
                break;
            }
            
            default:
            {
                break;
            }
        }
        
        if (maskResult > 0)
        {
            result = false;
        }
        else
        {
            result = true;
            
            ota_Status = ota_Status | lockMask;
        }
        
        return result;
    }
    
    private synchronized boolean unlock_OTA(int lockMask)
    {
        LogUtils.e(TAG, "OTADownloaderService::unlock_OTA() run... lockMask =  " + lockMask + "   ota_Status = "
            + ota_Status);
        
        boolean result = true;
        
        if (lockMask != OTA_IP_LOCK_MASK && lockMask != OTA_PVR_LOCK_MASK && lockMask != OTA_RECORD_LOCK_MASK
            && lockMask != OTA_DOWNLOAD_LOCK_MASK)
        {
            LogUtils.e(TAG, "The lockMask passed by is error... lockMask = " + lockMask);
            
            // 传递过来的掩码是错误的, 直接返回加锁失败标志
            return false;
            
        }
        
        ota_Status = ota_Status & (~lockMask);
        
        // If the ota is locked by PVR or RECORD, then you may need to tell the OTA to start download the update package
        switch (lockMask)
        {
        
            case OTA_PVR_LOCK_MASK:
            {
                LogUtils.e(TAG, "OTA_PVR_LOCK_MASK::ota ask for download " + requestForOTADownload + " times");
                
                if (requestForOTADownload > 0)
                {
                    ota_Handler.sendEmptyMessage(OTA_RECEIVE_UPDATE_NOTICE);
                    
                    requestForOTADownload = 0;
                }
                else
                {
                    LogUtils.e(TAG,
                        "OTA_PVR_LOCK_MASK::requestForOTADownload <= 0, do not show ota update notice dialog");
                }
                
                break;
            }
            
            case OTA_RECORD_LOCK_MASK:
            {
                
                LogUtils.e(TAG, "OTA_RECORD_LOCK_MASK::ota ask for download " + requestForOTADownload + " times");
                
                if (requestForOTADownload > 0)
                {
                    ota_Handler.sendEmptyMessage(OTA_RECEIVE_UPDATE_NOTICE);
                    
                    requestForOTADownload = 0;
                }
                else
                {
                    LogUtils.e(TAG,
                        "OTA_RECORD_LOCK_MASK::requestForOTADownload <= 0, do not show ota update notice dialog");
                }
                
                break;
                
            }
            
            case OTA_DOWNLOAD_LOCK_MASK:
            {
                requestForOTADownload--;
                
                break;
            }
            
        }
        
        result = true;
        
        return result;
    }
    
    private class OTADownloaderBinder extends OTADownloaderServiceAIDL.Stub
    {
        
        @Override
        public boolean try_Lock_OTA_IP()
            throws RemoteException
        {
            LogUtils.e(TAG, "OTADownloaderService::OTADownloaderBinder::try_Lock_OTA_IP() run...");
            
            return OTADownloaderService.this.try_Lock_OTA(OTA_IP_LOCK_MASK);
        }
        
        @Override
        public boolean try_Lock_OTA(int lockMask)
            throws RemoteException
        {
            LogUtils.e(TAG, "OTADownloaderService::OTADownloaderBinder::try_Lock_OTA() run...");
            
            return OTADownloaderService.this.try_Lock_OTA(lockMask);
        }
        
        @Override
        public boolean unlock_OTA_IP()
            throws RemoteException
        {
            LogUtils.e(TAG, "OTADownloaderService::OTADownloaderBinder::unlock_OTA_IP() run...");
            
            return OTADownloaderService.this.unlock_OTA(OTA_IP_LOCK_MASK);
        }
        
        @Override
        public boolean unlock_OTA(int lockMask)
            throws RemoteException
        {
            LogUtils.e(TAG, "OTADownloaderService::OTADownloaderBinder::unlock_OTA() run...");
            
            return OTADownloaderService.this.unlock_OTA(lockMask);
        }
    }
    
    @Override
    public IBinder onBind(Intent intent)
    {
        LogUtils.e(TAG, "OTADownloaderService::onBind() run...");
        
        if (ota_Downloader_Binder == null)
        {
            synchronized (this)
            {
                if (ota_Downloader_Binder == null)
                {
                    ota_Downloader_Binder = new OTADownloaderBinder();
                }
                
            }
        }
        
        return ota_Downloader_Binder;
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // Toast.makeText(OTADownloaderService.this,
        // "---->>>> OTADownloaderCenterService::onStartCommand() run.... <<<<----",
        // Toast.LENGTH_LONG).show();
        
        LogUtils.e(TAG, "AMLOGIC_SX::OTADownloaderCenterService::onStartCommand() run.........111");
        
        if (nitNative == null)
        {
            nitNative = new Nativenit();
        }
        
        MessageNitDownload nitDownloadMsg = new MessageNitDownload(ota_Handler, this);
        
        nitNative.DLMessageInit(nitDownloadMsg);
        
        return super.onStartCommand(intent, flags, startId);
    }
    
    @Override
    public void onDestroy()
    {
        super.onDestroy();
        
        unregister_All_Jni_Callbacks();
        
        unlock_OTA(OTA_DOWNLOAD_LOCK_MASK);
    }
}
