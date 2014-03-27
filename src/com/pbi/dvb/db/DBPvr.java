/*
 * �� �� ��:  DBPvr.java
 * ��    Ȩ:  Huawei Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  
 * �޸�ʱ��:  2012-12-1
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.db;

import android.net.Uri;
import android.provider.BaseColumns;

/**
 * <pvr databases.>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-12-1]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class DBPvr implements BaseColumns
{
    
    public static final String TABLE_PVR_NAME = "db_pvr_table";
    
    public static final String PVR_AUTHORITY = "com.pbi.dvb.provider.PvrProvider";
    
    public static final String DATABASE_PATH = "/data/data/com.pbi.dvb/databases";
    
    public static final String PVR_DATABASE_NAME = "db_pvr_data.db";
    
    public static final String PVR_DATABASE_FILE_PATH = DATABASE_PATH + "/" + PVR_DATABASE_NAME;
    
    private DBPvr()
    {
    }
    
    public static final Uri CONTENT_URI = Uri.parse("content://" + PVR_AUTHORITY + "/" + TABLE_PVR_NAME);
    
    public static final int ITEM = 1;
    
    public static final int ITEM_ID = 2;
    
    public static final String CONTENT_TYPE = "vnd.android.cursor.dir/vnd.dvb.pvr";
    
    public static final String CONTENT_ITEM_TYPE = "vnd.android.cursor.item/vnd.dvb.pvr";
    
    /* Repeat mode */
    public static final int CLOSE_MODE = 0;
    
    public static final int DO_ONCE_MODE = 1;
    
    public static final int EVERY_DAY_MODE = 2;
    
    public static final int EVERY_WEEK_MODE = 3;
    
    public static final int EVERY_WORKDAY_MODE = 4;
    
    /* Status of record */
    public static final int PVR_TASK_UNINITIALIZTION = 0;
    
    public static final int PVR_TASK_INITIALIZATION = 1;
    
    public static final int PVR_TASK_EXECUTING = 2;
    
    public static final int PVR_TASK_FINISHED = 3;
    
    // Cancel state is used when one task is canceled by user manually...Such click the cancel button...Such as stop
    // record task by remote control...
    public static final int PVR_TASK_CANCEL = 4;
    
    // Skip state is used for those tasks who should not be executed but it was started...
    public static final int PVR_TASK_SKIP = 5;
    
    public static final int PVR_STOP_TYPE_MANUALLY = 1;
    
    public static final int PVR_STOP_TYPE_AUTOMATICALLY = 2;
    
    public static final int PVR_STOP_TYPE_ERROR = 3;
    
    /* Type of the pvr task */
    public static final int RECORD_WAKEUP_MODE = 0;
    
    public static final int PLAY_WAKEUP_MODE = 1;
    
    public static final int RECORD_FINISHED = 0;
    
    /* The max records task in the database */
    public static final int MAX_RECORD_ITEMS = 10;
    
    /* The max task set in AlarmManager */
    public static final int MAX_TASK_ITEMS = 2;
    
    /* Time for DVB to initialize the JNI resource before recording */
    public static final int DVB_INIT_DELAY = 10000;
    
    /* 弹出预约任务对话框的提前量 */
    /* Amount time to show the task start dialog before the real start time. */
    /* Example:: A task from 12:00 ~ 13:00, the dialog may be shown at 11:58:50 */
    public static final int TASK_START_DELAY = 70000;
    
    /* 任务触发的提前量, 一个任务占用的时间范围为该任务的真实时间加上这个预设的量 */
    /*
     * Ahead time of the task... A having time a task get is it's between it's start time minus the TASK_TRIGGER_DELAY
     * and end time plus TASK_TRIGGER_DELAY
     */
    /* Example:: A task from 12:00 ~ 13:00, the conflict time for calculate is from 11:58 ~ 13:02 */
    public static final int TASK_TRIGGER_DELAY = 120000;
    
    /* The total time before system to sleep after the task has finished... */
    public static final int TASK_SUSPEND_DELAY = 60;
    
    public static final int MAX_RECORD_LENGTH = 24 * 60 * 60 * 1000;
    
    /* Flag for SUSPEND_FLAG */
    public static final int SUSPEND_AFTER_RECORD = 0;
    
    public static final int DO_NOT_SUSPEND_AFTER_RECORD = 1;
    
    /* Flag for TASK FLAG */
    public static final int TASK_DEFAULT_START_WITH_DIALOG = 0;
    
    public static final int TASK_EPG_START_WITHOUT_DIALOG = 1;
    
    /* The pvr_ID for real time record task in EPG */
    public static final int REAL_TIME_TASK_ID = Integer.MAX_VALUE - 1000;
    
    /**/
    public static final String PVR_ID = "p_id";
    
    public static final String PVR_MODE = "pvr_mode";
    
    public static final String PVR_WAKEUP_MODE = "pvr_wakeup_mode";
    
    public static final String PVR_SERVICE_ID = "pvr_service_id";
    
    public static final String PVR_LOGICAL_NUMBER = "pvr_logical_number";
    
    public static final String PVR_SET_DATE = "pvr_set_date";
    
    public static final String PVR_START_TIME = "pvr_start_time";
    
    public static final String PVR_RECORD_LENGTH = "pvr_record_length";
    
    public static final String PVR_WEEK_DATE = "pvr_week_date";
    
    public static final String PVR_PMT_PID = "pvr_pmt_pid";
    
    public static final String PVR_TYPE_TAG = "pvr_type_tag";
    
    public static final String PVR_TRIGGER_TIME = "pvr_trigger_time";
    
    public static final String IF_SKIP_TASK = "if_skip_task";
    
    public static final String PVR_IS_SLEEP = "pvr_is_sleep";
}
