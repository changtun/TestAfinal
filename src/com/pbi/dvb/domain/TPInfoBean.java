/*
 * �� �� ��:  TPInfoBean.java
 * ��    Ȩ:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  gtsong
 * �޸�ʱ��:  2012-10-29
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.domain;

/**
 * 
 * <TP Entity>
 * 
 * @author gtsong
 * @version [�汾��, 2012-7-19]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class TPInfoBean
{
    // Ƶ��id
    private Integer tpId;
    
    // tuner����
    private int tunerType;
    
    // tunerId
    private char tunerId;
    
    // ����id
    private Integer netId;
    
    // ԭʼ����id
    private Integer originalNetId;
    
    // ������id
    private Integer tranStreamId;
    
    // tunerƵ��
    private Integer tunerFreq;
    
    // tuner�ķ�����
    private Integer tunerSymbRate;
    
    // tuner�ĵ��Է�ʽ
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
