/*
 * File Name:  MessageNit.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.dvbinterface;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.pbi.dvb.global.Config;

public class MessageNit
{
    private static final String TAG = "MessageNit";
    
    private Handler handler;
    
    private Context context;
    
    public MessageNit()
    {
        super();
    }
    
    public MessageNit(Handler handler, Context context)
    {
        super();
        this.handler = handler;
        this.context = context;
    }
    
    public void NitUpdate(int param1)
    {
        SharedPreferences spNit = context.getSharedPreferences("dvb_nit", 8);
        int oriNitVer = spNit.getInt("version_code", 0);
        Log.i(TAG, "---------Origianl Nit Version code is ----------->>>" + oriNitVer);
        Log.i(TAG, "---------New Nit Version code is ----------->>>" + oriNitVer);
        if (null == handler)
        {
            return;
        }
        if (param1 != oriNitVer)
        {
            Message message = new Message();
            message.what = Config.NIT_UPDATE;
            message.obj = param1;
            handler.sendMessage(message);
        }
    }
    
    public void unRegisterHandler()
    {
        this.handler = null;
    }
    
    public native int nitSearchVer();
    
}