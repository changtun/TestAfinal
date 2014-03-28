/*
 * File Name:  BaseActivity.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  
 * Changed Time:  2014-3-28
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import net.tsz.afinal.FinalDb;
import android.app.Activity;
import android.os.Bundle;


/**
 * <Functional overview> <Functional Details>
 * 
 * @author
 * @version [Version Number, 2014-3-28]
 * @see [Relevant Class/Method]
 * @since [Product/Module Version]
 */
public class BaseActivity extends Activity
{
    private DVBApp app;
    
    public FinalDb db;
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        app = (DVBApp)getApplicationContext();
        db = app.getDb();
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
    }
    
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
    
}
