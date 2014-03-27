package com.pbi.dvb.db;

import android.net.Uri;
import android.provider.BaseColumns;

public final class DBService implements BaseColumns
{
    //Table name
    public static final String TABLE_SERVICE_NAME = "db_service_info_table";

    //Authority
    public static final String SERVICE_AUTHORITY="com.pbi.dvb.provider.ServiceProvider";
    
    private DBService(){}
    
    //Identification Uri
    public static final Uri CONTENT_URI =Uri.parse("content://"+SERVICE_AUTHORITY+"/"+TABLE_SERVICE_NAME);
    
    public static final int ITEM = 1;
    public static final int ITEM_ID = 2;
    
    //Type
    public static final String CONTENT_TYPE ="vnd.android.cursor.dir/vnd.dvb.service";
    public static final String CONTENT_ITEM_TYPE="vnd.android.cursor.item/vnd.dvb.service";
    
    //Include Table Const Column
    public static final String CHANNEL_NUMBER = "channel_number";
    public static final String LOGICAL_NUMBER = "logical_number";
    public static final String TP_ID = "tp_id";
    public static final String TUNER_TYPE = "tuner_type";
    public static final String SERVICE_ID = "service_id";
    public static final String SERVICE_TYPE = "service_type";
    public static final String REF_SERVICE_ID = "ref_service_id";
    public static final String PME_ID = "pmt_pid";
    public static final String CA_MODE = "ca_mode";
    public static final String CHANNEL_NAME = "channel_name";
    public static final String VOLUME = "volume";
    public static final String VIDEO_TYPE = "video_type";
    public static final String VIDEO_PID = "video_pid";
    public static final String AUDIO_MODE = "audio_mode";
    public static final String AUDIO_INDEX = "audio_index";
    public static final String AUDIO_TYPE = "audio_type";
    public static final String AUDIO_PID = "audio_pid";
    public static final String AUDIO_LANG = "audio_language";
    public static final String PCR_PID = "pcr_pid";
    public static final String FAV_FLAG = "fav_flag";
    public static final String LOCK_FLAG = "lock_flag";
    public static final String MOVE_FLAG = "move_flag";
    public static final String DEL_FLAG = "del_flag";
    public static final String CHANNEL_POSITION = "channel_position";
    
}
