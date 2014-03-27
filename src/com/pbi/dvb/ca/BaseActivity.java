package com.pbi.dvb.ca;

import android.app.FragmentTransaction;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.DVBBaseActivity;
import com.pbi.dvb.EpgActivity;
import com.pbi.dvb.R;
import com.pbi.dvb.TVChannelSearch;
import com.pbi.dvb.dvbinterface.MessageCa;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;

public class BaseActivity extends DVBBaseActivity implements PwdDialogListener
{
    
    protected String TAG = "BaseActivity";
    
    public static String PWD_DIALOG_TAG = "PWD_DIALOG_TAG";
    
    protected static final int request_set = 1;
    
    protected static final int request_query = 2;
    
    protected static final int request_cancel = 3;
    
    protected ColorStateList black;
    
    protected ColorStateList white;
    
    protected boolean cardExist = false;
    
    protected NativeCA nativeCA = new NativeCA();
    
    protected MessageCa messageCa = new MessageCa();
    
    protected TextView errorTV;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        nativeCA.CaMessageInit(messageCa);
        
        cardExist = isCardExist();
        Log.i(TAG, "BaseActivity!. cardExist = " + cardExist);
        
        Resources resources = getBaseContext().getResources();
        black = resources.getColorStateList(R.drawable.black);
        white = resources.getColorStateList(R.drawable.white);
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    protected boolean isCardExist()
    {
        int result = nativeCA.CaGetScStatus();// 卡是否存在
        if (result == 1)
        {
            return true;
        }
        return false;
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            this.finish();
        }
        else if (keyCode == Config.KEY_EPG && event.getRepeatCount() == 0)
        {
            String superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
            if (null == superPwd)
            {
                superPwd = Config.PROGRAMME_LOCK;
            }
            CommonUtils.skipActivity(this, EpgActivity.class, "superPwd", superPwd, -1);
        }
//        else if (keyCode == Config.KEY_SEARCH && event.getRepeatCount() == 0)
//        {
//            CommonUtils.skipActivity(this, TVChannelSearch.class, -1);
//        }
        return false;
    }
    
    protected void showToast(String message)
    {
        Log.i(TAG, "BaseActivity!. showToast() , message = " + message);
        
        Toast toast = Toast.makeText(getApplicationContext(), message, 0);
        toast.setGravity(Gravity.CENTER, 0, 0);
        toast.show();
    }
    
    protected void showPinPwdDialog()
    {
        Log.i(TAG, "BaseActivity!. showPinPwdDialog() = ");
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        PwdDialogFrag pdf = PwdDialogFrag.newInstance();
        pdf.show(ft, PWD_DIALOG_TAG);
    }
    
    @Override
    public void onDialogDone(String tag, boolean confirm, String message)
    {
        Log.i(TAG, "BaseActivity!. onDialogDone(), message = " + message);
    }
    
    public static String fillZero(int num)
    {
        return String.valueOf(num).length() == 2 ? String.valueOf(num) : "0" + num;
    }
    
}
