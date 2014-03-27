/*
 * File Name:  TVChannelPlay.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-07
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

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
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.drawable.BitmapDrawable;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnKeyListener;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.adapter.PlayProgramAdapter;
import com.pbi.dvb.aidl.OTADownloaderServiceAIDL;
import com.pbi.dvb.aidl.PvrRecordServiceAIDL;
import com.pbi.dvb.ca.CaEmail;
import com.pbi.dvb.dao.MailDao;
import com.pbi.dvb.dao.PageDao;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.dao.impl.MailDaoImpl;
import com.pbi.dvb.dao.impl.PageDaoImpl;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.dao.impl.TpInfoDaoImpl;
import com.pbi.dvb.domain.MountInfoBean;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.domain.SoundtrackBean;
import com.pbi.dvb.domain.TPInfoBean;
import com.pbi.dvb.dvbinterface.MessageCa;
import com.pbi.dvb.dvbinterface.MessageCa.ippnotifymsg;
import com.pbi.dvb.dvbinterface.MessageNit;
import com.pbi.dvb.dvbinterface.MessagePVR;
import com.pbi.dvb.dvbinterface.MessagePlayer;
import com.pbi.dvb.dvbinterface.MsgTimeUpdate;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caIppvBuy;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativeEpg;
import com.pbi.dvb.dvbinterface.NativeEpg.EpgEventInfo;
import com.pbi.dvb.dvbinterface.NativeEpg.EpgTime;
import com.pbi.dvb.dvbinterface.NativeEpg.peventRate;
import com.pbi.dvb.dvbinterface.NativeEpgMsg;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_Play_Param_t;
import com.pbi.dvb.dvbinterface.NativePlayer.DvbPVR_Play_t;
import com.pbi.dvb.dvbinterface.NativePlayer.DvbPVR_Rec_t;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Play_Status_t;
import com.pbi.dvb.dvbinterface.NativePlayer.PVR_Rec_Status_t;
import com.pbi.dvb.dvbinterface.Nativenit;
import com.pbi.dvb.dvbinterface.Nativetime;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.pvrinterface.impl.TickCountDownUtil;
import com.pbi.dvb.service.OTADownloaderService;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.StorageUtils;
import com.pbi.dvb.view.CustomMarqueeText;
import com.pbi.dvb.view.CustomToast;
import com.pbi.dvb.view.EpgDetailDialog;
import com.pbi.dvb.view.EpgDetailDialog.EpgDetailDialogCallBack;

public class TVChannelPlay extends DVBBaseActivity
{
    private static final String TAG = "TVChannelPlay";
    /*--Declare the player,ppc,epg  ca message and ipp object.--*/
    private NativeDrive nativeDrive;
    
    private NativePlayer dvbPlayer;
    
    private DVBPlayer_Play_Param_t playParams;
    
    private NativeEpg nativeEpg;
    
    private MessageCa messageCa;
    
    private boolean isEpg = false;
    
    /*--Audio--*/
    private AudioManager audioManager;
    
    private boolean g_bAudioIsMute;
    
    /*--Declare the parent-child lock grade.--*/
    private int currentEitGrade;
    
    private int originalGrade;
    
    private int tempGrade;
    
    /*--Digital key changing channel.--*/
    private TextView tvChannelNumber;
    
    private TextView tvChannelName2;
    
    private int numberInput = 0;
    
    private StringBuilder passwordInput;
    
    /*--Showing message dialog--*/
    private TextView tvDialogMessage;
    
    /*--Showing locked dialog.--*/
    private TextView tvPwdTitle;
    
    private TextView tvPwd;
    
    // private TextView tvPwdTips;
    
    private String superPwd;
    
    private LinearLayout llInputPwd;
    
    private boolean firGetServiceLock = true;
    
    private boolean serviceLock = false;// default no service lock.
    
    private boolean eitLevelLock = false; // default no eit level lock.
    
    private boolean eitCategoryLock = false; // default no eit category lock.
    private boolean caAdultLock = false;//default no ca adult lock.
    
    private Dialog eitLevelDialog;
    
    private Dialog caLevelDialog;
    
    private int category;
    
    private List<String> eitLimitcategorys;
    
    private Dialog eitCategoryDialog;
    
    private Dialog searchDialog;
    
    /*--Display the contents of  the programme.--*/
    private SurfaceView svProgram;
    
    /*--Declare the database operation objects--*/
    private ServiceInfoDao serviceInfoDao;
    
    /*--Page --*/
    private PageDao pdTv;
    
    private PageDao pdRadio;
    
    private PageDao pdFav;
    
    /*--OK list--*/
    private LinearLayout llShowTVList;
    
    private ListView lvChannelList;
    
    private PlayProgramAdapter adapter;
    
    private List<ServiceInfoBean> currentList; // the current display list.
    
    private List<ServiceInfoBean> tvList;
    
    private List<ServiceInfoBean> radioList;
    
    private List<ServiceInfoBean> favList;
    
    private TextView tvTypeName;
    
    private String strTypeName;
    
    private String strTV;
    
    private String strRadio;
    
    private String strFav;
    
    /*--Channel name and channel number--*/
    private int servicePosition;
    
    private int logicalNumber;
    
    private String strChannelName;
    
    private int serviceId;
    
    private int tpId;
    
    private char tmpServiceType;
    
    /*--Paging changing the channel.--*/
    private int listViewize = 10; // per page size.
    
    private int channelIndex = 0; // the index in the list.
    
    private int currentPosition;
    
    private int currentTvPage = 1;
    
    private int currentRadioPage = 1;
    
    private int currentFavPage = 1;
    
    private int playPosition;
    
    private int tvFlag = 1;
    
    private int radioFlag = 1;
    
    private int favFlag = 1;
    
    /*--Ca mail.--*/
    private ImageView ivMail;
    
    /*--Ca IPPV.--*/
    private LinearLayout llNotice;
    
    private TextView tvCaPwd;
    
    private StringBuilder caPwdInput;
    
    private StringBuilder iStar;
    
    private int pincode;
    
    /*--Child Lock--*/
    private FrameLayout flMain;//finger print.
    private TextView tvFpRt;
    
    /*--PVR--*/
    private LinearLayout llPvr;
    
    private ImageView ivRec;
    
    private TextView tvPvrName;
    
    private TextView tvPvrOper;
    
    private ImageView ivPvrOper;
    
    private SeekBar sbPvr;
    
    private BitmapDrawable bdPlay;
    
    private BitmapDrawable bdStop;
    
    private BitmapDrawable bdSpeed;
    
    private BitmapDrawable bdBackward;
    
    private BitmapDrawable bdRec;
    
    private boolean isRec = false;
    private int caMode;
    private int currentFreq;
    private int currentSymb;
    private int currentEmod;
    private int lastFreq;
    private int lastSymb;
    private int lastEmod;
    private String recFileName;
    
//    private boolean tmsStartTag = true;
    
    private boolean isTs = false;
    
    private String strTsName;
    
    private int recPlayTag = 0; // 0 stop or unplayed, 1 play, 2 pause
    
    private int pvrSpeed = 0;
    
    private ScheduledExecutorService pvrService;
    
//    private PBIManager pbiManager;
    
    private String recFilePath;
    
    private PvrConn pvrConn;
    
    private PvrRecordServiceAIDL pvrRecService;
    
    private Dialog pvrDialog;
    
    /*--Force Switch--*/
    private Dialog caDialog;
    
    // private boolean isFirstFs = true;
    private boolean isIptv = true;
    
    /*--Mul language--*/
    private LinearLayout llSoundtrack;
    
    private TextView tvLanguage;
    
    private TextView tvSoundtrack;
    
    private SoundtrackBean soundtrackBean;
    
    /*--Program banner.--*/
    private LinearLayout llBanner;
    
    private TextView tvChannelName;
    
    private ImageView ivCurrentTypeTag;
    
    private ImageView ivLangTag;
    
    private ImageView ivSoundtrackTag;
    
    private ProgressBar pbPlayProgress;
    
    private TextView tvFirstEventTimeBucket;
    
    private TextView tvFirstEventRate;
    
    private TextView tvFirstEventName;
    
    private TextView tvSecondEventTimeBucket;
    
    private TextView tvSecondEventRate;
    
    private TextView tvSecondEventName;
    
    private BitmapDrawable tvTag;
    
    private BitmapDrawable radioTag;
    
    private BitmapDrawable stereoTag;
    
    private BitmapDrawable mixTag;
    
    private BitmapDrawable leftTag;
    
    private BitmapDrawable rightTag;
    
    /*--PPC parameters.--*/
    private TpInfoDao tpInfoDao;
    
    private int transportStreamId;
    
    private int networkId;
    
    private char serviceType;
    
    private boolean isConnect;
    
    /*--NIT update.--*/
    private Dialog nitDialog;
    
    private int curNitCode;
    
    /*--Declare tasks.--*/
    private Runnable hideChannelListTask;
    
    private Runnable hideChannelNameTask;
    
    private Runnable hidePassWordDialog;
    
    private Runnable hideChannelNumberTask;
    
    private Runnable playChannelTask;
    
    private Runnable showNameTask;
    
    private Runnable showPwdTask;
    
    private Runnable numberPlayChannelTask;
    
//    private Runnable getPlayProgressTask;
    
    private Runnable hideFingerPrintTask;
    
    private Runnable hideRecTask;
    
    private OTA_Service_Connection ota_Status_Service_Conn;
    
    // EPG detail
    private EpgDetailDialog epgDetailDialog;
    
    // private PiwikTrackerBehaviorSurveyCenter behaviorSurveyCenter;
    // private UserBehaviorSurveyCenter behaviorSurveyCenter;
    
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
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.channel_show);
    }
    
    protected void onStart()
    {
        super.onStart();
        //init left/right key to volume up/down function.
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_VOICE_CONTROL);
        /*forbid this key switch left/right and volume up/down function.*/
        CommonUtils.forbidSwitchKeyFunction(this,false);
    }
    
    @Override
    protected void onUserLeaveHint()
    {
        handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        super.onUserLeaveHint();
    }
    
    private class OTA_Service_Connection implements ServiceConnection
    {
        private OTADownloaderServiceAIDL otaServiceBinder = null;
        
        public OTADownloaderServiceAIDL getOtaServiceBinder()
        {
            return otaServiceBinder;
        }
        
        @Override
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            LogUtils.e(TAG, "onServiceConnected() run!!!");
            if (service == null)
            {
                LogUtils.e(TAG, "onServiceConnected:: service == null");
            }
            otaServiceBinder = OTADownloaderServiceAIDL.Stub.asInterface(service);
        }
        
        @Override
        public void onServiceDisconnected(ComponentName name)
        {
        }
    }
    
    private synchronized void connect_To_OTA()
    {
        // Check if the ota is in downloading status...
        Intent service = new Intent();
        service.setAction("com.pbi.dvb.service.ota_downloader_service");
        if (ota_Status_Service_Conn == null)
        {
            ota_Status_Service_Conn = new OTA_Service_Connection();
        }
        bindService(service, ota_Status_Service_Conn, Context.BIND_AUTO_CREATE);
    }
    
    private synchronized void disconnect_From_OTA()
    {
        if (ota_Status_Service_Conn != null && ota_Status_Service_Conn.getOtaServiceBinder() != null)
        {
            unbindService(ota_Status_Service_Conn);
            ota_Status_Service_Conn = null;
        }
    }
    
    protected void onPause()
    {
        super.onPause();
        LogUtils.printLog(1, 3, TAG, "------>>onPause Status!!<<-------");
        /*recovery this key switch left/right and volume up/down function.*/
        CommonUtils.forbidSwitchKeyFunction(this,true);
        // finish the tms.
        if (isTs)
        {
            LogUtils.printLog(1, 3, TAG, "------>>TS Stop!!!<<-------");
            recPlayTag = 0;
            dvbPlayer.DVBPlayerPvrPlayStop();
            dvbPlayer.DVBPlayerPvrRecStop();
            if (null != strTsName)
            {
                // remove the tms file.
                dvbPlayer.DVBPlayerPvrRemoveFile(strTsName);
                try
                {
                    // sleep 1s for close the drive.
                    Thread.sleep(1000);
                }
                catch (InterruptedException e)
                {
                    e.printStackTrace();
                }
            }
            isRec = false;
            isTs = false;
        }
        if (!isEpg)
        {
            dvbPlayer.DVBPlayerStop();
        }
        dvbPlayer.DVBPlayerPvrMessageUnInit();
        if (isIptv)
        {
            // destory the dvb player.
            dvbPlayer.DVBPlayerDeinit();
            nativeDrive.DrvPlayerUnInit();
        }
    }
    
    @Override
    protected void onResume()
    {
        super.onResume();
        LogUtils.printLog(1, 3, TAG, "------>>onResume Status!!<<-------");
        nativeDrive = new NativeDrive(TVChannelPlay.this);
        nativeDrive.pvwareDRVInit();
        nativeDrive.CAInit();
        nativeDrive.DrvPlayerInit();
        initWidget();
        connect_To_OTA();
	dvbPlayer.DVBPlayerAVCtrlSetVPathPara(1);
    }
    
    @Override
    protected void onRestart()
    {
        super.onRestart();
        LogUtils.printLog(1, 3, TAG, "------>>onRestart Status!!<<-------");
    }
    
    protected void onStop()
    {
        super.onStop();
        
        LogUtils.printLog(1, 3, TAG, "------>>Stop Status!!<<-------" + isIptv);
        
        disconnect_From_OTA();
        
        ServiceInfoBean program = paserLastProgram(Config.DVB_LASTPLAY_SERVICE);
        
        recordLastProgram(program);
        
        messageCa.SetReminderStatus(0);
        
        // reminderService.shutdown();
        
        // quitPPC();
        
        if (null != recReceiver)
        {
            unregisterReceiver(recReceiver);
        }
        
        // clear ca dialog
        if (null != caDialog)
        {
            caDialog.dismiss();
        }
        
        // clear pvr dialog
        if (null != pvrDialog)
        {
            pvrDialog.dismiss();
        }
        
        // stop get pvr progress.
        if (null != pvrService && !pvrService.isShutdown())
        {
            pvrService.shutdown();
        }
        
        // record rec program.
        if (isRec && !isTs)
        {
            recordRecProgram();
        }
        
        // behaviorSurveyCenter.stopTask();
        //
        // behaviorSurveyCenter = null;
    }
    
    private void quitPPC()
    {
        new Thread(new Runnable()
        {
            public void run()
            {
                String endPPC = "http://127.0.0.1:9906/?opt=channel_stop";
                setPPCRequest(endPPC);
                LogUtils.printLog(1, 3, TAG, "--------->>PPC Ending !!---------");
            }
        }).start();
    }
    
    private void showDialogMessage(int resId)
    {
        if (!llNotice.isShown() && !llInputPwd.isShown())
        {
            tvDialogMessage.setVisibility(View.VISIBLE);
            tvDialogMessage.setText(getString(resId));
            // tvDialogMessage.setTextColor(white);
        }
    }
    
    private void initWidget()
    {
        // Get program type logo.
        Resources res = getResources();
        tvTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.tv_type));
        radioTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.radio_type));
        stereoTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.stereo_tag));
        mixTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.mix_tag));
        leftTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.left_tag));
        rightTag = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.right_tag));
        
        // Init tasks.
        hideChannelListTask = new HideChannelListTask();
        hideChannelNameTask = new HideChannelName();
        hidePassWordDialog = new HideInputPwdDialog();
        hideChannelNumberTask = new HideChannelNumberTask();
        
        playChannelTask = new PlayProgramTask();
        showNameTask = new ShowChannelNameTask();
        showPwdTask = new ShowPwdDialogTask();
        numberPlayChannelTask = new NumberPlayProgramTask();
        
//        getPlayProgressTask = new GetPlayProgressTask();
        hideFingerPrintTask = new HideFingerPrint();
        hideRecTask = new HideRecTask();
        
        // Init widget.
        llShowTVList = (LinearLayout)this.findViewById(R.id.ll_channel_list);
        llInputPwd = (LinearLayout)this.findViewById(R.id.ll_player_pwd);
        llBanner = (LinearLayout)this.findViewById(R.id.ll_banner);
        
        tvPwdTitle = (TextView)this.findViewById(R.id.tv_pwd_dialog_title);
        tvPwd = (TextView)this.findViewById(R.id.tv_play_pwd);
        tvPwd.setOnKeyListener(new PasswordOnKeyListener());
        
        svProgram = (SurfaceView)this.findViewById(R.id.sv_face);
        svProgram.setOnClickListener(new ViewOnClickListener());
        
        tvTypeName = (TextView)this.findViewById(R.id.tv_channel_list_title);
        tvChannelName = (TextView)this.findViewById(R.id.tv_channel_name);
        tvChannelNumber = (TextView)this.findViewById(R.id.tv_channel_number);
        tvChannelName2 = (TextView)this.findViewById(R.id.tv_service_name);
        tvDialogMessage = (TextView)this.findViewById(R.id.tv_dialog_message);
        
        // Banner widgets.
        ivCurrentTypeTag = (ImageView)this.findViewById(R.id.iv_play_type_tag);
        ivLangTag = (ImageView)this.findViewById(R.id.iv_lang_tag);
        ivSoundtrackTag = (ImageView)this.findViewById(R.id.iv_soundtrack_tag);
        pbPlayProgress = (ProgressBar)this.findViewById(R.id.pb_play_progress);
        pbPlayProgress.setMax(100);
        
        tvFirstEventTimeBucket = (TextView)this.findViewById(R.id.tv_first_event_bucket);
        tvFirstEventRate = (TextView)this.findViewById(R.id.tv_first_event_rate);
        tvFirstEventName = (TextView)this.findViewById(R.id.tv_first_event_name);
        tvSecondEventTimeBucket = (TextView)this.findViewById(R.id.tv_second_event_bucket);
        tvSecondEventRate = (TextView)this.findViewById(R.id.tv_second_event_rate);
        tvSecondEventName = (TextView)this.findViewById(R.id.tv_second_event_name);
        
        // Ca mail widgets.
        ivMail = (ImageView)this.findViewById(R.id.iv_mail_tag);
        
        // Ca ippv widgets and finger print.
        llNotice = (LinearLayout)this.findViewById(R.id.ll_ca_notice);
        tvCaPwd = (TextView)this.findViewById(R.id.tv_ippv_pwd);
        tvCaPwd.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
        tvCaPwd.setOnKeyListener(new CaPasswordOnKeyListener());
        
        flMain = (FrameLayout)this.findViewById(R.id.fl_tv_show);
        
        LayoutParams params = new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        tvFpRt = new TextView(this);
        tvFpRt.setLayoutParams(params);
        tvFpRt.setBackgroundColor(Color.BLUE);
        tvFpRt.setVisibility(View.INVISIBLE);
        
        llSoundtrack = (LinearLayout)this.findViewById(R.id.ll_soundtrack);
        tvLanguage = (TextView)this.findViewById(R.id.tv_st_language);
        tvSoundtrack = (TextView)this.findViewById(R.id.tv_st_soundtrack);
        soundtrackBean = new SoundtrackBean(this);
        
        passwordInput = new StringBuilder();
        caPwdInput = new StringBuilder();
        iStar = new StringBuilder();
        
        // nit update message.
        MessageNit messageNit = new MessageNit(playerHandler, this);
        new Nativenit().NitMessageInit(messageNit);
        
        // register the service content observer.
        // serviceObserver = new ServiceInfoObserver(playerHandler);
        // getContentResolver().registerContentObserver(DBService.CONTENT_URI, false, serviceObserver);
        
        // set current time.
        MsgTimeUpdate msgTime = new MsgTimeUpdate(this);
        new Nativetime().TimeUpdateInit(msgTime);
        
        // pvr
        llPvr = (LinearLayout)this.findViewById(R.id.ll_pvr);
        ivRec = (ImageView)this.findViewById(R.id.iv_pvr_rec);
        sbPvr = (SeekBar)this.findViewById(R.id.sb_pvr);
        tvPvrName = (TextView)this.findViewById(R.id.tv_pvr_channel_name);
        tvPvrOper = (TextView)this.findViewById(R.id.tv_pvr_operation);
        ivPvrOper = (ImageView)this.findViewById(R.id.iv_pvr_operation);
        bdPlay = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_play));
        bdStop = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_stop));
        bdSpeed = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_speed));
        bdBackward = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_backward));
        bdRec = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_record));
        pvrConn = new PvrConn();
        
        // Init player, ca，epg message and ipp object.
        dvbPlayer = NativePlayer.getInstance();
        dvbPlayer.DVBPlayerInit(null);
        dvbPlayer.DVBPlayerSetStopMode(1);
        MessagePlayer messagePlayer = new MessagePlayer(playerHandler, this);
        dvbPlayer.DVBPlayerMessageInit(messagePlayer);
        
        // init pvr message.
        MessagePVR pvrMessage = new MessagePVR(pvrHandler);
        dvbPlayer.DVBPlayerPvrMessageInit(pvrMessage);
        
        // Set surfaceview's position and size.
        NativePlayer.DRV_AVCtrl_VidWindow_t playWindow = dvbPlayer.new DRV_AVCtrl_VidWindow_t();
        playWindow.setWinX(0);
        playWindow.setWinY(0);
        playWindow.setWinWidth(1920);
        playWindow.setWinHeight(1080);
        dvbPlayer.DVBPlayerAVCtrlSetWindows(playWindow, 1);
        
        NativeCA nativeCA = new NativeCA();
        messageCa = new MessageCa(caHandler, this);
        nativeCA.CaMessageInit(messageCa);
        
        // Set audio.
        audioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
        int volume = 0;
        volume = audioManager.getLastAudibleStreamVolume(audioManager.STREAM_SYSTEM);
        volume *= 7;
        if (100 < volume)
        {
            volume = 100;
        }
        dvbPlayer.DVBPlayerAVCtrlSetVolume(volume);
        g_bAudioIsMute = audioManager.isStreamMute(AudioManager.STREAM_MUSIC);
        if (true == g_bAudioIsMute)
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(1);
        }
        else
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(0);
        }
        
        // Init the Epg and Epg message.
        nativeEpg = new NativeEpg();
        NativeEpgMsg messageEpg = new NativeEpgMsg(bannerHandler);
        nativeEpg.EpgMessageInit(messageEpg);
        
        // Processing mail logic.
        MailDao mDao = new MailDaoImpl(this);
        long unreads = mDao.getUnreadMailInfo();
        if (unreads > 0)
        {
            if (ivMail.getVisibility() == View.INVISIBLE)
            {
                ivMail.setVisibility(View.VISIBLE);
            }
        }
        else
        {
            ivMail.setVisibility(View.INVISIBLE);
        }
        
        // Init ppc.
        tpInfoDao = new TpInfoDaoImpl(this);
        serviceInfoDao = new ServiceInfoDaoImpl(TVChannelPlay.this);
        
        // Get channel list.
        tvList = serviceInfoDao.getTVChannelInfo();
        radioList = serviceInfoDao.getRadioChannelInfo();
        favList = serviceInfoDao.getFavChannelInfo();
        
        // Init paging.
        pdTv = new PageDaoImpl(tvList, listViewize);
        pdRadio = new PageDaoImpl(radioList, listViewize);
        pdFav = new PageDaoImpl(favList, listViewize);
        lvChannelList = (ListView)this.findViewById(R.id.lv_channel_list);
        
        // Get okList title.
        strTV = getString(R.string.TVChannel);
        strRadio = getString(R.string.BroadcastPrograms);
        strFav = getString(R.string.FavPrograms);
        
        // Judgment whether TV-playing or radio-playing.
        adapter = new PlayProgramAdapter(this);
        Bundle data = this.getIntent().getExtras();
        char titName = data.getChar("ServiceType");
        tmpServiceType = titName;
        strChannelName = data.getString("ServiceName");
        serviceId = data.getInt("ServiceId");
        tpId = data.getInt("TpId");
        servicePosition = data.getInt("ServicePos");
        logicalNumber = data.getInt("LogicalNumber");
        channelIndex = servicePosition - 1;
        playPosition = channelIndex % listViewize;
        try
        {
            superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
            originalGrade = android.provider.Settings.System.getInt(getContentResolver(), "eit_grade");
        }
        catch(Exception e)
        {
            superPwd = Config.PROGRAMME_LOCK;
            originalGrade = 1;// default value.
            e.printStackTrace();
        }
        eitLimitcategorys = CommonUtils.getNitCategorySettings(this);
        currentPosition = servicePosition - 1;
        
        lvChannelList.setAdapter(adapter);
        lvChannelList.setOnItemClickListener(new ChannelOnItemClickListener());
        lvChannelList.setOnItemSelectedListener(new ChannelOnItemSelectedListener());
        lvChannelList.setOnKeyListener(new ChannelOnKeyListener());
        
        // Set the current playing program's name in the banner.
        tvChannelName.setText(logicalNumber + "." + strChannelName);
        SurfaceHolder shHolder = svProgram.getHolder();
        shHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        shHolder.addCallback(mSHCallback);
        shHolder.setFormat(PixelFormat.VIDEO_HOLE);
        
        switchProgram(serviceId, logicalNumber, true);
        // Set origination ok list contents and program's logo in the banner.
        if (titName == Config.SERVICE_TYPE_RADIO)
        {
            adapter.setTvProgramList(radioList);
            setFocusInRadios();
        }
        else
        {
            adapter.setTvProgramList(tvList);
            setFocusInTvs();
        }
        
        // get rec status
        isRec = data.getBoolean("RecStatus");
//        isRec = true;
        if (isRec)
        {
            recFileName = data.getString("ServiceName");
            showPvrBanner();
        }
        
        registReceiver();
        
        new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                isConnect = CommonUtils.sendHttpRequest(0);
            }
        }).start();
    }
    
    private void registReceiver()
    {
        IntentFilter filter = new IntentFilter();
        filter.addAction(Config.PVR_TASK_COMPLETE);
        registerReceiver(recReceiver, filter);
    }
    
    private DVBPlayer_Play_Param_t getPlayerParams(int sId, int lnumber)
    {
        // Init the play params object instance.
        Tuner tuner = new Tuner();
        NativePlayer.DVBPlayer_Play_Param_t tParam = dvbPlayer.new DVBPlayer_Play_Param_t();
        Tuner.DVBCore_Cab_Desc_t tCable = tuner.new DVBCore_Cab_Desc_t();
        NativePlayer.DVBPlayer_Serv_Param_t tServParam = dvbPlayer.new DVBPlayer_Serv_Param_t();
        
        ServiceInfoBean serviceInfo = serviceInfoDao.getChannelInfoByServiceId(sId, lnumber);
        if (null != serviceInfo)
        {
            tpId = serviceInfo.getTpId();
            TPInfoBean tpInfo = tpInfoDao.getTpInfo(tpId);
            
            // Get ppc and epg params
            transportStreamId = tpInfo.getTranStreamId();
            networkId = tpInfo.getOriginalNetId();
            serviceId = serviceInfo.getServiceId();
            logicalNumber = serviceInfo.getLogicalNumber();
            servicePosition = serviceInfo.getChannelPosition();
            currentPosition = servicePosition - 1;
            serviceType = serviceInfo.getServiceType();
            caMode = serviceInfo.getCaMode();
            strChannelName = serviceInfo.getChannelName();
            
            int eProgType = Config.DVBPlayer_Prog_Type_SERVICE;
            
            // Judgment audio type.
            int audioPid = serviceInfo.getAudioPid();
            // audioLang = serviceInfo.getAudioLang();
            
            /*
             * the service lock only get once, and it has the highest priority.
             */
            if (firGetServiceLock)
            {
                if (serviceInfo.getLockFlag() == 1)
                {
                    // set the service lock.
                    serviceLock = true;
                    
                    // set the password dialog title.
                    tvPwdTitle.setText(getString(R.string.password_programme));
                    handler.removeCallbacks(showPwdTask);
                    // handler.post(showPwdTask);
                    // delay 500 millisecond, so when the password dialog is shown, the player is stopped.
                    handler.postDelayed(showPwdTask, 500);
                }
            }
            
            // Package the params of DVBCore_Cab_Desc_t
            currentFreq = tpInfo.getTunerFreq();
            currentSymb = tpInfo.getTunerSymbRate();
            currentEmod = tpInfo.getTunerEmod();
            tCable.setU8TunerId(tpInfo.getTunerId());
            tCable.setU32Freq(currentFreq);
            tCable.setU32SymbRate(currentSymb);
            tCable.seteMod(currentEmod);
            tCable.seteFEC_Inner(tpInfo.getEfecInner());
            tCable.seteFEC_Outer(tpInfo.getEfecOuter());
            
            // Package the params of DVBPlayer_Serv_Param_t.
            tServParam.seteServLockFlag(0);
            tServParam.setU16PMTPid(serviceInfo.getPmtPid());
            tServParam.setU16ServId(serviceId);
            tServParam.setU16NetId(networkId);
            tServParam.setU16TsId(transportStreamId);
            tServParam.setU16LogicNum(logicalNumber);
            tServParam.setu16AudioPID(audioPid);
            
            // Package the params of DVBPlayer_Play_Param_t.
            tParam.seteSignalSource(Config.DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBC);
            tParam.settCable(tCable);
            tParam.seteProgType(eProgType);
            tParam.settServParam(tServParam);
        }
        else
        {
            showToast(getString(R.string.invilid_programme), 1);
        }
        
        return tParam;
    }
    
    @SuppressWarnings("unchecked")
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        if (KeyEvent.ACTION_DOWN == event.getAction())
        {
            // page down
            if (event.getKeyCode() == Config.PAGE_DOWN)
            {
                if (isTs)
                {
                    speedTimeShift();
                    return false;
                }
                
                if (llShowTVList.isShown())
                {
                    // -The name of ok-list title is tv programs.
                    if (strTypeName.equals(strTV))
                    {
                        if (pdTv.getCurrentPage() != pdTv.getPageNum())
                        {
                            pdTv.nextPage();
                            currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            currentTvPage++;
                            adapter.notifyDataSetChanged();
                        }
                        else
                        {
                            pdTv.headPage();
                            currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentTvPage = 1;
                        }
                    }
                    // -The name of ok-list title is radio programs.
                    else if (strTypeName.equals(strRadio))
                    {
                        if (pdRadio.getCurrentPage() != pdRadio.getPageNum())
                        {
                            pdRadio.nextPage();
                            currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            currentRadioPage++;
                            adapter.notifyDataSetChanged();
                        }
                        else
                        {
                            pdRadio.headPage();
                            currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentRadioPage = 1;
                        }
                    }
                    // -The name of ok-list title is fav programs.
                    else if (strTypeName.equals(strFav))
                    {
                        if (pdFav.getCurrentPage() != pdFav.getPageNum())
                        {
                            pdFav.nextPage();
                            currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            currentFavPage++;
                            adapter.notifyDataSetChanged();
                        }
                        else
                        {
                            pdFav.headPage();
                            currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentFavPage = 1;
                        }
                    }
                }
                // return true;
            }
            // page up
            if (event.getKeyCode() == Config.PAGE_UP)
            {
                if (isTs)
                {
                    reverseTimeShift();
                    return false;
                }
                if (llShowTVList.isShown())
                {
                    // -The name of ok-list title is tv programs.
                    if (strTypeName.equals(strTV))
                    {
                        if (pdTv.getCurrentPage() != 1)
                        {
                            pdTv.prePage();
                            currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentTvPage--;
                        }
                        else
                        {
                            pdTv.lastPage();
                            currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentTvPage = pdTv.getPageNum();
                        }
                    }
                    // -The name of ok-list title is radio programs.
                    else if (strTypeName.equals(strRadio))
                    {
                        if (pdRadio.getCurrentPage() != 1)
                        {
                            pdRadio.prePage();
                            currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentRadioPage--;
                        }
                        else
                        {
                            pdRadio.lastPage();
                            currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentRadioPage = pdRadio.getPageNum();
                        }
                    }
                    // -The name of ok-list title is fav programs.
                    else if (strTypeName.equals(strFav))
                    {
                        if (pdFav.getCurrentPage() != 1)
                        {
                            pdFav.prePage();
                            currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentFavPage--;
                        }
                        else
                        {
                            pdFav.lastPage();
                            currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                            adapter.setTvProgramList(currentList);
                            adapter.notifyDataSetChanged();
                            currentFavPage = pdFav.getPageNum();
                        }
                    }
                }
            }
            // return true;
        }
        return super.dispatchKeyEvent(event);
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        LogUtils.printLog(1, 3, TAG, "----onKeyDown keyCode---->>>" + keyCode);
         switch (keyCode)
         {
         /*-------------Press the volume down key-----------------*/
         case Config.KEY_VOLUME_DOWN:
         volumeDownMethod();
         break;
         /*-------------Press the volume up key-----------------*/
         case Config.KEY_VOLUME_UP:
         volumeUpMethod();
         break;
         }
        if (event.getRepeatCount() == 0)
        {
            switch (keyCode)
            {
            /*-------------Press the down key-----------------*/
                case KeyEvent.KEYCODE_DPAD_DOWN:
                    if (!llNotice.isShown() && !llSoundtrack.isShown())
                    {
                        downKeyMethod();
                    }
                    break;
                /*-------------Press the up key-----------------*/
                case KeyEvent.KEYCODE_DPAD_UP:
                    if (!llNotice.isShown() && !llSoundtrack.isShown())
                    {
                        upKeyMethod();
                    }
                    break;
                /*-------------Press the ok key-----------------*/
                case KeyEvent.KEYCODE_DPAD_CENTER:
                    if (!llSoundtrack.isShown())
                    {
                        showOKList();
                    }
                    break;
                /*-------------Press the number key-----------------*/
                case KeyEvent.KEYCODE_0:
                case KeyEvent.KEYCODE_1:
                case KeyEvent.KEYCODE_2:
                case KeyEvent.KEYCODE_3:
                case KeyEvent.KEYCODE_4:
                case KeyEvent.KEYCODE_5:
                case KeyEvent.KEYCODE_6:
                case KeyEvent.KEYCODE_7:
                case KeyEvent.KEYCODE_8:
                case KeyEvent.KEYCODE_9:
                    return numberKeyMethod(keyCode);
                    /*-------------Press the mute key-----------------*/
                case Config.KEY_MUTE:
                    muteKeyMethod();
                    break;
                /*-------------Press the epg key-----------------*/
                case Config.KEY_EPG:
                    isEpg = true;
                    handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                    epgKeyMethod();
                    break;
//                case Config.KEY_SEARCH:
//                    if (null != searchDialog && searchDialog.isShowing())
//                    {
//                        return false;
//                    }
//                    fastAutoSearch();
//                    break;
                /*-------------Press the left key-----------------*/
                case KeyEvent.KEYCODE_DPAD_LEFT:
                    leftKeyMethod();
                    break;
                /*-------------Press the right key-----------------*/
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                    rightKeyMethod();
                    break;
                /*-------------Press the TV/Radio switch key-----------------*/
                case Config.KEY_SWITCH:
                    switchKeyMethod();
                    break;
                /*-------------Press the multi-language key-----------------*/
                case Config.KEY_MUL_LANGUAGE:
                    clearDialogs();
                    handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                    langKeyMethod();
                    break;
                /*-------------Press the back key-----------------*/
                case KeyEvent.KEYCODE_BACK:
                    backAndStopTmsFunction();
                    return false;
                case Config.KEY_SHOW:
                    if (isTs)
                    {
                        if (llPvr.isShown())
                        {
                            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                            llPvr.setVisibility(View.INVISIBLE);
                        }
                        else
                        {
                            llPvr.setVisibility(View.VISIBLE);
                        }
                        return false;
                    }
                    if (llSoundtrack.isShown())
                    {
                        llSoundtrack.setVisibility(View.INVISIBLE);
                    }
                    /* Start LinKang, 2013-12-24, Add the EPG detail content page... */
                    // if banner is shown, show EPG detail dialog
                    if (llBanner.isShown())
                    {
                        ServiceInfoBean serviceBean =
                            serviceInfoDao.getChannelInfoByServiceId(serviceId, logicalNumber);
                        
                        TPInfoBean tpInfoBean = tpInfoDao.getTpInfo(serviceBean.getTpId());
                        short serviceid = serviceBean.getServiceId().shortValue();
                        short tsid = tpInfoBean.getTranStreamId().shortValue();
                        short ornetowrkid = tpInfoBean.getOriginalNetId().shortValue();
                        
                        NativeEpg.extentinfo extentInfo = nativeEpg.new extentinfo();
                        String noContent = getResources().getString(R.string.epg_detail_no_content);
                        String firstExtendInfo = null;
                        nativeEpg.GetOneEventExtentInfo(serviceid,
                            tsid,
                            ornetowrkid,
                            (char)0,
                            (char)0,
                            (short)1,
                            extentInfo);
                        firstExtendInfo = extentInfo.getextentinfo();
                        String secondExtendInfo = null;
                        NativeEpg.extentinfo extentInfo2 = nativeEpg.new extentinfo();
                        nativeEpg.GetOneEventExtentInfo(serviceid,
                            tsid,
                            ornetowrkid,
                            (char)0,
                            (char)0,
                            (short)2,
                            extentInfo2);
                        secondExtendInfo = extentInfo2.getextentinfo();
                        
                        if (null == firstExtendInfo || "".equals(firstExtendInfo))
                        {
                            firstExtendInfo = noContent;
                        }
                        if (null == secondExtendInfo || "".equals(secondExtendInfo))
                        {
                            secondExtendInfo = noContent;
                        }
                        
                        List<String> extendInfos = new ArrayList<String>();
                        extendInfos.add(firstExtendInfo);
                        extendInfos.add(secondExtendInfo);
                        
                        LogUtils.e(TAG, "TVChannelPlay.onKeyDown:: firstExtendInfo = " + firstExtendInfo
                            + " secondExtendInfo = " + secondExtendInfo);
                        
                        if (null == epgDetailDialog)
                        {
                            epgDetailDialog =
                                new EpgDetailDialog(TVChannelPlay.this, R.layout.text_scroll_dialog, extendInfos,handler);
                            
                            epgDetailDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
                            
                            epgDetailDialog.setCallBack(new EpgDetailDialogCallBack()
                            {
                                @Override
                                public void onShowKey()
                                {
                                    CommonUtils.setLeftRightFunction(Config.SWITCH_TO_VOICE_CONTROL);
                                }
                            });
                            
                        }
                        else
                        {
                            epgDetailDialog.setNewEpgContent(extendInfos);
                        }
                        epgDetailDialog.show();
                        LogUtils.e(Config.DVB_TAG, "TVChannelPlay.onKeyDown:: epgDetailDialog.show() run...");
                        return true;
                    }
                    else
                    {
                        // show llBanner.
                        handler.removeCallbacks(showNameTask);
                        handler.post(showNameTask);
                        
                        handler.removeCallbacks(hideChannelNameTask);
                        handler.postDelayed(hideChannelNameTask, 1000 * 3);
                        handler.removeCallbacks(hideChannelNumberTask);
                        handler.postDelayed(hideChannelNumberTask, 1000 * 3);
                    }
                    break;

                case Config.KEY_FAST_CHANGE:
                    lookBackProgramme();
                    break;
                /*-------------Press the record key-----------------*/
                // case Config.KEY_REC:
                // if (!llSoundtrack.isShown())
                // {
                // recKeyMethod();
                // }
                // break;
                /*-------------Press the blue key-----------------*/
                case Config.KEY_PLAY_PAUSE:
                    if (llSoundtrack.isShown())
                    {
                        llSoundtrack.setVisibility(View.INVISIBLE);
                    }
                    // hide ok-list.
                    if (llShowTVList.isShown())
                    {
                        handler.post(hideChannelListTask);
                    }
                    if(isRec && !isTs)
                    {
                        LogUtils.printLog(1, 3, TAG, "--->>>Before you start tms function, stop background rec first!");
                        showStopRecDialog(getString(R.string.end_pvr1)+recFileName +"\n"+getString(R.string.end_pvr2));
                        return false;
                    }
                    startTimeShift();
                    break;
                /*-------------Press the yellow(F3) key---------------*/
                case Config.KEY_YELLOW:
                    CommonUtils.skipActivity(this,CaEmail.class , 0);
                    break;
            /*-------------Press the green key-----------------*/
             case Config.KEY_GREEN:
                    LogUtils.printLog(1, 3, TAG, " send request " + isConnect);
                    startCloudPvr();
             break;
            /*-------------Press the red key-----------------*/
            // case Config.KEY_RED:
            // if (null != f1Dialog)
            // {
            // f1Dialog.dismiss();
            // }
            // break;
            // case Config.KEY_FAV:
            // tvTypeName.setText(strFav);
            // strTypeName = strFav;
            // showOKList();
            // break;
            }
        }
        return super.onKeyDown(keyCode, event);
    }
    
    private void startCloudPvr()
    {
        if (isConnect)
        {
            isEpg = true;
            
            // set the left and right function.
            // setLeftRightFunction(1);
            Intent pvrIntent = new Intent();
            pvrIntent.setClassName("com.pbi.dvb", "com.pbi.dvb.nppvware.NpPvwareActivity");
            pvrIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            
            Bundle bundle = new Bundle();
            bundle.putInt("logical_number", logicalNumber);
            bundle.putString("service_name", strChannelName);
            bundle.putInt("frequency", playParams.gettCable().getU32Freq());
            bundle.putInt("symbrate", playParams.gettCable().getU32SymbRate());
            bundle.putInt("qam", playParams.gettCable().geteMod());
            bundle.putInt("service_id", playParams.gettServParam().getU16ServId());
            bundle.putInt("pmt_pid", playParams.gettServParam().getU16PMTPid());
            
            if (strTypeName.equalsIgnoreCase(strRadio))
            {
                bundle.putString("dvb_current_service_type", Config.DVB_RADIO_PROGRAMME);
            }
            else
            {
                bundle.putString("dvb_current_service_type", Config.DVB_TV_PROGRAMME);
            }
            pvrIntent.putExtras(bundle);
            startActivity(pvrIntent);
            return;
        }
        LogUtils.printLog(1, 3, TAG, "--->>> connect the cloud pvr server failed!");
    }
    
    private boolean numberKeyMethod(int keyCode)
    {
        if (!llShowTVList.isShown() && !llNotice.isShown()&& !llSoundtrack.isShown())
        {
            // tvChannelNumber.bringToFront();
            tvChannelNumber.requestFocus();
            tvChannelNumber.setFocusable(true);
            tvChannelNumber.setVisibility(View.VISIBLE);
            
            tvChannelName2.setText(null);
            tvChannelName2.setVisibility(View.VISIBLE);
            
            numberInput *= 10;
            numberInput += keyCode - KeyEvent.KEYCODE_0;
            LogUtils.printLog(1, 3, TAG, "----Number Input---->>>" + numberInput);
            tvChannelNumber.setText("" + numberInput);
            // tvChannelNumber.invalidate();
            
            if (numberInput < 1000)
            {
                handler.removeCallbacks(numberPlayChannelTask);
                handler.postDelayed(numberPlayChannelTask, 1500);
            }
        }
        return true;
    }
    
    private void backAndStopTmsFunction()
    {
        if (llSoundtrack.isShown())
        {
            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
            llSoundtrack.setVisibility(View.INVISIBLE);
            int audioPid = soundtrackBean.parserLang(tvLanguage.getText().toString());
            int audioMode = soundtrackBean.parserSoundtrack(tvSoundtrack.getText().toString());
            LogUtils.printLog(1, 3, TAG, "--->>> current audio pid (audio mode) is: " + audioPid + "(" + audioMode
                + ") will save to db.");
            serviceInfoDao.updateAudioInfo(serviceId, audioPid, 0, audioMode);
            return;
        }
        if (isRec)
        {
            if (isTs)
            {
                // show stop tms dialog.
                showStopRecDialog(getString(R.string.end_tms));
                return;
            }
            showStopRecDialog(getString(R.string.end_pvr1) + recFileName + "\n" + getString(R.string.end_pvr2));
        }
        finish();
    }
    
    private void showStopRecDialog(String message)
    {
        handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        // exit rec function.
        if (isRec)
        {
            pvrDialog =
                DialogUtils.twoButtonsDialogCreate(TVChannelPlay.this,
                    message,
                    new PVRPositiveButtonListener(),
                    new OnClickListener()
                    {
                        @Override
                        public void onClick(View v)
                        {
                            if (null != pvrDialog)
                            {
                                pvrDialog.dismiss();
                                LogUtils.e(TAG,
                                    "TVChannelPlay.backAndStopTmsFunction:: setLeftRightFunction to voice pvrDialog.dismiss();");
                                handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                            }
                        }
                    });
        }
    }
    
    private void showPvrBanner()
    {
        // hide epg banner.
        if (llBanner.isShown())
        {
            handler.post(hideChannelNameTask);
        }
        
        //show pvr banner.
        llPvr.setVisibility(View.VISIBLE);
        sbPvr.setFocusable(true);
        if (!ivRec.isShown())
        {
            if (isTs)
            {
                ivRec.setBackgroundResource(R.drawable.pvr_tms);
            }
            else
            {
                ivRec.setBackgroundResource(R.drawable.pvr_rec);
                 handler.removeCallbacks(hideRecTask);
                 handler.postDelayed(hideRecTask, 10 * 1000);
            }
            ivRec.setVisibility(View.VISIBLE);
        }
        tvPvrName.setText(logicalNumber + "." + strChannelName);
        
    }
    
    private void showToast(String content, int time)
    {
        CustomToast toast = new CustomToast(this);
        toast.show(content, time);
    }
    
//    private String getPvrFileName()
//    {
//        Calendar c = Calendar.getInstance();
//        int cYear = c.get(Calendar.YEAR);
//        String cMonth = CommonUtils.alignCharacter(1 + c.get(Calendar.MONTH));
//        String cDay = CommonUtils.alignCharacter(c.get(Calendar.DAY_OF_MONTH));
//        String cHour = CommonUtils.alignCharacter(c.get(Calendar.HOUR_OF_DAY));
//        String cMinute = CommonUtils.alignCharacter(c.get(Calendar.MINUTE));
//        String cSecond = CommonUtils.alignCharacter(c.get(Calendar.SECOND));
//        return "/" + "[" + strChannelName + "][" + cYear + "-" + cMonth + "-" + cDay + "_" + cHour + "-" + cMinute
//            + "-" + cSecond + "].ts";
//    }
    
    private void muteKeyMethod()
    {
        g_bAudioIsMute = !g_bAudioIsMute;
        if (true == g_bAudioIsMute)
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(1);
        }
        else
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(0);
        }
    }
    
    private void speedTimeShift()
    {
        // do not start timeshift.
        if (!isTs)
        {
            return;
        }
        if (!llPvr.isShown())
        {
            showPvrBanner();
        }
        if (recPlayTag != 0)
        {
            if (pvrSpeed < 5)
            {
                pvrSpeed = 5;
            }
            pvrSpeed++;
            
            if (pvrSpeed > 10)
            {
                pvrSpeed = 5;
            }
            LogUtils.printLog(1, 3, TAG, "---------------------->>>>STEP SPEED<<<--------------" + pvrSpeed);
            switch (pvrSpeed)
            {
                case 6:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>SPEED X2<<<--------------");
                    tvPvrOper.setText("X2");
                    ivPvrOper.setImageDrawable(bdSpeed);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 7:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>SPEED X4<<<--------------");
                    tvPvrOper.setText("X4");
                    ivPvrOper.setImageDrawable(bdSpeed);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 8:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>SPEED X8<<<--------------");
                    tvPvrOper.setText("X8");
                    ivPvrOper.setImageDrawable(bdSpeed);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 9:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>SPEED X16<<<--------------");
                    tvPvrOper.setText("X16");
                    ivPvrOper.setImageDrawable(bdSpeed);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 10:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>SPEED X32<<<--------------");
                    tvPvrOper.setText("X32");
                    ivPvrOper.setImageDrawable(bdSpeed);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                default:
                    tvPvrOper.setText(getString(R.string.pvr_start));
                    ivPvrOper.setImageDrawable(bdPlay);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(5);
                    break;
            }
        }
    }
    
    private void reverseTimeShift()
    {
        if (!isTs)
        {
            return;
        }
        if (!llPvr.isShown())
        {
            showPvrBanner();
        }
        if (recPlayTag != 0)
        {
            if (pvrSpeed > 5)
            {
                pvrSpeed = 5;
            }
            pvrSpeed--;
            if (pvrSpeed < 0)
            {
                pvrSpeed = 5;
            }
            LogUtils.printLog(1, 3, TAG, "---------------------->>>>Speed -- <<<--------------" + pvrSpeed);
            switch (pvrSpeed)
            {
                case 4:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>X2<<<--------------");
                    tvPvrOper.setText("X2");
                    ivPvrOper.setImageDrawable(bdBackward);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 3:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>X4<<<--------------");
                    tvPvrOper.setText("X4");
                    ivPvrOper.setImageDrawable(bdBackward);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 2:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>X8<<<--------------");
                    tvPvrOper.setText("X8");
                    ivPvrOper.setImageDrawable(bdBackward);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 1:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>X16<<<--------------");
                    tvPvrOper.setText("X16");
                    ivPvrOper.setImageDrawable(bdBackward);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                case 0:
                    LogUtils.printLog(1, 3, TAG, "---------------------->>>>X32<<<--------------");
                    tvPvrOper.setText("X32");
                    ivPvrOper.setImageDrawable(bdBackward);
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
                    break;
                default:
                    ivPvrOper.setImageDrawable(bdPlay);
                    tvPvrOper.setText(getString(R.string.pvr_start));
                    dvbPlayer.DVBPlayerPvrPlaySpeedCtrl(5);
                    break;
            }
        }
    }
    
    private void startTimeShift()
    {
        // start rec function.
        if (!isRec)
        {
            // clear seekbar buffer.
            sbPvr.setProgress(0);
            sbPvr.invalidate();
            
            // check the storage device;
            StorageUtils util = new StorageUtils(this);
            MountInfoBean mountInfoBean = util.getMobileHDDInfo();
            
            // no devices.
            if (null == mountInfoBean)
            {
                showToast(getString(R.string.hdd_no), Toast.LENGTH_SHORT);
                return;
            }
            LogUtils.printLog(1, 3, TAG, "--->>>storage free size is: " + mountInfoBean.getFreeSize());
            LogUtils.printLog(1, 3, TAG, "--->>>storage path is:" + mountInfoBean.getPath());
            
            int status = util.checkHDDSpace();
            if (status == 1)
            {
                LogUtils.printLog(1, 3, TAG, "--->>> storage device has no enough space.");
                showToast(getString(R.string.hdd_notEnough), Toast.LENGTH_SHORT);
                return;
            }
            else if (status == 2)
            {
                LogUtils.printLog(1, 3, TAG, "--->>> storage device has not found.");
                showToast(getString(R.string.hdd_no), Toast.LENGTH_SHORT);
                return;
            }
            LogUtils.printLog(1, 3, TAG, "--->>>start tms function.");
            
            ivPvrOper.setImageDrawable(bdStop);
            tvPvrOper.setText(getString(R.string.pvr_pause));
            
            // set ts file's name.
            strTsName = mountInfoBean.getPath() + "/time.ts";
            
            // start to ts rec.
            DvbPVR_Rec_t tPvrRecInfo = dvbPlayer.new DvbPVR_Rec_t();
            tPvrRecInfo.u8RecFlag = 2;
            tPvrRecInfo.sName = strTsName;
            
            // the usb storage must keep 50M free size.
            tPvrRecInfo.u64MaxFileLength = mountInfoBean.getFreeSize() - 50;
            dvbPlayer.DVBPlayerPvrRecStart(tPvrRecInfo);
            
            try
            {
                // get seekbar's progress.
                pvrService = Executors.newScheduledThreadPool(1);
                pvrService.scheduleWithFixedDelay(new GetTmsProgress(pvrHandler), 0, 2, TimeUnit.SECONDS);
            }
            catch (Exception e)
            {
                LogUtils.printLog(1, 3, TAG, "--->>> get tms progress exception.");
                e.printStackTrace();
            }
            
            // change the ts tag.
            isRec = true;
            isTs = true;
        }
        
        // according this tag,judge tms status.
        switch (recPlayTag)
        {
        // not play status. The initialize operation in the start method.
            case 0:
                LogUtils.printLog(1, 3, TAG, "--->>>tms init opeartion.");
                ivPvrOper.setImageDrawable(bdStop);
                tvPvrOper.setText(getString(R.string.pvr_pause));
                
                // init tms player.
                DvbPVR_Play_t tPvrPlay = dvbPlayer.new DvbPVR_Play_t();
                tPvrPlay.PvrPlayerType = 0;
                tPvrPlay.sName = strTsName;
                dvbPlayer.DVBPlayerPvrPlayStart(tPvrPlay);
                
                recPlayTag = 2;
                break;
            // pause
            case 1:
                LogUtils.printLog(1, 3, TAG, "--->>> tms pause status.");
                ivPvrOper.setImageDrawable(bdStop);
                tvPvrOper.setText(getString(R.string.pvr_pause));
                dvbPlayer.DVBPlayerPvrPlayPause();
                recPlayTag = 2;
                break;
            // play
            case 2:
                LogUtils.printLog(1, 3, TAG, "--->>> tms play status.");
                ivPvrOper.setImageDrawable(bdPlay);
                tvPvrOper.setText(getString(R.string.pvr_start));
                dvbPlayer.DVBPlayerPvrPlayResume();
                recPlayTag = 1;
                pvrSpeed = 5;// set the common play speed.
                break;
        }
        // show the tms banner.
        showPvrBanner();
    }
    
    private void langKeyMethod()
    {
        if (!ivRec.isShown())
        {
            clearDialogs();
            llSoundtrack.setVisibility(View.VISIBLE);
            
            tvLanguage.requestFocus();
            tvLanguage.setFocusable(true);
            tvLanguage.setFocusableInTouchMode(true);
            
            tvLanguage.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
            tvSoundtrack.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
            
            LogUtils.printLog(1, 3, TAG, "--->>>current lang is: " + soundtrackBean.getCurrentLang(serviceId, logicalNumber));
            LogUtils.printLog(1,
                3,
                TAG,
                "--->>>current soundtrack is: " + soundtrackBean.getCurrentSoundtrack(serviceId, logicalNumber));
            // init the language.
            if (null != soundtrackBean)
            {
                tvLanguage.setText(soundtrackBean.getCurrentLang(serviceId, logicalNumber));
                tvSoundtrack.setText(soundtrackBean.getCurrentSoundtrack(serviceId, logicalNumber));
            }
        }
    }
    
    private void switchKeyMethod()
    {
        if (!ivRec.isShown())
        {
            clearDialogs();
            SharedPreferences spChannel = getSharedPreferences(Config.DVB_LASTPLAY_SERVICE, 8);
            
            // get radio type params.
            int radioServiceId = spChannel.getInt("radio_serviceId", 0);
            int radioNumber = spChannel.getInt("radio_logicalNumber", 0);
            
            // get tv type params.
            int tvServiceId = spChannel.getInt("tv_serviceId", 0);
            int tvNumber = spChannel.getInt("tv_logicalNumber", 0);
            
            // check analytical program effectiveness.
            ServiceInfoBean rBean = serviceInfoDao.getChannelInfoByServiceId(radioServiceId, radioNumber);
            ServiceInfoBean tBean = serviceInfoDao.getChannelInfoByServiceId(tvServiceId, tvNumber);
            
            // current play service type is radio,switch to tv.
            if (strTypeName.equals(strRadio))
            {
                if (tvList.size() != 0)
                {
                    tvTypeName.setText(strTV);
                    strTypeName = strTV;
                    
                    // invalid tv program.
                    if (null == tBean)
                    {
                        ServiceInfoBean tvBean = tvList.get(0);
                        tvServiceId = tvBean.getServiceId();
                        tvNumber = tvBean.getLogicalNumber();
                    }
                    switchProgram(tvServiceId, tvNumber, false);
                    setFocusInTvs();
                }
            }
            // switch to radio.
            else
            {
                if (radioList.size() != 0)
                {
                    tvTypeName.setText(strRadio);
                    strTypeName = strRadio;
                    
                    // invalid radio program.
                    if (null == rBean)
                    {
                        ServiceInfoBean radioBean = radioList.get(0);
                        radioServiceId = radioBean.getServiceId();
                        radioNumber = radioBean.getLogicalNumber();
                    }
                    switchProgram(radioServiceId, radioNumber, false);
                    setFocusInRadios();
                }
            }
        }
    }
    
    @SuppressWarnings("unchecked")
    private void rightKeyMethod()
    {
        if (!ivRec.isShown())
        {
            if (llShowTVList.isShown())
            {
                strTypeName = tvTypeName.getText().toString();
                if (strTypeName.equals(strFav))
                {
                    tvTypeName.setText(strTV);
                    strTypeName = strTV;
                    currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                }
                else if (strTypeName.equals(strRadio))
                {
                    tvTypeName.setText(strFav);
                    strTypeName = strFav;
                    currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                }
                else if (strTypeName.equals(strTV))
                {
                    tvTypeName.setText(strRadio);
                    strTypeName = strRadio;
                    currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                }
                
                channelIndex = 0;
                lvChannelList.setSelection(0);
                lvChannelList.requestFocus(0);
                
                adapter.setTvProgramList(currentList);
                adapter.notifyDataSetChanged();
            }
            else if (!llShowTVList.isShown() && llSoundtrack.isShown())
            {
                if (tvLanguage.hasFocus())
                {
                    String nextLang = soundtrackBean.getNextLang();
                    tvLanguage.setText(nextLang);
                }
                else if (tvSoundtrack.hasFocus())
                {
                    String nextSoundtrack = soundtrackBean.getNextSoundtrack(tvSoundtrack.getText().toString());
                    tvSoundtrack.setText(nextSoundtrack);
                }
            }
        }
    }
    
    @SuppressWarnings("unchecked")
    private void leftKeyMethod()
    {
        if (!ivRec.isShown())
        {
            if (llShowTVList.isShown())
            {
                strTypeName = tvTypeName.getText().toString();
                LogUtils.printLog(1, 3, TAG, "-----------Current-OK-List-Title------------>>>" + strTypeName);
                if (strTypeName.equals(strTV))
                {
                    tvTypeName.setText(strFav);
                    strTypeName = strFav;
                    currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                }
                else if (strTypeName.equals(strRadio))
                {
                    tvTypeName.setText(strTV);
                    strTypeName = strTV;
                    currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                }
                else if (strTypeName.equals(strFav))
                {
                    tvTypeName.setText(strRadio);
                    strTypeName = strRadio;
                    currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                }
                
                channelIndex = 0;
                lvChannelList.setSelection(0);
                lvChannelList.requestFocus(0);
                
                adapter.setTvProgramList(currentList);
                adapter.notifyDataSetChanged();
            }
            else if (!llShowTVList.isShown() && llSoundtrack.isShown())
            {
                if (tvLanguage.hasFocus())
                {
                    String previousLang = soundtrackBean.getPreviousLang();
                    tvLanguage.setText(previousLang);
                }
                else if (tvSoundtrack.hasFocus())
                {
                    String previousSoundtrack = soundtrackBean.getPreviousSoundtrack(tvSoundtrack.getText().toString());
                    tvSoundtrack.setText(previousSoundtrack);
                }
            }
        }
        
    }
    
    private void epgKeyMethod()
    {
        if (!ivRec.isShown())
        {
            isIptv = false;
            Bundle bundle = new Bundle();
            bundle.putInt("LogicalNumber", logicalNumber);
            bundle.putInt("ServiceId", serviceId);
            bundle.putString("ServiceName", strChannelName);
            bundle.putChar("ServiceType", serviceType);
            CommonUtils.skipActivity(this, EpgActivity.class, bundle, Intent.FLAG_ACTIVITY_CLEAR_TOP);
        }
    }
    
    @SuppressWarnings("unchecked")
    private void upKeyMethod()
    {
        LogUtils.printLog(1, 3, TAG, "--->>> up key method!");
        clearDialogs();
            if (!llShowTVList.isShown())
            {
                channelIndex++;
                currentPosition++;
                
                if (currentPosition >= listViewize)
                {
                    currentPosition = 0;
                }
                
                channelChangebyUpDownKeys();
            }
            else
            {
                // -The name of ok-list title is TV programs.
                if (strTypeName.equals(strTV))
                {
                    --tvFlag;
                    if (pdTv.getCurrentPage() == 1 && tvFlag < 1)
                    {
                        pdTv.lastPage();
                        
                        currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentTvPage = pdTv.getPageNum();
                        
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        tvFlag = 1;
                    }
                    if (tvFlag < 1)
                    {
                        pdTv.prePage();
                        
                        currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentTvPage--;
                        
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        tvFlag = 1;
                    }
                }
                // -The name of ok-list title is radio programs.
                else if (strTypeName.equals(strRadio))
                {
                    --radioFlag;
                    if (pdRadio.getCurrentPage() == 1 && radioFlag < 1)
                    {
                        pdRadio.lastPage();
                        
                        currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentRadioPage = pdRadio.getPageNum();
                        
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        radioFlag = 1;
                    }
                    if (radioFlag < 1)
                    {
                        pdRadio.prePage();
                        
                        currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentRadioPage--;
                        
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        radioFlag = 1;
                    }
                }
                // -The name of ok-list title is fav programs.
                else if (strTypeName.equals(strFav))
                {
                    --favFlag;
                    if (pdFav.getCurrentPage() == 1 && favFlag < 1)
                    {
                        pdFav.lastPage();
                        
                        currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentFavPage = pdFav.getPageNum();
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        favFlag = 1;
                    }
                    if (favFlag < 1)
                    {
                        pdFav.prePage();
                        
                        currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                        adapter.setTvProgramList(currentList);
                        adapter.notifyDataSetChanged();
                        currentFavPage--;
                        
                        if (null != currentList && currentList.size() > 0)
                        {
                            lvChannelList.setSelection(currentList.size() - 1);
                            lvChannelList.requestFocus(currentList.size() - 1);
                        }
                        favFlag = 1;
                    }
                }
            }
    }
    
    private void volumeUpMethod()
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
        
        dvbPlayer.DVBPlayerAVCtrlSetVolume(volume);
        dvbPlayer.DVBPlayerAVCtrlAudMute(0);
        g_bAudioIsMute = false;
    }
    
    private void volumeDownMethod()
    {
        int volume = 0;
        
        volume = audioManager.getStreamVolume(audioManager.STREAM_SYSTEM);
        System.out.println("GetStreamVolume =" + volume);
        
        if (0 != volume)
        {
            volume -= 1;
        }
        
        volume *= 7;
        dvbPlayer.DVBPlayerAVCtrlSetVolume(volume);
        dvbPlayer.DVBPlayerAVCtrlAudMute(0);
        g_bAudioIsMute = false;
    }
    
    @SuppressWarnings("unchecked")
    private void downKeyMethod()
    {
        LogUtils.printLog(1, 3, TAG, "--->>>down key method.");
        clearDialogs();
        if (!llShowTVList.isShown())
        {
            channelIndex--;
            currentPosition--;
            
            if (currentPosition < 0)
            {
                currentPosition = listViewize - 1;
            }
            channelChangebyUpDownKeys();
        }
        else
        {
            // Dealing with the program classification.The name of the ok-list title is TV programs.
            if (strTypeName.equals(strTV))
            {
                ++tvFlag;
                if (pdTv.getCurrentPage() == pdTv.getPageNum() && tvFlag > 1)
                {
                    pdTv.headPage();
                    currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentTvPage = 1;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    tvFlag = 1;
                }
                if (tvFlag > 1)
                {
                    pdTv.nextPage();
                    currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentTvPage++;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    tvFlag = 1;
                }
            }
            // -The name of the ok-list title is radio programs.
            else if (strTypeName.equals(strRadio))
            {
                ++radioFlag;
                if (pdRadio.getCurrentPage() == pdRadio.getPageNum() && radioFlag > 1)
                {
                    pdRadio.headPage();
                    currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentRadioPage = 1;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    radioFlag = 1;
                }
                if (radioFlag > 1)
                {
                    pdRadio.nextPage();
                    currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentRadioPage++;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    radioFlag = 1;
                }
            }
            // -The name of the ok-list title is Fav programs.
            else if (strTypeName.equals(strFav))
            {
                ++favFlag;
                if (pdFav.getCurrentPage() == pdFav.getPageNum() && favFlag > 1)
                {
                    pdFav.headPage();
                    currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentFavPage = 1;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    favFlag = 1;
                }
                if (favFlag > 1)
                {
                    pdFav.nextPage();
                    currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                    adapter.setTvProgramList(currentList);
                    adapter.notifyDataSetChanged();
                    currentFavPage++;
                    
                    lvChannelList.setSelection(0);
                    lvChannelList.requestFocus(0);
                    favFlag = 1;
                }
            }
        }
    }
    
    private void lookBackProgramme()
    {
        clearDialogs();
        if (llShowTVList.isShown())
        {
            handler.removeCallbacks(hideChannelListTask);
            handler.post(hideChannelListTask);
        }
        else
        {
            if (!ivRec.isShown())
            {
                ServiceInfoBean lastPlayBean = paserLastProgram(Config.DVB_LOOKBACK_SERVICE);
                String lastName = lastPlayBean.getChannelName();
                if (null != lastName)
                {
                    Integer lastServiceId = lastPlayBean.getServiceId();
                    Integer lastLogicalNumber = lastPlayBean.getLogicalNumber();
                    
                    switchProgram(lastServiceId, lastLogicalNumber, false);
                    
                    // set focus in the ok-list.
                    if (serviceType == Config.SERVICE_TYPE_RADIO)
                    {
                        setFocusInRadios();
                    }
                    else
                    {
                        setFocusInTvs();
                    }
                }
            }
        }
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
            if (serviceType == Config.SERVICE_TYPE_RADIO)
            {
                lastServiceId = spChannel.getInt("radio_serviceId", 0);
                lastLogicalNumber = spChannel.getInt("radio_logicalNumber", 0);
                lastServicePosition = spChannel.getInt("radio_servicePosition", 0);
                lastTpId = spChannel.getInt("radio_tpId", 0);
                lastChannelName = spChannel.getString("radio_serviceName", null);
            }
            else
            {
                lastServiceId = spChannel.getInt("tv_serviceId", 0);
                lastLogicalNumber = spChannel.getInt("tv_logicalNumber", 0);
                lastServicePosition = spChannel.getInt("tv_servicePosition", 0);
                lastTpId = spChannel.getInt("tv_tpId", 0);
                lastChannelName = spChannel.getString("tv_serviceName", null);
            }
            
            sBean.setServiceId(lastServiceId);
            sBean.setLogicalNumber(lastLogicalNumber);
            sBean.setChannelPosition(lastServicePosition);
            sBean.setTpId(lastTpId);
            sBean.setChannelName(lastChannelName);
        }
        return sBean;
    }
    
    private void channelChangebyUpDownKeys()
    {
        int serId = 0;
        int logNum = 0;
        // Dealing with the program classification.The name of the ok-list title is TV programs.
        strTypeName = tvTypeName.getText().toString();
        if (strTypeName.equals(strTV))
        {
            if (channelIndex < 0)
            {
                channelIndex = tvList.size() - 1;
            }
            if (channelIndex >= tvList.size())
            {
                channelIndex = 0;
            }
            if (tvList.size() != 0)
            {
                serId = tvList.get(channelIndex).getServiceId();
                servicePosition = tvList.get(channelIndex).getChannelPosition();
                logNum = tvList.get(channelIndex).getLogicalNumber();
            }
        }
        // Radio programs.
        else if (strTypeName.equals(strRadio))
        {
            if (channelIndex < 0)
            {
                channelIndex = radioList.size() - 1;
            }
            if (channelIndex >= radioList.size())
            {
                channelIndex = 0;
            }
            if (radioList.size() != 0)
            {
                serId = radioList.get(channelIndex).getServiceId();
                servicePosition = radioList.get(channelIndex).getChannelPosition();
                logNum = radioList.get(channelIndex).getLogicalNumber();
            }
        }
        // Fav programs.
        else if (strTypeName.equals(strFav))
        {
            if (channelIndex < 0)
            {
                channelIndex = favList.size() - 1;
            }
            if (channelIndex >= favList.size())
            {
                channelIndex = 0;
            }
            if (favList.size() != 0)
            {
                serId = favList.get(channelIndex).getServiceId();
                servicePosition = favList.get(channelIndex).getChannelPosition();
                logNum = favList.get(channelIndex).getLogicalNumber();
            }
        }
        
        switchProgram(serId, logNum, false);
    }
    
    private int switchProgram(int serId, int logNum, boolean firstPlay)
    {
        playParams = getPlayerParams(serId, logNum);
        
        // first enter in this function,direct play last playing programme.
        if (firstPlay)
        {
            //record last play service's tp info.
            lastFreq = currentFreq;
            lastSymb = currentSymb;
            lastEmod = currentEmod;
            
            handler.removeCallbacks(playChannelTask);
            handler.post(playChannelTask);
            return 1;
        }
        
        //judge rec status.
        if(isRec)
        {
            //judge tms status.
            if(isTs)
            {
                showStopRecDialog(getString(R.string.end_tms));
                return 0;
            }
            
            LogUtils.printLog(1, 3, TAG, "--->>>currentFreq,currentSymb,currentEmod"+currentFreq+" "+currentSymb+" "+currentEmod);
            LogUtils.printLog(1, 3, TAG, "--->>>lastFreq,lastSymb,lastEmod"+lastFreq+" "+lastSymb+" "+lastEmod);
            //compare current play service's tp info and last play service's tp info
            if(currentFreq == lastFreq && currentSymb == lastSymb && currentEmod == lastEmod)
            {
                LogUtils.printLog(1, 3, TAG, "--->>>Rec: the same tp info,it can switch service.");
            }
            else
            {
                LogUtils.printLog(1, 3, TAG, "--->>>Rec: the different tp info,it can not switch service.");
                showStopRecDialog(getString(R.string.end_pvr1)+recFileName +"\n"+getString(R.string.end_pvr2));
                return 0;
            }
        }
        
        // record the program to file.
        ServiceInfoBean program = paserLastProgram(Config.DVB_LASTPLAY_SERVICE);
        if (null != program)
        {
            Integer pSerId = program.getServiceId();
            Integer pLogNumber = program.getLogicalNumber();
            
            LogUtils.printLog(1, 3, TAG, "--->>>dvb_last play service id<<<---" + pSerId);
            LogUtils.printLog(1, 3, TAG, "--->>>dvb_last play logical number <<<---" + pLogNumber);
            LogUtils.printLog(1, 3, TAG, "--->>>current service id<<<---" + serviceId);
            LogUtils.printLog(1, 3, TAG, "--->>>current logical number<<<---" + logicalNumber);
            
            // forbid current program switch.
            if (pSerId == serviceId && pLogNumber == logicalNumber)
            {
                LogUtils.printLog(1, 3, TAG, "--->>>The same service id & logical number!!<<<---");
                if (serviceType == tmpServiceType)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>>The same service type, cann't switch!!<<<---");
                }
                else
                {
                    handler.removeCallbacks(playChannelTask);
                    handler.post(playChannelTask);
                    return 1;
                }
            }
            else
            {
                //record last play service's tp info.
                lastFreq = currentFreq;
                lastSymb = currentSymb;
                lastEmod = currentEmod;
                
                // write to dvb_last_play_service.
                recordLastProgram(program);
                
                handler.removeCallbacks(playChannelTask);
                handler.post(playChannelTask);
                return 1;
            }
        }
        return 0;
    }
    
    private void showOKList()
    {
        clearDialogs();
        
        if (llBanner.isShown())
        {
            handler.post(hideChannelNameTask);
        }
        
        if (!llShowTVList.isShown() && !ivRec.isShown())
        {
            // number key switch.
            if (tvChannelNumber.isShown() && numberInput > 0)
            {
                handler.removeCallbacks(numberPlayChannelTask);
                handler.post(numberPlayChannelTask);
            }
            else
            {
                // set left and right key function.
                LogUtils.e(TAG, "TVChannelPlay.showOKList:: setLeftRightFunction to left&right");
                
                handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                
                llShowTVList.setVisibility(View.VISIBLE);
                if (strTypeName.equals(strTV))
                {
                    currentTvPage = channelIndex / listViewize + 1;
                    playPosition = channelIndex % listViewize;
                    pdTv.setCurrentPage(currentTvPage);
                    currentList = (List<ServiceInfoBean>)pdTv.getCurrentList();
                }
                else if (strTypeName.equals(strRadio))
                {
                    currentRadioPage = channelIndex / listViewize + 1;
                    playPosition = channelIndex % listViewize;
                    pdRadio.setCurrentPage(currentRadioPage);
                    currentList = (List<ServiceInfoBean>)pdRadio.getCurrentList();
                }
                else if (strTypeName.equals(strFav))
                {
                    currentFavPage = channelIndex / listViewize + 1;
                    playPosition = channelIndex % listViewize;
                    pdFav.setCurrentPage(currentFavPage);
                    currentList = (List<ServiceInfoBean>)pdFav.getCurrentList();
                }
                
                adapter.setTvProgramList(currentList);
                adapter.notifyDataSetChanged();
                
                lvChannelList.setSelection(playPosition);
                lvChannelList.requestFocus(playPosition);
                
                handler.removeCallbacks(hideChannelListTask);
                handler.postDelayed(hideChannelListTask, 1000 * 8);
            }
        }
    }
    
    
    /**
     * 
     * <Get epg contents.>
     * 
     * @see [类、类#方法、类#成员]
     */
    private void getEpgInfo()
    {
        // clear event rate
        tvFirstEventRate.setText(null);
        tvFirstEventRate.setBackgroundDrawable(null);
        tvSecondEventRate.setText(null);
        tvSecondEventRate.setBackgroundDrawable(null);
        
        EpgEventInfo firstEvent = nativeEpg.new EpgEventInfo();
        EpgEventInfo secondEvent = nativeEpg.new EpgEventInfo();
        
        int getPevent = nativeEpg.GetPevent((short)serviceId, (short)transportStreamId, (short)networkId, firstEvent);
        int getFevent = nativeEpg.GetFevent((short)serviceId, (short)transportStreamId, (short)networkId, secondEvent);
        
        // no epg
        if (getPevent != 0 || getFevent != 0)
        {
            LogUtils.printLog(1, 3, TAG, "--->>> no epg shown!");
            firstEvent = null;
            secondEvent = null;
            tvFirstEventTimeBucket.setText(null);
            tvFirstEventName.setText(null);
            tvSecondEventTimeBucket.setText(null);
            tvSecondEventName.setText(null);
            pbPlayProgress.setProgress(0);
            return;
        }
        
        // set the information in the banner.
        tvFirstEventTimeBucket.setText(firstEvent.getStartTime());
        String firstEventName = firstEvent.geteventname();
        tvFirstEventName.setFocusable(true);
        tvFirstEventName.setFocusableInTouchMode(true);
        tvFirstEventName.setText(firstEventName);
        
        HashMap<Integer, Integer> firstRateMap = firstEvent.getRate();
        if (null != firstRateMap && null != firstEventName)
        {
            Iterator<Integer> fristIterator = firstRateMap.keySet().iterator();
            int firstPicId;
            int firstTextId;
            if (fristIterator.hasNext())
            {
                firstPicId = fristIterator.next();
                firstTextId = firstRateMap.get(firstPicId);
                tvFirstEventRate.setBackgroundResource(firstPicId);
                tvFirstEventRate.setText(firstTextId);
            }
        }
        
        tvSecondEventTimeBucket.setText(secondEvent.getStartTime());
        String secondEventName = secondEvent.geteventname();
        tvSecondEventName.setFocusable(true);
        tvSecondEventName.setFocusableInTouchMode(true);
        tvSecondEventName.setText(secondEventName);
        
        HashMap<Integer, Integer> secondRateMap = secondEvent.getRate();
        if (null != secondRateMap && null != secondEventName)
        {
            Iterator<Integer> secondIterator = secondRateMap.keySet().iterator();
            int secondPicId;
            int secondTextId;
            if (secondIterator.hasNext())
            {
                secondPicId = secondIterator.next();
                secondTextId = secondRateMap.get(secondPicId);
                tvSecondEventRate.setBackgroundResource(secondPicId);
                tvSecondEventRate.setText(secondTextId);
            }
        }
        
        // get utc time.
        EpgTime utctime = nativeEpg.new EpgTime();
        nativeEpg.GetUtcTime(utctime);
        String currentTime = utctime.getutchour() + ":" + utctime.getutcminute() + ":" + utctime.getutcsecond();
        
        pbPlayProgress.setProgress(firstEvent.getPlayProgress(currentTime));
        pbPlayProgress.invalidate();
        
    }
    
    private void recordRecProgram()
    {
        Editor edit = getSharedPreferences("dvb_rec", 8).edit();
        edit.putInt("rec_serviceId", serviceId);
        edit.putInt("rec_logicalNumber", logicalNumber);
        edit.putInt("rec_servicePosition", servicePosition);
        edit.putString("rec_serviceName", strChannelName);
        edit.commit();
    }
    
    private void recordLastProgram(ServiceInfoBean sBean)
    {
        Editor edit = getSharedPreferences(Config.DVB_LOOKBACK_SERVICE, 8).edit();
        if (null != sBean)
        {
            Integer sId = sBean.getServiceId();
            Integer sNumber = sBean.getLogicalNumber();
            if (serviceType == Config.SERVICE_TYPE_RADIO)
            {
                // record last played radio program.
                edit.putInt("radio_serviceId", sId);
                edit.putInt("radio_logicalNumber", sNumber);
                edit.putInt("radio_servicePosition", sBean.getChannelPosition());
                edit.putInt("radio_tpId", sBean.getTpId());
                edit.putString("radio_serviceName", sBean.getChannelName());
            }
            else
            {
                // record last play tv program.
                edit.putInt("tv_serviceId", sId);
                edit.putInt("tv_logicalNumber", sNumber);
                edit.putInt("tv_servicePosition", sBean.getChannelPosition());
                edit.putInt("tv_tpId", sBean.getTpId());
                edit.putString("tv_serviceName", sBean.getChannelName());
            }
            edit.commit();
        }
    }
    
    /**
     * 
     * <Record the last playing program.>
     * 
     * @see [类、类#方法、类#成员]
     */
    private void recordProgram()
    {
        android.provider.Settings.System.putInt(getContentResolver(), "eit_grade", originalGrade);
        Editor edit =
            getSharedPreferences(Config.DVB_LASTPLAY_SERVICE,
                Context.MODE_WORLD_READABLE + Context.MODE_WORLD_WRITEABLE).edit();
        if (serviceType == Config.SERVICE_TYPE_RADIO)
        {
            // record currrent playing radio program and pvr status.
            android.provider.Settings.System.putString(getContentResolver(), "last_play_entrance", "Radio");
            edit.putInt("radio_serviceId", serviceId);
            edit.putInt("radio_logicalNumber", logicalNumber);
            edit.putInt("radio_servicePosition", servicePosition);
            edit.putInt("radio_tpId", tpId);
            edit.putString("radio_serviceName", strChannelName);
            edit.putBoolean("rec_status", isRec);
        }
        else
        {
            // record current playing tv program and pvr status.
            android.provider.Settings.System.putString(getContentResolver(), "last_play_entrance", "TV");
            edit.putInt("tv_serviceId", serviceId);
            edit.putInt("tv_logicalNumber", logicalNumber);
            edit.putInt("tv_servicePosition", servicePosition);
            edit.putInt("tv_tpId", tpId);
            edit.putString("tv_serviceName", strChannelName);
            edit.putBoolean("rec_status", isRec);
        }
        LogUtils.printLog(1, 3, TAG, "--->>> record the program to xml");
        edit.commit();
    }
    
    /**
     * <set the focus in the radio list.>
     * 
     * @see [类、类#方法、类#成员]
     */
    private void setFocusInRadios()
    {
        tvTypeName.setText(strRadio);
        strTypeName = strRadio;
        
        // find the position in the radio list.
        if (radioList.size() != 0)
        {
            for (int i = 0; i < radioList.size(); i++)
            {
                if (serviceId == radioList.get(i).getServiceId() && tpId == radioList.get(i).getTpId())
                {
                    channelIndex = i;
                    currentRadioPage = channelIndex / listViewize + 1;
                    playPosition = channelIndex % listViewize;
                    lvChannelList.setSelection(playPosition);
                    ivCurrentTypeTag.setImageDrawable(radioTag);
                }
            }
        }
    }
    
    /**
     * <set the focus in the tv list.>
     * 
     * @see [类、类#方法、类#成员]
     */
    private void setFocusInTvs()
    {
        tvTypeName.setText(strTV);
        strTypeName = strTV;
        
        LogUtils.printLog(1, 3, TAG, "tv list size--->>>" + tvList.size() + "");
        // find the position in the tv list.
        if (tvList.size() != 0)
        {
            for (int i = 0; i < tvList.size(); i++)
            {
                if (serviceId == tvList.get(i).getServiceId() && tpId == tvList.get(i).getTpId())
                {
                    channelIndex = i;
                    currentTvPage = channelIndex / listViewize + 1;
                    playPosition = channelIndex % listViewize;
                    lvChannelList.setSelection(playPosition);
                    ivCurrentTypeTag.setImageDrawable(tvTag);
                }
            }
        }
    }
    
    public void clearIppvBuffer()
    {
        iStar.delete(0, iStar.length());
        caPwdInput.delete(0, caPwdInput.length());
        tvCaPwd.setText(null);
    }
    
    public void clearDialogs()
    {
        if (null != nitDialog)
        {
            nitDialog.dismiss();
        }
        if (llNotice.isShown())
        {
            llNotice.setVisibility(View.INVISIBLE);
        }
        if (llInputPwd.isShown())
        {
            llInputPwd.setVisibility(View.INVISIBLE);
        }
        if (llSoundtrack.isShown())
        {
            llSoundtrack.setVisibility(View.INVISIBLE);
        }
    }
    
    /**
     * 
     * <send request to ppc.>
     * 
     * @param path
     * @see [类、类#方法、类#成员]
     */
    public void setPPCRequest(String path)
    {
        try
        {
            URL url = new URL(path);
            HttpURLConnection conn = (HttpURLConnection)url.openConnection();
            conn.setRequestMethod("GET");
            conn.setReadTimeout(1000);
            if (conn.getResponseCode() == 200)
            {
                LogUtils.printLog(1, 3, TAG, "------PPC is aliving!! ---------");
            }
        }
        catch (MalformedURLException e)
        {
            e.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    private void processDialog(String message, final Bundle bundle)
    {
        LogUtils.e(TAG, "TVChannelPlay.processDialog:: setLeftRightFunction to left&right");
        clearDialogs();
        
        // setLeftRightFunction(1);
        handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        
        caDialog = DialogUtils.twoButtonsDialogCreate(this, message, new OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (null != caDialog)
                {
                    handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                    caDialog.dismiss();
                }
                
                int nidOrFreq = bundle.getInt("ca_netid_or_freq", 0);
                int tsidOrMod = bundle.getInt("ca_tsid_or_mod", 0);
                int sidOrSymb = bundle.getInt("ca_serviceid_or_symb", 0);
                int type = bundle.getInt("ca_force_type", 0);
                if (type == 0)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>>User Force swith by channel!!");
                    caForceSwitchByChannel(nidOrFreq, tsidOrMod, sidOrSymb);
                }
                else if (type == 1)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>>User Force swith by tp!!");
                    int caServiceId = bundle.getInt("ca_force_esid", 0);
                    caForceSwitchByTpId(nidOrFreq, sidOrSymb, tsidOrMod, caServiceId);
                }
            }
        }, new OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                if (null != caDialog)
                {
                    handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                    caDialog.dismiss();
                    // setLeftRightFunction(0);
                }
            }
        });
    }
    
    /**
     * 
     * <Functional overview>
     * <Functional Details>
     * @param status 0 set success and start playing service; 1 set or play failed; 2 PIN code incorrect; 3 set level incorrect.  
     * @see [Class, Class#Method, Class#Member]
     */
    private void processChildLock(int status)
    {
        switch (status)
        {
            case 0:
                caAdultLock = false;
                handler.removeCallbacks(playChannelTask);
                handler.post(playChannelTask);
                break;
            case 1:
            case 2:
            case 3:
                showCaLevelDialog();
                break;
        }
    }
    
    private void caForceSwith(Bundle bundle)
    {
        if (null != bundle)
        {
            int progNumber = bundle.getInt("ca_logical_number", 0);
            int status = bundle.getInt("ca_force_status", 0);
            
            if (status == 1)
            {
                // clear ca froce message.
                if (null != caDialog)
                {
                    handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                    caDialog.dismiss();
                }
            }
            
            if (progNumber == serviceId)
            {
                // tvFs.setVisibility(View.VISIBLE);
                // tvFs.requestFocus();
                // tvFs.setFocusable(true);
                // tvFs.setFocusableInTouchMode(true);
                
                int type = bundle.getInt("ca_force_type", 0);
                
                String caMessage = bundle.getString("ca_message");
                int nidOrFreq = bundle.getInt("ca_netid_or_freq", 0);
                int tsidOrMod = bundle.getInt("ca_tsid_or_mod", 0);
                int sidOrSymb = bundle.getInt("ca_serviceid_or_symb", 0);
                
                if (status == 0 && type == 0)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>>Ca Force swith by channel!!");
                    caForceSwitchByChannel(nidOrFreq, tsidOrMod, sidOrSymb);
                }
                else if (status == 0 && type == 1)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>>Ca Force swith by tp!!");
                    int caServiceId = bundle.getInt("ca_force_esid", 0);
                    caForceSwitchByTpId(nidOrFreq, sidOrSymb, tsidOrMod, caServiceId);
                }
                else if (status == 2)
                {
                    if (null != caDialog)
                    {
                        handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                        caDialog.dismiss();
                    }
                    processDialog(caMessage, bundle);
                }
            }
            else
            {
                LogUtils.printLog(1, 3, TAG, "--->>>logical number not equals,invalid switch!!");
            }
        }
        else
        {
            LogUtils.printLog(1, 3, TAG, "--->>>Ca force bundle is null !!");
        }
    }
    
    private void caForceSwitchByChannel(int netId, int tsId, int serviceId)
    {
        int fsTpId = tpInfoDao.getTpIdByNetId(netId, tsId);
        LogUtils.printLog(1, 3, TAG, "-->>netId--tsId--tpId-->>"+netId+"  "+tsId +"  "+ fsTpId);
        LogUtils.printLog(1, 3, TAG, "-->>serviceId--fsTpId-->>"+serviceId +"  "+ fsTpId);
        ServiceInfoBean channelInfo = serviceInfoDao.getChannelInfo(fsTpId, serviceId);
        if (null != channelInfo)
        {
            clearDialogs();
            switchProgram(serviceId, channelInfo.getLogicalNumber(), false);
        }
        else
        {
            LogUtils.printLog(1, 5, TAG, "----The target program does not exist in the databases!------");
        }
    }
    
    private void caForceSwitchByTpId(int freq, int symbrate, int mod, int serviceId)
    {
        int fsTpId = tpInfoDao.getTpId(freq, symbrate, mod);
        // showToast("-->>serviceId--fsTpId-->>"+serviceId +"  "+ fsTpId, 0);
        ServiceInfoBean channelInfo = serviceInfoDao.getChannelInfo(fsTpId, serviceId);
        if (null != channelInfo)
        {
            clearDialogs();
            switchProgram(serviceId, channelInfo.getLogicalNumber(), false);
        }
        else
        {
            LogUtils.printLog(1, 5, TAG, "----The target program does not exist in the databases!------");
        }
    }
    
    private void finishTms()
    {
        // dismiss the dialog.
        if (null != pvrDialog)
        {
            pvrDialog.dismiss();
        }
        // stop get tms progress.
        if (null != pvrService && !pvrService.isShutdown())
        {
            pvrService.shutdown();
        }
        // recorver the pvr tag;
        tvPvrOper.setText(null);
        ivPvrOper.setImageDrawable(null);
        // stop tms function.
        if (isTs)
        {
            isTs = false;
            recPlayTag = 0;
            dvbPlayer.DVBPlayerPvrPlayStop();
        }
        // stop rec function.
        if (isRec)
        {
            isRec = false;
            dvbPlayer.DVBPlayerPvrRecStop();
            
            // call backgroud service to stop rec function.
            Intent intent = new Intent();
            intent.setAction("com.pbi.dvb.service.pvrBackgroudService");
            bindService(intent, pvrConn, BIND_AUTO_CREATE);
            LogUtils.printLog(1, 3, TAG, "----Call service method---to Stop Rec!!!");
            
            // send broadcast to notify android media to update the lib.
            if (null != recFilePath)
            {
                LogUtils.printLog(1, 3, TAG, "--->>>send broadcast to notify android media lib.");
                sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse("file://" + recFilePath)));
                // ota update.
                int count = 0;
                while (ota_Status_Service_Conn.getOtaServiceBinder() == null)
                {
                    try
                    {
                        count++;
                        Thread.sleep(500);
                        if (count >= 4)
                        {
                            break;
                        }
                    }
                    catch (InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                }
                if (ota_Status_Service_Conn.getOtaServiceBinder() != null)
                {
                    try
                    {
                        if (ota_Status_Service_Conn.getOtaServiceBinder()
                            .unlock_OTA(OTADownloaderService.OTA_RECORD_LOCK_MASK))
                        {
                            LogUtils.e(TAG, "RECORD::unlock the ota success....");
                        }
                        else
                        {
                            LogUtils.e(TAG, "RECORD::unlock the ota success....");
                        }
                    }
                    catch (RemoteException e)
                    {
                        e.printStackTrace();
                    }
                }
            }
        }
        // unbind the service.
        unbindService(pvrConn);
        ivRec.setVisibility(View.GONE);
        handler.removeCallbacks(hideRecTask);
        handler.post(hideRecTask);
    }
    
    class HideChannelListTask implements Runnable
    {
        public void run()
        {
            LogUtils.e(TAG, "TVChannelPlay.HideChannelListTask.run:: setLeftRightFunction to volume");
            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
            llShowTVList.setVisibility(View.INVISIBLE);
        }
    }
    
    /**
     * 
     * <Hide the password dialog.>
     * 
     * @author gtsong
     * @version [版本�? 2012-8-24]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class HideInputPwdDialog implements Runnable
    {
        public void run()
        {
            tvPwd.setText(null);
            llInputPwd.setVisibility(View.INVISIBLE);
        }
    }
    
    /**
     * 
     * <Show the password dialog.> <When the dialog is visable,the programme stopped.>
     * 
     * @author gtsong
     * @version [版本�? 2012-8-28]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class ShowPwdDialogTask implements Runnable
    {
        public void run()
        {
            dvbPlayer.DVBPlayerStop();
            if (tvDialogMessage.isShown())
            {
                tvDialogMessage.setVisibility(View.INVISIBLE);
            }
            handler.post(hideChannelNameTask);
            handler.post(hideChannelListTask);
            handler.removeCallbacks(playChannelTask);
            
            // record current service type.
            tmpServiceType = serviceType;
            recordProgram();
            
            if (!llInputPwd.isShown())
            {
                llInputPwd.bringToFront();
                llInputPwd.setVisibility(View.VISIBLE);
            }
            
            tvPwd.findFocus();
            tvPwd.setFocusable(true);
            tvPwd.setFocusableInTouchMode(true);
            
        }
    }
    
    /**
     * <Hide the banner.>
     * 
     * @author gtsong
     * @version [版本�? 2012-6-6]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class HideChannelName implements Runnable
    {
        public void run()
        {
            llBanner.setVisibility(View.INVISIBLE);
        }
    }
    
    /**
     * <Show the banner.>
     * 
     * @author gtsong
     * @version [版本�? 2012-6-6]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class ShowChannelNameTask implements Runnable
    {
        public void run()
        {
            // Set program's logo in the banner.
            if (serviceType == Config.SERVICE_TYPE_RADIO)
            {
                ivCurrentTypeTag.setImageDrawable(radioTag);
            }
            else
            {
                ivCurrentTypeTag.setImageDrawable(tvTag);
            }
            
            ServiceInfoBean bean = serviceInfoDao.getChannelInfoByServiceId(serviceId, logicalNumber);
            if (null != bean)
            {
                switch (bean.getAudioMode())
                {
                    case 1:
                        ivSoundtrackTag.setImageDrawable(leftTag);
                        break;
                    case 2:
                        ivSoundtrackTag.setImageDrawable(rightTag);
                        break;
                    case 3:
                        ivSoundtrackTag.setImageDrawable(mixTag);
                        break;
                    default:
                        ivSoundtrackTag.setImageDrawable(stereoTag);
                        break;
                }
            }
            
            tvChannelName.setText(logicalNumber + "." + strChannelName);
            
            tvChannelName2.setText(strChannelName);
            tvChannelNumber.setText(String.valueOf(logicalNumber));
            tvChannelName2.setVisibility(View.VISIBLE);
            tvChannelNumber.setVisibility(View.VISIBLE);
            
            tvFirstEventName.setFocusable(true);
            tvFirstEventName.setFocusableInTouchMode(true);
            tvFirstEventName.requestFocus();
            
             llBanner.invalidate();
            llBanner.setVisibility(View.VISIBLE);
        }
    }
    
    /**
     * <Play the program.>
     * 
     * @author gtsong
     * @version [版本�? 2012-6-6]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class PlayProgramTask implements Runnable
    {
        public void run()
        {
            LogUtils.e(TAG, "--->>>TVChannelPlay.PlayProgramTask run...");
            
            /*
             * locate the current service position on the ok-list.
             */
            if (strTypeName.equals(strTV))
            {
                currentTvPage = channelIndex / listViewize + 1;
            }
            else if (strTypeName.equals(strRadio))
            {
                currentRadioPage = channelIndex / listViewize + 1;
            }
            else if (strTypeName.equals(strFav))
            {
                currentFavPage = channelIndex / listViewize + 1;
            }
            playPosition = channelIndex % listViewize;
            
            // start a thread to send ppc request.
            new Thread(new PPCInfoTask()).start();
            
            /*
             * fill epg content on the banner.
             */
            getEpgInfo();
            getEitGrade();
            getEitCategory();
            if (eitLimitcategorys.contains(String.valueOf(category)))
            {
                eitCategoryLock = true;// set the eit category lock.
                showEitCategoryDialog();
                
                handler.removeCallbacks(showNameTask);
                handler.post(showNameTask);
                return;
            }
            
            if (currentEitGrade > originalGrade)
            {
                eitLevelLock = true; // set the eit level lock.
                showEitLevelDialog();
                
                handler.removeCallbacks(showNameTask);
                handler.post(showNameTask);
                return;
            }
            
            handler.removeCallbacks(showNameTask);
            handler.post(showNameTask);
            dvbPlayer.DVBPlayerPlayProgram(playParams);
            
            recordProgram();
            
            if (llShowTVList.isShown())
            {
                handler.removeCallbacks(hideChannelListTask);
                handler.postDelayed(hideChannelListTask, 1000 * 5);
            }
            
            handler.removeCallbacks(hideChannelNameTask);
            handler.postDelayed(hideChannelNameTask, 1000 * 8);
            
            handler.removeCallbacks(hideChannelNumberTask);
            handler.postDelayed(hideChannelNumberTask, 1000 * 8);
            
            // record current service type.
            tmpServiceType = serviceType;
        }
    }
    
    /**
     * <Hide the input numbers.>
     * 
     * @author gtsong
     * @version [版本�? 2012-6-14]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class HideChannelNumberTask implements Runnable
    {
        public void run()
        {
            tvChannelNumber.setVisibility(View.INVISIBLE);
            tvChannelName2.setVisibility(View.INVISIBLE);
        }
    }
    
    /**
     * 
     * <Change channel through the digital keys.>
     * 
     * @author gtsong
     * @version [版本�? 2012-9-11]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class NumberPlayProgramTask implements Runnable
    {
        public void run()
        {
            LogUtils.printLog(1, 3, TAG, "------------Number Switch !!!-------------");
            List<ServiceInfoBean> nBeans = serviceInfoDao.getChannelIdByLogicalNumber(numberInput);
            ServiceInfoBean nBean = null;
            if (nBeans.size() != 0)
            {
                nBean = nBeans.get(0);
            }
            if (null != nBean)
            {
                int nSerId = nBean.getServiceId();
                int nLogNum = nBean.getLogicalNumber();
                
                // show service name.
                tvChannelName2.setText(nBean.getChannelName());
                
                int switchVal = switchProgram(nSerId, nLogNum, false);
                if (switchVal == 1)
                {
                    char sType = nBean.getServiceType();
                    if (sType == Config.SERVICE_TYPE_RADIO)
                    {
                        setFocusInRadios();
                    }
                    else
                    {
                        setFocusInTvs();
                    }
                }
            }
            else
            {
                showToast(getString(R.string.invilid_programme), Toast.LENGTH_SHORT);
            }
            numberInput = 0;
            handler.post(hideChannelNumberTask);
        }
    }
    
    class ChannelOnKeyListener implements android.view.View.OnKeyListener
    {
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            if (llShowTVList.isShown())
            {
                handler.removeCallbacks(hideChannelListTask);
                handler.postDelayed(hideChannelListTask, 1000 * 5);
            }
            return false;
        }
        
    }
    
    class ViewOnClickListener implements OnClickListener
    {
        @SuppressWarnings("unchecked")
        public void onClick(View v)
        {
            if (!llSoundtrack.isShown())
            {
                showOKList();
            }
        }
    }
    
    class ChannelOnItemSelectedListener implements OnItemSelectedListener
    {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            currentPosition = position;
        }
        
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
    }
    
    class ChannelOnItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            int positionNew;
            
            // if (llShowTVList.isShown())
            // {
            // llShowTVList.setVisibility(View.INVISIBLE);
            // }
            adapter.setIndex(position);
            
            if (strTypeName.equals(strTV))
            {
                positionNew = position + (currentTvPage - 1) * listViewize;
                channelIndex = positionNew;
            }
            else if (strTypeName.equals(strRadio))
            {
                positionNew = position + (currentRadioPage - 1) * listViewize;
                channelIndex = positionNew;
            }
            else if (strTypeName.equals(strFav))
            {
                positionNew = position + (currentFavPage - 1) * listViewize;
                channelIndex = positionNew;
            }
            
            TextView tvServiceId = (TextView)view.findViewById(R.id.tv_play_serviceid);
            TextView tvChannelNumber = (TextView)view.findViewById(R.id.tv_play_program_number);
            
            int iNumber = Integer.parseInt(tvChannelNumber.getText().toString().trim());
            int iserid = Integer.parseInt(tvServiceId.getText().toString().trim());
            
            handler.removeCallbacks(hideChannelListTask);
            handler.post(hideChannelListTask);
            switchProgram(iserid, iNumber, false);
        }
    }
    
    class PPCInfoTask implements Runnable
    {
        public void run()
        {
            try
            {
                String startPPC =
                    "http://127.0.0.1:9906/?opt=channel_start&channel_name=" + CommonUtils.parseAscii(strChannelName)
                        + "&service_id=" + serviceId + "&transport_stream_id=" + transportStreamId + "&network_id="
                        + networkId;
                setPPCRequest(startPPC);
                LogUtils.printLog(1, 3, TAG, "--------->>PPC start !!---------" + startPPC);
            }
            catch (Exception e)
            {
                LogUtils.printLog(1, 3, TAG, "------PPC start exception!!---------");
                e.printStackTrace();
            }
        }
    }
    
    /**
     * 
     * <password dialog onkeyListener>
     * 
     * @author
     * @version [Version Number, 2014-1-10]
     * @see [Relevant Class/Method]
     * @since [Product/Module Version]
     */
    class PasswordOnKeyListener implements OnKeyListener
    {
        @Override
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            StringBuilder pStar = new StringBuilder();
            /*-------------Press the digital key-----------------*/
            if (keyCode >= KeyEvent.KEYCODE_0 && keyCode <= KeyEvent.KEYCODE_9)
            {
                if (event.getAction() == MotionEvent.ACTION_DOWN)
                {
                    passwordInput.append(keyCode - KeyEvent.KEYCODE_0);
                }
                
                if (null != passwordInput)
                {
                    for (int i = 0; i < passwordInput.length(); i++)
                    {
                        pStar.append("*");
                        tvPwd.setText(pStar);
                    }
                }
                
                if (passwordInput.length() == 4)
                {
                    pStar.delete(0, pStar.length());
                    // process ca audlt lock.
                    if(caAdultLock)
                    {
                        NativeCA nativeCA = new NativeCA();
                        int childPwd = Integer.parseInt(passwordInput.toString());
                        nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PARENTAL_CHANGE_QUERY, null, childPwd);
                        
                        pStar.delete(0, pStar.length());
                        passwordInput.delete(0, passwordInput.length());
                        handler.post(hidePassWordDialog);
                        return true;
                    }
                    
                    // process eit and service lock.
                    if (passwordInput.toString().equalsIgnoreCase(superPwd)
                        || passwordInput.toString().equalsIgnoreCase(Config.SUPER_PASSWORD))
                    {
                        // service lock.
                        if (serviceLock)
                        {
                            // the service lock only get once.
                            firGetServiceLock = false;
                            serviceLock = false;
                            pStar.delete(0, pStar.length());
                            passwordInput.delete(0, passwordInput.length());
                            handler.post(hidePassWordDialog);
                            
                            handler.removeCallbacks(playChannelTask);
                            handler.post(playChannelTask);
                            return true;
                        }
                        // eit category lock.
                        if (eitCategoryLock)
                        {
                            eitCategoryLock = false;
                            pStar.delete(0, pStar.length());
                            passwordInput.delete(0, passwordInput.length());
                            handler.post(hidePassWordDialog);
                            
                            // remove current categroy limit.
                            eitLimitcategorys.remove(String.valueOf(category));
                            handler.removeCallbacks(playChannelTask);
                            handler.post(playChannelTask);
                            return true;
                        }
                        
                        // eit level lock.
                        if (eitLevelLock)
                        {
                            eitLevelLock = false;
                            pStar.delete(0, pStar.length());
                            passwordInput.delete(0, passwordInput.length());
                            handler.post(hidePassWordDialog);
                            
                            // raise current eit level.
                            originalGrade = currentEitGrade;
                            handler.removeCallbacks(playChannelTask);
                            handler.post(playChannelTask);
                            
                            // if (originalGrade >= 3)
                            // {
                            // tempGrade = currentEitGrade;
                            // when the eit level greater than 3, five minutes later,recovery previous eit level.
                            // bannerHandler.sendEmptyMessageDelayed(Config.RECOVER_INITIAL_GRADE, 1000 * 60 * 5);
                            // }
                        }
                        
                    }
                    else
                    {
                        tvPwdTitle.setText(getString(R.string.password_wrong_tips));
                        Animation shake = AnimationUtils.loadAnimation(TVChannelPlay.this, R.anim.shake);
                        tvPwd.startAnimation(shake);
                        tvPwd.setText(null);
                        
                        pStar.delete(0, pStar.length());
                        passwordInput.delete(0, passwordInput.length());
                    }
                }
                return true;
            }
            /*-------------Press the del key-----------------*/
            if (keyCode == Config.KEY_FAST_CHANGE)
            {
                passwordInput.delete(0, passwordInput.length());
                pStar.delete(0, pStar.length());
                tvPwd.setText(null);
                tvPwdTitle.setText(getString(R.string.password_stb));
                return true;
            }
            return false;
        }
    }
    
    /**
     * 
     * <The ca's password dialog's key-listener.> <Receive the number from the password input box,and show the password
     * instead of '*'.>
     * 
     * @author gtsong
     * @version [版本�? 2012-10-17]
     * @see [相关�?方法]
     * @since [产品/模块版本]
     */
    class CaPasswordOnKeyListener implements OnKeyListener
    {
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            iStar.delete(0, iStar.length());
            /*-------------Press the digital key-----------------*/
            if (keyCode >= KeyEvent.KEYCODE_0 && keyCode <= KeyEvent.KEYCODE_9)
            {
                if (KeyEvent.ACTION_DOWN == event.getAction())
                {
                    caPwdInput.append(keyCode - KeyEvent.KEYCODE_0);
                }
                if (caPwdInput.length() == 4)
                {
                    pincode = Integer.parseInt(caPwdInput.toString());
                    return true;
                }
                if (null != caPwdInput)
                {
                    for (int i = 0; i < caPwdInput.length(); i++)
                    {
                        iStar.append("*");
                        tvCaPwd.setText(iStar);
                    }
                }
                return true;
            }
            
            if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0)
            {
                clearIppvBuffer();
                LogUtils.e(TAG, "TVChannelPlay.CaPasswordOnKeyListener:: setLeftRightFunction to left&right");
                
                // setLeftRightFunction(1);// left & right
                handler.sendEmptyMessage(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                llNotice.setVisibility(View.INVISIBLE);
                return true;
            }
            /*-------------Press the del key-----------------*/
            if (keyCode == KeyEvent.KEYCODE_DEL && event.getRepeatCount() == 0)
            {
                clearIppvBuffer();
                return true;
            }
            else
            {
                return false;
            }
        }
        
    }
    
    class GetTmsProgress implements Runnable
    {
        private Handler handler;
        
        public GetTmsProgress(Handler handler)
        {
            super();
            this.handler = handler;
        }
        
        public void run()
        {
            // get rec file info.
            PVR_Rec_Status_t recStatus = dvbPlayer.new PVR_Rec_Status_t();
            dvbPlayer.DVBPlayerPvrRecGetStatus(recStatus);
            int endTimeInMs = recStatus.getU32EndTimeInMs();
            
            // get current time.
            PVR_Play_Status_t tPvrStatus = dvbPlayer.new PVR_Play_Status_t();
            dvbPlayer.DVBPlayerPvrPlayGetStatus(tPvrStatus);
            int playTime = tPvrStatus.getU32CurPlayTimeInMs();
            
            LogUtils.printLog(1, 3, TAG, "------TMS End Time ----->>>" + endTimeInMs);
            LogUtils.printLog(1, 3, TAG, "------TMS Play Time ----->>>" + playTime);
            
            if (endTimeInMs != 0)
            {
                // int progress = playTime * 100 / (endTimeInMs - startTimeInMs);
                int progress = playTime * 100 / (endTimeInMs);
                Message msg = new Message();
                msg.what = Config.PVR_TMS_PROGRESS;
                msg.obj = progress;
                handler.sendMessage(msg);
            }
        }
    }
    
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.SWITCH_TO_VOICE_CONTROL:
                    LogUtils.printLog(1, 3, TAG, "--->>> revocery volume function!");
                    CommonUtils.setLeftRightFunction(Config.SWITCH_TO_VOICE_CONTROL);
                    break;
                case Config.SWITCH_TO_LEFT_RIGHT_CONTROL:
                    LogUtils.printLog(1, 3, TAG, "--->>> revocery left and right function!");
                    CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
                    break;
                case Config.RED_KEY_MESSAGE:
                    dismissNobuttonsDialog();
                    if(caAdultLock)
                    {
                        tvPwdTitle.setText(getString(R.string.E13));
                    }
                    else
                    {
                        tvPwdTitle.setText(getString(R.string.password_stb));
                    }
                    
                    handler.removeCallbacks(hideChannelNameTask);
                    handler.post(hideChannelNameTask);
                    
                    handler.removeCallbacks(hideChannelNumberTask);
                    handler.post(hideChannelNumberTask);
                    
                    handler.removeCallbacks(showPwdTask);
                    handler.post(showPwdTask);
                    break;
                    
                case Config.UP_SWITCH_SERVICE:
                    LogUtils.printLog(1, 3, TAG, "--->>> receive message UP_SWITCH_SERVICE!");
                    dismissNobuttonsDialog();
                    upKeyMethod();
                    break;
                    
                case Config.DOWN_SWITCH_SERVICE:
                    LogUtils.printLog(1, 3, TAG, "--->>> receive message DOWN_SWITCH_SERVICE!");
                    dismissNobuttonsDialog();
                    downKeyMethod();
                    break;
                case Config.NUMBER_SWITCH_SERVICE:
                    dismissNobuttonsDialog();
                    numberKeyMethod((Integer)msg.obj);
                    break;
                case Config.HIDE_BANNER_MESSAGE:
                    handler.removeCallbacks(hideChannelNameTask);
                    handler.postDelayed(hideChannelNameTask, 1000 * 3);
                    handler.removeCallbacks(hideChannelNumberTask);
                    handler.postDelayed(hideChannelNumberTask, 1000 * 3);
                    break;
                case Config.SHOW_BANNER_MESSAGE:
                    handler.removeCallbacks(hideChannelNameTask);
                    handler.removeCallbacks(hideChannelNumberTask);
                    handler.removeCallbacks(showNameTask);
                    handler.post(showNameTask);
                    break;
            }
        }
    };
    
    private Handler pvrHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.PVR_TMS_PROGRESS:
                    int tmsProgress = (Integer)msg.obj;
                    sbPvr.setProgress(tmsProgress);
                    sbPvr.invalidate();
                    break;
                case Config.PVR_REC_PROGRESS:
                    int recProgress = (Integer)msg.obj;
                    sbPvr.setProgress(recProgress);
                    sbPvr.invalidate();
                    break;
                case Config.PVR_PLAY_EOF:
                    LogUtils.printLog(1, 3, TAG, "----PVR Message---->>>" + getString(R.string.pvr_play_eof));
                    break;
                case Config.PVR_PLAY_SOF:
                    LogUtils.printLog(1, 3, TAG, "----PVR Message---->>>" + getString(R.string.pvr_play_sof));
                    break;
                case Config.PVR_PLAY_ERROR:
                    showToast(getString(R.string.pvr_inner_error), Toast.LENGTH_SHORT);
                    break;
                case Config.PVR_PLAY_REACH_REC:
                    if (isRec)
                    {
                        finishTms();
                    }
                    break;
                case Config.PVR_REC_DISKFULL:
                    showToast(getString(R.string.pvr_disk_full), Toast.LENGTH_SHORT);
                    finishTms();
                    break;
                case Config.PVR_REC_ERROR:
                    LogUtils.printLog(1, 3, TAG, "----PVR Message---->>>" + getString(R.string.pvr_inner_error));
                    showToast(getString(R.string.pvr_inner_error), 1);
                    break;
                case Config.PVR_REC_OVER_FIX:
                    showToast(getString(R.string.pvr_length), 1);
                    finishTms();
                    break;
                case Config.PVR_REC_REACH_PLAY:
                    LogUtils.printLog(1, 3, TAG, "----PVR Message---->>>PVR_REC_REACH_PLAY");
                    break;
                case Config.PVR_REC_DISK_SLOW:
                    showToast(getString(R.string.pvr_speed), 1);
                    break;
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
            LogUtils.printLog(1, 3, TAG, "--->>> CTI_CA_E"+msg.what);
            switch (msg.what)
            {
                case Config.CTI_CA_E01:
                    showDialogMessage(R.string.E01);
                    break;
                case Config.CTI_CA_E02:
                    showDialogMessage(R.string.E02);
                    break;
                case Config.CTI_CA_E03:
                    showDialogMessage(R.string.E03);
                    break;
                case Config.CTI_CA_E04:
                    showDialogMessage(R.string.E04);
                    break;
                case Config.CTI_CA_E05:
                    showDialogMessage(R.string.E05);
                    break;
                case Config.CTI_CA_E07:
                    showDialogMessage(R.string.E07);
                    break;
                case Config.CTI_CA_E10:
                    showDialogMessage(R.string.E10);
                    break;
                case Config.CTI_CA_E11:
                    showDialogMessage(R.string.E11);
                    break;
                case Config.CTI_CA_E12:
                    showDialogMessage(R.string.E12);
                    break;
                case Config.CTI_CA_E13:
                    caAdultLock = true;
                    showCaLevelDialog();
                    break;
                case Config.CTI_CA_E14:
                    showDialogMessage(R.string.E14);
                    break;
                case Config.CTI_CA_E15:
                    if (!llNotice.isShown())
                    {
                        showDialogMessage(R.string.E15);
                    }
                    break;
                case Config.CTI_CA_E00:
                case Config.CTI_CA_E06:
                case Config.CTI_CA_E08:
                case Config.CTI_CA_E16:
                case Config.CTI_CA_E50:
                case Config.CTI_CA_E56:
                    if (tvDialogMessage.isShown())
                    {
                        tvDialogMessage.setVisibility(View.INVISIBLE);
                    }
                    if (llNotice.isShown() || tvFpRt.isShown())
                    {
                        llNotice.setVisibility(View.INVISIBLE);
                        tvFpRt.setVisibility(View.INVISIBLE);
                    }
                    break;
                case Config.CTI_CA_E17:
                    showDialogMessage(R.string.E17);
                    break;
                case Config.CTI_CA_E18:
                    showDialogMessage(R.string.E18);
                    break;
                case Config.CTI_CA_E19:
                    showDialogMessage(R.string.E19);
                    break;
                case Config.CTI_CA_E20:
                    showDialogMessage(R.string.E20);
                    break;
                case Config.CTI_CA_E21:
                    showDialogMessage(R.string.E21);
                    break;
                case Config.CTI_CA_E22:
                    showDialogMessage(R.string.E22);
                    break;
                case Config.CTI_CA_E23:
                    showDialogMessage(R.string.E23);
                    break;
                case Config.CTI_CA_E24:
                    showDialogMessage(R.string.E24);
                    break;
                case Config.CTI_CA_E25:
                    showDialogMessage(R.string.E25);
                    break;
                case Config.CTI_CA_E26:
                    showDialogMessage(R.string.E26);
                    break;
                case Config.CTI_CA_E27:
                    showDialogMessage(R.string.E27);
                    break;
                case Config.CTI_CA_E28:
                    showDialogMessage(R.string.E28);
                    break;
                case Config.CTI_CA_E29:
                    showDialogMessage(R.string.E29);
                    break;
                case Config.CTI_CA_E30:
                    showDialogMessage(R.string.E30);
                    break;
                case Config.CTI_CA_E40:
                    showDialogMessage(R.string.E40);
                    break;
                case Config.CTI_CA_E41:
                    showDialogMessage(R.string.E41);
                    break;
                case Config.CTI_CA_E42:
                    showDialogMessage(R.string.E42);
                    break;
                case Config.CTI_CA_E43:
                    showDialogMessage(R.string.E43);
                    break;
                case Config.CTI_CA_E44:
                    showDialogMessage(R.string.E44);
                    break;
                case Config.CTI_CA_E45:
                    showDialogMessage(R.string.E45);
                    break;
                case Config.CTI_CA_E46:
                    showDialogMessage(R.string.E46);
                    break;
                case Config.CTI_CA_E47:
                    showDialogMessage(R.string.E47);
                    break;
                case Config.CTI_CA_E51:
                    showDialogMessage(R.string.E51);
                    break;
                case Config.CTI_CA_E52:
                    showDialogMessage(R.string.E52);
                    break;
                case Config.CTI_CA_E53:
                    showDialogMessage(R.string.E53);
                    break;
                case Config.CTI_CA_E54:
                    showDialogMessage(R.string.E54);
                    break;
                case Config.CTI_CA_E55:
                    showDialogMessage(R.string.E55);
                    break;
                case Config.CTI_CA_E57:
                    showDialogMessage(R.string.E57);
                    break;
                case Config.CA_PARENTAL_REPLY:
                    int parentalStatus = (Integer)msg.obj;
                    processChildLock(parentalStatus);
                    break;
                case Config.CA_MAIL:
                    if (ivMail.getVisibility() == View.INVISIBLE)
                    {
                        ivMail.setVisibility(View.VISIBLE);
                    }
                    break;
                case Config.CA_IPP_NOTICE:
                    ippvPurchase(msg);
                    break;
                case Config.CA_FP:
                    Bundle fpData = (Bundle)msg.obj;
                    String strFp = fpData.getString("cti_fp_str");
                    int fpType = fpData.getInt("cti_fp_type", 0);
                    
                    // showToast("--->>>Type: " + fpType + " --->> Fp: "+ strFp, 0);
                    switch (fpType)
                    {
                        case 0:
                            caHandler.post(hideFingerPrintTask);
                            break;
                        case 1:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(150.0f);
                            tvFpRt.setY(50.0f);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 2:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(250.0f);
                            tvFpRt.setY(50.0f);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 3:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(400.0f);
                            tvFpRt.setY(50.0f);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 4:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(700.0f);
                            tvFpRt.setY(50.0f);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 5:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(CommonUtils.getPlayWindow().left + 100);
                            tvFpRt.setY(CommonUtils.getPlayWindow().bottom - 50);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 6:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(CommonUtils.getPlayWindow().left + 200);
                            tvFpRt.setY(CommonUtils.getPlayWindow().bottom - 50);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 7:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(CommonUtils.getPlayWindow().left + 300);
                            tvFpRt.setY(CommonUtils.getPlayWindow().bottom - 50);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 8:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(CommonUtils.getPlayWindow().left + 600);
                            tvFpRt.setY(CommonUtils.getPlayWindow().bottom - 50);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                        case 9:
                            tvFpRt.setText(strFp);
                            tvFpRt.setVisibility(View.VISIBLE);
                            tvFpRt.setX(CommonUtils.getPlayWindow().left + 800);
                            tvFpRt.setY(CommonUtils.getPlayWindow().bottom - 50);
                            flMain.removeView(tvFpRt);
                            flMain.addView(tvFpRt);
                            break;
                    }
                    break;
                case Config.CA_FORCE_SWITCH:
                    Bundle forceData = (Bundle)msg.obj;
                    caForceSwith(forceData);
                    break;
                case Config.CA_OSD_SHOW:
                    String data = (String)msg.obj;
                    if (null != data && !"".equals(data))
                    {
                        showDefaultOSD(data);
                    }
                    break;
                case Config.CA_IPPV_QUERY_REPLY:
                    int ippStatus = (Integer)msg.obj;
                    switch (ippStatus)
                    {
                        case 0:
                            showToast(getString(R.string.cti_ippv_status0), 1);
                            break;
                        case 1:
                            showToast(getString(R.string.cti_ippv_status1), 1);
                            break;
                        case 2:
                            showToast(getString(R.string.cti_ippv_status2), 1);
                            break;
                        case 3:
                            showToast(getString(R.string.cti_ippv_status3), 1);
                            break;
                        case 4:
                            showToast(getString(R.string.cti_ippv_status4), 1);
                            break;
                        case 5:
                            showToast(getString(R.string.cti_ippv_status5), 1);
                            break;
                        case 6:
                            showToast(getString(R.string.cti_ippv_status6), 1);
                            break;
                        case 7:
                            showToast(getString(R.string.cti_ippv_status7), 1);
                            break;
                        case 8:
                            showToast(getString(R.string.cti_ippv_status8), 1);
                            break;
                    }
                    break;
            }
        }
    };
    
    /**
     * process player message.
     */
    private Handler playerHandler = new Handler()
    {
        @SuppressWarnings("unchecked")
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.NIT_UPDATE:
                    if (isRec)
                    {
                        return;
                    }
                    if (null != caDialog)
                    {
                        return;
                    }
                    if (null != nitDialog)
                    {
                        return;
                    }
                    LogUtils.e(TAG, "TVChannelPlay.playerHandler.NIT_UPDATE:: setLeftRightFunction to left&right");
                    curNitCode = (Integer)msg.obj;
                    final TickCountDownUtil mCountDown =
                        new TickCountDownUtil(DialogUtils.handler, 30, DialogUtils.UPDATE_BUTTON);
                    DialogUtils.curNitCode = curNitCode;
                    nitDialog =
                        DialogUtils.twoButtonsDialogWithTimeCreate(TVChannelPlay.this,
                            getString(R.string.NITUpdate),mCountDown,
                            new OnClickListener()
                            {
                                @Override
                                public void onClick(View v)
                                {
                                    if(null != mCountDown)
                                    {
                                        mCountDown.setRunFlag(false);
                                    }
                                    if (null != nitDialog)
                                    {
                                        if(null != mCountDown)
                                        {
                                            mCountDown.setRunFlag(false);
                                        }
                                        nitDialog.dismiss();
                                    }
                                    // update the new version code.
                                    Editor edit = getSharedPreferences("dvb_nit", 8).edit();
                                    edit.putInt("version_code", curNitCode);
                                    edit.commit();
                                    
                                    // package tp infomation.
                                    Bundle bundle = new Bundle();
                                    bundle.putInt("SearchMode", Config.Install_Search_Type_eAUTO_SEARCH);
                                    CommonUtils.skipActivity(TVChannelPlay.this,
                                        ChannelSearchProgress.class,
                                        bundle,
                                        Intent.FLAG_ACTIVITY_CLEAR_TOP );
                                }
                            },
                            new OnClickListener()
                            {
                                @Override
                                public void onClick(View v)
                                {
                                    if (null != nitDialog)
                                    {
                                        if(null != mCountDown)
                                        {
                                            mCountDown.setRunFlag(false);
                                        }
                                        nitDialog.dismiss();
                                        
                                        LogUtils.e(TAG,
                                            "TVChannelPlay.playerHandler.NIT_UPDATE:: nitDialog.dismiss() setLeftRightFunction to volume");
                                        
                                        handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                                    }
                                }
                            });
                    break;
                case Config.PLAYER_PROGRAM_INTERRUPT:
                    showDialogMessage(R.string.NoSignal);
                    break;
                case Config.PLAYER_LOCK_TUNER_SUCCESS:
                    if (tvDialogMessage.getText().toString().equals(getString(R.string.NoSignal)))
                    {
                        tvDialogMessage.setVisibility(View.GONE);
                    }
                    break;
                case Config.PLAYER_PMT_TIME_OUT:
                    showDialogMessage(R.string.ProgramInterrupt);
                    break;
                case Config.AV_FRAME_UNNORMAL:
                    showDialogMessage(R.string.ProgramInterrupt);
                    break;
                case Config.PLAYER_START:
                    String str = tvDialogMessage.getText().toString();
                    if (str.startsWith("E"))
                    {
                        LogUtils.printLog(1, 3, TAG, "--->>>ca message not clear dialog ~~");
                    }
                    else
                    {
                        LogUtils.printLog(1, 3, TAG, "--->>>receive player start message ! clear dialog ~~");
                        tvDialogMessage.setVisibility(View.INVISIBLE);
                    }
                    break;
                case Config.PLAYER_LANGUAGE:
                    ArrayList<MessagePlayer.oneCHaudio> listLang = (ArrayList<MessagePlayer.oneCHaudio>)msg.obj;
                    soundtrackBean.setListLang(listLang);
                    LogUtils.printLog(1,
                        3,
                        TAG,
                        "--->>>receive player lang message, the list size is: " + listLang.size());
                    if (listLang.size() > 1)
                    {
                        ivLangTag.setVisibility(View.VISIBLE);
                    }
                    else
                    {
                        ivLangTag.setVisibility(View.INVISIBLE);
                    }
                    break;
                case Config.UPDATE_CA_MODE:
                    // int cMode = (Integer)msg.obj;
                    //
                    // if (cMode != caMode)
                    // {
                    // LogUtils.printLog(1, 3, TAG, "--->>>current ca mode is" + cMode + "---original is " + caMode);
                    // serviceInfoDao.updateCaMode(serviceId, logicalNumber, cMode);
                    //
                    // // change the content in the ok-list.
                    // tvList = serviceInfoDao.getTVChannelInfo();
                    // radioList = serviceInfoDao.getRadioChannelInfo();
                    // favList = serviceInfoDao.getFavChannelInfo();
                    //
                    // pdTv = new PageDaoImpl(tvList, listViewize);
                    // pdRadio = new PageDaoImpl(radioList, listViewize);
                    // pdFav = new PageDaoImpl(favList, listViewize);
                    // }
                    break;
                case Config.UPDATE_PMT_PID:
                    Bundle data = (Bundle)msg.obj;
                    int tsId = data.getInt("tsId");
                    int serviceId = data.getInt("serviceId");
                    int pmtPid = data.getInt("pmdPid");
                    
                    int updateTpId = tpInfoDao.getTpId(tsId);
                    serviceInfoDao.updatePmtPid(serviceId, updateTpId, pmtPid);
                    break;
            }
        };
    };
    
    private Handler bannerHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.UPDATE_EPG_INFO:
                    LogUtils.printLog(1, 3, TAG, "--->>> current eit level has changed!");
                    Bundle epgData = (Bundle)msg.obj;
                    short mServiceId = epgData.getShort("serviceId");
                    short mTsId = epgData.getShort("tsid");
                    short mOnId = epgData.getShort("onid");
                    
                    if (mServiceId == serviceId && mTsId == transportStreamId && mOnId == networkId)
                    {
                        /*
                         * current eit level has changed,get the epg content and eit grade again.
                         */
                        LogUtils.printLog(1,
                            3,
                            TAG,
                            "--->>> current eit level has changed,get the epg content and eit grade again.");
                        getEpgInfo();
                        getEitGrade();
                        getEitCategory();
                        
                        if (eitLimitcategorys.contains(String.valueOf(category)))
                        {
                            eitCategoryLock = true;// set the eit category lock.
                            showEitCategoryDialog();
                            return;
                        }
                        if (currentEitGrade > originalGrade)
                        {
                            eitLevelLock = true; // set the eit level lock.
                            showEitLevelDialog();
                            return;
                        }
                    }
                    
                    break;
                case Config.RECOVER_INITIAL_GRADE:
                    // LogUtils.printLog(1, 3, TAG, "--->>> time's up, recover the eit level");
                    // originalGrade = tempGrade;
                    
                    /*
                     * the eit level has been recovery. simiulation the message 'UPDATE_EPG_INFO' get the epg content
                     * and eit grade again.
                     */
                    // handler.sendEmptyMessage(Config.UPDATE_EPG_INFO);
                    break;
            }
        }
        
    };
    
    private void showEitLevelDialog()
    {
        clearDialogs();
        
        if (null == eitLevelDialog)
        {
            dvbPlayer.DVBPlayerStop();
            eitLevelDialog =
                DialogUtils.noButtonsDialogCreate(TVChannelPlay.this,handler,
                    getString(R.string.viewing_level_control));
        }
    }
    
    private void showEitCategoryDialog()
    {
        clearDialogs();
        
        // dismiss the nit level dialog.
        if (null != eitLevelDialog)
        {
            eitLevelDialog.dismiss();
            eitLevelDialog = null;
        }
        dvbPlayer.DVBPlayerStop();
        // show the nit category dialog.
        if (null == eitCategoryDialog)
        {
            eitCategoryDialog =
                DialogUtils.noButtonsDialogCreate(TVChannelPlay.this,handler,
                    CommonUtils.getNitCategory(TVChannelPlay.this, category));
        }
    }
    
    private void showCaLevelDialog()
    {
        clearDialogs();
        if (null == caLevelDialog)
        {
            caLevelDialog = DialogUtils.noButtonsDialogCreate(TVChannelPlay.this, handler, getString(R.string.E13));
        }
    }
    
    private void getEitCategory()
    {
        EpgEventInfo firstEvent = nativeEpg.new EpgEventInfo();
        nativeEpg.GetEventInfoByIndex((short)serviceId,
            (short)transportStreamId,
            (short)networkId,
            0,
            1,
            firstEvent,
            null);
        category = firstEvent.getcontentnibble();
        LogUtils.printLog(1, 3, TAG, "--->>> epg category level " + category);
    }
    
    private void getEitGrade()
    {
        // get grade
        peventRate grade = nativeEpg.new peventRate();
        nativeEpg.GetRate((short)serviceId, (short)transportStreamId, (short)networkId, grade);
        currentEitGrade = CommonUtils.getEpgGrade(grade.getRate());
        LogUtils.printLog(1, 5, TAG, "--->>>getProgrammeGrade(): currentGrade(originalGrade) is  " + currentEitGrade
            + "(" + originalGrade + ")");
    }
    
    private void showDefaultOSD(String data)
    {
        // osd scroll banner
        final CustomMarqueeText autoScrollTextView = (CustomMarqueeText)findViewById(R.id.tv_osd);
        // 设置文本内容
        autoScrollTextView.setText(data);
        // 设置背景颜色
        autoScrollTextView.setBackgroundDrawable(getResources().getDrawable(R.drawable.purple));
        // 设置字体颜色
        // autoScrollTextView.setTextSize(20.0f);
        autoScrollTextView.setTextColor(getResources().getColor(R.drawable.white));
        
        // 设置位置
        ViewTreeObserver vto2 = autoScrollTextView.getViewTreeObserver();
        vto2.addOnGlobalLayoutListener(new OnGlobalLayoutListener()
        {
            @Override
            public void onGlobalLayout()
            {
                autoScrollTextView.getViewTreeObserver().removeGlobalOnLayoutListener(this);
                
                int areaWidth = autoScrollTextView.getWidth();
                int areaHeight = autoScrollTextView.getHeight();
                
                Log.e(TAG, "--->>> osd show area[width,height]" + "[" + areaWidth + "," + areaHeight + "]");
                autoScrollTextView.setStartPosition(areaWidth);
            }
        });
        
        // 设置滚动速度
        autoScrollTextView.setSpeed(Config.CA_OSD_INTERMEDIATE_SPEED);
        
        // 设置滚动次数
        autoScrollTextView.setRepeatTime(1);
        autoScrollTextView.startScroll();
    }
    
    private void ippvPurchase(Message msg)
    {
        // clear dialogs
        if (tvDialogMessage.isShown())
        {
            tvDialogMessage.setVisibility(View.INVISIBLE);
        }
        
        clearIppvBuffer();
        clearDialogs();
        
        Bundle bundle = (Bundle)msg.obj;
        final ippnotifymsg ippvData = (ippnotifymsg)bundle.getSerializable("ippv_data");
        
        if (null != ippvData)
        {
            int status = ippvData.getiStatus(); // 0 允许购买 2 余额不足
            if (status == 2)
            {
                // 弹框
                alertAssistDialog(getString(R.string.cti_ippv_status3));
            }
            else if (status == 0)
            {
                llNotice.setVisibility(View.VISIBLE);
                tvCaPwd.requestFocus();
                tvCaPwd.setFocusable(true);
                tvCaPwd.setFocusableInTouchMode(true);
                
                TextView tvName = (TextView)findViewById(R.id.tv_ippv_service_id);
                TextView tvCompany = (TextView)findViewById(R.id.tv_ippv_company);
                TextView tvCompanyName = (TextView)findViewById(R.id.tv_ippv_company_name);
                TextView tvUnit = (TextView)findViewById(R.id.tv_ippv_purchase_unit);
                
                Button btByChannel = (Button)findViewById(R.id.bt_ippv_by_channel);
                Button btByTime = (Button)findViewById(R.id.bt_ippv_by_time);
                Button btNotBuy = (Button)findViewById(R.id.bt_ippv_not_buy);
                btByChannel.setOnFocusChangeListener(new ButtonOnFocusChangeListener());
                btByTime.setOnFocusChangeListener(new ButtonOnFocusChangeListener());
                btNotBuy.setOnFocusChangeListener(new ButtonOnFocusChangeListener());
                
                tvName.setText(String.valueOf(int2Long(ippvData.getiIppvNum())));
                tvCompany.setText(getHexString(ippvData.getiIppId()));
                tvCompanyName.setText(ippvData.getsIppidName());
                
                int typeP = ippvData.getiiType_p();
                int typeT = ippvData.getiiType_t();
                
                int priceP = ippvData.getiPrice_p();
                int priceT = ippvData.getiPrice_t();
                
                String textP = getString(R.string.cti_ippv_purchase_by_channel_tips);
                String unitP = getString(R.string.cti_ippv_purchase_by_channel_unit);
                String textT = getString(R.string.cti_ippv_purchase_by_time_tips);
                String unitT = getString(R.string.cti_ippv_purchase_by_time_unit);
                
                // not by channel,not by time.
                if (typeP == 0 && typeT == 0)
                {
                    btByChannel.setFocusable(false);
                    btByChannel.setClickable(false);
                    
                    btByTime.setFocusable(false);
                    btByTime.setClickable(false);
                }
                // not by channel,by time.
                else if (typeP == 0 && typeT == 1)
                {
                    btByChannel.setFocusable(false);
                    btByChannel.setClickable(false);
                    
                    LogUtils.printLog(1, 3, TAG, "--->>> cti ippv support purchasing by time !! ");
                    tvUnit.setText(textT + priceT * 0.01 + unitT);
                }
                // by channel,not by time.
                else if (typeP == 1 && typeT == 0)
                {
                    btByTime.setFocusable(false);
                    btByTime.setClickable(false);
                    
                    LogUtils.printLog(1, 3, TAG, "--->>> cti ippv support purchasing by channel !! ");
                    tvUnit.setText(textP + priceP * 0.01 + unitP);
                }
                else
                {
                    tvUnit.setText(textT + priceT * 0.01 + unitT + "\n" + textP + priceP * 0.01 + unitP);
                }
                
                btByChannel.setOnClickListener(new OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        ippBuy(8, ippvData.getiIppvNum());
                    }
                });
                
                btByTime.setOnClickListener(new OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        ippBuy(1, ippvData.getiIppvNum());
                    }
                });
                
                btNotBuy.setOnClickListener(new OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        if (llNotice.isShown())
                        {
                            LogUtils.e(TAG,
                                "TVChannelPlay.playerHandler.NIT_UPDATE:: btNotBuy.setOnClickListener setLeftRightFunction to volume");
                            
                            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
                            llNotice.setVisibility(View.INVISIBLE);
                            clearIppvBuffer();
                            showDialogMessage(R.string.E20);
                        }
                    }
                });
                
            }
        }
    }
    
    private void alertAssistDialog(String message)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(getResources().getString(R.string.cti_ippv_title));
        builder.setMessage(message);
        builder.setPositiveButton(R.string.ca_ok, new android.content.DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface dialog, int which)
            {
                dialog.dismiss();
                showDialogMessage(R.string.E20);
            }
        });
        
        builder.create().show();
    }
    
    private long int2Long(int value)
    {
        Long lo = Long.valueOf("4294967296");
        return value + ((value < 0) ? lo : 0);
    }
    
    private String getHexString(int id)
    {
        String hexString = Integer.toHexString(id);
        if (hexString.length() == 1)
        {
            return "0x00000" + hexString;
        }
        if (hexString.length() == 2)
        {
            return "0x0000" + hexString;
        }
        if (hexString.length() == 3)
        {
            return "0x000" + hexString;
        }
        if (hexString.length() == 4)
        {
            return "0x00" + hexString;
        }
        if (hexString.length() == 5)
        {
            return "0x0" + hexString;
        }
        if (hexString.length() == 6)
        {
            return "0x" + hexString;
        }
        
        return null;
    }
    
    private void ippBuy(int buyWay, int channelId)
    {
        if (caPwdInput.length() < 4)
        {
            Animation shake = AnimationUtils.loadAnimation(this, R.anim.shake);
            tvCaPwd.startAnimation(shake);
        }
        else
        {
            NativeCA nativeCA = new NativeCA();
            LogUtils.printLog(1, 3, TAG, "--->>> input pin: ---" + pincode);
            caIppvBuy ippvBuy = nativeCA.new caIppvBuy(buyWay, channelId, pincode);
            LogUtils.printLog(1, 3, TAG, "--->>> ippv purchase params::  ---" + buyWay + "  " + channelId + "  "
                + pincode);
            nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_IPPV_QUERY, ippvBuy, pincode);
            
            clearIppvBuffer();
            LogUtils.e(TAG, "TVChannelPlay.playerHandler.ippBuy::  setLeftRightFunction to volume");
            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
            llNotice.setVisibility(View.INVISIBLE);
        }
    }
    
    private void dismissNobuttonsDialog()
    {
        if (null != eitCategoryDialog)
        {
            eitCategoryDialog.dismiss();
            eitCategoryDialog = null;
            return;
        }
        if (null != eitLevelDialog)
        {
            eitLevelDialog.dismiss();
            eitLevelDialog = null;
            return;
        }
        if (null != caLevelDialog)
        {
            caLevelDialog.dismiss();
            caLevelDialog = null;
            return;
        }
    }
    
    class HideFingerPrint implements Runnable
    {
        public void run()
        {
            tvFpRt.setVisibility(View.INVISIBLE);
        }
    }
    
    class HideRecTask implements Runnable
    {
        public void run()
        {
            if (llPvr.isShown())
            {
                llPvr.setVisibility(View.INVISIBLE);
            }
        }
    }
    
    class PVRPositiveButtonListener implements OnClickListener
    {
        public void onClick(View v)
        {
            LogUtils.printLog(1, 3, TAG, "--->>> rec status is: " + isRec);
            LogUtils.printLog(1, 3, TAG, "--->>> tms status is: " + isTs);
            finishTms();
            handler.sendEmptyMessage(Config.SWITCH_TO_VOICE_CONTROL);
        }
    }
    
    class ButtonOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            Button button = (Button)v;
            if (hasFocus)
            {
                button.setBackgroundResource(R.drawable.button_search_light);
                button.setTextColor(getResources().getColor(R.color.white_color));
            }
            else
            {
                button.setBackgroundResource(R.drawable.button_search);
                button.setTextColor(getResources().getColor(R.color.black));
            }
        }
    }
    
    class TextViewOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            TextView tv = (TextView)v;
            if (hasFocus)
            {
                tv.setBackgroundResource(R.drawable.select_s_focus);
            }
            else
            {
                tv.setBackgroundResource(R.drawable.select_s);
            }
        }
    }
    
    private BroadcastReceiver recReceiver = new BroadcastReceiver()
    {
        public void onReceive(Context context, Intent intent)
        {
            LogUtils.printLog(1, 3, TAG, "-----Recevie the broadcast!!--------");
            
            // hide rec tag
            if (ivRec.isShown())
            {
                ivRec.setVisibility(View.INVISIBLE);
                isRec = false;
            }
            handler.post(hideRecTask);
        }
    };
    
    private class PvrConn implements ServiceConnection
    {
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            pvrRecService = PvrRecordServiceAIDL.Stub.asInterface(service);
            try
            {
                if (null != service && null != pvrRecService)
                {
                    PvrRecordServiceAIDL.Stub.asInterface(service).stopRecord();
                }
            }
            catch (RemoteException e)
            {
                e.printStackTrace();
                LogUtils.printLog(1, 3, TAG, "-------Pvr Servcie AIDL exception!!!----");
            }
        }
        
        public void onServiceDisconnected(ComponentName name)
        {
        }
    }
}
