package com.pbi.dvb.pvrinterface.impl;

import android.os.Handler;
import android.os.Message;

import com.pbi.dvb.utils.LogUtils;

public class TickCountDownUtil extends Thread
{
    private static final String TAG = "TickCountDownUtil";
    private int total_Time = 0;
    private Handler handler = null;
    private boolean runFlag = true;
    private int msg_Type;
    
    public TickCountDownUtil(Handler handler, int total_Time, int msg_Type)
    {
        this.handler = handler;
        this.total_Time = total_Time;
        this.msg_Type = msg_Type;
        if (handler == null)
        {
            throw new RuntimeException("In TickCountDownUtil, the handler should not be null");
        }
    }
    
    @Override
    public void run()
    {
        while (runFlag && total_Time >= 0)
        {
            Message message = handler.obtainMessage();
            // PvrRecordService.UPDATE_OK_BUTTON
            message.what = msg_Type;
            message.arg1 = (int)total_Time;
            total_Time = total_Time - 1;
            handler.sendMessage(message);
            LogUtils.e(TAG, "---->>>> TickCountDown:: " + total_Time + "<<<<----");
            
            try
            {
                Thread.sleep(1000);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }
        }
    }
    
    public boolean isRunFlag()
    {
        return runFlag;
    }
    
    public void setRunFlag(boolean runFlag)
    {
        this.runFlag = runFlag;
    }
}
