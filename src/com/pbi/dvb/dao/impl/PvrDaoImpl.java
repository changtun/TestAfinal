/*
 * �� �� ��:  PvrDaoImpl.java
 * ��    Ȩ:  Huawei Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  
 * �޸�ʱ��:  2012-12-1
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.dao.impl;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import android.content.ContentProviderOperation;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.OperationApplicationException;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.RemoteException;
import android.util.Log;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.db.DBopenHelper;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.utils.LogUtils;

/**
 * <һ�仰���ܼ���> <������ϸ����>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-12-1]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class PvrDaoImpl implements PvrDao
{
    private static final String TAG = "PvrDaoImpl";
    
    private Context context = null;
    
    private long next_Record_Start_Time = Long.MAX_VALUE;
    
    public PvrDaoImpl(Context context)
    {
        
        this.context = context;
        
    }
    
    @Override
    public int add_Record_Task(PvrBean task)
    {
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return -1;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in add_Record_Task(PvrBean task)");
            
            return -2;
        }
        
        ContentValues values = new ContentValues();
        
        if (null != task.getPvrId() && task.getPvrId() != 0)
        {
            values.put(DBPvr.PVR_ID, task.getPvrId());
        }
        
        values.put(DBPvr.PVR_MODE, task.getPvrMode() == null ? 0 : task.getPvrMode());
        values.put(DBPvr.PVR_WAKEUP_MODE, task.getPvrWakeMode() == null ? 0 : task.getPvrWakeMode());
        values.put(DBPvr.PVR_SERVICE_ID, task.getServiceId() == null ? 0 : task.getServiceId());
        values.put(DBPvr.PVR_LOGICAL_NUMBER, task.getLogicalNumber() == null ? 0 : task.getLogicalNumber());
        values.put(DBPvr.PVR_SET_DATE, task.getPvrSetDate() == null ? 0 : task.getPvrSetDate());
        values.put(DBPvr.PVR_START_TIME, task.getPvrStartTime() == null ? 0 : task.getPvrStartTime());
        values.put(DBPvr.PVR_RECORD_LENGTH, task.getPvrRecordLength() == null ? 0 : task.getPvrRecordLength());
        values.put(DBPvr.PVR_WEEK_DATE, task.getPvrWeekDate() == null ? 0 : task.getPvrWeekDate());
        values.put(DBPvr.PVR_PMT_PID, task.getPvr_pmt_pid() == null ? 0 : task.getPvr_pmt_pid());
        values.put(DBPvr.PVR_TYPE_TAG, task.getPvr_type_tag() == null ? 0 : task.getPvr_type_tag());
        values.put(DBPvr.PVR_IS_SLEEP, task.getPvr_is_sleep() == null ? 0 : task.getPvr_is_sleep().intValue());
        
        resolver.insert(DBPvr.CONTENT_URI, values);
        
        LogUtils.e(TAG, "after resolver.insert()");
        
        return 0;
    }
    
    @Override
    public PvrBean get_First_Record_Task(Integer... skipID)
    {
        LogUtils.e(TAG, ">>>>>> get_First_Record_Task begin <<<<<<");
        
        for (int i = 0; skipID != null && i < skipID.length; i++)
        {
            
            LogUtils.e(TAG, "skipID = " + skipID[i].intValue());
            
        }
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        // get_PVR_Filter() ;
        // Cursor filterCursor = resolver.query(DBPvr.CONTENT_URI, new String[]
        // {
        // DBPvr.PVR_ID, DBPvr.PVR_MODE, DBPvr.PVR_SET_DATE,
        // DBPvr.PVR_START_TIME, DBPvr.PVR_WEEK_DATE,
        // DBPvr.PVR_RECORD_LENGTH }, null, null, null);
        
        Cursor filterCursor = resolver.query(DBPvr.CONTENT_URI, null, null, null, null);
        
        PvrBean pvrBean = null;
        
        PvrBean returnBean = null;
        
        if (filterCursor != null)
        {
            
            Calendar calendar = Calendar.getInstance();
            
            Long current_Time = calendar.getTimeInMillis();
            
            calendar.set(Calendar.HOUR_OF_DAY, 0);
            calendar.set(Calendar.MINUTE, 0);
            calendar.set(Calendar.SECOND, 0);
            calendar.set(Calendar.MILLISECOND, 0);
            
            // Long current_Time = Calendar.getInstance().getTimeInMillis()
            // - calendar.getTimeInMillis();
            
            current_Time = current_Time - calendar.getTimeInMillis();
            
            Long current_Date = calendar.getTimeInMillis();
            
            int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
            
            next_Record_Start_Time = Long.MAX_VALUE;
            
            while (filterCursor.moveToNext())
            {
                
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(filterCursor.getLong(filterCursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(filterCursor.getLong(filterCursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(filterCursor.getLong(filterCursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(filterCursor.getInt(filterCursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
                
                boolean skipFlag = false;
                
                for (int i = 0; skipID != null && i < skipID.length; i++)
                {
                    
                    if (skipID[i].intValue() == pvrBean.getPvrId().intValue())
                    {
                        
                        skipFlag = true;
                        
                        break;
                        
                    }
                    
                }
                
                if (skipFlag)
                {
                    
                    LogUtils.i(TAG, "The task is skipped... The pvr_ID = " + pvrBean.getPvrId() + "    Task mode = "
                        + pvrBean.getPvrMode());
                    
                    continue;
                    
                }
                
                // mode ��ģʽ����int 0 �رգ�1һ�� 2 ÿ�� 3 ÿ�� 4 ������
                
                LogUtils.i(TAG, "The task did not be skipped... The pvr_ID = " + pvrBean.getPvrId()
                    + "    Task mode = " + pvrBean.getPvrMode());
                
                switch (pvrBean.getPvrMode())
                {
                
                    case DBPvr.CLOSE_MODE:
                        
                        LogUtils.i(TAG, "A colsed task item...PvrID = " + pvrBean.getPvrId());
                        
                        pvrBean = null;
                        
                        break;
                    
                    case DBPvr.DO_ONCE_MODE:
                        
                        LogUtils.e(TAG, "A just_do_once task item");
                        
                        if (current_Date > pvrBean.getPvrSetDate())
                        {
                            
                            // Ӧ�ùرյ���Ŀ
                            LogUtils.e(TAG, "A just_do_once task item that should be closed");
                            
                            pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
                            
                            update_Record_Task(pvrBean);
                            
                            break;
                            
                        }
                        
                        pvrBean = filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                        
                        break;
                    
                    case DBPvr.EVERY_DAY_MODE:
                        
                        LogUtils.e(TAG, "Repeated EVERY_DAY task   current_Time = " + current_Time);
                        
                        pvrBean = filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                        
                        break;
                    
                    case DBPvr.EVERY_WEEK_MODE:
                        
                        LogUtils.e(TAG, "Repeated EVERY_WEEK task");
                        
                        pvrBean = filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                        
                        break;
                    
                    case DBPvr.EVERY_WORKDAY_MODE:
                        
                        LogUtils.e(TAG, "Repeated EVERY_WORK_TIME");
                        
                        pvrBean = filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                        
                        break;
                    
                    default:
                        
                        LogUtils.e(TAG, "In get_First_Record_Task_1() of PvrDaoImpl.class, the mode of pvr is error.");
                        
                        LogUtils.e(TAG, "PVR_MODE = " + pvrBean.getPvrMode());
                        
                        break;
                
                }
                
                if (pvrBean != null)
                {
                    
                    returnBean = pvrBean;
                    
                }
                
            }
            
            if (!filterCursor.isClosed())
            {
                
                filterCursor.close();
                
            }
        }
        
        next_Record_Start_Time = Long.MAX_VALUE;
        
        if (returnBean == null)
        {
            
            LogUtils.e(TAG, "-------->>>>The result of get_First_Record_Task() is null....<<<<--------");
            
        }
        else
        {
            
            LogUtils.e(TAG, "-------->>>>The result of get_First_Record_Task() is not null....<<<<--------");
            
            LogUtils.e(TAG, returnBean.toString());
            
        }
        
        LogUtils.e(TAG, "-------->>>> get_First_Record_Task end <<<<<<");
        
        return returnBean;
    }
    
    private PvrBean filter_PvrBean(PvrBean pvrBean, long current_Date, long current_Time, int week_Of_Today)
    {
        
        LogUtils.e(TAG, "filter_PvrBean:current_Time = " + current_Time);
        
        long interval_Time = pvrBean.get_Adjust_StartTime(current_Date, current_Time, week_Of_Today);
        
        if (interval_Time == Long.MAX_VALUE && pvrBean.getPvrMode() == DBPvr.DO_ONCE_MODE)
        {
            
            LogUtils.e(TAG, "After get_Adjust_StartTime, the interval_Time is Long.MAX_VALUE...");
            
            LogUtils.e(TAG, "Item should be closed");
            
            // ���ڵ�һ��������, ��Ҫ�ر�
            pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
            
            update_Record_Task(pvrBean);
        }
        
        LogUtils.e(TAG, "pvr_ID = " + pvrBean.getPvrId() + "   inverval_Time = " + interval_Time
            + " Record_Start_Time = " + next_Record_Start_Time);
        
        if (interval_Time < next_Record_Start_Time)
        {
            
            next_Record_Start_Time = interval_Time;
            
            LogUtils.e(TAG, "next_Record_Start_Time = " + next_Record_Start_Time);
            
            return pvrBean;
        }
        
        return null;
    }
    
    // @Override
    // public PvrBean get_First_Record_Task(Integer skipID) {
    //
    // LogUtils.e(TAG,
    // "skip = " + (skipID == null ? "null" : skipID.intValue()));
    // LogUtils.e(TAG, ">>>>>> get_First_Record_Task begin <<<<<<");
    //
    // if (context == null) {
    //
    // LogUtils.e(TAG,
    // "The context is null in add_Record_Task of PvrDaoImpl.class");
    //
    // return null;
    // }
    //
    // ContentResolver resolver = context.getContentResolver();
    //
    // if (resolver == null) {
    // LogUtils.e(TAG,
    // "Can not get ContentResolver in get_First_Record_Task()");
    //
    // return null;
    // }
    //
    // // get_PVR_Filter() ;
    // Cursor filterCursor = resolver.query(DBPvr.CONTENT_URI, new String[] {
    // DBPvr.PVR_ID, DBPvr.PVR_MODE, DBPvr.PVR_SET_DATE,
    // DBPvr.PVR_START_TIME, DBPvr.PVR_WEEK_DATE,
    // DBPvr.PVR_RECORD_LENGTH }, null, null, null);
    //
    // List<PvrBean> pvrBeans = null;
    //
    // PvrBean pvrBean = null;
    //
    // pvrBeans = new ArrayList<PvrBean>();
    //
    // if (filterCursor != null) {
    //
    // while (filterCursor.moveToNext()) {
    //
    // pvrBean = new PvrBean();
    //
    // pvrBean.setPvrId(filterCursor.getInt(filterCursor
    // .getColumnIndex(DBPvr.PVR_ID)));
    // pvrBean.setPvrMode(filterCursor.getInt(filterCursor
    // .getColumnIndex(DBPvr.PVR_MODE)));
    // pvrBean.setPvrSetDate(filterCursor.getLong(filterCursor
    // .getColumnIndex(DBPvr.PVR_SET_DATE)));
    // pvrBean.setPvrStartTime(filterCursor.getLong(filterCursor
    // .getColumnIndex(DBPvr.PVR_START_TIME)));
    // pvrBean.setPvrWeekDate(filterCursor.getInt(filterCursor
    // .getColumnIndex(DBPvr.PVR_WEEK_DATE)));
    // pvrBean.setPvrRecordLength(filterCursor.getLong(filterCursor
    // .getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
    //
    // pvrBeans.add(pvrBean);
    //
    // }
    //
    // if (!filterCursor.isClosed()) {
    //
    // filterCursor.close();
    //
    // }
    // }
    //
    // Calendar calendar = Calendar.getInstance();
    //
    // Long current_Time = calendar.getTimeInMillis();
    //
    // calendar.set(Calendar.HOUR_OF_DAY, 0);
    // calendar.set(Calendar.MINUTE, 0);
    // calendar.set(Calendar.SECOND, 0);
    // calendar.set(Calendar.MILLISECOND, 0);
    //
    // // Long current_Time = Calendar.getInstance().getTimeInMillis()
    // // - calendar.getTimeInMillis();
    //
    // current_Time = current_Time - calendar.getTimeInMillis();
    //
    // Long current_Date = calendar.getTimeInMillis();
    //
    // int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
    //
    // PvrBean returnBean = null;
    //
    // PvrBean resultBean = null;
    //
    // // LogUtils.e(TAG, "pvrBeans' size = " + pvrBeans.size());
    //
    // for (int i = 0; pvrBeans != null && i < pvrBeans.size(); i++) {
    //
    // pvrBean = pvrBeans.get(i);
    //
    // if (skipID != null
    // && skipID.intValue() == pvrBean.getPvrId().intValue()) {
    //
    // continue;
    //
    // }
    //
    // // mode ��ģʽ����int 0 �رգ�1һ�� 2 ÿ�� 3 ÿ�� 4 ������
    // switch (pvrBean.getPvrMode()) {
    //
    // case DBPvr.CLOSE_MODE:
    //
    // LogUtils.e(TAG, "A colsed task item");
    //
    // break;
    //
    // case DBPvr.DO_ONCE_MODE:
    //
    // LogUtils.e(TAG, "A just_do_once task item");
    //
    // if (current_Date > pvrBean.getPvrSetDate()) {
    //
    // // Ӧ�ùرյ���Ŀ
    // LogUtils.e(TAG,
    // "A just_do_once task item that should be closed");
    //
    // pvrBean.setPvrMode(DBPvr.CLOSE_MODE);
    //
    // update_Record_Task(pvrBean);
    //
    // break;
    //
    // }
    //
    // returnBean = filter_PvrBean(pvrBean, current_Date,
    // current_Time, week_Of_Today);
    //
    // break;
    //
    // case DBPvr.EVERY_DAY_MODE:
    //
    // LogUtils.e(TAG, "Repeated EVERY_DAY task   current_Time = "
    // + current_Time);
    //
    // returnBean = filter_PvrBean(pvrBean, current_Date,
    // current_Time, week_Of_Today);
    //
    // break;
    //
    // case DBPvr.EVERY_WEEK_MODE:
    //
    // LogUtils.e(TAG, "Repeated EVERY_WEEK task");
    //
    // returnBean = filter_PvrBean(pvrBean, current_Date,
    // current_Time, week_Of_Today);
    //
    // break;
    //
    // case DBPvr.EVERY_WORKDAY_MODE:
    //
    // LogUtils.e(TAG, "Repeated EVERY_WORK_TIME");
    //
    // returnBean = filter_PvrBean(pvrBean, current_Date,
    // current_Time, week_Of_Today);
    //
    // break;
    //
    // default:
    //
    // LogUtils.e(TAG,
    // "In get_First_Record_Task_1() of PvrDaoImpl.class, the mode of pvr is error.");
    //
    // LogUtils.e(TAG, "PVR_MODE = " + pvrBean.getPvrMode());
    //
    // break;
    //
    // }
    //
    // if (returnBean != null) {
    //
    // LogUtils.e(TAG, "returnBean != null");
    //
    // resultBean = get_Record_Task_By_ID(returnBean.getPvrId());
    //
    // LogUtils.e(TAG, "resultBean = " + resultBean);
    //
    // }
    // }
    //
    // next_Record_Start_Time = Long.MAX_VALUE;
    //
    // LogUtils.e(TAG, ">>>>>> get_First_Record_Task end <<<<<<");
    //
    // return resultBean;
    // }
    
    // return 0 means success.
    // return -1 means the context is null ;
    // return -2 means can not get ContentResolver from Context.
    @Override
    public int update_Record_Task(PvrBean task)
    {
        synchronized (this)
        {
            if (null == task)
            {
                return 0;
            }
            
            if (context == null)
            {
                
                LogUtils.e(TAG, "The context is null in update_Record_Task of PvrDaoImpl.class");
                
                return -1;
            }
            
            ContentResolver resolver = context.getContentResolver();
            
            if (resolver == null)
            {
                LogUtils.e(TAG, "Can not get ContentResolver in update_Record_Task(PvrBean task)");
                
                return -2;
            }
            
            ContentValues values = new ContentValues();
            
            values.put(DBPvr.PVR_MODE, task.getPvrMode());
            values.put(DBPvr.PVR_WAKEUP_MODE, task.getPvrWakeMode());
            values.put(DBPvr.PVR_SERVICE_ID, task.getServiceId());
            values.put(DBPvr.PVR_LOGICAL_NUMBER, task.getLogicalNumber());
            values.put(DBPvr.PVR_SET_DATE, task.getPvrSetDate());
            values.put(DBPvr.PVR_START_TIME, task.getPvrStartTime());
            values.put(DBPvr.PVR_RECORD_LENGTH, task.getPvrRecordLength());
            values.put(DBPvr.PVR_WEEK_DATE, task.getPvrWeekDate());
            values.put(DBPvr.PVR_PMT_PID, task.getPvr_pmt_pid());
            values.put(DBPvr.PVR_TYPE_TAG, task.getPvr_type_tag());
            values.put(DBPvr.PVR_IS_SLEEP, task.getPvr_is_sleep());
            
            LogUtils.e(TAG, "before resolver.update");
            
            LogUtils.e(TAG, "pvrID = " + task.getPvrId() + " serviceID = " + task.getServiceId());
            
            resolver.update(DBPvr.CONTENT_URI, values, DBPvr.PVR_ID + " =  ? ", new String[] {task.getPvrId() + ""});
            LogUtils.e(TAG, "after resolver.update");
            
            return 0;
        }
    }
    
    // return 0 means success.
    // return -1 means the context is null ;
    // return -2 means can not get ContentResolver from Context.
    @Override
    public int delete_Record_Task(PvrBean pvrBean)
    {
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return -1;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in add_Record_Task(PvrBean task)");
            
            return -2;
        }
        
        resolver.delete(DBPvr.CONTENT_URI, DBPvr.PVR_ID + " = ?", new String[] {pvrBean.getPvrId() + ""});
        
        return 0;
    }
    
    @Override
    public int delete_Record_Task_By_ID(Integer pvr_ID)
    {
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return -1;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in add_Record_Task(PvrBean task)");
            
            return -2;
        }
        
        resolver.delete(DBPvr.CONTENT_URI, DBPvr.PVR_ID + " = ?", new String[] {pvr_ID + ""});
        
        return 0;
        
    }
    
    @Override
    public PvrBean get_Record_Task_By_ID(Integer _ID)
    {
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_Record_Task_By_ID()");
            
            return null;
        }
        
        Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null, DBPvr.PVR_ID + " = ?", new String[] {_ID + ""}, null);
        
        PvrBean pvrBean = null;
        
        if (cursor != null)
        {
            if (cursor.moveToNext())
            {
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
            }
            
            if (!cursor.isClosed())
            {
                cursor.close();
            }
        }
        
        return pvrBean;
        
    }
    
    @Override
    public List<PvrBean> get_All_Record_Task()
    {
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null, null, null, DBPvr.PVR_ID);
        
        List<PvrBean> pvrBeans = new ArrayList<PvrBean>();
        
        PvrBean pvrBean = null;
        
        if (cursor != null)
        {
            
            while (cursor.moveToNext())
            {
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
                
                pvrBeans.add(pvrBean);
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
        }
        
        return pvrBeans;
    }
    
    /*
     * ������ȡ��ݿ�������ʮ�������(�����رյ�/��������ݵ�)
     */
    @Override
    public List<PvrBean> get_All_Illegal_Tasks()
    {
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        Cursor cursor = null;
        
        // ��ȡ��ݿ��Ѿ��е��������
        cursor =
            resolver.query(DBPvr.CONTENT_URI, new String[] {"count(*)"}, DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG, null, null);
        
        int task_Count = 0;
        
        if (cursor != null)
        {
            
            if (cursor.moveToNext())
            {
                
                task_Count = cursor.getInt(0);
                
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
            
            if (task_Count < DBPvr.MAX_RECORD_ITEMS)
            {
                
                PvrBean addBean = null;
                
                for (int i = 0; i < DBPvr.MAX_RECORD_ITEMS - task_Count; i++)
                {
                    
                    addBean = new PvrBean();
                    
                    addBean.setPvrMode(DBPvr.CLOSE_MODE);
                    
                    add_Record_Task(addBean);
                    
                }
            }
            else if (task_Count > DBPvr.MAX_RECORD_ITEMS)
            {
                Log.w(TAG, "In " + DBPvr.TABLE_PVR_NAME
                    + " table, the number of records are bigger than DBPvr.MAX_RECORD_ITEMS(" + DBPvr.MAX_RECORD_ITEMS
                    + ")");
            }
            
        }
        else
        {
            
            throw new RuntimeException(
                "In get_All_Illegal_Tasks() function: Get an closed cursor that can not be used.");
            
        }
        
        cursor =
            resolver.query(DBPvr.CONTENT_URI,
                null,
                DBPvr.PVR_TYPE_TAG + "!=" + DBPvr.TASK_EPG_START_WITHOUT_DIALOG,
                null,
                DBPvr.PVR_ID);
        
        List<PvrBean> pvrBeans = null;
        
        PvrBean pvrBean = null;
        
        if (cursor != null)
        {
            
            pvrBeans = new ArrayList<PvrBean>();
            
            int count = 0;
            
            while (cursor.moveToNext())
            {
                
                if (count >= 10)
                {
                    break;
                }
                
                count++;
                
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
                
                Calendar calendar = Calendar.getInstance();
                
                Long current_Time = calendar.getTimeInMillis();
                
                calendar.set(Calendar.HOUR_OF_DAY, 0);
                calendar.set(Calendar.MINUTE, 0);
                calendar.set(Calendar.SECOND, 0);
                calendar.set(Calendar.MILLISECOND, 0);
                
                // Long current_Time = Calendar.getInstance().getTimeInMillis()
                // - calendar.getTimeInMillis();
                
                current_Time = current_Time - calendar.getTimeInMillis();
                
                Long current_Date = calendar.getTimeInMillis();
                
                int week_Of_Today = calendar.get(Calendar.DAY_OF_WEEK);
                
                if (pvrBean.getPvrMode() != DBPvr.CLOSE_MODE)
                {
                    
                    filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                    
                }
                
                pvrBeans.add(pvrBean);
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
        }
        
        if (pvrBeans == null)
        {
            
            LogUtils.e(TAG, "Failed to get records from database " + DBPvr.TABLE_PVR_NAME);
            
        }
        
        return pvrBeans;
        
    }
    
    @Override
    public PvrBean get_First_Close_Task()
    {
        LogUtils.e(TAG, " get_First_Close_Task() begin ");
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        // Cursor temp = null;
        //
        // temp = resolver.query(DBPvr.CONTENT_URI, new String[] { "count(*)" },
        // null, null, DBPvr.PVR_ID);
        //
        // if (temp.moveToNext()) {
        // int count = temp.getInt(0);
        // LogUtils.e(TAG, "gong you " + count + " tiao shuju");
        // }
        
        // Ϊʲô����һ��Ҫд����???DBPvr.PVR_MODE=0....
        // Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null,
        // DBPvr.PVR_MODE
        // + " = ? ", new String[] { String.valueOf(DBPvr.CLOSE_MODE) },
        // DBPvr.PVR_ID);
        //
        // Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null,
        // DBPvr.PVR_MODE
        // + " = ? ", new String[] { 0 + "" },
        // DBPvr.PVR_ID);
        //
        // Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null,
        // DBPvr.PVR_MODE
        // + " = ? ", new String[] { "0" },
        // DBPvr.PVR_ID);
        // Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null,
        // DBPvr.PVR_MODE
        // + " = ? ", new String[] { "0" }, DBPvr.PVR_ID);
        
        /* DBPvr.PVR_MODE = 0 means DBPvr.PVR_MODE = DBPvr.CLOSE_MODE */
        Cursor cursor =
            resolver.query(DBPvr.CONTENT_URI, null, DBPvr.PVR_MODE + " = 0 and " + DBPvr.PVR_TYPE_TAG + " != "
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG, null, DBPvr.PVR_ID);
        
        // LogUtils.e(TAG, "URI = " + DBPvr.PVR_MODE + " = " +
        // DBPvr.CLOSE_MODE);
        
        PvrBean pvrBean = null;
        
        if (cursor != null)
        {
            if (cursor.moveToNext())
            {
                // LogUtils.e(TAG, "get data from cursor to PvrBean begin");
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
                
                // LogUtils.e(TAG, "get data from cursor to PvrBean end");
                
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
            
        }
        else
        {
            
            throw new RuntimeException("In get_First_Close_Task(), get a null cursor.");
        }
        
        // LogUtils.e(TAG, " get_First_Close_Task() end ");
        
        return pvrBean;
    }
    
    @Override
    public int get_Count_Of_Unclose_Task()
    {
        LogUtils.e(TAG, " get_Count_Of_Unclose_Task() begin ");
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return -1;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return -1;
        }
        
        Cursor cursor =
            resolver.query(DBPvr.CONTENT_URI, new String[] {"count(*)"}, DBPvr.PVR_MODE + " != " + DBPvr.CLOSE_MODE
                + " and " + DBPvr.PVR_TYPE_TAG + " != " + DBPvr.TASK_EPG_START_WITHOUT_DIALOG, null, null);
        
        int result = -1;
        
        if (cursor != null)
        {
            
            if (cursor.moveToNext())
            {
                
                result = cursor.getInt(0);
                
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
            
        }
        else
        {
            
            throw new RuntimeException("In get_First_Close_Task(), get a null cursor.");
        }
        
        return result;
    }
    
    @Override
    public int get_Count_Of_Conflict_Task(PvrBean pvrBean)
    {
        
        LogUtils.e(TAG, " get_Count_Of_Conflict_Task() begin ");
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return -1;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return -1;
        }
        
        int result = 0;
        
        String strSQL = null;
        
        Cursor cursor = null;
        
        DBopenHelper helper = new DBopenHelper(context);
        
        SQLiteDatabase db = null;
        
        switch (pvrBean.getPvrMode())
        {
        
            case DBPvr.CLOSE_MODE:
                
                return 0;
                
            case DBPvr.DO_ONCE_MODE:
                
                strSQL = get_Do_Once_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_DAY_MODE:
                
                strSQL = get_Every_Day_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_WEEK_MODE:
                
                strSQL = get_Every_Week_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_WORKDAY_MODE:
                
                strSQL = getEvery_Work_Day_Task_SQL(pvrBean);
                
                break;
            
            default:
                
                LogUtils.e(TAG, "Unexpected value for repeat mode... SQL SELECTION = ERROR");
                
                return -3;
                
        }
        
        // new String[]
        // {startTime, startTime, startTime+recordLength,
        // startTime+recordLength, startDate, weekDate, weekDate, weekDate
        // , startTime + 24 * 60 * 60 * 1000, startTime + 24 * 60 * 60 * 1000,
        // startDate, weekDate - 1, weekDate, weekDate, startTime,
        // startTime+recordLength, startDate, weekDate + 1, weekDate,
        // weekDate}
        
        LogUtils.e(TAG, " SQL SELECTION = " + strSQL);
        
        db = helper.getWritableDatabase();
        
        if (db != null)
        {
            
            if (db.isOpen())
            {
                
                cursor = db.rawQuery(strSQL, null);
                
            }
        }
        
        if (cursor != null)
        {
            
            if (!cursor.isClosed() && cursor.moveToNext())
            {
                
                result = cursor.getInt(0);
                
                LogUtils.e(TAG, "Conflict result = " + result);
                
            }
            else
            {
                
                if (cursor.isClosed())
                {
                    
                    LogUtils.e(TAG, " get a closed cursor");
                    
                }
                else
                {
                    
                    LogUtils.e(TAG, "occur some problem while executing cursor.moveToNext() ");
                    
                }
            }
            
            cursor.close();
            
        }
        else
        {
            
            LogUtils.e(TAG, "cursor == null");
            
            return -1;
            
        }
        
        if (db != null)
        {
            
            if (db.isOpen())
            {
                
                db.close();
                
                db = null;
            }
            
        }
        else
        {
            
            LogUtils.e(TAG, "The  writable SQLiteDatabase is null");
            
            return -2;
            
        }
        
        helper.close();
        
        helper = null;
        
        return result;
    }
    
    private String get_Every_Day_Task_SQL(PvrBean pvrBean)
    {
        
        String sql =
            "select count(*) from " + DBPvr.TABLE_PVR_NAME + " where (" + DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG + ") AND ( " + DBPvr.PVR_ID + " != " + pvrBean.getPvrId()
                + " ) AND ((((( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " ) <= "
                + pvrBean.getPvrStartTime() + " AND " + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME
                + " + " + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR (("
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " ) < "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " AND "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " <= ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR (( " + pvrBean.getPvrStartTime()
                + " <= " + DBPvr.PVR_START_TIME + " ) AND ( " + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
                + " ) <= " + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " )) AND ( " + DBPvr.PVR_MODE
                + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " >= " + pvrBean.getPvrSetDate() + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WEEK_MODE + " ) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE
                + " ))) OR (( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " < "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " AND "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " < ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " =  " + DBPvr.DO_ONCE_MODE + " AND (( "
                + DBPvr.PVR_SET_DATE + " + " + 24 * 60 * 60 * 1000 + " ) >= " + pvrBean.getPvrSetDate() + " )) OR ( "
                + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE + " ) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WORKDAY_MODE + " ))) OR (( " + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME
                + " - " + DBPvr.TASK_TRIGGER_DELAY + " + " + 24 * 60 * 60 * 1000 + " ) AND ( " + DBPvr.PVR_START_TIME
                + " - " + DBPvr.TASK_TRIGGER_DELAY + " + " + 24 * 60 * 60 * 1000 + " ) <= "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " ) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " > " + pvrBean.getPvrSetDate() + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WEEK_MODE + " ) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " ))))";
        
        return sql;
    }
    
    private String getEvery_Work_Day_Task_SQL(PvrBean pvrBean)
    {
        
        String sql =
            "select count(*) from " + DBPvr.TABLE_PVR_NAME + " where (" + DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG + ") AND  ( " + DBPvr.PVR_ID + " != " + pvrBean.getPvrId()
                + " ) AND ((((( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " ) <= "
                + pvrBean.getPvrStartTime() + " AND " + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME
                + " + " + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " < ( "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " ) AND "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " <= ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR (( " + pvrBean.getPvrStartTime()
                + " <= " + DBPvr.PVR_START_TIME + " ) AND ( " + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
                + " ) <= " + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " )) AND ( " + DBPvr.PVR_MODE
                + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_WEEK_DATE + " >= 2 AND " + DBPvr.PVR_WEEK_DATE + " <= 6) AND ( " + DBPvr.PVR_SET_DATE
                + " >= " + pvrBean.getPvrSetDate() + " )) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE
                + " AND ( " + DBPvr.PVR_WEEK_DATE + " >= 2 AND " + DBPvr.PVR_WEEK_DATE + " <= 6)) OR ( "
                + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " ))) OR (( " + DBPvr.PVR_START_TIME + " - "
                + DBPvr.TASK_TRIGGER_DELAY + " < " + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " AND "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " < ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND (( "
                + DBPvr.PVR_SET_DATE + " + " + 24 * 60 * 60 * 1000 + " ) >= " + pvrBean.getPvrSetDate() + " ) AND ( "
                + DBPvr.PVR_WEEK_DATE + " >= 1 AND " + DBPvr.PVR_WEEK_DATE + " <= 5)) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " >= 1 AND " + DBPvr.PVR_WEEK_DATE
                + " ) <= 5) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " ))) OR (( "
                + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + "
                + 24 * 60 * 60 * 1000 + " ) AND ( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + "
                + 24 * 60 * 60 * 1000 + " ) <= " + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength())
                + " ) AND ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.DO_ONCE_MODE + " AND ( " + DBPvr.PVR_SET_DATE + " > " + pvrBean.getPvrSetDate() + " ) AND ( "
                + DBPvr.PVR_WEEK_DATE + " >= 3 AND " + DBPvr.PVR_WEEK_DATE + " <= 7)) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " >= 3 AND " + DBPvr.PVR_WEEK_DATE
                + " <= 7)) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " ))))";
        
        return sql;
    }
    
    private String get_Every_Week_Task_SQL(PvrBean pvrBean)
    {
        
        String sql =
            "select count(*) from " + DBPvr.TABLE_PVR_NAME + " where (" + DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG + ") AND  ( " + DBPvr.PVR_ID + " != " + pvrBean.getPvrId()
                + " ) AND ((((( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " ) <=  "
                + pvrBean.getPvrStartTime() + " AND " + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME
                + " + " + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " < "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " AND "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " <= ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) OR (( " + pvrBean.getPvrStartTime()
                + " <= " + DBPvr.PVR_START_TIME + " ) AND ( " + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
                + " ) <= " + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " )) AND ( " + DBPvr.PVR_MODE
                + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_WEEK_DATE + " = " + pvrBean.getPvrWeekDate() + " AND " + DBPvr.PVR_SET_DATE + " >= "
                + pvrBean.getPvrSetDate() + " )) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( "
                + DBPvr.PVR_WEEK_DATE + " = " + pvrBean.getPvrWeekDate() + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WORKDAY_MODE + " AND (2 <= " + pvrBean.getPvrWeekDate() + " AND "
                + pvrBean.getPvrWeekDate() + " <= 5)))) OR (( " + DBPvr.PVR_START_TIME + " - "
                + DBPvr.TASK_TRIGGER_DELAY + " < " + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " AND "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " < (" + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + " )) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND (( "
                + DBPvr.PVR_SET_DATE + " + " + 24 * 60 * 60 * 1000 + " ) >= " + pvrBean.getPvrSetDate() + " ) AND ( "
                + DBPvr.PVR_WEEK_DATE + " = " + minusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( " + DBPvr.PVR_MODE
                + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " = "
                + minusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WORKDAY_MODE + " AND (3 <= " + pvrBean.getPvrWeekDate() + " AND "
                + pvrBean.getPvrWeekDate() + " <= 7)))) OR (( " + pvrBean.getPvrStartTime() + " < ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + " + 24 * 60 * 60 * 1000 + " ) AND ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + " + 24 * 60 * 60 * 1000 + " ) <= "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " ) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " > " + pvrBean.getPvrSetDate() + " ) AND ( " + DBPvr.PVR_WEEK_DATE + " = "
                + plusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE
                + " AND ( " + DBPvr.PVR_WEEK_DATE + " = " + plusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( "
                + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " AND (1 <= " + pvrBean.getPvrWeekDate()
                + " AND " + pvrBean.getPvrWeekDate() + " <= 5)))))";
        
        return sql;
    }
    
    private String get_Do_Once_Task_SQL(PvrBean pvrBean)
    {
        // ������, ��ѯ������ȷ�Ľ��...
        // selection = "((( " + DBPvr.PVR_START_TIME + " <= ? AND ? < ( "
        // + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
        // + ")) OR ( " + DBPvr.PVR_START_TIME + " < ?  AND ? <= ( "
        // + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
        // + "))) AND ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.DO_ONCE_MODE + " AND ( " + DBPvr.PVR_SET_DATE
        // + " = ? )) " + " OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE
        // + " = ? )) OR (" + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_WORKDAY_MODE
        // + " AND ( 2 <= ?  AND ? <= 6)))) OR (( "
        // + DBPvr.PVR_START_TIME + " < ? AND ? < ( "
        // + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
        // + ")) AND ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.DO_ONCE_MODE + " AND ( " + DBPvr.PVR_START_TIME
        // + " + 24 * 60 * 60 * 1000 = ?)) OR ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( "
        // + DBPvr.PVR_WEEK_DATE + " = ? )) OR ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.EVERY_WORKDAY_MODE
        // + " AND (3 <= ? AND ? <= 7)))) OR ((( ? < "
        // + DBPvr.PVR_START_TIME + " + 24 * 60 * 60 * 1000 ) AND ( "
        // + DBPvr.PVR_START_TIME
        // + " + 24 * 60 * 60 * 1000 < ? )) AND ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
        // + DBPvr.PVR_SET_DATE
        // + " - 24 * 60 * 60 * 1000 = ? )) OR ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( "
        // + DBPvr.PVR_WEEK_DATE + " = ? )) OR ( " + DBPvr.PVR_MODE
        // + " = " + DBPvr.EVERY_WORKDAY_MODE
        // + " AND (1 <= ? AND ? <= 5))))";
        //
        // SQLiteDatabase db = helper.getWritableDatabase();
        //
        // cursor = db.query(
        // DBPvr.TABLE_PVR_NAME,
        // new String[] { "count(*)" },
        // selection,
        // new String[] {
        // pvrBean.getPvrStartTime() + "",
        // pvrBean.getPvrStartTime() + "",
        // (pvrBean.getPvrStartTime() + pvrBean
        // .getPvrRecordLength()) + "",
        // (pvrBean.getPvrStartTime() + pvrBean
        // .getPvrRecordLength()) + "",
        // pvrBean.getPvrSetDate() + "",
        // pvrBean.getPvrWeekDate() + "",
        // pvrBean.getPvrWeekDate() + "",
        // pvrBean.getPvrWeekDate() + "",
        // (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 60)
        // + "",
        // (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 60)
        // + "",
        // pvrBean.getPvrSetDate() + "",
        // minusWeekDate(pvrBean.getPvrWeekDate()) + "",
        // pvrBean.getPvrWeekDate() + "",
        // pvrBean.getPvrWeekDate() + "",
        // pvrBean.getPvrStartTime() + "",
        // (pvrBean.getPvrStartTime() + pvrBean
        // .getPvrRecordLength()) + "",
        // pvrBean.getPvrSetDate() + "",
        // plusWeekDate(pvrBean.getPvrWeekDate()) + "",
        // pvrBean.getPvrWeekDate() + "",
        // pvrBean.getPvrWeekDate() + "", }, null, null, null);
        
        // ����, ��û�����ǰ����֧��
        // selection = "select count(*) from "
        // + DBPvr.TABLE_PVR_NAME
        // + " where "
        // + "((( "
        // + DBPvr.PVR_START_TIME
        // + " <= "
        // + pvrBean.getPvrStartTime()
        // + " AND "
        // + pvrBean.getPvrStartTime()
        // + " < ( "
        // + DBPvr.PVR_START_TIME
        // + " + "
        // + DBPvr.PVR_RECORD_LENGTH
        // + ")) OR ( "
        // + DBPvr.PVR_START_TIME
        // + " < "
        // + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength())
        // + "  AND "
        // + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength())
        // + " <= ( "
        // + DBPvr.PVR_START_TIME
        // + " + "
        // + DBPvr.PVR_RECORD_LENGTH
        // + "))) AND ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_DAY_MODE
        // + " OR ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.DO_ONCE_MODE
        // + " AND ( "
        // + DBPvr.PVR_SET_DATE
        // + " = "
        // + pvrBean.getPvrSetDate()
        // + " )) "
        // + " OR ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_WEEK_MODE
        // + " AND ( "
        // + DBPvr.PVR_WEEK_DATE
        // + " = "
        // + pvrBean.getPvrWeekDate()
        // + " )) OR ("
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_WORKDAY_MODE
        // + " AND ( 2 <= "
        // + pvrBean.getPvrWeekDate()
        // + "  AND "
        // + pvrBean.getPvrWeekDate()
        // + " <= 6)))) OR (( "
        // + DBPvr.PVR_START_TIME
        // + " < "
        // + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 60)
        // + " AND "
        // + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 60)
        // + " < ( "
        // + DBPvr.PVR_START_TIME
        // + " + "
        // + DBPvr.PVR_RECORD_LENGTH
        // + ")) AND ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_DAY_MODE
        // + " OR ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.DO_ONCE_MODE
        // + " AND ( "
        // + DBPvr.PVR_START_TIME
        // + " + 24 * 60 * 60 * 1000 = "
        // + pvrBean.getPvrSetDate()
        // + ")) OR ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_WEEK_MODE
        // + " AND ( "
        // + DBPvr.PVR_WEEK_DATE
        // + " = "
        // + minusWeekDate(pvrBean.getPvrWeekDate())
        // + " )) OR ( "
        // + DBPvr.PVR_MODE
        // + " = "
        // + DBPvr.EVERY_WORKDAY_MODE
        // + " AND (3 <= "
        // + pvrBean.getPvrWeekDate()
        // + " AND "
        // + pvrBean.getPvrWeekDate()
        // + " <= 7)))) OR ((( "
        // + pvrBean.getPvrStartTime()
        // + " < "
        // + DBPvr.PVR_START_TIME
        // + " + 24 * 60 * 60 * 1000 ) AND ( "
        // + DBPvr.PVR_START_TIME
        // + " + 24 * 60 * 60 * 1000 < "
        // + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength())
        // + " )) AND ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.DO_ONCE_MODE + " AND ( " + DBPvr.PVR_SET_DATE
        // + " - 24 * 60 * 60 * 1000 = " + pvrBean.getPvrSetDate()
        // + " )) OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE
        // + " = " + plusWeekDate(pvrBean.getPvrWeekDate())
        // + " )) OR ( " + DBPvr.PVR_MODE + " = "
        // + DBPvr.EVERY_WORKDAY_MODE + " AND (1 <= "
        // + pvrBean.getPvrWeekDate() + " AND "
        // + pvrBean.getPvrWeekDate() + " <= 5))))";
        
        String sql =
            "select count(*) from " + DBPvr.TABLE_PVR_NAME + " where (" + DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG + ") AND  ( " + DBPvr.PVR_ID + " != " + pvrBean.getPvrId()
                + " ) AND " + "(((( " + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " <= "
                + pvrBean.getPvrStartTime() + " AND " + pvrBean.getPvrStartTime() + " < ( " + DBPvr.PVR_START_TIME
                + " + " + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + ")) OR ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " < "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + "  AND "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " <= ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + ")) OR (( " + pvrBean.getPvrStartTime()
                + " <= " + DBPvr.PVR_START_TIME + " ) AND ( " + DBPvr.PVR_START_TIME + " + " + DBPvr.PVR_RECORD_LENGTH
                + " ) <= " + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " )) AND ( " + DBPvr.PVR_MODE
                + " = " + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " = " + pvrBean.getPvrSetDate() + " )) " + " OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " = " + pvrBean.getPvrWeekDate()
                + " )) OR (" + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WORKDAY_MODE + " AND ( 2 <= "
                + pvrBean.getPvrWeekDate() + "  AND " + pvrBean.getPvrWeekDate() + " <= 6)))) OR ((( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " ) < "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " AND "
                + (pvrBean.getPvrStartTime() + 24 * 60 * 60 * 1000) + " < ( " + DBPvr.PVR_START_TIME + " + "
                + DBPvr.PVR_RECORD_LENGTH + " + " + DBPvr.TASK_TRIGGER_DELAY + ")) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " + 24 * 60 * 60 * 1000 = " + pvrBean.getPvrSetDate() + ")) OR ( "
                + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " = "
                + minusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WORKDAY_MODE + " AND (3 <= " + pvrBean.getPvrWeekDate() + " AND "
                + pvrBean.getPvrWeekDate() + " <= 7)))) OR ((( " + pvrBean.getPvrStartTime() + " < "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + 24 * 60 * 60 * 1000 ) AND ( "
                + DBPvr.PVR_START_TIME + " - " + DBPvr.TASK_TRIGGER_DELAY + " + 24 * 60 * 60 * 1000 < "
                + (pvrBean.getPvrStartTime() + pvrBean.getPvrRecordLength()) + " )) AND ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_DAY_MODE + " OR ( " + DBPvr.PVR_MODE + " = " + DBPvr.DO_ONCE_MODE + " AND ( "
                + DBPvr.PVR_SET_DATE + " - 24 * 60 * 60 * 1000 = " + pvrBean.getPvrSetDate() + " )) OR ( "
                + DBPvr.PVR_MODE + " = " + DBPvr.EVERY_WEEK_MODE + " AND ( " + DBPvr.PVR_WEEK_DATE + " = "
                + plusWeekDate(pvrBean.getPvrWeekDate()) + " )) OR ( " + DBPvr.PVR_MODE + " = "
                + DBPvr.EVERY_WORKDAY_MODE + " AND (1 <= " + pvrBean.getPvrWeekDate() + " AND "
                + pvrBean.getPvrWeekDate() + " <= 5)))))";
        
        return sql;
    }
    
    private int plusWeekDate(int weekDate)
    {
        
        int reuslt = 0;
        
        switch (weekDate)
        {
        
            case Calendar.SUNDAY:
                
                reuslt = Calendar.MONDAY;
                
                break;
            
            case Calendar.MONDAY:
                
                reuslt = Calendar.TUESDAY;
                
                break;
            
            case Calendar.TUESDAY:
                
                reuslt = Calendar.WEDNESDAY;
                
                break;
            
            case Calendar.WEDNESDAY:
                
                reuslt = Calendar.THURSDAY;
                
                break;
            
            case Calendar.THURSDAY:
                
                reuslt = Calendar.FRIDAY;
                
                break;
            
            case Calendar.FRIDAY:
                
                reuslt = Calendar.SATURDAY;
                
                break;
            
            case Calendar.SATURDAY:
                
                reuslt = Calendar.SUNDAY;
                
                break;
        }
        
        return reuslt;
        
    }
    
    private int minusWeekDate(int weekDate)
    {
        
        int reuslt = 0;
        
        switch (weekDate)
        {
        
            case Calendar.SUNDAY:
                
                reuslt = Calendar.SATURDAY;
                
                break;
            
            case Calendar.MONDAY:
                
                reuslt = Calendar.SUNDAY;
                
                break;
            
            case Calendar.TUESDAY:
                
                reuslt = Calendar.MONDAY;
                
                break;
            
            case Calendar.WEDNESDAY:
                
                reuslt = Calendar.TUESDAY;
                
                break;
            
            case Calendar.THURSDAY:
                
                reuslt = Calendar.WEDNESDAY;
                
                break;
            
            case Calendar.FRIDAY:
                
                reuslt = Calendar.THURSDAY;
                
                break;
            
            case Calendar.SATURDAY:
                
                reuslt = Calendar.FRIDAY;
                
                break;
        }
        
        return reuslt;
        
    }
    
    @Override
    public void init_Pvr_Database(int new_Item_Count)
    {
        
        LogUtils.e(TAG, "PvrDaoImpl::init_Pvr_Database() begin ");
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return;
        }
        
        ArrayList<ContentProviderOperation> batch_Ops = new ArrayList<ContentProviderOperation>();
        
        for (int i = 0; i < new_Item_Count; i++)
        {
            
            LogUtils.e(TAG, "Add batch options " + i);
            
            batch_Ops.add(ContentProviderOperation.newInsert(DBPvr.CONTENT_URI)
                .withValue(DBPvr.PVR_MODE, DBPvr.CLOSE_MODE)
                .withValue(DBPvr.PVR_WAKEUP_MODE, 0)
                .withValue(DBPvr.PVR_SERVICE_ID, 0)
                .withValue(DBPvr.PVR_LOGICAL_NUMBER, 0)
                .withValue(DBPvr.PVR_SET_DATE, 0)
                .withValue(DBPvr.PVR_START_TIME, 0)
                .withValue(DBPvr.PVR_RECORD_LENGTH, 0)
                .withValue(DBPvr.PVR_WEEK_DATE, 0)
                .withValue(DBPvr.PVR_PMT_PID, 0)
                .withValue(DBPvr.PVR_TYPE_TAG, 0)
                .withValue(DBPvr.PVR_IS_SLEEP, 0)
                .build());
            
        }
        
        try
        {
            
            resolver.applyBatch(DBPvr.PVR_AUTHORITY, batch_Ops);
            
            LogUtils.e(TAG, "In PvrDaoImpl::init_Pvr_Database After resolver.applyBatch");
            
        }
        catch (RemoteException e)
        {
            
            LogUtils.e(TAG, "In PvrDaoImpl::init_Pvr_Database RemoteException error");
            
            e.printStackTrace();
            
        }
        catch (OperationApplicationException e)
        {
            
            LogUtils.e(TAG, "In PvrDaoImpl::init_Pvr_Database OperationApplicationException error");
            
            e.printStackTrace();
            
        }
    }
    
    @Override
    public void filter_All_Task()
    {
        
        if (context == null)
        {
            
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            throw new RuntimeException(
                "---->>>> PvrDaoImpl:: The context is null...Please check wether the system is busy and the system recyle the context... <<<<----");
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            throw new RuntimeException(
                "---->>>> PvrDaoImpl:: The resolver is null...Please check wether the system is busy and the system recyle the context... <<<<----");
            
        }
        
        Cursor cursor = resolver.query(DBPvr.CONTENT_URI, null, null, null, DBPvr.PVR_START_TIME);
        
        PvrBean pvrBean = null;
        
        Calendar current = Calendar.getInstance();
        
        long current_Date = 0;
        long current_Time = 0;
        int week_Of_Today = 0;
        
        current_Time = current.getTimeInMillis();
        
        current.set(Calendar.HOUR_OF_DAY, 0);
        current.set(Calendar.MINUTE, 0);
        current.set(Calendar.SECOND, 0);
        current.set(Calendar.MILLISECOND, 0);
        
        current_Date = current.getTimeInMillis();
        current_Time = current_Time - current_Date;
        week_Of_Today = current.get(Calendar.DAY_OF_WEEK);
        
        if (cursor != null)
        {
            
            while (cursor.moveToNext())
            {
                
                pvrBean = new PvrBean();
                
                pvrBean.setPvrId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
                pvrBean.setPvrMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE)));
                pvrBean.setPvrWakeMode(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WAKEUP_MODE)));
                pvrBean.setServiceId(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_SERVICE_ID)));
                pvrBean.setLogicalNumber(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_LOGICAL_NUMBER)));
                
                pvrBean.setPvrSetDate(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE)));
                pvrBean.setPvrStartTime(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_START_TIME)));
                pvrBean.setPvrRecordLength(cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_RECORD_LENGTH)));
                
                pvrBean.setPvrWeekDate(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_WEEK_DATE)));
                pvrBean.setPvr_pmt_pid(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_PMT_PID)));
                pvrBean.setPvr_type_tag(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_TYPE_TAG)));
                pvrBean.setPvr_is_sleep(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_IS_SLEEP)));
                
                filter_PvrBean(pvrBean, current_Date, current_Time, week_Of_Today);
                
            }
            
            if (!cursor.isClosed())
            {
                
                cursor.close();
                
            }
        }
        
    }
    
    @Override
    public List<Integer> getIdOfConflictTasks(PvrBean pvrBean)
    {
        LogUtils.e(TAG, " get_Count_Of_Conflict_Task() begin ");
        
        List<Integer> result = null;
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        String strSQL = null;
        
        Cursor cursor = null;
        
        DBopenHelper helper = new DBopenHelper(context);
        
        SQLiteDatabase db = null;
        
        switch (pvrBean.getPvrMode())
        {
            case DBPvr.CLOSE_MODE:
                
                return null;
                
            case DBPvr.DO_ONCE_MODE:
                
                strSQL = get_Do_Once_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_DAY_MODE:
                
                strSQL = get_Every_Day_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_WEEK_MODE:
                
                strSQL = get_Every_Week_Task_SQL(pvrBean);
                
                break;
            
            case DBPvr.EVERY_WORKDAY_MODE:
                
                strSQL = getEvery_Work_Day_Task_SQL(pvrBean);
                
                break;
            
            default:
                
                LogUtils.e(TAG, "Unexpected value for repeat mode... SQL SELECTION = ERROR");
                
                return null;
                
        }
        
        strSQL = strSQL.replace("count(*)", DBPvr.PVR_ID);
        
        // new String[]
        // {startTime, startTime, startTime+recordLength,
        // startTime+recordLength, startDate, weekDate, weekDate, weekDate
        // , startTime + 24 * 60 * 60 * 1000, startTime + 24 * 60 * 60 * 1000,
        // startDate, weekDate - 1, weekDate, weekDate, startTime,
        // startTime+recordLength, startDate, weekDate + 1, weekDate,
        // weekDate}
        
        LogUtils.e(TAG, " SQL SELECTION = " + strSQL);
        
        db = helper.getWritableDatabase();
        
        if (db != null)
        {
            if (db.isOpen())
            {
                cursor = db.rawQuery(strSQL, null);
            }
        }
        
        if (cursor != null)
        {
            while (!cursor.isClosed() && cursor.moveToNext())
            {
                if (null == result)
                {
                    result = new ArrayList<Integer>();
                }
                
                result.add(cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID)));
            }
            
            cursor.close();
        }
        else
        {
            
            LogUtils.e(TAG, "cursor == null");
            
            return null;
            
        }
        
        if (db != null)
        {
            if (db.isOpen())
            {
                db.close();
                
                db = null;
            }
            
        }
        else
        {
            LogUtils.e(TAG, "The  writable SQLiteDatabase is null");
            
            return null;
        }
        
        helper.close();
        
        helper = null;
        
        return result;
    }
    
    @Override
    public void insert_REAL_TIME_Task()
    {
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.insert_REAL_TIME_Task run...");
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in add_Record_Task(PvrBean task)");
            
            return;
        }
        
        PvrBean addBean = new PvrBean();
        addBean.setPvrId(DBPvr.REAL_TIME_TASK_ID);
        /* When do the initialization operation, the pvr_mode must be CLOSE_MODE */
        addBean.setPvrMode(DBPvr.CLOSE_MODE);
        addBean.setPvr_type_tag(DBPvr.TASK_EPG_START_WITHOUT_DIALOG);
        
        add_Record_Task(addBean);
        
        return;
    }
    
    @Override
    public PvrBean get_REAL_TIME_Task()
    {
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.insert_REAL_TIME_Task try to get the REAL TIME task" + "" + "cd ...");
        
        PvrBean addBean = get_Record_Task_By_ID(DBPvr.REAL_TIME_TASK_ID);
        
        return addBean;
    }
    
    @Override
    public void cancelTaskByIDS(List<Integer> ids)
    {
        if (null == ids)
        {
            LogUtils.e(TAG, "dao.impl.PvrDaoImpl.cancelTaskByIDS:: null == ids.. Didn't do anything here...");
            
            return;
        }
        
        if (context == null)
        {
            LogUtils.e(TAG,
                "dao.impl.PvrDaoImpl.cancelTaskByIDS:: The context is null in update_Record_Task of PvrDaoImpl.class");
            
            return;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG,
                "dao.impl.PvrDaoImpl.cancelTaskByIDS:: Can not get ContentResolver in update_Record_Task(PvrBean task)");
            
            return;
        }
        
        for (Integer id : ids)
        {
            if (null == id)
            {
                continue;
            }
            
            ContentValues values = new ContentValues();
            
            values.put(DBPvr.PVR_MODE, DBPvr.CLOSE_MODE);
            
            LogUtils.e(TAG, "dao.impl.PvrDaoImpl.cancelTaskByIDS:: close task pvrID = " + id);
            
            resolver.update(DBPvr.CONTENT_URI, values, DBPvr.PVR_ID + " =  ? ", new String[] {id + ""});
        }
        
        return;
    }
    
    public void removeAll()
    {
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.removeAll run...");
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return;
        }
        
        Cursor countCursor =
            resolver.query(DBPvr.CONTENT_URI, new String[] {"count(*)"}, DBPvr.PVR_ID + "!= -1", null, DBPvr.PVR_ID);
        
        int num = 0;
        
        if (null != countCursor && !countCursor.isClosed() && countCursor.moveToFirst())
        {
            num = countCursor.getInt(0);
            
            if (!countCursor.isClosed())
            {
                countCursor.close();
            }
        }
        
        if (num > (DBPvr.MAX_RECORD_ITEMS + 1))
        {
            resolver.delete(DBPvr.CONTENT_URI, null, null);
        }
        
        Cursor cursor = null;
        
        // ��ȡ��ݿ��Ѿ��е��������
        cursor =
            resolver.query(DBPvr.CONTENT_URI, new String[] {"count(*)"}, DBPvr.PVR_TYPE_TAG + "!="
                + DBPvr.TASK_EPG_START_WITHOUT_DIALOG, null, DBPvr.PVR_ID);
        
        int task_Count = 0;
        
        if (cursor != null)
        {
            if (cursor.moveToNext())
            {
                task_Count = cursor.getInt(0);
                
            }
            
            if (!cursor.isClosed())
            {
                cursor.close();
            }
            
            if (task_Count < DBPvr.MAX_RECORD_ITEMS)
            {
                PvrBean addBean = null;
                
                for (int i = 0; i < DBPvr.MAX_RECORD_ITEMS - task_Count; i++)
                {
                    addBean = new PvrBean();
                    
                    addBean.setPvrMode(DBPvr.CLOSE_MODE);
                    
                    add_Record_Task(addBean);
                }
            }
            else if (task_Count > DBPvr.MAX_RECORD_ITEMS)
            {
                Log.w(TAG, "In " + DBPvr.TABLE_PVR_NAME
                    + " table, the number of records are bigger than DBPvr.MAX_RECORD_ITEMS(" + DBPvr.MAX_RECORD_ITEMS
                    + ")");
            }
        }
        else
        {
            throw new RuntimeException(
                "In get_All_Illegal_Tasks() function: Get an closed cursor that can not be used.");
        }
        
        List<PvrBean> tasks = get_All_Illegal_Tasks();
        
        for (int i = 1; null != tasks && i <= tasks.size(); i++)
        {
            PvrBean task = tasks.get(i);
            
            if (task.getPvrId() != i)
            {
                ContentValues values = new ContentValues();
                
                values.put(DBPvr.PVR_ID, i);
                
                resolver.update(DBPvr.CONTENT_URI, values, DBPvr.PVR_ID + "=" + i, null);
            }
        }
        
        return;
    }
    
    @Override
    public List<Integer> checkTaskExists(int serviceId, long setDate, long lstart, long lstop)
    {
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: dao.impl.PvrDaoImpl.checkTaskExists run...");
        
        if (context == null)
        {
            LogUtils.e(TAG, "The context is null in add_Record_Task of PvrDaoImpl.class");
            
            return null;
        }
        
        ContentResolver resolver = context.getContentResolver();
        
        if (resolver == null)
        {
            LogUtils.e(TAG, "Can not get ContentResolver in get_First_Record_Task()");
            
            return null;
        }
        
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: serviceId = " + serviceId + " setDate = " + setDate
            + " lstart = " + lstart + " lend = " + lstop);
        
        String taskExistsCheckSQL =
            DBPvr.PVR_MODE + "!=" + DBPvr.CLOSE_MODE + " and " + DBPvr.PVR_SERVICE_ID + "=" + serviceId + " and "
                + DBPvr.PVR_START_TIME + "=" + lstart + " and ((" + DBPvr.PVR_WAKEUP_MODE + "="
                + DBPvr.RECORD_WAKEUP_MODE + " and " + DBPvr.PVR_RECORD_LENGTH + "=" + (lstop - lstart) + " ) OR ("
                + DBPvr.PVR_WAKEUP_MODE + "=" + DBPvr.PLAY_WAKEUP_MODE + " and " + DBPvr.PVR_RECORD_LENGTH + "=0"
                + ")) and " + DBPvr.PVR_TYPE_TAG + "!=" + DBPvr.TASK_EPG_START_WITHOUT_DIALOG;
        
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: taskExistsCheckSQL = " + taskExistsCheckSQL);
        
        Cursor cursor =
            resolver.query(DBPvr.CONTENT_URI,
                new String[] {DBPvr.PVR_ID, DBPvr.PVR_MODE, DBPvr.PVR_SET_DATE},
                taskExistsCheckSQL,
                null,
                DBPvr.PVR_ID);
        
        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists::  SQL SELECTION = " + DBPvr.PVR_MODE + "!="
            + DBPvr.CLOSE_MODE + " and " + DBPvr.PVR_SERVICE_ID + "=" + serviceId + " and " + DBPvr.PVR_START_TIME
            + "=" + lstart + " and " + DBPvr.PVR_RECORD_LENGTH + "=" + (lstop - lstart) + " and " + DBPvr.PVR_TYPE_TAG
            + "!=" + DBPvr.TASK_EPG_START_WITHOUT_DIALOG);
        
        List<Integer> ids = null;
        
        if (null != cursor && !cursor.isClosed())
        {
            LogUtils.e(TAG, " dao.impl.PvrDaoImpl.checkTaskExists:: getCount = " + cursor.getCount());
            
            ids = new ArrayList<Integer>();
            
            while (cursor.moveToNext())
            {
                int id = cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_ID));
                
                int pvrModeInDB = cursor.getInt(cursor.getColumnIndex(DBPvr.PVR_MODE));
                
                long pvrSetDateInDB = cursor.getLong(cursor.getColumnIndex(DBPvr.PVR_SET_DATE));
                
                LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: query id = " + id + " pvrModeInDB = "
                    + pvrModeInDB + " pvrSetDateInDB = " + pvrSetDateInDB);
                
                if (pvrModeInDB == DBPvr.DO_ONCE_MODE)
                {
                    // Check if it's the same date...
                    if (pvrSetDateInDB != setDate)
                    {
                        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: pvrSetDate != setDate");
                        
                        continue;
                    }
                }
                else if (pvrModeInDB == DBPvr.EVERY_WEEK_MODE)
                {
                    // Check if the week day is the same...
                    Calendar cal = Calendar.getInstance();
                    
                    cal.setTimeInMillis(pvrSetDateInDB);
                    int weekDayInDB = cal.get(Calendar.DAY_OF_WEEK);
                    
                    cal.setTimeInMillis(setDate);
                    int weekDayPassIn = cal.get(Calendar.DAY_OF_WEEK);
                    
                    if (weekDayInDB != weekDayPassIn)
                    {
                        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: weekDay < 1 && weekDay > 5");
                        
                        continue;
                    }
                }
                else if (pvrModeInDB == DBPvr.EVERY_WORKDAY_MODE)
                {
                    // Check if the week day is between Monday and Friday...
                    
                    Calendar cal = Calendar.getInstance();
                    
                    cal.setTimeInMillis(setDate);
                    
                    int weekDay = cal.get(Calendar.DAY_OF_WEEK);
                    
                    if (weekDay < 1 && weekDay > 5)
                    {
                        LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: weekDay < 1 && weekDay > 5");
                        
                        continue;
                    }
                }
                
                ids.add(id);
            }
            
            cursor.close();
        }
        else
        {
            LogUtils.e(TAG, "dao.impl.PvrDaoImpl.checkTaskExists:: null == cursor || cursor.isClosed()");
        }
        
        return ids;
    }
}