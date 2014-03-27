package com.pbi.dvb.nppvware;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.remote.Remote;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.AbsoluteLayout;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.nppvware.Player.MediaPlayerCore;
import com.pbi.dvb.nppvware.plugins.PaintSurface;
import com.pbi.dvb.nppvware.NpHls;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.LogUtils;

public class NpPvwareActivity extends Activity
{
    protected static final String TAG = "NpPvwareActivity";
    
    public static WebView wv;
    
    public static String FilePath;
    
    public Handler mMediaHandler = null;
    
    private RunJavaScript mJS;
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);// BY SCZ 12-08-20
        // getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN ,
        // WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.main);
        
        // BrowserJni jni = new BrowserJni();
        NativeDrive jni = new NativeDrive(NpPvwareActivity.this);
        // int i = jni.plus(22, 33);
        
        // set remote left/right function
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        
        wv = (WebView)findViewById(R.id.wv);
        wv.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);
        
        wv.getSettings().setPluginsEnabled(true);
        wv.getSettings().setJavaScriptEnabled(true);
        wv.setVerticalScrollBarEnabled(false);
        wv.setHorizontalScrollBarEnabled(false);// ����������
        
        mJS = new RunJavaScript(this);
        wv.addJavascriptInterface(mJS, "pbiandroidvideo");
        wv.addJavascriptInterface(this, "koal");

		new NpHls(wv);
        
        FilePath = this.getFilesDir() + "/";
        
        Bundle bundle = this.getIntent().getExtras();
        int logicalNumber = 0,frequency = 0,symb = 0,mod = 0,serviceId = 0,pmtPid = 0;
        String serviceName = null,currentServiceType = null ;
        if (null != bundle)
        {
             logicalNumber = bundle.getInt("logical_number");
             serviceName = bundle.getString("service_name");
             frequency = bundle.getInt("frequency");
             symb = bundle.getInt("symbrate");
             mod = bundle.getInt("qam");
             serviceId = bundle.getInt("service_id");
             pmtPid = bundle.getInt("pmt_pid");
             currentServiceType = bundle.getString("dvb_current_service_type");
        }
        
    String pvrData ="dvbc://" 
            + logicalNumber + "." 
            + serviceName + "."
            + frequency + "." 
            + symb+ "." 
            + mod+ "."
            +serviceId+ "."
            +pmtPid;
    
        String new_url =
            String.format(Config.PVR_SERVER+"/index.html?ch_num=%s&servtype=%s",
                pvrData,
                currentServiceType);
        Log.d("PXWANG say  :", pvrData);
        
        Log.d("PXWANG say mac address   :", getMacAddress());
        
        wv.loadUrl(new_url);
        Log.d("PXWANG say  :", new_url);
        
        wv.setBackgroundColor(0);
        wv.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
        wv.getBackground().setAlpha(0);
        
        wv.setOnKeyListener(new View.OnKeyListener()
        {
            public boolean onKey(View v, int keyCode, KeyEvent event)
            {
                // TODO Auto-generated method stub
                if (event.getAction() == KeyEvent.ACTION_DOWN)
                {
                    if ((event.getKeyCode() == 24) || (event.getKeyCode() == 25))
                        return false;
                    /*
                     * else if (keyCode == KeyEvent.KEYCODE_BACK) { back2DVB(101, 101, "BTV北京卫视"); }
                     */
                    else
                    {
                        String szLog =
                            String.format("OnKeyListener keycode=%d, event.keycode=%d", keyCode, event.getKeyCode());
                        
                        // String szUrl= String.format("javascript:music_onKey(%d)", event.getKeyCode());
                        String szUrl = String.format("javascript:Pvr_onkey(%d)", event.getKeyCode());
                        Log.d("PXWANG say  :", szUrl);
                        wv.loadUrl(szUrl);
                        wv.setBackgroundColor(Color.BLACK);
                        return true;
                    }
                    // return true;
                }
                return false;
            }
            
        });
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        // TODO Auto-generated method stub
        if (KeyEvent.KEYCODE_BACK == keyCode)
        {
            if (g_MediaController != null)
            {
                Message msg = g_MediaController.obtainMessage();
                msg.what = PaintSurface.g_MSG_PlayerCtrl_Release;
                g_MediaController.sendMessage(msg);
            }
            
            finish();
            return true;
        }
        
        return super.onKeyDown(keyCode, event);
    }
    
    private static Handler g_MediaController = null;
    
    private static MediaPlayerCore mPlayer = null;
    
    public static void setMediaController(Handler h, MediaPlayerCore p)
    {
        g_MediaController = h;
        mPlayer = p;
    }
    
    public static void Event_PlayComplete()
    {
        wv.loadUrl("javascript:Event_PlayComplete()");
    }
    
    public static void Event_PlayError()
    {
        wv.loadUrl("javascript:Event_PlayError()");
    }
    
    public static void Event_PlayPrepared()
    {
        wv.loadUrl("javascript:Event_PlayPrepared()");
    }
    
    public static String Get_Local()
    {
        return FilePath;
    }
    
    public static void SavePicture(String PicWebUrl, String PicLocalPath)
    {
        
        Log.d("PXWANG say  PicWebUrl: ", PicWebUrl);
        Log.d("PXWANG say  PicLocalPath: ", PicLocalPath);
        try
        {
            FileOutputStream fos = new FileOutputStream(FilePath + PicLocalPath);
            InputStream is = new URL(PicWebUrl).openStream();
            
            int data = is.read();
            while (data != -1)
            {
                fos.write(data);
                data = is.read();
            }
            is.close();
            fos.close();
            
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        
        Log.d("PXWANG say  Finish: ", PicLocalPath);
    }
    
    public static void SaveFile(String FileData, String FileLocalPath)
    {
        
        Log.d("PXWANG say  PicWebUrl: ", FileLocalPath);
        try
        {
            FileOutputStream fos = new FileOutputStream(FilePath + FileLocalPath);
            byte[] bytes = FileData.getBytes();
            fos.write(bytes);
            fos.close();
            
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        
    }
    
    public void playVideoCommandFormJS(String szUrl)
    {
        Log.d("pxwang_plugin", "***********playVideoCommandFormJS***********");
        if (g_MediaController != null)
        {
            Log.d("dw_plugin", "***********g_MediaController is not null***********");
            Message msg = g_MediaController.obtainMessage();
            msg.what = PaintSurface.g_MSG_PlayerCtrl_Play;
            msg.obj = new String(szUrl);
            g_MediaController.sendMessage(msg);
        }
        // playVideoOnBox(szUrl);
    }
    
    public void stopVideoCommandFormJS()
    {
        Log.d("pxwang_plugin", "***********stopVideoCommandFormJS***********");
        if (g_MediaController != null)
            g_MediaController.sendEmptyMessage(PaintSurface.g_MSG_PlayerCtrl_Stop);
    }
    
    public void releaseVideoCommandFormJS()
    {
        if (g_MediaController != null)
            g_MediaController.sendEmptyMessage(PaintSurface.g_MSG_PlayerCtrl_Release);
    }
    
    public void pauseVideoCommandFormJS()
    {
        if (g_MediaController != null)
            g_MediaController.sendEmptyMessage(PaintSurface.g_MSG_PlayerCtrl_Pause);
    }
    
    public void resumeVideoCommandFormJS()
    {
        if (g_MediaController != null)
            g_MediaController.sendEmptyMessage(PaintSurface.g_MSG_PlayerCtrl_Resume);
    }
    
    public void seektoVideoCommandFormJS(int millsec)
    {
        if (g_MediaController != null)
        {
            String val = String.valueOf(millsec);
            Log.d("dw_plugin", "***********g_MediaController is not null***********");
            Message msg = g_MediaController.obtainMessage();
            msg.what = PaintSurface.g_MSG_PlayerCtrl_Seekto;
            msg.obj = new String(val);
            g_MediaController.sendMessage(msg);
        }
    }
    
    public boolean isplayingVideoCommandFormJS()
    {
        boolean value = false;
        // if(g_MediaController != null)
        // value = mPlayer.isPlaying();
        return value;
    }
    
    public int getvideowidthVideoCommandFormJS()
    {
        int value = 0;
        if (mPlayer == null)
        {
            value = 555;
            return value;
        }
        if (g_MediaController != null && mPlayer != null)
            value = mPlayer.getvideowidth();
        return value;
    }
    
    public int getvideoheightVideoCommandFormJS()
    {
        int value = 0;
        if (g_MediaController != null)
            value = mPlayer.getvideoheight();
        return value;
    }
    
    public int getcurrentpositionVideoCommandFormJS()
    {
        int value = 0;
        if (g_MediaController != null)
            value = mPlayer.getcurrentposition();
        return value;
    }
    
    public int getdurationVideoCommandFormJS()
    {
        int value = 0;
        if (g_MediaController != null)
            value = mPlayer.getduration();
        return value;
    }
    
    public void setloopingVideoCommandFormJS(int loop_flag)
    {
        if (g_MediaController != null)
        {
            String val = String.valueOf(loop_flag);
            Message msg = g_MediaController.obtainMessage();
            msg.what = PaintSurface.g_MSG_PlayerCtrl_SetLooping;
            msg.obj = new String(val);
            g_MediaController.sendMessage(msg);
        }
    }
    
    public boolean isloopingVideoCommandFormJS()
    {
        boolean value = false;
        // if(g_MediaController != null)
        // value = mPlayer.isLooping();
        return value;
    }
    
    public void setvolumeVideoCommandFormJS(int volume_val)
    {
        if (g_MediaController != null)
        {
            String val = String.valueOf(volume_val);
            Message msg = g_MediaController.obtainMessage();
            msg.what = PaintSurface.g_MSG_PlayerCtrl_SetVolume;
            msg.obj = new String(val);
            g_MediaController.sendMessage(msg);
        }
    }
    
    public void callbrowser(final String szTest)
    {
        /*
         * Intent mIntent = new Intent("android.intent.action.MAIN"); ComponentName comp = new
         * ComponentName("com.android.browser", "com.android.browser.BrowserActivity"); mIntent.setComponent(comp);
         * mIntent.addCategory("android.intent.category.LAUNCHER"); startActivity(mIntent); //finish();
         */
        
        /*
         * Uri u = Uri.parse("http://www.sohu.com"); Intent it = new Intent(); it.setData(u); it.setAction(
         * Intent.ACTION_VIEW); it.setClassName("com.android.browser","com.android.browser.BrowserActivity");
         * startActivity(it);
         */
        // Log.d("callbrowser", szTest);
        Uri u = Uri.parse(szTest);
        Intent it = new Intent();
        it.setData(u);
        it.setAction(Intent.ACTION_VIEW);
        it.setClassName("com.android.browser", "com.android.browser.BrowserActivity");
        startActivity(it);
    }
    
    public void pxwang_debug(final String szTest)
    {
        Log.d("pxwang_show", szTest);
    }
    
    public void finishactivity()
    {
        finish();
        android.os.Process.killProcess(android.os.Process.myPid());
    }
    
    protected static String[] mPP_Buf = new String[] {"-cache", "7680", "-cache-min", "4", "-sky_aspect", "1"};
    
    private static final int START_VIDEOPLAYER_REQUEST = 333;
    
    private void playVideoOnBox(String szUrl)
    {
        Intent intent = new Intent();
        intent.setClassName("com.skyviia.VideoPlayer", "com.skyviia.VideoPlayer.Player.VideoPlayerUI_OSD");
        Uri uri = Uri.parse(szUrl);
        intent.setData(uri);
        intent.putExtra("PLAYER_PARAMS", mPP_Buf);
        // intent.putExtra("DISPLAY_MODE", WindowManager.DISP_MODE.DISP_720P.getValue());
        startActivityForResult(intent, START_VIDEOPLAYER_REQUEST);
    }
    
    public String getMacAddress()
    {
        String macAddress = SystemProperties.get("ubootenv.var.ethaddr");
        return macAddress.toLowerCase();
    }
    
    public void back2DVB(int serviceId, int logicalNumber, String serviceName, String serviceType)
    {
        // stop the player
        if (null != mPlayer)
        {
            mPlayer.release();
            mPlayer = null;
        }
        
        Log.d("pxwang_plugin", "***********back2DVB***********");
        Intent intent = new Intent();
        intent.setClassName("com.pbi.dvb", "com.pbi.dvb.SplashActivity");
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        Uri uri;
        if (null == serviceType)
        {
            uri = Uri.parse("dvb:television");
        }
        else
        {
            uri = Uri.parse("dvb:" + serviceType);
        }
        intent.setData(uri);
        writeDVBParams(serviceId, logicalNumber, serviceName, serviceType);
        startActivity(intent);
    }
    
    private void writeDVBParams(int serviceId, int logicalNumber, String serviceName, String serviceType)
    {
        try
        {
            Context dvbContext = this.createPackageContext("com.pbi.dvb", Context.CONTEXT_IGNORE_SECURITY);
            Editor edit =
                dvbContext.getSharedPreferences(Config.DVB_LASTPLAY_SERVICE,
                    Context.MODE_WORLD_READABLE + Context.MODE_WORLD_WRITEABLE).edit();
            if (serviceType.equalsIgnoreCase("broadcast"))
            {
                edit.putInt("radio_serviceId", serviceId);
                edit.putInt("radio_logicalNumber", logicalNumber);
                edit.putString("radio_serviceName", serviceName);
            }
            else
            {
                android.provider.Settings.System.putString(getContentResolver(), "last_play_entrance", "TV");
                edit.putInt("tv_serviceId", serviceId);
                edit.putInt("tv_logicalNumber", logicalNumber);
                edit.putString("tv_serviceName", serviceName);
                edit.commit();
            }
        }
        catch (NameNotFoundException e)
        {
            e.printStackTrace();
        }
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        
        if (null != mPlayer)
        {
            mPlayer.release();
            mPlayer = null;
        }
        wv.freeMemory();
        
    }
}
