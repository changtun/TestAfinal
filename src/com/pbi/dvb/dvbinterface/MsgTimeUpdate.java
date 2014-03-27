package com.pbi.dvb.dvbinterface;

import android.content.Context;
import android.content.Intent;

import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class MsgTimeUpdate
{
    private Context context;
    
    public MsgTimeUpdate(Context context)
    {
        super();
        this.context = context;
    }

    public void TimeUpdate()
    {
        sendAlertCurrentTimeBroadCast();
    }
    
    private void sendAlertCurrentTimeBroadCast()
    {
        LogUtils.printLog(1, 3, "NativeDrive", "--->>>send alert current time broadcast!!<<<---");
        Intent intent = new Intent(Config.ACTION_ALERT_CURRENT_TIME);
        context.sendBroadcast(intent);
    }
    
}

