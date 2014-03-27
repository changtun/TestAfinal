package com.pbi.dvb.dvbinterface;

import com.pbi.dvb.service.OTADownloaderService;
import com.pbi.dvb.utils.LogUtils;

import android.content.Context;
import android.os.Handler;

public class MessageNitDownload
{
	private static final String TAG = "MessageNitDownload";

	private Handler handler;

	private Context context;

	public MessageNitDownload(Handler handler, Context context)
	{
		super();
		this.handler = handler;
		this.context = context;
	}

	/**
	* ����"1" ��Ҫ����������ʾ����ʾ�û��Ƿ�������������,
	* ����û�ѡ���,������,�ڱ��ιۿ����ӵ������,
	* ����Ҫ�ٵ�������ʾ����,���´����½���dvb��ʱ���������������Ϣ.
	* 
	* @param param1
	* @see [�ࡢ��#��������#��Ա]
	*/
	public void DLNitUpdate(int param1)
	{
        LogUtils.e(TAG, "JNI call MessageNitDownload::DLNitUpdate() function...");

        handler.sendEmptyMessage(OTADownloaderService.OTA_RECEIVE_UPDATE_NOTICE);
}

}
