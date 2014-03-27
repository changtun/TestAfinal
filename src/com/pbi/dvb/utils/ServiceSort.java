package com.pbi.dvb.utils;

import java.util.Comparator;

import com.pbi.dvb.domain.ServiceInfoBean;

public class ServiceSort implements Comparator<ServiceInfoBean>
{
    public int compare(ServiceInfoBean lsBean, ServiceInfoBean rsBean)
    {
        int typeSort = lsBean.getServiceId() - rsBean.getServiceId();
        return typeSort;
    }
    
}
