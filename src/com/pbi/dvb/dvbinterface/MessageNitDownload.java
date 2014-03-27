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
	* 传入"1" 需要弹出升级提示框提示用户是否下载升级数据,
	* 如果用户选择否,则不下载,在本次观看电视的情况下,
	* 不需要再弹升级提示框了,在下次重新进入dvb的时候会重新搜升级消息.
	* 
	* @param param1
	* @see [类、类#方法、类#成员]
	*/
	public void DLNitUpdate(int param1)
	{
        LogUtils.e(TAG, "JNI call MessageNitDownload::DLNitUpdate() function...");

        handler.sendEmptyMessage(OTADownloaderService.OTA_RECEIVE_UPDATE_NOTICE);
}

}
