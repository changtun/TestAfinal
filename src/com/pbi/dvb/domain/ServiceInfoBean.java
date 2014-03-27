package com.pbi.dvb.domain;

public class ServiceInfoBean
{
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
    
    /**
     * ��ȡ channelNumber
     * 
     * @return ���� channelNumber
     */
    public Integer getChannelNumber()
    {
        return channelNumber;
    }
    
    /**
     * ���� channelNumber
     * 
     * @param ��channelNumber���и�ֵ
     */
    public void setChannelNumber(Integer channelNumber)
    {
        this.channelNumber = channelNumber;
    }
    
    /**
     * ��ȡ logicalNumber
     * 
     * @return ���� logicalNumber
     */
    public Integer getLogicalNumber()
    {
        return logicalNumber;
    }
    
    /**
     * ���� logicalNumber
     * 
     * @param ��logicalNumber���и�ֵ
     */
    public void setLogicalNumber(Integer logicalNumber)
    {
        this.logicalNumber = logicalNumber;
    }
    
    /**
     * ��ȡ tpId
     * 
     * @return ���� tpId
     */
    public Integer getTpId()
    {
        return tpId;
    }
    
    /**
     * ���� tpId
     * 
     * @param ��tpId���и�ֵ
     */
    public void setTpId(Integer tpId)
    {
        this.tpId = tpId;
    }
    
    /**
     * ��ȡ tunerType
     * 
     * @return ���� tunerType
     */
    public Integer getTunerType()
    {
        return tunerType;
    }
    
    /**
     * ���� tunerType
     * 
     * @param ��tunerType���и�ֵ
     */
    public void setTunerType(Integer tunerType)
    {
        this.tunerType = tunerType;
    }
    
    /**
     * ��ȡ serviceId
     * 
     * @return ���� serviceId
     */
    public Integer getServiceId()
    {
        return serviceId;
    }
    
    /**
     * ���� serviceId
     * 
     * @param ��serviceId���и�ֵ
     */
    public void setServiceId(Integer serviceId)
    {
        this.serviceId = serviceId;
    }
    
    /**
     * ��ȡ serviceType
     * 
     * @return ���� serviceType
     */
    public char getServiceType()
    {
        return serviceType;
    }
    
    /**
     * ���� serviceType
     * 
     * @param ��serviceType���и�ֵ
     */
    public void setServiceType(char serviceType)
    {
        this.serviceType = serviceType;
    }
    
    /**
     * ��ȡ refServiceId
     * 
     * @return ���� refServiceId
     */
    public Integer getRefServiceId()
    {
        return refServiceId;
    }
    
    /**
     * ���� refServiceId
     * 
     * @param ��refServiceId���и�ֵ
     */
    public void setRefServiceId(Integer refServiceId)
    {
        this.refServiceId = refServiceId;
    }
    
    /**
     * ��ȡ pmtPid
     * 
     * @return ���� pmtPid
     */
    public Integer getPmtPid()
    {
        return pmtPid;
    }
    
    /**
     * ���� pmtPid
     * 
     * @param ��pmtPid���и�ֵ
     */
    public void setPmtPid(Integer pmtPid)
    {
        this.pmtPid = pmtPid;
    }
    
    /**
     * ��ȡ caMode
     * 
     * @return ���� caMode
     */
    public Integer getCaMode()
    {
        return caMode;
    }
    
    /**
     * ���� caMode
     * 
     * @param ��caMode���и�ֵ
     */
    public void setCaMode(int caMode)
    {
        this.caMode = caMode;
    }
    
    /**
     * ��ȡ channelName
     * 
     * @return ���� channelName
     */
    public String getChannelName()
    {
        return channelName;
    }
    
    /**
     * ���� channelName
     * 
     * @param ��channelName���и�ֵ
     */
    public void setChannelName(String channelName)
    {
        this.channelName = channelName;
    }
    
    /**
     * ��ȡ volume
     * 
     * @return ���� volume
     */
    public Integer getVolume()
    {
        return volume;
    }
    
    /**
     * ���� volume
     * 
     * @param ��volume���и�ֵ
     */
    public void setVolume(Integer volume)
    {
        this.volume = volume;
    }
    
    /**
     * ��ȡ videoType
     * 
     * @return ���� videoType
     */
    public Integer getVideoType()
    {
        return videoType;
    }
    
    /**
     * ���� videoType
     * 
     * @param ��videoType���и�ֵ
     */
    public void setVideoType(Integer videoType)
    {
        this.videoType = videoType;
    }
    
    /**
     * ��ȡ videoPid
     * 
     * @return ���� videoPid
     */
    public Integer getVideoPid()
    {
        return videoPid;
    }
    
    /**
     * ���� videoPid
     * 
     * @param ��videoPid���и�ֵ
     */
    public void setVideoPid(Integer videoPid)
    {
        this.videoPid = videoPid;
    }
    
    /**
     * ��ȡ audioMode
     * 
     * @return ���� audioMode
     */
    public Integer getAudioMode()
    {
        return audioMode;
    }
    
    /**
     * ���� audioMode
     * 
     * @param ��audioMode���и�ֵ
     */
    public void setAudioMode(Integer audioMode)
    {
        this.audioMode = audioMode;
    }
    
    /**
     * ��ȡ audioIndex
     * 
     * @return ���� audioIndex
     */
    public Integer getAudioIndex()
    {
        return audioIndex;
    }
    
    /**
     * ���� audioIndex
     * 
     * @param ��audioIndex���и�ֵ
     */
    public void setAudioIndex(Integer audioIndex)
    {
        this.audioIndex = audioIndex;
    }
    
    /**
     * ��ȡ audioType
     * 
     * @return ���� audioType
     */
    public Integer getAudioType()
    {
        return audioType;
    }
    
    /**
     * ���� audioType
     * 
     * @param ��audioType���и�ֵ
     */
    public void setAudioType(Integer audioType)
    {
        this.audioType = audioType;
    }
    
    /**
     * ��ȡ audioPid
     * 
     * @return ���� audioPid
     */
    public Integer getAudioPid()
    {
        return audioPid;
    }
    
    /**
     * ���� audioPid
     * 
     * @param ��audioPid���и�ֵ
     */
    public void setAudioPid(Integer audioPid)
    {
        this.audioPid = audioPid;
    }
    
    /**
     * ��ȡ audioLang
     * 
     * @return ���� audioLang
     */
    public String getAudioLang()
    {
        return audioLang;
    }
    
    /**
     * ���� audioLang
     * 
     * @param ��audioLang���и�ֵ
     */
    public void setAudioLang(String audioLang)
    {
        this.audioLang = audioLang;
    }
    
    /**
     * ��ȡ pcrPid
     * 
     * @return ���� pcrPid
     */
    public Integer getPcrPid()
    {
        return pcrPid;
    }
    
    /**
     * ���� pcrPid
     * 
     * @param ��pcrPid���и�ֵ
     */
    public void setPcrPid(Integer pcrPid)
    {
        this.pcrPid = pcrPid;
    }
    
    /**
     * ��ȡ favFlag
     * 
     * @return ���� favFlag
     */
    public Integer getFavFlag()
    {
        return favFlag;
    }
    
    /**
     * ���� favFlag
     * 
     * @param ��favFlag���и�ֵ
     */
    public void setFavFlag(Integer favFlag)
    {
        this.favFlag = favFlag;
    }
    
    /**
     * ��ȡ lockFlag
     * 
     * @return ���� lockFlag
     */
    public Integer getLockFlag()
    {
        return lockFlag;
    }
    
    /**
     * ���� lockFlag
     * 
     * @param ��lockFlag���и�ֵ
     */
    public void setLockFlag(Integer lockFlag)
    {
        this.lockFlag = lockFlag;
    }
    
    /**
     * ��ȡ moveFlag
     * 
     * @return ���� moveFlag
     */
    public Integer getMoveFlag()
    {
        return moveFlag;
    }
    
    /**
     * ���� moveFlag
     * 
     * @param ��moveFlag���и�ֵ
     */
    public void setMoveFlag(Integer moveFlag)
    {
        this.moveFlag = moveFlag;
    }
    
    /**
     * ��ȡ delFlag
     * 
     * @return ���� delFlag
     */
    public Integer getDelFlag()
    {
        return delFlag;
    }
    
    /**
     * ���� delFlag
     * 
     * @param ��delFlag���и�ֵ
     */
    public void setDelFlag(Integer delFlag)
    {
        this.delFlag = delFlag;
    }
    
    /**
     * ��ȡ channelPosition
     * 
     * @return ���� channelPosition
     */
    public Integer getChannelPosition()
    {
        return channelPosition;
    }
    
    /**
     * ���� channelPosition
     * 
     * @param ��channelPosition���и�ֵ
     */
    public void setChannelPosition(Integer channelPosition)
    {
        this.channelPosition = channelPosition;
    }
    
    /**
     * ��ȡ tpInfoBean
     * 
     * @return ���� tpInfoBean
     */
    public TPInfoBean getTpInfoBean()
    {
        return tpInfoBean;
    }
    
    /**
     * ���� tpInfoBean
     * 
     * @param ��tpInfoBean���и�ֵ
     */
    public void setTpInfoBean(TPInfoBean tpInfoBean)
    {
        this.tpInfoBean = tpInfoBean;
    }
    
}
