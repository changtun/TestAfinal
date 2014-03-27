package com.pbi.dvb.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.pbi.dvb.service.PvrGlobalService;
import com.pbi.dvb.service.PvrInitService;
import com.pbi.dvb.utils.LogUtils;

public class OnBootupPvrReceiver extends BroadcastReceiver
{
    
    private static final String TAG = "OnBootupPvrReceiver";
    
    @Override
    public void onReceive(Context context, Intent intent)
    {
        LogUtils.e(TAG, "PVR::OnBootupPvrReceiver::Receive the broadcast");
        LogUtils.printLog(1, 3, "OnBootupPvrReceiver", "--->>>Receive the broadcast!!");
        
        context.startService(new Intent("com.pbi.dvb.service.LoadDriveService"));
        
        final Context mContext = context;
        
        Intent service = new Intent(context, PvrGlobalService.class);
        
        service.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        
        context.startService(service);
        
        new Thread()
        {
            
            public void run()
            {
                
                Intent pvr_Init_Service = new Intent(mContext, PvrInitService.class);
                
                pvr_Init_Service.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                
                mContext.startService(pvr_Init_Service);
                
            };
            
        }.start();
        
    }
}
