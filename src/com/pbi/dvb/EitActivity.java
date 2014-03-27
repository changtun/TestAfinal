package com.pbi.dvb;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.ServiceConnection;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RelativeLayout;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.adapter.EpgServiceAdapter;
import com.pbi.dvb.aidl.PvrRecordServiceAIDL;
import com.pbi.dvb.dao.PageDao;
import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.PageDaoImpl;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.MessageCa;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativeEpg;
import com.pbi.dvb.dvbinterface.NativeEpg.EpgEventInfo;
import com.pbi.dvb.dvbinterface.NativeEpg.peventRate;
import com.pbi.dvb.dvbinterface.NativeEpgMsg;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_Play_Param_t;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Rec_Status_t;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrUtils;
import com.pbi.dvb.utils.StorageUtils;
import com.pbi.dvb.view.EpgDetailDialog;
import com.pbi.dvb.view.EpgDetailDialog.EpgDetailDialogCallBack;

public class EitActivity extends Activity
{
    /*2014-02-19 gtsong add dvb locks start.*/
    public static final int PROCESS_LOCKS = 200;
    private boolean firstTimeServiceLock = true;
    private boolean serviceLock = false; 
    private boolean eitLevelLock = false; 
    private boolean eitCategoryLock = false;
    private boolean caAdultLock = false;
    private int originalGrade;
    private int currentGrade;
    private int category;
    private List<String> eitLimitcategorys;
    private Dialog passwordDialog;
    /*2014-02-19 gtsong add dvb locks end.*/
    
    /*
     * Start LinKang, 2014-01-02, The eit_Show flag is used for indicate whether the page will jump to EPG page after
     * current page finished..
     */
    private boolean epg_Show = false;
    
    /*
     * End LinKang, 2014-01-02, The eit_Show flag is used for indicate whether the page will jump to EPG page after
     * current page finished..
     */
    
    private String TAG = "EitActivity";
    
    // service:频道信息
    private Runnable playTask;
    private ListView serviceList;
    
    private List<ServiceInfoBean> serviceInfoBeans;
    
    private EpgServiceAdapter serviceAdapter; // 分页Adapter
    
    private TpInfoDao tpInfoDao;
    
    // 分页
    private List<ServiceInfoBean> currentList;
    
    private PageDao pd;
    
    private int channelIndex = 0;
    
    private int listViewize = 7;
    
    private int currentPosition;
    
    private int currentPageNum;
    
    private int flag = 1;
    
    // event:节目信息
    private ListView eventList;
    
    private List<NativeEpg.EpgEventInfo> epgEventInfoBeans;
    
    // 正在播放的节目名字、时长、进度信息
    private TextView curPlayServiceName;
    
    private TextView curPlayServiceTimelength;
    
    private ProgressBar curPlayServiceProgress;
    
    private SurfaceView surfaceView;
    
    private SurfaceHolder surfHolder;
    
    private TextView curChanName;
    
    // EIT类目显示
    private GridView gv_EitCategoryHeader;
    
    // EI类目的适配器
    private EitCategoryAdapter eitCategoryAdapter;
    
    private String[] eit_Type_Names;
    
    // EPG_JNI变量
    private NativePlayer nativePlayer;
    
    private NativeDrive nativeDrive;
    
    private NativeEpg nativeEpg;
    
    private NativeEpgMsg messageEpg;
    
    private MessageCa messageCa;
    
    private boolean isIPTV = true;
    
    private boolean isDVB = false;
    
    /*--Audio--*/
    private AudioManager audioManager;
    
    private boolean g_bAudioIsMute;
    
    // 记录当前选择的信息---全局变量
    private Button weekdayBn; // 记录选择的EPG星期(获取和失去焦点时)
    
    /* 0代表今天, 1代表明天, 以此类推... */
    private int[] weekDaysForQuery = null;
    
    private int weekday; // 星期，默认取"今天"
    
    private int eit_Type = 4;// EIT类型, 默认取0... 具体的意义要参考局方的定义
    
    private String playChanName; // 频道名称
    
    private int channelNum; // 每条记录的主键
    
    private int serviceId;
    
    private int logicalNumber;
    
    // 底部信息切换
    private ImageView serviceImage;
    
    private ImageView eventImage;
    
    private ImageView focusImage;
    
    private ImageView weekdayImage;
    
    private ImageView PvrOrderImage;
    
    private TextView serviceText;
    
    private TextView eventText;
    
    private TextView focusText;
    
    private TextView weekdayText;
    
    private TextView PvrOrderText;
    
    // 设置颜色
    private ColorStateList black;
    
    private ColorStateList white;
    
    // event节目级别，CA卡成人级别，盒子(亲子锁和节目锁)密码
    
    private char serviceType = Config.SERVICE_TYPE_TV;
    private Toast caMessageToast;
    
    @Override
    protected void onResume()
    {
        super.onResume();
        
        LogUtils.e(TAG, "EitActivity.onResume run...");
        
        /* Start LinKang, 2013-12-19, Switch the left and right key to control direction... */
//        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        /* End LinKang, 2013-12-19, Switch the left and right key to control direction... */
    }
    
    private static SurfaceHolder.Callback mSHCallback = new SurfaceHolder.Callback()
    {
        public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
        {
            // initSurface(holder);
        }
        
        public void surfaceCreated(SurfaceHolder holder)
        {
            initSurface(holder);
        }
        
        public void surfaceDestroyed(SurfaceHolder holder)
        {
        }
        
        private void initSurface(SurfaceHolder h)
        {
            Canvas c = null;
            try
            {
                c = h.lockCanvas();
            }
            finally
            {
                if (c != null)
                    h.unlockCanvasAndPost(c);
            }
        }
    };
    
    /**
     * process ca message.
     */
    private Handler caHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            
            Log.i(TAG, "msg.what = " + msg.what);
            switch (msg.what)
            {
            // case Config.CTI_CA_E00:
            // showCaMessageToast(getResources().getString(R.string.E00));
            // break;
                case Config.CTI_CA_E01:
                    showCaMessageToast(getResources().getString(R.string.E01));
                    break;
                case Config.CTI_CA_E02:
                    showCaMessageToast(getResources().getString(R.string.E02));
                    break;
                case Config.CTI_CA_E03:
                    showCaMessageToast(getResources().getString(R.string.E03));
                    break;
                case Config.CTI_CA_E04:
                    showCaMessageToast(getResources().getString(R.string.E04));
                    break;
                case Config.CTI_CA_E05:
                    showCaMessageToast(getResources().getString(R.string.E05));
                    break;
                // case Config.CTI_CA_E06:
                // showCaMessageToast(getResources().getString(R.string.E06));
                // break;
                case Config.CTI_CA_E07:
                    showCaMessageToast(getResources().getString(R.string.E07));
                    break;
                // case Config.CTI_CA_E08:
                // showCaMessageToast(getResources().getString(R.string.E08));
                // break;
                case Config.CTI_CA_E10:
                    showCaMessageToast(getResources().getString(R.string.E10));
                    break;
                case Config.CTI_CA_E11:
                    showCaMessageToast(getResources().getString(R.string.E11));
                    break;
                case Config.CTI_CA_E12:
                    showCaMessageToast(getResources().getString(R.string.E12));
                    break;
                case Config.CTI_CA_E13:
                    caAdultLock = true;
                    if (null == passwordDialog)
                    {
                        passwordDialog =
                            DialogUtils.passwordDialogCreate(EitActivity.this, getString(R.string.E13), handler, true);
                    }
                    break;
                    // input ca password feedback.
                case Config.CA_PARENTAL_REPLY:
                    int parentalStatus = (Integer)msg.obj;
                    switch (parentalStatus)
                    {
                        case 0:
                            caAdultLock = false;
                            nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                            break;
                        case 1:
                        case 2:
                        case 3:
                            if (null == passwordDialog)
                            {
                                passwordDialog =
                                    DialogUtils.passwordDialogCreate(EitActivity.this,
                                        getString(R.string.E13),
                                        handler,
                                        true);
                            }
                            break;
                    }
                    break;
                case Config.CTI_CA_E14:
                    showCaMessageToast(getResources().getString(R.string.E14));
                    break;
                case Config.CTI_CA_E15:
                    showCaMessageToast(getResources().getString(R.string.E15));
                    break;
                // case Config.CTI_CA_E16:
                // showCaMessageToast(getResources().getString(R.string.E16));
                // break;
                case Config.CTI_CA_E17:
                    showCaMessageToast(getResources().getString(R.string.E17));
                    break;
                case Config.CTI_CA_E18:
                    showCaMessageToast(getResources().getString(R.string.E18));
                    break;
                case Config.CTI_CA_E19:
                    showCaMessageToast(getResources().getString(R.string.E19));
                    break;
                case Config.CTI_CA_E20:
                    showCaMessageToast(getResources().getString(R.string.E20));
                    break;
                case Config.CTI_CA_E21:
                    showCaMessageToast(getResources().getString(R.string.E21));
                    break;
                case Config.CTI_CA_E22:
                    showCaMessageToast(getResources().getString(R.string.E22));
                    break;
                case Config.CTI_CA_E23:
                    showCaMessageToast(getResources().getString(R.string.E23));
                    break;
                case Config.CTI_CA_E24:
                    showCaMessageToast(getResources().getString(R.string.E24));
                    break;
                case Config.CTI_CA_E25:
                    showCaMessageToast(getResources().getString(R.string.E25));
                    break;
                case Config.CTI_CA_E26:
                    showCaMessageToast(getResources().getString(R.string.E26));
                    break;
                case Config.CTI_CA_E27:
                    showCaMessageToast(getResources().getString(R.string.E27));
                    break;
                case Config.CTI_CA_E28:
                    showCaMessageToast(getResources().getString(R.string.E28));
                    break;
                case Config.CTI_CA_E29:
                    showCaMessageToast(getResources().getString(R.string.E29));
                    break;
                case Config.CTI_CA_E30:
                    showCaMessageToast(getResources().getString(R.string.E30));
                    break;
                case Config.CTI_CA_E40:
                    showCaMessageToast(getResources().getString(R.string.E40));
                    break;
                case Config.CTI_CA_E41:
                    showCaMessageToast(getResources().getString(R.string.E41));
                    break;
                case Config.CTI_CA_E42:
                    showCaMessageToast(getResources().getString(R.string.E42));
                    break;
                case Config.CTI_CA_E43:
                    showCaMessageToast(getResources().getString(R.string.E43));
                    break;
                case Config.CTI_CA_E44:
                    showCaMessageToast(getResources().getString(R.string.E44));
                    break;
                case Config.CTI_CA_E45:
                    showCaMessageToast(getResources().getString(R.string.E45));
                    break;
                case Config.CTI_CA_E46:
                    showCaMessageToast(getResources().getString(R.string.E46));
                    break;
                case Config.CTI_CA_E47:
                    showCaMessageToast(getResources().getString(R.string.E47));
                    break;
                // case Config.CTI_CA_E50:
                // showCaMessageToast(getResources().getString(R.string.E50));
                // break;
                case Config.CTI_CA_E51:
                    showCaMessageToast(getResources().getString(R.string.E51));
                    break;
                case Config.CTI_CA_E52:
                    showCaMessageToast(getResources().getString(R.string.E52));
                    break;
                case Config.CTI_CA_E53:
                    showCaMessageToast(getResources().getString(R.string.E53));
                    break;
                case Config.CTI_CA_E54:
                    showCaMessageToast(getResources().getString(R.string.E54));
                    break;
                case Config.CTI_CA_E55:
                    showCaMessageToast(getResources().getString(R.string.E55));
                    break;
                // case Config.CTI_CA_E56:
                // showCaMessageToast(getResources().getString(R.string.E56));
                // break;
                case Config.CTI_CA_E57:
                    showCaMessageToast(getResources().getString(R.string.E57));
                    break;
            }
        }
    };
    
    /**
     * 更新对话框
     */
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case PROCESS_LOCKS:
                    
                    if(null !=passwordDialog)
                    {
                        passwordDialog.dismiss();
                        passwordDialog = null;
                    }
                    //ca adult lock.
                    if(caAdultLock)
                    {
                        NativeCA nativeCA = new NativeCA();
                        int childPwd = Integer.parseInt((String)msg.obj);
                        nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PARENTAL_CHANGE_QUERY, null, childPwd);
                        return;
                    }
                    //service lock.
                    if(serviceLock)
                    {
                        firstTimeServiceLock = false;
                        serviceLock = false;
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        return;
                    }
                    //eit category lock.
                    if(eitCategoryLock)
                    {
                        eitCategoryLock = false;
                        eitLimitcategorys.remove(String.valueOf(category));
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        return;
                    }
                    if(eitLevelLock)
                    {
                        eitLevelLock = false;
                        originalGrade = currentGrade;
                        android.provider.Settings.System.putInt(getContentResolver(), "eit_grade", originalGrade);
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        return;
                    }
                    break;
                case Config.UPDATE_EPG_INFO:
                    Log.i(TAG, " ************** update epg info ! ************");
                    handler.removeCallbacks(playTask);
                    handler.post(playTask);
                    break;
                case Config.UP_SWITCH_SERVICE:
                    dismissDialogs();
                    upKeyMethod();
                    break;
                case Config.DOWN_SWITCH_SERVICE:
                    dismissDialogs();
                    downKeyMethod();
                    break;
                case Config.EPG_PLAYING_INFO:
                    Bundle bundle1 = (Bundle)msg.obj;
                    String serviceName = bundle1.getString("serviceName");
                    String serviceTimelength = bundle1.getString("serviceTimelength");
                    int serviceProgress = bundle1.getInt("serviceProgress");
                    
                    curPlayServiceName.setText(serviceName);
                    curPlayServiceTimelength.setText(serviceTimelength);
                    curPlayServiceProgress.setSecondaryProgress(serviceProgress);
                    
                    curPlayServiceName.invalidate();
                    curPlayServiceTimelength.invalidate();
                    curPlayServiceProgress.invalidate();
                    break;
                case Config.EPG_EVENT_LIST:
                    
                    LogUtils.e(TAG, "EitActivity.handler.handleMessage.EPG_EVENT_LIST:: run...");
                    
                    Bundle bundle2 = (Bundle)msg.obj;
                    
                    int epglength = 0;
                    
                    if (null != bundle2)
                    {
                        epglength = bundle2.getInt("epglength");
                    }
                    
                    LogUtils.e(TAG, "EitActivity.handler.handleMessage.EPG_EVENT_LIST:: epgLength = " + epglength);
                    
                    List<HashMap<String, Object>> data = getEventData();
                    
                    SimpleAdapter eventAdapter =
                        new SimpleAdapter(EitActivity.this, data, R.layout.epg_event_item, new String[] {"eventImage",
                            "eventRate", "eventTime", "eventName", "eventYMD"}, new int[] {R.id.event_image,
                            R.id.event_rate, R.id.event_time, R.id.event_name, R.id.event_ymd});
                    
                    eventList.setAdapter(eventAdapter);
                    eventList.setOnItemClickListener(new ChannelOnItemClickListener());
                    eventList.invalidate();
                    break;
            
            }
        }
    };
    
    public synchronized List<HashMap<String, Object>> getEventData()
    {
        List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();
        for (Iterator<NativeEpg.EpgEventInfo> itr = epgEventInfoBeans.iterator(); itr.hasNext();)
        {
            NativeEpg.EpgEventInfo eventInfo = itr.next();
            
            String epgstartHour = timeFormat(eventInfo.getstarthour() + "");
            String epgstartMinute = timeFormat(eventInfo.getstartminute() + "");
            
            String epgendHour = timeFormat(eventInfo.getendhour() + "");
            String epgendMinute = timeFormat(eventInfo.getendminute() + "");
            
            HashMap<String, Object> item = new HashMap<String, Object>();
            item.put("id", eventInfo.geteventid());
            item.put("eventTime", epgstartHour + ":" + epgstartMinute + "-" + epgendHour + ":" + epgendMinute);
            item.put("eventName", eventInfo.geteventname());
            
            int rate = eventInfo.getrate();
            if (rate == 0)
            {
                item.put("eventImage", R.drawable.epg_rate_green);
                item.put("eventRate", getResources().getString(R.string.rate_normal));
            }
            else if (rate >= 1 && rate <= 6)
            {
                item.put("eventImage", R.drawable.epg_rate_blue);
                item.put("eventRate", getResources().getString(R.string.rate_protect));
            }
            else if (rate >= 7 && rate <= 12)
            {
                item.put("eventImage", R.drawable.epg_rate_yellow);
                item.put("eventRate", getResources().getString(R.string.rate_assist));
                
            }
            else if (rate >= 13 && rate <= 14)
            {
                item.put("eventImage", R.drawable.epg_rate_red);
                item.put("eventRate", getResources().getString(R.string.rate_adult));
            }
            else if (rate >= 15)
            {
                item.put("eventImage", R.drawable.epg_rate_pink);
                item.put("eventRate", getResources().getString(R.string.rate_special));
            }
            
            String eventY = eventInfo.getstartyear() + "";
            String eventM = timeFormat(eventInfo.getstartmonth() + "");
            String eventD = timeFormat(eventInfo.getstartdate() + "");
            item.put("eventYMD", eventY + "-" + eventM + "-" + eventD);
            
            data.add(item);
        }
        return data;
    }
    
    protected void onCreate(Bundle savedInstanceState)
    {
        Log.i(TAG, "onCreate() : ======================= ");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.eit_main);
        
        Bundle data = this.getIntent().getExtras();
        if (null != data)
        {
            serviceId = data.getInt("ServiceId", 0);
            logicalNumber = data.getInt("LogicalNumber");
            serviceType = data.getChar(Config.DVB_SERVICE_TYPE);
			
            LogUtils.e(TAG, "EitActivity.onCreate:: serviceType[int] = " + (int)serviceType + " serviceType[char] = "
                + serviceType);
        }
        else
        {
            ServiceInfoBean serviceInfo = paserLastProgram(Config.DVB_LASTPLAY_SERVICE);
            
            serviceId = serviceInfo.getServiceId();
            logicalNumber = serviceInfo.getLogicalNumber();
            serviceType = Config.SERVICE_TYPE_NOTYPE;
        }
		try
        {
            originalGrade = android.provider.Settings.System.getInt(getContentResolver(), "eit_grade");
        }
        catch(Exception e)
        {
            originalGrade = 1;// default value.
            e.printStackTrace();
        }
        eitLimitcategorys = CommonUtils.getNitCategorySettings(this);
        
        Log.i(TAG, "onCreate() : logicalNumber = " + logicalNumber);
        Log.i(TAG, "onCreate() : serviceId = " + serviceId);
        Log.i(TAG, "onCreate() : originalGrade = " + originalGrade);
        
        Resources res = getResources();
        
        eit_Type_Names = res.getStringArray(R.array.EitCategory);
        
        initWidget();
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
    
    protected void onStart()
    {
        super.onStart();
        Log.i(TAG, "onStart() : ======================= ");
        //init left/right key to left/right function.
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        /*forbid this key switch left/right and volume up/down function.*/
        CommonUtils.forbidSwitchKeyFunction(this,false);
        initDrive();
        
        if (!serviceInfoBeans.isEmpty())
        {
            handler.removeCallbacks(playTask);
            handler.post(playTask);
        }

    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
        /*recover this key switch left/right and volume up/down function.*/
        CommonUtils.forbidSwitchKeyFunction(this,true);
        /*
         * Start LinKang, 2013-01-02, If the EPG page is the next page to be shown.. Do not destroy the resource related
         * to DVB...
         */
//        if (!epg_Show)
//        {
//            /* Start LinKang, 2013-12-19, Switch the left and right key to control volume... */
//            CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
//            /* End LinKang, 2013-12-19, Switch the left and right key to control volume... */
//        }
//        else
//        {
//            CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
//        }
        
        if (caMessageToast != null)
        {
            caMessageToast = null;
        }
        
        if (!isDVB && !epg_Show)
        {
            nativePlayer.DVBPlayerStop();
        }
        
        if (isIPTV && !epg_Show)
        {
            // destory the dvb player.
            nativePlayer.DVBPlayerDeinit();
            nativeDrive.DrvPlayerUnInit();
        }
        
        epg_Show = false;
        /*
         * End LinKang, 2013-01-02, If the EPG page is the next page to be shown.. Do not destroy the resource related
         * to DVB...
         */
        
        // nativePlayer.DVBPlayerStop();
        // nativePlayer.DVBPlayerDeinit();
        // nativeDrive.DrvPlayerUnInit();
        // finish();
    }
    
    protected void onStop()
    {
        super.onStop();
        Log.i(TAG, "onStop() : ======================= ");
        LogUtils.e(TAG, "EitActivity.onStop run...");
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    /**
     * 初始化底层jni驱动等
     */
    private void initDrive()
    {
        nativeDrive = new NativeDrive(EitActivity.this);
        nativeDrive.pvwareDRVInit();
        nativeDrive.CAInit();
        nativeDrive.DrvPlayerInit();
        
        nativePlayer = NativePlayer.getInstance();
        nativeEpg = new NativeEpg();
        messageEpg = new NativeEpgMsg(handler);
        nativeEpg.EpgMessageInit(messageEpg);
        
        NativeCA nativeCA = new NativeCA();
        messageCa = new MessageCa(caHandler, this);
        nativeCA.CaMessageInit(messageCa);
        
        // 设置小视频播放的坐标及大小
        NativePlayer.DRV_AVCtrl_VidWindow_t playWindow = nativePlayer.new DRV_AVCtrl_VidWindow_t();
        /*
         * //DisplayManager display = new DisplayManager(); Display display = getWindowManager().getDefaultDisplay();
         * Rect rect = new Rect(); display.getRectSize(rect); //rect.bottom += 48; Log.i(TAG,
         * "rect.right"+rect.right+"rect.bottom"+rect.bottom+"rect.top"+rect.top+"rect.left"+rect.left);
         */
        Rect rect = new Rect();
        rect = CommonUtils.getPlayWindow();
        playWindow.setWinX((1340 + rect.left) * rect.right / 1920);
        playWindow.setWinY((110 + rect.top) * rect.bottom / 1080);
        playWindow.setWinWidth(510 * rect.right / 1920);
        playWindow.setWinHeight(250 * rect.bottom / 1080);
        nativePlayer.DVBPlayerAVCtrlSetWindows(playWindow, 1);
        
        // Set audio.
        audioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
        int volume = 0;
        
        volume = audioManager.getLastAudibleStreamVolume(audioManager.STREAM_SYSTEM);
        volume *= 7;
        if (100 < volume)
        {
            volume = 100;
        }
        nativePlayer.DVBPlayerAVCtrlSetVolume(volume);
        
        g_bAudioIsMute = audioManager.isStreamMute(AudioManager.STREAM_MUSIC);
        if (true == g_bAudioIsMute)
        {
            nativePlayer.DVBPlayerAVCtrlAudMute(1);
        }
        else
        {
            nativePlayer.DVBPlayerAVCtrlAudMute(0);
        }
        
    }
    
    /**
     * 初始化页面布局中的控件及资源
     */
    private void initWidget()
    {
        
        black = getBaseContext().getResources().getColorStateList(R.drawable.black);
        white = getBaseContext().getResources().getColorStateList(R.drawable.white);
        
        curPlayServiceName = (TextView)findViewById(R.id.cur_service_name);
        curPlayServiceTimelength = (TextView)findViewById(R.id.cur_service_timelength);
        curPlayServiceProgress = (ProgressBar)findViewById(R.id.cur_service_progress);
        
        gv_EitCategoryHeader = (GridView)findViewById(R.id.gv_EitCategoryHeader);
        clearGridViewPosition(gv_EitCategoryHeader);
        eitCategoryAdapter = new EitCategoryAdapter();
        LogUtils.e(TAG, "eitCategoryAdapter.size = " + eitCategoryAdapter.getCount());
        gv_EitCategoryHeader.setAdapter(eitCategoryAdapter);
        gv_EitCategoryHeader.setOnItemSelectedListener(new EitCategoryOnItemSelectedLinstener());
        
        initRadioBnOf7Day();
        
        init_WeekDaysForQuery();
        
        serviceList = (ListView)findViewById(R.id.service_list);
        serviceList.setDividerHeight(0);
        eventList = (ListView)findViewById(R.id.event_list);
        
        initServiceInfos();
        
        epgEventInfoBeans = new ArrayList<NativeEpg.EpgEventInfo>();
        tpInfoDao = new TpInfoDaoImpl(EitActivity.this);
        
        serviceImage = (ImageView)findViewById(R.id.service_image);
        eventImage = (ImageView)findViewById(R.id.event_image);
        focusImage = (ImageView)findViewById(R.id.focus_image);
        weekdayImage = (ImageView)findViewById(R.id.weekday_image);
        PvrOrderImage = (ImageView)findViewById(R.id.pvr_image);
        serviceText = (TextView)findViewById(R.id.service_text);
        eventText = (TextView)findViewById(R.id.event_text);
        focusText = (TextView)findViewById(R.id.focus_text);
        weekdayText = (TextView)findViewById(R.id.weekday_text);
        PvrOrderText = (TextView)findViewById(R.id.pvr_text);
        
        focusInServiceList();
        
        serviceList.setOnFocusChangeListener(new OnFocusChangeListener()
        {
            @Override
            public void onFocusChange(View v, boolean hasFocus)
            {
                if (hasFocus)
                {
                    focusInServiceList();
                    serviceListGetFocus();
                }
                else
                {
                    serviceListLostFocus();
                }
            }
        });
        
        surfaceView = (SurfaceView)findViewById(R.id.epg_sur_view);
        surfHolder = surfaceView.getHolder();
        surfHolder.setFixedSize(surfaceView.getWidth(), surfaceView.getHeight());
        surfHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        surfHolder.addCallback(mSHCallback);
        surfHolder.setFormat(PixelFormat.VIDEO_HOLE);
        curChanName = (TextView)findViewById(R.id.cur_chan_name);
        
        playTask = new RefreshPlayingAndEventTask();
    }
    
    private void init_WeekDaysForQuery()
    {
        Date date = new Date();
        Calendar cd = Calendar.getInstance();
        cd.setTime(date);
        
        final int week = cd.get(Calendar.DAY_OF_WEEK) - 1; // 0,1,2,...,6，分别代表星期日,星期一,...,星期六。
        
        LogUtils.e(TAG, "EitActivity.init_WeekDaysForQuery:: week = " + week);
        
        weekDaysForQuery = new int[] {0};
    }
    
    /**
     * 七天按钮控件初始化
     */
    private void initRadioBnOf7Day()
    {
    }
    
    /**
     * 获取及显示service,频道列表信息
     */
    @SuppressWarnings("unchecked")
    private void initServiceInfos()
    {
        ServiceInfoDao service = new ServiceInfoDaoImpl(EitActivity.this);
        switch (serviceType)
        {
            case Config.SERVICE_TYPE_RADIO:
                serviceInfoBeans = service.getRadioChannelInfo();
                break;
            default:
                serviceInfoBeans = service.getTVChannelInfo();
                break;
        }
        
        pd = new PageDaoImpl(serviceInfoBeans, listViewize);
        
        // 分页初始化
        channelIndex = getIndexByLogicalNum(logicalNumber); // 在节目频道集合中的索引(0,1,...)
        currentPageNum = channelIndex / listViewize + 1; // 列表当前显示的页数
        currentPosition = channelIndex % listViewize; // 列表项在当前页中的位置
        Log.i(TAG, "initServiceInfos() : channelIndex = " + channelIndex);
        Log.i(TAG, "initServiceInfos() : currentPageNum = " + currentPageNum);
        Log.i(TAG, "initServiceInfos() : currentPosition = " + currentPosition);
        
        // 全局变量初始化
        playChanName = serviceInfoBeans.get(channelIndex).getChannelName();
        channelNum = serviceInfoBeans.get(channelIndex).getChannelNumber();
        
        LogUtils.e(TAG, "EitActivity.initServiceInfos:: playChanName = " + playChanName + " channelNum = " + channelNum);
        
        // 自定义适配器，初始化当前页(currentPageNum)数据
        pd.setCurrentPage(currentPageNum);
        currentList = (List<ServiceInfoBean>)pd.getCurrentList();
        serviceAdapter = new EpgServiceAdapter(this);
        serviceAdapter.setTvProgramList(currentList);
        
        // 播放当前页的第currentPosition项，默认是第0项
        serviceList.setAdapter(serviceAdapter);
        serviceList.setSelection(currentPosition);
        serviceList.requestFocus(currentPosition);
        
        serviceList.setOnItemSelectedListener(new ServiceOnItemSelectedListener());
        serviceList.setOnItemClickListener(new ChannelOnItemClickListener());
    }
    
    public synchronized void clearEventInfos()
    {
        epgEventInfoBeans.clear(); // 清空event集合中的信息
    }
    
    public synchronized void refreshEventInfos(int eventNum, NativeEpg.EpgEventInfo[] eventArray)
    {
        for (int j = 0; j < eventNum; j++)
        {
            epgEventInfoBeans.add(eventArray[j]);
        }
    }
    
    /**
     * 更新Event信息的任务类
     */
    class RefreshEventsTask implements Runnable
    {
        /* The weekdays contains the time gap of the eit you want to download... */
        
        public RefreshEventsTask()
        {
        }
        
        public void run()
        {
            if (null == weekDaysForQuery)
            {
                LogUtils.e(TAG,
                    "EitActivity.RefreshEventsTask.run:: weekdays == null... Do not get any eit information...");
                
                return;
            }
            
            clearEventInfos();
            ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
            TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
            short serviceid = serviceBean.getServiceId().shortValue();
            short tsid = tpInfoBean.getTranStreamId().shortValue();
            short ornetowrkid = tpInfoBean.getOriginalNetId().shortValue();
            
            NativeEpg.EPGOnePage epgOnePage = nativeEpg.new EPGOnePage();
            
            int totalGet = 0;
            
            int eventNum = 0;
            
            for (int i = 0; i < 50; i++)
            {
                int currentWeekDay = weekDaysForQuery[totalGet];
                // int currentWeekDay = i % 7;
                
                LogUtils.e(TAG, "eit_type = " + eit_Type + "currentWeekDay = " + currentWeekDay + " ..."
                    + (char)currentWeekDay + " weekDaysForQuery.length = " + weekDaysForQuery.length);
                
                nativeEpg.GetEventListInfoByNibble(serviceid,
                    tsid,
                    ornetowrkid,
                    (char)currentWeekDay,
                    (short)1,
                    (short)100,
                    epgOnePage,
                    (short)eit_Type);
                
                eventNum = epgOnePage.gettotalnum();
                
                if (eventNum != 0)
                {
                    LogUtils.e(TAG, "EitActivity.RefreshEventsTask.run:: eventNum = " + eventNum + " try count = " + i);
                    
                    totalGet++;
                    
                    NativeEpg.EpgEventInfo[] eventArray = epgOnePage.getEpgList();
                    
                    refreshEventInfos(eventNum, eventArray);
                    
                    if (totalGet == weekDaysForQuery.length)
                    {
                        Bundle bundle = new Bundle();
                        
                        bundle.putInt("epglength", eventNum);
                        
                        Message message = new Message();
                        
                        message.obj = bundle;
                        
                        message.what = Config.EPG_EVENT_LIST;
                        
                        handler.sendMessage(message);
                        
                        return;
                    }
                }
                else
                {
                    LogUtils.e(TAG, "EitActivity.RefreshEventsTask.run:: eventNum == 0");
                }
            }
            
            eventNum = epgOnePage.gettotalnum();
            
            if (0 != eventNum)
            {
                LogUtils.e(TAG, "EitActivity.RefreshEventsTask.run:: eventNum == 0, send EPG_EVENT_LIST message...");
                
                NativeEpg.EpgEventInfo[] eventArray = epgOnePage.getEpgList();
                
                LogUtils.e(TAG, "EitActivity.RefreshEventsTask.run:: eventArray.length = "
                    + (null == eventArray ? 0 : eventArray.length));
                
                refreshEventInfos(epgOnePage.gettotalnum(), eventArray);
                
                Bundle bundle = new Bundle();
                
                bundle.putInt("epglength", eventNum);
                
                Message message = new Message();
                
                message.obj = bundle;
                
                message.what = Config.EPG_EVENT_LIST;
                
                handler.sendMessage(message);
            }
            else
            {
                LogUtils.e(TAG, "EitActivity.RefreshEventsTask.run:: eventNum == 0, send epglength = 0 message...");
                
                Bundle bundle = new Bundle();
                bundle.putInt("epglength", 0);
                
                Message message = new Message();
                message.obj = bundle;
                message.what = Config.EPG_EVENT_LIST;
                handler.sendMessage(message);
            }
            
        }
        
    }
    
    /**
     * 更新当前正在播放节目的任务类
     */
    class RefreshPlayingTask implements Runnable
    {
        public void run()
        {
            ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
            NativeEpg.EpgEventInfo epgEventInfo = nativeEpg.new EpgEventInfo();
            TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
            
            for (int i = 0; i < 50; i++)
            {
                nativeEpg.GetPevent(serviceBean.getServiceId().shortValue(),
                    tpInfoBean.getTranStreamId().shortValue(),
                    tpInfoBean.getOriginalNetId().shortValue(),
                    epgEventInfo);
                if (epgEventInfo.geteventname() != null)
                {
                    String epgstartYear = epgEventInfo.getstartyear() + "";
                    String epgstartMonth = timeFormat(epgEventInfo.getstartmonth() + "");
                    String epgstartDay = timeFormat(epgEventInfo.getstartdate() + "");
                    String epgstartHour = timeFormat(epgEventInfo.getstarthour() + "");
                    String epgstartMinute = timeFormat(epgEventInfo.getstartminute() + "");
                    String epgstartSecond = timeFormat(epgEventInfo.getstartsecond() + "");
                    
                    String epgendYear = epgEventInfo.getendyear() + "";
                    String epgendMonth = timeFormat(epgEventInfo.getendmonth() + "");
                    String epgendDay = timeFormat(epgEventInfo.getenddate() + "");
                    String epgendHour = timeFormat(epgEventInfo.getendhour() + "");
                    String epgendMinute = timeFormat(epgEventInfo.getendminute() + "");
                    String epgendSecond = timeFormat(epgEventInfo.getendsecond() + "");
                    
                    String epgStartDate =
                        epgstartYear + "-" + epgstartMonth + "-" + epgstartDay + " " + epgstartHour + ":"
                            + epgstartMinute + ":" + epgstartSecond;
                    
                    String epgEndDate =
                        epgendYear + "-" + epgendMonth + "-" + epgendDay + " " + epgendHour + ":" + epgendMinute + ":"
                            + epgendSecond;
                    
                    String timelength =
                        epgStartDate.substring(epgStartDate.indexOf(" ") + 1) + "/"
                            + epgEndDate.substring(epgEndDate.indexOf(" ") + 1);
                    
                    Bundle bundle = new Bundle();
                    bundle.putString("serviceName", epgEventInfo.geteventname());
                    bundle.putString("serviceTimelength", timelength);
                    bundle.putInt("serviceProgress", getProgressValue(epgStartDate, epgEndDate));
                    
                    Message message = new Message();
                    message.obj = bundle;
                    message.what = Config.EPG_PLAYING_INFO;
                    handler.sendMessage(message);
                    
                    return;
                }
            }
            Bundle bundle = new Bundle();
            bundle.putString("serviceName", null);
            bundle.putString("serviceTimelength", "00:00:00/00:00:00");
            bundle.putInt("serviceProgress", 0);
            
            Message message = new Message();
            message.obj = bundle;
            message.what = Config.EPG_PLAYING_INFO;
            handler.sendMessage(message);
        }
        
    }
    
    /**
     * 计算进度条值
     */
    private int getProgressValue(String epgStartDate, String epgEndDate)
    {
        int progress = 0;
        SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        
        // EPG返回的当前时间
        NativeEpg.EpgTime epgTime = nativeEpg.new EpgTime();
        nativeEpg.GetUtcTime(epgTime);
        int year = epgTime.getutcyear();
        int month = epgTime.getutcmonth();
        int day = epgTime.getutcdate();
        int hour = epgTime.getutchour();
        int minute = epgTime.getutcminute();
        int second = epgTime.getutcsecond();
        String s_date = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
        
        try
        {
            Date startDate = df.parse(epgStartDate);
            Date endDate = df.parse(epgEndDate);
            Date curDate = df.parse(s_date);
            long l1 = curDate.getTime() - startDate.getTime();
            long l2 = endDate.getTime() - startDate.getTime();
            progress = (int)((double)l1 * 100 / l2);
            return progress;
        }
        catch (ParseException e)
        {
            e.printStackTrace();
        }
        return progress;
    }
    
    /**
     * 格式化日期、时间，不足两位，前面用0补齐
     */
    public String timeFormat(String str)
    {
        return str.length() == 1 ? "0" + str : str;
    }
    
    /**
     * 通过节目名称获取节目播放的参数
     */
    public DVBPlayer_Play_Param_t getPlayerParamsByChanNum(int channelNum)
    {
        Tuner tuner = new Tuner();
        NativePlayer.DVBPlayer_Play_Param_t tParam = nativePlayer.new DVBPlayer_Play_Param_t();
        Tuner.DVBCore_Cab_Desc_t tCable = tuner.new DVBCore_Cab_Desc_t();
        NativePlayer.DVBPlayer_Serv_Param_t tServParam = nativePlayer.new DVBPlayer_Serv_Param_t();
        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
        
        LogUtils.e(TAG, "EitActivity.getPlayerParamsByChanNum:: service id = " + serviceBean.getServiceId());
        LogUtils.e(TAG, "EitActivity.getPlayerParamsByChanNum:: logicalNumber = " + serviceBean.getLogicalNumber());
        
        int tpId = serviceBean.getTpId();
        TPInfoBean tpInfo = tpInfoDao.getTpInfo(tpId);
        
        // 节目号
        int channelNumber = serviceBean.getChannelNumber();
        
        LogUtils.e(TAG, "EitActivity.getPlayerParamsByChanNum:: channelNumber = " + channelNumber);
        
        // 封装参数
        int eProgType = Config.DVBPlayer_Prog_Type_SERVICE;
        int eServiceLockFlag = 0;
        int u16ServiceId = serviceBean.getServiceId();
        int u16PMTid = serviceBean.getPmtPid();
        int ulogicnum = serviceBean.getLogicalNumber();
        
        int eSignalSource = Config.DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBC;
        char u8TunerId = tpInfo.getTunerId();
        int u32Freq = tpInfo.getTunerFreq();
        int u32SymbRate = tpInfo.getTunerSymbRate();
        
        int eMod = tpInfo.getTunerEmod();
        int eFEC_Inner = tpInfo.getEfecInner();
        int eFEC_Outer = tpInfo.getEfecOuter();
        
        tCable.seteFEC_Inner(eFEC_Inner);
        tCable.seteFEC_Outer(eFEC_Outer);
        tCable.seteMod(eMod);
        tCable.setU32SymbRate(u32SymbRate);
        tCable.setU32Freq(u32Freq);
        tCable.setU8TunerId(u8TunerId);
        
        tServParam.seteServLockFlag(eServiceLockFlag);
        tServParam.setU16PMTPid(u16PMTid);
        tServParam.setU16ServId(u16ServiceId);
        tServParam.setU16LogicNum(ulogicnum);
        
        tParam.seteSignalSource(eSignalSource);
        tParam.settCable(tCable);
        tParam.seteProgType(eProgType);
        tParam.settServParam(tServParam);
        
        return tParam;
    }
    
    /**
     * 通过主键channel_number，获取它在集合中的索引
     */
    public int getIndexByChannelNum(int channelNum)
    {
        for (int i = 0; i < serviceInfoBeans.size(); i++)
        {
            if (serviceInfoBeans.get(i).getChannelNumber() == channelNum)
            {
                return i;
            }
        }
        return 0;// 没查询到时，返回0，播放第一个节目
    }
    
    public int getIndexByLogicalNum(int logicalNum)
    {
        for (int i = 0; i < serviceInfoBeans.size(); i++)
        {
            if (logicalNum == serviceInfoBeans.get(i).getLogicalNumber())
            {
                return i;
            }
        }
        return 0;// 没查询到时，返回0，播放第一个节目
    }
    
    /**
     * 更新event信息
     */
    private void refreshEventList()
    {
        new Thread(new RefreshEventsTask()).start();
    }
    
	    class RefreshPlayingAndEventTask implements Runnable
    {
        @Override
        public void run()
        {
            refreshPlayingAndEvent();
        }
    }
	
    /**
     * 更新event信息
     */
    public void refreshEventInfo()
    {
        new Thread(new RefreshPlayingTask()).start();
        new Thread(new RefreshEventsTask()).start();
    }
    
    /**
     * 更新当前频道播放节目，及Events信息
     */
    public void refreshPlayingAndEvent()
    {
        Log.i(TAG, "==== refreshPlayingAndEvent() ===== : playChanName = " + playChanName + " ; weekday = " + weekday);
        
        // 1.更新正在播放的节目名称、时间及进度
        new Thread(new RefreshPlayingTask()).start();
        
        // 2.获取及显示event,节目列表信息
        new Thread(new RefreshEventsTask()).start();
        
        // 3.停止视频的播放
        nativePlayer.DVBPlayerStop();
        
        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
        
        /*
         * the service lock only get once, and it has the highest priority.
         */
        if (firstTimeServiceLock)
        {
            if (serviceBean.getLockFlag() == 1)
            {
                LogUtils.printLog(1, 3, TAG, "--->>> service lock is shown, stop dvb player.");
                // set the service lock.
                serviceLock = true;
                if (null == passwordDialog)
                {
                    passwordDialog =
                        DialogUtils.passwordDialogCreate(this, getString(R.string.password_stb), handler, false);
                }
                return;
            }
        }
        
        /*
         * the eit category lock.
         */
        TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
        short serviceid = serviceBean.getServiceId().shortValue();
        short tsid = tpInfoBean.getTranStreamId().shortValue();
        short ornetowrkid = tpInfoBean.getOriginalNetId().shortValue();
        EpgEventInfo firstEvent = nativeEpg.new EpgEventInfo();
        nativeEpg.GetEventInfoByIndex((short)serviceId,
            tsid,
            ornetowrkid,
            0,
            1,
            firstEvent,
            null);
        category = firstEvent.getcontentnibble();
        if(eitLimitcategorys.contains(String.valueOf(category)))
        {
            LogUtils.printLog(1, 3, TAG, "--->>> eit level lock is shown, stop dvb player.");
            //set the eit category lock.
            eitCategoryLock = true;
            if(null == passwordDialog)
            {
                passwordDialog = DialogUtils.passwordDialogCreate(this,CommonUtils.getNitCategory(this, category),handler,false);
            }
            return;
        }
        
        /*
         * the eit level lock.
         */
        peventRate grade = nativeEpg.new peventRate();
        nativeEpg.GetRate(serviceid, tsid, ornetowrkid, grade);
        currentGrade = CommonUtils.getEpgGrade(grade.getRate());
        
        if (currentGrade > originalGrade)
        {
            LogUtils.printLog(1, 3, TAG, "--->>> eit level lock is shown, stop dvb player.");
            // set the eit level lock.
            eitLevelLock = true;
            if(null == passwordDialog)
            {
                passwordDialog = DialogUtils.passwordDialogCreate(this,getString(R.string.viewing_level_control),handler,false);
            }
            return;
    

        }
        else
        {
            Log.i(TAG, "-----DVBPlayer----Start----->>>" + channelNum);
            nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
        }
        
    }
    
    /**
     * 显示CA消息
     */
    public void showCaMessageToast(String toastMsg)
    {
        if (caMessageToast == null)
        {
            caMessageToast = Toast.makeText(getApplicationContext(), "", 0);
            caMessageToast.setGravity(Gravity.CENTER, 400, -85);
        }
        caMessageToast.setText(toastMsg);
        caMessageToast.show();
    }
    
    private void upKeyMethod()
    {
        currentPosition--;
        if (currentPosition < 0)
        {
            currentPosition = listViewize - 1;
        }
        int index = (currentPageNum - 1) * listViewize + currentPosition;
        playChanName = serviceInfoBeans.get(index).getChannelName();
        channelNum = serviceInfoBeans.get(index).getChannelNumber();
        curChanName.setText(playChanName);
        serviceList.setSelection(currentPosition);
        
        handler.removeCallbacks(playTask);
        handler.post(playTask);
    }
    
    private void downKeyMethod()
    {
        currentPosition++;
        if (currentPosition >= listViewize)
        {
            currentPosition = 0;
        }
        int index = (currentPageNum - 1) * listViewize + currentPosition;
        playChanName = serviceInfoBeans.get(index).getChannelName();
        channelNum = serviceInfoBeans.get(index).getChannelNumber();
        curChanName.setText(playChanName);
        serviceList.setSelection(currentPosition);
        
        handler.removeCallbacks(playTask);
        handler.post(playTask);
    }
    
    private void dismissDialogs()
    {
        if (null != passwordDialog)
        {
            passwordDialog.dismiss();
            passwordDialog = null;
            return;
        }
    }
    
    public class ServiceOnItemSelectedListener implements OnItemSelectedListener
    {
        
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            currentPosition = position;
            int index = (currentPageNum - 1) * listViewize + currentPosition;
            playChanName = serviceInfoBeans.get(index).getChannelName();
            channelNum = serviceInfoBeans.get(index).getChannelNumber();
            curChanName.setText(playChanName);
            
            handler.removeCallbacks(playTask);
            handler.post(playTask);
        }
        
        public void onNothingSelected(AdapterView<?> arg0)
        {
        }
        
    }
    
    @SuppressWarnings("unchecked")
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        if (KeyEvent.ACTION_DOWN == event.getAction())
        {
            if (event.getKeyCode() == Config.PAGE_UP)
            {
                // 向上翻页键keyCode
                focusInServiceList();
                
                if (pd.getCurrentPage() != 1)
                {
                    pd.prePage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    currentPageNum--;
                    
                    serviceList.setAdapter(serviceAdapter);
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                else
                {
                    pd.lastPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    currentPageNum = pd.getPageNum();
                    
                    serviceList.setAdapter(serviceAdapter);
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                
            }
            else if (event.getKeyCode() == Config.PAGE_DOWN)
            {// 向下翻页键
                focusInServiceList();
                
                if (pd.getCurrentPage() != pd.getPageNum())
                {
                    pd.nextPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    currentPageNum++;
                    
                    serviceList.setAdapter(serviceAdapter);
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                else
                {
                    pd.headPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    currentPageNum = 1;
                    
                    serviceList.setAdapter(serviceAdapter);
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                
            }
            else if (event.getKeyCode() == Config.KEY_REC)
            {
                // 预约录制
                if (eventList.hasFocus())
                {
                    View eventView = eventList.getSelectedView();
                    if (eventView != null)
                    {
                        String time = ((TextView)eventView.findViewById(R.id.event_time)).getText().toString();
                        String times[] = time.split("-");
                        long lstart = PvrUtils.convertFromHM(times[0]);
                        long lend = PvrUtils.convertFromHM(times[1]);
                        if (lend < lstart)
                        {
                            lend = lend + PvrUtils.convertFromHM("23:59");
                        }
                        String eventYMD = ((TextView)eventView.findViewById(R.id.event_ymd)).getText().toString();
                        
                        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
                        
                        Intent intent = new Intent(this, PvrOrderInfos.class);
                        intent.putExtra("flag", "epg_pvr");
                        intent.putExtra("serviceId", serviceBean.getServiceId());
                        intent.putExtra("logicalNum", serviceBean.getLogicalNumber());
                        intent.putExtra("startTime", times[0]);
                        intent.putExtra("length", PvrUtils.getHMFromLong(lend - lstart));
                        intent.putExtra("eventYMD", eventYMD);
                        startActivity(intent);
                        
                        // nativePlayer.DVBPlayerDeinit();
                    }
                    
                }
            }
        }
        return super.dispatchKeyEvent(event);
    }
    
    @SuppressWarnings("unchecked")
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        LogUtils.e(TAG, "EitActivity.onKeyDown:: keyCode = " + keyCode);
        
        if (keyCode == Config.KEY_BLUE)// BLUE_KEY
        {
            LogUtils.e(TAG, "EitActivity.onKeyDown:: serviceType[int] = " + (int)serviceType);
            
            epg_Show = true;
            
            Intent intent = new Intent();
            
            Bundle bundle = new Bundle();
            
            bundle.putChar(Config.DVB_SERVICE_TYPE, serviceType);
            
            intent.putExtras(bundle);
            
            Uri uri = Uri.parse("dvb:" + Config.DVB_EPG + "");
            
            intent.setData(uri);
            
            intent.setClassName("com.pbi.dvb", "com.pbi.dvb.SplashActivity");
            
            startActivity(intent);
          
            return true;
        }
        
        if (keyCode == Config.KEY_SHOW)// 显示键
        {
//            for (int i = 0; i < 3; i++)
//            {
//                CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
//            }
        }
        
        if (keyCode == Config.KEY_RED)// RED KEY
        {
            if (null != gv_EitCategoryHeader && gv_EitCategoryHeader.hasFocus())
            {
                focusInServiceList();
                weekday = 0;
                serviceListGetFocus();
            }
            else if (null != gv_EitCategoryHeader)
            {
                eitCategoryHeaderGetFocus();
            }
        }
        
        // Log.i(TAG, "******* onKeyDown() ******* , keyCode = " + keyCode);
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            // 返回键
            if (caMessageToast != null)
            {
                caMessageToast = null;
            }
            // nativePlayer.DVBPlayerDeinit();
            finish();
            // System.exit(0);
        }
        /*-------------Press the volume down key-----------------*/
        if (keyCode == Config.KEY_MUTE && event.getRepeatCount() == 0)
        {
            g_bAudioIsMute = !g_bAudioIsMute;
            if (true == g_bAudioIsMute)
            {
                nativePlayer.DVBPlayerAVCtrlAudMute(1);
            }
            else
            {
                nativePlayer.DVBPlayerAVCtrlAudMute(0);
            }
        }
        /*-------------Press the volume down key-----------------*/
        if (keyCode == Config.KEY_VOLUME_DOWN && event.getRepeatCount() == 0)
        {
            int volume = 0;
            
            volume = audioManager.getStreamVolume(audioManager.STREAM_SYSTEM);
            System.out.println("GetStreamVolume =" + volume);
            
            if (0 != volume)
            {
                volume -= 1;
            }
            
            volume *= 7;
            nativePlayer.DVBPlayerAVCtrlSetVolume(volume);
            nativePlayer.DVBPlayerAVCtrlAudMute(0);
            g_bAudioIsMute = false;
            
        }
        /*-------------Press the volume up key-----------------*/
        if (keyCode == Config.KEY_VOLUME_UP && event.getRepeatCount() == 0)
        {
            int volume = 0;
            volume = audioManager.getStreamVolume(audioManager.STREAM_SYSTEM);
            System.out.println("GetStreamVolume =" + volume);
            
            if (15 != volume)
            {
                volume += 1;
            }
            
            volume *= 7;
            if (volume >= 100)
            {
                volume = 100;
            }
            nativePlayer.DVBPlayerAVCtrlSetVolume(volume);
            nativePlayer.DVBPlayerAVCtrlAudMute(0);
            g_bAudioIsMute = false;
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_UP && event.getRepeatCount() == 0)
        {// 上
            if (serviceList.hasFocus())
            {
                Log.i(TAG, "---------up Clicked : serviceList.hasFocus() --");
                focusInServiceList();
                serviceList.setNextFocusUpId(R.id.service_list);
                --flag;
                serviceAdapter = new EpgServiceAdapter(this);
                if (pd.getCurrentPage() == 1 && flag < 1)
                {
                    pd.lastPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    serviceAdapter.notifyDataSetChanged();
                    currentPageNum = pd.getPageNum();
                    flag = 1;
                    serviceList.setAdapter(serviceAdapter);
                    currentPosition = currentList.size() - 1;
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                if (flag < 1)
                {
                    pd.prePage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    serviceAdapter.notifyDataSetChanged();
                    flag = 1;
                    currentPageNum--;
                    serviceList.setAdapter(serviceAdapter);
                    currentPosition = listViewize - 1;
                    serviceList.setSelection(currentPosition);
                    serviceList.requestFocus(currentPosition);
                }
                serviceList.invalidate();
            }
            else if (eventList.hasFocus())
            {
                Log.i(TAG, "---------up Clicked : eventList --> radioGroup --");
                focusInRadioGroup();
                
                switch (weekday)
                {
                    case 0:
                        eventList.setNextFocusUpId(R.id.radiobn_0);
                        break;
                    case 1:
                        eventList.setNextFocusUpId(R.id.radiobn_1);
                        break;
                    case 2:
                        eventList.setNextFocusUpId(R.id.radiobn_2);
                        break;
                    case 3:
                        eventList.setNextFocusUpId(R.id.radiobn_3);
                        break;
                    case 4:
                        eventList.setNextFocusUpId(R.id.radiobn_4);
                        break;
                    case 5:
                        eventList.setNextFocusUpId(R.id.radiobn_5);
                        break;
                    case 6:
                        eventList.setNextFocusUpId(R.id.radiobn_6);
                        break;
                }
                
            }
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN && event.getRepeatCount() == 0)
        {
            // 下
            if (serviceList.hasFocus())
            {
                Log.i(TAG, "---------down Clicked ：serviceList.hasFocus() -----------");
                focusInServiceList();
                ++flag;
                if (pd.getCurrentPage() == pd.getPageNum() && flag > 1)
                {
                    pd.headPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    serviceAdapter.notifyDataSetChanged();
                    serviceList.setSelection(1);
                    serviceList.requestFocus(1);
                    flag = 1;
                    currentPageNum = 1;
                }
                if (flag > 1)
                {
                    pd.nextPage();
                    currentList = (List<ServiceInfoBean>)pd.getCurrentList();
                    serviceAdapter.setTvProgramList(currentList);
                    serviceAdapter.notifyDataSetChanged();
                    flag = 1;
                    currentPageNum++;
                    serviceList.setSelection(1);
                    serviceList.requestFocus(1);
                }
                serviceList.setAdapter(serviceAdapter);
                serviceList.invalidate();
            }
            else if (gv_EitCategoryHeader.hasFocus())
            {
                radioBnSetBG();
                if (epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------down Clicked : radioGroup --> serviceList --");
                    focusInServiceList();
                    serviceListGetFocus();
                    return true;
                }
                else
                {
                    Log.i(TAG, "---------down Clicked : radioGroup --> eventList --");
                    focusInEventList();
                    eventListGetFocus();
                }
            }
            
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT)
        {
            // 左
            // if (radioGroup.hasFocus())
            // {
            // weekday = weekday - 1;
            // if (weekday < 0)
            // {
            // Log.i(TAG, "---------left Clicked : weekday == 0 -----------");
            // focusInServiceList();
            // weekday = 0;
            // serviceListGetFocus();
            // }
            // else
            // {
            // Log.i(TAG, "---------left Clicked : weekday > 0 -----------");
            // focusInRadioGroup();
            // }
            // radioBnSetBG();
            // refreshEventInfo();
            // }
            // else if (eventList.hasFocus())
            if (eventList.hasFocus())
            {
                Log.i(TAG, "---------left Clicked : eventList --> serviceList -----------");
                focusInServiceList();
                serviceListGetFocus();
            }
            serviceList.invalidate();
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT)
        {
            // 右
            // if (radioGroup.hasFocus())
            // {
            // Log.i(TAG, "---------right Clicked : radioGroup --> radioGroup ------------");
            // focusInRadioGroup();
            // weekday = weekday == 6 ? 6 : (weekday + 1);
            // radioBnSetBG();
            // refreshEventInfo();
            // }
            // else if (serviceList.hasFocus())
            if (serviceList.hasFocus())
            {
                serviceListLostFocus();
                if (epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------right Clicked : serviceList --> radioGroup ------------");
                    // focusInRadioGroup();
                    // serviceList.setNextFocusRightId(R.id.gv_EitCategoryHeader);
                    // eitCategoryHeaderGetFocus();
                    // radioBnSetBG();
                    // refreshPlayingAndEvent();
                    eitCategoryHeaderGetFocus();
                    
                    return true;
                }
                else if (!epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------right Clicked : serviceList --> eventList ------------");
                    focusInEventList();
                    serviceList.setNextFocusRightId(R.id.event_list);
                    eventListGetFocus();
                }
            }
            serviceList.invalidate();
        }
     
        
        return false;
    }
    
    private void eitCategoryHeaderGetFocus()
    {
        gv_EitCategoryHeader.requestFocus();
    }
    
    public void serviceListLostFocus()
    {
        serviceList.getChildAt(currentPosition).setBackgroundResource(R.drawable.select_s);
        ((TextView)serviceList.getChildAt(currentPosition).findViewById(R.id.epg_service)).setTextColor(black);
    }
    
    @SuppressWarnings("unchecked")
    public void serviceListGetFocus()
    {
        View view = serviceList.getChildAt(currentPosition);
        
        if (null != view)
        {
            view.setBackgroundDrawable(null);
            
            ((TextView)view.findViewById(R.id.epg_service)).setTextColor(white);
        }
        
        currentList = (List<ServiceInfoBean>)pd.getCurrentList();
        serviceAdapter.setTvProgramList(currentList);
        serviceList.setAdapter(serviceAdapter);
        
        serviceList.setSelection(currentPosition);
        serviceList.requestFocus(currentPosition);
    }
    
    public void eventListGetFocus()
    {
        Log.i(TAG, "### eventListGetFocus() ######");
        eventList.setSelection(0);
        eventList.requestFocus(0);
    }
    
    /**
     * 修改RadioButton的背景
     */
    public void radioBnSetBG()
    {
        // weekdayBn.setBackgroundDrawable(null);
        // weekdayBn.setTextColor(white);
        //
        // switch (weekday)
        // {
        // case 0:
        // radioBn0.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn0.setTextColor(black);
        // weekdayBn = radioBn0;
        // break;
        // case 1:
        // radioBn1.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn1.setTextColor(black);
        // weekdayBn = radioBn1;
        // break;
        // case 2:
        // radioBn2.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn2.setTextColor(black);
        // weekdayBn = radioBn2;
        // break;
        // case 3:
        // radioBn3.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn3.setTextColor(black);
        // weekdayBn = radioBn3;
        // break;
        // case 4:
        // radioBn4.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn4.setTextColor(black);
        // weekdayBn = radioBn4;
        // break;
        // case 5:
        // radioBn5.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn5.setTextColor(black);
        // weekdayBn = radioBn5;
        // break;
        // case 6:
        // radioBn6.setBackgroundResource(R.drawable.div_l_focus);
        // radioBn6.setTextColor(black);
        // weekdayBn = radioBn6;
        // break;
        // default:
        // break;
        // }
    }
    
    public void focusInServiceList()
    {
        // 选择节目、切换焦点
        serviceImage.setVisibility(View.VISIBLE);
        serviceText.setVisibility(View.VISIBLE);
        
        eventImage.setVisibility(View.GONE);
        eventText.setVisibility(View.GONE);
        
        focusImage.setVisibility(View.VISIBLE);
        focusText.setVisibility(View.VISIBLE);
        
        weekdayImage.setVisibility(View.GONE);
        weekdayText.setVisibility(View.GONE);
        
        PvrOrderImage.setVisibility(View.GONE);
        PvrOrderText.setVisibility(View.GONE);
    }
    
    public void focusInEventList()
    {
        // 选择节目，预约
        serviceImage.setVisibility(View.GONE);
        serviceText.setVisibility(View.GONE);
        
        eventImage.setVisibility(View.VISIBLE);
        eventText.setVisibility(View.VISIBLE);
        
        focusImage.setVisibility(View.VISIBLE);
        focusText.setVisibility(View.VISIBLE);
        
        weekdayImage.setVisibility(View.GONE);
        weekdayText.setVisibility(View.GONE);
        
        PvrOrderImage.setVisibility(View.VISIBLE);
        PvrOrderText.setVisibility(View.VISIBLE);
    }
    
    public void focusInRadioGroup()
    {
        // 切换EPG日期
        serviceImage.setVisibility(View.GONE);
        serviceText.setVisibility(View.GONE);
        
        eventImage.setVisibility(View.GONE);
        eventText.setVisibility(View.GONE);
        
        focusImage.setVisibility(View.GONE);
        focusText.setVisibility(View.GONE);
        
        weekdayImage.setVisibility(View.VISIBLE);
        weekdayText.setVisibility(View.VISIBLE);
        
        PvrOrderImage.setVisibility(View.GONE);
        PvrOrderText.setVisibility(View.GONE);
    }
    
    class ChannelOnItemClickListener implements OnItemClickListener
    {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            isDVB = true;
            isIPTV = false;
            Intent intent = new Intent(EitActivity.this, TVChannelPlay.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
            intent.putExtra("ServiceType", serviceBean.getServiceType());
            intent.putExtra("ServiceId", serviceBean.getServiceId());
            intent.putExtra("LogicalNumber", serviceBean.getLogicalNumber());
            intent.putExtra("ServiceName", serviceBean.getChannelName());
            
            LogUtils.printLog(1, 3, TAG, "--->>>ServiceId : " + serviceBean.getServiceId() + " LogicalNumber: "
                + serviceBean.getLogicalNumber() + " ServiceName : " + serviceBean.getChannelName());
            startActivity(intent);
            finish();
        }
    }
    
    /* Start LinKang, 2013-12-21, Add the eit category... */
    private class EitCategoryAdapter extends BaseAdapter
    {
        @Override
        public int getCount()
        {
            return null == eit_Type_Names ? 0 : eit_Type_Names.length;
        }
        
        @Override
        public Object getItem(int position)
        {
            return eit_Type_Names[position];
        }
        
        @Override
        public long getItemId(int position)
        {
            return position;
        }
        
        @Override
        public View getView(int position, View convertView, ViewGroup parent)
        {
            View view = convertView;
            
            ViewHolder vHolder = null;
            
            if (null == view)
            {
                view = View.inflate(EitActivity.this, R.layout.eit_main_category_item, null);
            }
            else
            {
                vHolder = (ViewHolder)view.getTag();
            }
            
            if (null == vHolder)
            {
                vHolder = new ViewHolder();
                
                vHolder.bt_EitCagegory = (TextView)view.findViewById(R.id.bt_EitCagegory);
                
                vHolder.bt_EitCagegory.setTag(position);
                
                view.setTag(vHolder);
            }
            
            vHolder.bt_EitCagegory.setText(eit_Type_Names[position]);
            
            return view;
        }
    }
    
    private static class ViewHolder
    {
        public TextView bt_EitCagegory = null;
    }
    
    private class EitCategoryOnItemSelectedLinstener implements OnItemSelectedListener
    {
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            eit_Type = position + 1;
            
            // refreshPlayingAndEvent();
            
            refreshEventList();
        }
        
        @Override
        public void onNothingSelected(AdapterView<?> parent)
        {
            LogUtils.e(TAG, "EitCategoryOnItemSelectedLinstener.onNothingSelected run...");
        }
    }
    
    private class EitCategoryOnFocusChangeListener implements OnFocusChangeListener
    {
        @Override
        public void onFocusChange(View v, boolean hasFocus)
        {
            ViewHolder vHolder = (ViewHolder)v.getTag();
            
            if (!hasFocus)
            {
                clearGridViewPosition(v);
            }
        }
    }
    
    private void clearGridViewPosition(View v)
    {
        try
        {
            @SuppressWarnings("unchecked")
            Class<GridView> clazz = (Class<GridView>)Class.forName("android.widget.GridView");
            
            Method[] fields = clazz.getDeclaredMethods();
            
            for (Method field : fields)
            {
                if ("setSelectionInt".equals(field.getName()))
                {
                    field.setAccessible(true);
                    
                    field.invoke(v, new Object[] {Integer.valueOf(-1)});
                }
            }
        }
        catch (ClassNotFoundException e)
        {
            e.printStackTrace();
        }
        catch (IllegalAccessException e)
        {
            e.printStackTrace();
        }
        catch (InvocationTargetException e)
        {
            e.printStackTrace();
        }
    }
    /* End LinKang, 2013-12-21, Add the eit category... */
}
