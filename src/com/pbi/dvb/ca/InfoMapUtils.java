package com.pbi.dvb.ca;

import com.pbi.dvb.R;

import android.content.Context;

public class InfoMapUtils {
	
	/**
	 * status：智能卡当前状态
 0，智能卡状态正确
 1，未插入智能卡
 2，智能卡硬件错误
 4，智能卡软件错误
 6，子卡认证失败
 7，子卡认证成功
 8，母卡验证失败
	 */
	public static String matchStatus(int status,Context ctx){
		String result = "";
		switch (status) {
		case 0:
			result = ctx.getResources().getString(R.string.card_status_insert);
			break;
//		case 1:
//			result = ctx.getResources().getString(R.string.card_status_1);
//			break;
//		case 2:
//			result = ctx.getResources().getString(R.string.card_status_2);
//			break;
//		case 4:
//			result = ctx.getResources().getString(R.string.card_status_4);
//			break;
//		case 6:
//			result = ctx.getResources().getString(R.string.card_status_6);
//			break;
//		case 7:
//			result = ctx.getResources().getString(R.string.card_status_7);
//			break;
//		case 8:
//			result = ctx.getResources().getString(R.string.card_status_8);
//			break;
		default:
			result = ctx.getResources().getString(R.string.card_status_extract);
			break;
		}
		return result;
	}
	
    
    /**
     * 智能卡使用状态:0,智能卡有效; 1,智能卡暂停; 2,智能卡被取消;    
     */
	public static String matchUsedStatus(int status,Context ctx){
		String result = "";
		switch (status) {
		case 0:
			result = ctx.getResources().getString(R.string.card_usedstatus_effect);
			break;
		case 1:
			result = ctx.getResources().getString(R.string.card_usedstatus_pause);
			break;
		case 2:
			result = ctx.getResources().getString(R.string.card_usedstatus_cancel);
			break;
		default:
			break;
		}
		return result;
	}

    
    /**
     * 父母锁控制级别:0,无限制; 4-18为年龄;    
     */
	public static String matchLockControl(int age, Context ctx) {
		String result = "" + age;
		switch (age) {
		case 0:
			result = ctx.getResources().getString(R.string.card_lock_control);
			break;
		default:
			break;
		}
		return result;
	}

    /**
     * 智能卡类型：0,母卡; 1,子卡; 2,卡类型错误
     */
	public static String matchType(int type,Context ctx){
		String result = "";
		switch (type) {
		case 0:
			result = ctx.getResources().getString(R.string.card_type_mother);
			break;
		case 1:
			result = ctx.getResources().getString(R.string.card_type_son);
			break;
		case 2:
			result = ctx.getResources().getString(R.string.card_type_error);
			break;
		default:
			break;
		}
		return result;
	}
	
    

    /**
     * 智能卡配对状态: 0,未配对; 1,已配对; 5,未注册STB Number; 7,与STB不匹配
     */
	public static String matchMatch(int match,Context ctx){
		String result = "";
		switch (match) {
		case 0:
			result = ctx.getResources().getString(R.string.card_match_not);
			break;
		case 1:
			result = ctx.getResources().getString(R.string.card_match_yes);
			break;
		case 5:
			result = ctx.getResources().getString(R.string.card_match_noregister);
			break;
		case 7:
			result = ctx.getResources().getString(R.string.card_match_nostb);
			break;
		default:
			break;
		}
		return result;
	}

    /**
     *  机顶盒配对状态:2,STB未配对; 3,STB已配对; 6, STB未注册STB Number
     */
	public static String matchStbMatch(int match,Context ctx){
		String result = "";
		switch (match) {
		case 2:
			result = ctx.getResources().getString(R.string.card_stbmatch_not);
			break;
		case 3:
			result = ctx.getResources().getString(R.string.card_stbmatch_yes);
			break;
		case 6:
			result = ctx.getResources().getString(R.string.card_stbmatch_noregister);
			break;
		default:
			break;
		}
		return result;
	}

    /**
     *  节目提供商/频道授权状态:0,PPID有效; 1,PPID无效
     */
	public static String matchPpidStatus(int status,Context ctx){
		String result = "";
		switch (status) {
		case 0:
			result = ctx.getResources().getString(R.string.card_ppid_status_effect);
			break;
		case 1:
			result = ctx.getResources().getString(R.string.card_ppid_status_pause);
			break;
		default:
			break;
		}
		return result;
	}
	
	/**
	 * 十进制-->2字节十六进制
	 */
	public static String convertTwoHexString(int num) {
		// return "0x" + Integer.toHexString(num).toUpperCase();

		String temp = Integer.toHexString(num).toUpperCase();
		switch (temp.length()) {
		case 1:
			return "0x000" + temp;
		case 2:
			return "0x00" + temp;
		case 3:
			return "0x0" + temp;
		default:
			return "0x" + temp;
		}
	}
	
	/**
	 * 十进制-->3字节十六进制
	 */
	public static String convertHexString(int num) {
		// return "0x" + Integer.toHexString(num).toUpperCase();

		String temp = Integer.toHexString(num).toUpperCase();
		switch (temp.length()) {
		case 1:
			return "0x00000" + temp;
		case 2:
			return "0x0000" + temp;
		case 3:
			return "0x000" + temp;
		case 4:
			return "0x00" + temp;
		case 5:
			return "0x0" + temp;
		default:
			return "0x" + temp;
		}
	}
}
