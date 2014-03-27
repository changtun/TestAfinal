package com.pbi.dvb.factorytest;

import android.os.SystemProperties;
import android.net.ethernet.EthernetDevInfo;
import android.net.ethernet.EthernetManager;
import android.net.NetworkUtils;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.media.AudioManager;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Bundle;
import android.view.Menu;//add
import android.view.MenuItem; //add
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.MountInfoBean;
import com.pbi.dvb.dvbinterface.MessagePlayer;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeDrive;
import com.pbi.dvb.dvbinterface.NativeInstallation;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_AVPid_t;
import com.pbi.dvb.dvbinterface.NativePlayer.DVBPlayer_Play_Param_t;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.StorageUtils;

public class FactoryTest extends Activity
{
    static final String TAG = "FactoryTest";
    
    int current_item = 2;
    
    static final int MAX_ITEM = 24;
    
    boolean bFrontPanel = false;
    
    boolean bUsb1 = false;
    
    boolean bUsb2 = false;
    
    boolean bUsb3 = false;
    
    boolean bsdCard = false;
    
    boolean bEthernet = false;
    
    boolean isfirst = true;
    
    boolean bDVB = false;
    
    boolean bSmart = false;
    
    // boolean bHdmi = false;
    // boolean bHdmiSpidf = false;
    boolean bAVTest = false;
    
    boolean bTube = false;
    
    int w = 0;
    
    // boolean bTuner = false;
    protected Handler m_timeHandle = new Handler();
    
    // private ISkyPlayerService m_Service = null;
    private Context m_AppContext = null;
    
    int audio_out_modeBak = 0;
    
    int testType = 0;// 1-> hdmi,2->av; 3->spdif,4->tuner
    
    String StrMAC = "";
    
    // private ToggleButton toggle;
    private TextView ipStateTextView;
    
    private TextView ipAddrTextView;
    
    private TextView frontpanel, tv, tv5, frontpanel_button;
    
    private EthernetManager mEthManager;
    
    private String ipAddr;
    
    private boolean isFirstInFactory = true;
    
    private String versionnumber = "";
    
    private java.util.Timer timer;
    
    private TextView textView1, textView2, textView3, textView4, textView5, textView6, textView7, textView8, textView9,
        textView10, textView11;
    
    public static TextView textView12;
    
    public static boolean is_success, is_frontpanel;
    
    final String[] strArr = new String[] {"00x00", "11x11", "22x22", "33x33", "44x44", "55x55", "66x66", "77x77",
        "88x88", "99x99"};
    
    private NativeDrive drive;
    private boolean isAVTest = false;
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        int i = 0;
        super.onCreate(savedInstanceState);
        setContentView(R.layout.factorytest);
        textView1 = (TextView)findViewById(R.id.text_type1);
        textView2 = (TextView)findViewById(R.id.text_type2);
        textView3 = (TextView)findViewById(R.id.text_type3);
        textView4 = (TextView)findViewById(R.id.text_type4);
        textView5 = (TextView)findViewById(R.id.text_type5);
        textView6 = (TextView)findViewById(R.id.text_type6);
        textView7 = (TextView)findViewById(R.id.text_type7);
        textView8 = (TextView)findViewById(R.id.text_type8);
        textView9 = (TextView)findViewById(R.id.text_type9);
        textView10 = (TextView)findViewById(R.id.text_type10);
        textView11 = (TextView)findViewById(R.id.text_type11);
        textView12 = (TextView)findViewById(R.id.text_type12);
        is_frontpanel = false;
        m_AppContext = getApplicationContext();
        TextView versionTextView = (TextView)findViewById(R.id.test_version);
        versionnumber = getContentResolver().getType(Uri.parse("content://com.pbi.pbiprovider/pbi/0"));
        // versionTextView.setText(getResources().getString(R.string.testversion)+getResources().getString(R.string.versionnumber));
        versionTextView.setText(versionnumber);
        tv = (TextView)findViewById(R.id.factorytest_1);
        tv.setBackgroundColor(0xFFFFFF00);
        tv.setTextColor(0xFF0000FF);
        tv5 = (TextView)findViewById(R.id.factorytest_5); // mac
        frontpanel_button = (TextView)findViewById(R.id.factorytest_12);
        StrMAC = SystemProperties.get("ubootenv.var.ethaddr");
        
        // �������״̬��1(ETH_STATE_DISABLED)--�رգ�2(ETH_STATE_ENABLED)--����������ʼ��IPֵ
        // toggle = (ToggleButton) findViewById(R.id.ip_state);
        ipStateTextView = (TextView)findViewById(R.id.factorytest_6);
        ipAddrTextView = (TextView)findViewById(R.id.factorytest_7);
        frontpanel = (TextView)findViewById(R.id.factorytest_8);
        mEthManager = (EthernetManager)getSystemService(Context.ETH_SERVICE);
        Log.e("===========EthernetManager===============", EthernetManager.ETH_STATE_ENABLED + "");
        Log.e("===========EthernetManager===============", EthernetManager.ETH_STATE_DISABLED + "");
        if (mEthManager.getEthState() == EthernetManager.ETH_STATE_ENABLED)
        {
            ipStateTextView.setText(getResources().getString(R.string.ip_open));
            ipAddrTextView.setText(getResources().getString(R.string.ip_addr) + getIpAddr());
            // init frontpanel
            frontpanel.setText(getResources().getString(R.string.frontpanel_on));
        }
        else
        {
            // toggle.setChecked(false);
            ipStateTextView.setText(getResources().getString(R.string.ip_close));
            ipAddrTextView.setText(getResources().getString(R.string.ip_addr));
            // init frontpanel
            frontpanel.setText(getResources().getString(R.string.frontpanel_off));
        }
        
        IntentFilter filter = new IntentFilter();
        filter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        registerReceiver(new NetConnectivityChangeReceiver(), filter);
        
    } // end onCreate
    
    /**
     * ��ȡIP��ַ
     */
    public String getIpAddr()
    {
        EthernetDevInfo mEthInfo = mEthManager.getSavedEthConfig();
        Log.e("TAG", "--------------------" + mEthInfo.getConnectMode());
        Log.e("TAG", "--------------------" + EthernetDevInfo.ETH_CONN_MODE_DHCP);
        Log.e("TAG", "--------------------" + EthernetDevInfo.ETH_CONN_MODE_MANUAL);
        
        if (mEthInfo.getConnectMode().equals(EthernetDevInfo.ETH_CONN_MODE_DHCP)
            || mEthInfo.getConnectMode().equals(EthernetDevInfo.ETH_CONN_MODE_MANUAL))
        {
            DhcpInfo dhcpInfo = mEthManager.getDhcpInfo();
            String ip = NetworkUtils.intToInetAddress(dhcpInfo.ipAddress).getHostAddress();
            return ip;
        }
        return null;
        
    }
    
    private void changeIpAddr(String ipAddr)
    {
        ipAddrTextView.setText(getResources().getString(R.string.ip_addr) + ipAddr);
        if (isfirst)
        {
            
        }
        else
        {
            if (ipAddr.length() > 1)
            {
                textView7.setBackgroundResource(R.drawable.frist_setting_success);
            }
            else
            {
                textView7.setBackgroundResource(R.drawable.frist_setting_fale);
            }
        }
        
    }
    
    boolean enabledipStateTextView = true;
    
    private void networkTest()
    {
        if (!enabledipStateTextView)
        {
            return;
        }
        ipStateTextView.setBackgroundColor(0x7DC0C0C0);
        enabledipStateTextView = false;
        if (ipStateTextView.getText().equals(getResources().getString(R.string.ip_open)))
        {
            mEthManager.setEthEnabled(false);
            ipStateTextView.setText(getResources().getString(R.string.ip_close));
            ipAddrTextView.setText(getResources().getString(R.string.ip_addr) + "");
        }
        else
        {
            mEthManager.setEthEnabled(true);
            ipStateTextView.setText(getResources().getString(R.string.ip_open));
        }
        // if(ipStateTextView.isEnabled()){
        // ipStateTextView.setEnabled(false);
        // if(ipStateTextView.getText().equals(getResources().getString(R.string.ip_open))){
        // mEthManager.setEthEnabled(false);
        // if (mEthManager.getEthState() == EthernetManager.ETH_STATE_DISABLED)
        // {
        // ipStateTextView.setText(getResources().getString(R.string.ip_close));
        // ipAddrTextView.setText(getResources().getString(R.string.ip_addr) +
        // "");
        // }
        // }else{
        // mEthManager.setEthEnabled(true);
        // if (mEthManager.getEthState() == EthernetManager.ETH_STATE_ENABLED) {
        // String ipAddr = getIpAddr();
        // while(ipAddr.equals("0.0.0.0")){
        // ipAddr = getIpAddr();
        // }
        // ipStateTextView.setText(getResources().getString(R.string.ip_open));
        // ipAddrTextView.setText(getResources().getString(R.string.ip_addr) +
        // ipAddr);
        // }
        // }
        // ipStateTextView.setEnabled(true);
        // }
    }
    
    public void AVTestACK()
    {
        AlertDialog dlgPSW;
        LayoutInflater inflater = LayoutInflater.from(this);
        
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.avtestmessage));
        
        dlgPSW =
            new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
                .setCancelable(true)
                .setPositiveButton(R.string.dialog_yes, new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                        bAVTest = true;
                        TextView tv = (TextView)findViewById(R.id.factorytest_1);
                        tv.setText(getResources().getString(R.string.avtestok));
                          textView1.setBackgroundResource(R.drawable.frist_setting_success);
                    }
                    
                })
                .setNegativeButton(R.string.dialog_no, new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                        bAVTest = false;
                        TextView tv = (TextView)findViewById(R.id.factorytest_1);
                        tv.setText(getResources().getString(R.string.avtestfail));
                         textView1.setBackgroundResource(R.drawable.frist_setting_fale);
                    }
                })
                .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 240;
        
        w.setAttributes(lp);
        
    }
    
    private boolean recursiveParse1(File file, int count)
    {
        // Recursively parse all contents.
        File[] files = file.listFiles();
        if (files != null)
        {
            int len = files.length;
            for (int x = 0; x < len; x++)
            {
                File childFile = files[x];
                if (childFile.isDirectory())
                {
                    if (count + 1 < 5)
                    {
                        if (!recursiveParse1(childFile, count + 1))
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    String fileName = childFile.getName();
                    
                    if (fileName.equals("test.txt"))
                    {
                        bUsb1 = true;
                        return false;
                    }
                    
                }
            }
        }
        return true;
    }
    
    private boolean recursiveParse2(File file, int count)
    {
        // Recursively parse all contents.
        File[] files = file.listFiles();
        
        if (files != null)
        {
            int len = files.length;
            for (int x = 0; x < len; x++)
            {
                File childFile = files[x];
                if (childFile.isDirectory())
                {
                    if (count + 1 < 5)
                    {
                        if (!recursiveParse2(childFile, count + 1))
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    String fileName = childFile.getName();
                    
                    if (fileName.equals("test.txt"))
                    {
                        bUsb2 = true;
                        return false;
                    }
                    
                }
            }
        }
        return true;
    }
    
    private boolean recursiveParse3(File file, int count)
    {
        // Recursively parse all contents.
        File[] files = file.listFiles();
        
        if (files != null)
        {
            int len = files.length;
            for (int x = 0; x < len; x++)
            {
                File childFile = files[x];
                if (childFile.isDirectory())
                {
                    if (count + 1 < 5)
                    {
                        if (!recursiveParse3(childFile, count + 1))
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    String fileName = childFile.getName();
                    if (fileName.equals("test.txt"))
                    {
                        bUsb3 = true;
                        return false;
                    }
                    
                }
            }
        }
        return true;
    }
    
    private boolean sdCardParse(File file, int count)
    {
        // Recursively parse all contents.
        File[] files = file.listFiles();
        
        if (files != null)
        {
            int len = files.length;
            for (int x = 0; x < len; x++)
            {
                File childFile = files[x];
                if (childFile.isDirectory())
                {
                    if (count + 1 < 5)
                    {
                        if (!sdCardParse(childFile, count + 1))
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    String fileName = childFile.getName();
                    if (fileName.equals("test.txt"))
                    {
                        bsdCard = true;
                        return false;
                    }
                    
                }
            }
        }
        return true;
    }
    
    void showUsbTestResult2()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        if (bUsb2)
            tv.setText(getResources().getString(R.string.usb2testok));
        else
            tv.setText(getResources().getString(R.string.usb2testfail));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    // TextView tv = (TextView)
                    // findViewById(R.id.factorytest_5);
                    // String stv = tv.getText().toString();
                    // if (bUsb2)
                    // tv.setText("05. USB2 test    ok!");
                    // else
                    // tv.setText("05. USB2 test    fail!");
                }
            })
            
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    void showUsbTestResult3()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        if (bUsb2)
            tv.setText(getResources().getString(R.string.usb3testok));
        else
            tv.setText(getResources().getString(R.string.usb3testfail));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    // TextView tv = (TextView)
                    // findViewById(R.id.factorytest_6);
                    // String stv = tv.getText().toString();
                    // if (bUsb2)
                    // tv.setText("06. USB3 test    ok!");
                    // else
                    // tv.setText("06. USB3 test    fail!");
                }
            })
            
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    void showSDCardTestResult()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
       	if (bsdCard){
			tv.setText(getResources().getString(R.string.sdcardtestok));
			textView2.setBackgroundResource(R.drawable.frist_setting_success );
		}else{
			textView2.setBackgroundResource(R.drawable.frist_setting_fale);
			tv.setText(getResources().getString(R.string.sdcardtestfail));}
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    TextView tv = (TextView)findViewById(R.id.factorytest_2);
                    // String stv = tv.getText().toString();
                    	if (bsdCard){
									tv.setText(getResources().getString(
											R.string.sdcardtestok));
								textView2.setBackgroundResource(R.drawable.frist_setting_success );}
								else{
									textView2.setBackgroundResource(R.drawable.frist_setting_fale);
									tv.setText(getResources().getString(
											R.string.sdcardtestfail));}
                }
            })
            
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    void showUsbTestResult()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
       	if (bUsb1){
			tv.setText(getResources().getString(R.string.usbtestok));
			textView4.setBackgroundResource(R.drawable.frist_setting_success );
		}else{
		tv.setText(getResources().getString(R.string.usbtestfail));
		textView4.setBackgroundResource(R.drawable.frist_setting_fale);}
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    TextView tv = (TextView)findViewById(R.id.factorytest_4);
                    // String stv = tv.getText().toString();
                   	if (bUsb1){
									tv.setText(getResources().getString(
											R.string.usbtestok));
										textView4.setBackgroundResource(R.drawable.frist_setting_success );	
								}else{
									tv.setText(getResources().getString(
										R.string.usbtestfail));
										textView4.setBackgroundResource(R.drawable.frist_setting_fale);}
                }
            })
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    void showEthernetTestResult()
    {
        AlertDialog dlgPSW;
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        if (bEthernet)
        {
            tv.setText(getResources().getString(R.string.ethtestok));
        }
        else
            tv.setText(getResources().getString(R.string.ethtestfail));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    
                    TextView tv = (TextView)findViewById(R.id.factorytest_3);
                    // String stv = tv.getText().toString();
                    if (bEthernet)
                    {
                        tv.setText(getResources().getString(R.string.ethtestok));
                        textView3.setBackgroundResource(R.drawable.frist_setting_success);
                    }
                    else
                    {
                        tv.setText(getResources().getString(R.string.ethtestfail));
                        textView3.setBackgroundResource(R.drawable.frist_setting_fale);
                    }
                    
                }
                
            })
            
            .create();
        
        dlgPSW.show();
        
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    /* 1AVTest */
    public void avTest()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        
        String uri = testPath();
        if (TextUtils.isEmpty(uri))
        {
            tv.setText(getResources().getString(R.string.testfilenoexist));
            dlgPSW =
                new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
                    .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
                    {
                        
                        @Override
                        public void onClick(DialogInterface dialog, int which)
                        {
                            // TODO Auto-generated method stub
                            bAVTest = false;
                            
                        }
                    })
                    .create();
        }
        else
        {
            tv.setText(getResources().getString(R.string.avteststart));
            dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
            // .setCancelable(true)
                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                        bAVTest = true;
                     //   textView1.setBackgroundColor(getResources().getColor(R.color.red));
                        testType = 1;
                        Intent intent = new Intent();
                        intent.setClass(FactoryTest.this, AvPlayer.class);
//                        intent.setClassName("com.pbi.dvb.factorytest", "com.pbi.dvb.factorytest.AvPlayer");
                        // String uri = "/mnt/sda4/test.mp4";
                        String uri = testPath();
                        Log.d(TAG, "uri is : " + uri);
                        Bundle bundle = new Bundle();
                        bundle.putString("url", uri);
                        // intent.setData(uri);
                        intent.putExtras(bundle);
                        // intent.putExtra("DISPLAY_MODE",
                        // WindowManager.DISP_MODE.DISP_720P.getValue());
                        startActivityForResult(intent, 333);
                        
                    }
                    
                })
                .create();
        }
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    private String testPath()
    {
        Log.d(TAG, "testPath");
        List<String> fileList = null;
        BufferedReader br = null;
        String path = null;
        try
        {
            fileList = new ArrayList<String>();
            Runtime r = Runtime.getRuntime();
            Process p = r.exec("ls mnt");
            br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String inline;
            while ((inline = br.readLine()) != null)
            {
                
                if (inline.contains("sd"))
                {
                    fileList.add(inline);
                }
            }
            File f = null;
            for (String str : fileList)
            {
                f = new File("/mnt/" + str + "/test.mp4");
                if (f.exists())
                {
                    path = f.getAbsolutePath();
                    break;
                }
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        finally
        {
            try
            {
                if (br != null)
                {
                    br.close();
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
        }
        
        return path;
    }
    
    /* sdCardTest */
    public void sdCardTest()
    {
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.sdcardteststart));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    File mntFilePath = new File("/mnt/sdcard");
                    dialog.dismiss();
                    try
                    {
                        Thread.sleep(4000);
                    }
                    catch (Exception e)
                    {
                        // exception = e;
                    }
                    bsdCard = false;
                    sdCardParse(mntFilePath, 1);
                    showSDCardTestResult();
                }
                
            })
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    /* ethernetTest */
    public void ethernetTest()
    {
        
        AlertDialog dlgPSW;
        
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.ethteststart));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    dialog.dismiss();
                    m_timeHandle.postDelayed(EthernetOnclickRunnable, 500);
                }
            })
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    /* frontPanelTest */
     private void frontPanelTest() {
    
     // TODO Auto-generated method stub
     // turn off
     if (frontpanel.getText().equals(
     getResources().getString(R.string.frontpanel_on))) {
    
     File file = new File("/sys/class/gpio/cmd");
     if (file != null) {
     OutputStreamWriter osw;
     try {
     osw = new OutputStreamWriter(new FileOutputStream(
     "/sys/class/gpio/cmd"), "utf-8");
     try {
     osw.write("w:d:1:0");
     osw.flush();
     osw.close();
     Log.d(TAG, "Write \"w:d:1:0\" to /sys/class/gpio/cmd");
     } catch (IOException e) {
     e.printStackTrace();
     }
     } catch (UnsupportedEncodingException e1) {
     e1.printStackTrace();
     } catch (FileNotFoundException e1) {
     e1.printStackTrace();
     }
     frontpanel.setText(getResources().getString(
     R.string.frontpanel_off));
     } else {
     frontpanel.setText(getResources().getString(
     R.string.frontpanel_fail));
     Log.d(TAG, "Open file failed !");
     }
     } else {
     // turn on
     File file = new File("/sys/class/gpio/cmd");
     if (file != null) {
     OutputStreamWriter osw;
     try {
     osw = new OutputStreamWriter(new FileOutputStream(
     "/sys/class/gpio/cmd"), "utf-8");
     try {
     osw.write("w:d:1:1");
     osw.flush();
     osw.close();
     Log.d(TAG, "Write \"w:d:1:1\" to /sys/class/gpio/cmd");
     } catch (IOException e) {
     e.printStackTrace();
     }
     } catch (UnsupportedEncodingException e1) {
     e1.printStackTrace();
     } catch (FileNotFoundException e1) {
     e1.printStackTrace();
     }
     frontpanel.setText(getResources().getString(
     R.string.frontpanel_on));
     } else {
     Log.d(TAG, "Open file failed !");
     frontpanel.setText(getResources().getString(
     R.string.frontpanel_fail));
     }
     }
     }
    
    /* USBTest */
    public void usb1Test()
    {
        
        AlertDialog dlgPSW;
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.usbteststart));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    MountInfoBean mobileHDDInfo = new StorageUtils(FactoryTest.this).getMobileHDDInfo();
                    File mntFilePath = null;
                    if (null != mobileHDDInfo)
                    {
                        String path = mobileHDDInfo.getPath();
                        
                        LogUtils.printLog(1, 3, TAG, " path --->>>" +path);
                        mntFilePath = new File(path);
                        dialog.dismiss();
                        try
                        {
                            Thread.sleep(2000);
                        }
                        catch (Exception e)
                        {
                            // exception = e;
                        }
                        
                        
                    }
                    bUsb1 = false;
                    if (null != mntFilePath)
                    {
                        recursiveParse1(mntFilePath, 1);
                    }
                    
                    showUsbTestResult();
                    
                }
                
            })
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    public void usb2Test()
    {
        AlertDialog dlgPSW;
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.usb2teststart));
        dlgPSW =
            new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
                {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                        File mntFilePath = new File("/mnt/sdb1");
                        dialog.dismiss();
                        try
                        {
                            Thread.sleep(5000);
                        }
                        catch (Exception e)
                        {
                            // exception = e;
                        }
                        bUsb2 = false;
                        recursiveParse2(mntFilePath, 1);
                        showUsbTestResult2();
                    }
                })
                .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
        
    }
    
    private void usb3Test()
    {
        
        // TODO Auto-generated method stub
        AlertDialog dlgPSW;
        LayoutInflater inflater = LayoutInflater.from(this);
        final View textEntryView = inflater.inflate(R.layout.test_dialog, null);
        TextView tv = (TextView)textEntryView.findViewById(R.id.test_dlg_textview);
        tv.setText(getResources().getString(R.string.usb3teststart));
        dlgPSW = new AlertDialog.Builder(FactoryTest.this).setView(textEntryView)
        // .setCancelable(true)
            .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int whichButton)
                {
                    File mntFilePath = new File("/mnt/sdc1");
                    dialog.dismiss();
                    try
                    {
                        Thread.sleep(5000);
                    }
                    catch (Exception e)
                    {
                        // exception = e;
                    }
                    bUsb3 = false;
                    recursiveParse3(mntFilePath, 1);
                    showUsbTestResult3();
                    
                }
                
            })
            .create();
        
        dlgPSW.show();
        Window w = dlgPSW.getWindow();
        WindowManager.LayoutParams lp = w.getAttributes();
        lp.x = 80;
        lp.y = 50;
        lp.width = 500;
        lp.height = 440;
        
        w.setAttributes(lp);
    }
    
    public void factorytest()
    {
        Log.e("==========zd====factorytest()========", "current_item : " + current_item);
        switch (current_item)
        {
            case 2:
                isAVTest = true;
                avTest();
                
                break;
            case 4:
                sdCardTest();
              
                break;
            case 6:
                ethernetTest();
                if (bEthernet)
                {
                    textView3.setBackgroundResource(R.drawable.frist_setting_success);
                }
                else
                {
                    textView3.setBackgroundResource(R.drawable.frist_setting_fale);
                }
                break;
            // case 4:
            // frontPanelTest();
            // break;
            case 8:
                usb1Test();
                
                break;
            // case 5:
            // usb2Test();
            // break;
            // case 6:
            // usb3Test();
            // break;
            case 10:
                if (StrMAC.length() > 1)
                {
                    tv5.setText("05. MAC : " + StrMAC);
                    textView5.setBackgroundResource(R.drawable.frist_setting_success);
                }
                else
                {
                    textView5.setBackgroundResource(R.drawable.frist_setting_fale);
                }
                break;
            case 14:
                isfirst = false;
                if (ipAddr.length() > 1)
                {
                    textView7.setBackgroundResource(R.drawable.frist_setting_success);
                }
                else
                {
                    textView7.setBackgroundResource(R.drawable.frist_setting_fale);
                }
                break;
            case 12:
                Log.e("==========zd====factorytest() case 9: ========", "current_item : " + current_item);
                networkTest();
                
                textView6.setBackgroundResource(R.drawable.frist_setting_success);
                
                break;
            case 16:
                Log.d(TAG, "front panel test");
                frontPanelTest();
                
                textView8.setBackgroundResource(R.drawable.frist_setting_success);
                
                break;
            case 18:
                // ������dvb�ɹ����ı�־�滻bAVTest
                dvbTest();
                break;
            case 20:
                nixieTubeTest();
                break;
            case 22:
                smartCardTest();
                if (bSmart)
                {
                    textView11.setBackgroundResource(R.drawable.frist_setting_success);
                }
                else
                {
                    textView11.setBackgroundResource(R.drawable.frist_setting_fale);
                }
                break;
            case 24:
                Log.e("TAG", "===============进来了 24");
                Intent intent1 = new Intent();
                intent1.setClassName("com.pbi.dvb", "com.pbi.dvb.factorytest.FrontpanelActivity");
                startActivity(intent1);
                break;
            default:
                break;
        }
    }
    
    // ����ܲ���
    private void nixieTubeTest()
    {
        drive = new NativeDrive();
        final Timer timer = new Timer();
        
        new AlertDialog.Builder(this).setTitle(getString(R.string.tube))
            .setCancelable(false)
            .setMessage(getString(R.string.fac_tube_message))
            .setPositiveButton(getString(R.string.standby_yes), new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int i)
                {
                    textView10.setBackgroundResource(R.drawable.frist_setting_success);
                    dialog.cancel();
                    timer.cancel();
                    
                    drive.DisPlay("--x--");
                }
            })
            .setNegativeButton(getString(R.string.standby_no), new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int id)
                {
                    textView10.setBackgroundResource(R.drawable.frist_setting_fale);
                    dialog.cancel();
                    timer.cancel();
                    
                    drive.DisPlay("--x--");
                }
            })
            .show();
        
        ScheduleTask task = new ScheduleTask();
        timer.scheduleAtFixedRate(task, 0, 100);
    }
    
    class ScheduleTask extends TimerTask
    {
        private int pos = 0;
        
        public int getPos()
        {
            return pos;
        }
        
        @Override
        public void run()
        {
            if (pos < strArr.length)
            {
                LogUtils.printLog(1, 3, "FactoryTest", strArr[pos]);
                if (null != drive)
                {
                    drive.DisPlay(strArr[pos]);
                }
            }
            ++pos;
            if (pos >= strArr.length)
            {
                pos = 0;
            }
        }
    }
    
    // dvb test
    private void smartCardTest()
    {
        NativeCA nativeCA = new NativeCA();
        int test = nativeCA.factoryResetTest();
        if (test == 0)
        {
            bSmart = true;
        }
        else
        {
            bSmart = false;
        }
    }
    
    // dvb test
    private NativeInstallation dvb;
    
    private NativePlayer dvbPlayer;
    
    private NativeDrive nativeDrive;
    
    @Override
    protected void onStart()
    {
        super.onStart();
        // init dvb
        nativeDrive = new NativeDrive(FactoryTest.this);
        
        // create surfaceview.
        SurfaceView surfaceView = (SurfaceView)findViewById(R.id.video);
        SurfaceHolder surfHolder = surfaceView.getHolder();
        surfHolder.setFixedSize(surfaceView.getWidth(), surfaceView.getHeight());
        surfHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        surfHolder.addCallback(new SurfaceHolder.Callback()
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
        });
        surfHolder.setFormat(PixelFormat.VIDEO_HOLE);
    }
    
    public static Rect getPlayWindow()
    {
        String displaymode_path = "/sys/class/display/mode";
        String dispMode = null;
        Rect rect = new Rect();
        rect.top = 0;
        rect.left = 0;
        rect.bottom = 720 + rect.top - 48;
        rect.right = 1280 + rect.left;
        File file = new File(displaymode_path);
        if (!file.exists())
        {
            return rect;
        }
        
        // read
        try
        {
            BufferedReader in = new BufferedReader(new FileReader(displaymode_path), 32);
            try
            {
                dispMode = in.readLine();
                if (dispMode == null)
                {// not exist,default m2,lvds1080p
                    rect.top = 0;
                    rect.left = 0;
                    rect.bottom = 1079 + rect.top - 48;
                    rect.right = 1919 + rect.left;
                    
                }
                else
                {
                    if (dispMode.equals("480p") || dispMode.equals("480i"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 480 + rect.top - 48;
                        rect.right = 720 + rect.left;
                    }
                    else if (dispMode.equals("576p") || dispMode.equals("576i"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 576 + rect.top - 48;
                        rect.right = 720 + rect.left;
                    }
                    else if (dispMode.equals("720p") || dispMode.equals("720i"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 720 + rect.top - 48;
                        rect.right = 1280 + rect.left;
                    }
                    else if (dispMode.equals("1080p") || dispMode.equals("1080i"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 1080 + rect.top - 48;
                        rect.right = 1920 + rect.left;
                    }
                    
                }
            }
            finally
            {
                in.close();
                return rect;
            }
        }
        catch (IOException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return rect;
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        
        if (!isAVTest)
        {
            if (null != dvbPlayer)
            {
                dvbPlayer.DVBPlayerStop();
                dvbPlayer.DVBPlayerDeinit();
            }
            if (null != nativeDrive)
            {
                nativeDrive.DrvPlayerUnInit();
            }
        }
    }
    
    private void dvbTest()
    {
        isAVTest = false;
        
        nativeDrive.DrvPlayerInit();
        dvbPlayer = NativePlayer.getInstance();
        dvbPlayer.DVBPlayerInit(null);
        
        // **********************預設PID******************************************
        // play dvb
        // dvbPlayer = NativePlayer.getInstance();
        DVBPlayer_Play_Param_t playParams = dvbPlayer.new DVBPlayer_Play_Param_t();
//        MessagePlayer messagePlayer = new MessagePlayer(handler);
//        dvbPlayer.DVBPlayerMessageInit(messagePlayer);
        
        // set window.
        NativePlayer.DRV_AVCtrl_VidWindow_t playWindow = dvbPlayer.new DRV_AVCtrl_VidWindow_t();
        Rect rect = new Rect();
        rect = getPlayWindow();
        playWindow.setWinX((10 + rect.left) * rect.right / 1920);
        playWindow.setWinY((790 + rect.top) * rect.bottom / 1080);
        playWindow.setWinWidth(350 * rect.right / 1920);
        playWindow.setWinHeight(300 * rect.bottom / 1080);
        dvbPlayer.DVBPlayerAVCtrlSetWindows(playWindow, 1);
        
        // Set audio.
        AudioManager audioManager = (AudioManager)FactoryTest.this.getSystemService(Context.AUDIO_SERVICE);
        int volume = 0;
        
        volume = audioManager.getLastAudibleStreamVolume(audioManager.STREAM_SYSTEM);
        volume *= 7;
        if (100 < volume)
        {
            volume = 100;
        }
        dvbPlayer.DVBPlayerAVCtrlSetVolume(volume);
        
        boolean g_bAudioIsMute = audioManager.isStreamMute(AudioManager.STREAM_MUSIC);
        if (true == g_bAudioIsMute)
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(1);
        }
        else
        {
            dvbPlayer.DVBPlayerAVCtrlAudMute(0);
        }
        
        // package play params.
        DVBCore_Cab_Desc_t tCable = new Tuner().new DVBCore_Cab_Desc_t();
        
        char u8TunerId = 0;
        int u32Freq = 306000;
        int u32SymbRate = 5217;
        int eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_256QAM;
        int eFEC_Inner = 0;
        int eFEC_Outer = 0;
        
//         char u8TunerId = 0;
//         int u32Freq = 698000;
//         int u32SymbRate = 6875;
//         int eMod = 2;
//         int eFEC_Inner = 0;
//         int eFEC_Outer = 0;
        
        tCable.setU8TunerId(u8TunerId);
        tCable.setU32Freq(u32Freq);
        tCable.setU32SymbRate(u32SymbRate);
        tCable.seteMod(eMod);
        tCable.seteFEC_Inner(eFEC_Inner);
        tCable.seteFEC_Outer(eFEC_Outer);
        
        DVBPlayer_AVPid_t tAVPids = dvbPlayer.new DVBPlayer_AVPid_t();
        int u16AudioPid = 251;
        int u16VideoPid = 250;
        int u16PCRPid = 250;
        byte u8AudioFmt = 3;
        byte u8VideoFmt = 1;
        
//         int u16AudioPid = 0x3fd;
//         int u16VideoPid = 0x3fc;
//         int u16PCRPid = 0;
//         byte u8AudioFmt = 3;
//         byte u8VideoFmt = 1;
        
        tAVPids.setU16AudioPid(u16AudioPid);
        tAVPids.setU16VideoPid(u16VideoPid);
        tAVPids.setU8AudioFmt(u8AudioFmt);
        tAVPids.setU8VideoFmt(u8VideoFmt);
        
        playParams.seteSignalSource(Config.DVBCore_Signal_Source_eDVBCORE_SIGNAL_SOURCE_DVBC);
        playParams.seteProgType(1);
        playParams.settCable(tCable);
        playParams.settAVPids(tAVPids);
        
        LogUtils.printLog(1, 4, TAG, "**********   dvb factory test params ***********");
        LogUtils.printLog(1, 4, TAG, "*              frequency : " +u32Freq);
        LogUtils.printLog(1, 4, TAG, "*              symbRate : " +u32SymbRate);
        LogUtils.printLog(1, 4, TAG, "*              audioPid : " +u16AudioPid);
        LogUtils.printLog(1, 4, TAG, "*              videoPid : " +u16VideoPid);
        LogUtils.printLog(1, 4, TAG, "*              pcrPid : " +u16PCRPid);
        LogUtils.printLog(1, 4, TAG, "*              audioFmt : " +u8AudioFmt);
        LogUtils.printLog(1, 4, TAG, "*              videoFmt : " +u8VideoFmt);
        dvbPlayer.DVBPlayerPlayProgram(playParams);
        
        new AlertDialog.Builder(this).setTitle(getString(R.string.fac_dvb_test))
        .setCancelable(false)
        .setMessage(getString(R.string.fac_dvb_message))
        .setPositiveButton(getString(R.string.standby_yes), new DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface dialog, int i)
            {
                dialog.cancel();
                textView9.setBackgroundResource(R.drawable.frist_setting_success);
                dvbPlayer.DVBPlayerStop();
            }
        })
        .setNegativeButton(getString(R.string.standby_no), new DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface dialog, int id)
            {
                dialog.cancel();
                textView9.setBackgroundResource(R.drawable.frist_setting_fale);
                dvbPlayer.DVBPlayerStop();
            }
        })
        .show();
        
    }
    
//    private Handler handler = new Handler()
//    {
//        public void handleMessage(Message msg)
//        {
//            switch (msg.what)
//            {
//                case Config.PLAYER_PROGRAM_INTERRUPT:
//                    bDVB = false;
//                    if (bDVB)
//                    {
//                        textView9.setBackgroundResource(R.drawable.frist_setting_success);
//                    }
//                    else
//                    {
//                        textView9.setBackgroundResource(R.drawable.frist_setting_fale);
//                    }
//                    break;
//                
//                case Config.PLAYER_START:
//                    bDVB = true;
//                    if (bDVB)
//                    {
//                        textView9.setBackgroundResource(R.drawable.frist_setting_success);
//                    }
//                    else
//                    {
//                        textView9.setBackgroundResource(R.drawable.frist_setting_fale);
//                    }
//                    break;
//            }
//        };
//    };
    
    public void replaceFile(int i, int which, String path)
    {
        String targetString = "=";
        File file = new File(path);
        if (file.isDirectory())
        {
            Log.d("TestFile", "The File doesn't not exist.");
        }
        String[] str = {"0", "1", "2", "3"};
        String properString = str[which];
        
        // RandomAccessFile raf=new RandomAccessFile("d:/data.txt","rwd");
        try
        {
            InputStream instream = new FileInputStream(file);
            
            InputStreamReader inputreader = new InputStreamReader(instream);
            BufferedReader buffreader = new BufferedReader(inputreader);
            
            BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file)));
            
            StringBuilder sb = new StringBuilder();
            String line;
            Log.e("TAG", "0==========");
            while ((line = buffreader.readLine()) != null)
            {
                sb.append(line);
                Log.e("TAG", "0==========" + sb.toString());
                int count = 0;
                if (!line.startsWith("##"))
                {
                    int index = 0;
                    if (i == count)
                    {
                        index = sb.indexOf(targetString) + 1;
                        sb.replace(index, index + 1, properString);
                        index += properString.length();
                        
                    }
                    count++;
                    bw.write(sb.toString(), 0, sb.length());
                    Log.e("TAG", "==========" + sb.toString());
                    bw.newLine();
                    // sb.delete(0, sb.length());
                }
                
            }
            buffreader.close();
            bw.flush();
            bw.close();
        }
        catch (FileNotFoundException e)
        {
            System.out.println("File not found!!");
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    public String ReadTxtFile(String strFilePath)
    {
        String path = strFilePath;
        String content = ""; // �ļ������ַ�
        // ���ļ�
        File file = new File(path);
        // ���path�Ǵ��ݹ����Ĳ��������һ����Ŀ¼���ж�
        if (file.isDirectory())
        {
            Log.d("TestFile", "The File doesn't not exist.");
        }
        else
        {
            try
            {
                InputStream instream = new FileInputStream(file);
                if (instream != null)
                {
                    InputStreamReader inputreader = new InputStreamReader(instream);
                    BufferedReader buffreader = new BufferedReader(inputreader);
                    String line;
                    // ���ж�ȡ
                    
                    while ((line = buffreader.readLine()) != null)
                    {
                        
                        if (!line.startsWith("##"))
                        {
                            content += line.substring(line.length() - 2, line.length() - 1);
                            
                        }
                        
                    }
                    instream.close();
                }
            }
            catch (java.io.FileNotFoundException e)
            {
                Log.d("TestFile", "The File doesn't not exist.");
            }
            catch (IOException e)
            {
                Log.d("TestFile", e.getMessage());
            }
        }
        return content;
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        boolean bIsHandled = false;
        TextView tv;
        if (event.getRepeatCount() == 0)
        {
            switch (keyCode)
            {
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                {
                    if (current_item == MAX_ITEM)
                        return true;
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFF0000FF);
                    tv.setTextColor(0xFFFFFFFF);
                    current_item += 2;
                    
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFFFFFF00);
                    tv.setTextColor(0xFF0000FF);
                }
                    return true;
                    
                case KeyEvent.KEYCODE_DPAD_LEFT:
                {
                    if (current_item == 2)
                        return true;
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFF0000FF);
                    tv.setTextColor(0xFFFFFFFF);
                    current_item -= 2;
                    
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFFFFFF00);
                    tv.setTextColor(0xFF0000FF);
                }
                    return true;
                    
                case KeyEvent.KEYCODE_DPAD_DOWN:
                {
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFF0000FF);
                    tv.setTextColor(0xFFFFFFFF);
                    current_item += 4;
                    if (current_item > MAX_ITEM)
                    {
                        if (current_item - 2 < MAX_ITEM)
                        {
                            current_item -= 2;
                        }
                        else
                        {
                            current_item = 2;
                        }
                        
                    }
                    
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFFFFFF00);
                    tv.setTextColor(0xFF0000FF);
                    Log.e("86777", "86777" + current_item);
                }
                    return true;
                    
                case KeyEvent.KEYCODE_DPAD_UP:
                {
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFF0000FF);
                    tv.setTextColor(0xFFFFFFFF);
                    current_item = current_item - 4;
                    if (current_item < 2)
                        current_item = MAX_ITEM;
                    tv = (TextView)findViewById(R.id.factorytest_1 + (current_item - 2));
                    tv.setBackgroundColor(0xFFFFFF00);
                    tv.setTextColor(0xFF0000FF);
                }
                    return true;
                    
                case KeyEvent.KEYCODE_ENTER:
                    // Log.d(TAG, "toggle.isChecked : " + toggle.isChecked());
                    factorytest();
                    return true;
                    
                case 23:
                    Log.d(TAG, "KEY 23");
                    factorytest();
                    return true;
            }
            
        }
        return super.onKeyDown(keyCode, event);
        
    } // end key press
    
    protected Runnable EthernetOnclickRunnable = new Runnable()
    {
        public void run()
        {
            
            int i = 0;
            String value1 = null;
            String value2 = null;
            bEthernet = false;
            
            while (i < 4)
            
            {
                // try {
                // Thread.sleep(5000);
                // } catch (Exception e) {
                // exception = e;
                // }
                // String value = null;
                
                NetworkInfo info =
                    (NetworkInfo)((ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
                
                // boolean NetworkInfoboolean = false;
                if (info == null || !info.isConnected())
                {
                    // return false;
                    bEthernet = false;
                }
                else
                {
                    bEthernet = true;
                }
                Log.e("110444444444444444444444", "110444444444444444444444" + bEthernet);
                
                i++;
                
            }
            
            showEthernetTestResult();
            
        }
    };
    
    protected Runnable m_showPreviewDialog2 = new Runnable()
    {
        public void run()
        {
            
            /*
             * SystemService.stop("set_scan");
             * 
             * 
             * 
             * FileReader fr = null; ArrayList<channelInfo>ChInfoListTemp = null; try{ fr = new
             * FileReader("/etc/channels.conf"); }catch(FileNotFoundException e){ return; }
             * 
             * try{ String str=null; BufferedReader br = new BufferedReader (fr);
             * 
             * str = br.readLine(); if(str!=null) {
             * 
             * if(str.indexOf("[")< 0 ) {
             * 
             * } else { bTuner= false; br.close(); frontendTestFail(); return ; } } br.close(); } catch(IOException e){
             * 
             * }
             * 
             * 
             * Intent intent1 = new Intent(FactoryTest.this, TVtest.class);
             * 
             * startActivity(intent1);
             * 
             * testType = 4;
             */
            
        }
    };
    
    @Override
    public void onResume()
    {
        super.onResume();
        
        if (testType == 1)// hdmi
        {
            AVTestACK();
        }
        else if (testType == 2)
        {
            frontPanelACK();
        }
        
        testType = 0;
        if (is_frontpanel)
        {
            if (is_success)
            {
                textView12.setBackgroundResource(R.drawable.frist_setting_success);
            }
            else
            {
                textView12.setBackgroundResource(R.drawable.frist_setting_fale);
            }
        }
        
    }
    
    private void frontPanelACK()
    {
        // TODO Auto-generated method stub
        
    }
    
    class NetConnectivityChangeReceiver extends BroadcastReceiver
    {
        
        public void onReceive(Context context, Intent intent)
        {
            if (intent.getBooleanExtra(ConnectivityManager.EXTRA_NO_CONNECTIVITY, false))
            {
                ipAddr = "";
            }
            else
            {
                ipAddr = getIpAddr();
            }
            changeIpAddr(ipAddr);
            enabledipStateTextView = true;
            if (isFirstInFactory)
            {
                isFirstInFactory = false;
            }
            else if (current_item == 8)
            {
                // ipStateTextView.setBackgroundColor(0xFFFFFF00);
                // ipStateTextView.setTextColor(0xFF0000FF);
            }
        }
    }
    
}// Main end

