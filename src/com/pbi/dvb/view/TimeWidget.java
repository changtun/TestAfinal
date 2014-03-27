package com.pbi.dvb.view;

import java.util.Calendar;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.database.ContentObserver;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.TextView;

import com.pbi.dvb.R;

/**
 * <自定义DigitalClock控件>
 * @author gtsong
 * @version [版本号, 2012-5-15]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class TimeWidget extends TextView
{
    
    Calendar mCalendar;
    
    private final static String m12 = "h:mm:ss";
    private final static String m24 = "k:mm:ss";

    private FormatChangeObserver mFormatChangeObserver;
    
    private Runnable mTicker;
    private Handler mHandler;
    
    private boolean mTickerStopped = false;
    
    String mFormat;
    
    private String[] weekdays;
    
    public TimeWidget(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        initClock(context);
    }
    
    public TimeWidget(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        initClock(context);
    }
    
    public TimeWidget(Context context)
    {
        super(context);
        initClock(context);
    }
    
    private void initClock(Context context)
    {
        Resources r = context.getResources();
        weekdays =
            new String[] {r.getString(R.string.Sun), r.getString(R.string.Mon), r.getString(R.string.Tue),
                r.getString(R.string.Wed), r.getString(R.string.Thr), r.getString(R.string.Fri),
                r.getString(R.string.Sat)};
        if (null ==mCalendar)
        {
            mCalendar = Calendar.getInstance();
        }
        
        mFormatChangeObserver = new FormatChangeObserver();
        // 注册，监听系统日期设置数据库的改变
        getContext().getContentResolver().registerContentObserver(Settings.System.CONTENT_URI, true,mFormatChangeObserver);
        setFormat();
    }
    
    
    protected void onDetachedFromWindow()
    {
        super.onDetachedFromWindow();
        mTickerStopped = true;
    }
    
    private boolean get24HourMode()
    {
        return android.text.format.DateFormat.is24HourFormat(getContext());
    }
    
    private void setFormat()
    {
        if (get24HourMode())
        {
            mFormat = m24;
        }
        else
        {
            mFormat = m12;
        }
    }
    
    /**
     * <格式化时间字符串>
     * @param t
     * @return
     * @see [类、类#方法、类#成员]
     */
    private static String format(int t)
    {
        String s = "" + t;
        if (s.length() == 1)
        {
            s = "0" + s;
        }
        return s;
    }
    
    
    public boolean onTouchEvent(MotionEvent event)
    {
        getContext().startActivity(new Intent(android.provider.Settings.ACTION_DATE_SETTINGS));
        return super.onTouchEvent(event);
    }
    
    protected void onAttachedToWindow()
    {
        mTickerStopped = false;
        super.onAttachedToWindow();
        mHandler = new Handler();
        
        /**
         * requests a tick on the next hard-second boundary
         */
        mTicker = new Runnable()
        {
            public void run()
            {
                if (mTickerStopped)
                    return;
                mCalendar.setTimeInMillis(System.currentTimeMillis());
                
                int myear = (mCalendar.get(Calendar.YEAR));
                int mmonth = (mCalendar.get(Calendar.MONTH) + 1);// 月份+1是一年中的第几个月
                int mmonthday = (mCalendar.get(Calendar.DAY_OF_MONTH));// 一月中的日期
                final int mweekday = (mCalendar.get(Calendar.DAY_OF_WEEK)) - 1;
                
                final String mDate = format(myear) + "-" + format(mmonth) + "-" + format(mmonthday);
                
//                setText(mDate + " " + weekdays[mweekday]+"   "+DateFormat.format(mFormat, mCalendar));
                setText(mDate + " " + weekdays[mweekday]);
                invalidate();
                long now = SystemClock.uptimeMillis();
                long next = now + (1000 - now % 1000);
                mHandler.postAtTime(mTicker, next);
            }
        };
        mTicker.run();
    }
    
    private class FormatChangeObserver extends ContentObserver
    {
        public FormatChangeObserver()
        {
            super(new Handler());
        }
        
        public void onChange(boolean selfChange)
        {
            setFormat();
        }
    }
    
}