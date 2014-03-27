/*
 * 文 件 名:  DVBConfigActivity.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd.Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  
 * 修改时间:  2013-6-3
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb;

import android.app.Activity;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.pbi.dvb.domain.ConfigBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author 姓名 工号
 * @version [版本号, 2013-6-3]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class DVBConfigActivity extends Activity
{
    private static final String TAG = "DVBConfigActivity";
    
    private ConfigBean configBean;
    
    private RadioGroup rgTunerid;
    
    private RadioButton rbTuner1, rbTuner2;
    
    private RadioGroup rgBandwidth;
    
    private RadioButton rbBandwidth6, rbBandwidth8, rbBandwidthOther;
    
    private RadioGroup rgBat;
    
    private RadioButton rbBatLimit, rbBatNoLimit;
    
    private RadioGroup rgEncode;
    
    private RadioButton rbGbk, rbBig5, rbUnicode, rbUtf8, rbOther;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dvb_config);
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
        
        initView();
        
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch (keyCode)
        {
            case Config.KEY_GREEN:
                save();
                break;
            case Config.KEY_RED:
                finish();
                break;
        }
        return super.onKeyDown(keyCode, event);
    }
    
    private void save()
    {
        int tunerid = 0;
        int bandwidth = 0;
        int bat = 0;
        int encode = 2;
        
        switch (rgTunerid.getCheckedRadioButtonId())
        {
            case R.id.rb_tuner_0:
                tunerid = 0;
                break;
            
            case R.id.rb_tuner_2:
                tunerid = 2;
                break;
        }
        
        switch (rgBandwidth.getCheckedRadioButtonId())
        {
            case R.id.rb_bandwidth_6m:
                bandwidth = 0;
                break;
            
            case R.id.rb_bandwidth_8m:
                bandwidth = 1;
                break;
            
            case R.id.rb_bandwidth_other:
                bandwidth = 1;
                break;
        }
        
        switch (rgBat.getCheckedRadioButtonId())
        {
            case R.id.rb_bat_limit:
                bat = 0;
                break;
            
            case R.id.rb_bat_no_limit:
                bat = 1;
                break;
        }
        
        switch (rgEncode.getCheckedRadioButtonId())
        {
            case R.id.rb_encode_gbk:
                encode = 0;
                break;
            case R.id.rb_encode_big5:
                encode = 1;
                break;
            case R.id.rb_encode_unicode:
                encode = 2;
                break;
            case R.id.rb_encode_utf8:
                encode = 3;
                break;
            case R.id.rb_encode_other:
                encode = 0;
                break;
        }
        
        configBean.setBandwidth(bandwidth);
        configBean.setBatLimit(bat);
        configBean.setEncodeType(encode);
        
        LogUtils.printLog(1, 3, TAG, "--->>> tunerid :: " + tunerid);
        LogUtils.printLog(1, 3, TAG, "--->>> bandwidth :: " + bandwidth);
        LogUtils.printLog(1, 3, TAG, "--->>> bat :: " + bat);
        LogUtils.printLog(1, 3, TAG, "--->>> encode :: " + encode);
        configBean.setConfigParams(configBean);
        
        //save tuner id ;
        Editor edit = getSharedPreferences("double_tuner", 8).edit();
        edit.putInt("tunerid", tunerid);
        edit.commit();
        
        finish();
    }
    
    private void initView()
    {
        TextView tvVersion = (TextView)this.findViewById(R.id.tv_dvb_version);
        tvVersion.setText(getDVBVersion());
        
        rgTunerid = (RadioGroup)this.findViewById(R.id.rg_tunerid);
       rbTuner1 =  (RadioButton)this.findViewById(R.id.rb_tuner_0);
       rbTuner2 = (RadioButton)this.findViewById(R.id.rb_tuner_2);
        
        rgBandwidth = (RadioGroup)this.findViewById(R.id.rg_bandwidth);
        rbBandwidth6 = (RadioButton)this.findViewById(R.id.rb_bandwidth_6m);
        rbBandwidth8 = (RadioButton)this.findViewById(R.id.rb_bandwidth_8m);
        rbBandwidthOther = (RadioButton)this.findViewById(R.id.rb_bandwidth_other);
        
        rgBat = (RadioGroup)this.findViewById(R.id.rg_bat);
        rbBatLimit = (RadioButton)this.findViewById(R.id.rb_bat_limit);
        rbBatNoLimit = (RadioButton)this.findViewById(R.id.rb_bat_no_limit);
        
        rgEncode = (RadioGroup)this.findViewById(R.id.rg_encode);
        rbGbk = (RadioButton)this.findViewById(R.id.rb_encode_gbk);
        rbBig5 = (RadioButton)this.findViewById(R.id.rb_encode_big5);
        rbUnicode = (RadioButton)this.findViewById(R.id.rb_encode_unicode);
        rbUtf8 = (RadioButton)this.findViewById(R.id.rb_encode_utf8);
        rbOther = (RadioButton)this.findViewById(R.id.rb_encode_other);
        
        configBean = new ConfigBean();
        ConfigBean cBean = configBean.getConfigParams();
        if (null == cBean)
        {
            LogUtils.printLog(1, 5, TAG, "--->>> read dvb_config fail ---");
            rbBandwidth6.setChecked(true);
            rbBatLimit.setChecked(true);
            rbUnicode.setChecked(true);
        }
        else
        {
            // init bandwidth
            switch (cBean.getBandwidth())
            {
                case 0:
                    rbBandwidth6.setChecked(true);
                    break;
                case 1:
                    rbBandwidth8.setChecked(true);
                    break;
                default:
                    rbBandwidth8.setChecked(true);
                    break;
            }
            
            // init bat limit
            switch (cBean.getBatLimit())
            {
                case 0:
                    rbBatLimit.setChecked(true);
                    break;
                case 1:
                    rbBatNoLimit.setChecked(true);
                    break;
                default:
                    rbBatLimit.setChecked(true);
                    break;
            }
            
            // init encode type
            switch (cBean.getEncodeType())
            {
                case 0:
                    rbGbk.setChecked(true);
                    break;
                case 1:
                    rbBig5.setChecked(true);
                    break;
                case 2:
                    rbUnicode.setChecked(true);
                    break;
                case 3:
                    rbUtf8.setChecked(true);
                    break;
                default:
                    rbUnicode.setChecked(true);
                    break;
            }
        }
    }
    
    private String getDVBVersion()
    {
        PackageManager packageManager = this.getPackageManager();
        PackageInfo packInfo = null;
        try
        {
            packInfo = packageManager.getPackageInfo(getPackageName(), 0);
        }
        catch (NameNotFoundException e)
        {
            e.printStackTrace();
        }
        return packInfo.versionName;
    }
    
}
