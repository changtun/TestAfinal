package com.pbi.dvb.ca;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caParental;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.LogUtils;

public class CaRateControl extends BaseActivity
{
    private static final int CA_RATE_COMMON = 1;
    
    private static final int CA_RATE_PROTECT = 2;
    
    private static final int CA_RATE_AID = 3;
    
    private static final int CA_RATE_LIMIT = 4;
    
    private static final int CA_RATE_SPECIAL = 5;
    
    private String TAG = Config.DVB_TAG;
    
    private int pwdlength = 4;
    
    private Handler caHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Log.i(TAG, "CaRateControlNew!  msg.what = " + msg.what);
            
            switch (msg.what)
            {
                case Config.CA_MSG_CARD_Status:
                    int status = msg.arg1;
                    if (status == 1)
                    {
                        showErrorInfo();
                    }
                    else
                    {
                        showRateSetView();
                    }
                    break;
                
                case Config.CA_PARENTAL_REPLY:
                    Log.i(TAG, "CaRateControlNew!  msg.arg1 = " + msg.arg1);
                    
                    if (msg.arg1 == 0)
                    {
                        showToast(getResources().getString(R.string.rate_success));
                    }
                    else
                    {
                        showToast(getResources().getString(R.string.rate_fail));
                    }
                    break;
            
            }
        }
    };
    
    private LinearLayout rateSetLL;
    
    private LinearLayout ll_rate_choice;
    
    private EditText et_rate_password;
    
    private Button bt_save_rate_modify;
    
    private TextView tv_ShowCALevel;
    
    private String[] ca_rates;
    
    private int currentCaRate = CA_RATE_COMMON;
    
    private void showErrorInfo()
    {
        errorTV.setVisibility(View.VISIBLE);
        rateSetLL.setVisibility(View.GONE);
    }
    
    private void showRateSetView()
    {
        rateSetLL.setVisibility(View.VISIBLE);
        errorTV.setVisibility(View.GONE);
    }
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ca_rate_control_new);
        
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        
        ca_rates = getResources().getStringArray(R.array.ca_rate_level);
        
        rateSetLL = (LinearLayout)findViewById(R.id.rate_set_ll);
        
        errorTV = (TextView)findViewById(R.id.error_tv);
        
        ll_rate_choice = (LinearLayout)findViewById(R.id.ll_rate_choice);
        
        tv_ShowCALevel = (TextView)findViewById(R.id.tv_ShowCALevel);
        
        changeCaRate(CA_RATE_COMMON);
        
        et_rate_password = (EditText)findViewById(R.id.et_rate_password);
        bt_save_rate_modify = (Button)findViewById(R.id.bt_save_rate_modify);
        
        messageCa.setHandler(caHandler, this);
        nativeCA.CaMessageInit(messageCa);
        
        bt_save_rate_modify.setOnClickListener(new OnClickListener()
        {
            public void onClick(View v)
            {
                if (rateSetLL.getVisibility() == View.VISIBLE)
                {
                    Log.i(TAG, "rateSetLL Visibility = visibility");
                    savePwModify();
                }
                else
                {
                    Log.i(TAG, "rateSetLL Visibility = gone");
                    CaRateControl.this.finish();
                }
            }
        });
        
        if (cardExist)
        {
            showRateSetView();
        }
        else
        {
            showErrorInfo();
        }
    }
    
    private void savePwModify()
    {
        String rate = tv_ShowCALevel.getText().toString();
        
        String password = et_rate_password.getText().toString();
        
        int ca_rate_level = -1;
        
        for (int i = 0; null != ca_rates && i < ca_rates.length; i++)
        {
            if (rate.equals(ca_rates[i]))
            {
                ca_rate_level = i;
                
                break;
            }
        }
        
        ca_rate_level++;
        
        if ("".equals(password) || password == null || password.length() != pwdlength)
        {
            showToast(getResources().getString(R.string.password_message));
            return;
        }
        
        Log.i(TAG, "CaRateControlNew! passwrod = " + password);
        Log.i(TAG, "CaRateControlNew! rate = " + ca_rate_level);
        
        LogUtils.e(TAG, "ca.CaRateControlNew.savePwModify:: set ca_rate_level = " + ca_rate_level + " password = "
            + password);
        
        caParental caparent = nativeCA.new caParental(Integer.valueOf(password), ca_rate_level); // ??
        nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_SET_PARENTAL_QUERY, caparent, 0);
        
        Log.i(TAG, "CaRateControlNew!  SendMessageToCA.. set rate control ");
    }
    
    private void changeCaRate(int ca_rate)
    {
        tv_ShowCALevel.setText(ca_rates[ca_rate - 1]);
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        LogUtils.e(TAG, "ca.CaRateControlNew.onKeyDown:: keyCode = " + keyCode);
        
        super.onKeyDown(keyCode, event);
        
        if (ll_rate_choice.hasFocus())
        {
            switch (keyCode)
            {
                case KeyEvent.KEYCODE_DPAD_LEFT:
                {
                    
                    if (currentCaRate > 1)
                    {
                        currentCaRate--;
                    }
                    
                    changeCaRate(currentCaRate);
                    
                    return true;
                }
                
                case KeyEvent.KEYCODE_DPAD_RIGHT:
                {
                    if (ll_rate_choice.hasFocus())
                    {
                        if (currentCaRate < 5)
                        {
                            currentCaRate++;
                        }
                        
                        changeCaRate(currentCaRate);
                    }
                    
                    return true;
                }
            }
        }
        else
        {
            LogUtils.e(TAG, "ca.CaRateControlNew.onKeyDown:: ll_rate_choice has no focus...");
        }
        
        return false;
    }
}
