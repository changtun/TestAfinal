/*
 * File Name:  TVChannelSearch.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import android.app.Activity;
import android.content.Intent;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnKeyListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.pbi.dvb.adapter.QamAdapter;
import com.pbi.dvb.dvbinterface.MsgTimeUpdate;
import com.pbi.dvb.dvbinterface.Nativetime;
import com.pbi.dvb.dvbinterface.Tuner;
import com.pbi.dvb.dvbinterface.Tuner.CablePercent;
import com.pbi.dvb.dvbinterface.Tuner.DVBCore_Cab_Desc_t;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;

public class TVChannelSearch extends Activity
{
    protected static final String TAG = "TVChannelSearch";
    private TextView tvQAM;
    private TextView tvScan;
    
    private TextView tvStrength;
    private TextView tvQuality;
    
    private Button btSearch;
    private EditText etInputFrequency;
    
    private EditText etSymbolRate;
    
    private PopupWindow popQAM;
    
    private PopupWindow popScan;
    
    private ListView lvQAM;
    
    private ListView lvScan;
    
    private String[] qamArray;
    
    private String[] scanArray;
    
    private String strInputFrequency;
    
    private String strSymbolRate;
    
    private int u32Freq;
    
    private int u32SymbRate;
    
    private String strQam;
    
    private String strScan;
    
    private QamAdapter qamAdapter;
    
    private QamAdapter scanAdapter;
    
    private ProgressBar pbstrength;
    
    private ProgressBar pbquality;
    
    private ColorStateList black;
    
    private ColorStateList white;
    
    private HandlerThread mhandlerThread;
    
    private MyHandler childHandler;
    
    private int mulKey;
    
    private Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case Config.UPDATE_STRENGTH_QUALITY:
                    
                    Bundle bundle = (Bundle)msg.obj;
                    int sValue = bundle.getInt("strength");
                    int qValue = bundle.getInt("quality");
                    
                    pbstrength.setProgress(sValue);
                    pbstrength.invalidate();
                    
                    pbquality.setProgress(qValue);
                    pbquality.invalidate();
                    
                    tvStrength.setText(sValue + "%");
                    tvQuality.setText(qValue + "%");
                    break;
            }
        }
    };
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.channel_search);
        
    }
    
    protected void onStart()
    {
        super.onStart();
        initWidget();
        loadDrive();
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    private void loadDrive()
    {
        // set current time.
        MsgTimeUpdate msgTime = new MsgTimeUpdate(this);
        new Nativetime().TimeUpdateInit(msgTime);
        
        mhandlerThread = new HandlerThread("sub_thread");
        mhandlerThread.start();
        childHandler = new MyHandler(mhandlerThread.getLooper());
        
        getStrengthQuality();
    }
    
    private void initWidget()
    {
        // Get the color in the resources.
        Resources resources = getBaseContext().getResources();
        black = resources.getColorStateList(R.drawable.black);
        white = resources.getColorStateList(R.drawable.white);
        
        etInputFrequency = (EditText)this.findViewById(R.id.et_input_frequency);
        etSymbolRate = (EditText)this.findViewById(R.id.et_symbol_rate);
        etInputFrequency.setOnFocusChangeListener(new InputFreqFocusChangeListener());
        etSymbolRate.setOnFocusChangeListener(new InputQamFocusChangeListener());
        
        etInputFrequency.addTextChangedListener(new TextChangeListener());
        etSymbolRate.addTextChangedListener(new TextChangeListener());
        
        etInputFrequency.setOnKeyListener(new FrequencyOnKeyListener());
        etSymbolRate.setOnKeyListener(new SymbolrateOnKeyListener());
        
        btSearch = (Button)this.findViewById(R.id.bt_search_now);
        btSearch.setOnFocusChangeListener(new SearchOnFocusChangeListener());
        btSearch.setOnClickListener(new SearchOnClickListener());
        
        pbstrength = (ProgressBar)this.findViewById(R.id.pb_search_strength);
        pbquality = (ProgressBar)this.findViewById(R.id.pb_search_quality);
        pbstrength.setMax(100);
        pbquality.setMax(100);
        
        tvStrength = (TextView)this.findViewById(R.id.tv_search_signal_strength);
        tvQuality = (TextView)this.findViewById(R.id.tv_search_signal_quality);
        
        // Ban popup the system keybord.
        etInputFrequency.setInputType(InputType.TYPE_NULL);
        etSymbolRate.setInputType(InputType.TYPE_NULL);
        
        // Custom dropdown list.
        tvQAM = (TextView)this.findViewById(R.id.bt_qam);
        lvQAM = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        qamArray = new String[] {"16 QAM", "32 QAM", "64 QAM", "128 QAM", "256 QAM"};
        tvQAM.setFocusable(true);
        tvQAM.setClickable(true);
        tvQAM.setOnFocusChangeListener(new QamOnFocusChangeListener());
        tvQAM.setOnClickListener(new QamPopWindowOnClickListener());
        qamAdapter = new QamAdapter(TVChannelSearch.this, qamArray);
        lvQAM.setAdapter(qamAdapter);
        
        // Get rid of the system divider.
        lvQAM.setDivider(null);
        lvQAM.setOnItemClickListener(new QamPopWindowOnItemClickListener());
        lvQAM.setOnItemSelectedListener(new QamOnItemSelectedListener());
        
        tvScan = (TextView)this.findViewById(R.id.bt_scan);
        lvScan = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        scanArray = new String[] {getString(R.string.ManualScan), getString(R.string.AutoScan)};
//        tvScan.setFocusable(true);
//        tvScan.setClickable(true);
        tvScan.setOnFocusChangeListener(new ScanOnFocusChangeListener());
//        tvScan.setOnClickListener(new ScanPopWindowOnClickListener());
//        scanAdapter = new QamAdapter(TVChannelSearch.this, scanArray);
//        lvScan.setAdapter(scanAdapter);
//        lvScan.setDivider(null);
//        lvScan.setOnItemClickListener(new ScanOnItemClickListener());
//        lvScan.setOnItemSelectedListener(new ScanOnItemSelectedListener());
        
    }
    
    /**
     * 
     * Get the signal strength and quality.
     * 
     */
    private void getStrengthQuality()
    {
        strInputFrequency = etInputFrequency.getText().toString();
        strSymbolRate = etSymbolRate.getText().toString();
        strQam = tvQAM.getText().toString();
        
        Bundle mBundle = new Bundle();
        mBundle.putString("InputFrequency", strInputFrequency);
        mBundle.putString("SymbolRate", strSymbolRate);
        mBundle.putString("QAM", strQam);
        
        Message msg = new Message();
        msg.what = Config.NOTIFICATION_CHILD_THREAD;
        msg.obj = mBundle;
        childHandler.sendMessage(msg);
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0)
        {
            finish();
        }
        return false;
    }
    
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        int keyCode = event.getKeyCode();
        if (keyCode == Config.KEY_RED && event.getAction() == KeyEvent.ACTION_UP)
        {
            if (mulKey == 0)
            {
                mulKey++;
            }
            else
            {
                mulKey = 0;
            }
            return true;
        }
        if (keyCode == Config.KEY_GREEN && event.getAction() == KeyEvent.ACTION_UP)
        {
            if (mulKey == 1)
            {
                mulKey++;
            }
            else
            {
                mulKey = 0;
            }
            return true;
        }
        if (keyCode == Config.KEY_YELLOW && event.getAction() == KeyEvent.ACTION_UP)
        {
            if (mulKey == 2)
            {
                mulKey++;
            }
            else
            {
                mulKey = 0;
            }
            return true;
        }
        if (keyCode == Config.KEY_BLUE && event.getAction() == KeyEvent.ACTION_UP)
        {
            if (mulKey == 3)
            {
//                mulKey++;
                CommonUtils.skipActivity(this, DVBConfigActivity.class, -1);
            }
            else
            {
                mulKey = 0;
            }
            return true;
        }
        if (keyCode != KeyEvent.KEYCODE_DPAD_CENTER && event.getAction() == KeyEvent.ACTION_UP)
        {
            mulKey = 0;
        }
        return super.dispatchKeyEvent(event);
    }
    
    /**
     * 
     * <Functional overview>
     * <Functional Details>
     * @return 1 manual search,0 auto search
     * @see [Class, Class#Method, Class#Member]
     */
    private int searsearchModeParser()
    {
        strScan = tvScan.getText().toString();
        Log.i(TAG, "-----------Search Mode is -------->>>" + strScan);
        if (null != strScan && !"".equals(strScan))
        {
            if (strScan.equals(getString(R.string.ManualScan)))
            {
                return Config.Install_Search_Type_eTP_SEARCH;
            }
        }
        return Config.Install_Search_Type_eAUTO_SEARCH;
    }
    
    /**
     * 
     * <Init the custom dropdown list.>
     * 
     * @see [�ࡢ��#��������#��Ա]
     */
    private void initPop()
    {
        int width = tvQAM.getWidth();
        int height = LayoutParams.WRAP_CONTENT;
        
        popQAM = new PopupWindow(lvQAM, width, height, true);
        popScan = new PopupWindow(lvScan, width, height, true);
        
        // Click in other regions of the pop-up window disappears.
        popQAM.setBackgroundDrawable(new ColorDrawable(0xA0838383));
        popScan.setBackgroundDrawable(new ColorDrawable(0xA0838383));
    }
    
    public void skipNextActivity(int searchMode)
    {
        strInputFrequency = etInputFrequency.getText().toString();
        strSymbolRate = etSymbolRate.getText().toString();
        strQam = tvQAM.getText().toString();
        
        Bundle bundle = new Bundle();
        bundle.putInt("SearchMode", searchMode);
        bundle.putString("InputFrequency", strInputFrequency);
        bundle.putString("SymbolRate", strSymbolRate);
        bundle.putString("QAM", strQam);
        bundle.putBoolean("HasBat", false);
        CommonUtils.skipActivity(this, ChannelSearchProgress.class, bundle, Intent.FLAG_ACTIVITY_CLEAR_TOP);
    }
    
    class QamPopWindowOnClickListener implements OnClickListener
    {
        public void onClick(View v)
        {
            if (null == popQAM)
            {
                initPop();
            }
            if (!popQAM.isShowing())
            {
                popQAM.showAsDropDown(tvQAM, 0, -5);
            }
            else
            {
                popQAM.dismiss();
            }
        }
    }
    
    class ScanPopWindowOnClickListener implements OnClickListener
    {
        public void onClick(View v)
        {
            if (null == popScan)
            {
                initPop();
            }
            if (!popScan.isShowing())
            {
                popScan.showAsDropDown(tvScan, 0, -5);
            }
            else
            {
                popScan.dismiss();
            }
        }
    }
    
    class QamOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                tvQAM.setBackgroundResource(R.drawable.channel_play_bg);
                tvQAM.setTextColor(white);
            }
            else
            {
                tvQAM.setBackgroundResource(R.drawable.qam_normal);
                tvQAM.setTextColor(black);
            }
        }
    }
    
    class ScanOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                tvScan.setBackgroundResource(R.drawable.qam_focused);
                tvScan.setTextColor(white);
            }
            else
            {
                tvScan.setBackgroundResource(R.drawable.qam_normal);
                tvScan.setTextColor(black);
            }
        }
    }
    
    class QamPopWindowOnItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            String qamStr = qamArray[position];
            tvQAM.setText(qamStr);
            popQAM.dismiss();
            etSymbolRate.setFocusable(true);
            
            tvQAM.setBackgroundResource(R.drawable.channel_play_bg);
            tvQAM.setTextColor(white);
            
            strInputFrequency = etInputFrequency.getText().toString();
            strSymbolRate = etSymbolRate.getText().toString();
            strQam = tvQAM.getText().toString();
            
            Bundle bundle = new Bundle();
            bundle.putString("InputFrequency", strInputFrequency);
            bundle.putString("SymbolRate", strSymbolRate);
            bundle.putString("QAM", strQam);
            
            Log.i(TAG, "--Send Message to Child Thread!!!-->>" + strInputFrequency + "-----" + strSymbolRate
                + "-------" + strQam);
            Message message = new Message();
            message.what = Config.NOTIFICATION_CHILD_THREAD;
            message.obj = bundle;
            childHandler.sendMessage(message);
            
        }
    }
    
    class QamOnItemSelectedListener implements OnItemSelectedListener
    {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            tvQAM.setBackgroundResource(R.drawable.qam_normal);
            tvQAM.setTextColor(black);
        }
        
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
        
    }
    
    class ScanOnItemSelectedListener implements OnItemSelectedListener
    {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            tvScan.setBackgroundResource(R.drawable.qam_normal);
            tvScan.setTextColor(black);
        }
        
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
        
    }
    
    class ScanOnItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            String scanStr = scanArray[position];
            tvScan.setText(scanStr);
            popScan.dismiss();
            
            tvScan.setBackgroundResource(R.drawable.qam_focused);
            tvScan.setTextColor(white);
        }
    }
    
    class SearchOnClickListener implements OnClickListener
    {
        public void onClick(View v)
        {
            int searchMode = searsearchModeParser();
            skipNextActivity(searchMode);
        }
    }
    
    class SearchOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                btSearch.setBackgroundResource(R.drawable.channel_play_bg);
                btSearch.setTextColor(white);
            }
            else
            {
                btSearch.setBackgroundResource(R.drawable.button_search);
                btSearch.setTextColor(black);
            }
        }
    }
    
    class InputFreqFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                etInputFrequency.setTextColor(white);
            }
            else
            {
                etInputFrequency.setTextColor(black);
            }
        }
    }
    
    class InputQamFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                etSymbolRate.setTextColor(white);
            }
            else
            {
                etSymbolRate.setTextColor(black);
            }
        }
    }
    
    class FrequencyOnKeyListener implements OnKeyListener
    {
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
            {
                Log.i(TAG, "---------------DEL has clicked!!-----------------");
                etInputFrequency.setText(null);
            }
            return false;
        }
    }
    
    class SymbolrateOnKeyListener implements OnKeyListener
    {
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            if (keyCode == Config.DELETE || keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
            {
                etSymbolRate.setText(null);
            }
            return false;
        }
    }
    
    class TextChangeListener implements TextWatcher
    {
        public void beforeTextChanged(CharSequence s, int start, int count, int after)
        {
        }
        
        public void onTextChanged(CharSequence s, int start, int before, int count)
        {
        }
        
        public void afterTextChanged(Editable s)
        {
            if (s.length() >= 4)
            {
                strInputFrequency = etInputFrequency.getText().toString();
                strSymbolRate = etSymbolRate.getText().toString();
                strQam = tvQAM.getText().toString();
                
                Bundle bundle = new Bundle();
                bundle.putString("InputFrequency", strInputFrequency);
                bundle.putString("SymbolRate", strSymbolRate);
                bundle.putString("QAM", strQam);
                
                Log.i(TAG, "--Send Message to Child Thread!!!-->>" + strInputFrequency + "-----" + strSymbolRate
                    + "-------" + strQam);
                Message message = new Message();
                message.what = Config.NOTIFICATION_CHILD_THREAD;
                message.obj = bundle;
                childHandler.sendMessage(message);
            }
            
            

        }
        
    }
    
    class MyHandler extends Handler
    {
        public MyHandler()
        {
            super();
        }
        
        public MyHandler(Looper looper)
        {
            super(looper);
        }
        
        public void handleMessage(Message msg)
        {
            if (msg.what == Config.NOTIFICATION_CHILD_THREAD)
            {
                Bundle cbundle = (Bundle)msg.obj;
                strInputFrequency = cbundle.getString("InputFrequency");
                strSymbolRate = cbundle.getString("SymbolRate");
                strQam = cbundle.getString("QAM");
                
                if (strInputFrequency.length() >= 5 && strSymbolRate.length() == 4)
                {
                    Log.i(TAG, "--Frequency: --->>>" + strInputFrequency);
                    Log.i(TAG, "--SymbolRate: --->>>" + strSymbolRate);
                    Log.i(TAG, "--Qam: --->>>" + strQam);
                    
                    Tuner tuner = new Tuner();
                    CablePercent cable = tuner.new CablePercent();
                    DVBCore_Cab_Desc_t dvbCoreCabDesc = tuner.new DVBCore_Cab_Desc_t();
                    
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
                    
                    dvbCoreCabDesc.setU32Freq(u32Freq);
                    dvbCoreCabDesc.setU32SymbRate(u32SymbRate);
                    
                    int eMod = CommonUtils.getModHandling(strQam);
                    
                    dvbCoreCabDesc.seteMod(eMod);
                    dvbCoreCabDesc.seteFEC_Inner(0);
                    dvbCoreCabDesc.seteFEC_Outer(0);
                    dvbCoreCabDesc.setU8TunerId((char)0);
                    
                    tuner.getTunerCablePercent(dvbCoreCabDesc, cable);
                    
                    int qualityValue = cable.getQuality();
                    int strengthValue = cable.getStrength();
                    
                    Bundle bundle = new Bundle();
                    bundle.putInt("quality", qualityValue);
                    bundle.putInt("strength", strengthValue);
                    
                    Log.i(TAG, "---------Signal Quality --->>>" + qualityValue);
                    Log.i(TAG, "---------Signal Strength --->>>" + strengthValue);
                    
                    Message message = new Message();
                    message.obj = bundle;
                    message.what = Config.UPDATE_STRENGTH_QUALITY;
                    handler.sendMessage(message);
                    
                }
            }
        }
    }
    
}
