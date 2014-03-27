/*
 * File Name:  ChannelSearchProgress.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ContentProviderOperation;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.pbi.dvb.adapter.SearchProgressAdapter;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.db.DBService;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.MessageInstallation;
import com.pbi.dvb.dvbinterface.MessageNit;
import com.pbi.dvb.dvbinterface.NativeInstallation;
import com.pbi.dvb.dvbinterface.NativeInstallation.Install_Cfg_t;
import com.pbi.dvb.dvbinterface.NativeInstallation.Install_Serv_t;
import com.pbi.dvb.dvbinterface.NativeInstallation.Install_Tp_t;
import com.pbi.dvb.dvbinterface.Nativenit;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.GetSignalInfoTask;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.ServiceSort;
import com.pbi.dvb.view.CustomProgressDialog;

public class ChannelSearchProgress extends Activity
{
    private static final String TAG = "ChannelSearchProgress";
    private Dialog cDialog;
    private TextView tvAllPrograms;
    private TextView tvTVprograms;
    private TextView tvRadioPrograms;
    private TextView tvSearchProgress;
    private TextView tvInputFrequency;
    private TextView tvSymbolRate;
    private TextView tvQam;
    private TextView tvStrength;
    private TextView tvQuality;
    private ListView lvTVPrograms;
    private ListView lvRadioPrograms;
    private ProgressBar pbSearchProgress;
    private ProgressBar pbProgressQuality;
    private ProgressBar pbProgressStrength;
    private String strInputFrequency;
    private String strSymbolRate;
    private String strQam;
    private int searchMode;
    private boolean hasBat;
    
    private MessageInstallation msgInstall;
    private NativeInstallation dvb;
    private NativeInstallation.Install_Search_Result_t searchResult;
    private ScheduledExecutorService newScheduledThreadPool;
    private CustomProgressDialog pdSaveProgram;
    
    private List<String> tvNames;
    private List<String> radioNames;
    
    private int allCount = 0;
    private int tvCount = 0;
    private int radioCount = 0;
    
    private int u32Freq;
    private int u32SymbRate;
    
    private SearchProgressAdapter tvAdapter;
    private SearchProgressAdapter radioAdapter;
    
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.SIGNAL_INTERRUPT:
                    CommonUtils.skipActivity(ChannelSearchProgress.this,
                        AlertDialog.class,
                        "dialogMessage",
                        getString(R.string.NoSignal),
                        -1);
                    break;
                case Config.UPDATE_STRENGTH_QUALITY:
                    Bundle bundle = (Bundle)msg.obj;
                    int qValue = bundle.getInt("quality");
                    int sValue = bundle.getInt("strength");
                    
                    pbProgressQuality.setProgress(qValue);
                    pbProgressQuality.invalidate();
                    
                    pbProgressStrength.setProgress(sValue);
                    pbProgressStrength.invalidate();
                    
                    tvQuality.setText(qValue + "%");
                    tvStrength.setText(sValue + "%");
                    break;
                case Config.UPDATE_SEARCH_PROGRESS:
                    Bundle mBundle = (Bundle)msg.obj;
                    int progress = mBundle.getInt("progress");
                    int freq = mBundle.getInt("frequency");
                    pbSearchProgress.setProgress(progress);
                    pbSearchProgress.invalidate();
                    tvSearchProgress.setText(progress + "%");
                    pbSearchProgress.invalidate();
                    tvInputFrequency.setText(freq + "");
                    break;
                case Config.AUTO_SEARCH_COMPLETE:
                    Log.i(TAG, "---------------------AUTO-SEARCH-SUCCESS!!------------------");
                    new SaveProgramTask().execute();
                    break;
                case Config.AUTO_SEARCH_FAILED:
                    Log.i(TAG, "---------------------AUTO-SEARCH-FAILED!!------------------");
                    processDialog();
                    break;
                case Config.SIGLE_TP_SEARCH_COMPLETE:
                    Log.i(TAG, "---------------------SINGLE-TP-SEARCH-SUCCESS!!------------------");
                    Bundle beValues = (Bundle)msg.obj;
                    int freqValue = beValues.getInt("frequency");
                    int symbValue = beValues.getInt("symbRate");
                    int modValue = beValues.getInt("mod");
                    
                    switch (modValue)
                    {
                        case 0:
                            tvQam.setText("16QAM");
                            break;
                        case 1:
                            tvQam.setText("32QAM");
                            break;
                        case 2:
                            tvQam.setText("64QAM");
                            break;
                        case 3:
                            tvQam.setText("128QAM");
                            break;
                        case 4:
                            tvQam.setText("256QAM");
                            break;
                    }
                    
                    String nameValue = beValues.getString("name");
                    char serviceType = beValues.getChar("serviceType");
                    
                    allCount++;
                    
                    // manual search,no type service looks as tv type.
                    if (searchMode == Config.Install_Search_Type_eTP_SEARCH)
                    {
                        if (serviceType == Config.SERVICE_TYPE_TV || serviceType == Config.SERVICE_TYPE_NOTYPE)
                        {
                            tvNames.add(nameValue);
                            tvCount++;
                        }
                    }
                    
                    // auto search
                    if (searchMode == Config.Install_Search_Type_eAUTO_SEARCH || searchMode == Config.Install_Search_Type_eALL_SEARCH)
                    {
                        if (serviceType == Config.SERVICE_TYPE_TV)
                        {
                            tvNames.add(nameValue);
                            tvCount++;
                        }
                    }
                    
                    if (serviceType == Config.SERVICE_TYPE_RADIO)
                    {
                        radioNames.add(nameValue);
                        radioCount++;
                    }
                    
                    tvInputFrequency.setText(freqValue + "");
                    tvSymbolRate.setText(symbValue + "");
                    
                    tvAllPrograms.setText(allCount + "");
                    tvTVprograms.setText(tvCount + "");
                    tvRadioPrograms.setText(radioCount + "");
                    
                    if (tvNames.size() > 9)
                    {
                        tvNames.remove(0);
                    }
                    
                    if (radioNames.size() > 9)
                    {
                        radioNames.remove(0);
                    }
                    
                    tvAdapter.setProgramNames(tvNames);
                    tvAdapter.notifyDataSetChanged();
                    lvTVPrograms.setAdapter(tvAdapter);
                    
                    radioAdapter.setProgramNames(radioNames);
                    radioAdapter.notifyDataSetChanged();
                    lvRadioPrograms.setAdapter(radioAdapter);
                    
                    break;
                case Config.SIGLE_TP_SEARCH_FAILED:
                    Log.i(TAG, "---------------------SINGLE-TP-SEARCH-FAILED!!------------------");
                    break;
            }
        }
    };
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.channel_search_progress);
    }
    
    protected void onStart()
    {
        super.onStart();
        initWidget();
        startSearch();
    }
    
    public void startSearch()
    {
        new Thread(new SearchTask()).start();
        
        // 开一个线程，每隔1s去获取最新的数据
        Thread td = new Thread(new GetSignalInfoTask(handler));
        newScheduledThreadPool = Executors.newScheduledThreadPool(1);
        newScheduledThreadPool.scheduleWithFixedDelay(td, 0, 1, TimeUnit.SECONDS);
    }
    
    private void initWidget()
    {
        dvb = new NativeInstallation();
        searchResult = dvb.new Install_Search_Result_t();
        
        tvSearchProgress = (TextView)this.findViewById(R.id.tv_search_progress);
        tvInputFrequency = (TextView)this.findViewById(R.id.tv_input_frequency_show);
        tvSymbolRate = (TextView)this.findViewById(R.id.tv_symbol_rate_show);
        tvQam = (TextView)this.findViewById(R.id.tv_qam_show);
        tvStrength = (TextView)this.findViewById(R.id.tv_strength);
        tvQuality = (TextView)this.findViewById(R.id.tv_quality);
        tvAllPrograms = (TextView)this.findViewById(R.id.tv_all_programs);
        tvTVprograms = (TextView)this.findViewById(R.id.tv_tv_programs);
        tvRadioPrograms = (TextView)this.findViewById(R.id.tv_radio_programs);
        
        Bundle bundle = getIntent().getExtras();
        if (null != bundle)
        {
            searchMode = bundle.getInt("SearchMode");
            switch (searchMode)
            {
            // get frequency params from TVChannelSearch Activity
                case Config.Install_Search_Type_eTP_SEARCH:
                    LogUtils.printLog(1, 3, TAG, "--->>> manual search.  " + searchMode);
                    strInputFrequency = bundle.getString("InputFrequency");
                    strSymbolRate = bundle.getString("SymbolRate");
                    strQam = bundle.getString("QAM");
                    hasBat = bundle.getBoolean("HasBat");
                    break;
                
                // get default frequency from SearchFreqSettings Activity.
                case Config.Install_Search_Type_eAUTO_SEARCH:
                    LogUtils.printLog(1, 3, TAG, "--->>>  auto search. " + searchMode);
                    searchMode = Config.Install_Search_Type_eAUTO_SEARCH;// auto search
                    strInputFrequency = getMainFrequency();
                    strSymbolRate = getString(R.string.SymbolRateDefault);
                    strQam = getString(R.string.ModulationModeDefault);
                    hasBat = false;
                    break;
                case Config.Install_Search_Type_eALL_SEARCH:
                    LogUtils.printLog(1, 3, TAG, "--->>>all search. " + searchMode);
                    searchMode = Config.Install_Search_Type_eALL_SEARCH;// all search
                    strInputFrequency = getMainFrequency();
                    strSymbolRate = getString(R.string.SymbolRateDefault);
                    strQam = getString(R.string.ModulationModeDefault);
                    break;
            }
        }
        
        tvInputFrequency.setText(strInputFrequency);
        tvSymbolRate.setText(strSymbolRate);
        tvQam.setText(strQam);
        
        lvTVPrograms = (ListView)this.findViewById(R.id.lv_tv_programs);
        lvRadioPrograms = (ListView)this.findViewById(R.id.lv_radio_programs);
        
        tvAdapter = new SearchProgressAdapter(this);
        radioAdapter = new SearchProgressAdapter(this);
        
        lvTVPrograms.setDividerHeight(0);
        lvRadioPrograms.setDividerHeight(0);
        lvTVPrograms.setSelector(new ColorDrawable(Color.TRANSPARENT));
        lvRadioPrograms.setSelector(new ColorDrawable(Color.TRANSPARENT));
        
        pbSearchProgress = (ProgressBar)this.findViewById(R.id.pb_searchProgress);
        pbProgressQuality = (ProgressBar)this.findViewById(R.id.pb_progress_quality);
        pbProgressStrength = (ProgressBar)this.findViewById(R.id.pb_progress_strength);
        
        pbSearchProgress.setMax(100);
        pbProgressQuality.setMax(100);
        pbProgressStrength.setMax(100);
        
        // 存储节目
        tvNames = new ArrayList<String>();
        radioNames = new ArrayList<String>();
    }
    
    private String getMainFrequency()
    {
        String mainFrequency = getString(R.string.InputFrequencyDefault);
        SharedPreferences spSettings = getSharedPreferences("search_settings", 8);
        if (null == spSettings)
        {
            return mainFrequency;
        }
        return spSettings.getString("main_frequency", mainFrequency);
    }
    
    private void startProgressDialog()
    {
        if (null == pdSaveProgram)
        {
            pdSaveProgram = CustomProgressDialog.createDialog(this, R.layout.progress_dialog);
            pdSaveProgram.setMessage(getString(R.string.SaveProgram));
        }
        pdSaveProgram.show();
    }
    
    private void stopProgressDialog()
    {
        if (null != pdSaveProgram)
        {
            pdSaveProgram.dismiss();
            pdSaveProgram = null;
        }
    }
    
    protected void onStop()
    {
        super.onStop();
        Log.i(TAG, "---------Search on Stop!!------------");
        stopProgressDialog();
        dvb.installationStop();
        tvNames.clear();
        radioNames.clear();
        dvb.installationDeinit();
        newScheduledThreadPool.shutdown();
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    /**
     * <搜索节目>
     * 
     * @see [类、类#方法、类#成员]
     */
    public void searchChannelInfo()
    {
        dvb = new NativeInstallation();
        dvb.installationInit();
        
        // 注册消息监听
        msgInstall = new MessageInstallation(handler);
        dvb.installMessageInit(msgInstall);
        
        Install_Cfg_t icfg = dvb.new Install_Cfg_t();
        icfg.ptTimeout = dvb.new Install_Timeout_t();
        
        // 指定Tuner的参数类型为C类型
        icfg.seteSignalSource(Config.DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBC);
        if (!hasBat)
        {
            icfg.u8SearchLimitFlag = 1;
        }
        else
        {
            // 0 has bat,1 no bat
            icfg.u8SearchLimitFlag = 0;
        }
        
        // 声明Tuner参数的对象
        NativeInstallation.DVBCore_Cab_Desc_t tunerDesc = dvb.new DVBCore_Cab_Desc_t();
        
        if (null != strInputFrequency && !"".equals(strInputFrequency))
        {
            u32Freq = Integer.parseInt(strInputFrequency);
        }
        if (null != strSymbolRate && !"".equals(strSymbolRate))
        {
            u32SymbRate = Integer.parseInt(strSymbolRate);
        }
        else
        {
            u32Freq = Integer.parseInt(getString(R.string.InputFrequencyDefault));
            u32SymbRate = Integer.parseInt(getString(R.string.SymbolRateDefault));
        }
        int eMod = CommonUtils.getModHandling(strQam);
        
        SharedPreferences spChannel = getSharedPreferences("double_tuner", 8);
        int tunerid = spChannel.getInt("radio_serviceId", 0);
        LogUtils.printLog(1, 3, TAG, "--->>> tuner id is " + tunerid);
        
        tunerDesc.setU8TunerId((char)tunerid);
        tunerDesc.setU32Freq(u32Freq);
        tunerDesc.setU32SymbRate(u32SymbRate);
        tunerDesc.seteMod(eMod);
        
        // manual search
        if (searchMode == Config.Install_Search_Type_eTP_SEARCH)
        {
            Log.i(TAG, "---Search Type--->>>---Single  TP ---------");
            
            icfg.seteSearchType(Config.Install_Search_Type_eTP_SEARCH);
            icfg.setU32MainTpNum(0);
            icfg.setU32NormalTpNum(1);
            
            icfg.puNormalTpDescList = new NativeInstallation.DVBCore_Cab_Desc_t[1];
            icfg.puNormalTpDescList[0] = tunerDesc;
        }
        // auto search
        if (searchMode == Config.Install_Search_Type_eAUTO_SEARCH)
        {
            Log.i(TAG, "---Search Type--->>>---Auto Search ---------");
            icfg.seteSearchType(Config.Install_Search_Type_eAUTO_SEARCH);
            icfg.setU32MainTpNum(1);
            icfg.setU32NormalTpNum(0);
            NativeInstallation.DVBCore_Cab_Desc_t[] puMainTpDescList = new NativeInstallation.DVBCore_Cab_Desc_t[1];
            puMainTpDescList[0] = tunerDesc;
            icfg.setPuMainTpDescList(puMainTpDescList);
        }
        // all search
        if (searchMode == Config.Install_Search_Type_eALL_SEARCH)
        {
            Log.i(TAG, "---Search Type--->>>---All Search ---------");
            icfg.seteSearchType(Config.Install_Search_Type_eALL_SEARCH);
            icfg.setU32MainTpNum(1);
            icfg.setU32NormalTpNum(0);
            NativeInstallation.DVBCore_Cab_Desc_t[] puMainTpDescList = new NativeInstallation.DVBCore_Cab_Desc_t[1];
            puMainTpDescList[0] = tunerDesc;
            icfg.setPuMainTpDescList(puMainTpDescList);
        }
        
        Log.i(TAG, "--------------------DVB Search Params ---------------------------");
        Log.i(TAG, "---------Search Mode is----------------------->>>" + searchMode);
        Log.i(TAG, "---------Input Frequency is----------------------->>>" + tunerDesc.getU32Freq());
        Log.i(TAG, "---------SysbolRate is----------------------->>>" + tunerDesc.getU32SymbRate());
        Log.i(TAG, "---------QAM is----------------------->>>" + tunerDesc.geteMod());
        Log.i(TAG, "---------HasBAT----------------------->>>" + hasBat);
        
        int reCode = dvb.installationStart(icfg);
        Log.i(TAG, "---------------------------------->>>>>>>>>>Re Code ----------->>>>>" + reCode);
    }
    
    /**
     * <save tp information>
     * 
     * @see [类、类#方法、类#成员]
     */
    public void saveTpInfoData()
    {
        List<TPInfoBean> tpInfoInSearchResult = getTpInfoInSearchResult(searchResult);
        
        if (searchMode == Config.Install_Search_Type_eAUTO_SEARCH || searchMode == Config.Install_Search_Type_eALL_SEARCH)
        {
            //clear db.
            TpInfoDao tpInfoDao = new TpInfoDaoImpl(this);
            ServiceInfoDao serviceInfoDao = new ServiceInfoDaoImpl(this);
            serviceInfoDao.clearService();
            tpInfoDao.clearTpData();
        }
        if (null != tpInfoInSearchResult)
        {
            TpInfoDao tpInfoDao = new TpInfoDaoImpl(this);
            
            for (TPInfoBean tpInfoBean : tpInfoInSearchResult)
            {
                int freq = tpInfoBean.getTunerFreq();
                int symbRate = tpInfoBean.getTunerSymbRate();
                
                // check this tp whether exist or not.
                int tpId = tpInfoDao.getTpId(freq, symbRate);
                if (tpId == 0)
                {
                    tpInfoDao.addTpInfo(tpInfoBean);
                }
                else
                {
                    tpInfoDao.updateServiceInfo(tpInfoBean, tpId);
                }
            }
        }
        else
        {
            Message message = new Message();
            message.what = Config.AUTO_SEARCH_FAILED;
            handler.sendMessage(message);
        }
    }
    
    /**
     * <save service information>
     * 
     * @see [类、类#方法、类#成员]
     */
    public void saveServiceInfoData()
    {
        List<ServiceInfoBean> serviceInfoInSearchResult = getServiceInfoInSearchResult(searchResult);
        List<ServiceInfoBean> newServiceList = new ArrayList<ServiceInfoBean>();
        List<ServiceInfoBean> curServiceList = new ArrayList<ServiceInfoBean>();
        // String serviceName;
        
        if (null != serviceInfoInSearchResult)
        {
            TpInfoDao tpInfoDao = new TpInfoDaoImpl(this);
            ServiceInfoDao serviceInfoDao = new ServiceInfoDaoImpl(this);

            if (searchMode == Config.Install_Search_Type_eAUTO_SEARCH)
            {
                //update nit version.
                MessageNit messageNit = new MessageNit();
                Nativenit nativenit = new Nativenit();
                nativenit.NitMessageInit(messageNit);
                Editor edit = getSharedPreferences("dvb_nit", 8).edit();
                int nitVersion = messageNit.nitSearchVer();
                Log.i(TAG, "--->>>the nit version is: " + nitVersion);
                edit.putInt("version_code", nitVersion);
                edit.commit();
                nativenit.NitMessageUnint();
            }
            
            for (ServiceInfoBean serviceInfoBean : serviceInfoInSearchResult)
            {
                int freq = serviceInfoBean.getTpInfoBean().getTunerFreq();
                int symbRate = serviceInfoBean.getTpInfoBean().getTunerSymbRate();
                int tpId = tpInfoDao.getTpId(freq, symbRate);
                
                // tp has existed.
                if (tpId != 0)
                {
                    serviceInfoBean.setTpId(tpId);
                    // get service type.
                    char serviceType = serviceInfoBean.getServiceType();
                    
                    // manual search,no type service looks as tv type.
                    if (searchMode == Config.Install_Search_Type_eTP_SEARCH)
                    {
                        if (serviceType == Config.SERVICE_TYPE_NOTYPE)
                        {
                            // change the unknown service name to tv.
                            serviceType = Config.SERVICE_TYPE_TV;
                            serviceInfoBean.setServiceType(serviceType);
                        }
                    }
                    
                    // if (serviceType == Config.SERVICE_TYPE_TV || serviceType == Config.SERVICE_TYPE_RADIO)
                    // {
                    // serviceName = serviceInfoBean.getChannelName();
                    //
                    // // Treatment of name is empty.
                    // if (null == serviceName || "".equals(serviceName))
                    // {
                    // serviceName = "SDT:" + serviceInfoBean.getServiceId();
                    // serviceInfoBean.setChannelName(serviceName);
                    // Log.i(TAG, "------channel name is null !!!------->>>" + serviceName);
                    // }
                    // }
                    newServiceList.add(serviceInfoBean);
                }
            }
            
            // processing the searched collections.
            if (null != newServiceList || newServiceList.size() != 0)
            {
                List<ServiceInfoBean> oriServiceList = serviceInfoDao.getAllChannelInfo(false);
                if (oriServiceList.size() != 0)
                {
                    for (ServiceInfoBean oriBean : oriServiceList)
                    {
                        int oriServiceId = oriBean.getServiceId();
                        int oriTpId = oriBean.getTpId();
                        
                        // wheather the new search programme exist or not.
                        boolean isExist = false;
                        
                        for (ServiceInfoBean newBean : newServiceList)
                        {
                            // The programme has already existed.
                            if (oriServiceId == newBean.getServiceId() && oriTpId == newBean.getTpId())
                            {
                                isExist = true;
                                
                                // Recovery the original settings.
                                newBean.setFavFlag(oriBean.getFavFlag());
                                newBean.setLockFlag(oriBean.getLockFlag());
                            }
                        }
                        
                        // 数据库中原有的，新搜索时没搜到的节目
                        if (!isExist)
                        {
                            curServiceList.add(oriBean);
                        }
                    }
                }
                // 数据库中没有节目时
                for (ServiceInfoBean serviceInfoBean : newServiceList)
                {
                    if (serviceInfoBean.getLogicalNumber() < 65535)
                    {
                        curServiceList.add(serviceInfoBean);
                    }
                }
                
                // sort the services.
                Collections.sort(curServiceList, new ServiceSort());
                
                // save the services.
                ArrayList<ContentProviderOperation> ops = new ArrayList<ContentProviderOperation>();
                ops.add(serviceInfoDao.clearServiceData());
                
                for (int i = 0; i < curServiceList.size(); i++)
                {
                    ops.add(serviceInfoDao.addServiceInfo(curServiceList.get(i), i + 1));
                }
                try
                {
                    getContentResolver().applyBatch(DBService.SERVICE_AUTHORITY, ops);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                
            }
        }
        else
        {
            Message message = new Message();
            message.what = Config.AUTO_SEARCH_FAILED;
            handler.sendMessage(message);
        }
        
        // clear data.
        serviceInfoInSearchResult = null;
        newServiceList = null;
        curServiceList = null;
        
    }
    
//    public boolean onKeyDown(int keyCode, KeyEvent event)
//    {
//        if (keyCode == KeyEvent.KEYCODE_BACK)
//        {
//            return true;
//        }
//        return false;
//    }
    
    /**
     * 
     * Start a sub-thread to search the dvb program.
     * 
     * @author gtsong
     * @version [版本号, 2012-7-23]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    class SearchTask implements Runnable
    {
        public void run()
        {
            searchChannelInfo();
        }
    }
    
    class SaveProgramTask extends AsyncTask<Void, Void, Void>
    {
        protected void onPreExecute()
        {
            startProgressDialog();
        }
        
        protected Void doInBackground(Void... params)
        {
            dvb.installationGetSearchRet(searchResult);
            saveTpInfoData();
            saveServiceInfoData();
            
            dvb.installationDeinit();
            newScheduledThreadPool.shutdownNow();
            return null;
        }
        
        protected void onPostExecute(Void result)
        {
            finish();
        }
        
    }
    
    public void processDialog()
    {
        if(null == cDialog)
        {
            cDialog = DialogUtils.twoButtonsDialogCreate(ChannelSearchProgress.this, getString(R.string.searchNotice), new OnClickListener()
            {
                
                @Override
                public void onClick(View v)
                {
                    if (null != cDialog)
                    {
                        cDialog.dismiss();
                    }
                    finish();
//                    CommonUtils.skipActivity(ChannelSearchProgress.this, TVChannelSearch.class, -1);
                }
            }, new OnClickListener()
            {
                
                @Override
                public void onClick(View v)
                {
                    if (null != cDialog)
                    {
                        cDialog.dismiss();
                    }
                    finish();
                }
            });
        }
    }
    
    public List<TPInfoBean> getTpInfoInSearchResult(NativeInstallation.Install_Search_Result_t searchResult)
    {
        Install_Tp_t[] tpList = searchResult.getPtTpList();
        List<TPInfoBean> tpInfoBeans = new ArrayList<TPInfoBean>();
        TPInfoBean tpInfoBean = null;
        
        if (tpList.length != 0)
        {
            for (int i = 0; i < tpList.length; i++)
            {
                tpInfoBean = new TPInfoBean();
                
                int signalSource = tpList[i].geteSignalSource();
                int netId = tpList[i].getU16NetId();
                int onId = tpList[i].getU16ON_Id();
                int tsId = tpList[i].getU16TS_Id();
                
                NativeInstallation.DVBCore_Cab_Desc_t tunerDesc =
                    (NativeInstallation.DVBCore_Cab_Desc_t)tpList[i].getuTunerDesc();
                
                char tunerId = tunerDesc.getU8TunerId();
                int fec_Inner = tunerDesc.geteFEC_Inner();
                int fec_Outer = tunerDesc.geteFEC_Outer();
                int mod = tunerDesc.geteMod();
                int freq = tunerDesc.getU32Freq();
                int symbRate = tunerDesc.getU32SymbRate();
                
                tpInfoBean.setTunerType(signalSource);
                tpInfoBean.setTunerId(tunerId);
                tpInfoBean.setNetId(netId);
                tpInfoBean.setOriginalNetId(onId);
                tpInfoBean.setTranStreamId(tsId);
                tpInfoBean.setTunerFreq(freq);
                tpInfoBean.setTunerSymbRate(symbRate);
                tpInfoBean.setTunerEmod(mod);
                tpInfoBean.setEfecInner(fec_Inner);
                tpInfoBean.setEfecOuter(fec_Outer);
                
                tpInfoBeans.add(tpInfoBean);
            }
        }
        return tpInfoBeans;
    }
    
    public List<ServiceInfoBean> getServiceInfoInSearchResult(NativeInstallation.Install_Search_Result_t searchResult)
    {
        ServiceInfoBean serviceInfoBean = null;
        TPInfoBean tpInfoBean = null;
        List<ServiceInfoBean> serviceInfoBeans = new ArrayList<ServiceInfoBean>();
        Install_Serv_t[] servList = searchResult.getPtServList();
        
        if (servList.length != 0)
        {
            for (int i = 0; i < servList.length; i++)
            {
                serviceInfoBean = new ServiceInfoBean();
                
                int logicalNumber = servList[i].getu16LogicalNum();
                String servName = servList[i].getsServName();
                int pid = servList[i].getU16PMTPid();
                int servId = servList[i].getU16ServId();
                int caMode = servList[i].getU8FreeCA_Mode();
                char servType = servList[i].getU8ServType();
                
                Install_Tp_t tpDesc = servList[i].getPtTpDesc();
                
                int signalSource = tpDesc.geteSignalSource();
                int netId = tpDesc.getU16NetId();
                int onId = tpDesc.getU16ON_Id();
                int tsId = tpDesc.getU16TS_Id();
                
                DVBCore_Cab_Desc_t tunerDesc = tpDesc.getuTunerDesc();
                
                char tunerId = tunerDesc.getU8TunerId();
                int freq = tunerDesc.getU32Freq();
                int symbRate = tunerDesc.getU32SymbRate();
                int mod = tunerDesc.geteMod();
                int fec_Inner = tunerDesc.geteFEC_Inner();
                int fec_Outer = tunerDesc.geteFEC_Outer();
                
                tpInfoBean = new TPInfoBean();
                
                tpInfoBean.setTunerType(signalSource);
                tpInfoBean.setTunerId(tunerId);
                tpInfoBean.setNetId(netId);
                tpInfoBean.setOriginalNetId(onId);
                tpInfoBean.setTranStreamId(tsId);
                tpInfoBean.setTunerFreq(freq);
                tpInfoBean.setTunerSymbRate(symbRate);
                tpInfoBean.setTunerEmod(mod);
                tpInfoBean.setEfecInner(fec_Inner);
                tpInfoBean.setEfecOuter(fec_Outer);
                
                serviceInfoBean.setTunerType(signalSource);
                serviceInfoBean.setServiceId(servId);
                
                serviceInfoBean.setServiceType(servType);
                serviceInfoBean.setPmtPid(pid);
                serviceInfoBean.setCaMode(caMode);
                if (null == servName || "".equals(servName))
                {
                    servName = "SDT:" + servId;
                }
                // if (caMode == 0x01)
                // {
                // serviceInfoBean.setChannelName("$ "+servName);
                // }
                // else
                // {
                serviceInfoBean.setChannelName(servName);
                // }
                serviceInfoBean.setLogicalNumber(logicalNumber);
                serviceInfoBean.setTpInfoBean(tpInfoBean);
                
                serviceInfoBeans.add(serviceInfoBean);
            }
        }
        return serviceInfoBeans;
    }
    
}
