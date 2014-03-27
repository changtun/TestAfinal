package com.pbi.dvb.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.content.Context;

import com.pbi.dvb.R;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.domain.ServiceInfoBean;

public class PvrUtils {

	/**
	 * 预约类型：关、一次、一周、每周、每工作日
	 * 0 关闭，1一次, 2 每天 ,3 每周, 4 工作日
	 */
	public static String[] getModeArray(Context context) {
		String[] modeArray = new String[] {
				context.getResources().getString(R.string.mode_off),
				context.getResources().getString(R.string.mode_onetime),
				context.getResources().getString(R.string.mode_perday),
				context.getResources().getString(R.string.mode_perweek),
				context.getResources().getString(R.string.mode_wrokingday) };
		return modeArray;
	}

	public static String getNameByModeId(Context context, int id) {
		String[] modeArray = getModeArray(context);
		if(id<0|| id>modeArray.length){
			return modeArray[0];
		}
		return modeArray[id];

	}

	public static int getIdByModeName(Context context, String name) {
		String modeArray[] = getModeArray(context);
		for (int i = 0; i < modeArray.length; i++) {
			if (name.equals(modeArray[i])) {
				return i;
			}
		}
		return -1;

	}

	/**
	 *  預約結束后待機：否、是
	 *  0是，1否
	 */
	public static String[] getStandbyArray(Context context) {
		String[] StandbyArray = new String[] {
				context.getResources().getString(R.string.standby_yes),
				context.getResources().getString(R.string.standby_no) };
		return StandbyArray;
	}

	public static String getNameByStandbyId(Context context, int id) {
		return getStandbyArray(context)[id];

	}

	public static int getIdByStandbyName(Context context, String name) {
		String StandbyArray[] = getStandbyArray(context);
		for (int i = 0; i < StandbyArray.length; i++) {
			if (name.equals(StandbyArray[i])) {
				return i;
			}
		}
		return -1;
	}

	/**
	 *  唤醒模式：录影、播放
	 *  0录影，1播放
	 */
	public static String[] getWakeupArray(Context context) {
		String[] wakeupArray = new String[] {
				context.getResources().getString(R.string.wake_record),
				context.getResources().getString(R.string.wake_play) };
		return wakeupArray;
	}

	public static String getNameByWakeupId(Context context, int id) {
		return getWakeupArray(context)[id];

	}

	public static int getIdByWakeupName(Context context, String name) {
		String wakeupArray[] = getWakeupArray(context);
		for (int i = 0; i < wakeupArray.length; i++) {
			if (name.equals(wakeupArray[i])) {
				return i;
			}
		}
		return -1;

	}
	
	
	public static String[] getWeekdateArray(Context context) {
		String[] weekName = new String[]{ 
				context.getResources().getString(R.string.Sun),
				context.getResources().getString(R.string.Mon),
				context.getResources().getString(R.string.Tue),
				context.getResources().getString(R.string.Wed),
				context.getResources().getString(R.string.Thr),
				context.getResources().getString(R.string.Fri),
				context.getResources().getString(R.string.Sat) };
		return weekName;
	}
	
	/**
	 * 预约类型=每周，根据int获取String
	 */
	public static String getNameByWeekday(Context context, int weekdate) {
		
		int[] weekId = new int[]{
				Calendar.SUNDAY,
				Calendar.MONDAY,
				Calendar.TUESDAY,
				Calendar.WEDNESDAY,
				Calendar.THURSDAY,
				Calendar.FRIDAY,
				Calendar.SATURDAY};

		String[] weekName = getWeekdateArray(context);
		
		for(int i=0; i<weekId.length; i++){
			if(weekdate == weekId[i]){
				return weekName[i];
			}
		}
		return weekName[0];
		
	}
	
	/**
	 * 预约类型=每周，根据String获取int
	 */
	public static int getWeekdateByName(Context context, String name) {
		
		int[] weekId = {
				Calendar.SUNDAY,
				Calendar.MONDAY,
				Calendar.TUESDAY,
				Calendar.WEDNESDAY,
				Calendar.THURSDAY,
				Calendar.FRIDAY,
				Calendar.SATURDAY};

		String[] weekName = getWeekdateArray(context);
		
		for(int i=0; i<weekId.length; i++){
			if(name.equals(weekName[i])){
				return weekId[i];
			}
		}
		return -1;
		
	}
	
	/**
	 * 频道集合
	 */
	public static List<ServiceInfoBean> getServiceInfoList(Context context){
		ServiceInfoDao service = new ServiceInfoDaoImpl(context);
		return service.getAllChannelInfo(true);
	}
	
	public static String getNameByLogicalNum(Context context, int logicalNum) {
		ServiceInfoDao service = new ServiceInfoDaoImpl(context);
		List<ServiceInfoBean> serviceInfoBeans = service.getAllChannelInfo(true);

		if (!serviceInfoBeans.isEmpty()) {
			int size = serviceInfoBeans.size();
			for (int i = 0; i < size; i++) {
				if (logicalNum == serviceInfoBeans.get(i).getLogicalNumber()) {
					return serviceInfoBeans.get(i).getChannelName();
				}
			}
			return serviceInfoBeans.get(0).getChannelName();
		}
		return "";
	}

	public static int getServiceIdByLogicalNum(Context context, int logicalNum) {
		ServiceInfoDao service = new ServiceInfoDaoImpl(context);
		List<ServiceInfoBean> serviceInfoBeans = service.getAllChannelInfo(true);

		if (!serviceInfoBeans.isEmpty()) {
			int size = serviceInfoBeans.size();
			for (int i = 0; i<size; i++) {
				if (logicalNum == serviceInfoBeans.get(i).getLogicalNumber()) {
					return serviceInfoBeans.get(i).getServiceId();
				}
			}
			return serviceInfoBeans.get(0).getServiceId();
		}
		return -1;
	}
	
	public static String getCurYMD(){
		 Calendar ca = Calendar.getInstance();
		 int year = ca.get(Calendar.YEAR); // 获取年份
		 int month = ca.get(Calendar.MONTH) + 1; // 获取月份
		 int day = ca.get(Calendar.DATE); // 获取日
		 return year + "-" + fillZero(month) + "-" + fillZero(day);
	}
	
	public static String getCurHM(){
		 Calendar ca = Calendar.getInstance();
		 int hour = ca.get(Calendar.HOUR_OF_DAY);// 小时(24小时制)
		 int minute = ca.get(Calendar.MINUTE); // 分
		 String curTime = fillZero(hour) + ":" + fillZero(minute);
		 
		 long curLong = convertFromHM(curTime);
		 long fiveMinLong = 300000;
		 
		 return getHMFromLong(curLong + fiveMinLong);
	}
	
	public static int getWeekFromYMD(String ymd){
		String[] strs = ymd.split("-");
		int year = Integer.parseInt(strs[0]);
		int month = Integer.parseInt(strs[1]);
		int day = Integer.parseInt(strs[2]);
		Calendar c = Calendar.getInstance();
		c.set(year, month - 1, day);
		int week = c.get(Calendar.DAY_OF_WEEK);
		return week;
	}
	/**
	 * @param yyyy-MM-dd
	 * @return "yyyy-MM-dd 00:00:00"的long类型数据
	 */
	public static long convertFromYMD(String ymd){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date date;
		long result = 0;
		try {
			date = df.parse(ymd + " 00:00:00");
			result = date.getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return result;
	}

	/**
	 * @param HH:mm
	 * @return "yyyy-MM-dd HH:mm:00"-"yyyy-MM-dd 00:00:00"的long类型数据
	 */
	public static long convertFromHM(String hm){
		Calendar ca = Calendar.getInstance();
		int year = ca.get(Calendar.YEAR);
		int month = ca.get(Calendar.MONTH) + 1;
		int day = ca.get(Calendar.DATE);
		String ymd = year + "-" + fillZero(month) + "-" + fillZero(day) + " ";
		
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date date;
		Date dateCompare;
		long result = 0;
		try {
			date = df.parse(ymd + hm + ":00");
			dateCompare = df.parse(ymd + "00:00:00");
			result = date.getTime() - dateCompare.getTime();//毫秒
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	public static String fillZero(int num){
		return String.valueOf(num).length()==2 ? String.valueOf(num) : "0"+num;
	}
	
	public static long convertMSFromHM(String hm){
		String str[] = hm.split(":");
		return (Integer.valueOf(str[0])*60 + Integer.valueOf(str[1]))*60*1000;
	}
	
	public static String getHMFormMS(long l){
		long sumMin = l/(60*1000);
		long hour = sumMin/60;
		long min = sumMin%60;
		return fillZero((int)hour) + ":" + fillZero((int)min);
	}
	
	public static String getYMDFromLong(long ldate){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date date = new Date(ldate);  
		String dateTime = df.format(date);

		String result = dateTime.substring(0, 10);
		return result;
	}

	public static String getHMFromLong(long lhm){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		
		Calendar ca = Calendar.getInstance();
		int year = ca.get(Calendar.YEAR);
		int month = ca.get(Calendar.MONTH) + 1;
		int day = ca.get(Calendar.DATE);
		String ymd = year + "-" + fillZero(month) + "-" + fillZero(day) + " ";
		
		Date dateCompare;
		long lCompare = 0;
		try {
			dateCompare = df.parse(ymd + "00:00:00");
			lCompare = dateCompare.getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		
		Date date = new Date(lhm + lCompare);  
		String dateTime = df.format(date);

		String result = dateTime.substring(11, 16);
		return result;
	}

	public static String getHMSFromPvrTime(long lhm){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Calendar ca = Calendar.getInstance();
		int year = ca.get(Calendar.YEAR);
		int month = ca.get(Calendar.MONTH) + 1;
		int day = ca.get(Calendar.DATE);
		String ymd = year + "-" + fillZero(month) + "-" + fillZero(day) + " ";
		Date dateCompare;
		long lCompare = 0;
		try {
			dateCompare = df.parse(ymd + "00:00:00");
			lCompare = dateCompare.getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		Date date = new Date(lhm + lCompare);  
		String dateTime = df.format(date);
		String result = dateTime.substring(11);
		return result;
	}
	public static boolean validateYMD(String ymd) {
		String str = "^((([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8]))))|((([0-9]{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))-02-29))";
		Pattern pattern = Pattern.compile(str);
		Matcher matcher = pattern.matcher(ymd);
		return matcher.matches();
	}
	
	public static boolean validateHM(String hm) {
		String str = "([0-1]?[0-9]|2[0-3]):([0-5][0-9])";
		
		Pattern pattern = Pattern.compile(str);
		Matcher matcher = pattern.matcher(hm);
		return matcher.matches();
	}
	
	public static boolean validateHMS(String hms) {
		String str = "([0-1]?[0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])";
		
		Pattern pattern = Pattern.compile(str);
		Matcher matcher = pattern.matcher(hms);
		return matcher.matches();
	}
}
