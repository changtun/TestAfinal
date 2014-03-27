/*
 * File Name:  MailDao.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-07
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.dao;

import java.util.List;

import com.pbi.dvb.domain.MailBean;

public interface MailDao
{
    
    /**
     * 
     * <add mail>
     * 
     * @param mailBean
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void addMailInfo(MailBean mailBean);
    
    /**
     * 
     * <get mails>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<MailBean> getMailInfo();
    
    /**
     * 
     * <get specified number mails.>
     * 
     * @param number
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract List<MailBean> getPartMailInfo(int number);
    
    /**
     * 
     * <get mails by mail id.>
     * 
     * @param mailId
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract MailBean getMailInfoById(int mailId);
    
    /**
     * 
     * <delete mail by mail id.>
     * 
     * @param mailId
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void deleteMailById(int mailId);
    
    /**
     * 
     * <delete all mails.>
     * 
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void deleteMail();
    
    /**
     * 
     * <update mail status>
     * 
     * @param mailId
     * @param status 0:unread, 1:read 
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updateMailStatus(int mailId, int status);
    
    /**
     * 
     * <get mail count.>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract long getMailAccounts();
    
    /**
     * 
     * <get mail by mail index.>
     * 
     * @param mailIndex
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract int getMailIdByIdIndex(int mailIndex);
    
    /**
     * 
     * <get unread mail information.>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract long getUnreadMailInfo();
    
    /**
     * 
     * <update mail info by mail id.>
     * 
     * @param mailId
     * @param mBean
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract void updateMailInfo(int mailId, MailBean mBean);
    
    /**
     * 
     * <get mail by mail type,which mail type is 2.>
     * 
     * @return
     * @see [Class, Class#Method, Class#Member]
     */
    public abstract MailBean getMailInfoByType2();
    
}