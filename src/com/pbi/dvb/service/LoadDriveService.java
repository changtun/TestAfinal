/*
 * File Name:  LoadDriveService.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.service;

import java.util.Calendar;

import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.util.Log;

import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativeSystemInfo;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.dvbinterface.Tuner.CablePercent;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.pvrinterface.impl.TaskCompleteCallBack;
import com.pbi.dvb.utils.AlarmTaskUtil;
import com.pbi.dvb.utils.LogUtils;

public class LoadDriveService extends Service
{
    protected static final String TAG = "LoadDriveService";
    
    public void onCreate()
    {
        super.onCreate();
        LogUtils.printLog(1, 3, "LoadDriveService", "--->>>LoadDriveService has started!!");
    }
    
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtils.e(TAG, "LoadDriveService::onStartCommand() run...");
        
        new Thread(new Runnable()
        {
            public void run()
            {
                synchronized (LoadDriveService.class)
                {
                    LogUtils.e(TAG, "---->>>> LoadDriverService time test begin <<<<----", true);
                    
                    NativeDrive nativeDrive = new NativeDrive(LoadDriveService.this);
                    nativeDrive.pvwareDRVInit();
                    nativeDrive.CAInit();
                    
                    NativeSystemInfo SystemInfo = new NativeSystemInfo();
                    SystemInfo.SystemInfoInit(LoadDriveService.this);
                    
                    LogUtils.printLog(1, 3, "LoadDriveService", "--->>>LoadDriveService has completed!!");
                }
                
                // lock tuner
                // cancel the lockTuner() function for the DVB will start earlier than current service
                // So if lockTuner() here will change the frequency of DVB, make the signal be interrupted
                // lockTuner();
                // LogUtils.printLog(1, 3, "LoadDriveService", "--->>>LockTuner has completed!!");
                
                LogUtils.e(TAG, "---->>>> LoadDriverService time test end <<<<----", true);
                // Open the OTA downloader service
                Intent ota_Downloader_Service = new Intent(LoadDriveService.this, OTADownloaderService.class);
                ota_Downloader_Service.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                startService(ota_Downloader_Service);
                
                int count = 0;
                while (count < 180)
                {
                    count++;
                    if (count <= 30)
                    {
                        LogUtils.e(TAG, "---->>>> Calendar.getInstance().get(Calendar.YEAR) = "
                            + Calendar.getInstance().get(Calendar.YEAR) + " <<<<----");
                    }
                    if (Calendar.getInstance().get(Calendar.YEAR) > 2000)
                    {
                        LogUtils.e(TAG, "---->>>> start init time delay break... <<<<----");
                        break;
                    }
                    try
                    {
                        Thread.sleep(1000);
                    }
                    catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                }
                
                AlarmTaskUtil alarmUtil = new AlarmTaskUtil();
                
                Log.e("Alarm", "OnBootupPvrReceiver.onReceive: setNextTask");
                
                alarmUtil.setNextTask(LoadDriveService.this, new TaskCompleteCallBack(LoadDriveService.this));
            }
        }).start();
        return super.onStartCommand(intent, flags, startId);
    }
    
    private void lockTuner()
    {
        // lock tuner in last play program, ensure the system time is the current time.
        Tuner tuner = new Tuner();
        CablePercent cable = tuner.new CablePercent();
        DVBCore_Cab_Desc_t dvbCoreCabDesc = tuner.new DVBCore_Cab_Desc_t();
        
        ServiceInfoDao serInfoDao = new ServiceInfoDaoImpl(this);
        TpInfoDao tpInfoDao = new TpInfoDaoImpl(this);
        SharedPreferences spChannel = getSharedPreferences("dvb_channel", 8);
        
        int tvServiceId = spChannel.getInt("tv_serviceId", 0);
        int tvNumber = spChannel.getInt("tv_logicalNumber", 0);
        
        ServiceInfoBean tBean = serInfoDao.getChannelInfoByServiceId(tvServiceId, tvNumber);
        
        // lock the default tp.
        if (null == tBean)
        {
            LogUtils.printLog(1, 3, TAG, "--->>> can not find last play service,lock the default tp.");
            dvbCoreCabDesc.setU32Freq(801000);
            dvbCoreCabDesc.setU32SymbRate(5217);
            dvbCoreCabDesc.seteMod(Config.DVBCore_Modulation_eDVBCORE_MOD_256QAM);
           // dvbCoreCabDesc.seteBandWidth(1);// 6M
            
            dvbCoreCabDesc.seteFEC_Inner(0);
            dvbCoreCabDesc.seteFEC_Outer(0);
            dvbCoreCabDesc.setU8TunerId((char)0);
        }
        else
        {
            Integer tpId = tBean.getTpId();
            TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(tpId);
            if (null == tpInfoBean)
            {
                LogUtils.printLog(1, 3, TAG, "--->>>can not find tp infomation of last play service.");
                return;
            }
            LogUtils.printLog(1, 3, TAG, "--->>>lock tp of last play service.");
            dvbCoreCabDesc.setU32Freq(tpInfoBean.getTunerFreq());
            dvbCoreCabDesc.setU32SymbRate(tpInfoBean.getTunerSymbRate());
            dvbCoreCabDesc.seteMod(tpInfoBean.getTunerEmod());
            //dvbCoreCabDesc.seteBandWidth(tpInfoBean.getBandwidth());
            
            dvbCoreCabDesc.seteFEC_Inner(0);
            dvbCoreCabDesc.seteFEC_Outer(0);
            dvbCoreCabDesc.setU8TunerId((char)0);
        }
        
        tuner.getTunerCablePercent(dvbCoreCabDesc, cable);
        
    }
    
    public void onDestroy()
    {
        super.onDestroy();
    }
    
    public IBinder onBind(Intent intent)
    {
        return null;
    }
    
}
