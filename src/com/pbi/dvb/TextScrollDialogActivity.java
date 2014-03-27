package com.pbi.dvb;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.TextView;

public class TextScrollDialogActivity extends Activity
{
    
    private TextView tv_ScrollText;
    
    private String scrollText = null;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        Intent intent = getIntent();
        
        if (null != intent)
        {
            scrollText = intent.getStringExtra("extentInfo");
        }
        
        setContentView(R.layout.text_scroll_dialog);
        
        init_View();
    }
    
    private void init_View()
    {
        tv_ScrollText = (TextView)findViewById(R.id.tv_ScrollText);
        
        tv_ScrollText.setText((null == scrollText ? "" : scrollText));
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
        {
            finish();
        }
        
        return super.onKeyDown(keyCode, event);
    }
    
}
