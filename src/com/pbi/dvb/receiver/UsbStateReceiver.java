/*
 * 文 件 名:  UsbStateReceiver.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  gtsong
 * 修改时间:  2013-12-13
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.receiver;

import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.utils.LogUtils;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
 
/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author
 * @version [版本号, 2013-12-13]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class UsbStateReceiver extends BroadcastReceiver
{
    public static final int USB_MOUNT = 1;
    public static final int USB_UNMOUNT = 0;
    private static final String TAG = "usbStates";
//    private static UsbStateReceiver usbStateReceiver;
    private Context mContext;
    private Handler mHandler;
    
    public UsbStateReceiver(Context context,Handler handler)
    {
        super();
        this.mContext = context;
        this.mHandler = handler;
    }
    
//    public static UsbStateReceiver getUsbStateReceiver(Context context,Handler handler)
//    {
//        if (null == usbStateReceiver)
//        {
//            synchronized (UsbStateReceiver.class)
//            {
//                if (null == usbStateReceiver)
//                {
//                    usbStateReceiver = new UsbStateReceiver(context,handler);
//                }
//            }
//        }
//        return usbStateReceiver;
//    }
    
    public void registerUsbReceiver()
    {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_MEDIA_MOUNTED);
        filter.addAction(Intent.ACTION_MEDIA_CHECKING);
        filter.addAction(Intent.ACTION_MEDIA_EJECT);
        filter.addAction(Intent.ACTION_MEDIA_REMOVED);
        filter.addDataScheme("file");
        this.mContext.registerReceiver(this, filter);
    }
    
    public void unRegisterUsbReceiver()
    {
        this.mContext.unregisterReceiver(this);
    }
    
    @Override
    public void onReceive(Context context, Intent intent)
    {
        if(null == mHandler)
        {
            LogUtils.printLog(1, 5, TAG, "---usb state has changed,but the handler is null!");
            return;
        }
        String action = intent.getAction();
        LogUtils.printLog(1, 3, TAG, "--->>>"+action);
        if(action.equalsIgnoreCase(Intent.ACTION_MEDIA_MOUNTED) ||action.equalsIgnoreCase(Intent.ACTION_MEDIA_CHECKING))
        {
            mHandler.sendEmptyMessage(USB_MOUNT);
        }
        else
        {
            mHandler.sendEmptyMessage(USB_UNMOUNT);
        }
    }
}
