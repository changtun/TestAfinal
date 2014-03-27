/*
 * 文 件 名:  UsbStateService.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  gtsong
 * 修改时间:  2013-12-16
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.service;

import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.receiver.UsbStateReceiver;
import com.pbi.dvb.utils.LogUtils;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author
 * @version [版本号, 2013-12-16]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class UsbStateService extends Service
{
    private static final String TAG = "usbStates";

    private UsbStateReceiver usbStateReceiver;
    
    private Handler mHandler = new Handler()
    {
        public void handleMessage(android.os.Message msg)
        {
            switch (msg.what)
            {
                /** when remove the usb storage,stop rec function. */
                case UsbStateReceiver.USB_UNMOUNT:
                    NativePlayer dvbPlayer = NativePlayer.getInstance();
                    dvbPlayer.DVBPlayerPvrPlayStop();
                    dvbPlayer.DVBPlayerPvrRecStop();
                    break;
                
                case UsbStateReceiver.USB_MOUNT:
                    break;
            }
        };
    };
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        //register usb state receiver
        usbStateReceiver = new UsbStateReceiver(this, mHandler);
        usbStateReceiver.registerUsbReceiver();
        LogUtils.printLog(1, 3, TAG, "---register the usbstates receiver!");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }
    
    @Override
    public void onDestroy()
    {
        if(null != usbStateReceiver)
        {
            usbStateReceiver.unRegisterUsbReceiver();
        }
        super.onDestroy();
    }
}
