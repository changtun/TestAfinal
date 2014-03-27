/*
 * 文 件 名:  ConfigBean.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd.Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  
 * 修改时间:  2013-6-4
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.domain;


/**
 * </mnt/expand/dvb_config 文件对应的实体>
 * 
 * @author 姓名 工号
 * @version [版本号, 2013-6-4]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class ConfigBean
{
    public static final String DVB_CONFIG_PATH = "/mnt/expand/dvb_config";
    private static final String TAG = "ConfigBean";
    
    private int bandwidth;
    private int batLimit;
    private int encodeType;
    
    public ConfigBean()
    {
        super();
    }
    
    public int getBandwidth()
    {
        return bandwidth;
    }
    
    public void setBandwidth(int bandwidth)
    {
        this.bandwidth = bandwidth;
    }
    
    public int getBatLimit()
    {
        return batLimit;
    }
    
    public void setBatLimit(int batLimit)
    {
        this.batLimit = batLimit;
    }
    
    public int getEncodeType()
    {
        return encodeType;
    }
    
    public void setEncodeType(int encodeType)
    {
        this.encodeType = encodeType;
    }
    
    public ConfigBean getConfigParams()
    {
        ConfigBean configBean = new ConfigBean();
        getParams(configBean);
        return configBean;
        
//        List<String> data = FileUtils.readFileByLines(DVB_CONFIG_PATH);
//        ConfigBean configBean = new ConfigBean();
//        if (null != data)
//        {
//            for (int i = 0; i < data.size(); i++)
//            {
//                String content = data.get(i);
//                
//                if (content.startsWith("#"))
//                {
//                    // 过滤注释
//                }
//                else
//                {
//                    String[] split = content.split("=");
//                    if (null != split)
//                    {
//                        String str = split[1].substring(0,split[1].length()-1);
//                        if (split[0].equals("TunerBandWidth"))
//                        {
//                            configBean.setBandwidth(Integer.parseInt(str));
//                        }
//                        if (split[0].equals("SearchBAT"))
//                        {
//                            configBean.setBatLimit(Integer.parseInt(str));
//                        }
//                        if (split[0].equals("DefaultCharacterEncode"))
//                        {
//                            configBean.setEncodeType(Integer.parseInt(str));
//                        }
//                    }
//                }
//            }
//        }
//        return configBean;
    }
    
    public void setConfigParams(ConfigBean cBean)
    {
        setParams(cBean);
//        List<String> newData = new ArrayList<String>();
//        List<String> data = FileUtils.readFileByLines(DVB_CONFIG_PATH);
//        if (null != data)
//        {
//            for (int i = 0; i < data.size(); i++)
//            {
//                String content = data.get(i);
//                
//                if (content.startsWith("#"))
//                {
//                    // 注释
//                    LogUtils.printLog(1, 3,TAG, "-------->>>>array comment ----->>>" + content);
//                    newData.add(content);
//                }
//                else
//                {
//                    LogUtils.printLog(1, 3,TAG, "-------->>>>array params ----->>>" + content);
//                    String[] split = content.split("=");
//                    if (null != split)
//                    {
//                        if (split[0].equals("TunerBandWidth"))
//                        {
//                            split[1] = String.valueOf(cBean.getBandwidth()); 
//                            String bandwidth = split[0] + "=" + split[1] + ".";
//                            LogUtils.printLog(1, 3,TAG, "---->>> after alert :: " + bandwidth);
//                            newData.add(bandwidth);
//                        }
//                        if (split[0].equals("SearchBAT"))
//                        {
//                            split[1] = String.valueOf(cBean.getBatLimit());
//                            String bat = split[0] + "=" + split[1] + ".";
//                            LogUtils.printLog(1, 3,TAG, "---->>> after alert :: " + bat);
//                            newData.add(bat);
//                        }
//                        if (split[0].equals("DefaultCharacterEncode"))
//                        {
//                            split[1] = String.valueOf(cBean.getEncodeType());
//                            String encode = split[0] + "=" + split[1] + ".";
//                            LogUtils.printLog(1, 3,TAG, "---->>> after alert :: " + encode);
//                            newData.add(encode);
//                        }
//                    }
//                }
//            }
//        }
//        
//        FileUtils.writeFile(DVB_CONFIG_PATH, newData);
    }
    
    
    private native void getParams(ConfigBean cBean);
    private native void setParams(ConfigBean cBean);
    
}
