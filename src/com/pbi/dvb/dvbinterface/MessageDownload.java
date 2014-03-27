package com.pbi.dvb.dvbinterface;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.pbi.dvb.service.OTADownloaderService;
import com.pbi.dvb.utils.LogUtils;

public class MessageDownload
{
    private static final String TAG = "MessageDownload";
    
    private Handler handler;
    
    private Context context;
    
    public MessageDownload(Handler handler, Context context)
    {
        super();
        this.handler = handler;
        this.context = context;
    }
    
    /**
     * ����ٷֱ�"100", ���Բ���ֱ����ʾ�������
     * 
     * @param param1
     * @see [�ࡢ��#��������#��Ա]
     */
    public void downloadCompleted(int param)
    {
        LogUtils.e(TAG, "---->>>> downloadCompleted run... ");
        
        Message msg = handler.obtainMessage();
        
        msg.what = OTADownloaderService.OTA_DOWNLOAD_FINISHED;
        
        msg.arg1 = param;
        
        handler.sendMessage(msg);
    }
    
    /**
     * ����ʧ����Ϣ
     * 
     * @param param1
     * @see [�ࡢ��#��������#��Ա]
     */
    public void downloadFailed(int param)
    {
        Message msg = handler.obtainMessage();
        
        msg.what = OTADownloaderService.OTA_DOWNLOAD_ERROR;
        
        msg.arg1 = param;
        
        handler.sendMessage(msg);
    }
    
    /**
     * �������ؽ��
     * 
     * @param param1
     * @see [�ࡢ��#��������#��Ա]
     */
    public void downloadProgress(int param)
    {
        Message msg = handler.obtainMessage();
        
        msg.what = OTADownloaderService.OTA_UPDATE_DOWNLOAD_PROGRESS;
        
        msg.arg1 = param;
        
        handler.sendMessage(msg);
    }
    
}
