/*
 * �� �� ��:  PvrBean.java
 * ��    Ȩ:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  gtsong
 * �޸�ʱ��:  2012-10-29
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.domain;

import java.util.Calendar;
import java.util.Date;

import android.content.Context;
import android.util.Log;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

/**
 * <һ�仰���ܼ���> <������ϸ����>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-10-29]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class PvrBean
{
    
    private static final String TAG = Config.DVB_TAG;
    
    private Integer pvrId;
    
    private Integer pvrMode;
    
    private Integer pvrWakeMode;
    
    private Integer serviceId;
    
    private Integer logicalNumber;
    
    private Long pvrSetDate;
    
    private Long pvrStartTime;
    
    private Long pvrRecordLength;
    
    private Integer pvrWeekDate;
    
    private Integer pvr_pmt_pid;
    
    private Integer pvr_is_sleep;
    
    private Integer pvr_type_tag;
    
    private ServiceInfoBean serviceBean = null;
    
    /*
     * Commented out by LinKang at 2012-12-05
     */
    // private Integer pvrAVId;
    
    public PvrBean()
    {
        super();
        
        LogUtils.e(TAG, "domain.PvrBean:: run...");
        
        pvrMode = DBPvr.CLOSE_MODE;
        
        pvrWakeMode = DBPvr.RECORD_WAKEUP_MODE;
        
        serviceId = 0;
        
        logicalNumber = 0;
        
        pvrSetDate = 0l;
        
        pvrStartTime = 0l;
        
        pvrRecordLength = 0l;
        
        pvrWeekDate = 0;
        
        pvr_pmt_pid = 0;
        
        pvr_is_sleep = DBPvr.SUSPEND_AFTER_RECORD;
        
        pvr_type_tag = DBPvr.TASK_DEFAULT_START_WITH_DIALOG;
    }
    
    /**
     * »ñÈ¡ pvrId
     * 
     * @return ·µ»Ø pvrId
     */
    public Integer getPvrId()
    {
        return pvrId;
    }
    
    /**
     * ÉèÖÃ pvrId
     * 
     * @param ¶ÔpvrId½øÐÐ¸³Öµ
     */
    public void setPvrId(Integer pvrId)
    {
        this.pvrId = pvrId;
    }
    
    /**
     * »ñÈ¡ pvrMode
     * 
     * @return ·µ»Ø pvrMode
     */
    public Integer getPvrMode()
    {
        return pvrMode;
    }
    
    /**
     * ÉèÖÃ pvrMode
     * 
     * @param ¶ÔpvrMode½øÐÐ¸³Öµ
     */
    public void setPvrMode(Integer pvrMode)
    {
        this.pvrMode = pvrMode;
    }
    
    /**
     * »ñÈ¡ pvrWakeMode
     * 
     * @return ·µ»Ø pvrWakeMode
     */
    public Integer getPvrWakeMode()
    {
        return pvrWakeMode;
    }
    
    /**
     * ÉèÖÃ pvrWakeMode
     * 
     * @param ¶ÔpvrWakeMode½øÐÐ¸³Öµ
     */
    public void setPvrWakeMode(Integer pvrWakeMode)
    {
        this.pvrWakeMode = pvrWakeMode;
    }
    
    /**
     * »ñÈ¡ serviceId
     * 
     * @return ·µ»Ø serviceId
     */
    public Integer getServiceId()
    {
        return serviceId;
    }
    
    /**
     * ÉèÖÃ serviceId
     * 
     * @param ¶ÔserviceId½øÐÐ¸³Öµ
     */
    public void setServiceId(Integer serviceId)
    {
        this.serviceId = serviceId;
    }
    
    /**
     * »ñÈ¡ logicalNumber
     * 
     * @return ·µ»Ø logicalNumber
     */
    public Integer getLogicalNumber()
    {
        return logicalNumber;
    }
    
    /**
     * ÉèÖÃ logicalNumber
     * 
     * @param ¶ÔlogicalNumber½øÐÐ¸³Öµ
     */
    public void setLogicalNumber(Integer logicalNumber)
    {
        this.logicalNumber = logicalNumber;
    }
    
    /**
     * »ñÈ¡ pvrSetDate
     * 
     * @return ·µ»Ø pvrSetDate
     */
    public Long getPvrSetDate()
    {
        return pvrSetDate;
    }
    
    /**
     * ÉèÖÃ pvrSetDate
     * 
     * @param ¶ÔpvrSetDate½øÐÐ¸³Öµ
     */
    public void setPvrSetDate(Long pvrSetDate)
    {
        this.pvrSetDate = pvrSetDate;
    }
    
    /**
     * »ñÈ¡ pvrStartTime
     * 
     * @return ·µ»Ø pvrStartTime
     */
    public Long getPvrStartTime()
    {
        return pvrStartTime;
    }
    
    /**
     * ÉèÖÃ pvrStartTime
     * 
     * @param ¶ÔpvrStartTime½øÐÐ¸³Öµ
     */
    public void setPvrStartTime(Long pvrStartTime)
    {
        this.pvrStartTime = pvrStartTime;
    }
    
    /**
     * »ñÈ¡ pvrRecordLength
     * 
     * @return ·µ»Ø pvrRecordLength
     */
    public Long getPvrRecordLength()
    {
        return pvrRecordLength;
    }
    
    /**
     * ÉèÖÃ pvrRecordLength
     * 
     * @param ¶ÔpvrRecordLength½øÐÐ¸³Öµ
     */
    public void setPvrRecordLength(Long pvrRecordLength)
    {
        this.pvrRecordLength = pvrRecordLength;
    }
    
    /**
     * »ñÈ¡ pvrWeekDate
     * 
     * @return ·µ»Ø pvrWeekDate
     */
    public Integer getPvrWeekDate()
    {
        return pvrWeekDate;
    }
    
    /**
     * ÉèÖÃ pvrWeekDate
     * 
     * @param ¶ÔpvrWeekDate½øÐÐ¸³Öµ
     */
    public void setPvrWeekDate(Integer pvrWeekDate)
    {
        this.pvrWeekDate = pvrWeekDate;
    }
    
    public Integer getPvr_pmt_pid()
    {
        return pvr_pmt_pid;
    }
    
    public void setPvr_pmt_pid(Integer pvr_pmt_pid)
    {
        this.pvr_pmt_pid = pvr_pmt_pid;
    }
    
    public Integer getPvr_is_sleep()
    {
        return pvr_is_sleep;
    }
    
    public void setPvr_is_sleep(Integer pvr_is_sleep)
    {
        this.pvr_is_sleep = pvr_is_sleep;
    }
    
    public Integer getPvr_type_tag()
    {
        return pvr_type_tag;
    }
    
    public void setPvr_type_tag(Integer pvr_type_tag)
    {
        this.pvr_type_tag = pvr_type_tag;
    }
    
    public long get_Adjust_StartTime()
    {
        
        Calendar current = Calendar.getInstance();
        
        long current_Time = current.getTimeInMillis();
        
        current.set(Calendar.HOUR_OF_DAY, 0);
        current.set(Calendar.MINUTE, 0);
        current.set(Calendar.SECOND, 0);
        current.set(Calendar.MILLISECOND, 0);
        
        long currentDate = current.getTimeInMillis();
        current_Time = current_Time - currentDate;
        int week_Of_Today = current.get(Calendar.DAY_OF_WEEK);
        
        long interval_Time = 0;
        
        int pvrWeekDate = 0;
        
        interval_Time = pvrStartTime - current_Time;
        
        LogUtils.e(TAG, "******** ******** ******** ******** ");
        LogUtils.e(TAG, "currentDate = " + currentDate);
        LogUtils.e(TAG, "pvrStartTime = " + pvrStartTime + " ... current_Time = " + current_Time);
        LogUtils.e(TAG, "interval_Time111 = " + interval_Time);
        
        week_Of_Today = swap_WeekDay(week_Of_Today);
        pvrWeekDate = swap_WeekDay(this.pvrWeekDate);
        
        // »¹Ã»¿ªÊ¼²¥
        long week_Day_Interval;
        
        switch (pvrMode)
        {
        
            case DBPvr.DO_ONCE_MODE:
                
                if (pvrSetDate >= currentDate)
                {
                    interval_Time = interval_Time + (pvrSetDate - currentDate);
                    
                    if (interval_Time + pvrRecordLength < -100)
                    {
                        
                        interval_Time = Long.MAX_VALUE;
                        
                    }
                    
                }
                else if (pvrSetDate < currentDate)
                {
                    
                    interval_Time = Long.MAX_VALUE;
                    
                }
                
                break;
            
            case DBPvr.EVERY_DAY_MODE:
                
                if (interval_Time + pvrRecordLength < -100)
                {
                    
                    interval_Time = interval_Time + 24 * 60 * 60 * 1000;
                    
                    LogUtils.e(TAG, "interval_Time222 = " + interval_Time);
                    
                }
                
                break;
            
            case DBPvr.EVERY_WEEK_MODE:
                
                if (pvrWeekDate >= week_Of_Today)
                {
                    
                    week_Day_Interval = pvrWeekDate - week_Of_Today;
                    
                    if (week_Day_Interval == 0 && (interval_Time + pvrRecordLength) < -100)
                    {
                        
                        week_Day_Interval = 7;
                        
                    }
                    
                    LogUtils.e(TAG, "pvrWeekDate = " + pvrWeekDate + " week_Of_Today = " + week_Of_Today);
                    LogUtils.e(TAG, "week_Day_Interval111 = " + interval_Time);
                    
                }
                else
                {
                    
                    week_Day_Interval = pvrWeekDate + 7 - week_Of_Today;
                    
                    LogUtils.e(TAG, "week_Day_Interval222 = " + interval_Time);
                    
                }
                
                if (week_Day_Interval > Long.MAX_VALUE / (24 * 60 * 60 * 1000))
                {
                    
                    week_Day_Interval = Long.MAX_VALUE / (24 * 60 * 60 * 1000);
                    
                    LogUtils.e(TAG, "week_Day_Interval333 = " + interval_Time);
                    
                }
                
                interval_Time = interval_Time + 24 * 60 * 60 * 1000 * week_Day_Interval;
                
                LogUtils.e(TAG, "interval_Time333 = " + interval_Time);
                
                break;
            
            case DBPvr.EVERY_WORKDAY_MODE:
                
                // Ã¿¹¤×÷ÈÕÖØ¸´
                if (week_Of_Today == 6)
                {
                    
                    week_Day_Interval = 2;
                    
                    LogUtils.e(TAG, "week_Day_Interval444 = " + week_Day_Interval);
                    
                }
                else if (week_Of_Today == 7)
                {
                    
                    week_Day_Interval = 1;
                    
                    LogUtils.e(TAG, "week_Day_Interval555 = " + week_Day_Interval);
                    
                }
                else
                {
                    
                    // week_Day_Interval = pvrWeekDate - week_Of_Today;
                    
                    week_Day_Interval = 0;
                    
                }
                
                if (week_Day_Interval == 0 && (interval_Time + pvrRecordLength) < -100)
                {
                    
                    week_Day_Interval += 1;
                    
                }
                
                if (week_Day_Interval > Long.MAX_VALUE / (24 * 60 * 60 * 1000))
                {
                    
                    week_Day_Interval = Long.MAX_VALUE / (24 * 60 * 60 * 1000);
                    
                    LogUtils.e(TAG, "week_Day_Interval777 = " + interval_Time);
                    
                }
                
                interval_Time = interval_Time + 24 * 60 * 60 * 1000 * week_Day_Interval;
                
                LogUtils.e(TAG, "interval_Time555 = " + interval_Time);
                
                break;
        }
        
        LogUtils.e(TAG, "before return from adjust time : interval_Time = " + interval_Time);
        
        return interval_Time;
    }
    
    public long get_Adjust_StartTime(long current_Date, long current_Time, int week_Of_Today)
    {
        
        long interval_Time = 0;
        
        int pvrWeekDate = 0;
        
        interval_Time = pvrStartTime - current_Time;
        
        LogUtils.e(TAG, "******** ******** ******** ******** ");
        LogUtils.e(TAG, "currentDate = " + current_Date);
        LogUtils.e(TAG, "pvrStartTime = " + pvrStartTime + " ... current_Time = " + current_Time);
        LogUtils.e(TAG, "interval_Time111... = " + interval_Time);
        
        week_Of_Today = swap_WeekDay(week_Of_Today);
        pvrWeekDate = swap_WeekDay(this.pvrWeekDate);
        
        // »¹Ã»¿ªÊ¼²¥
        long week_Day_Interval;
        
        switch (pvrMode)
        {
            case DBPvr.DO_ONCE_MODE:
            {
                if (pvrSetDate >= current_Date)
                {
                    interval_Time = interval_Time + (pvrSetDate - current_Date);
                    
                    if (interval_Time + pvrRecordLength < -100)
                    {
                        interval_Time = Long.MAX_VALUE;
                        
                    }
                    
                }
                else if (pvrSetDate < current_Date)
                {
                    interval_Time = Long.MAX_VALUE;
                }
                
                break;
            }
            
            case DBPvr.EVERY_DAY_MODE:
            {
                if (interval_Time + pvrRecordLength < -100)
                {
                    
                    interval_Time = interval_Time + 24 * 60 * 60 * 1000;
                    
                    LogUtils.e(TAG, "interval_Time222 = " + interval_Time);
                    
                }
                
                break;
            }
            
            case DBPvr.EVERY_WEEK_MODE:
            {
                if (pvrWeekDate >= week_Of_Today)
                {
                    
                    week_Day_Interval = pvrWeekDate - week_Of_Today;
                    
                    if (week_Day_Interval == 0 && (interval_Time + pvrRecordLength) < -100)
                    {
                        
                        week_Day_Interval = 7;
                        
                    }
                    
                    LogUtils.e(TAG, "pvrWeekDate = " + pvrWeekDate + " week_Of_Today = " + week_Of_Today);
                    LogUtils.e(TAG, "week_Day_Interval111 = " + interval_Time);
                    
                }
                else
                {
                    
                    week_Day_Interval = pvrWeekDate + 7 - week_Of_Today;
                    
                    LogUtils.e(TAG, "week_Day_Interval222 = " + interval_Time);
                    
                }
                
                if (week_Day_Interval > Long.MAX_VALUE / (24 * 60 * 60 * 1000))
                {
                    
                    week_Day_Interval = Long.MAX_VALUE / (24 * 60 * 60 * 1000);
                    
                    LogUtils.e(TAG, "week_Day_Interval333 = " + interval_Time);
                    
                }
                
                interval_Time = interval_Time + 24 * 60 * 60 * 1000 * week_Day_Interval;
                
                LogUtils.e(TAG, "interval_Time333 = " + interval_Time);
                
                break;
            }
            
            case DBPvr.EVERY_WORKDAY_MODE:
            {
                // Ã¿¹¤×÷ÈÕÖØ¸´
                if (week_Of_Today == 6)
                {
                    
                    week_Day_Interval = 2;
                    
                    LogUtils.e(TAG, "week_Day_Interval444 = " + week_Day_Interval);
                    
                }
                else if (week_Of_Today == 7)
                {
                    
                    week_Day_Interval = 1;
                    
                    LogUtils.e(TAG, "week_Day_Interval555 = " + week_Day_Interval);
                    
                }
                else
                {
                    
                    // week_Day_Interval = pvrWeekDate - week_Of_Today;
                    
                    week_Day_Interval = 0;
                    
                }
                
                if (week_Day_Interval == 0 && (interval_Time + pvrRecordLength) < -100)
                {
                    
                    week_Day_Interval += 1;
                    
                }
                
                if (week_Day_Interval > Long.MAX_VALUE / (24 * 60 * 60 * 1000))
                {
                    
                    week_Day_Interval = Long.MAX_VALUE / (24 * 60 * 60 * 1000);
                    
                    LogUtils.e(TAG, "week_Day_Interval777 = " + interval_Time);
                    
                }
                
                interval_Time = interval_Time + 24 * 60 * 60 * 1000 * week_Day_Interval;
                
                LogUtils.e(TAG, "interval_Time555 = " + interval_Time);
                
                break;
            }
        }
        
        LogUtils.e(TAG, "before return from adjust time : interval_Time = " + interval_Time);
        
        return interval_Time;
    }
    
    private int swap_WeekDay(int week_Of_Today)
    {
        
        int result = 0;
        
        LogUtils.e(TAG, "week_of_today = " + week_Of_Today);
        
        switch (week_Of_Today)
        {
            case 1:
                
                result = 7;
                
                break;
            
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                
                result = week_Of_Today - 1;
                
                break;
            
            default:
                
                LogUtils.e(TAG, "Unexpected value of DATE_OF_WEEK");
                
                break;
        }
        
        LogUtils.e(TAG, "Adusted week_of_today = " + result);
        
        return result;
    }
    
    public long get_Adjust_Next_StartTime(long current_Date, long current_Time, int week_Of_Today)
    {
        
        long interval_Time = get_Adjust_StartTime(current_Date, current_Time, week_Of_Today);
        
        switch (pvrMode)
        {
        
            case DBPvr.DO_ONCE_MODE:
                
                interval_Time = Long.MAX_VALUE;
                
                break;
            
            case DBPvr.EVERY_DAY_MODE:
                
                interval_Time += 24 * 60 * 60 * 1000;
                
                LogUtils.e(TAG, "Next_interval_Time222 = " + interval_Time);
                
                break;
            
            case DBPvr.EVERY_WEEK_MODE:
                
                interval_Time += 24 * 60 * 60 * 1000 * 7;
                
                break;
            
            case DBPvr.EVERY_WORKDAY_MODE:
                
                Calendar cal = Calendar.getInstance();
                
                cal.setTime(new Date(interval_Time));
                
                int week = cal.get(Calendar.DAY_OF_WEEK);
                
                week = swap_WeekDay(week);
                
                int day_Interval = 0;
                
                if (week == 5)
                {
                    
                    day_Interval = 3;
                    
                }
                else if (week == 6)
                {
                    
                    day_Interval = 2;
                    
                }
                else
                {
                    
                    day_Interval = 1;
                    
                }
                
                interval_Time += 24 * 60 * 60 * 1000 * day_Interval;
                
                break;
        }
        
        return interval_Time;
    }
    
    public long get_Adjust_Next_StartTime(long current_Date, long current_Time, int week_Of_Today, int skipCount)
    {
        
        long interval_Time = get_Adjust_StartTime(current_Date, current_Time, week_Of_Today);
        
        switch (pvrMode)
        {
        
            case DBPvr.DO_ONCE_MODE:
                
                interval_Time = Long.MAX_VALUE;
                
                break;
            
            case DBPvr.EVERY_DAY_MODE:
                
                interval_Time += 24 * 60 * 60 * 1000 * skipCount;
                
                LogUtils.e(TAG, "Next_interval_Time222 = " + interval_Time);
                
                break;
            
            case DBPvr.EVERY_WEEK_MODE:
                
                interval_Time += 24 * 60 * 60 * 1000 * 7 * skipCount;
                
                break;
            
            case DBPvr.EVERY_WORKDAY_MODE:
                
                Calendar cal = Calendar.getInstance();
                
                cal.setTime(new Date(interval_Time));
                
                int week = cal.get(Calendar.DAY_OF_WEEK);
                
                week = swap_WeekDay(week);
                
                int day_Interval = 0;
                
                if (week == 6)
                {
                    
                    LogUtils.e(TAG,
                        "---->>>> Staturday is not a day for Weekday Mode... It's not expected to be executed here <<<<----");
                    
                    day_Interval = 2;
                    
                }
                else if (week == 7)
                {
                    
                    LogUtils.e(TAG,
                        "---->>>> Sunday is not a day for Weekday Mode... It's not expected to be executed here <<<<----");
                    
                    day_Interval = 1;
                    
                }
                
                if (6 - week > skipCount)
                {
                    
                    day_Interval += skipCount;
                    
                }
                else
                {
                    
                    day_Interval = day_Interval + (6 - week + 2);
                    
                    skipCount = skipCount - (6 - week - 1);
                    
                    while (skipCount / 5 > 0)
                    {
                        
                        day_Interval += 7;
                        
                        skipCount = skipCount - 5;
                        
                    }
                    
                    day_Interval += skipCount;
                    
                }
                
                interval_Time = interval_Time + 24 * 60 * 60 * 1000 * day_Interval;
                
                break;
        }
        
        return interval_Time;
    }
    
    public ServiceInfoBean getServiceBean()
    {
        return serviceBean;
    }
    
    public void setServiceBean(ServiceInfoBean serviceBean)
    {
        this.serviceBean = serviceBean;
    }
    
    public void update(Context context)
    {
        
        PvrDao pvrDao = new PvrDaoImpl(context);
        
        pvrDao.update_Record_Task(this);
    }
    
    public boolean checkTimeExpired()
    {
        
        boolean result = false;
        
        Calendar midNight = Calendar.getInstance();
        
        long currentTime = midNight.getTimeInMillis();
        
        midNight.set(Calendar.HOUR_OF_DAY, 0);
        midNight.set(Calendar.MINUTE, 0);
        midNight.set(Calendar.SECOND, 0);
        midNight.set(Calendar.MILLISECOND, 0);
        
        currentTime = currentTime - midNight.getTimeInMillis();
        
        if (getPvrMode() == DBPvr.DO_ONCE_MODE && getPvrStartTime() < currentTime
            && getPvrSetDate() == midNight.getTimeInMillis())
        {
            
            result = true;
            
        }
        
        return result;
    }
    
    public boolean checkTaskConfict(Context context)
    {
        // Return ture means the task has no conflict with the items in database
        // Return false means that you can not insert this bean into database
        
        Log.e(TAG, "pvrBean = " + this.toString());
        
        boolean result = false;
        
        PvrDao pvrDao = new PvrDaoImpl(context);
        
        int conflict = pvrDao.get_Count_Of_Conflict_Task(this);
        
        Log.e(TAG, "There are total " + conflict + " conflict tasks");
        
        if (conflict == 0)
        {
            
            result = true;
            
        }
        
        return result;
    }
    
    @Override
    public String toString()
    {
        return "PvrBean [pvrId=" + pvrId + ", pvrMode=" + pvrMode + ", pvrWakeMode=" + pvrWakeMode + ", serviceId="
            + serviceId + ", logicalNumber=" + logicalNumber + ", pvrSetDate=" + pvrSetDate + ", pvrStartTime="
            + pvrStartTime + ", pvrRecordLength=" + pvrRecordLength + ", pvrWeekDate=" + pvrWeekDate + ", pvr_pmt_pid="
            + pvr_pmt_pid + ", pvr_is_sleep=" + pvr_is_sleep + ", pvr_type_tag=" + pvr_type_tag + ", serviceBean="
            + serviceBean + "]";
    }
    
}
