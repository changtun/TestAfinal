package com.pbi.dvb.view;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.database.ContentObserver;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.Settings;
import android.text.format.DateFormat;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.NativeEpg;

public class ClockWidget extends TextView {

	Calendar mCalendar;

	private final static String m12 = "h:mm:ss aa";
	private final static String m24 = "k:mm:ss";

	private FormatChangeObserver mFormatChangeObserver;

	private Runnable mTicker;
	private Handler mHandler;

	private boolean mTickerStopped = false;

	String mFormat;

	private String[] weekdays;

	// ���粻����ʱ����ʾDVBʱ��
	private Context context;
	// private EthernetManager ethManager;
	private long timeAsHistory;
	private long timeAsCurrent;
	private NativeEpg nativeEpg;
	private NativeEpg.EpgTime epgTime;

	private String TAG = "ClockWidget";

	public ClockWidget(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		initClock(context);
	}

	public ClockWidget(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		initClock(context);
	}

	public ClockWidget(Context context) {
		super(context);
		this.context = context;
		initClock(context);
	}

	private void initClock(Context context) {
		Resources r = context.getResources();
		weekdays = new String[] { r.getString(R.string.Sun),
				r.getString(R.string.Mon), r.getString(R.string.Tue),
				r.getString(R.string.Wed), r.getString(R.string.Thr),
				r.getString(R.string.Fri), r.getString(R.string.Sat) };
		if (null == mCalendar) {
			mCalendar = Calendar.getInstance();
		}

		nativeEpg = new NativeEpg();
		epgTime = nativeEpg.new EpgTime();
		timeAsHistory = Long.valueOf("1350611755000");// 2012-10-19 9:55:55
		// Log.i(TAG, "timeAsHistory = " + timeAsHistory);

		// SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		// Calendar ca = Calendar.getInstance();
		// int year = ca.get(Calendar.YEAR); // ��ȡ���
		// int month = ca.get(Calendar.MONTH) + 1; // ��ȡ�·�
		// int day = ca.get(Calendar.DATE); // ��ȡ��
		// int hour = ca.get(Calendar.HOUR_OF_DAY);// Сʱ(24Сʱ��)
		// int minute = ca.get(Calendar.MINUTE); // ��
		// int second = ca.get(Calendar.SECOND); // ��
		// String compare_date = year + "-" + month + "-" + day + " " + hour +
		// ":" + minute + ":" + second;
		// Log.i(TAG, "compare_date = " + compare_date);
		// try {
		// timeAsHistory = df.parse(compare_date).getTime();
		// Log.i(TAG, "timeAsHistory = " + timeAsHistory);
		// } catch (Exception e) {
		// e.printStackTrace();
		// }

		mFormatChangeObserver = new FormatChangeObserver();
		// ע�ᣬ����ϵͳ�����������ݿ�ĸı�
		getContext().getContentResolver().registerContentObserver(
				Settings.System.CONTENT_URI, true, mFormatChangeObserver);
		setFormat();
	}

	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		mTickerStopped = true;
	}

	private boolean get24HourMode() {
		return android.text.format.DateFormat.is24HourFormat(getContext());
	}

	private void setFormat() {
		if (get24HourMode()) {
			mFormat = m12;
		} else {
			mFormat = m24;
		}
	}

	/**
	 * <��ʽ��ʱ���ַ���>
	 * 
	 * @param t
	 * @return
	 * @see [�ࡢ��#��������#��Ա]
	 */
	private static String format(int t) {
		String s = "" + t;
		if (s.length() == 1) {
			s = "0" + s;
		}
		return s;
	}

	public boolean onTouchEvent(MotionEvent event) {
		getContext().startActivity(new Intent(android.provider.Settings.ACTION_DATE_SETTINGS));
		return super.onTouchEvent(event);
	}

	protected void onAttachedToWindow() {
		mTickerStopped = false;
		super.onAttachedToWindow();
		mHandler = new Handler();

		/**
		 * requests a tick on the next hard-second boundary
		 */
		mTicker = new Runnable() {
			public void run() {
				if (mTickerStopped)
					return;

				// ȡϵͳ��ǰʱ��
				SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
				mCalendar.setTimeInMillis(System.currentTimeMillis());
				int year = mCalendar.get(Calendar.YEAR); // ��ȡ���
				int month = mCalendar.get(Calendar.MONTH) + 1; // ��ȡ�·�
				int day = mCalendar.get(Calendar.DATE); // ��ȡ��
				int hour = mCalendar.get(Calendar.HOUR_OF_DAY); // Сʱ(24Сʱ��)
				int minute = mCalendar.get(Calendar.MINUTE); // ��
				int second = mCalendar.get(Calendar.SECOND); // ��
				final int mweekday = (mCalendar.get(Calendar.DAY_OF_WEEK)) - 1;

				try {
					String cur_date = year + "-" + month + "-" + day + " "+ hour + ":" + minute + ":" + second;
					// Log.i(TAG, "cur_date = " + cur_date);
					timeAsCurrent = df.parse(cur_date).getTime();
					// Log.i(TAG, "timeAsCurrent = " + timeAsCurrent);
				} catch (Exception e) {
					e.printStackTrace();
				}

				// �ж�ϵͳ��ǰʱ���Ƿ���������
				if (timeAsCurrent > timeAsHistory) {// ��ȡ��������ʱ�䣬��ʾ����
					mCalendar.setTimeInMillis(System.currentTimeMillis());
					setText(DateFormat.format(mFormat, mCalendar));
					invalidate();
					long now = SystemClock.uptimeMillis();
					long next = now + (1000 - now % 1000);
					mHandler.postAtTime(mTicker, next);
				} else {// ���粻���ã���ȡDVBʱ�䣬����ʾ
					nativeEpg.GetUtcTime(epgTime);
					int epgYear = epgTime.getutcyear();
					int epgMonth = epgTime.getutcmonth();
					int epgDay = epgTime.getutcdate();
					int epgHour = epgTime.getutchour();
					int epgMinute = epgTime.getutcminute();
					int epgSecond = epgTime.getutcsecond();

					long dvb_time = new Date(epgYear-1900, epgMonth-1, epgDay, epgHour, epgMinute, epgSecond).getTime();
					setText(DateFormat.format(mFormat, dvb_time));

					invalidate();
//					long now = SystemClock.uptimeMillis();
//					long next = now + (1000 - now % 1000);
					long next = dvb_time + (1000 - dvb_time % 1000);
					mHandler.postAtTime(mTicker, next);
				}

			}
		};
		mTicker.run();
	}

	public String getWeekday(int year, int month, int day) {
		int a = (14 - month) / 12;
		int y = year - a;
		int m = month + 12 * a - 2;
		int d = (day + y + y / 4 - y / 100 + y / 400 + 31 * m / 12) % 7;
		return weekdays[d];
	}

//	public boolean isNetworkExist() {
//	ethManager = (EthernetManager) this.context.getSystemService(Context.ETH_SERVICE);
//	if (ethManager.getEthState() == EthernetManager.ETH_STATE_ENABLED) {
//		return true;
//	}
//	return false;
//}

	private class FormatChangeObserver extends ContentObserver {
		public FormatChangeObserver() {
			super(new Handler());
		}

		public void onChange(boolean selfChange) {
			setFormat();
		}
	}

}
