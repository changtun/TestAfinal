package com.pbi.dvb;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.content.res.Resources;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.graphics.drawable.BitmapDrawable;
import android.media.AudioManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.net.Uri;

import com.pbi.dvb.domain.UserBehaviorSurvey;
import com.pbi.dvb.dvbinterface.MessagePVR;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.service.UsbStateService;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrUtils;

//import com.pbi.dvb.utils.UserBehaviorSurveyCenter;

public class PvrPlay extends Activity
{
    private String TAG = "PvrPlay";
    
    private String absolutePath = "";
    
    private String channelName = "";
    
    private String playTimeProgress = "";
    
    private int playSeekbarProgress = 0;
    
    private SurfaceView playSurfaceView;
    
    /*--PVR--*/
    private LinearLayout llPvr;
    
    private TextView tvPvrName;
    
    private TextView tvPvrTime;
    
    private SeekBar sbPvr;
    
    private ImageView ivPvrOper;
    
    private TextView tvPvrOper;
    
    private BitmapDrawable bdPlay;
    
    private BitmapDrawable bdPause;
    
    private BitmapDrawable bdSpeed;
    
    private BitmapDrawable bdBackward;
    
    private boolean isPlayState = false;// play=true；pause=false
    
    private boolean pvrSpeed = false;
    
    private NativeDrive nativeDrive;
    
    private NativePlayer nativePlayer;

	private int screenMode = 0;
    
    /*--Audio--*/
    private AudioManager audioManager;
    
    private boolean g_bAudioIsMute;
    
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.PVR_PLAY_ERROR:
                case Config.PVR_PLAY_EOF:
                    backKeyMethod();
                    break;
                case Config.PVR_REC_REACH_PLAY:
                    
                    Log.i(TAG, "PVR_REC_REACH_PLAY---------------------------------------");
                    tvPvrOper.setText(getString(R.string.pvr_start));
                    ivPvrOper.setImageDrawable(bdPlay);
                    if (!llPvr.isShown())
                        showPvrBanner();
                    handler.postDelayed(new HideRecTask(), 5 * 1000);
                    break;
                case 1:
                    computeProgress();
                    tvPvrTime.setText(playSeekbarProgress+"%");
                    tvPvrTime.invalidate();
                    sbPvr.setProgress(playSeekbarProgress);
                    sbPvr.invalidate();
                    break;
            }
        }
    };
    
    private Timer timer;
    
    private TimerTask timerTask = new TimerTask()
    {
        public void run()
        {
            Message message = new Message();
            message.what = 1;
            if(!pvrSpeed)
            {
                handler.sendMessage(message);
            }
        }
    };
    
    protected void onCreate(Bundle arg0)
    {
        super.onCreate(arg0);
        setContentView(R.layout.pvr_play);
        
        Intent intent = getIntent();
        absolutePath = intent.getStringExtra("absolutePath");
        String fileName = intent.getStringExtra("fileName");
        Log.i(TAG, "absolutePath = " + absolutePath);
        Log.i(TAG, "fileName = " + fileName);
        // absolutePath =
        // /mnt/sda/sda1/pvr/[CCTV文艺][2012-12-26_15-30-40].ts.idx
        // fileName = [CCTV文艺][2012-12-26_15-30-40].ts.idx
        if (!"".equals(fileName))
        {
            int location = fileName.indexOf(".");
            channelName = fileName.substring(1, location);
        }
        
//        IntentFilter intentFilter = new IntentFilter(Intent.ACTION_MEDIA_MOUNTED);
//        intentFilter.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
//        intentFilter.addDataScheme("file");
//        registerReceiver(mMountReceiver, intentFilter);
        
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
        /** start the UsbstateService */
        Intent intent = new Intent(PvrPlay.this, UsbStateService.class);
        this.startService(intent);
        LogUtils.printLog(1, 3, "usbStates", "--- start usb states service!");
        
        if (timer == null)
        {
            timer = new Timer();
            timer.schedule(timerTask, 1000, 1000);
        }
        initWidget();
        initDrive();
        
        playFromStart();
    }
    
    public void onDestroy()
    {
        super.onDestroy();
    }
    
    private void playFromStart()
    {
        int ret = 0;
        NativePlayer.DvbPVR_Play_t tPvrPlayInfo = nativePlayer.new DvbPVR_Play_t();
        tPvrPlayInfo.PvrPlayerType = 2;
        tPvrPlayInfo.sName = absolutePath;// getPrefix(absolutePath).toString();
        ret = nativePlayer.DVBPlayerPvrPlayStart(tPvrPlayInfo);
        if (ret != 0)
            backKeyMethod();
        isPlayState = true;
        
    }
    
    /**
     * /mnt/sda/sda1/pvr/[CCTV文艺][2012-12-26_15-30-40].ts.idx 去掉.idx后缀
     */
    private StringBuilder getPrefix(String absolutePath)
    {
        int position = absolutePath.lastIndexOf(".");
        int length = absolutePath.length();
        if (position >= 0)
            return new StringBuilder(absolutePath.substring(0, length - 4));
        else
            return new StringBuilder("");
    }
    
    private void initDrive()
    {
        nativeDrive = new NativeDrive(PvrPlay.this);
        nativeDrive.pvwareDRVInit();
        nativeDrive.CAInit();
        nativeDrive.DrvPlayerInit();
        
        nativePlayer = NativePlayer.getInstance();
        MessagePVR messagePVR = new MessagePVR(handler);
        nativePlayer.DVBPlayerPvrMessageInit(messagePVR);
        
        // Set audio.
        audioManager = (AudioManager)this.getSystemService(Context.AUDIO_SERVICE);
        int volume = getCurSysVolume();
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
    
    private int getCurSysVolume()
    {
        int max = audioManager.getStreamMaxVolume(audioManager.STREAM_SYSTEM);
        int min = 0;
        int volume = audioManager.getLastAudibleStreamVolume(audioManager.STREAM_SYSTEM);
        volume = (volume - min) * 100 / (max - min);
        if (0 > volume)
        {
            volume = 0;
        }
        if (100 < volume)
        {
            volume = 100;
        }
		System.out.println("GetStreamVolume =" + volume);
        return volume;
    }
    
    private void initWidget()
    {
        playSurfaceView = (SurfaceView)findViewById(R.id.pvr_play_surface);
        SurfaceHolder playSurfaceHolder = playSurfaceView.getHolder();
        playSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        playSurfaceHolder.addCallback(mSHCallback);
        playSurfaceHolder.setFormat(PixelFormat.VIDEO_HOLE);
        
        llPvr = (LinearLayout)this.findViewById(R.id.ll_pvr);
        tvPvrName = (TextView)this.findViewById(R.id.tv_pvr_channel_name);
        tvPvrTime = (TextView)this.findViewById(R.id.tv_pvr_time);
        sbPvr = (SeekBar)this.findViewById(R.id.sb_pvr);
        sbPvr.requestFocus();
        sbPvr.setFocusable(true);
        sbPvr.setFocusable(true);
        tvPvrOper = (TextView)this.findViewById(R.id.tv_pvr_operation);
        ivPvrOper = (ImageView)this.findViewById(R.id.iv_pvr_operation);
        
        Resources res = getResources();
        bdPlay = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_play));
        bdPause = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_stop));
        bdSpeed = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_speed));
        bdBackward = new BitmapDrawable(CommonUtils.getBitmap(this, R.drawable.pvr_backward));
		screenMode = CommonUtils.getScreenMode();
		CommonUtils.setScreenMode(1);// 0:NORMAL 1:FULLSTRETCH 2:RATIO4_3 3:RATIO16_9
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch (keyCode)
        {
            case Config.KEY_VOLUME_DOWN:// Press the volume down key
                volumeDownMethod();
                break;
            case Config.KEY_VOLUME_UP:// Press the volume up key
                volumeUpMethod();
                break;
            case Config.PAGE_UP:
                pvrSpeed = true;
                playSeekbarProgress--;
                if (playSeekbarProgress < 0)
                {
                    playSeekbarProgress = 0;
                }
               
                LogUtils.printLog(1, 3, TAG, "--->>> progress is : " + playSeekbarProgress);
                tvPvrTime.setText(playSeekbarProgress+"%");
                tvPvrTime.invalidate();
                sbPvr.setProgress(playSeekbarProgress);
                sbPvr.invalidate();
                break;
            case Config.PAGE_DOWN:
                pvrSpeed = true;
                playSeekbarProgress++;
                if (playSeekbarProgress > 100)
                {
                    playSeekbarProgress = 100;
                }
                LogUtils.printLog(1, 3, TAG, "--->>> progress is : " + playSeekbarProgress);
                tvPvrTime.setText(playSeekbarProgress+"%");
                tvPvrTime.invalidate();
                sbPvr.setProgress(playSeekbarProgress);
                sbPvr.invalidate();
                break;
        }
        
        if (event.getRepeatCount() == 0)
        {
            switch (keyCode)
            {
                case Config.KEY_PLAY_PAUSE:
                    blueKeyMethod();
                    break;
                case KeyEvent.KEYCODE_BACK:
                    backKeyMethod();
                    break;
                case Config.KEY_MUTE:
                    muteKeyMethod();
                    break;
            }
        }
        return false;
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event)
    {
        if (event.getRepeatCount() == 0)
        {
            switch (keyCode)
            {
                case Config.KEY_GREEN:
                    pvrSpeed = false;
                    LogUtils.printLog(1, 3, TAG, "--->>> back !");
                    if (!llPvr.isShown())
                    {
                        showPvrBanner();
                    }
                    ivPvrOper.setImageDrawable(bdBackward);
                    nativePlayer.DVBPlayerPvrPlaySpeedCtrl(playSeekbarProgress);
                    
                    break;
                case Config.KEY_YELLOW:
                    pvrSpeed = true;
                    LogUtils.printLog(1, 3, TAG, "--->>> forward !");
                    if (!llPvr.isShown())
                    {
                        showPvrBanner();
                    }
                    ivPvrOper.setImageDrawable(bdSpeed);
                    nativePlayer.DVBPlayerPvrPlaySpeedCtrl(playSeekbarProgress);
                    break;
            }
        }
        return false;
    }
    
    private void muteKeyMethod()
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
    
    @SuppressWarnings("static-access")
    private void volumeUpMethod()
    {
        int volume = getCurSysVolume();
        System.out.println("GetStreamVolume =" + volume);
        if (100 > volume)
        {
            volume += 1;
        }
        nativePlayer.DVBPlayerAVCtrlSetVolume(volume);
        nativePlayer.DVBPlayerAVCtrlAudMute(0);
        g_bAudioIsMute = false;
    }
    
    @SuppressWarnings("static-access")
    private void volumeDownMethod()
    {
        int volume = getCurSysVolume();
        System.out.println("GetStreamVolume =" + volume);
        if (0 != volume)
        {
            volume -= 1;
        }
        nativePlayer.DVBPlayerAVCtrlSetVolume(volume);
        nativePlayer.DVBPlayerAVCtrlAudMute(0);
        g_bAudioIsMute = false;
    }
    
    /**
     * 快进
     */
    // private void yellowKeyMethod()
    // {
    // if (pvrSpeed < 5)
    // {
    // pvrSpeed = 5;
    // }
    // pvrSpeed++;
    //
    // if (pvrSpeed > 10)
    // {
    // pvrSpeed = 5;
    // }
    // SpeedAndBackward("X8", bdSpeed, 8);
    // }
    //
    // /**
    // * 快退
    // */
    // private void greenKeyMethod()
    // {
    // SpeedAndBackward("X8", bdBackward, 2);
    // }
    
    class HideRecTask implements Runnable
    {
        public void run()
        {
            if (llPvr.isShown())
            {
                String curState = tvPvrOper.getText().toString();
                // Log.i(TAG, "curState = " + curState);
                if (getString(R.string.pvr_start).equals(curState))
                {
                    llPvr.setVisibility(View.INVISIBLE);
                }
                else if (getString(R.string.pvr_pause).equals(curState))
                {
                    llPvr.setVisibility(View.INVISIBLE);
                }
            }
        }
    }
    
    public void SpeedAndBackward(String oper, BitmapDrawable drawable, int pvrSpeed)
    {
        tvPvrOper.setText(oper);
        ivPvrOper.setImageDrawable(drawable);
        nativePlayer.DVBPlayerPvrPlaySpeedCtrl(pvrSpeed);
        
        if (!llPvr.isShown())
        {
            showPvrBanner();
        }
    }
    
    private void showPvrBanner()
    {
        llPvr.setVisibility(View.VISIBLE);
        tvPvrName.setText(channelName);
        computeProgress();
        tvPvrTime.setText(playSeekbarProgress+"%");
        tvPvrTime.invalidate();
        sbPvr.setProgress(playSeekbarProgress);
        sbPvr.invalidate();
    }
    
    private void computeProgress()
    {
        NativePlayer.PVR_Play_File_Info_t tPvrFileInfo = nativePlayer.new PVR_Play_File_Info_t();
        nativePlayer.DVBPlayerPvrPlayGetFileInfo(tPvrFileInfo);
        NativePlayer.PVR_Play_Status_t tPvrStatus = nativePlayer.new PVR_Play_Status_t();
        nativePlayer.DVBPlayerPvrPlayGetStatus(tPvrStatus);
        
        int start = tPvrFileInfo.getU32StartTimeInMs();
        int end = tPvrFileInfo.getU32EndTimeInMs();
        int cur = tPvrStatus.getU32CurPlayTimeInMs();
        // Log.i(TAG, "start = " + start + "," +
        // PvrUtils.getHMSFromPvrTime(start));
        // Log.i(TAG, "end = " + end + "," + PvrUtils.getHMSFromPvrTime(end));
        // Log.i(TAG, "cur = " + cur + "," + PvrUtils.getHMSFromPvrTime(cur));
        
//        playTimeProgress = PvrUtils.getHMSFromPvrTime(cur) + "/" + PvrUtils.getHMSFromPvrTime(end);
//        playTimeProgress = (cur * 100 / end) + "%";
        playSeekbarProgress = (int)((double)cur * 100 / end);
    }
    
    /**
     * 暂停和播放
     */
    private void blueKeyMethod()
    {
        if (isPlayState)
        {// 播放状态-->暂停
            Log.i(TAG, "play --> pause , isPlayState = " + isPlayState);
            isPlayState = false;
            ivPvrOper.setImageDrawable(bdPause);
            tvPvrOper.setText(getString(R.string.pvr_pause));
            nativePlayer.DVBPlayerPvrPlayPause();
        }
        else
        {// 暂停状态-->播放状态
            Log.i(TAG, "pause --> play , isPlayState = " + isPlayState);
            isPlayState = true;
            ivPvrOper.setImageDrawable(bdPlay);
            tvPvrOper.setText(getString(R.string.pvr_start));
            nativePlayer.DVBPlayerPvrPlayResume();
        }
        
        if (!llPvr.isShown())
        {
            showPvrBanner();
            handler.postDelayed(new HideRecTask(), 15 * 1000);
        }
        else
        {
            handler.postDelayed(new HideRecTask(), 15 * 1000);
        }
    }
    
    /**
     * 退出
     */
    private void backKeyMethod()
    {
        nativePlayer.DVBPlayerPvrPlayStop();
        nativePlayer.DVBPlayerPvrMessageUnInit();
        nativeDrive.DrvPlayerUnInit();
        if (timer != null)
        {
            timer.cancel();
            timer = null;
        }
        finish();
    }
    
    protected void onPause()
    {
        super.onPause();
		CommonUtils.setScreenMode(screenMode);
        backKeyMethod();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
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
}
