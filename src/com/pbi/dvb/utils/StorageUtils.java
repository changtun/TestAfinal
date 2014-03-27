/*
 * File Name:  StorageUtils.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.utils;

import java.util.ArrayList;
import java.util.List;
import java.io.File;

import android.content.Context;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.util.Log;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.MountInfoBean;

/**
 * 
 * <storage utils>
 * 
 * @author gtsong
 * @version [Version Number, 2013-12-19]
 * @see [Relevant Class/Method]
 * @since [Product/Module Version]
 */
public class StorageUtils
{
    private static final String TAG = "StorageUtils";
    
    private StorageManager mStorageManager;
    
    private Context context;
    
    public StorageUtils(Context context)
    {
        super();
        this.context = context;
        mStorageManager = (StorageManager)context.getSystemService(Context.STORAGE_SERVICE);
    }
    
    /**
     * 
     * <check the external devices status>
     * 
     * @return 0:success 1:not enough spaces 2:no devices
     * @see [Class,Class#Method,Class#Member]
     */
    public int checkHDDSpace()
    {
        long freeSize;
        MountInfoBean bean = null;
        int result = 0;
        try
        {
            bean = getMobileHDDInfo();
            if (null != bean)
            {
                freeSize = bean.getFreeSize();
                LogUtils.printLog(1, 3, TAG, "--->>>check the external device free size is: " + freeSize);
                if (freeSize < Integer.parseInt(context.getString(R.string.free_space)))
                {
                    result = 1;
                }
            }
            else
            {
                result = 2;
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return result;
    }
    
    /**
     * 
     * <get usb stroage devices>
     * 
     * @return usb stroage devices
     * @see [Class,Class#Method,Class#Member]
     */
    public MountInfoBean getMobileHDDInfo()
    {
        List<MountInfoBean> list = getAllMountDevices();
        long totleSize = 0;
        long freeSize = 0;
        MountInfoBean mBean = null;
        android.os.StatFs statfs = null;
        
        if (null == list)
        {
            return null;
        }
        
        for (MountInfoBean bean : list)
        {
            // usb stroage.
            if (bean.getType() == 1)
            {
                try
                {
                    statfs = new android.os.StatFs(bean.getPath());
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                long[] deviceInfo = getStorageSize(statfs);
                if (deviceInfo != null && deviceInfo.length > 0)
                {
                    totleSize = deviceInfo[0];
                    freeSize = deviceInfo[1];
                    bean.setTotleSize(totleSize);
                    bean.setFreeSize(freeSize);
                    mBean = bean;
                    break;
                }
            }
        }
        return mBean;
    }
    
    /**
     * <get all external devices> <Functional Details>
     * 
     * @return external devices collection.
     * @see [Class,Class#Method,Class#Member]
     */
    private List<MountInfoBean> getAllMountDevices()
    {
        List<MountInfoBean> list = new ArrayList<MountInfoBean>();
        MountInfoBean bean = null;
        String path = null;
        StorageVolume[] storageVolumes = mStorageManager.getVolumeList();
        
        if (null != storageVolumes)
        {
            for (int i = 0; i < storageVolumes.length; i++)
            {
                bean = new MountInfoBean();
                path = storageVolumes[i].getPath();
                bean.setPath(path);
                bean.setType(getPathType(path));
                list.add(bean);
            }
        }
        return list;
    }
    
    private int getPathType(String path)
    {
        if (path.contains("/mnt/nand"))
        {
            return (3);
        }
        else if (path.contains("/mnt/sdcard"))
        {
            return (2);
        }
        else if (!path.contains("/mnt/sdcard") && path.contains("mnt/sd"))
        {
            return (1);
        }
        // other devices.
        else
        {
            return (0);
        }
    }
    
    /**
     * 
     * <Functional overview> <Functional Details>
     * 
     * @param statfs
     * @return long type array,array[0]:total size,array[1] free size
     * @see [Class,Class#Method,Class#Member]
     */
    private long[] getStorageSize(android.os.StatFs statfs)
    {
        if(null == statfs)
        {
            return null;
        }
        long[] result = null;
        long nSDFreeSize = 0L;
        // get block mounts in the sdcard
        long nTotalBlocks = statfs.getBlockCount();
        // get every block's size.
        long nBlocSize = statfs.getBlockSize();
        // get the storage's total size(MB).
        long nSDTotalSize = nTotalBlocks * nBlocSize / 1024 / 1024;
        
        if (nSDTotalSize > Integer.parseInt(context.getString(R.string.min_space)))
        {
            // get the storage's avaliable block mounts.
            long nAvailaBlock = statfs.getAvailableBlocks();
            // get the storage's avaliable size(MB).
            nSDFreeSize = nAvailaBlock * nBlocSize / 1024 / 1024;
            result = new long[2];
            result[0] = nSDTotalSize;
            result[1] = nSDFreeSize;
        }
        return result;
    }
    
}
