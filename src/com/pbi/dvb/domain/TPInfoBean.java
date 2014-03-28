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

@Table(name = "db_tp_info_table")
public class TPInfoBean
{
    private int id;
    
    private Integer tpId;
    
    private int tunerType;
    
    private char tunerId;
    
    private Integer netId;
    
    private Integer originalNetId;
    
    private Integer tranStreamId;
    
    private Integer tunerFreq;
    
    private Integer tunerSymbRate;
    
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
    
    public int getId()
    {
        return id;
    }
    
    public void setId(int id)
    {
        this.id = id;
    }
    
    public void setEfecInner(Integer efecInner)
    {
        this.efecInner = efecInner;
    }
    
    public void setEfecOuter(Integer efecOuter)
    {
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
