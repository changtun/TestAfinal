/*
 * File Name:  TpInfoDaoImpl.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <add update soundtrack and language method.>
 */
package com.pbi.dvb.dao.impl;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;

import com.pbi.dvb.dao.TpInfoDao;
import com.pbi.dvb.db.DBTp;
import com.pbi.dvb.domain.TPInfoBean;

public class TpInfoDaoImpl implements TpInfoDao
{
    private Context context;
    
    public TpInfoDaoImpl(Context context)
    {
        this.context = context;
    }
    
    public void addTpInfo(TPInfoBean tpInfoBean)
    {
        ContentValues values = new ContentValues();
        values.put(DBTp.TUNER_TYPE, tpInfoBean.getTunerType());
        values.put(DBTp.TUNER_ID, String.valueOf(tpInfoBean.getTunerId()));
        values.put(DBTp.NET_ID, tpInfoBean.getNetId());
        values.put(DBTp.ORIGINAL_NET_ID, tpInfoBean.getOriginalNetId());
        values.put(DBTp.TS_ID, tpInfoBean.getTranStreamId());
        values.put(DBTp.FREQUENCY, tpInfoBean.getTunerFreq());
        values.put(DBTp.SYMB_RATE, tpInfoBean.getTunerSymbRate());
        values.put(DBTp.EMOD, tpInfoBean.getTunerEmod());
        values.put(DBTp.EFEC_INNER, tpInfoBean.getEfecInner());
        values.put(DBTp.EFEC_OUTER, tpInfoBean.getEfecOuter());
        context.getContentResolver().insert(DBTp.CONTENT_URI, values);
    }
    
    public int getTpId(int freq, int symbRate)
    {
        int tpId = 0;
        String[] projection = new String[] {DBTp.TP_ID};
        String selection = DBTp.FREQUENCY + " =" + freq + " and " + DBTp.SYMB_RATE + " =" + symbRate;
        Cursor cursor = context.getContentResolver().query(DBTp.CONTENT_URI, projection, selection, null, null);
        if (cursor.moveToFirst())
        {
            int tpIdIndex = cursor.getColumnIndex(DBTp.TP_ID);
            tpId = cursor.getInt(tpIdIndex);
        }
        cursor.close();
        return tpId;
    }
    
    public int getTpId(int freq, int symbRate, int mod)
    {
        int tpId = 0;
        String[] projection = new String[] {DBTp.TP_ID};
        String selection =
            DBTp.FREQUENCY + " =" + freq + " and " + DBTp.SYMB_RATE + " =" + symbRate + " and " + DBTp.EMOD + " ="
                + mod;
        Cursor cursor = context.getContentResolver().query(DBTp.CONTENT_URI, projection, selection, null, null);
        if (cursor.moveToFirst())
        {
            int tpIdIndex = cursor.getColumnIndex(DBTp.TP_ID);
            tpId = cursor.getInt(tpIdIndex);
        }
        cursor.close();
        return tpId;
    }
    
    public int getTpIdByNetId(int netid, int tsid)
    {
        int tpId = 0;
        String[] projection = new String[] {DBTp.TP_ID};
        // String selection = DBTp.NET_ID +" ="+ netid +" and "+DBTp.TS_ID +" = " +tsid;
        String selection = DBTp.TS_ID + " = " + tsid;
        Cursor cursor = context.getContentResolver().query(DBTp.CONTENT_URI, projection, selection, null, null);
        if (cursor.moveToFirst())
        {
            int tpIdIndex = cursor.getColumnIndex(DBTp.TP_ID);
            tpId = cursor.getInt(tpIdIndex);
        }
        cursor.close();
        return tpId;
    }
    
    public int getTpId(int tsId)
    {
        int tpId = 0;
        String[] projection = new String[] {DBTp.TP_ID};
        String selection = DBTp.TS_ID + " =" + tsId;
        Cursor cursor = context.getContentResolver().query(DBTp.CONTENT_URI, projection, selection, null, null);
        if (cursor.moveToFirst())
        {
            int tpIdIndex = cursor.getColumnIndex(DBTp.TP_ID);
            tpId = cursor.getInt(tpIdIndex);
        }
        cursor.close();
        return tpId;
    }
    
    public TPInfoBean getTpInfo(int tpId)
    {
        TPInfoBean tpInfoBean = null;
        String selection = DBTp.TP_ID + " = " + tpId;
        Cursor cursor = context.getContentResolver().query(DBTp.CONTENT_URI, null, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            int tunerTypeIndex = cursor.getColumnIndex(DBTp.TUNER_TYPE);
            int tunerIdIndex = cursor.getColumnIndex(DBTp.TUNER_ID);
            int netIdIndex = cursor.getColumnIndex(DBTp.NET_ID);
            int onIdIndex = cursor.getColumnIndex(DBTp.ORIGINAL_NET_ID);
            int tsIdIndex = cursor.getColumnIndex(DBTp.TS_ID);
            int freqIndex = cursor.getColumnIndex(DBTp.FREQUENCY);
            int srIndex = cursor.getColumnIndex(DBTp.SYMB_RATE);
            int emodIndex = cursor.getColumnIndex(DBTp.EMOD);
            int eiIndex = cursor.getColumnIndex(DBTp.EFEC_INNER);
            int eoIndex = cursor.getColumnIndex(DBTp.EFEC_OUTER);
            
            int tunerType = cursor.getInt(tunerTypeIndex);
            
            String strTunerId = cursor.getString(tunerIdIndex);
            char tunerId;
            if (strTunerId.length() > 0)
            {
                tunerId = strTunerId.charAt(0);
            }
            else
            {
                tunerId = '0';
            }
            
            int netId = cursor.getInt(netIdIndex);
            int onId = cursor.getInt(onIdIndex);
            int tsId = cursor.getInt(tsIdIndex);
            int freq = cursor.getInt(freqIndex);
            int symbRate = cursor.getInt(srIndex);
            int emod = cursor.getInt(emodIndex);
            int efecInner = cursor.getInt(eiIndex);
            int efecOuter = cursor.getInt(eoIndex);
            
            tpInfoBean =
                new TPInfoBean(tpId, tunerType, tunerId, netId, onId, tsId, freq, symbRate, emod, efecInner, efecOuter);
        }
        cursor.close();
        return tpInfoBean;
    }
    
    public void updateServiceInfo(TPInfoBean tpInfoBean, int tpId)
    {
        ContentValues values = new ContentValues();
        values.put(DBTp.TUNER_TYPE, tpInfoBean.getTunerType());
        values.put(DBTp.TUNER_ID, String.valueOf(tpInfoBean.getTunerId()));
        values.put(DBTp.NET_ID, tpInfoBean.getNetId());
        values.put(DBTp.ORIGINAL_NET_ID, tpInfoBean.getOriginalNetId());
        values.put(DBTp.TS_ID, tpInfoBean.getTranStreamId());
        values.put(DBTp.FREQUENCY, tpInfoBean.getTunerFreq());
        values.put(DBTp.SYMB_RATE, tpInfoBean.getTunerSymbRate());
        values.put(DBTp.EMOD, tpInfoBean.getTunerEmod());
        values.put(DBTp.EFEC_INNER, tpInfoBean.getEfecInner());
        values.put(DBTp.EFEC_OUTER, tpInfoBean.getEfecOuter());
        String where = DBTp.TP_ID + " = " + tpId;
        context.getContentResolver().update(DBTp.CONTENT_URI, values, where, null);
    }
    
    @Override
    public void clearTpData()
    {
        context.getContentResolver().delete(DBTp.CONTENT_URI, null, null);
    }
    
}
