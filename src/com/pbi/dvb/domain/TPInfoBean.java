/*
 * 文 件 名:  TPInfoBean.java
 * 版    权:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  gtsong
 * 修改时间:  2012-10-29
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.domain;

/**
 * 
 * <TP Entity>
 * 
 * @author gtsong
 * @version [版本号, 2012-7-19]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class TPInfoBean
{
    // 频点id
    private Integer tpId;
    
    // tuner类型
    private int tunerType;
    
    // tunerId
    private char tunerId;
    
    // 网络id
    private Integer netId;
    
    // 原始网络id
    private Integer originalNetId;
    
    // 传输流id
    private Integer tranStreamId;
    
    // tuner频率
    private Integer tunerFreq;
    
    // tuner的符码率
    private Integer tunerSymbRate;
    
    // tuner的调试方式
    private int tunerEmod;
    
    private Integer efecInner;
    
    private Integer efecOuter;
    
    public TPInfoBean()
    {
        super();
    }
    
    public TPInfoBean(Integer tpId, int tunerType, char tunerId, Integer netId, Integer originalNetId,
        Integer tranStreamId, Integer tunerFreq, Integer tunerSymbRate, int tunerEmod, Integer efecInner,
        Integer efecOuter)
    {
        super();
        this.tpId = tpId;
        this.tunerType = tunerType;
        this.tunerId = tunerId;
        this.netId = netId;
        this.originalNetId = originalNetId;
        this.tranStreamId = tranStreamId;
        this.tunerFreq = tunerFreq;
        this.tunerSymbRate = tunerSymbRate;
        this.tunerEmod = tunerEmod;
        this.efecInner = efecInner;
        this.efecOuter = efecOuter;
    }
    
    public Integer getTpId()
    {
        return tpId;
    }
    
    public void setTpId(Integer tpId)
    {
        this.tpId = tpId;
    }
    
    public int getTunerType()
    {
        return tunerType;
    }
    
    public void setTunerType(int tunerType)
    {
        this.tunerType = tunerType;
    }
    
    public char getTunerId()
    {
        return tunerId;
    }
    
    public void setTunerId(char tunerId)
    {
        this.tunerId = tunerId;
    }
    
    public Integer getNetId()
    {
        return netId;
    }
    
    public void setNetId(Integer netId)
    {
        this.netId = netId;
    }
    
    public Integer getOriginalNetId()
    {
        return originalNetId;
    }
    
    public void setOriginalNetId(Integer originalNetId)
    {
        this.originalNetId = originalNetId;
    }
    
    public Integer getTranStreamId()
    {
        return tranStreamId;
    }
    
    public void setTranStreamId(Integer tranStreamId)
    {
        this.tranStreamId = tranStreamId;
    }
    
    public Integer getTunerFreq()
    {
        return tunerFreq;
    }
    
    public void setTunerFreq(Integer tunerFreq)
    {
        this.tunerFreq = tunerFreq;
    }
    
    public Integer getTunerSymbRate()
    {
        return tunerSymbRate;
    }
    
    public void setTunerSymbRate(Integer tunerSymbRate)
    {
        this.tunerSymbRate = tunerSymbRate;
    }
    
    public int getTunerEmod()
    {
        return tunerEmod;
    }
    
    public void setTunerEmod(int tunerEmod)
    {
        this.tunerEmod = tunerEmod;
    }
    
    public int getEfecInner()
    {
        return efecInner;
    }
    
    public void setEfecInner(int efecInner)
    {
        this.efecInner = efecInner;
    }
    
    public int getEfecOuter()
    {
        return efecOuter;
    }
    
    public void setEfecOuter(int efecOuter)
    {
        this.efecOuter = efecOuter;
    }
    
}
