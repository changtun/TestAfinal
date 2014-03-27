package com.pbi.dvb.utils;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.global.Config;

/**
 * 
 * Get the values of signal's strength and quality.
 * 
 * @author gtsong
 * @version [版本号, 2012-7-24]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class GetSignalInfoTask implements Runnable
{
    private Handler handler;
    
    public GetSignalInfoTask(Handler handler)
    {
        this.handler = handler;
    }
    
    public void run()
    {
        Tuner tuner = new Tuner();
        int qualityValue = tuner.getTunerQualityPercent(0);
        int strengthValue = tuner.getTunerStrengthPercent(0);
        
        Bundle bundle = new Bundle();
        bundle.putInt("quality", qualityValue);
        bundle.putInt("strength", strengthValue);
        
        Message message = new Message();
        message.obj = bundle;
        message.what = Config.UPDATE_STRENGTH_QUALITY;
        handler.sendMessage(message);
    }
}