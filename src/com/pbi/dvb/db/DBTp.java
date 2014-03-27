package com.pbi.dvb.db;

import android.net.Uri;
import android.provider.BaseColumns;

public final class DBTp implements BaseColumns
{
    public static final String TABLE_TP_NAME = "db_tp_info_table";
    
    public static final String  TP_AUTHORITY = "com.pbi.dvb.provider.TPProvider";
    
    private DBTp(){};
    
    public static final Uri CONTENT_URI = Uri.parse("content://"+TP_AUTHORITY+"/"+TABLE_TP_NAME);
    
    public static final int ITEM = 1;
    public static final int ITEM_ID = 2;
    
    public static final String CONTENT_TYPE ="vnd.android.cursor.dir/vnd.dvb.tp";
    public static final String CONTENT_ITEM_TYPE="vnd.android.cursor.item/vnd.dvb.tp";
    
    //Include Table Const Column
    public static final String TP_ID = "tp_id";
    public static final String TUNER_TYPE = "tuner_type";
    public static final String TUNER_ID = "tuner_id";
    public static final String NET_ID = "net_id";
    public static final String ORIGINAL_NET_ID = "original_net_id";
    public static final String TS_ID = "ts_id";
    public static final String FREQUENCY = "freq";
    public static final String SYMB_RATE = "symb_rate";
    public static final String EMOD = "emod";
    public static final String EFEC_INNER = "efec_inner";
    public static final String EFEC_OUTER = "efec_outer";
}
