package com.pbi.dvb.domain;

public class CustomerVarBean
{
    private String name = null;
    
    private String value = null;
    
    public String getName()
    {
        return name;
    }
    
    public void setName(String name)
    {
        this.name = name;
    }
    
    public String getValue()
    {
        return value;
    }
    
    public void setValue(String value)
    {
        this.value = value;
    }
    
    public boolean compair(CustomerVarBean bean)
    {
        boolean result = true;
        
        if (null != name && null != bean.getName() && !name.equals(bean.getName()))
        {
            result = false;
        }
        else if (null != name && null == bean.getName())
        {
            result = false;
        }
        else if (null == name && null != bean.getName())
        {
            result = false;
        }
        
        if (null != value && null != bean.getValue() && !value.equals(bean.getValue()))
        {
            result = false;
        }
        else if (null != value && null == bean.getValue())
        {
            result = false;
        }
        else if (null == value && null != bean.getValue())
        {
            result = false;
        }
        
        return result;
    }
    
    @Override
    public String toString()
    {
        return "CustomerVarBean [name=" + name + ", value=" + value + "]";
    }
    
}
