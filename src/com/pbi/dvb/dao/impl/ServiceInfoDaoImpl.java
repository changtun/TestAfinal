/*
 * File Name:  ServiceInfoDaoImpl.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <the class ServiceInfoDao's implmentation >
 * Changed By:  gtsong
 * Changed Time:  2014-01-07
 * Changed Content:  <add update soundtrack and language method.>
 */
package com.pbi.dvb.dao.impl;

import java.util.ArrayList;
import java.util.List;

import android.content.ContentProviderOperation;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;

import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.db.DBService;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class ServiceInfoDaoImpl implements ServiceInfoDao
{
    private Context context;
    
    public ServiceInfoDaoImpl(Context context)
    {
        this.context = context;
    }
    
    public ContentProviderOperation addServiceInfo(ServiceInfoBean serviceInfoBean, int position)
    {
        ContentValues values = new ContentValues();
        values.put(DBService.LOGICAL_NUMBER, serviceInfoBean.getLogicalNumber());
        values.put(DBService.TP_ID, serviceInfoBean.getTpId());
        values.put(DBService.TUNER_TYPE, serviceInfoBean.getTunerType());
        values.put(DBService.SERVICE_ID, serviceInfoBean.getServiceId());
        values.put(DBService.SERVICE_TYPE, String.valueOf(serviceInfoBean.getServiceType()));
        values.put(DBService.REF_SERVICE_ID, serviceInfoBean.getRefServiceId());
        values.put(DBService.PME_ID, serviceInfoBean.getPmtPid());
        values.put(DBService.CA_MODE, String.valueOf(serviceInfoBean.getCaMode()));
        values.put(DBService.CHANNEL_NAME, serviceInfoBean.getChannelName());
        values.put(DBService.VOLUME, serviceInfoBean.getVolume());
        values.put(DBService.VIDEO_TYPE, serviceInfoBean.getVideoType());
        values.put(DBService.VIDEO_PID, serviceInfoBean.getVideoPid());
        values.put(DBService.AUDIO_MODE, serviceInfoBean.getAudioMode());
        values.put(DBService.AUDIO_INDEX, serviceInfoBean.getAudioIndex());
        values.put(DBService.AUDIO_TYPE, serviceInfoBean.getAudioType());
        values.put(DBService.AUDIO_PID, serviceInfoBean.getAudioPid());
        values.put(DBService.AUDIO_LANG, serviceInfoBean.getAudioLang());
        values.put(DBService.PCR_PID, serviceInfoBean.getPcrPid());
        values.put(DBService.FAV_FLAG, serviceInfoBean.getFavFlag());
        values.put(DBService.LOCK_FLAG, serviceInfoBean.getLockFlag());
        values.put(DBService.MOVE_FLAG, serviceInfoBean.getMoveFlag());
        values.put(DBService.DEL_FLAG, serviceInfoBean.getDelFlag());
        values.put(DBService.CHANNEL_POSITION, position);
        
        return ContentProviderOperation.newInsert(DBService.CONTENT_URI).withValues(values).build();
    }
    
    public List<ServiceInfoBean> getAllChannelInfo(boolean hasCaMode)
    {
        Cursor cursor =
            context.getContentResolver().query(DBService.CONTENT_URI, null, null, null, DBService.LOGICAL_NUMBER);
        List<ServiceInfoBean> serviceInfos = new ArrayList<ServiceInfoBean>();
        ServiceInfoBean serviceInfo = null;
        while (cursor.moveToNext())
        {
            serviceInfo = parserCursor(cursor, hasCaMode);
            serviceInfos.add(serviceInfo);
            serviceInfo = null;
        }
        cursor.close();
        return serviceInfos;
    }
    
    public List<ServiceInfoBean> getTVChannelInfo()
    {
        Cursor cursor =
            context.getContentResolver().query(DBService.CONTENT_URI, null, null, null, DBService.LOGICAL_NUMBER);
        List<ServiceInfoBean> serviceInfos = new ArrayList<ServiceInfoBean>();
        ServiceInfoBean serviceInfo;
        while (cursor.moveToNext())
        {
            serviceInfo = parserCursor(cursor, true);
            if (serviceInfo.getServiceType() == Config.SERVICE_TYPE_TV)
            {
                serviceInfos.add(serviceInfo);
            }
            serviceInfo = null;
        }
        cursor.close();
        return serviceInfos;
    }
    
    public List<ServiceInfoBean> getRadioChannelInfo()
    {
        Cursor cursor =
            context.getContentResolver().query(DBService.CONTENT_URI, null, null, null, DBService.LOGICAL_NUMBER);
        List<ServiceInfoBean> serviceInfos = new ArrayList<ServiceInfoBean>();
        ServiceInfoBean serviceInfo;
        while (cursor.moveToNext())
        {
            serviceInfo = parserCursor(cursor, true);
            if (serviceInfo.getServiceType() == Config.SERVICE_TYPE_RADIO)
            {
                serviceInfos.add(serviceInfo);
            }
            serviceInfo = null;
        }
        cursor.close();
        return serviceInfos;
    }
    
    public ServiceInfoBean getChannelInfoByChannelName(String channelName)
    {
        ServiceInfoBean serviceInfo = null;
        String selection = DBService.CHANNEL_NAME + " = ? ";
        String[] selectionArgs = new String[] {channelName};
        Cursor cursor = context.getContentResolver().query(DBService.CONTENT_URI, null, selection, selectionArgs, null);
        
        if (cursor.moveToFirst())
        {
            serviceInfo = parserCursor(cursor, true);
        }
        cursor.close();
        return serviceInfo;
    }
    
    public ServiceInfoBean getChannelInfoByServiceId(int serviceId, int logicalNumber)
    {
        ServiceInfoBean serviceInfo = null;
        String selection =
            DBService.SERVICE_ID + " = " + serviceId + " and " + DBService.LOGICAL_NUMBER + " = " + logicalNumber;
        Cursor cursor = context.getContentResolver().query(DBService.CONTENT_URI, null, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            serviceInfo = parserCursor(cursor, true);
        }
        cursor.close();
        return serviceInfo;
    }
    
    public List<ServiceInfoBean> getChannelIdByLogicalNumber(int logicalNumber)
    {
        ServiceInfoBean serviceInfo = null;
        List<ServiceInfoBean> serviceInfos = new ArrayList<ServiceInfoBean>();
        String selection = DBService.LOGICAL_NUMBER + " = " + logicalNumber;
        Cursor cursor = context.getContentResolver().query(DBService.CONTENT_URI, null, selection, null, null);
        
        while (cursor.moveToNext())
        {
            serviceInfo = parserCursor(cursor, true);
            serviceInfos.add(serviceInfo);
            serviceInfo = null;
        }
        cursor.close();
        return serviceInfos;
    }
    
    public ContentProviderOperation clearServiceData()
    {
        return ContentProviderOperation.newDelete(DBService.CONTENT_URI).build();
    }
    
    public void clearService()
    {
        context.getContentResolver().delete(DBService.CONTENT_URI, null, null);
    }
    
    public ContentProviderOperation updateServiceSettings(ServiceInfoBean sBean, int channelNumber)
    {
        ContentValues values = new ContentValues();
        values.put(DBService.FAV_FLAG, sBean.getFavFlag());
        values.put(DBService.LOCK_FLAG, sBean.getLockFlag());
        String where = DBService.CHANNEL_NUMBER + " = " + channelNumber;
        return ContentProviderOperation.newUpdate(DBService.CONTENT_URI)
            .withValues(values)
            .withSelection(where, null)
            .build();
    }
    
    // public int checkIsExist(String channelName)
    // {
    // int isExist = 0;
    // if (null != channelName && !"".equals(channelName))
    // {
    // String[] projection = new String[] {"count(channel_number)"};
    // String selection = DBService.CHANNEL_NAME +" = ?";
    // String[] selectionArgs = new String[] {channelName};
    // Cursor cursor =
    // context.getContentResolver().query(DBService.CONTENT_URI, projection, selection, selectionArgs, null);
    //
    // if (cursor.moveToFirst())
    // {
    // isExist = cursor.getInt(0);
    // }
    // cursor.close();
    // }
    // return isExist;
    // }
    
    // public List<String> getTVChannelNames()
    // {
    // Cursor cursor =
    // context.getContentResolver().query(DBService.CONTENT_URI, null, null, null, DBService.CHANNEL_POSITION);
    // List<String> tvChannelNames = new ArrayList<String>();
    // while (cursor.moveToNext())
    // {
    // int serviceTypeIndex = cursor.getColumnIndex(DBService.SERVICE_TYPE);
    // int channelNameIndex = cursor.getColumnIndex(DBService.CHANNEL_NAME);
    //
    // char serviceType = cursor.getString(serviceTypeIndex).charAt(0);
    // String channelName = cursor.getString(channelNameIndex);
    //
    // if (serviceType == Config.SERVICE_TYPE_TV)
    // {
    // tvChannelNames.add(channelName);
    // }
    // }
    // cursor.close();
    // return tvChannelNames;
    // }
    
    // public List<String> getRadioChanneNames()
    // {
    // Cursor cursor =
    // context.getContentResolver().query(DBService.CONTENT_URI, null, null, null, DBService.CHANNEL_POSITION);
    // List<String> radioChannelNames = new ArrayList<String>();
    // while (cursor.moveToNext())
    // {
    // int serviceTypeIndex = cursor.getColumnIndex(DBService.SERVICE_TYPE);
    // int channelNameIndex = cursor.getColumnIndex(DBService.CHANNEL_NAME);
    //
    // char serviceType = cursor.getString(serviceTypeIndex).charAt(0);
    // String channelName = cursor.getString(channelNameIndex);
    //
    // if (serviceType == Config.SERVICE_TYPE_RADIO)
    // {
    // radioChannelNames.add(channelName);
    // }
    // }
    // cursor.close();
    // return radioChannelNames;
    // }
    
    public List<ServiceInfoBean> getFavChannelInfo()
    {
        String selection = DBService.FAV_FLAG + " = 1 ";
        Cursor cursor =
            context.getContentResolver().query(DBService.CONTENT_URI, null, selection, null, DBService.LOGICAL_NUMBER);
        List<ServiceInfoBean> serviceInfos = new ArrayList<ServiceInfoBean>();
        ServiceInfoBean serviceInfo;
        while (cursor.moveToNext())
        {
            serviceInfo = parserCursor(cursor, true);
            serviceInfos.add(serviceInfo);
            serviceInfo = null;
        }
        cursor.close();
        return serviceInfos;
    }
    
    public ServiceInfoBean getChannelInfo(int tpId, int serviceid)
    {
        ServiceInfoBean serviceInfo = null;
        String selection = DBService.TP_ID + " = " + tpId + " and " + DBService.SERVICE_ID + " = " + serviceid;
        Cursor cursor =
            context.getContentResolver().query(DBService.CONTENT_URI, null, selection, null, DBService.LOGICAL_NUMBER);
        if (cursor.moveToFirst())
        {
            serviceInfo = parserCursor(cursor, true);
        }
        cursor.close();
        return serviceInfo;
    }
    
    // public int getChannelNumber(int serviceId,int tpId)
    // {
    // int chNo = 0;
    // String[] projection = new String[] {DBService.CHANNEL_NUMBER};
    // String selection = DBService.SERVICE_ID+" = "+serviceId+" and "+DBService.TP_ID+" = "+tpId;
    // Cursor cursor = context.getContentResolver().query(DBService.CONTENT_URI, projection, selection, null, null);
    // if (cursor.moveToFirst())
    // {
    // int chNoIndex = cursor.getColumnIndex(DBService.CHANNEL_NUMBER);
    // chNo = cursor.getInt(chNoIndex);
    // }
    // return chNo;
    // }
    
    public void updateAudioInfo(int serviceId, int audioPid, int audioType, int audioMode)
    {
        String where = DBService.SERVICE_ID + " =" + serviceId;
        ContentValues values = new ContentValues();
        values.put(DBService.AUDIO_PID, audioPid);
        values.put(DBService.AUDIO_TYPE, audioType);
        values.put(DBService.AUDIO_MODE, audioMode);
        context.getContentResolver().update(DBService.CONTENT_URI, values, where, null);
    }
    
    public void updateVolumeInfo(int serviceId, int volume)
    {
        String where = DBService.SERVICE_ID + " =" + serviceId;
        ContentValues values = new ContentValues();
        values.put(DBService.VOLUME, volume);
        context.getContentResolver().update(DBService.CONTENT_URI, values, where, null);
    }
    
    public ContentProviderOperation clearServiceDataByChannelNumber(int channelNumber)
    {
        String selection = DBService.CHANNEL_NUMBER + " = " + channelNumber;
        return ContentProviderOperation.newDelete(DBService.CONTENT_URI).withSelection(selection, null).build();
    }
    
    public void updateCaMode(int serviceId, int logicalNumber, int caMode)
    {
        String where =
            DBService.SERVICE_ID + "= " + serviceId + " and " + DBService.LOGICAL_NUMBER + " = " + logicalNumber;
        LogUtils.printLog(1, 3, "ServiceInfoDao", "update ca mode --->>>" + where + "ca mode is" + caMode);
        ContentValues values = new ContentValues();
        values.put(DBService.CA_MODE, caMode);
        context.getContentResolver().update(DBService.CONTENT_URI, values, where, null);
    }
    
    /**
     * 
     * <If the hasCaMode is true,there is a '$' tag before the service name.The DVB-JYSX version has cancelled this
     * function.>
     * 
     * 
     * @param cursor
     * @param hasCaMode
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    private ServiceInfoBean parserCursor(Cursor cursor, boolean hasCaMode)
    {
        // ServiceInfoBean serviceInfo = null;
        
        int channelNumberIndex = cursor.getColumnIndex(DBService.CHANNEL_NUMBER);
        int logicalNumberIndex = cursor.getColumnIndex(DBService.LOGICAL_NUMBER);
        int tpIdIndex = cursor.getColumnIndex(DBService.TP_ID);
        int tunerTypeIndex = cursor.getColumnIndex(DBService.TUNER_TYPE);
        int serviceIdIndex = cursor.getColumnIndex(DBService.SERVICE_ID);
        int serviceTypeIndex = cursor.getColumnIndex(DBService.SERVICE_TYPE);
        int refServiceIdIndex = cursor.getColumnIndex(DBService.REF_SERVICE_ID);
        int pmtPidIndex = cursor.getColumnIndex(DBService.PME_ID);
        int caModeIndex = cursor.getColumnIndex(DBService.CA_MODE);
        int channelNameIndex = cursor.getColumnIndex(DBService.CHANNEL_NAME);
        
        int volumeIndex = cursor.getColumnIndex(DBService.VOLUME);
        int videoTypeIndex = cursor.getColumnIndex(DBService.VIDEO_TYPE);
        int videoPidIndex = cursor.getColumnIndex(DBService.VIDEO_PID);
        int audioModeIndex = cursor.getColumnIndex(DBService.AUDIO_MODE);
        int audioIndexIndex = cursor.getColumnIndex(DBService.AUDIO_INDEX);
        int audioTypeIndex = cursor.getColumnIndex(DBService.AUDIO_TYPE);
        int audioPidIndex = cursor.getColumnIndex(DBService.AUDIO_PID);
        int audioLangIndex = cursor.getColumnIndex(DBService.AUDIO_LANG);
        int pcrPidIndex = cursor.getColumnIndex(DBService.PCR_PID);
        
        int favFlagIndex = cursor.getColumnIndex(DBService.FAV_FLAG);
        int lockFlagIndex = cursor.getColumnIndex(DBService.LOCK_FLAG);
        int moveFlagIndex = cursor.getColumnIndex(DBService.MOVE_FLAG);
        int delFlagIndex = cursor.getColumnIndex(DBService.DEL_FLAG);
        int channelPositionIndex = cursor.getColumnIndex(DBService.CHANNEL_POSITION);
        
        int channelNumber = cursor.getInt(channelNumberIndex);
        int logicalNumber = cursor.getInt(logicalNumberIndex);
        int tpId = cursor.getInt(tpIdIndex);
        int tunerType = cursor.getInt(tunerTypeIndex);
        int serviceId = cursor.getInt(serviceIdIndex);
        
        String strServiceType = cursor.getString(serviceTypeIndex);
        char serviceType;
        if (strServiceType.length() > 0)
        {
            serviceType = strServiceType.charAt(0);
        }
        else
        {
            serviceType = '0';
        }
        
        int refServiceId = cursor.getInt(refServiceIdIndex);
        int pmtPid = cursor.getInt(pmtPidIndex);
        
        int caMode = cursor.getInt(caModeIndex);
        String channelName = cursor.getString(channelNameIndex);
        
        // if (hasCaMode && caMode == 1)
        // {
        // channelName = "$ " + channelName;
        // }
        
        int volume = cursor.getInt(volumeIndex);
        int videoType = cursor.getInt(videoTypeIndex);
        int videoPid = cursor.getInt(videoPidIndex);
        int audioMode = cursor.getInt(audioModeIndex);
        int audioIndex = cursor.getInt(audioIndexIndex);
        int audioType = cursor.getInt(audioTypeIndex);
        int audioPid = cursor.getInt(audioPidIndex);
        String audioLang = cursor.getString(audioLangIndex);
        int pcrPid = cursor.getInt(pcrPidIndex);
        
        int favFlag = cursor.getInt(favFlagIndex);
        int lockFlag = cursor.getInt(lockFlagIndex);
        int moveFlag = cursor.getInt(moveFlagIndex);
        int delFlag = cursor.getInt(delFlagIndex);
        int channelPosition = cursor.getInt(channelPositionIndex);
        
        ServiceInfoBean serviceInfo =
            new ServiceInfoBean(channelNumber, logicalNumber, tpId, tunerType, serviceId, serviceType, refServiceId,
                pmtPid, caMode, channelName, volume, videoType, videoPid, audioMode, audioIndex, audioType, audioPid,
                audioLang, pcrPid, favFlag, lockFlag, moveFlag, delFlag, channelPosition);
        
        return serviceInfo;
    }
    
    public void updatePmtPid(int serviceId, int tpId, int pmtPid)
    {
        String where = DBService.SERVICE_ID + "= " + serviceId + " and " + DBService.TP_ID + " = " + tpId;
        ContentValues values = new ContentValues();
        values.put(DBService.PME_ID, pmtPid);
        context.getContentResolver().update(DBService.CONTENT_URI, values, where, null);
    }
    
}
