package com.pbi.dvb.domain;

public class MailBean
{
    private Integer mailId;
    private Integer mailIndex;
    private int mailType;
    private Integer mailStatus;
    private int mailClass;
    private int mailPriority;
    private int mailPeriod;
    private String mailFrom;
    private String mailTitle;
    private String mailTime;
    private String mailContent;
   
    
    
    public MailBean()
    {
        super();
    }

    public MailBean(int mailId, int mailIndex,int mailType,  int mailStatus ,int mailClass, int mailPriority,int mailPeriod,String mailFrom,String mailTitle, String mailTime,
        String mailContent)
    {
        super();
        this.mailId = mailId;
        this.mailIndex = mailIndex;
        this.mailType = mailType;
        this.mailStatus = mailStatus;
        this.mailClass = mailClass;
        this.mailPeriod = mailPeriod;
        this.mailPriority = mailPriority;
        this.mailFrom = mailFrom;
        this.mailTitle = mailTitle;
        this.mailTime = mailTime;
        this.mailContent = mailContent;
        
    }
    
    public Integer getMailId()
    {
        return mailId;
    }
    
    public void setMailId(Integer mailId)
    {
        this.mailId = mailId;
    }
    
    public Integer getMailIndex()
    {
        return mailIndex;
    }
    
    public void setMailIndex(Integer mailIndex)
    {
        this.mailIndex = mailIndex;
    }
    
    public int getMailType()
    {
        return mailType;
    }
    
    public void setMailType(int mailType)
    {
        this.mailType = mailType;
    }
    
    public String getMailTitle()
    {
        return mailTitle;
    }
    
    public void setMailTitle(String mailTitle)
    {
        this.mailTitle = mailTitle;
    }
    
    public String getMailTime()
    {
        return mailTime;
    }
    
    public void setMailTime(String mailTime)
    {
        this.mailTime = mailTime;
    }
    
    public Integer getMailStatus()
    {
        return mailStatus;
    }
    
    public void setMailStatus(Integer mailStatus)
    {
        this.mailStatus = mailStatus;
    }
    
    public String getMailContent()
    {
        return mailContent;
    }
    
    public void setMailContent(String mailContent)
    {
        this.mailContent = mailContent;
    }

    public int getMailClass()
    {
        return mailClass;
    }

    public void setMailClass(int mailClass)
    {
        this.mailClass = mailClass;
    }

    public int getMailPriority()
    {
        return mailPriority;
    }

    public void setMailPriority(int mailPriority)
    {
        this.mailPriority = mailPriority;
    }

    public int getMailPeriod()
    {
        return mailPeriod;
    }

    public void setMailPeriod(int mailPeriod)
    {
        this.mailPeriod = mailPeriod;
    }

    public String getMailFrom()
    {
        return mailFrom;
    }

    public void setMailFrom(String mailFrom)
    {
        this.mailFrom = mailFrom;
    }
    
    
}
