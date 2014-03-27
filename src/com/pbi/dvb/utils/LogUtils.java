/*
 * �� �� ��:  LogUtils.java
 * ��    Ȩ:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  
 * �޸�ʱ��:  2012-12-15
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.utils;

import java.util.Date;

import android.util.Log;

/**
 * <Android log utils>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-12-15]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class LogUtils
{
    
    public static final int LOG_VERBOSE = 1;
    
    public static final int LOG_DEBUG = 2;
    
    public static final int LOG_INFO = 3;
    
    public static final int LOG_WARN = 4;
    
    public static final int LOG_ERROR = 5;
    
    private static final int level = LOG_DEBUG;
    
    /**
     * 
     * <Android print log method>
     * 
     * @param isShown 1 show the log ,0 hide the log.
     * @param logType 1 verbose 2 debug 3 info 4 warn 5 error
     * @param tag
     * @param content
     * @see [�ࡢ��#��������#��Ա]
     */
    public static void printLog(int isShown, int logType, String tag, String content)
    {
        
        if (isShown == 1)
        {
            
            switch (logType)
            {
            
                case LOG_VERBOSE:
                    
                    Log.v(tag, content);
                    
                    break;
                
                case LOG_DEBUG:
                    
                    Log.d(tag, content);
                    
                    break;
                
                case LOG_INFO:
                    
                    Log.i(tag, content);
                    
                    break;
                
                case LOG_WARN:
                    
                    Log.w(tag, content);
                    
                    break;
                
                case LOG_ERROR:
                    
                    Log.e(tag, content);
                    
                    break;
            
            }
        }
    }
    
    public static void e(String tag, String msg)
    {
        
        if (level <= LOG_ERROR)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.e(tag, msg);
            
        }
    }
    
    public static void w(String tag, String msg)
    {
        
        if (level <= LOG_WARN)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.w(tag, msg);
            
        }
        
    }
    
    public static void i(String tag, String msg)
    {
        
        if (level <= LOG_INFO)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.i(tag, msg);
            
        }
    }
    
    public static void d(String tag, String msg)
    {
        
        if (level <= LOG_DEBUG)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.d(tag, msg);
            
        }
        
    }
    
    public static void v(String tag, String msg)
    {
        
        if (level <= LOG_VERBOSE)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.v(tag, msg);
            
        }
        
    }
    
    public static void e(String tag, String msg, boolean timeFlag)
    {
        
        if (timeFlag)
        {
            
            Log.e(tag, "-------->>>> Now is " + new Date().toString() + " <<<<--------");
            
        }
        
        if (level <= LOG_ERROR)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.e(tag, msg);
            
        }
    }
    
    public static void w(String tag, String msg, boolean timeFlag)
    {
        
        if (timeFlag)
        {
            
            Log.v(tag, "-------->>>> Now is " + new Date().toString() + " <<<<--------");
            
        }
        
        if (level >= LOG_WARN)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.w(tag, msg);
            
        }
        
    }
    
    public static void i(String tag, String msg, boolean timeFlag)
    {
        
        if (timeFlag)
        {
            
            Log.v(tag, "-------->>>> Now is " + new Date().toString() + " <<<<--------");
            
        }
        
        if (level <= LOG_INFO)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.i(tag, msg);
            
        }
    }
    
    public static void d(String tag, String msg, boolean timeFlag)
    {
        
        if (timeFlag)
        {
            
            Log.v(tag, "-------->>>> Now is " + new Date().toString() + " <<<<--------");
            
        }
        
        if (level <= LOG_DEBUG)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.d(tag, msg);
            
        }
        
    }
    
    public static void v(String tag, String msg, boolean timeFlag)
    {
        
        if (timeFlag)
        {
            
            Log.v(tag, "-------->>>> Now is " + new Date().toString() + " <<<<--------");
            
        }
        
        if (level <= LOG_VERBOSE)
        {
            if (!msg.startsWith("---->>>>"))
            {
                msg = "---->>>> " + msg + " <<<<----";
            }
            
            Log.v(tag, msg);
            
        }
        
    }
    
}
