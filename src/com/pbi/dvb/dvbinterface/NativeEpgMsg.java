/*
 * File Name:  TVChannelPlay.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-07
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.dvbinterface;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class NativeEpgMsg
{
    private Handler handler;
    
    private Message message;
    
    public NativeEpgMsg()
    {
        super();
    }
    
    public NativeEpgMsg(Handler handler)
    {
        super();
        this.handler = handler;
    }
    
    /**
     * <Epg message> <��Ҫ��ֵ�ǰ�ǹۿ�״̬����epg״̬, ����ǹۿ�״̬,��Ҫ�жϵ�ǰ��pevent �Ƿ��г��˼���, pevent
     * ͨ��Java_com_pbi_dvb_dvbinterface_NativeEpg_GetPevent ��ȡ �����epg�˵�����Ҫ����ˢ�µ�ǰEVENT ��ʾ����>
     * 
     * @param serviceid
     * @param tsid
     * @param onid
     * @see [�ࡢ��#��������#��Ա]
     */
    public void epgmsgNotify(short serviceid, short tsid, short onid)
    {
        LogUtils.printLog(1, 5, "epgmsgNotify", "epgmsgNotify Coming!!!");
        Bundle bundle = new Bundle();
        bundle.putShort("serviceId", serviceid);
        bundle.putShort("tsid", tsid);
        bundle.putShort("onid", onid);
        message = new Message();
        message.obj = bundle;
        message.what = Config.UPDATE_EPG_INFO;
        handler.removeMessages(Config.UPDATE_EPG_INFO);
        handler.sendMessage(message);
    }
    
}
