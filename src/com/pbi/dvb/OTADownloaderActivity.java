package com.pbi.dvb;

import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.TextView;

public class OTADownloaderActivity extends Activity
{
    
    protected static final String TAG = null;
    
    private TextView tv_ShowOTAUpdateNotice;
    
    private OTA_Finish_Receiver finishSelfReceiver;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.ota_downloader);
        
        init();
    }
    
    private void init()
    {
        tv_ShowOTAUpdateNotice = (TextView)findViewById(R.id.tv_ShowOTAUpdateNotice);
        
        tv_ShowOTAUpdateNotice.setText(R.string.ota_downloader_notice);
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            this.finish();
        }
        
        return true;
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
    }
    
    @Override
    protected void onResume()
    {
        IntentFilter finishSelfReceiverFilter = new IntentFilter();
        
        finishSelfReceiverFilter.addAction(Config.CLOSE_OTA_ACTIVITY);
        
        if (finishSelfReceiver == null)
        {
            finishSelfReceiver = new OTA_Finish_Receiver();
            
            try
            {
                unregisterReceiver(finishSelfReceiver);
            }
            catch (IllegalArgumentException e)
            {
            }
            
            registerReceiver(finishSelfReceiver, finishSelfReceiverFilter);
        }
        
        super.onResume();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        
        if (finishSelfReceiver != null)
        {
            try
            {
                unregisterReceiver(finishSelfReceiver);
            }
            catch (IllegalArgumentException e)
            {
            }
            
            finishSelfReceiver = null;
            
        }
    }
    
    private class OTA_Finish_Receiver extends BroadcastReceiver
    {
        
        @Override
        public void onReceive(Context context, Intent intent)
        {
            LogUtils.e(TAG, "OTA_Finish_Receiver::onReceive() run... finish the OTA_Activity");
            
            // Maybe here need to start the SplashActivity
            
            OTADownloaderActivity.this.finish();
        }
    }
    
}