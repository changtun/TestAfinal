package com.pbi.dvb.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DBopenHelper extends SQLiteOpenHelper
{
    private static final String DATABASE_NAME = "db_dvb_data.dat";
    
    private static final int DATABASE_VERSION = 1;
    
    public DBopenHelper(Context context)
    {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }
    
    public void onCreate(SQLiteDatabase db)
    {
        String createTpTable ="create table " 
                + DBTp.TABLE_TP_NAME + " (" 
                + DBTp.TP_ID + " integer primary key autoincrement,"
                + DBTp.TUNER_TYPE + "," 
                + DBTp.TUNER_ID + ","
                + DBTp.NET_ID + ","
                + DBTp.ORIGINAL_NET_ID + ","
                + DBTp.TS_ID + "," 
                + DBTp.FREQUENCY + ","
                + DBTp.SYMB_RATE + ","
                + DBTp.EMOD + "," 
                + DBTp.EFEC_INNER+ ","
                + DBTp.EFEC_OUTER + ");";
        
        String createServiceTable ="create table " 
                + DBService.TABLE_SERVICE_NAME + " ("
                + DBService.CHANNEL_NUMBER+ " integer primary key autoincrement," 
                + DBService.LOGICAL_NUMBER + "," 
                + DBService.TP_ID + ","
                + DBService.TUNER_TYPE + ","
                + DBService.SERVICE_ID + ","
                + DBService.SERVICE_TYPE + ","
                + DBService.REF_SERVICE_ID + "," 
                + DBService.PME_ID + ","
                + DBService.CA_MODE + ","
                + DBService.CHANNEL_NAME + "," 
                + DBService.VOLUME + "," 
                + DBService.VIDEO_TYPE + ","
                + DBService.VIDEO_PID + ","
                + DBService.AUDIO_MODE + "," 
                + DBService.AUDIO_INDEX + ","
                + DBService.AUDIO_TYPE + ","
                + DBService.AUDIO_PID + "," 
                + DBService.AUDIO_LANG + ","
                + DBService.PCR_PID + "," 
                + DBService.FAV_FLAG + "," 
                + DBService.LOCK_FLAG + ","
                + DBService.MOVE_FLAG+ "," 
                + DBService.DEL_FLAG + "," 
                + DBService.CHANNEL_POSITION + ");";
        
        String createMailTable ="create table " 
                + DBMail.TABLE_MAIL_NAME + " (" 
                + DBMail.MAIL_ID + " integer primary key autoincrement,"
                + DBMail.MAIL_INDEX + "," 
                + DBMail.MAIL_TYPE + "," 
                + DBMail.MAIL_STATUS + "," 
                + DBMail.MAIL_CLASS + ","
                + DBMail.MAIL_PRIORITY + "," 
                + DBMail.MAIL_PERIOD + "," 
                + DBMail.MAIL_FROM + "," 
                + DBMail.MAIL_TITLE + ","
                + DBMail.MAIL_POST_TIME + "," 
                + DBMail.MAIL_CONTENT + ");";
        
        String createPvrTable ="create table " 
                + DBPvr.TABLE_PVR_NAME + " (" 
                + DBPvr.PVR_ID + " integer primary key autoincrement,"
                + DBPvr.PVR_MODE + "," 
                + DBPvr.PVR_WAKEUP_MODE + "," 
                + DBPvr.PVR_SERVICE_ID + ","
                + DBPvr.PVR_LOGICAL_NUMBER + "," 
                + DBPvr.PVR_SET_DATE + "," 
                + DBPvr.PVR_START_TIME + ","
                + DBPvr.PVR_WEEK_DATE + "," 
                + DBPvr.PVR_RECORD_LENGTH + "," 
                + DBPvr.PVR_PMT_PID + ","
                + DBPvr.PVR_IS_SLEEP + "," 
                + DBPvr.PVR_TYPE_TAG + ");";
        
        db.execSQL(createTpTable);
        db.execSQL(createServiceTable);
        db.execSQL(createMailTable);
        db.execSQL(createPvrTable);
    }
    
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion)
    {
        db.execSQL("drop table if exists " + DBService.TABLE_SERVICE_NAME + ";");
        db.execSQL("drop table if exists " + DBTp.TABLE_TP_NAME + ";");
        db.execSQL("drop table if exists " + DBMail.TABLE_MAIL_NAME + ";");
        db.execSQL("drop table if exists " + DBPvr.TABLE_PVR_NAME + ";");
        
        onCreate(db);
    }
    
}
