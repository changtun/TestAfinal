/*
 * File Name:  CommonUtils.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.os.Bundle;

import com.pbi.dvb.R;
import com.pbi.dvb.global.Config;
import android.remote.Remote;
import android.os.PBIManager;
import android.util.Log;

public class CommonUtils
{
    private static final String TAG = "CommonUtils";
    
    private static String SCREEN_MODE_FILE = "/sys/class/video/screen_mode";
    
    public static void skipActivity(Activity activity, Class<?> clazz, Bundle bundle, int flag)
    {
        Intent intent = new Intent(activity, clazz);
        if (flag > 0)
        {
            intent.setFlags(flag);
        }
        if (null != bundle)
        {
            intent.putExtras(bundle);
        }
        activity.startActivity(intent);
        activity.finish();
        
    }
    
    public static void skipActivity(Activity activity, Class<?> clazz, int flag)
    {
        Intent intent = new Intent(activity, clazz);
        if (flag > 0)
        {
            intent.setFlags(flag);
        }
        activity.startActivity(intent);
        activity.finish();
    }
    
    public static void skipActivity(Activity activity, Class<?> clazz, String name, String value, int flag)
    {
        Intent intent = new Intent(activity, clazz);
        if (flag > 0)
        {
            intent.setFlags(flag);
        }
        intent.putExtra(name, value);
        activity.startActivity(intent);
        activity.finish();
    }
    
    /**
     * 
     * <Skip to the activity with a boolean type value.>
     * 
     * @param activity
     * @param clazz
     * @param name
     * @param value
     * @param flag
     * @see [�ࡢ��#��������#��Ա]
     */
    public static void skipActivity(Activity activity, Class<?> clazz, String name, Boolean value, int flag)
    {
        Intent intent = new Intent(activity, clazz);
        if (flag > 0)
        {
            intent.setFlags(flag);
        }
        intent.putExtra(name, value);
        activity.startActivity(intent);
        activity.finish();
    }
    
    /**
     * 
     * <Skip to the activity with a integer type value.>
     * 
     * @param activity
     * @param clazz
     * @param name
     * @param value
     * @param flag
     * @see [�ࡢ��#��������#��Ա]
     */
    public static void skipActivity(Activity activity, Class<?> clazz, String name, Integer value, int flag)
    {
        Intent intent = new Intent(activity, clazz);
        if (flag > 0)
        {
            intent.setFlags(flag);
        }
        intent.putExtra(name, value);
        activity.startActivity(intent);
        activity.finish();
    }
    
    /**
     * 
     * 
     * @param str
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public static String parseAscii(String str)
    {
        StringBuffer sb = new StringBuffer();
        byte[] bytes;
        try
        {
            bytes = str.getBytes("utf-8");
            for (byte b : bytes)
            {
                sb.append(Integer.toHexString(b & 0xff));
            }
        }
        catch (UnsupportedEncodingException e)
        {
            e.printStackTrace();
        }
        return sb.toString().toUpperCase();
    }
    
    /**
     * <�ַ��ȡ����>
     * 
     * @param str ������ַ�
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public static int getModHandling(String strQam)
    {
        int mod = 0;
        int eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_64QAM;
        String[] result = strQam.trim().split(" ");
        if (null != result[0] && !"".equals(result[0]))
        {
            mod = Integer.parseInt(result[0]);
        }
        
        switch (mod)
        {
            case 16:
                eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_16QAM;
                break;
            case 32:
                eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_32QAM;
                break;
            case 64:
                eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_64QAM;
                break;
            case 128:
                eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_128QAM;
                break;
            case 256:
                eMod = Config.DVBCore_Modulation_eDVBCORE_MOD_256QAM;
                break;
        }
        return eMod;
    }
    
    /**
     * 
     * <�ַ��ʽ��>
     * 
     * @param character
     * @return
     * @see [TVChannelPlay#getEpgInfo()]
     */
    public static String alignCharacter(short character)
    {
        if (character < 10)
        {
            return "0" + character;
        }
        return "" + character;
    }
    
    public static String alignCharacter(int character)
    {
        if (character < 10)
        {
            return "0" + character;
        }
        return "" + character;
    }
    
    public static String getMailTime(int year, int month, int day, int hour, int minute)
    {
        return year + "-" + month + "-" + day + "  " + alignCharacter(hour) + ":" + alignCharacter(minute);
    }
    
    /**
     * 
     * <Functional overview>
     * 
     * @param getGrade
     * @return 0 普 1 保 2 辅 3 限 4特
     * @see [Class, Class#Method, Class#Member]
     */
    public static int getEpgGrade(int getGrade)
    {
        LogUtils.printLog(1, 3, TAG, "--->>> the epg eit age is: " + getGrade);
        if (getGrade == 0)
        {
           return  0;
        }
        else if (1 <= getGrade && getGrade <= 6)
        {
            return 1;
        }
        else if (7 <= getGrade && getGrade <= 12)
        {
            return 2;
        }
        else if (13 <= getGrade && getGrade <= 14)
        {
            return 3;
        }
        //(getGrade == 15)
        else
        {
            return 4;
        }
    }
    
    /**
     * 
     * <get bitmap from resources.>
     * 
     * @param context
     * @param resId
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public static Bitmap getBitmap(Context context, int resId)
    {
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inPreferredConfig = Bitmap.Config.RGB_565;
        opt.inJustDecodeBounds = false;
        opt.inSampleSize = 2; // width��hight��Ϊԭ����ʮ��һ
        opt.inPurgeable = true;
        opt.inInputShareable = true;
        InputStream is = context.getResources().openRawResource(resId);
        return BitmapFactory.decodeStream(is, null, opt);
    }
    
    public static void releaseBitmap(Bitmap bitmap)
    {
        if (bitmap.isRecycled())
        {
            bitmap.recycle();
            System.gc();
        }
    }

	public static String getPlayMode()
	{
		String displaymode_path = "/sys/class/display/mode";
        String dispMode = "720p";
		File file = new File(displaymode_path);
        if (!file.exists())
        {
            return dispMode;
        }
		try
        {
            BufferedReader in = new BufferedReader(new FileReader(displaymode_path), 32);
            try
            {
                dispMode = in.readLine();
                if (dispMode == null)
                {// not exist,default 720p
                    dispMode = "720p";                    
                }                
            }
            finally
            {
                in.close();
                // return rect;
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        return dispMode;
	}
    public static Rect getPlayWindow()
    {
        String displaymode_path = "/sys/class/display/mode";
        String dispMode = null;
        Rect rect = new Rect();
        rect.top = 0;
        rect.left = 0;
        rect.bottom = 720 + rect.top;
        rect.right = 1280 + rect.left;
        File file = new File(displaymode_path);
        if (!file.exists())
        {
            return rect;
        }
        
        // read
        try
        {
            BufferedReader in = new BufferedReader(new FileReader(displaymode_path), 32);
            try
            {
                dispMode = in.readLine();
                if (dispMode == null)
                {// not exist,default m2,lvds1080p
                    rect.top = 0;
                    rect.left = 0;
                    rect.bottom = 1079 + rect.top;
                    rect.right = 1919 + rect.left;
                    
                }
                else
                {
                    if (dispMode.contains("480"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 480 + rect.top;
                        rect.right = 720 + rect.left;
                    }
                    else if (dispMode.contains("576"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 576 + rect.top;
                        rect.right = 720 + rect.left;
                    }
                    else if (dispMode.contains("720"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 720 + rect.top;
                        rect.right = 1280 + rect.left;
                    }
                    else if (dispMode.contains("1080"))
                    {
                        rect.top = 0;
                        rect.left = 0;
                        rect.bottom = 1080 + rect.top;
                        rect.right = 1920 + rect.left;
                    }
                    
                }
            }
            finally
            {
                in.close();
                // return rect;
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        return rect;
    }
    
    public static boolean sendHttpRequest(int channelId)
    {
        try
        {
            URL url = new URL(Config.PVR_SERVER);
            HttpURLConnection conn = (HttpURLConnection)url.openConnection();
            conn.setConnectTimeout(2000);
            conn.setRequestMethod("GET");
            
            LogUtils.printLog(1, 3, TAG, "--->>> response code is " + conn.getResponseCode());
            if (conn.getResponseCode() == 200)
            {
                return true;
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return false;
        }
        return false;
    }
    
    public static String getNitCategory(Context context, int category)
    {
        StringBuilder builder = new StringBuilder();
        builder.append(context.getString(R.string.category_level_control));
        switch (category)
        {
            case 1:
                builder.append(context.getString(R.string.category_movie));
                break;
            case 2:
                builder.append(context.getString(R.string.category_albums));
                break;
            case 3:
                builder.append(context.getString(R.string.category_soap));
                break;
            case 4:
                builder.append(context.getString(R.string.category_variety));
                break;
            case 5:
                builder.append(context.getString(R.string.category_music));
                break;
            case 6:
                builder.append(context.getString(R.string.category_cognitivene));
                break;
            case 7:
                builder.append(context.getString(R.string.category_sports));
                break;
            case 8:
                builder.append(context.getString(R.string.category_cartoons));
                break;
            case 9:
                builder.append(context.getString(R.string.category_news));
                break;
            case 10:
                builder.append(context.getString(R.string.category_opera));
                break;
            case 11:
                builder.append(context.getString(R.string.category_other));
                break;
        }
        return builder.append(context.getString(R.string.category_controlling)).toString();
    }
    
    /**
     * 
     * <swith the left/right function on the RCU>
     * 
     * @param value Config.SWITCH_TO_LEFT_RIGHT_CONTROL Config.SWITCH_TO_VOICE_CONTROL
     * @see [Class, Class#Method, Class#Member]
     */
    public static void setLeftRightFunction(int value)
    {
        Remote remote = new Remote();
        remote.Remote_init();
        int val = remote.Remote_get_vlaue();
        LogUtils.printLog(1, 3, TAG, "--->>> the remote original value(current value) is" + val + "(" + value + ")");
        if (val == value)
        {
            return;
        }
        remote.Remote_set_vlaue(value);// 0 音量 1左右
        remote.Remote_exit();
    }
    
    /**
     * 
     * <This key used for swithing left/right and volume up/down on the RCU.>
     * 
     * @param isOpen false:forbid, true: not forbid.
     * @see [Class, Class#Method, Class#Member]
     */
    public static void forbidSwitchKeyFunction(Context context,boolean isOpen)
    {
        PBIManager pbiManager = (PBIManager)context.getSystemService("pbi");
        boolean status = pbiManager.getChangeRemote();
        LogUtils.printLog(1, 3, TAG, "--->>> the remote original status(current status) is" + status + "(" + isOpen + ")");
        if(isOpen == status)
        {
            return;
        }
        pbiManager.setChangeRemote(isOpen);
    }
    
    public static List<String> getNitCategorySettings(Context context)
    {
        String epgType = android.provider.Settings.System.getString(context.getContentResolver(), "positionList");
        Log.i(TAG, "--->>> eit category is: " + epgType);
        List<String> list = new ArrayList<String>();
        if (null != epgType)
        {
            String[] types = epgType.split("#");
            for (String string : types)
            {
                Log.i(TAG, "--->>> epg type is :  " + string);
                list.add(string);
            }
        }
        return list;
    }

  public static int getScreenMode()
    {
        File file = new File(SCREEN_MODE_FILE);
        if (!file.exists())
        {
            return 0;
        }
        
        String mode = null;
        int ret = 0;
        try
        {
            BufferedReader in = new BufferedReader(new FileReader(SCREEN_MODE_FILE), 32);
            try
            {
                mode = in.readLine();
                mode = mode.substring(0, 1);
                ret = Integer.parseInt(mode);
            }
            finally
            {
                in.close();
            }
            Log.i("zshang", "getScreenMode , ret = " + ret);
            return ret;
        }
        catch (Exception e)
        {
            return 0;
        }
    }
    
    public static int setScreenMode(int mode)
    {
        File file = new File(SCREEN_MODE_FILE);
        if (!file.exists())
        {
            return 0;
        }
        
        try
        {
            BufferedWriter out = new BufferedWriter(new FileWriter(SCREEN_MODE_FILE), 32);
            String strMode = "".valueOf(mode);
            try
            {
                out.write(strMode);
                Log.i("zshang", "setScreenMode , strMode = " + strMode);
            }
            finally
            {
                out.close();
            }
            return 1;
        }
        catch (Exception e)
        {
            return 0;
        }
    }
}
