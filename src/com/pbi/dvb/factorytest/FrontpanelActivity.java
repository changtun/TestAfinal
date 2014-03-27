package com.pbi.dvb.factorytest;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.widget.TextView;

import com.pbi.dvb.R;

public class FrontpanelActivity extends Activity
{
    TextView text_menu, text_ok, text_up, text_down, text_left, text_right, text_yes, text_no;
    
    boolean left_first, right_first, ok_first, menu_first;
    
    boolean is_yes;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.frontpanel_dialog);
        
        text_down = (TextView)findViewById(R.id.dialog_down);
        text_up = (TextView)findViewById(R.id.dialog_up);
        text_menu = (TextView)findViewById(R.id.dialog_menu);
        text_ok = (TextView)findViewById(R.id.dialog_ok);
        text_left = (TextView)findViewById(R.id.dialog_left);
        text_right = (TextView)findViewById(R.id.dialog_right);
        text_yes = (TextView)findViewById(R.id.dialog_yes);
        text_no = (TextView)findViewById(R.id.dialog_no);
        left_first = false;
        right_first = false;
        ok_first = false;
        is_yes = true;
        menu_first = false;
        text_yes.setBackgroundColor(Color.YELLOW);
        FactoryTest.is_frontpanel = true;
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        switch (keyCode)
        {
            case KeyEvent.KEYCODE_DPAD_UP:
                text_up.setBackgroundColor(Color.GREEN);
                break;
            case KeyEvent.KEYCODE_DPAD_DOWN:
                text_down.setBackgroundColor(Color.GREEN);
                
                break;
            case KeyEvent.KEYCODE_DPAD_LEFT:
                if (!left_first)
                {
                    left_first = true;
                    text_left.setBackgroundColor(Color.GREEN);
                }
                else
                {
                    if (is_yes)
                    {
                        is_yes = false;
                        text_no.setBackgroundColor(Color.YELLOW);
                        text_yes.setBackgroundColor(Color.GRAY);
                    }
                    else
                    {
                        is_yes = true;
                        text_yes.setBackgroundColor(Color.YELLOW);
                        text_no.setBackgroundColor(Color.GRAY);
                    }
                }
                
                break;
            case KeyEvent.KEYCODE_DPAD_RIGHT:
                if (!right_first)
                {
                    right_first = true;
                    text_right.setBackgroundColor(Color.GREEN);
                }
                else
                {
                    if (is_yes)
                    {
                        is_yes = false;
                        text_no.setBackgroundColor(Color.YELLOW);
                        text_yes.setBackgroundColor(Color.GRAY);
                    }
                    else
                    {
                        is_yes = true;
                        text_yes.setBackgroundColor(Color.YELLOW);
                        text_no.setBackgroundColor(Color.GRAY);
                    }
                }
                break;
            case KeyEvent.KEYCODE_DPAD_CENTER:
                if (!ok_first)
                {
                    ok_first = true;
                    text_ok.setBackgroundColor(Color.GREEN);
                }
                else
                {
                    if (is_yes)
                    {
                        FactoryTest.is_success = true;
                    }
                    else
                    {
                        FactoryTest.is_success = false;
                    }
                    finish();
                    Log.e("TAG", "finish()=======================");
                }
                
                break;
            case 4:
                if (!menu_first)
                {
                    menu_first = true;
                    text_menu.setBackgroundColor(Color.GREEN);
                    return true;
                }
                else
                {
                    finish();
                    // int currentVersion = android.os.Build.VERSION.SDK_INT;
                    // if (currentVersion > android.os.Build.VERSION_CODES.ECLAIR_MR1) {
                    // Intent startMain = new Intent(Intent.ACTION_MAIN);
                    // startMain.addCategory(Intent.CATEGORY_HOME);
                    // startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    // startActivity(startMain);
                    // System.exit(0);
                    // } else {// android2.1
                    // ActivityManager am = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
                    // am.restartPackage(getPackageName());
                    // }
                    
                }
                
                break;
            default:
                break;
        }
        return super.onKeyDown(keyCode, event);
    }
    
    @Override
    protected void onResume()
    {
        text_down.setBackgroundColor(Color.GRAY);
        text_up.setBackgroundColor(Color.GRAY);
        text_left.setBackgroundColor(Color.GRAY);
        text_right.setBackgroundColor(Color.GRAY);
        text_ok.setBackgroundColor(Color.GRAY);
        text_menu.setBackgroundColor(Color.GRAY);
        text_no.setBackgroundColor(Color.GRAY);
        text_yes.setBackgroundColor(Color.YELLOW);
        super.onResume();
    }
}
