package com.pbi.dvb.view;

import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrDialogManager;

import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.WindowManager;

public abstract class SelfControlDialog
{
    private static final String TAG = "SelfControlDialog";
    
    protected View mainDialog = null;
    
    protected View ok_Button = null;
    
    protected View cancel_Button = null;
    
    protected TimeControlCallBack timeControlCallBack = null;
    
    protected TimeControlThread timeControlThread;
    
    protected Context context = null;
    
    protected WindowManager.LayoutParams dialogParams;
    
    public void setDialogParams(WindowManager.LayoutParams dialogParams)
    {
        this.dialogParams = dialogParams;
    }
    
    public SelfControlDialog(Context context, int xml_ID)
    {
        this.context = context;
        
        mainDialog = View.inflate(context, xml_ID, null);
        
        if (mainDialog == null)
        {
            LogUtils.e(TAG, "Failed to inflate the SelfControlDialog: xml_ID = " + xml_ID);
        }
    }
    
    public void init_OK_Button(int button_ID, OnClickListener click_Listener,
        OnFocusChangeListener focus_Change_Listener)
    {
        if (mainDialog != null)
        {
            ok_Button = mainDialog.findViewById(button_ID);
            
            ok_Button.setOnClickListener(click_Listener);
            
            ok_Button.setOnFocusChangeListener(focus_Change_Listener);
        }
    }
    
    public void init_Cancel_Button(int button_ID, OnClickListener click_Listener,
        OnFocusChangeListener focus_Change_Listener)
    {
        if (mainDialog != null)
        {
            cancel_Button = mainDialog.findViewById(button_ID);
            
            cancel_Button.setOnClickListener(click_Listener);
            
            cancel_Button.setOnFocusChangeListener(focus_Change_Listener);
        }
    }
    
    public void init_Mid_Button(int button_ID, OnClickListener click_Listener,
        OnFocusChangeListener focus_Change_Listener)
    {
        if (mainDialog != null)
        {
            cancel_Button = mainDialog.findViewById(button_ID);
            
            cancel_Button.setOnClickListener(click_Listener);
            
            cancel_Button.setOnFocusChangeListener(focus_Change_Listener);
        }
    }
    
    public void set_CallBack(TimeControlCallBack timeControlCallBack)
    {
        this.timeControlCallBack = timeControlCallBack;
    }
    
    public abstract void adjustDialogView();
    
    public void show()
    {
        // 开启一个新的线程, 用来处理类似倒计时或者到点了发送提示消息等任务
        // 具體的任務由TimeControlCallBack決定
        cancelTimeControlTask();
        
        if (timeControlCallBack != null)
        {
            timeControlThread = new TimeControlThread(timeControlCallBack);
            
            timeControlThread.start();
        }
        
        // 調整對話框的大小用來適配屏幕, 不需要適配的話, 讓函數體爲空就可以了
        adjustDialogView();
        
        // 将对话框显示到屏幕上
        if (dialogParams == null)
        {
            LogUtils.e(TAG, "dialogParams == null");
            
            PvrDialogManager.getInstance().addView(context, mainDialog);
        }
        else
        {
            LogUtils.e(TAG, "dialogParams != null");
            
            PvrDialogManager.getInstance().addView(context, mainDialog, dialogParams);
        }
    }
    
    public void dismiss()
    {
        // 停止後臺執行的任務
        cancelTimeControlTask();
        
        // 将对话框从系统中移除
        PvrDialogManager.getInstance().removeView(context, mainDialog);
    }
    
    // 用來停止後臺執行的任務, 比如更新對話框中的倒計時時間等...
    private void cancelTimeControlTask()
    {
        if (timeControlCallBack != null)
        {
            if (timeControlThread != null)
            {
                timeControlThread.stopSelf();
                
                timeControlThread = null;
            }
        }
    }
    
    public abstract class TimeControlCallBack
    {
        public abstract void startTask();
        
        public abstract void stopTask();
    }
    
    private class TimeControlThread extends Thread
    {
        private TimeControlCallBack callBack;
        
        public TimeControlThread(TimeControlCallBack callBack)
        {
            this.callBack = callBack;
        }
        
        @Override
        public void run()
        {
            super.run();
            
            callBack.startTask();
        }
        
        public void stopSelf()
        {
            callBack.stopTask();
            
            callBack = null;
            
            this.interrupt();
        }
    }
}
