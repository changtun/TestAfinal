package com.pbi.dvb;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.http.conn.util.InetAddressUtils;

import android.os.SystemProperties;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.database.Cursor;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.SoundtrackBean;
import com.pbi.dvb.domain.SystemInfoBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_Info_t;
import com.pbi.dvb.dvbinterface.Tuner.CablePercent;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class SystemInfoActivity extends Activity
{
    
    private static final String TAG = "SystemInfoActivity";
    
    protected static final int LOAD_RESOURCE_FINISHED = 0;
    
    protected static final int QUERY_SIGNAL_QUALITY = 1;
    
    private ListView ll_ShowSystemInfo;
    
    private SystemInfoAdapter sysInfoAdapter = new SystemInfoAdapter();
    
    private List<SystemInfoBean> sysInfos = new ArrayList<SystemInfoBean>();
    
    private ProgressBar pb_search_strength;
    
    private ProgressBar pb_search_quality;
    
    private TextView tv_search_signal_strength;
    
    private TextView tv_search_signal_quality;
    
    private HandlerThread mHandlerThread = null;
    
    private SystemInfoHandler sysInfoHandler = null;
    
    private final String SYS_INFO_FREQUENCY = "freq";
    
    private final String SYS_INFO_SYMBOL = "symbol";
    
    private final String SYS_INFO_QAM = "qam";
    
    private final String SYS_INFO_TUNER_ID = "tuner_id";
    
    private Map<String, Integer> signal_Query_Params = new HashMap<String, Integer>();
    
    private Handler mHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            
            switch (msg.what)
            {
                case LOAD_RESOURCE_FINISHED:
                {
                    ll_ShowSystemInfo.setAdapter(sysInfoAdapter);
                    
                    ll_ShowSystemInfo.requestFocus();
                    
                    LogUtils.e(TAG, "SystemInfoActivity.mHandler:: adapter's size = " + sysInfoAdapter.getCount());
                    
                    Message newMsg = sysInfoHandler.obtainMessage();
                    
                    Bundle bundle = new Bundle();
                    
                    bundle.putInt(SYS_INFO_FREQUENCY, signal_Query_Params.get(SYS_INFO_FREQUENCY));
                    bundle.putInt(SYS_INFO_SYMBOL, signal_Query_Params.get(SYS_INFO_SYMBOL));
                    bundle.putInt(SYS_INFO_QAM, signal_Query_Params.get(SYS_INFO_QAM));
                    bundle.putInt(SYS_INFO_TUNER_ID, signal_Query_Params.get(SYS_INFO_TUNER_ID));
                    
                    newMsg.obj = bundle;
                    
                    newMsg.what = QUERY_SIGNAL_QUALITY;
                    
                    LogUtils.e(TAG,
                        "SystemInfoActivity.mHandler.handleMessage:: LOAD_RESOURCE_FINISHED send QUERY_SIGNAL_QUALITY = "
                            + QUERY_SIGNAL_QUALITY);
                    
                    sysInfoHandler.sendMessage(newMsg);
                    
                    break;
                }
                
                case Config.UPDATE_STRENGTH_QUALITY:
                {
                    Bundle bundle = (Bundle)msg.obj;
                    
                    int sValue = bundle.getInt("strength");
                    int qValue = bundle.getInt("quality");
                    
                    pb_search_strength.setProgress(sValue);
                    pb_search_strength.invalidate();
                    
                    pb_search_quality.setProgress(qValue);
                    pb_search_quality.invalidate();
                    
                    tv_search_signal_strength.setText(sValue + "%");
                    tv_search_signal_quality.setText(qValue + "%");
                    
                    Message newMsg = sysInfoHandler.obtainMessage();
                    
                    Bundle mBundle = new Bundle();
                    
                    mBundle.putInt(SYS_INFO_FREQUENCY, signal_Query_Params.get(SYS_INFO_FREQUENCY));
                    mBundle.putInt(SYS_INFO_SYMBOL, signal_Query_Params.get(SYS_INFO_SYMBOL));
                    mBundle.putInt(SYS_INFO_QAM, signal_Query_Params.get(SYS_INFO_QAM));
                    mBundle.putInt(SYS_INFO_TUNER_ID, signal_Query_Params.get(SYS_INFO_TUNER_ID));
                    
                    newMsg.obj = mBundle;
                    
                    newMsg.what = QUERY_SIGNAL_QUALITY;
                    
                    LogUtils.e(TAG,
                        "SystemInfoActivity.mHandler.handleMessage:: UPDATE_STRENGTH_QUALITY send QUERY_SIGNAL_QUALITY = "
                            + QUERY_SIGNAL_QUALITY);
                    
                    sysInfoHandler.sendMessageDelayed(newMsg, 1 * 1000);
                    
                    break;
                }
                
                default:
                {
                    LogUtils.e(TAG, "SystemInfoActivity.mHandler.handleMessage:: msg.what is undealed. msg.what = "
                        + msg.what);
                    
                    break;
                }
            }
        }
    };
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.system_info);
        
        init_View();
        
        init_Resource();
    }
    
    private void init_Resource()
    {
        mHandlerThread = new HandlerThread("sub_thread");
        
        mHandlerThread.start();
        
        sysInfoHandler = new SystemInfoHandler(mHandlerThread.getLooper());
        
        if (sysInfos == null || sysInfos.size() <= 0)
        {
            LogUtils.e(TAG,
                "SystemInfoActivity.init_Resource():: sysInfos != null and sysInfos.size = " + sysInfos.size(),
                true);
            
            new Thread()
            {
                @Override
                public void run()
                {
                    super.run();
                    
                    String str_Unknown = SystemInfoActivity.this.getResources().getString(R.string.sys_info_unknown);
                    
                    if (null == sysInfos)
                    {
                        sysInfos = new ArrayList<SystemInfoBean>();
                    }
                    
                    SystemInfoBean sysInfo = null;
                    
                    Resources res = SystemInfoActivity.this.getResources();
                    
                    /* Software Version */
                    ContentResolver resolover = SystemInfoActivity.this.getContentResolver();
                    
                    Uri uri = Uri.parse("content://com.pbi.pbiprovider/pbi/0");
                    
                    Cursor cursor = resolover.query(uri, null, null, null, null);
                    
                    String versionNumber = null;
                    
                    if (null != cursor && cursor.moveToNext())
                    {
                        versionNumber = cursor.getString(cursor.getColumnIndex("_address"));
                    }
                    
                    if (null == versionNumber)
                    {
                        versionNumber = str_Unknown;
                    }
                    
                    String sys_Info_Name = res.getString(R.string.sys_info_software_version);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    sysInfo.setValue(versionNumber);
                    sysInfos.add(sysInfo);
                    
                    /* MAC address */
                    sys_Info_Name = res.getString(R.string.sys_info_mac_address);
                    String ethaddrMAC = SystemProperties.get("ubootenv.var.ethaddr");
                    // String mac = ethaddrMAC.toLowerCase().replace(":", "");
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    sysInfo.setValue(ethaddrMAC);
                    sysInfos.add(sysInfo);
                    
                    /* IP Address... */
                    sys_Info_Name = res.getString(R.string.sys_info_ip_address);
                    
                    String ipAddr = getIpAddress();
                    
                    if (null == ipAddr)
                    {
                        ipAddr = "127.0.0.1";
                    }
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    sysInfo.setValue(ipAddr);
                    sysInfos.add(sysInfo);
                    
                    /* Start LinKang, 2013-12-20, Get the frequency/symbol/QAM information by the last viewed program... */
                    ServiceInfoBean serviceInfo = paserLastProgram(Config.DVB_LASTPLAY_SERVICE);
                    
                    /* Program name */
                    sys_Info_Name = res.getString(R.string.sys_info_program);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name + serviceInfo.getServiceId() + " : ");
                    sysInfo.setValue(serviceInfo == null ? str_Unknown : serviceInfo.getChannelName());
                    sysInfos.add(sysInfo);
                    
                    /* Voice Channel */
                    sys_Info_Name = res.getString(R.string.sys_info_voice_channel);
                    
                    SoundtrackBean soundtrackBean = new SoundtrackBean(SystemInfoActivity.this);
                    String voice_Channel = soundtrackBean.getCurrentSoundtrack(serviceInfo.getServiceId(), serviceInfo.getLogicalNumber());
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
//                    NativePlayer nPlayer = NativePlayer.getInstance();
//                    int audMode = nPlayer.DVBPlayerAVCtrlGetAudMode();
//                    
//                    
//                    String voice_Channel = null;
//                    
//                    LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: audMode = " + audMode);
//                    
//                    switch (audMode)
//                    {
//                        case 0:
//                        {
//                            voice_Channel = res.getString(R.string.three_dimensional_stereo);
//                            
//                            break;
//                        }
//                        case 1:
//                        {
//                            voice_Channel = res.getString(R.string.left_stereo);
//                            
//                            break;
//                        }
//                        
//                        case 2:
//                        {
//                            voice_Channel = res.getString(R.string.right_stereo);
//                        }
//                        
//                        default:
//                        {
//                            LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: unknown audMode = " + audMode);
//                            
//                            voice_Channel = str_Unknown;
//                        }
//                    }
                    
                    // sysInfo.setValue(audMode + "");
                    sysInfo.setValue(voice_Channel);
                    sysInfos.add(sysInfo);
                    
                    TpInfoDao tpInfoDao = new TpInfoDaoImpl(SystemInfoActivity.this);
                    
                    TPInfoBean tpInfo = null;
                    
                    if (null != serviceInfo)
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: serviceInfo != null...  tpid = "
                            + serviceInfo.getTpId() + " serviceID = " + serviceInfo.getServiceId());
                        
                        tpInfo = tpInfoDao.getTpInfo(serviceInfo.getTpId() == null ? -1 : serviceInfo.getTpId());
                    }
                    else
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: serviceInfo == null...");
                    }
                    
                    if (tpInfo == null)
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: tpInfo == null...");
                    }
                    else
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource:: tpInfo != null...");
                    }
                    
                    if (null != tpInfo)
                    {
                        /* TS ID */
                        sys_Info_Name = res.getString(R.string.sys_info_trans_stream_id);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue((tpInfo.getTranStreamId() == null ? str_Unknown : tpInfo.getTranStreamId())
                            + "");
                        sysInfos.add(sysInfo);
                    }
                    else
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_trans_stream_id);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue(str_Unknown);
                        sysInfos.add(sysInfo);
                    }
                    
                    /* Service ID */
                    sys_Info_Name = res.getString(R.string.sys_info_service_id);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    sysInfo.setValue(serviceInfo == null ? str_Unknown : serviceInfo.getServiceId() + "");
                    sysInfos.add(sysInfo);
                    
                    /* VPID */
                    NativePlayer nPlayer = NativePlayer.getInstance();
                    DVBPlayer_Info_t pInfo = nPlayer.new DVBPlayer_Info_t();
                    nPlayer.DVBPlayerGetInfo(pInfo);
                    
                    if (pInfo == null)
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource():: pInfo == null");
                    }
                    else
                    {
                        LogUtils.e(TAG, "SystemInfoActivity.init_Resource()::  vpid = " + pInfo.getU16AudioPid()
                            + " apid = " + pInfo.getU16VideoPid());
                    }
                    
                    sys_Info_Name = res.getString(R.string.sys_info_video_pid);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    // sysInfo.setValue(serviceInfo.getVideoPid() + "");
                    sysInfo.setValue(pInfo == null ? str_Unknown : pInfo.getU16VideoPid() + "");
                    sysInfos.add(sysInfo);
                    
                    /* APID */
                    sys_Info_Name = res.getString(R.string.sys_info_audio_pid);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    // sysInfo.setValue(serviceInfo.getAudioPid() + "");
                    sysInfo.setValue(pInfo == null ? str_Unknown : pInfo.getU16AudioPid() + "");
                    sysInfos.add(sysInfo);
                    
                    /* PCR */
                    sys_Info_Name = res.getString(R.string.sys_info_pcr);
                    
                    sysInfo = new SystemInfoBean();
                    sysInfo.setName(sys_Info_Name);
                    // sysInfo.setValue(serviceInfo.getPcrPid() + "");
                    sysInfo.setValue(pInfo == null ? str_Unknown : pInfo.getU16PCRPid() + "");
                    sysInfos.add(sysInfo);
                    
                    if (null != tpInfo)
                    {
                        
                        /* Frequence */
                        sys_Info_Name = res.getString(R.string.sys_info_frequency);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue((tpInfo.getTunerFreq() == null ? str_Unknown : tpInfo.getTunerFreq()) + "");
                        sysInfos.add(sysInfo);
                    }
                    else
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_frequency);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue(str_Unknown);
                        sysInfos.add(sysInfo);
                    }
                    
                    /* Symbol */
                    if (null != tpInfo)
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_symbol);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue((tpInfo.getTunerSymbRate() == null ? str_Unknown : tpInfo.getTunerSymbRate())
                            + "");
                        sysInfos.add(sysInfo);
                    }
                    else
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_symbol);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue(str_Unknown);
                        sysInfos.add(sysInfo);
                    }
                    
                    /* QAM */
                    if (null != tpInfo)
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_qam);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        
                        String qam_String = null;
                        
                        switch (tpInfo.getTunerEmod())
                        {
                            case 0:
                            {
                                qam_String = "16 QAM";
                                
                                break;
                            }
                            
                            case 1:
                            {
                                qam_String = "32 QAM";
                                
                                break;
                            }
                            
                            case 2:
                            {
                                qam_String = "64 QAM";
                                
                                break;
                            }
                            
                            case 3:
                            {
                                qam_String = "128 QAM";
                                
                                break;
                            }
                            
                            case 4:
                            {
                                qam_String = "256 QAM";
                                
                                break;
                            }
                            
                            case 5:
                            {
                                qam_String = "512 QAM";
                                
                                break;
                            }
                        }
                        
                        sysInfo.setValue(qam_String);
                        sysInfos.add(sysInfo);
                    }
                    else
                    {
                        sys_Info_Name = res.getString(R.string.sys_info_qam);
                        
                        sysInfo = new SystemInfoBean();
                        sysInfo.setName(sys_Info_Name);
                        sysInfo.setValue(str_Unknown);
                        sysInfos.add(sysInfo);
                    }
                    
                    if (null != tpInfo)
                    {
                        signal_Query_Params.put(SYS_INFO_FREQUENCY, tpInfo.getTunerFreq());
                        signal_Query_Params.put(SYS_INFO_SYMBOL, tpInfo.getTunerSymbRate());
                        signal_Query_Params.put(SYS_INFO_QAM, tpInfo.getTunerEmod());
                        signal_Query_Params.put(SYS_INFO_TUNER_ID, (int)tpInfo.getTunerId());
                    }
                    else
                    {
                        signal_Query_Params.put(SYS_INFO_FREQUENCY, -1);
                        signal_Query_Params.put(SYS_INFO_SYMBOL, -1);
                        signal_Query_Params.put(SYS_INFO_QAM, -1);
                        signal_Query_Params.put(SYS_INFO_TUNER_ID, -1);
                    }
                    
                    mHandler.sendEmptyMessage(LOAD_RESOURCE_FINISHED);
                    /* End LinKang, 2013-12-20, Get the frequency/symbol/QAM information by the last viewed program... */
                }
                
                private String getIpAddress()
                {
                    try
                    {
                        for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();)
                        {
                            NetworkInterface intf = en.nextElement();
                            
                            for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();)
                            {
                                InetAddress inetAddress = enumIpAddr.nextElement();
                                
                                if (!inetAddress.isLoopbackAddress()
                                    && InetAddressUtils.isIPv4Address(inetAddress.getHostAddress()))
                                {
                                    return inetAddress.getHostAddress();
                                }
                                
                            }
                        }
                    }
                    catch (SocketException ex)
                    {
                        return null;
                    }
                    
                    return null;
                }
                
                private ServiceInfoBean paserLastProgram(String spName)
                {
                    ServiceInfoBean sBean = new ServiceInfoBean();
                    
                    if (null != spName)
                    {
                        SharedPreferences spChannel = getSharedPreferences(spName, 8);
                        
                        int lastServiceId;
                        int lastLogicalNumber;
                        int lastServicePosition;
                        int lastTpId;
                        
                        String lastChannelName;
                        
                        lastServiceId = spChannel.getInt("tv_serviceId", 0);
                        lastLogicalNumber = spChannel.getInt("tv_logicalNumber", 0);
                        lastServicePosition = spChannel.getInt("tv_servicePosition", 0);
                        lastTpId = spChannel.getInt("tv_tpId", 0);
                        lastChannelName = spChannel.getString("tv_serviceName", null);
                        
                        sBean.setServiceId(lastServiceId);
                        sBean.setLogicalNumber(lastLogicalNumber);
                        sBean.setChannelPosition(lastServicePosition);
                        sBean.setTpId(lastTpId);
                        sBean.setChannelName(lastChannelName);
                    }
                    
                    return sBean;
                }
                
            }.start();
        }
        else
        {
            LogUtils.e(TAG, "SystemInfoActivity.init_Resource():: "
                + (sysInfos == null ? "null" : "sysInfos.size = " + sysInfos.size()), true);
        }
    }
    
    private void init_View()
    {
        ll_ShowSystemInfo = (ListView)findViewById(R.id.ll_ShowSystemInfo);
        
        ll_ShowSystemInfo.setItemsCanFocus(true);
        
        pb_search_strength = (ProgressBar)findViewById(R.id.pb_search_strength);
        
        pb_search_quality = (ProgressBar)findViewById(R.id.pb_search_quality);
        
        tv_search_signal_strength = (TextView)findViewById(R.id.tv_search_signal_strength);
        
        tv_search_signal_quality = (TextView)findViewById(R.id.tv_search_signal_quality);
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
    }
    
    @Override
    protected void onResume()
    {
        super.onResume();
    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
    }
    
    @Override
    protected void onDestroy()
    {
        /* Stop the thread for update the signal quality&strength */
        sysInfoHandler.runFlag = false;
        
        super.onDestroy();
    }
    
    private class SystemInfoHandler extends Handler
    {
        public boolean runFlag = true;
        
        public SystemInfoHandler(Looper looper)
        {
            super(looper);
        }
        
        @Override
        public void handleMessage(Message msg)
        {
            super.handleMessage(msg);
            
            LogUtils.e(TAG, "SystemInfoActivity.SystemInfoHandler.handleMessage run... msg.what = " + msg.what);
            
            switch (msg.what)
            {
                case QUERY_SIGNAL_QUALITY:
                {
                    if (!runFlag)
                    {
                        return;
                    }
                    
                    LogUtils.e(TAG, "SystemInfoActivity.SystemInfoHandler.handleMessage:: QUERY_SIGNAL_QUALITY run...");
                    
                    Bundle bundle = (Bundle)msg.obj;
                    
                    int freq = bundle.getInt(SYS_INFO_FREQUENCY);
                    
                    int symbol_Rate = bundle.getInt(SYS_INFO_SYMBOL);
                    
                    int qam = bundle.getInt(SYS_INFO_QAM);
                    
                    int tunerID = bundle.getInt(SYS_INFO_TUNER_ID);
                    
                    LogUtils.e(TAG, "SystemInfoActivity.SystemInfoHandler.handleMessage:: freq = " + freq
                        + " symbol_Rate = " + symbol_Rate + " qam = " + qam + " tunerID = " + tunerID);
                    
                    getStrengthQuality(freq, symbol_Rate, qam, tunerID);
                    
                    break;
                }
                
                default:
                {
                    LogUtils.e(TAG,
                        "SystemInfoActivity.SystemInfoHandler.handleMessage:: msg.what is undealed. msg.what = "
                            + msg.what);
                    
                    break;
                }
            }
        }
        
    }
    
    private class SystemInfoAdapter extends BaseAdapter
    {
        
        @Override
        public int getCount()
        {
            return sysInfos == null ? 0 : sysInfos.size();
        }
        
        @Override
        public Object getItem(int position)
        {
            return sysInfos.get(position);
        }
        
        @Override
        public long getItemId(int position)
        {
            return position;
        }
        
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            LogUtils.e(TAG, "SystemInfoActivity.SystemInfoAdapter.getView run... position = " + position);
            
            ViewHolder vHolder = null;
            
            View view = convertView;
            
            if (null == view)
            {
                view = View.inflate(SystemInfoActivity.this, R.layout.system_info_item, null);
            }
            else
            {
                vHolder = (ViewHolder)convertView.getTag();
            }
            
            if (null == vHolder)
            {
                vHolder = new ViewHolder();
                
                vHolder.tv_Name = (TextView)view.findViewById(R.id.tv_Name);
                vHolder.tv_Value = (TextView)view.findViewById(R.id.tv_Value);
                
                view.setTag(vHolder);
            }
            
            vHolder.tv_Name.setText(sysInfos.get(position).getName());
            vHolder.tv_Value.setText(sysInfos.get(position).getValue());
            
            view.setFocusable(true);
            view.setFocusableInTouchMode(true);
            vHolder.tv_Value.setFocusable(true);
            vHolder.tv_Value.setFocusableInTouchMode(true);
            
            view.setOnFocusChangeListener(new OnFocusChangeListener()
            {
                @Override
                public void onFocusChange(View v, boolean hasFocus)
                {
                    LogUtils.e(TAG, "SystemInfoActivity.SystemInfoAdapter:: view got focus... hasFocus = " + hasFocus);
                    
                    ViewHolder vHolder = (ViewHolder)v.getTag();
                    
                    if (null != vHolder)
                    {
                        if (hasFocus)
                        {
                            vHolder.tv_Value.setBackgroundResource(R.drawable.channel_play_bg);
                            
                            vHolder.tv_Value.setTextColor(Color.WHITE);
                        }
                        else
                        {
                            vHolder.tv_Value.setBackgroundColor(Color.WHITE);
                            
                            vHolder.tv_Value.setTextColor(Color.BLACK);
                        }
                    }
                }
            });
            
            vHolder.tv_Value.setOnFocusChangeListener(new OnFocusChangeListener()
            {
                @Override
                public void onFocusChange(View v, boolean hasFocus)
                {
                    LogUtils.e(TAG, "SystemInfoActivity.SystemInfoAdapter::tv_Value got focus... hasFocus = "
                        + hasFocus);
                    
                    if (hasFocus)
                    {
                        v.setBackgroundResource(R.drawable.div_l_focus);
                    }
                    else
                    {
                        v.setBackgroundColor(Color.WHITE);
                    }
                }
            });
            
            return view;
        }
    }
    
    private static class ViewHolder
    {
        TextView tv_Name = null;
        
        TextView tv_Value = null;
    }
    
    private void getStrengthQuality(int freq, int symbol_Rate, int qam, int tunerID)
    {
        LogUtils.e(TAG, "SystemInfoActivity.getStrengthQuality run...");
        
        Log.i(TAG, "--Frequency: --->>>" + freq);
        Log.i(TAG, "--SymbolRate: --->>>" + symbol_Rate);
        Log.i(TAG, "--Qam: --->>>" + qam);
        Log.i(TAG, "--tunerID: --->>>" + tunerID);
        
        Tuner tuner = new Tuner();
        // CablePercent cable = tuner.new CablePercent();
        // DVBCore_Cab_Desc_t dvbCoreCabDesc = tuner.new DVBCore_Cab_Desc_t();
        // dvbCoreCabDesc.setU32Freq(freq);
        // dvbCoreCabDesc.setU32SymbRate(symbol_Rate);
        
        int eMode = 0;
        
        if (qam >= 0 && qam <= 16)
        {
            eMode = 0;
        }
        else if (qam > 16 && qam <= 32)
        {
            eMode = 1;
        }
        else if (qam > 32 && qam <= 64)
        {
            eMode = 2;
        }
        else if (qam > 64 && qam <= 128)
        {
            eMode = 3;
        }
        else if (qam > 128 && qam <= 256)
        {
            eMode = 4;
        }
        else if (qam > 256 && qam <= 512)
        {
            eMode = 5;
        }
        
        // dvbCoreCabDesc.seteMod(eMode);
        // dvbCoreCabDesc.seteFEC_Inner(0);
        // dvbCoreCabDesc.seteFEC_Outer(0);
        // dvbCoreCabDesc.setU8TunerId((char)0);
        
        // tuner.getTunerCablePercent(dvbCoreCabDesc, cable);
        
        // int qualityValue = cable.getQuality();
        // int strengthValue = cable.getStrength();
        int qualityValue = tuner.getTunerQualityPercent(tunerID);
        int strengthValue = tuner.getTunerStrengthPercent(tunerID);
        
        Bundle bundle = new Bundle();
        bundle.putInt("quality", qualityValue);
        bundle.putInt("strength", strengthValue);
        
        Log.i(TAG, "---------Signal Quality --->>>" + qualityValue);
        Log.i(TAG, "---------Signal Strength --->>>" + strengthValue);
        
        Message message = new Message();
        message.obj = bundle;
        message.what = Config.UPDATE_STRENGTH_QUALITY;
        mHandler.sendMessage(message);
    }
    
}
