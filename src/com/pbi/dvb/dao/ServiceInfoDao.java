/*
 * File Name:  ServiceInfoDao.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-07
 * Changed Content:  <add update soundtrack and language method.>
 */
package com.pbi.dvb.dao;

import java.util.List;

import android.content.ContentProviderOperation;

import com.pbi.dvb.domain.ServiceInfoBean;

public interface ServiceInfoDao
{
    /**
     * 
     * <add service>
     * 
     * @param serviceInfoBean
     * @param position
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ContentProviderOperation addServiceInfo(ServiceInfoBean serviceInfoBean, int position);
    
    /**
     * 
     * <get all services>
     * 
     * @param hasCaMode
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<ServiceInfoBean> getAllChannelInfo(boolean hasCaMode);
    
    /**
     * 
     * <get tv services>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<ServiceInfoBean> getTVChannelInfo();
    
    /**
     * 
     * <get radio services>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<ServiceInfoBean> getRadioChannelInfo();
    
    /**
     * 
     * <get favourite services>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<ServiceInfoBean> getFavChannelInfo();
    
    /**
     * 
     * <get service by service id and logical number>
     * 
     * @param serviceId
     * @param logicalNumber
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ServiceInfoBean getChannelInfoByServiceId(int serviceId, int logicalNumber);
    
    /**
     * 
     * <get service by logical number>
     * 
     * @param logicalNumber
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<ServiceInfoBean> getChannelIdByLogicalNumber(int logicalNumber);
    
    /**
     * 
     * <clear all services in db>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ContentProviderOperation clearServiceData();
    
    /**
     * 
     * <clear the current logical number's service>
     * 
     * @param channelNumber
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ContentProviderOperation clearServiceDataByChannelNumber(int channelNumber);
    
    /**
     * 
     * <clear all services.>
     * 
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void clearService();
    
    /**
     * 
     * <update the current logical number's service.>
     * 
     * @param sBean
     * @param channelNumber
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ContentProviderOperation updateServiceSettings(ServiceInfoBean sBean, int channelNumber);
    
    /**
     * 
     * <get service by tp id and service id>
     * 
     * @param tpId
     * @param serviceid
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract ServiceInfoBean getChannelInfo(int tpId, int serviceid);
    
    /**
     * 
     * <update audio information by service id>
     * 
     * @param serviceId
     * @param audioPid
     * @param audioType
     * @param audioLang
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updateAudioInfo(int serviceId, int audioPid, int audioType, int audioMode);
    
    /**
     * 
     * <update ca mode by service id and logical number>
     * 
     * @param serviceId
     * @param logicalNumber
     * @param caMode :whether the service name has '$' or not
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updateCaMode(int serviceId, int logicalNumber, int caMode);
    
    /**
     * 
     * <update the pmtpid by service id and tp id>
     * 
     * @param serviceId
     * @param tpId
     * @param pmtPid
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updatePmtPid(int serviceId, int tpId, int pmtPid);
    
}