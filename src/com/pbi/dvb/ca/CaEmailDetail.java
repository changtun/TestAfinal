package com.pbi.dvb.ca;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.TextView;

import com.pbi.dvb.DVBBaseActivity;
import com.pbi.dvb.EpgActivity;
import com.pbi.dvb.R;
import com.pbi.dvb.dao.MailDao;
import com.pbi.dvb.dao.impl.MailDaoImpl;
import com.pbi.dvb.domain.MailBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;

public class CaEmailDetail extends DVBBaseActivity
{
    private TextView emailTile;
    
    private TextView emailTime;
    
    private TextView emailContent;
    
    private MailDao mailDao;
    
    private int emailId;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ca_email_detail);
        
        emailTile = (TextView)findViewById(R.id.ca_email_detail_title);
        emailTime = (TextView)findViewById(R.id.ca_email_detail_time);
        emailContent = (TextView)findViewById(R.id.ca_email_detail_content);
        emailContent.setFocusable(true);
        
        mailDao = new MailDaoImpl(CaEmailDetail.this);
        Intent intent = getIntent();
        emailId = intent.getExtras().getInt("emailId");
        Log.i("CaEmailDetail", "------------------ CaEmailDetail , emailId = " + emailId);
        queryDetailInfosByID();
        
    }
    
    private void queryDetailInfosByID()
    {
        MailBean bean = mailDao.getMailInfoById(emailId);
        if (bean != null)
        {
            emailTile.setText(bean.getMailTitle());
            emailTime.setText(bean.getMailTime());
            emailContent.setText(bean.getMailContent());
        }
        
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {// 返回键
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
    
}
