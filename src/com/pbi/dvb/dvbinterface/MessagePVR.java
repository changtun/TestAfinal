package com.pbi.dvb.dvbinterface;

import android.os.Handler;

import com.pbi.dvb.global.Config;


public class MessagePVR
{
    private Handler handler;
    
    public MessagePVR(Handler handler)
    {
        super();
        this.handler = handler;
    }

    /**
     * 
     * <���ŵ��ļ�β >
     * @see [�ࡢ��#��������#��Ա]
     */
    public void PvrPlayMsgEof()
	{
		handler.sendEmptyMessage(Config.PVR_PLAY_EOF);
	}
	
    /**
     * 
     * <���˵��ļ�ͷ >
     * @see [�ࡢ��#��������#��Ա]
     */
	public void PvrPlayMsgSof()
	{
		handler.sendEmptyMessage(Config.PVR_PLAY_SOF);
	}
	
	/**
	 * 
	 * <�����ڲ�����>
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrPlayMsgError()
	{
		handler.sendEmptyMessage(Config.PVR_PLAY_ERROR);
	}
	
	/**
	 * 
	 * <ʱ�Ƶ�ʱ�򲥷�׷��¼�� >
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrPlayMsgReachRec()
	{
		handler.sendEmptyMessage(Config.PVR_PLAY_REACH_REC);
	}
	
	/**
	 * 
	 * <������ >
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrRecMsgDiskFull()
	{
		handler.sendEmptyMessage(Config.PVR_REC_DISKFULL);;
	}
	
	/**
	 * 
	 * <¼���ڲ����� >
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrRecMsgError()
	{
		handler.sendEmptyMessage(Config.PVR_REC_ERROR);
	}
	
	/**
	 * 
	 * <¼�Ƴ��ȴﵽָ���ĳ���,ֻ�зǻ���¼�Ʋ�������¼�>
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrRecMsgOverFix()
	{
		handler.sendEmptyMessage(Config.PVR_REC_OVER_FIX);
	}
	
	/**
	 * 
	 * <ʱ�Ƶ�ʱ��¼��׷�ϲ��� >
	 * @see [�ࡢ��#��������#��Ա]
	 */
	public void PvrRecMsgReachPlay()
	{
		handler.sendEmptyMessage(Config.PVR_REC_REACH_PLAY);
	}
	
	/**
	 * 
	 * <���̴洢�ٶ�����¼���ٶ� >
	 * @see [�ࡢ��#��������#��Ա]
	 */

	public void PvrRecMsgDiskSlow()
	{
		handler.sendEmptyMessage(Config.PVR_REC_DISK_SLOW);
	}					
	
}
