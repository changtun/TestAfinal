/*
 * File Name:  ServiceInfoBean.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.domain;

import net.tsz.afinal.annotation.sqlite.Table;

@Table(name = "db_service_info_table")
public class ServiceInfoBean
{
    private Integer id;
    private Integer channelNumber;
    
    private Integer logicalNumber;
    
    private Integer tpId;
    
    private Integer tunerType;
    
    private Integer serviceId;
    
    private char serviceType;
    
    private Integer refServiceId;
    
    private Integer pmtPid;
    
    private Integer caMode;
    
    private String channelName;
    
    private Integer volume;
    
    private Integer videoType;
    
    private Integer videoPid;
    
    private Integer audioMode;
    
    private Integer audioIndex;
    
    private Integer audioType;
    
    private Integer audioPid;
    
    private String audioLang;
    
    private Integer pcrPid;
    
    private Integer favFlag;
    
    private Integer lockFlag;
    
    private Integer moveFlag;
    
    private Integer delFlag;
    
    private Integer channelPosition;
    
    private TPInfoBean tpInfoBean;
    
    public ServiceInfoBean()
    {
        super();
    }
    
    public ServiceInfoBean(Integer channelNumber, Integer logicalNumber, Integer tpId, Integer tunerType,
        Integer serviceId, char serviceType, Integer refServiceId, Integer pmtPid, Integer caMode, String channelName,
        Integer volume, Integer videoType, Integer videoPid, Integer audioMode, Integer audioIndex, Integer audioType,
        Integer audioPid, String audioLang, Integer pcrPid, Integer favFlag, Integer lockFlag, Integer moveFlag,
        Integer delFlag, Integer channelPosition)
    {
        super();
        this.channelNumber = channelNumber;
        this.logicalNumber = logicalNumber;
        this.tpId = tpId;
        this.tunerType = tunerType;
        this.serviceId = serviceId;
        this.serviceType = serviceType;
        this.refServiceId = refServiceId;
        this.pmtPid = pmtPid;
        this.caMode = caMode;
        this.channelName = channelName;
        this.volume = volume;
        this.videoType = videoType;
        this.videoPid = videoPid;
        this.audioMode = audioMode;
        this.audioIndex = audioIndex;
        this.audioType = audioType;
        this.audioPid = audioPid;
        this.audioLang = audioLang;
        this.pcrPid = pcrPid;
        this.favFlag = favFlag;
        this.lockFlag = lockFlag;
        this.moveFlag = moveFlag;
        this.delFlag = delFlag;
        this.channelPosition = channelPosition;
    }
    
    
    public Integer getId()
    {
        return id;
    }

    public void setId(Integer id)
    {
        this.id = id;
    }

    public Integer getChannelNumber()
    {
        return channelNumber;
    }
    
    public void setChannelNumber(Integer channelNumber)
    {
        this.channelNumber = channelNumber;
    }
    
    public Integer getLogicalNumber()
    {
        return logicalNumber;
    }

    public void setLogicalNumber(Integer logicalNumber)
    {
        this.logicalNumber = logicalNumber;
    }

    public Integer getTpId()
    {
        return tpId;
    }

    public void setTpId(Integer tpId)
    {
        this.tpId = tpId;
    }

    public Integer getTunerType()
    {
        return tunerType;
    }

    public void setTunerType(Integer tunerType)
    {
        this.tunerType = tunerType;
    }

    public Integer getServiceId()
    {
        return serviceId;
    }

    public void setServiceId(Integer serviceId)
    {
        this.serviceId = serviceId;
    }

    public char getServiceType()
    {
        return serviceType;
    }

    public void setServiceType(char serviceType)
    {
        this.serviceType = serviceType;
    }

    public Integer getRefServiceId()
    {
        return refServiceId;
    }

    public void setRefServiceId(Integer refServiceId)
    {
        this.refServiceId = refServiceId;
    }

    public Integer getPmtPid()
    {
        return pmtPid;
    }

    public void setPmtPid(Integer pmtPid)
    {
        this.pmtPid = pmtPid;
    }

    public Integer getCaMode()
    {
        return caMode;
    }

    public void setCaMode(int caMode)
    {
        this.caMode = caMode;
    }

    public String getChannelName()
    {
        return channelName;
    }

    public void setChannelName(String channelName)
    {
        this.channelName = channelName;
    }

    public Integer getVolume()
    {
        return volume;
    }

    public void setVolume(Integer volume)
    {
        this.volume = volume;
    }

    public Integer getVideoType()
    {
        return videoType;
    }

    public void setVideoType(Integer videoType)
    {
        this.videoType = videoType;
    }

    public Integer getVideoPid()
    {
        return videoPid;
    }

    public void setVideoPid(Integer videoPid)
    {
        this.videoPid = videoPid;
    }

    public Integer getAudioMode()
    {
        return audioMode;
    }

    public void setAudioMode(Integer audioMode)
    {
        this.audioMode = audioMode;
    }

    public Integer getAudioIndex()
    {
        return audioIndex;
    }

    public void setAudioIndex(Integer audioIndex)
    {
        this.audioIndex = audioIndex;
    }

    public Integer getAudioType()
    {
        return audioType;
    }

    public void setAudioType(Integer audioType)
    {
        this.audioType = audioType;
    }

    public Integer getAudioPid()
    {
        return audioPid;
    }

    public void setAudioPid(Integer audioPid)
    {
        this.audioPid = audioPid;
    }

    public String getAudioLang()
    {
        return audioLang;
    }

    public void setAudioLang(String audioLang)
    {
        this.audioLang = audioLang;
    }

    public Integer getPcrPid()
    {
        return pcrPid;
    }

    public void setPcrPid(Integer pcrPid)
    {
        this.pcrPid = pcrPid;
    }

    public Integer getFavFlag()
    {
        return favFlag;
    }

    public void setFavFlag(Integer favFlag)
    {
        this.favFlag = favFlag;
    }

    public Integer getLockFlag()
    {
        return lockFlag;
    }

    public void setLockFlag(Integer lockFlag)
    {
        this.lockFlag = lockFlag;
    }

    public Integer getMoveFlag()
    {
        return moveFlag;
    }

    public void setMoveFlag(Integer moveFlag)
    {
        this.moveFlag = moveFlag;
    }

    public Integer getDelFlag()
    {
        return delFlag;
    }

    public void setDelFlag(Integer delFlag)
    {
        this.delFlag = delFlag;
    }

    public Integer getChannelPosition()
    {
        return channelPosition;
    }

    public void setChannelPosition(Integer channelPosition)
    {
        this.channelPosition = channelPosition;
    }

    public TPInfoBean getTpInfoBean()
    {
        return tpInfoBean;
    }

    public void setTpInfoBean(TPInfoBean tpInfoBean)
    {
        this.tpInfoBean = tpInfoBean;
    }
    
}
