/*
 * File Name:  MountInfoBean.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.domain;

public class MountInfoBean
{
    private String path;
    private int type;
    private String label;
    private String partition;
    private long totleSize;
    private long freeSize;
    
    public long getTotleSize()
    {
        return totleSize;
    }
    
    public void setTotleSize(long totleSize)
    {
        this.totleSize = totleSize;
    }
    
    public long getFreeSize()
    {
        return freeSize;
    }
    
    public void setFreeSize(long freeSize)
    {
        this.freeSize = freeSize;
    }
    
    public String getPath()
    {
        return path;
    }
    
    public void setPath(String path)
    {
        this.path = path;
    }
    
    public int getType()
    {
        return type;
    }
    
    public void setType(int type)
    {
        this.type = type;
    }
    
    public String getLabel()
    {
        return label;
    }
    
    public void setLabel(String label)
    {
        this.label = label;
    }
    
    public String getPartition()
    {
        return partition;
    }
    
    public void setPartition(String partition)
    {
        this.partition = partition;
    }
    
}
