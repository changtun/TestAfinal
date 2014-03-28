/*
 * File Name:  DVBApp.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  
 * Changed Time:  2014-3-28
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;


import net.tsz.afinal.FinalDb;
import android.app.Application;

/**
 * <Functional overview> <Functional Details>
 * 
 * @author
 * @version [Version Number, 2014-3-28]
 * @see [Relevant Class/Method]
 * @since [Product/Module Version]
 */
public class DVBApp extends Application
{
    private static final String DB_NAME = "db_dvb_data.db";
    
    private FinalDb db;
    
    @Override
    public void onCreate()
    {
        super.onCreate();
        if (null == db)
        {
            db = FinalDb.create(this, DB_NAME);
        }
    }
    
    public FinalDb getDb()
    {
        if (null == db)
        {
            db = FinalDb.create(this, DB_NAME);
        }
        return db;
    }
    
}
