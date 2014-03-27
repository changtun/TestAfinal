/*
 * File Name:  TpInfoDao.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <add update soundtrack and language method.>
 */
package com.pbi.dvb.dao;

import com.pbi.dvb.domain.TPInfoBean;

public interface TpInfoDao
{
    
    /**
     * 
     * <add tp>
     * 
     * @param tpInfoBean
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void addTpInfo(TPInfoBean tpInfoBean);
    
    /**
     * 
     * <get tp by frequency and symbol rate>
     * 
     * @param freq
     * @param symbRate
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract int getTpId(int freq, int symbRate);
    
    public abstract int getTpId(int freq, int symbRate, int mod);
    
    /**
     * 
     * <get tp by network id and ts id.>
     * 
     * @param netid
     * @param tsid
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract int getTpIdByNetId(int netid, int tsid);
    
    /**
     * 
     * <get tp by ts id.>
     * 
     * @param tsId
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract int getTpId(int tsId);
    
    /**
     * 
     * <get tp by tp id.>
     * 
     * @param tpId
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract TPInfoBean getTpInfo(int tpId);
    
    /**
     * 
     * <update tp information by tp id.>
     * 
     * @param tpInfoBean
     * @param tpId
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updateServiceInfo(TPInfoBean tpInfoBean, int tpId);
    
    /**
     * 
     * <clear tp table.>
     * 
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void clearTpData();
    
}