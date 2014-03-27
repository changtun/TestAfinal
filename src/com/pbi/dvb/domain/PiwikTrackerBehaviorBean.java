package com.pbi.dvb.domain;

import java.util.ArrayList;
import java.util.List;

import com.pbi.dvb.utils.LogUtils;

import android.os.Parcel;
import android.os.Parcelable;

public class PiwikTrackerBehaviorBean implements Parcelable
{
    public static final int PUSH_BEHAVIOR = 0;
    
    public static final int DISCONNECT_FROM_SERVER = 1;
    
    private static final String TAG = "PiwikTrackerBehaviorBean";
    
    private List<CustomerVarBean> customerVars = new ArrayList<CustomerVarBean>();
    
    private boolean unInterruptable = false;
    
    private Integer type = null;
    
    public PiwikTrackerBehaviorBean()
    {
        unInterruptable = false;
    }
    
    public void addCustomerVar(String name, String value)
    {
        CustomerVarBean varBean = new CustomerVarBean();
        
        varBean.setName(name);
        varBean.setValue(value);
        
        customerVars.add(varBean);
    }
    
    public int clearCustomerVars()
    {
        int size = customerVars == null ? 0 : customerVars.size();
        
        customerVars.clear();
        
        return size;
    }
    
    public Integer getType()
    {
        return type;
    }
    
    public void setType(Integer type)
    {
        this.type = type;
    }
    
    public List<CustomerVarBean> getCustomerVars()
    {
        return customerVars;
    }
    
    public String[] tranlateCustomerVarsToArray()
    {
        if (customerVars == null || customerVars.size() <= 0)
        {
            return null;
        }
        
        String[] result = new String[customerVars.size() * 2];
        
        for (int i = 0; i / 2 < customerVars.size();)
        {
            CustomerVarBean var = customerVars.get(i / 2);
            
            result[i++] = var.getName();
            result[i++] = var.getValue();
        }
        
        for (int i = 0; i < result.length;)
        {
            LogUtils.e(TAG, "name = " + result[i++] + " value = " + result[i++] + " vars.size = " + customerVars.size());
        }
        
        return result;
    }
    
    public void setCustomerVars(List<CustomerVarBean> customerVars)
    {
        this.customerVars = customerVars;
    }
    
    public boolean checkBehaviorTheSame(PiwikTrackerBehaviorBean behavior)
    {
        boolean result = true;
        
        List<CustomerVarBean> varBeans = behavior.getCustomerVars();
        
        if (customerVars.size() == varBeans.size())
        {
            for (int i = 0; i < customerVars.size(); i++)
            {
                if (!customerVars.get(i).compair(varBeans.get(i)))
                {
                    result = false;
                    
                    break;
                }
            }
        }
        else
        {
            result = false;
        }
        
        return result;
    }
    
    public boolean isUnInterruptable()
    {
        return unInterruptable;
    }
    
    public void setUnInterruptable(boolean unInterruptable)
    {
        this.unInterruptable = unInterruptable;
    }
    
    @Override
    public int describeContents()
    {
        return 0;
    }
    
    @Override
    public void writeToParcel(Parcel parcel, int countI)
    {
        byte interruptOption = 0;
        
        if (unInterruptable)
        {
            
            interruptOption = 1;
            
        }
        else
        {
            
            interruptOption = 0;
            
        }
        
        parcel.writeByte(interruptOption);
        parcel.writeInt(type);
        
        parcel.writeInt(customerVars == null ? 0 : customerVars.size());
        
        for (int i = 0; null != customerVars && i < customerVars.size(); i++)
        {
            parcel.writeString(customerVars.get(i).getName());
            
            parcel.writeString(customerVars.get(i).getValue());
        }
    }
    
    public static final Parcelable.Creator<PiwikTrackerBehaviorBean> CREATOR = new Creator<PiwikTrackerBehaviorBean>()
    {
        @Override
        public PiwikTrackerBehaviorBean createFromParcel(Parcel parcel)
        {
            
            PiwikTrackerBehaviorBean behavior = new PiwikTrackerBehaviorBean();
            
            byte interruptOption = parcel.readByte();
            
            if (interruptOption > 0)
            {
                
                behavior.setUnInterruptable(true);
                
            }
            else
            {
                
                behavior.setUnInterruptable(false);
                
            }
            
            behavior.setType(parcel.readInt());
            
            int customerVarLength = parcel.readInt();
            
            // behavior.setProject(parcel.readString());
            // behavior.setAction(parcel.readString());
            
            String name = null;
            
            String value = null;
            
            for (int i = 0; i < customerVarLength; i++)
            {
                name = parcel.readString();
                
                value = parcel.readString();
                
                behavior.addCustomerVar(name, value);
            }
            
            return behavior;
            
        }
        
        @Override
        public PiwikTrackerBehaviorBean[] newArray(int i)
        {
            
            return new PiwikTrackerBehaviorBean[i];
            
        }
    };
    
    @Override
    public String toString()
    {
        return "PiwikTrackerBehaviorBean [customerVars=" + customerVars + ", unInterruptable=" + unInterruptable
            + ", type=" + type + "]";
    }
    
    public String getSurveyPath()
    {
        StringBuilder sb = new StringBuilder();
        
        for (int i = 0; i < customerVars.size(); i++)
        {
            sb.append("/" + customerVars.get(i).getValue());
        }
        
        return sb.toString();
    }
    
    public String getSurveyPathNew()
    {
        StringBuilder sb = new StringBuilder();
        
        for (int i = 0; i < customerVars.size(); i++)
        {
            if (i == 0)
            {
                sb.append("?");
            }
            
            CustomerVarBean var = new CustomerVarBean();
            
            var = customerVars.get(i);
            
            sb.append(var.getName() + "=" + var.getValue());
            
            if (i < customerVars.size() - 1)
            {
                sb.append("&");
            }
        }
        
        return sb.toString();
    }
    
    public String getCallbackURL(String url)
    {
        if (url == null)
        {
            return null;
        }
        else if ("about:blank".equals(url))
        {
            return url;
        }
        
        String result = url;
        
        if (url.endsWith("/"))
        {
            result = url.substring(0, url.lastIndexOf("/"));
        }
        
        result += getSurveyPath();
        
        return result;
    }
    
}
