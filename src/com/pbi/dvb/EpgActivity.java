package com.pbi.dvb;

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
import android.content.ServiceConnection;
import android.content.res.ColorStateList;
import android.graphics.Canvas;
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
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
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

public class EpgActivity extends Activity
{
    protected static final int CLEAR_REC_TAG = 0;
    
    protected static final int SHOW_REC_TAG = 1;
    
    /* 2014-02-19 gtsong add dvb locks start. */
    public static final int PROCESS_LOCKS = 200;
    
    private boolean firstTimeServiceLock = true;
    
    private boolean serviceLock = false;
    
    private boolean eitLevelLock = false;
    
    private boolean eitCategoryLock = false;
    
    private boolean caAdultLock = false;
    
    // private String superPwd;
    private int originalGrade;
    
    private int currentGrade;
    
    private int category;
    
    private List<String> eitLimitcategorys;
    
    private Dialog passwordDialog;
    
    /* 2014-02-19 gtsong add dvb locks end. */
    
    private String TAG = "EpgActivity";
    
    // service:频道信息
    private Runnable playTask;
    private ListView serviceList;
    private TextView tvServiceTitle;
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
    
    private SimpleAdapter eventAdapter;
    
    private List<NativeEpg.EpgEventInfo> epgEventInfoBeans;
    
    private int currentSelected = -1;
    
    // 正在播放的节目名字、时长、进度信息
    private TextView curPlayServiceName;
    
    private TextView curPlayServiceTimelength;
    
    private ProgressBar curPlayServiceProgress;
    
    private SurfaceView surfaceView;
    
    private SurfaceHolder surfHolder;
    
    private TextView curChanName;
    
    // 7天显示控件
    private RadioGroup radioGroup;
    
    private RadioButton radioBn0;
    
    private RadioButton radioBn1;
    
    private RadioButton radioBn2;
    
    private RadioButton radioBn3;
    
    private RadioButton radioBn4;
    
    private RadioButton radioBn5;
    
    private RadioButton radioBn6;
    
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
    
    private int weekday; // 星期，默认取"今天"
    
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
    
    private ImageView playOrderImage;
    
    private TextView serviceText;
    
    private TextView eventText;
    
    private TextView focusText;
    
    private TextView weekdayText;
    
    private TextView PvrOrderText;
    
    private TextView playOrderText;
    
    // 设置颜色
    private ColorStateList black;
    
    private ColorStateList white;
    
    // event节目级别，CA卡成人级别，盒子(亲子锁和节目锁)密码
    
    private char serviceType = Config.SERVICE_TYPE_TV;
    
    private Toast caMessageToast;
    
    // Record if the EitActivity is going to be shown...
    private boolean eit_Show = false;
    
    @Override
    protected void onResume()
    {
        super.onResume();
        
        /* Start LinKang, 2013-12-19, Switch the left and right key to control direction... */
        LogUtils.e(TAG, "EpgActivity.onResume:: switch key to left&right control...");
        
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
                            DialogUtils.passwordDialogCreate(EpgActivity.this, getString(R.string.E13), handler, true);
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
                                    DialogUtils.passwordDialogCreate(EpgActivity.this,
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
        private EventListAdapter eventListAdapter;
        
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case PROCESS_LOCKS:
                    if (null != passwordDialog)
                    {
                        passwordDialog.dismiss();
                        passwordDialog = null;
                    }
                    // ca adult lock.
                    if (caAdultLock)
                    {
                        NativeCA nativeCA = new NativeCA();
                        int childPwd = Integer.parseInt((String)msg.obj);
                        nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PARENTAL_CHANGE_QUERY, null, childPwd);
                        return;
                    }
                    // service lock.
                    if (serviceLock)
                    {
                        firstTimeServiceLock = false;
                        serviceLock = false;
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        return;
                    }
                    // eit category lock.
                    if (eitCategoryLock)
                    {
                        eitCategoryLock = false;
                        eitLimitcategorys.remove(String.valueOf(category));
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        return;
                    }
                    if (eitLevelLock)
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
                    Bundle bundle2 = (Bundle)msg.obj;
                    int epglength = bundle2.getInt("epglength");
                    
                    List<HashMap<String, Object>> data = getEventData();
                    
                    // eventAdapter =
                    // new SimpleAdapter(EpgActivity.this, data, R.layout.epg_event_item, new String[] {"eventImage",
                    // "eventRate", "eventTime", "eventName", "event_Pvr_Status", "eventYMD"}, new int[] {
                    // R.id.event_image, R.id.event_rate, R.id.event_time, R.id.event_name, R.id.event_Pvr_Status,
                    // R.id.event_ymd});
                    
                    eventListAdapter = new EventListAdapter(data);
                    
                    eventList.setAdapter(eventListAdapter);
                    eventList.setOnItemClickListener(new ChannelOnItemClickListener());
                    eventList.setOnItemSelectedListener(new ChannelOnItemSelectedListener());
                    eventList.invalidate();
                    
                    break;
                
                case Config.UPDAE_EVENT_PVR_STATUS:
                {
                    refreshPvrTasks();
                    
                    // eventList.invalidate();
                    // eventList.invalidateViews();
                    eventListAdapter.notifyDataSetChanged();
                    
                    break;
                }
                
                case CLEAR_REC_TAG:
                {
                    rl_rec_status.setVisibility(View.INVISIBLE);
                    
                    break;
                }
                
                case SHOW_REC_TAG:
                {
                    rl_rec_status.setVisibility(View.VISIBLE);
                    
                    break;
                }
            }
        }
    };
    
    private class EventListAdapter extends BaseAdapter
    {
        
        private List<HashMap<String, Object>> data;
        
        public EventListAdapter(List<HashMap<String, Object>> data)
        {
            super();
            
            this.data = data;
        }
        
        @Override
        public int getCount()
        {
            return data == null ? 0 : data.size();
        }
        
        @Override
        public Object getItem(int position)
        {
            return data == null ? null : data.get(position);
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
            
            EventListViewHolder vHolder = null;
            
            if (null == view)
            {
                view = View.inflate(EpgActivity.this, R.layout.epg_event_item, null);
            }
            
            vHolder = (EventListViewHolder)view.getTag();
            
            if (null == vHolder)
            {
                vHolder = new EventListViewHolder();
                
                vHolder.event_image = (ImageView)view.findViewById(R.id.event_image);
                vHolder.event_rate = (TextView)view.findViewById(R.id.event_rate);
                vHolder.event_time = (TextView)view.findViewById(R.id.event_time);
                vHolder.event_name = (TextView)view.findViewById(R.id.event_name);
                vHolder.event_Pvr_Status = (ImageView)view.findViewById(R.id.event_Pvr_Status);
                vHolder.event_ymd = (TextView)view.findViewById(R.id.event_ymd);
                
                view.setTag(vHolder);
            }
            
            HashMap<String, Object> item = data.get(position);
            
            vHolder.event_image.setImageResource((Integer)item.get("eventImage"));
            vHolder.event_rate.setText((String)item.get("eventRate"));
            vHolder.event_time.setText((String)item.get("eventTime"));
            vHolder.event_name.setText((String)item.get("eventName"));
            vHolder.event_Pvr_Status.setImageDrawable(null);
            vHolder.event_ymd.setText((String)item.get("eventYMD"));
            
            PvrDao pvrDao = new PvrDaoImpl(EpgActivity.this);
            
            int service_ID = (Integer)item.get("service_ID");
            long event_Date = (Long)item.get("event_date");
            long lstart = (Long)item.get("lstart");
            long lstop = (Long)item.get("lstop");
            
            // List<Integer> ids = pvrDao.checkTaskExists(service_ID, event_Date, lstart, lstop);
            
            PvrBean existsTask = checkTaskExists(service_ID, event_Date, lstart, lstop);
            
            LogUtils.e(TAG, "EpgActivity.EventListAdapter.getView:: taskExists = "
                + (existsTask == null ? "null" : "notNull") + " eventTime = " + ((String)item.get("eventTime")));
            
            if (null != existsTask)
            {
                // item.put("event_Pvr_Status", getResources().getString(R.string.pvr_task_exists));
                
                int taskType = existsTask.getPvrWakeMode() == null ? -1 : existsTask.getPvrWakeMode();
                
                switch (taskType)
                {
                    case DBPvr.PLAY_WAKEUP_MODE:
                    {
                        vHolder.event_Pvr_Status.setImageResource(R.drawable.epg_pvr_play);
                        
                        break;
                    }
                    case DBPvr.RECORD_WAKEUP_MODE:
                    {
                        vHolder.event_Pvr_Status.setImageResource(R.drawable.epg_pvr_rec);
                        
                        break;
                    }
                    default:
                    {
                        LogUtils.e(TAG, "EpgActivity.EventListAdapter.getView:: unknown pvr type[" + taskType + "]");
                    }
                }
                
            }
            
            // eventAdapter =
            // new SimpleAdapter(EpgActivity.this, data, R.layout.epg_event_item, new String[] {"eventImage",
            // "eventRate", "eventTime", "eventName", "event_Pvr_Status", "eventYMD"}, new int[] {
            // R.id.event_image, R.id.event_rate, R.id.event_time, R.id.event_name, R.id.event_Pvr_Status,
            // R.id.event_ymd});
            
            return view;
        }
    }
    
    private class EventListViewHolder
    {
        public ImageView event_image = null;
        
        public TextView event_rate = null;
        
        public TextView event_time = null;
        
        public TextView event_name = null;
        
        public ImageView event_Pvr_Status = null;
        
        public TextView event_ymd = null;
        
    }
    
    private PopupWindow epgDetailPopUpWindow;
    
    private EpgDetailDialog epgDetailDialog;
    
    private List<PvrBean> pvrTasks = new ArrayList<PvrBean>();
    
    private EpgServiceConnection conn;
    
    private BroadcastReceiver recMonitor;
    
    private RelativeLayout rl_rec_status;
    
    public synchronized List<HashMap<String, Object>> getEventData()
    {
        PvrDao pvrDao = new PvrDaoImpl(EpgActivity.this);
        
        refreshPvrTasks();
        
        List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();
        for (Iterator<NativeEpg.EpgEventInfo> itr = epgEventInfoBeans.iterator(); itr.hasNext();)
        {
            NativeEpg.EpgEventInfo eventInfo = itr.next();
            
            String epgstartHour = timeFormat(eventInfo.getstarthour() + "");
            String epgstartMinute = timeFormat(eventInfo.getstartminute() + "");
            
            String epgendHour = timeFormat(eventInfo.getendhour() + "");
            String epgendMinute = timeFormat(eventInfo.getendminute() + "");
            
            // LogUtils.e(TAG, "EpgActivity.getEventData:: epgstartHour:epgstartMinute = " + epgstartHour + ":"
            // + epgstartMinute + "epgendHour:epgendMinute = " + epgendHour + ":" + epgendMinute);
            
            long lstart = PvrUtils.convertFromHM(epgstartHour + ":" + epgstartMinute);
            long lend = PvrUtils.convertFromHM(epgendHour + ":" + epgendMinute);
            
            // LogUtils.e(TAG, "EpgActivity.getEventData:: lstart = " + lstart + " lstop = " + lend);
            
            ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
            
            String yearStr = eventInfo.getstartyear() + "";
            String monthStr = eventInfo.getstartmonth() + "";
            String dayStr = eventInfo.getstartdate() + "";
            
            String eventYMD = yearStr + "-" + monthStr + "-" + dayStr;
            
            long eventDate = PvrUtils.convertFromYMD(eventYMD);
            
            // List<Integer> tasks = pvrDao.checkTaskExists(serviceBean.getServiceId(), eventDate, lstart, lend);
            
            HashMap<String, Object> item = new HashMap<String, Object>();
            
            item.put("service_ID", serviceBean.getServiceId());
            item.put("event_date", eventDate);
            item.put("lstart", lstart);
            item.put("lstop", lend);
            
            // if (tasks != null && tasks.size() > 0)
            // {
            // // item.put("event_Pvr_Status", getResources().getString(R.string.pvr_task_exists));
            // item.put("event_Pvr_Status", R.drawable.icon);
            // }
            // else
            // {
            // item.put("event_Pvr_Status", null);
            // }
            
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
    
    private void refreshPvrTasks()
    {
        synchronized (pvrTasks)
        {
            pvrTasks.clear();
            
            PvrDao pvrDao = new PvrDaoImpl(EpgActivity.this);
            
            List<PvrBean> tasks = pvrDao.get_All_Illegal_Tasks();
            
            if (null != tasks)
            {
                pvrTasks.addAll(tasks);
            }
        }
    }
    
    private PvrBean checkTaskExists(final int serviceId, final long setDate, final long lstart, final long lend)
    {
        synchronized (pvrTasks)
        {
            LogUtils.e(TAG, "EpgActivity.checkTaskExists:: serviceId = " + serviceId + " setDate = " + setDate
                + " lstart = " + lstart + " lend = " + lend);
            
            PvrBean existsTask = null;
            
            PvrBean task = null;
            
            for (int i = 0; null != pvrTasks && i < pvrTasks.size(); i++)
            {
                task = pvrTasks.get(i);
                
                if (null == task)
                {
                    continue;
                }
                
                if (task.getPvrMode() == DBPvr.CLOSE_MODE)
                {
                    continue;
                }
                
                int taskType =
                    task.getPvr_type_tag() == null ? DBPvr.TASK_DEFAULT_START_WITH_DIALOG : task.getPvr_type_tag();
                
                int serviceIdInDB = (task.getServiceId() == null ? -1 : task.getServiceId());
                
                long lstartInDB = (task.getPvrStartTime() == null ? 0 : task.getPvrStartTime());
                
                long taskLengthInDB = (task.getPvrRecordLength() == null ? 0 : task.getPvrRecordLength());
                
                long adjustLend = lend;
                
                if (null != task.getPvrWakeMode() && task.getPvrWakeMode() == DBPvr.PLAY_WAKEUP_MODE)
                {
                    LogUtils.e(TAG, "");
                    
                    adjustLend = lstart;
                }
                
                LogUtils.e(TAG, "EpgActivity.checkTaskExists:: wakeUpmode = "
                    + (task.getPvrWakeMode() == DBPvr.PLAY_WAKEUP_MODE ? "playMode" : "recordMode") + " taskType = "
                    + taskType + " serviceId = " + serviceIdInDB + " lstartInDB = " + lstartInDB + " lStopFromDB = "
                    + (lstartInDB + taskLengthInDB) + " lendAfterAdjust = " + adjustLend);
                
                if (taskType != DBPvr.TASK_EPG_START_WITHOUT_DIALOG && serviceId == serviceIdInDB
                    && lstart == lstartInDB && (adjustLend == (lstartInDB + taskLengthInDB)))
                {
                    switch (task.getPvrMode())
                    {
                        case DBPvr.DO_ONCE_MODE:
                        {
                            if (setDate == task.getPvrSetDate())
                            {
                                existsTask = task;
                            }
                            
                            break;
                        }
                        
                        case DBPvr.EVERY_DAY_MODE:
                        {
                            existsTask = task;
                            
                            break;
                        }
                        
                        case DBPvr.EVERY_WEEK_MODE:
                        {
                            Calendar cal = Calendar.getInstance();
                            
                            cal.setTimeInMillis(task.getPvrSetDate());
                            
                            int weekDayInDB = cal.get(Calendar.DAY_OF_WEEK);
                            
                            cal.setTimeInMillis(setDate);
                            
                            int weekDayOfTask = cal.get(Calendar.DAY_OF_WEEK);
                            
                            if (weekDayInDB == weekDayOfTask)
                            {
                                existsTask = task;
                            }
                            
                            break;
                        }
                        
                        case DBPvr.EVERY_WORKDAY_MODE:
                        {
                            Calendar cal = Calendar.getInstance();
                            
                            cal.setTimeInMillis(setDate);
                            
                            int weekDayOfTask = cal.get(Calendar.DAY_OF_WEEK);
                            
                            if (weekDayOfTask > 0 && weekDayOfTask < 6)
                            {
                                existsTask = task;
                            }
                            
                            break;
                        }
                        
                        case DBPvr.CLOSE_MODE:
                        {
                            existsTask = task;
                            
                            break;
                        }
                    }
                    
                    if (null != existsTask)
                    {
                        break;
                    }
                }
                else
                {
                    task = null;
                    
                    continue;
                }
            }
            
            LogUtils.e(TAG, "EpgActivity.checkTaskExists:: checkResult = " + existsTask);
            
            return existsTask;
        }
    }
    
    protected void onCreate(Bundle savedInstanceState)
    {
        Log.i(TAG, "onCreate() : ======================= ");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.epg_main);
        
        Bundle data = this.getIntent().getExtras();
        serviceId = data.getInt("ServiceId", 0);
        logicalNumber = data.getInt("LogicalNumber");
        serviceType = data.getChar(Config.DVB_SERVICE_TYPE);
        try
        {
            originalGrade = android.provider.Settings.System.getInt(getContentResolver(), "eit_grade");
        }
        catch (Exception e)
        {
            originalGrade = 1;// default value.
            e.printStackTrace();
        }
        eitLimitcategorys = CommonUtils.getNitCategorySettings(this);
        
        LogUtils.e(TAG, "EpgActivity.onCreate:: serviceType = " + serviceType);
        
//        if (serviceType != Config.SERVICE_TYPE_TV && serviceType != Config.SERVICE_TYPE_RADIO
//            && serviceType != Config.SERVICE_TYPE_NOTYPE)
//        {
//            serviceType = Config.SERVICE_TYPE_NOTYPE;
//        }
        
        Log.i(TAG, "onCreate() : logicalNumber = " + logicalNumber);
        Log.i(TAG, "onCreate() : serviceId = " + serviceId);
        Log.i(TAG, "onCreate() : originalGrade = " + originalGrade);
        
        initWidget();
        
        /* Start LinKang, 2014-02-13, Register the receiver for monitor the recording status */
        IntentFilter recFilter = new IntentFilter();
        
        recFilter.addAction(Config.PVR_TASK_COMPLETE);
        recFilter.addAction(Config.PVR_TASK_START_RECORD);
        
        recMonitor = new BroadcastReceiver()
        {
            @Override
            public void onReceive(Context context, Intent intent)
            {
                LogUtils.e(TAG, "EpgActivity.recMonitor:: receive BroadcastReceiver action = " + intent.getAction());
                
                if (null != intent)
                {
                    String action = intent.getAction();
                    
                    if (null != action && Config.PVR_TASK_COMPLETE.equalsIgnoreCase(action))
                    {
                        handler.sendEmptyMessage(CLEAR_REC_TAG);
                    }
                    else if (null != action && Config.PVR_TASK_START_RECORD.equalsIgnoreCase(action))
                    {
                        handler.sendEmptyMessage(SHOW_REC_TAG);
                    }
                }
            }
        };
        
        registerReceiver(recMonitor, recFilter);
        /* End LinKang, 2014-02-13, Register the receiver for monitor the recording status */
        
        /* Start LinKang, 2014-02-12, Connect to record background... */
        Intent service = new Intent();
        
        service.setAction("com.pbi.dvb.service.pvrBackgroudService");
        
        conn = new EpgServiceConnection();
        
        boolean bindFlag = bindService(service, conn, Context.BIND_AUTO_CREATE);
        
        if (bindFlag)
        {
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service success");
        }
        else
        {
            LogUtils.e(TAG, "service.PvrRecordService.onStartCommand:: Bind service failed");
        }
        /* End LinKang, 2014-02-12, Connect to record background... */
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
        
        /* Start LinKang, 2014-02-13, Check if the DVB is in recording status... */
        PVR_Rec_Status_t recFile = nativePlayer.new PVR_Rec_Status_t();
        nativePlayer.DVBPlayerPvrRecGetStatus(recFile);
        int rec_status = recFile.getEnState();
        
        // The DVB is in recording status...
        if (rec_status == 2 || rec_status == 3)
        {
            handler.sendEmptyMessage(SHOW_REC_TAG);
        }
        /* End LinKang, 2014-02-13, Check if the DVB is in recording status... */
    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
        
        /*
         * Start LinKang, 2014-02-14, Move from onStop to onPause, Fix the bug that show black view twice when exit the
         * EPG page...
         */
        Log.i(TAG, "EpgActivity.onPause() : ======================= ");
        /*recovery this key switch left/right and volume up/down function.*/
        CommonUtils.forbidSwitchKeyFunction(this,true);
        if (caMessageToast != null)
        {
            caMessageToast = null;
        }
        
        if (!isDVB && !eit_Show)
        {
            nativePlayer.DVBPlayerStop();
        }
		else
		{
			nativePlayer.DVBPlayerAVCtrlSetVPathPara(1);
		}
        
        if (isIPTV && !eit_Show)
        {
            // destory the dvb player.
            nativePlayer.DVBPlayerDeinit();
            nativeDrive.DrvPlayerUnInit();
        }
        
        // nativePlayer.DVBPlayerStop();
        // nativePlayer.DVBPlayerDeinit();
        // nativeDrive.DrvPlayerUnInit();
        // finish();
        /*
         * End LinKang, 2014-02-14, Move from onStop to onPause, Fix the bug that show black view twice when exit the
         * EPG page...
         */
    }
    
    protected void onStop()
    {
        super.onStop();
        
        Log.i(TAG, "onStop() : ======================= ");
        /* Start LinKang, 2013-12-19, Switch the left and right key to control volume... */
//        if (!eit_Show)
//        {
//            LogUtils.e(TAG, "EpgActivity.onStop:: setLeftRightFunction(Config.SWITCH_TO_VOICE_CONTROL) run...");
//            
//            CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
//        }
//        else
//        {
//            LogUtils.e(TAG, "EpgActivity.onStop:: eit_Show = " + eit_Show + " do not switch the to VOICE_CONTROl");
//        }
        
        // Reset the operation record of EIT category...
        eit_Show = false;
        /* End LinKang, 2013-12-19, Switch the left and right key to control volume... */
        
        /* Start LinKang, 2014-02-13, Unregister the receiver for monitor the recording status */
        if (null != recMonitor)
        {
            try
            {
                unregisterReceiver(recMonitor);
            }
            catch (Exception e)
            {
            }
        }
        /* End LinKang, 2014-02-13, Unregister the receiver for monitor the recording status */
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
        nativeDrive = new NativeDrive(EpgActivity.this);
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

		nativePlayer.DVBPlayerAVCtrlSetVPathPara(0);
        
        // 设置小视频播放的坐标及大小
        NativePlayer.DRV_AVCtrl_VidWindow_t playWindow = nativePlayer.new DRV_AVCtrl_VidWindow_t();
        /*
         * //DisplayManager display = new DisplayManager(); Display display = getWindowManager().getDefaultDisplay();
         * Rect rect = new Rect(); display.getRectSize(rect); //rect.bottom += 48; Log.i(TAG,
         * "rect.right"+rect.right+"rect.bottom"+rect.bottom+"rect.top"+rect.top+"rect.left"+rect.left);
         */
        Rect rect = new Rect();
        rect = CommonUtils.getPlayWindow();
        playWindow.setWinX(893);
        playWindow.setWinY(73);
        playWindow.setWinWidth(340);
        playWindow.setWinHeight(167);
        nativePlayer.DVBPlayerAVCtrlSetWindows(playWindow, 0);
        
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
        /* When DVB is in recording status, show it... */
        rl_rec_status = (RelativeLayout)findViewById(R.id.rl_rec_status);
        
        black = getBaseContext().getResources().getColorStateList(R.drawable.black);
        white = getBaseContext().getResources().getColorStateList(R.drawable.white);
        
        curPlayServiceName = (TextView)findViewById(R.id.cur_service_name);
        curPlayServiceTimelength = (TextView)findViewById(R.id.cur_service_timelength);
        curPlayServiceProgress = (ProgressBar)findViewById(R.id.cur_service_progress);
        
        radioGroup = (RadioGroup)findViewById(R.id.radiogroup);
        radioBn0 = (RadioButton)findViewById(R.id.radiobn_0);
        radioBn1 = (RadioButton)findViewById(R.id.radiobn_1);
        radioBn2 = (RadioButton)findViewById(R.id.radiobn_2);
        radioBn3 = (RadioButton)findViewById(R.id.radiobn_3);
        radioBn4 = (RadioButton)findViewById(R.id.radiobn_4);
        radioBn5 = (RadioButton)findViewById(R.id.radiobn_5);
        radioBn6 = (RadioButton)findViewById(R.id.radiobn_6);
        
        initRadioBnOf7Day();
        
        tvServiceTitle = (TextView)this.findViewById(R.id.tv_service_title);
        serviceList = (ListView)findViewById(R.id.service_list);
        serviceList.setDividerHeight(0);
        eventList = (ListView)findViewById(R.id.event_list);
        
        initServiceInfos();
        
        epgEventInfoBeans = new ArrayList<NativeEpg.EpgEventInfo>();
        tpInfoDao = new TpInfoDaoImpl(EpgActivity.this);
        
        serviceImage = (ImageView)findViewById(R.id.service_image);
        eventImage = (ImageView)findViewById(R.id.event_image);
        focusImage = (ImageView)findViewById(R.id.focus_image);
        weekdayImage = (ImageView)findViewById(R.id.weekday_image);
        PvrOrderImage = (ImageView)findViewById(R.id.order_play_image);
        playOrderImage = (ImageView)findViewById(R.id.order_pvr_image);
        serviceText = (TextView)findViewById(R.id.service_text);
        eventText = (TextView)findViewById(R.id.event_text);
        focusText = (TextView)findViewById(R.id.focus_text);
        weekdayText = (TextView)findViewById(R.id.weekday_text);
        PvrOrderText = (TextView)findViewById(R.id.order_pvr_text);
        playOrderText = (TextView)findViewById(R.id.order_play_text);
        
        focusInServiceList();
        
        surfaceView = (SurfaceView)findViewById(R.id.epg_sur_view);
        surfHolder = surfaceView.getHolder();
        surfHolder.setFixedSize(surfaceView.getWidth(), surfaceView.getHeight());
        surfHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        surfHolder.addCallback(mSHCallback);
        surfHolder.setFormat(PixelFormat.VIDEO_HOLE);
        curChanName = (TextView)findViewById(R.id.cur_chan_name);
        
        playTask = new RefreshPlayingAndEventTask();
    }
    
    /**
     * 七天按钮控件初始化
     */
    private void initRadioBnOf7Day()
    {
        Date date = new Date();
        Calendar cd = Calendar.getInstance();
        cd.setTime(date);
        final int week = cd.get(Calendar.DAY_OF_WEEK) - 1; // 0,1,2,...,6，分别代表星期日,星期一,...,星期六。
        
        String[] weeks =
            {getResources().getString(R.string.Sun), getResources().getString(R.string.Mon),
                getResources().getString(R.string.Tue), getResources().getString(R.string.Wed),
                getResources().getString(R.string.Thr), getResources().getString(R.string.Fri),
                getResources().getString(R.string.Sat)};
        
        radioBn0.setText(weeks[(week + 0) % 7]);
        radioBn1.setText(weeks[(week + 1) % 7]);
        radioBn2.setText(weeks[(week + 2) % 7]);
        radioBn3.setText(weeks[(week + 3) % 7]);
        radioBn4.setText(weeks[(week + 4) % 7]);
        radioBn5.setText(weeks[(week + 5) % 7]);
        radioBn6.setText(weeks[(week + 6) % 7]);
        
        // 焦点初始化
        weekday = 0;
        radioBn0.setChecked(true);
        radioBn0.setBackgroundResource(R.drawable.div_l_focus);
        radioBn0.setTextColor(black);
        weekdayBn = radioBn0;
        
    }
    
    /**
     * 获取及显示service,频道列表信息
     */
    @SuppressWarnings("unchecked")
    private void initServiceInfos()
    {
        ServiceInfoDao service = new ServiceInfoDaoImpl(EpgActivity.this);
        switch (serviceType)
        {
            case Config.SERVICE_TYPE_RADIO:
                tvServiceTitle.setText(getString(R.string.RadioChannel));
                serviceInfoBeans = service.getRadioChannelInfo();
                break;
            default:
                tvServiceTitle.setText(getString(R.string.TVChannel));
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
        public void run()
        {
            clearEventInfos();
            ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
            TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
            short serviceid = serviceBean.getServiceId().shortValue();
            short tsid = tpInfoBean.getTranStreamId().shortValue();
            short ornetowrkid = tpInfoBean.getOriginalNetId().shortValue();
            
            NativeEpg.EPGOnePage epgOnePage = nativeEpg.new EPGOnePage();
            
            for (int i = 0; i < 50; i++)
            {
                nativeEpg.GetEventListInfo(serviceid,
                    tsid,
                    ornetowrkid,
                    (char)weekday,
                    (short)1,
                    (short)100,
                    epgOnePage);
                int eventNum = epgOnePage.gettotalnum();
                if (eventNum != 0)
                {
                    NativeEpg.EpgEventInfo[] eventArray = epgOnePage.getEpgList();
                    refreshEventInfos(eventNum, eventArray);
                    
                    Bundle bundle = new Bundle();
                    bundle.putInt("epglength", eventNum);
                    
                    Message message = new Message();
                    message.obj = bundle;
                    message.what = Config.EPG_EVENT_LIST;
                    handler.sendMessage(message);
                    return;
                }
            }
            
            Bundle bundle = new Bundle();
            bundle.putInt("epglength", 0);
            
            Message message = new Message();
            message.obj = bundle;
            message.what = Config.EPG_EVENT_LIST;
            handler.sendMessage(message);
            
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
        
        int tpId = serviceBean.getTpId();
        TPInfoBean tpInfo = tpInfoDao.getTpInfo(tpId);
        
        // 节目号
        int channelNumber = serviceBean.getChannelNumber();
        
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
    public void refreshEventInfo()
    {
        new Thread(new RefreshPlayingTask()).start();
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
        nativeEpg.GetEventInfoByIndex((short)serviceId, tsid, ornetowrkid, 0, 1, firstEvent, null);
        category = firstEvent.getcontentnibble();
        if (eitLimitcategorys.contains(String.valueOf(category)))
        {
            LogUtils.printLog(1, 3, TAG, "--->>> eit level lock is shown, stop dvb player.");
            // set the eit category lock.
            eitCategoryLock = true;
            if (null == passwordDialog)
            {
                passwordDialog =
                    DialogUtils.passwordDialogCreate(this, CommonUtils.getNitCategory(this, category), handler, false);
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
            if (null == passwordDialog)
            {
                passwordDialog =
                    DialogUtils.passwordDialogCreate(this, getString(R.string.viewing_level_control), handler, false);
            }
            return;
        }
        
        /*
         * normal play service.
         */
        LogUtils.e(TAG, "EpgActivity.refreshPlayingAndEvent:: -----DVBPlayer----Start----->>> " + channelNum);
        PVR_Rec_Status_t recFile = nativePlayer.new PVR_Rec_Status_t();
        nativePlayer.DVBPlayerPvrRecGetStatus(recFile);
        int rec_status = recFile.getEnState();
        
        // The DVB is in recording status...
        if (rec_status == 2 || rec_status == 3)
        {
            if (null != conn)
            {
                PvrRecordServiceAIDL binder = conn.getPvrRecordBinder();
                
                int[] tunerInfo = null;
                
                try
                {
                    if (null != binder)
                    {
                        tunerInfo = binder.getTunerInfo();
                    }
                }
                catch (RemoteException e)
                {
                    e.printStackTrace();
                }
                
                if (null != tunerInfo && tunerInfo.length == 3)
                {
                    if (tunerInfo[0] != tpInfoBean.getTunerFreq() || tunerInfo[1] != tpInfoBean.getTunerSymbRate()
                        || tunerInfo[2] != tpInfoBean.getTunerEmod())
                    {
                        /* Change the frequency of tuner... Check if to pause the record task... */
                        
                        // stop the record ==>
                        // nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                        // Don't stop recording ==> nativePlayer.DVBPlayerStop();
                        new AlertDialog.Builder(EpgActivity.this).setTitle(R.string.frequency_conflict_header)
                            .setMessage(R.string.frequency_conflict_message)
                            .setPositiveButton(R.string.OK, new DialogInterface.OnClickListener()
                            {
                                
                                @Override
                                public void onClick(DialogInterface dialog, int which)
                                {
                                    nativePlayer.DVBPlayerPvrRecStop();
                                    
                                    try
                                    {
                                        conn.pvrRecordBinder.stopRecord();
                                    }
                                    catch (RemoteException e)
                                    {
                                        e.printStackTrace();
                                    }
                                    
                                    /* Normal play */
                                    nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                                }
                            })
                            .setNegativeButton(R.string.Cancel, new DialogInterface.OnClickListener()
                            {
                                @Override
                                public void onClick(DialogInterface dialog, int which)
                                {
                                    nativePlayer.DVBPlayerStop();
                                }
                            })
                            .show();
                    }
                    else
                    {
                        /* Normal play */
                        nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                    }
                }
                else
                {
                    /* PVR record is not running now.... Or can not get the PVR record tuner info... */
                    nativePlayer.DVBPlayerStop();
                    // nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
                }
            }
            else
            {
                /* Can not check if the PVR record is running... Stop the play... */
                
                nativePlayer.DVBPlayerStop();
            }
        }
        else
        {
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
            {// 向上翻页键keyCode
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
            {// 预约录制
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
            else if (event.getKeyCode() == Config.KEY_RED)
            {
                LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: event.getKeyCode() == Config.KEY_RED");
                
                // 预约录制
                if (eventList.hasFocus())
                {
                    LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: eventList.hasFocus()");
                    
                    final View eventView = eventList.getSelectedView();
                    
                    if (eventView != null)
                    {
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: eventView != null");
                        
                        /* Initialize the start time */
                        String time = ((TextView)eventView.findViewById(R.id.event_time)).getText().toString();
                        
                        String times[] = time.split("-");
                        
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: time = " + time + " time[0] = "
                            + times[0] + " time[1] = " + times[1]);
                        
                        /* Initialize the start and end time of the selected program */
                        long lstart = PvrUtils.convertFromHM(times[0]);
                        long lend = PvrUtils.convertFromHM(times[1]);
                        
                        /* Start time show not later than stop time */
                        if (lend < lstart)
                        {
                            lend = lend + PvrUtils.convertFromHM("23:59");
                        }
                        
                        /* Initialize the year/month/date */
                        String eventYMD = ((TextView)eventView.findViewById(R.id.event_ymd)).getText().toString();
                        
                        long eventDate = PvrUtils.convertFromYMD(eventYMD);
                        
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: lstart = " + lstart + " lstop = "
                            + lend + " eventDate = " + eventDate + " eventYMD = " + eventYMD);
                        
                        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
                        
                        Calendar currentCal = Calendar.getInstance();
                        
                        /* Use to judge the type of the RECORD TASK... It's base on the eventStartTime and eventEndTime */
                        long currentTime = currentCal.getTimeInMillis();
                        
                        /* Get the start time of the event */
                        currentCal.setTimeInMillis(eventDate + lstart);
                        
                        long eventStartTime = currentCal.getTimeInMillis();
                        
                        /* Get the end time of the event */
                        currentCal.setTimeInMillis(eventDate + lend);
                        
                        long eventEndTime = currentCal.getTimeInMillis();
                        
                        /* Get the week day of the event */
                        currentCal.setTimeInMillis(eventDate);
                        
                        int eventWeekDay = currentCal.get(Calendar.DAY_OF_WEEK);
                        
                        /* Print some log... */
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: currentTime = " + currentTime
                            + " eventStartTime = " + eventStartTime + " eventEndTime = " + eventEndTime
                            + " eventWeekDay = " + eventWeekDay);
                        
                        // TODO: Check if the task has been reserved... If so, cancel the task...
                        final PvrDao pvrDao = new PvrDaoImpl(EpgActivity.this);
                        
                        List<Integer> ids = pvrDao.checkTaskExists(serviceBean.getServiceId(), eventDate, lstart, lend);
                        
                        if (null != ids && ids.size() > 0)
                        {
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: the task has exists...");
                            
                            for (int i = 0; i < ids.size(); i++)
                            {
                                PvrBean pvrBean = new PvrBean();
                                
                                pvrBean.setPvrId(ids.get(i));
                                
                                pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                                
                                pvrDao.update_Record_Task(pvrBean);
                            }
                            
                            ImageView event_Pvr_Status = (ImageView)eventView.findViewById(R.id.event_Pvr_Status);
                            
                            event_Pvr_Status.setImageDrawable(null);
                            
                            return true;
                        }
                        
                        /*
                         * If currentTime is between start time and end time of the event... It means to start a real
                         * time task without notice dialog...
                         */
                        if (currentTime >= eventStartTime && currentTime < eventEndTime)
                        {
                            // Start a real time record task...
                            final StorageUtils sUtil = new StorageUtils(EpgActivity.this);
                            
                            /* Can not find the storage device... */
                            if (sUtil.checkHDDSpace() != 0)
                            {
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME  Can not find the storage device...");
                                
                                return false;
                            }
                            else
                            {
                                LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME storage device ok...");
                            }
                            
                            /*
                             * Get the real time reservation from the database... The ID is unique set to
                             * DBPvr.REAL_TIME_TASK_ID
                             */
                            PvrBean mBean = pvrDao.get_Record_Task_By_ID(DBPvr.REAL_TIME_TASK_ID);
                            
                            if (null == mBean)
                            {
                                /* Initialize the real time task in database */
                                pvrDao.insert_REAL_TIME_Task();
                                
                                /* Get the real time task from database */
                                mBean = pvrDao.get_REAL_TIME_Task();
                            }
                            
                            /* Can not get a real time task... */
                            if (mBean == null)
                            {
                                LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME mBean == null");
                                
                                return false;
                            }
                            
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME mBean != null");
                            
                            /* Get the year/month/day of the event */
                            currentCal.setTimeInMillis(eventDate);
                            int year = currentCal.get(Calendar.YEAR);
                            int month = currentCal.get(Calendar.MONTH);
                            int day = currentCal.get(Calendar.DAY_OF_MONTH);
                            
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME eventYear = " + year
                                + " eventMonth = " + month + " eventDay = " + day, true);
                            
                            /* Modify the year/month/day information of current time */
                            /*
                             * Actually the year/month/day must the same with current day... You can see the lastest log
                             * above...
                             */
                            Calendar now = Calendar.getInstance();
                            
                            now.set(Calendar.YEAR, year);
                            now.set(Calendar.MONTH, month);
                            now.set(Calendar.DAY_OF_MONTH, day);
                            
                            /* Initialize the REAL_TIME task... */
                            mBean.setPvrWakeMode(DBPvr.RECORD_WAKEUP_MODE);
                            mBean.setServiceId(serviceBean.getServiceId());
                            mBean.setLogicalNumber(serviceBean.getLogicalNumber());
                            // globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                            mBean.setPvrSetDate(eventDate);
                            mBean.setPvrStartTime(now.getTimeInMillis() - mBean.getPvrSetDate());
                            mBean.setPvrWeekDate(eventWeekDay);
                            mBean.setPvrRecordLength((eventEndTime - now.getTimeInMillis()) >= 0 ? (eventEndTime - now.getTimeInMillis())
                                : 0);
                            mBean.setPvr_is_sleep(DBPvr.SUSPEND_AFTER_RECORD);
                            mBean.setPvr_type_tag(DBPvr.TASK_EPG_START_WITHOUT_DIALOG);
                            /* The setPvrMode is useless...Here is just try to maintain the integrity.. */
                            mBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                            
                            /* Check the conflict tasks... */
                            final List<Integer> conflictIDS = pvrDao.getIdOfConflictTasks(mBean);
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME switch task status to CLOSE_MODE...");
                            
                            /*
                             * Set the pvr_mode of REAL_TIME task to close to make sure the Database observer will
                             * receive the change...
                             */
                            mBean.setPvrMode(DBPvr.CLOSE_MODE);
                            
                            pvrDao.update_Record_Task(mBean);
                            
                            final PvrBean udpateBean = mBean;
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME "
                                    + (null == conflictIDS ? "conflictIDS == null" : "conflictIDS.size = "
                                        + conflictIDS.size()));
                            
                            if (null != conflictIDS && conflictIDS.size() > 0)
                            {
                                String msg = getResources().getString(R.string.real_time_task_conflict);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        /* Set all conflict tasks' status to CLOSE_MODE */
                                        pvrDao.cancelTaskByIDS(conflictIDS);
                                        
                                        /*
                                         * We will set the pvr_mode of REAL_TIME to CLOSE_MODE when get from the
                                         * database. This will make sure the Database observer will receive the change
                                         * broadcast
                                         */
                                        udpateBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                                        
                                        LogUtils.e(TAG,
                                            "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME switch task status to DO_ONCE_MODE in CONFLICTS_DIALOG...");
                                        
                                        /* update the task's data */
                                        pvrDao.update_Record_Task(udpateBean);
                                        
                                        dialog.dismiss();
                                        
                                        handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                                    }
                                })
                                    .setNegativeButton(R.string.ca_cancel, new DialogInterface.OnClickListener()
                                    {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which)
                                        {
                                            dialog.dismiss();
                                        }
                                    });
                                builder.show();
                            }
                            else
                            {
                                // Toast.makeText(EpgActivity.this,
                                // "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME pvrDao.update_Record_Task(udpateBean);",
                                // Toast.LENGTH_LONG)
                                // .show();
                                
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEY_RED::REAL_TIME switch task status to DO_ONCE_MODE in NO_CONFLICTS...");
                                
                                /*
                                 * We will set the pvr_mode of REAL_TIME to CLOSE_MODE when get from the database. This
                                 * will make sure the Database observer will receive the change broadcast
                                 */
                                udpateBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                                
                                /* update the task's data */
                                pvrDao.update_Record_Task(udpateBean);
                                
                                handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                            }
                            
                            return true;
                        }
                        else if (eventStartTime > currentTime)
                        {
                            // Add a reservation record task...
                            
                            /*
                             * The PvrBean return by get_First_Close_Task() will make sure that the pvr_mode is
                             * CLOSE_MODE
                             */
                            PvrBean mBean = pvrDao.get_First_Close_Task();
                            
                            if (mBean == null)
                            {
                                LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED::reservation::  mBean == null");
                                
                                String msg = getResources().getString(R.string.pvr_orderfull_info);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        dialog.dismiss();
                                    }
                                });
                                
                                builder.show();
                                
                                return true;
                            }
                            
                            /* Initialize the reservation record task... */
                            mBean.setPvrWakeMode(DBPvr.RECORD_WAKEUP_MODE);
                            mBean.setServiceId(serviceBean.getServiceId());
                            mBean.setLogicalNumber(serviceBean.getLogicalNumber());
                            // globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                            mBean.setPvrSetDate(eventDate);
                            mBean.setPvrStartTime(lstart);
                            mBean.setPvrWeekDate(eventWeekDay);
                            mBean.setPvrRecordLength(PvrUtils.convertMSFromHM(PvrUtils.getHMFromLong(lend - lstart)));
                            mBean.setPvr_is_sleep(DBPvr.SUSPEND_AFTER_RECORD);
                            mBean.setPvr_type_tag(DBPvr.TASK_DEFAULT_START_WITH_DIALOG);
                            mBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                            
                            final PvrBean udpateBean = mBean;
                            
                            final List<Integer> conflictIDS = pvrDao.getIdOfConflictTasks(mBean);
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEY_RED::reservation:: "
                                    + (null == conflictIDS ? "conflictIDS == null" : "conflictIDS.size = "
                                        + conflictIDS.size()));
                            
                            /* Check the conflict tasks... */
                            if (null != conflictIDS && conflictIDS.size() > 0)
                            {
                                String msg = getResources().getString(R.string.reservation_task_conflict);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        /* Set all conflict tasks' status to CLOSE_MODE */
                                        pvrDao.cancelTaskByIDS(conflictIDS);
                                        
                                        /* update the task's data */
                                        pvrDao.update_Record_Task(udpateBean);
                                        
                                        // /* Set the 'RESERVATION' flag */
                                        // ImageView event_Pvr_Status =
                                        // (ImageView)eventView.findViewById(R.id.event_Pvr_Status);
                                        //
                                        // event_Pvr_Status.setImageResource(R.drawable.icon);
                                        
                                        dialog.dismiss();
                                        
                                        handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                                    }
                                }).setNegativeButton(R.string.ca_cancel, new DialogInterface.OnClickListener()
                                {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which)
                                    {
                                        dialog.dismiss();
                                    }
                                });
                                
                                builder.show();
                                
                                return true;
                            }
                            else
                            {
//                                Toast.makeText(EpgActivity.this,
//                                    "EpgActivity.dispatchKeyEvent.KEY_RED::reservation:: pvrDao.update_Record_Task(udpateBean);",
//                                    Toast.LENGTH_LONG)
//                                    .show();
                                
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEY_RED::reservation:: switch task status to DO_ONCE_MODE in NO_CONFLICTS...");
                                
                                pvrDao.update_Record_Task(udpateBean);
                                
                                // /* Set the 'RESERVATION' flag */
                                // ImageView event_Pvr_Status =
                                // (ImageView)eventView.findViewById(R.id.event_Pvr_Status);
                                //
                                // event_Pvr_Status.setImageResource(R.drawable.icon);
                                
                                handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                            }
                        }
                        else
                        {
                            String msg = getResources().getString(R.string.task_expired);
                            AlertDialog.Builder builder = new AlertDialog.Builder(this);
                            builder.setTitle(getResources().getString(R.string.pvr_title));
                            builder.setMessage(msg);
                            builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                            {
                                public void onClick(DialogInterface dialog, int arg1)
                                {
                                    dialog.dismiss();
                                }
                            });
                            
                            builder.show();
                        }
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "EpgActivity.dispatchKeyEvent.KEY_RED:: eventView == null.. Do not start any record task...");
                    }
                }
                else
                {
                    LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEY_RED:: eventList has not focus");
                }
            }
            else if (event.getKeyCode() == KeyEvent.KEYCODE_DPAD_CENTER)
            {
                // Reservation play
                if (eventList.hasFocus())
                {
                    LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: eventList.hasFocus()");
                    
                    final View eventView = eventList.getSelectedView();
                    
                    if (eventView != null)
                    {
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: eventView != null");
                        
                        /* Initialize the start time */
                        String time = ((TextView)eventView.findViewById(R.id.event_time)).getText().toString();
                        
                        String times[] = time.split("-");
                        
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: time = " + time
                            + " time[0] = " + times[0] + " time[1] = " + times[1]);
                        
                        /* Initialize the start and end time of the selected program */
                        long lstart = PvrUtils.convertFromHM(times[0]);
                        long lend = PvrUtils.convertFromHM(times[1]);
                        
                        /* Start time show not later than stop time */
                        if (lend < lstart)
                        {
                            lend = lend + PvrUtils.convertFromHM("23:59");
                        }
                        
                        /* Initialize the year/month/date */
                        String eventYMD = ((TextView)eventView.findViewById(R.id.event_ymd)).getText().toString();
                        
                        long eventDate = PvrUtils.convertFromYMD(eventYMD);
                        
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: lstart = " + lstart
                            + " lstop = " + lend + " eventDate = " + eventDate + " eventYMD = " + eventYMD);
                        
                        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
                        
                        Calendar currentCal = Calendar.getInstance();
                        
                        /* Use to judge the type of the RECORD TASK... It's base on the eventStartTime and eventEndTime */
                        long currentTime = currentCal.getTimeInMillis();
                        
                        /* Get the start time of the event */
                        currentCal.setTimeInMillis(eventDate + lstart);
                        
                        long eventStartTime = currentCal.getTimeInMillis();
                        
                        /* Get the end time of the event */
                        currentCal.setTimeInMillis(eventDate + lend);
                        
                        long eventEndTime = currentCal.getTimeInMillis();
                        
                        /* Get the week day of the event */
                        currentCal.setTimeInMillis(eventDate);
                        
                        int eventWeekDay = currentCal.get(Calendar.DAY_OF_WEEK);
                        
                        /* Print some log... */
                        LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: currentTime = "
                            + currentTime + " eventStartTime = " + eventStartTime + " eventEndTime = " + eventEndTime
                            + " eventWeekDay = " + eventWeekDay);
                        
                        // TODO: Check if the task has been reserved... If so, cancel the task...
                        final PvrDao pvrDao = new PvrDaoImpl(EpgActivity.this);
                        
                        List<Integer> ids = pvrDao.checkTaskExists(serviceBean.getServiceId(), eventDate, lstart, lend);
                        
                        if (null != ids && ids.size() > 0)
                        {
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: the task has exists...");
                            
                            for (int i = 0; i < ids.size(); i++)
                            {
                                PvrBean pvrBean = new PvrBean();
                                
                                pvrBean.setPvrId(ids.get(i));
                                
                                pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                                
                                pvrDao.update_Record_Task(pvrBean);
                            }
                            
                            /* Set the 'RESERVATION' flag */
                            ImageView event_Pvr_Status = (ImageView)eventView.findViewById(R.id.event_Pvr_Status);
                            
                            event_Pvr_Status.setImageDrawable(null);
                            
                            return true;
                        }
                        
                        /*
                         * If currentTime is between start time and end time of the event... It means to start a real
                         * time task without notice dialog...
                         */
                        if (currentTime >= eventStartTime && currentTime < eventEndTime)
                        {
                            // Start a real time play task...
                            
                            /*
                             * Get the real time reservation from the database... The ID is unique set to
                             * DBPvr.REAL_TIME_TASK_ID
                             */
                            PvrBean mBean = pvrDao.get_Record_Task_By_ID(DBPvr.REAL_TIME_TASK_ID);
                            
                            if (null == mBean)
                            {
                                /* Initialize the real time task in database */
                                pvrDao.insert_REAL_TIME_Task();
                                
                                /* Get the real time task from database */
                                mBean = pvrDao.get_REAL_TIME_Task();
                            }
                            
                            /* Can not get a real time task... */
                            if (mBean == null)
                            {
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME mBean == null");
                                
                                return false;
                            }
                            
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME mBean != null");
                            
                            /* Get the year/month/day of the event */
                            currentCal.setTimeInMillis(eventDate);
                            int year = currentCal.get(Calendar.YEAR);
                            int month = currentCal.get(Calendar.MONTH);
                            int day = currentCal.get(Calendar.DAY_OF_MONTH);
                            
                            LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME eventYear = "
                                + year + " eventMonth = " + month + " eventDay = " + day, true);
                            
                            /* Modify the year/month/day information of current time */
                            /*
                             * Actually the year/month/day must the same with current day... You can see the latest log
                             * above...
                             */
                            Calendar now = Calendar.getInstance();
                            
                            now.set(Calendar.YEAR, year);
                            now.set(Calendar.MONTH, month);
                            now.set(Calendar.DAY_OF_MONTH, day);
                            
                            /* Initialize the REAL_TIME task... */
                            mBean.setPvrWakeMode(DBPvr.PLAY_WAKEUP_MODE);
                            mBean.setServiceId(serviceBean.getServiceId());
                            mBean.setLogicalNumber(serviceBean.getLogicalNumber());
                            // globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                            mBean.setPvrSetDate(eventDate);
                            mBean.setPvrStartTime(now.getTimeInMillis() - mBean.getPvrSetDate());
                            mBean.setPvrWeekDate(eventWeekDay);
                            
                            long taskLength = eventEndTime - now.getTimeInMillis();
                            
                            taskLength = taskLength >= 0 ? taskLength : 0;
                            
                            LogUtils.d(TAG,
                                "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME  taskLength = "
                                    + taskLength);
                            
                            mBean.setPvrRecordLength(taskLength);
                            
                            mBean.setPvr_is_sleep(DBPvr.SUSPEND_AFTER_RECORD);
                            mBean.setPvr_type_tag(DBPvr.TASK_EPG_START_WITHOUT_DIALOG);
                            /* The setPvrMode is useless...Here is just try to maintain the integrity.. */
                            mBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                            
                            final PvrBean udpateBean = mBean;
                            
                            /* Check the conflict tasks... */
                            final List<Integer> conflictIDS = pvrDao.getIdOfConflictTasks(mBean);
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME "
                                    + (null == conflictIDS ? "conflictIDS == null" : "conflictIDS.size = "
                                        + conflictIDS.size()));
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME switch task status to CLOSE_MODE...");
                            
                            /*
                             * Set the pvr_mode of REAL_TIME task to close to make sure the Database observer will
                             * receive the change...
                             */
                            mBean.setPvrMode(DBPvr.CLOSE_MODE);
                            
                            mBean.setPvrStartTime(mBean.getPvrStartTime() + DBPvr.TASK_TRIGGER_DELAY);
                            mBean.setPvrRecordLength(0l);
                            
                            pvrDao.update_Record_Task(udpateBean);
                            
                            if (null != conflictIDS && conflictIDS.size() > 0)
                            {
                                String msg = getResources().getString(R.string.real_time_task_conflict);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        /* Set all conflict tasks' status to CLOSE_MODE */
                                        pvrDao.cancelTaskByIDS(conflictIDS);
                                        
                                        /*
                                         * We will set the pvr_mode of REAL_TIME to CLOSE_MODE when get from the
                                         * database. This will make sure the Database observer will receive the change
                                         * broadcast
                                         */
                                        udpateBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                                        
                                        LogUtils.e(TAG,
                                            "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME switch task status to DO_ONCE_MODE in CONFLICTS_DIALOG...");
                                        
                                        /* update the task's data */
                                        pvrDao.update_Record_Task(udpateBean);
                                        
                                        dialog.dismiss();
                                        
                                        handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                                        
                                    }
                                })
                                    .setNegativeButton(R.string.ca_cancel, new DialogInterface.OnClickListener()
                                    {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which)
                                        {
                                            dialog.dismiss();
                                        }
                                    });
                                builder.show();
                            }
                            else
                            {
//                                Toast.makeText(EpgActivity.this,
//                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME pvrDao.update_Record_Task(udpateBean);",
//                                    Toast.LENGTH_LONG)
//                                    .show();
                                
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::REAL_TIME switch task status to DO_ONCE_MODE in NO_CONFLICTS...");
                                
                                /*
                                 * We will set the pvr_mode of REAL_TIME to CLOSE_MODE when get from the database. This
                                 * will make sure the Database observer will receive the change broadcast
                                 */
                                udpateBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                                
                                /* update the task's data */
                                pvrDao.update_Record_Task(udpateBean);
                                
                                handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                            }
                            
                            return true;
                        }
                        else if (eventStartTime > currentTime)
                        {
                            // Add a reservation record task...
                            
                            /*
                             * The PvrBean return by get_First_Close_Task() will make sure that the pvr_mode is
                             * CLOSE_MODE
                             */
                            PvrBean mBean = pvrDao.get_First_Close_Task();
                            
                            if (mBean == null)
                            {
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::reservation::  mBean == null");
                                
                                String msg = getResources().getString(R.string.pvr_orderfull_info);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        dialog.dismiss();
                                    }
                                });
                                
                                builder.show();
                                
                                return true;
                            }
                            
                            /* Initialize the reservation record task... */
                            mBean.setPvrWakeMode(DBPvr.PLAY_WAKEUP_MODE);
                            mBean.setServiceId(serviceBean.getServiceId());
                            mBean.setLogicalNumber(serviceBean.getLogicalNumber());
                            // globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                            mBean.setPvrSetDate(eventDate);
                            mBean.setPvrStartTime(lstart);
                            mBean.setPvrWeekDate(eventWeekDay);
                            mBean.setPvrRecordLength(0l);
                            mBean.setPvr_is_sleep(DBPvr.SUSPEND_AFTER_RECORD);
                            mBean.setPvr_type_tag(DBPvr.TASK_DEFAULT_START_WITH_DIALOG);
                            mBean.setPvrMode(DBPvr.DO_ONCE_MODE);
                            
                            final PvrBean udpateBean = mBean;
                            
                            final List<Integer> conflictIDS = pvrDao.getIdOfConflictTasks(mBean);
                            
                            LogUtils.e(TAG,
                                "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::reservation:: "
                                    + (null == conflictIDS ? "conflictIDS == null" : "conflictIDS.size = "
                                        + conflictIDS.size()));
                            
                            /* Check the conflict tasks... */
                            if (null != conflictIDS && conflictIDS.size() > 0)
                            {
                                String msg = getResources().getString(R.string.reservation_task_conflict);
                                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                                builder.setTitle(getResources().getString(R.string.pvr_title));
                                builder.setMessage(msg);
                                builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                                {
                                    public void onClick(DialogInterface dialog, int arg1)
                                    {
                                        /* Set all conflict tasks' status to CLOSE_MODE */
                                        pvrDao.cancelTaskByIDS(conflictIDS);
                                        
                                        /* update the task's data */
                                        pvrDao.update_Record_Task(udpateBean);
                                        
                                        /* Set the 'RESERVATION' flag */
                                        ImageView event_Pvr_Status =
                                            (ImageView)eventView.findViewById(R.id.event_Pvr_Status);
                                        
                                        event_Pvr_Status.setImageResource(R.drawable.icon);
                                        
                                        dialog.dismiss();
                                        
                                        handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                                    }
                                }).setNegativeButton(R.string.ca_cancel, new DialogInterface.OnClickListener()
                                {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which)
                                    {
                                        dialog.dismiss();
                                    }
                                });
                                
                                builder.show();
                            }
                            else
                            {
//                                Toast.makeText(EpgActivity.this,
//                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::reservation:: pvrDao.update_Record_Task(udpateBean);",
//                                    Toast.LENGTH_LONG)
//                                    .show();
                                
                                LogUtils.e(TAG,
                                    "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER::reservation:: switch task status to DO_ONCE_MODE in NO_CONFLICTS...");
                                
                                pvrDao.update_Record_Task(udpateBean);
                                handler.sendEmptyMessage(Config.UPDAE_EVENT_PVR_STATUS);
                            }
                            
                            return true;
                        }
                        else
                        {
                            String msg = getResources().getString(R.string.task_expired);
                            AlertDialog.Builder builder = new AlertDialog.Builder(this);
                            builder.setTitle(getResources().getString(R.string.pvr_title));
                            builder.setMessage(msg);
                            builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
                            {
                                public void onClick(DialogInterface dialog, int arg1)
                                {
                                    dialog.dismiss();
                                }
                            });
                            
                            builder.show();
                            return true;
                        }
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: eventView == null.. Do not start any record task...");
                    }
                }
                else
                {
                    LogUtils.e(TAG, "EpgActivity.dispatchKeyEvent.KEYCODE_DPAD_CENTER:: eventList has not focus");
                }
            }
        }
        return super.dispatchKeyEvent(event);
    }
    
    private class EpgServiceConnection implements ServiceConnection
    {
        private PvrRecordServiceAIDL pvrRecordBinder = null;
        
        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            LogUtils.e(TAG, "EpgActivity.EpgServiceConnection:: onServiceConnected() run!!!");
            if (service == null)
            {
                LogUtils.e(TAG, "EpgActivity.EpgServiceConnection:: onServiceConnected:: service == null");
            }
            
            pvrRecordBinder = PvrRecordServiceAIDL.Stub.asInterface(service);
            nativePlayer.DVBPlayerPlayProgram(getPlayerParamsByChanNum(channelNum));
        }
        
        @Override
        public void onServiceDisconnected(ComponentName name)
        {
        }
        
        public PvrRecordServiceAIDL getPvrRecordBinder()
        {
            return pvrRecordBinder;
        }
    }
    
    @SuppressWarnings("unchecked")
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        LogUtils.e(TAG, "EpgActivity.onKeyDown:: keyCode = " + keyCode);
        
        if (event.getRepeatCount() == 0)
        {
            // Log.i(TAG, "******* onKeyDown() ******* , keyCode = " + keyCode);
            if (keyCode == Config.KEY_BLUE)// BLUE KEY
            {
                Intent intent = new Intent();
                Bundle bundle = new Bundle();
                bundle.putChar(Config.DVB_SERVICE_TYPE, serviceType);
                bundle.putInt("LogicalNumber", logicalNumber);
                bundle.putInt("ServiceId", serviceId);
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: serviceType = " + (int)serviceType);
                
                intent.putExtras(bundle);
                Uri uri = Uri.parse("dvb:" + Config.DVB_EIT + "");
                intent.setData(uri);
                intent.setClassName("com.pbi.dvb", "com.pbi.dvb.EitActivity");
                startActivity(intent);
                eit_Show = true;
                return true;
            }
            
            if (keyCode == Config.KEY_SHOW)// 显示键
            {
                if (null == epgDetailDialog || !epgDetailDialog.isShowing())
                {
//                    CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                    
                    if (eventList.hasFocus())
                    {
                        ServiceInfoBean serviceBean = serviceInfoBeans.get(getIndexByChannelNum(channelNum));
                        TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
                        short serviceid = serviceBean.getServiceId().shortValue();
                        short tsid = tpInfoBean.getTranStreamId().shortValue();
                        short ornetowrkid = tpInfoBean.getOriginalNetId().shortValue();
                        
                        String noContent = getResources().getString(R.string.epg_detail_no_content);
                        NativeEpg.extentinfo extentInfo = nativeEpg.new extentinfo();
                        
                        nativeEpg.GetOneEventExtentInfo(serviceid,
                            tsid,
                            ornetowrkid,
                            (char)0,
                            (char)weekday,
                            (short)currentSelected,
                            extentInfo);
                        
                        String firstExtendInfo = extentInfo.getextentinfo();
                        
                        if (null == firstExtendInfo || "".equals(firstExtendInfo))
                        {
                            firstExtendInfo = noContent;
                        }
                        
                        nativeEpg.GetOneEventExtentInfo(serviceid,
                            tsid,
                            ornetowrkid,
                            (char)0,
                            (char)weekday,
                            (short)(currentSelected + 1),
                            extentInfo);
                        
                        String sencondExtendInfo = extentInfo.getextentinfo();
                        
                        if (null == sencondExtendInfo || "".equals(sencondExtendInfo))
                        {
                            sencondExtendInfo = noContent;
                        }
                        
                        List<String> extendInfos = new ArrayList<String>();
                        
                        extendInfos.add(firstExtendInfo);
                        extendInfos.add(sencondExtendInfo);
                        
                        if (null == epgDetailDialog)
                        {
                            epgDetailDialog =
                                new EpgDetailDialog(EpgActivity.this, R.layout.text_scroll_dialog, extendInfos);
                            
                            epgDetailDialog.setCallBack(new EpgDetailDialogCallBack()
                            {
                                @Override
                                public void onShowKey()
                                {
//                                    CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                                }
                            });
                            
                            epgDetailDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
                        }
                        else
                        {
                            epgDetailDialog.setNewEpgContent(extendInfos);
                        }
                        
                        epgDetailDialog.show();
                        return true;
                    }
                }
                
//                CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
            }
        }
        
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {// 返回键
            if (caMessageToast != null)
            {
                caMessageToast = null;
            }
            finish();
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
        {
            // 上
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
                
                radioBnSetBG();
                
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
        {// 下
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
            else if (radioGroup.hasFocus())
            {
                radioBnSetBG();
                if (epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------down Clicked : radioGroup --> serviceList --");
                    focusInServiceList();
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
        else if (keyCode == KeyEvent.KEYCODE_DPAD_LEFT) // 左键
        {
            LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> weeDay [" + weekday + "]");
            if (radioGroup.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> radioGroup.hasFocus()...");
                weekday = weekday - 1;
                if (weekday < 0)
                {
                    Log.i(TAG, "---------left Clicked : weekday == 0 -----------");
                    focusInServiceList();
                    radioBnSetBG();
                    serviceListGetFocus();
                }
                else
                {
                    Log.i(TAG, "---------left Clicked : weekday > 0 -----------");
                    focusInRadioGroup();
                    radioBnSetBG();
                    refreshEventInfo();
                    return true;
                }
            }
            else if (eventList.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> eventList.hasFocus()...");
                weekday--;
                
                if (weekday < 0)
                {
                    weekday = 0;
                    LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> eventList --> serviceList...");
                    focusInServiceList();
                    serviceListGetFocus();
                }
                else
                {
                    LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> eventList refresh from [" + (weekday + 1)
                        + "] to [" + weekday + "]...");
                    
                    radioBnSetBG();
                    refreshEventInfo();
                    eventListGetFocus();
                    return true;
                }
            }
            else if (serviceList.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> serviceList.hasFocus()...");
            }
            else
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: left key ==> unknownViews.hasFocus()...");
            }
            serviceList.invalidate();
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_RIGHT) // 右
        {
            LogUtils.e(TAG, "EpgActivity.onKeyDown:: right key ==> weeDay [" + weekday + "]");
            
            if (radioGroup.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: rifht key ==> radioGroup.hasFocus()...");
                Log.i(TAG, "---------right Clicked : radioGroup --> radioGroup ------------");
                focusInRadioGroup();
                weekday = weekday == 6 ? 0 : (weekday + 1);
                radioBnSetBG();
                refreshEventInfo();
                
                return true;
            }
            else if (serviceList.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: rifht key ==> serviceList.hasFocus()...");
                serviceListLostFocus();
                
                if (epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------right Clicked : serviceList --> radioGroup ------------");
                    focusInRadioGroup();
                    serviceList.setNextFocusRightId(R.id.radiogroup);
                    radioBnSetBG();
                    handler.removeCallbacks(playTask);
                    handler.post(playTask);
                }
                else if (!epgEventInfoBeans.isEmpty())
                {
                    Log.i(TAG, "---------right Clicked : serviceList --> eventList ------------");
                    focusInEventList();
                    serviceList.setNextFocusRightId(R.id.event_list);
                    eventListGetFocus();
                }
                
                return true;
            }
            else if (eventList.hasFocus())
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: rifht key ==> eventList.hasFocus()...");
                weekday++;
                if (weekday >= 7)
                {
                    weekday = 0;
                }
                
                radioBnSetBG();
                refreshEventInfo();
                return true;
            }
            else
            {
                LogUtils.e(TAG, "EpgActivity.onKeyDown:: rifht key ==> unknownViews getFocus...");
            }
            serviceList.invalidate();
        }
        return false;
    }
    
    public void serviceListLostFocus()
    {
        serviceList.getChildAt(currentPosition).setBackgroundResource(R.drawable.select_s);
        ((TextView)serviceList.getChildAt(currentPosition).findViewById(R.id.epg_service)).setTextColor(black);
    }
    
    @SuppressWarnings("unchecked")
    public void serviceListGetFocus()
    {
        serviceList.getChildAt(currentPosition).setBackgroundDrawable(null);
        ((TextView)serviceList.getChildAt(currentPosition).findViewById(R.id.epg_service)).setTextColor(white);
        
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
    public synchronized void radioBnSetBG()
    {
        LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: weekday = " + weekday);
        if (weekday >= 0)
        {
            weekdayBn.setBackgroundDrawable(null);
            weekdayBn.setTextColor(white);
        }
        
        switch (weekday)
        {
            case 0:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 0 = ");
                radioBn0.setBackgroundResource(R.drawable.div_l_focus);
                radioBn0.setTextColor(black);
                weekdayBn = radioBn0;
                break;
            
            case 1:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 1 = ");
                radioBn1.setBackgroundResource(R.drawable.div_l_focus);
                radioBn1.setTextColor(black);
                weekdayBn = radioBn1;
                break;
            
            case 2:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 2 = ");
                radioBn2.setBackgroundResource(R.drawable.div_l_focus);
                radioBn2.setTextColor(black);
                weekdayBn = radioBn2;
                break;
            
            case 3:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 3 = ");
                radioBn3.setBackgroundResource(R.drawable.div_l_focus);
                radioBn3.setTextColor(black);
                weekdayBn = radioBn3;
                break;
            
            case 4:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 4 = ");
                radioBn4.setBackgroundResource(R.drawable.div_l_focus);
                radioBn4.setTextColor(black);
                weekdayBn = radioBn4;
                break;
            
            case 5:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 5 = ");
                radioBn5.setBackgroundResource(R.drawable.div_l_focus);
                radioBn5.setTextColor(black);
                weekdayBn = radioBn5;
                break;
            
            case 6:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case 6 = ");
                radioBn6.setBackgroundResource(R.drawable.div_l_focus);
                radioBn6.setTextColor(black);
                weekdayBn = radioBn6;
                break;
            
            case -1:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case -1 = ");
                weekdayBn = radioBn0;
                break;
            
            default:
                LogUtils.e(TAG, "EpgActivity.radioBnSetBG:: in case unknown...");
                break;
        }
        
        if (weekday >= 0)
        {
            weekdayBn.requestFocus();
        }
        else
        {
            weekday = 0;
        }
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
        
        playOrderImage.setVisibility(View.GONE);
        playOrderText.setVisibility(View.GONE);
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
        
        playOrderImage.setVisibility(View.VISIBLE);
        playOrderText.setVisibility(View.VISIBLE);
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
        
        playOrderImage.setVisibility(View.GONE);
        playOrderText.setVisibility(View.GONE);
    }
    
    class ChannelOnItemClickListener implements OnItemClickListener
    {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            isDVB = true;
            isIPTV = false;
            Intent intent = new Intent(EpgActivity.this, TVChannelPlay.class);
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
    
    class ChannelOnItemSelectedListener implements OnItemSelectedListener
    {
        
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            currentSelected = position + 1;
        }
        
        @Override
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
    }
    
}
