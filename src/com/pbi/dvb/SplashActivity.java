/*
 * File Name:  SplashActivity.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import java.io.File;
import java.util.List;

import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnKeyListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings.SettingNotFoundException;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;

import com.pbi.dvb.ChannelSearchProgress;
import com.pbi.dvb.EitActivity;
import com.pbi.dvb.EpgActivity;
import com.pbi.dvb.OTADownloaderActivity;
import com.pbi.dvb.PvrOrderList;
import com.pbi.dvb.PvrPlayList;
import com.pbi.dvb.R;
import com.pbi.dvb.SearchFreqSettings;
import com.pbi.dvb.SystemInfoActivity;
import com.pbi.dvb.TVChannelEdit;
import com.pbi.dvb.TVChannelPlay;
import com.pbi.dvb.TVChannelSearch;
import com.pbi.dvb.ca.CaAuthInfo;
import com.pbi.dvb.ca.CaEmail;
import com.pbi.dvb.ca.CaPasswordSet;
import com.pbi.dvb.ca.CaRateControl;
import com.pbi.dvb.ca.CaStateInfo;
import com.pbi.dvb.ca.PlayControlMain;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.domain.MountInfoBean;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.dvbinterface.NativeDownload;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Rec_Status_t;
import com.pbi.dvb.dvbinterface.NativeSystemInfo;
import com.pbi.dvb.factorytest.FactoryTest;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.service.LoadDriveService;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.StorageUtils;
import com.pbi.dvb.view.CustomProgressDialog;
import com.pbi.dvb.view.CustomToast;

public class SplashActivity extends Activity
{
    private static final String TAG = "SplashActivity";
    
    private TextView tvTips;
    
    private CustomProgressDialog progressDialog;
    
    private Dialog searchDialog;
    
    private List<ServiceInfoBean> tvList;
    
    private List<ServiceInfoBean> radioList;
    
    private int servicePos;
    
    private int serviceId;
    
    private int logicalNumber;
    
    private String serviceName;
    
    private char serviceType;
    
    private String superPwd;
    
    private int grade;
    
    private String entrance;
    
    private boolean recStatus = false;
    
    private Bundle recBundle;
    
    private NativeDrive nativeDrive;
    private int bootDefaultStatus;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash);
        
        LogUtils.e(TAG, "SplashActivity::onCreate() run...");
    }
    
    protected void onStart()
    {
        super.onStart();
        initWidget();
        startProgressDialog();
        new Thread(new LoadDriveTask(handler)).start();
        
        LogUtils.e(TAG, "SplashActivity::onStart() run...");
    }
    
    private void initWidget()
    {
        tvTips = (TextView)this.findViewById(R.id.tv_sp_tips);
    }
    
    protected void onStop()
    {
        super.onStop();
        
        stopProgressDialog();
        clearData();
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch (keyCode)
        {
            case KeyEvent.KEYCODE_BACK:
                if (null != searchDialog)
                {
                    searchDialog.dismiss();
                }
                finish();
                break;
        }
        return false;
    }
    
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.CONNECTION_SUCCESS:
                    stopProgressDialog();
                    
                    if(bootDefaultStatus != 0)
                    {
                        LogUtils.printLog(1, 3, TAG, "--- start to play default channel " + bootDefaultStatus);
                        go2BootDefaultChannel(bootDefaultStatus);
                        return;
                    }
                    
                    // process rec status.
                    LogUtils.printLog(1, 3, TAG, "---Rec Status----->>>>" + recStatus);
                    LogUtils.printLog(1, 3, TAG, "---entrance---------->>>>" + entrance);
                    if (recStatus)
                    {
                        if (null != recBundle)
                        {
                            if (entrance.equals(Config.DVB_TV_PROGRAMME) || entrance.equals(Config.DVB_RADIO_PROGRAMME)
                                || entrance.equals(Config.DVB_CHANNEL_SEARCH)
                                || entrance.equals(Config.DVB_CA_USER_VIEW) || entrance.equals(Config.DVB_CHANNEL_EDIT)
                                || entrance.equals(Config.DVB_EPG))
                            {
                                new CustomToast(SplashActivity.this).show(getString(R.string.pvr_recording), 0);
                                CommonUtils.skipActivity(SplashActivity.this,
                                    TVChannelPlay.class,
                                    recBundle,
                                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
                            }
                            else
                            {
                                parserEntrance();
                            }
                        }
                        else
                        {
                            handler.sendEmptyMessage(Config.NO_PROGRAMME);
                        }
                    }
                    else
                    {
                        LogUtils.printLog(1, 3, TAG, "-------Not Start Rec ----entrance--->>>" + entrance);
                        if (null != entrance)
                        {
                            parserEntrance();
                        }
                    }
                    break;
                case Config.NO_PROGRAMME:
                    tvTips.setVisibility(View.VISIBLE);
                    break;
            }
        }
    };
    
    /**
     * 
     * <Parser the entrance from launcher.>
     * 
     * @see [类、类#方法、类#成员]
     */
    private void parserEntrance()
    {
        /* Start LinKang, 2013-12-24, Jump to SystemInfoActivity page... */
        if (null != entrance && entrance.equalsIgnoreCase(Config.DVB_SYSTEM_INFO))
        {
            CommonUtils.skipActivity(SplashActivity.this, SystemInfoActivity.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        /* End LinKang, 2013-12-24, Jump to SystemInfoActivity page... */
        
        // check if the ota is downloading now...
        // For OTADownloaderService may be force to quit...
        // So here user NativeDownload to get the ota status...
        NativeDownload nativeDownload = new NativeDownload();
        
        int otaStatus = nativeDownload.downloadGetStatus();
        
        if (otaStatus == 1)
        {
            LogUtils.e(TAG, "The OTA is in downloading status for nativeDownload.downloadGetStatus() == 1");
            CommonUtils.skipActivity(SplashActivity.this, OTADownloaderActivity.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
            return;
        }
        else
        {
            LogUtils.e(TAG, "The OTA is not in downloading status for nativeDownload.downloadGetStatus() != 1");
        }
        // Factory test
        if (entrance.equals(Config.DVB_FACTORY_TEST))
        {
            CommonUtils.skipActivity(SplashActivity.this, FactoryTest.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Television Programme
        if (entrance.equals(Config.DVB_TV_PROGRAMME))
        {
            if (tvList.size() != 0)
            {
                Bundle tvBundle = packageServiceInfo();
                CommonUtils.skipActivity(SplashActivity.this,
                    TVChannelPlay.class,
                    tvBundle,
                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
            }
            else
            {
                if (null == searchDialog)
                {
                    searchDialog =
                        DialogUtils.twoButtonsDialogCreate(SplashActivity.this,
                            getString(R.string.no_program_auto_search),
                            new OnClickListener()
                            {
                                
                                @Override
                                public void onClick(View v)
                                {
                                    if (null != searchDialog)
                                    {
                                        searchDialog.dismiss();
                                    }
                                    Bundle bundle = new Bundle();
                                    bundle.putInt("SearchMode", Config.Install_Search_Type_eAUTO_SEARCH);
                                    CommonUtils.skipActivity(SplashActivity.this,
                                        ChannelSearchProgress.class,
                                        bundle,
                                        Intent.FLAG_ACTIVITY_CLEAR_TOP);
                                }
                            }, new OnClickListener()
                            {
                                
                                @Override
                                public void onClick(View v)
                                {
                                    if (null != searchDialog)
                                    {
                                        searchDialog.dismiss();
                                    }
                                }
                            });
                }
            }
        }
        
        // Radio programme
        else if (entrance.equals(Config.DVB_RADIO_PROGRAMME))
        {
            if (radioList.size() != 0)
            {
                Bundle radioInfos = packageServiceInfo();
                CommonUtils.skipActivity(SplashActivity.this,
                    TVChannelPlay.class,
                    radioInfos,
                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
            }
            else
            {
                handler.sendEmptyMessage(Config.NO_PROGRAMME);
            }
        }
        // Epg
        else if (entrance.equals(Config.DVB_EPG))
        {
            if (tvList.size() != 0 || radioList.size() != 0)
            {
                Bundle epgInfos = packageServiceInfo();
                
                Bundle bundle = getIntent().getExtras();
                
                if (null == bundle)
                {
                    LogUtils.e(TAG, "SplashActivity.onKeyDown:: Epg bundle == null");
                    
                    epgInfos.putChar(Config.DVB_SERVICE_TYPE, Config.SERVICE_TYPE_NOTYPE);
                }
                else
                {
                    char mServiceType = bundle.getChar(Config.DVB_SERVICE_TYPE);
                    
                    if (mServiceType != Config.SERVICE_TYPE_TV && mServiceType != Config.SERVICE_TYPE_RADIO)
                    {
                        mServiceType = Config.SERVICE_TYPE_NOTYPE;
                    }
                    
                    epgInfos.putChar(Config.DVB_SERVICE_TYPE, mServiceType);
                }
                
                CommonUtils.skipActivity(SplashActivity.this,
                    EpgActivity.class,
                    epgInfos,
                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
            }
            else
            {
                handler.sendEmptyMessage(Config.NO_PROGRAMME);
            }
        }
        // EIT
        else if (entrance.equals(Config.DVB_EIT))
        {
            if (tvList.size() != 0 || radioList.size() != 0)
            {
                Bundle epgInfos = packageServiceInfo();
                
                Bundle bundle = getIntent().getExtras();
                
                if (null == bundle)
                {
                    LogUtils.e(TAG, "SplashActivity.onKeyDown:: Eit bundle == null");
                    
                    epgInfos.putChar(Config.DVB_SERVICE_TYPE, Config.SERVICE_TYPE_NOTYPE);
                }
                else
                {
                    char mServiceType = bundle.getChar(Config.DVB_SERVICE_TYPE);
                    
                    if (mServiceType != Config.SERVICE_TYPE_TV && mServiceType != Config.SERVICE_TYPE_RADIO)
                    {
                        mServiceType = Config.SERVICE_TYPE_NOTYPE;
                    }
                    
                    epgInfos.putChar(Config.DVB_SERVICE_TYPE, mServiceType);
                }
                
                CommonUtils.skipActivity(SplashActivity.this,
                    EitActivity.class,
                    epgInfos,
                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
            }
            else
            {
                handler.sendEmptyMessage(Config.NO_PROGRAMME);
            }
        }
        // Search channels
        else if (entrance.equals(Config.DVB_CHANNEL_SEARCH))
        {
            CommonUtils.skipActivity(SplashActivity.this, TVChannelSearch.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // direct auto search
        else if (entrance.equals(Config.DVB_DIRECT_SEARCH))
        {
            Bundle bundle = new Bundle();
            bundle.putInt("SearchMode", Config.Install_Search_Type_eAUTO_SEARCH);
            CommonUtils.skipActivity(SplashActivity.this,
                ChannelSearchProgress.class,
                bundle,
                Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // all search
        else if (entrance.equals(Config.DVB_ALL_SEARCH))
        {
            Bundle bundle = new Bundle();
            bundle.putInt("SearchMode", Config.Install_Search_Type_eALL_SEARCH);
            CommonUtils.skipActivity(SplashActivity.this,
                ChannelSearchProgress.class,
                bundle,
                Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        else if (entrance.equals(Config.DVB_FREQ_SETTING))
        {
            CommonUtils.skipActivity(SplashActivity.this, SearchFreqSettings.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        else if (entrance.equals(Config.DVB_BOOT_DEFAULT_CHANNEL))
        {
            CommonUtils.skipActivity(SplashActivity.this, BootDefaultServiceActivity.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Edit channels
        else if (entrance.equals(Config.DVB_CHANNEL_EDIT))
        {
            if (tvList.size() != 0 || radioList.size() != 0)
            {
                Bundle editInfos = packageServiceInfo();
                CommonUtils.skipActivity(SplashActivity.this,
                    TVChannelEdit.class,
                    editInfos,
                    Intent.FLAG_ACTIVITY_CLEAR_TOP);
            }
            else
            {
                handler.sendEmptyMessage(Config.NO_PROGRAMME);
            }
        }
        // pvr list
        else if (entrance.equals(Config.DVB_PVR_LIST))
        {
            CommonUtils.skipActivity(SplashActivity.this, PvrOrderList.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        else if (entrance.equals(Config.DVB_PVR_PLAY_LIST))
        {
            CommonUtils.skipActivity(SplashActivity.this, PvrPlayList.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca status
        else if (entrance.equals(Config.DVB_CA_STATUS))
        {
            CommonUtils.skipActivity(SplashActivity.this, CaStateInfo.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca password settings
        else if (entrance.equals(Config.DVB_CA_PWD_SETTINGS))
        {
            CommonUtils.skipActivity(SplashActivity.this, CaPasswordSet.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca adult grade
        else if (entrance.equals(Config.DVB_CA_GRADE))
        {
            CommonUtils.skipActivity(SplashActivity.this, CaRateControl.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca auth
        else if (entrance.equals(Config.DVB_CA_AUTH))
        {
            CommonUtils.skipActivity(SplashActivity.this, CaAuthInfo.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca email
        else if (entrance.equals(Config.DVB_CA_EMAIL))
        {
            CommonUtils.skipActivity(SplashActivity.this, CaEmail.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        // Ca zimu card feeding
        else if (entrance.equals(Config.DVB_CA_USER_VIEW))
        {
            CommonUtils.skipActivity(SplashActivity.this, PlayControlMain.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
        
    }
    
    private void startProgressDialog()
    {
        if (null == progressDialog)
        {
            progressDialog = CustomProgressDialog.createDialog(this, R.layout.progress_loading);
        }
        progressDialog.show();
        progressDialog.setOnKeyListener(new OnKeyListener()
        {
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event)
            {
                if (keyCode == KeyEvent.KEYCODE_BACK)
                {
                    return true;
                }
                return false;
            }
        });
    }
    
    private void stopProgressDialog()
    {
        if (null != progressDialog)
        {
            progressDialog.dismiss();
            progressDialog = null;
        }
    }
    
    private void clearData()
    {
        tvTips.setVisibility(View.INVISIBLE);
        superPwd = null;
        entrance = null;
        tvList = null;
        radioList = null;
    }
    
    /**
     * <Package the service informations for the playing page.>
     * 
     * @param playChannelName
     * @return
     * @see [类、类#方法、类#成员]
     */
    private Bundle packageServiceInfo()
    {
        Bundle bundle = new Bundle();
        bundle.putInt("ServicePos", servicePos);
        bundle.putInt("LogicalNumber", logicalNumber);
        bundle.putInt("ServiceId", serviceId);
        bundle.putString("ServiceName", serviceName);
        bundle.putChar("ServiceType", serviceType);
        bundle.putString("superPwd", superPwd);
        bundle.putInt("Grade", grade);
        bundle.putBoolean("RecStatus", recStatus);
        return bundle;
    }
    
    private Bundle parserRecProgram()
    {
        ServiceInfoDao serInfoDao = new ServiceInfoDaoImpl(this);
        SharedPreferences spRec = getSharedPreferences("dvb_rec", MODE_WORLD_READABLE | MODE_WORLD_WRITEABLE);
        
        // parser rec programme.
        int recServiceId = spRec.getInt("rec_serviceId", 0);
        int recNumber = spRec.getInt("rec_logicalNumber", 0);
        int recServicePos = spRec.getInt("rec_servicePosition", 0);
        String recServiceName = spRec.getString("rec_serviceName", null);
        
        ServiceInfoBean rBean = serInfoDao.getChannelInfoByServiceId(recServiceId, recNumber);
        Bundle bundle = new Bundle();
        if (null != rBean)
        {
            bundle.putInt("ServicePos", recServicePos);
            bundle.putInt("LogicalNumber", recNumber);
            bundle.putInt("ServiceId", recServiceId);
            bundle.putString("ServiceName", recServiceName);
            bundle.putChar("ServiceType", serviceType);
            bundle.putString("superPwd", superPwd);
            bundle.putInt("Grade", grade);
            bundle.putBoolean("RecStatus", recStatus);
        }
        return bundle;
    }
    
    /**
     * 
     * <Parser the last play program.>
     * 
     * @return
     * @see [类、类#方法、类#成员]
     */
    private void parserLastProgram()
    {
        ServiceInfoDao serInfoDao = new ServiceInfoDaoImpl(this);
        tvList = serInfoDao.getTVChannelInfo();
        radioList = serInfoDao.getRadioChannelInfo();
        SharedPreferences spChannel = getSharedPreferences(Config.DVB_LASTPLAY_SERVICE, MODE_WORLD_READABLE);
        
        // parser last play programme.
        int radioServiceId = spChannel.getInt("radio_serviceId", 0);
        int radioNumber = spChannel.getInt("radio_logicalNumber", 0);
        int radioServicePos = spChannel.getInt("radio_servicePosition", 0);
        String radioServiceName = spChannel.getString("radio_serviceName", null);
        
        int tvServiceId = spChannel.getInt("tv_serviceId", 0);
        int tvNumber = spChannel.getInt("tv_logicalNumber", 0);
        int tvServicePos = spChannel.getInt("tv_servicePosition", 0);
        String tvServiceName = spChannel.getString("tv_serviceName", null);
        
        ServiceInfoBean rBean = serInfoDao.getChannelInfoByServiceId(radioServiceId, radioNumber);
        ServiceInfoBean tBean = serInfoDao.getChannelInfoByServiceId(tvServiceId, tvNumber);
        
        if (null != entrance)
        {
            if (entrance.equals(Config.DVB_RADIO_PROGRAMME))
            {
                if (radioList.size() != 0)
                {
                    // invalid radio programme.
                    if (null == rBean || null == radioServiceName)
                    {
                        ServiceInfoBean radioBean = radioList.get(0);
                        
                        serviceId = radioBean.getServiceId();
                        servicePos = radioBean.getChannelPosition();
                        logicalNumber = radioBean.getLogicalNumber();
                        serviceName = radioBean.getChannelName();
                        serviceType = radioBean.getServiceType();
                    }
                    else
                    {
                        serviceId = radioServiceId;
                        servicePos = radioServicePos;
                        logicalNumber = radioNumber;
                        serviceName = radioServiceName;
                        serviceType = Config.SERVICE_TYPE_RADIO;
                    }
                }
            }
            if (entrance.equals(Config.DVB_TV_PROGRAMME) || entrance.equals(Config.DVB_EPG)
                || entrance.equals(Config.DVB_CA_USER_VIEW))
            {
                if (tvList.size() != 0)
                {
                    // invalid tv program.
                    if (null == tBean || null == tvServiceName)
                    {
                        ServiceInfoBean tvBean = tvList.get(0);
                        
                        serviceId = tvBean.getServiceId();
                        servicePos = tvBean.getChannelPosition();
                        logicalNumber = tvBean.getLogicalNumber();
                        serviceName = tvBean.getChannelName();
                        serviceType = tvBean.getServiceType();
                    }
                    else
                    {
                        serviceId = tvServiceId;
                        servicePos = tvServicePos;
                        logicalNumber = tvNumber;
                        serviceName = tvServiceName;
                        serviceType = Config.SERVICE_TYPE_TV;
                    }
                }
                else
                {
                    handler.sendEmptyMessage(Config.NO_PROGRAMME);
                }
            }
        }
    }

    private void go2BootDefaultChannel(int serviceId)
    {
        ServiceInfoDao serInfoDao = new ServiceInfoDaoImpl(this);
        List<ServiceInfoBean> nBeans = serInfoDao.getChannelIdByLogicalNumber(serviceId);
        ServiceInfoBean nBean = null;
        if (nBeans.size() != 0)
        {
            nBean = nBeans.get(0);
        }
        if(null == nBean)
        {
            LogUtils.printLog(1, 5, TAG, "--->>> get boot default channel is null !");
            finish();
            return;
        }
        
        Bundle bundle = new Bundle();
        bundle.putInt("ServicePos", nBean.getChannelPosition());
        bundle.putInt("LogicalNumber", serviceId);
        bundle.putInt("ServiceId", serviceId);
        bundle.putString("ServiceName", nBean.getChannelName());
        bundle.putChar("ServiceType", nBean.getServiceType());
        bundle.putString("superPwd", superPwd);
        bundle.putInt("Grade", grade);
        bundle.putBoolean("RecStatus", recStatus);
        CommonUtils.skipActivity(SplashActivity.this,
            TVChannelPlay.class,
            bundle,
            Intent.FLAG_ACTIVITY_CLEAR_TOP);
    }
    
    protected void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        Uri data = intent.getData();
        entrance = data.getSchemeSpecificPart();
        
        // get the super and parent-child's password.
        superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
        if (null == superPwd)
        {
            superPwd = Config.PROGRAMME_LOCK;
        }
        
        bootDefaultStatus = intent.getIntExtra("service_id", 0);
        if(bootDefaultStatus == 0)
        {
            if (null != entrance)
            {
                parserLastProgram();
                parserEntrance();
            }
        }
        // start to play boot default channel.
        else
        {
            LogUtils.printLog(1, 3, TAG, "--- start to play default channel " + bootDefaultStatus);
            go2BootDefaultChannel(bootDefaultStatus);
        }
        
    }
    
    class LoadDriveTask implements Runnable
    {
        private Handler handler;
        
        public LoadDriveTask(Handler handler)
        {
            super();
            this.handler = handler;
        }
        
        public void run()
        {
            // parser the entrance of dvb.
            Uri data = getIntent().getData();
            entrance = data.getSchemeSpecificPart();
            
            bootDefaultStatus = getIntent().getIntExtra("service_id", 0);
            
            synchronized (LoadDriveService.class)
            {
                // load drive and init the CA.
                nativeDrive = new NativeDrive(SplashActivity.this);
                nativeDrive.pvwareDRVInit();
                nativeDrive.CAInit();
                
                NativeSystemInfo SystemInfo = new NativeSystemInfo();
                SystemInfo.SystemInfoInit(SplashActivity.this);
            }
            // load player
            NativePlayer dvbPlayer = NativePlayer.getInstance();
            dvbPlayer.DVBPlayerInit(null);
            dvbPlayer.DVBPlayerSetStopMode(1);
            
            // parser the last play program.
            parserLastProgram();
            
            // process rec logic.
            synchronized (SplashActivity.class)
            {
                PVR_Rec_Status_t recFile = dvbPlayer.new PVR_Rec_Status_t();
                dvbPlayer.DVBPlayerPvrRecGetStatus(recFile);
                int status = recFile.getEnState();
                LogUtils.printLog(1, 3, TAG, "-----Rec File return value----->>>" + status);
                if (status == 2 || status == 3)
                {
                    recStatus = true;
                    recBundle = parserRecProgram();
                }
            }
            
            // delete the time.ts.
            StorageUtils util = new StorageUtils(SplashActivity.this);
            MountInfoBean mountInfoBean = util.getMobileHDDInfo();
            if (null != mountInfoBean)
            {
                // set ts file's name.
                String tmsFilePath = mountInfoBean.getPath() + "/time.ts";
                LogUtils.printLog(1, 3, TAG, "------time file path------->>>" + tmsFilePath);
                File file = new File(tmsFilePath);
                if (file.exists())
                {
                    dvbPlayer.DVBPlayerPvrRemoveFile(tmsFilePath);
                }
            }
            
            // get the super and parent-child's password.
            superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
            if (null == superPwd)
            {
                superPwd = Config.SUPER_PASSWORD;
            }
            
            // get programme grade.
            try
            {
                grade = android.provider.Settings.System.getInt(getContentResolver(), "eit_grade");
            }
            catch (SettingNotFoundException e)
            {
                // get the eit grade exception, set the default value.
                grade = 1;
                e.printStackTrace();
            }
            handler.sendEmptyMessage(Config.CONNECTION_SUCCESS);
        }
    }
    
}
