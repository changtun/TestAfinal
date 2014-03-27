package com.pbi.dvb.view;

import android.content.Context;
import android.os.Handler;
import android.webkit.WebView;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.service.WebViewManagerService;
import com.pbi.dvb.utils.LogUtils;

public abstract class UserBehaviorSurveyWebview extends WebView
{
    
    public final static int UNINITIALIZATION = 0;
    
    public final static int INITIALIZATION = 1;
    
    public final static int CALL_LOAD_FUNCTION = 2;
    
    public final static int LOAD_STARTED = 3;
    
    public final static int LOAD_FINISHED = 4;
    
    public final static int LOAD_TIMEOUT = 5;
    
    protected int load_Status;
    
    protected PiwikTrackerBehaviorBean behavior = null;
    
    protected Load_Timeout_Check timeout_Check = null;
    
    protected String completeURL = null;
    
    protected String DISCONNECT_URL = "about:blank";
    
    public UserBehaviorSurveyWebview(Context context, PiwikTrackerBehaviorBean behavior)
    {
        super(context);
        
        this.behavior = behavior;
        
        load_Status = UNINITIALIZATION;
    }
    
    public boolean isAvailable()
    {
        return (load_Status == INITIALIZATION || load_Status == LOAD_FINISHED || load_Status == LOAD_TIMEOUT);
    }
    
    // A timeout control thread for WebView, when the WebView start to connect
    // to the server. The thread start to run... During the settled default
    // timeout, if the WebView connect to the server successfully, the WebView's
    // callback will call the thread.interrupt() function to stop the thread.
    // If the WebView do not connect to the server in limited time. The thread
    // will change the WebView's status. But even the timeout is triggered. It
    // doesn't means that the connect is failed. Sometimes it may connect to the
    // server at edge time.
    protected class Load_Timeout_Check extends Thread
    {
        
        private static final String TAG = "UserBehaviorSurveyWebview";
        
        private long defaultTimeoutWait = 10000;
        
        private long minumConnectTime = 1000;
        
        private Handler handler = null;
        
        private String load_URL = null;
        
        public Load_Timeout_Check(Handler handler)
        {
            this.handler = handler;
        }
        
        public Load_Timeout_Check(Handler handler, String load_URL)
        {
            LogUtils.e(TAG, "in Load_Timeout_Check, the load_URL = " + load_URL);
            
            this.handler = handler;
            
            this.load_URL = load_URL;
            
        }
        
        public void setDefaultTimeoutWait(long defaultTimeoutWait)
        {
            this.defaultTimeoutWait = defaultTimeoutWait;
        }
        
        public void run()
        {
            
            LogUtils.e(TAG, "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check::run() run... <<<<----");
            
            long startTime = System.currentTimeMillis();
            
            synchronized (this)
            {
                try
                {
                    LogUtils.e(TAG,
                        "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check::run() before wait...<<<<----",
                        true);
                    
                    Thread.sleep(defaultTimeoutWait);
                    
                    LogUtils.e(TAG,
                        "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check::run() after wait...<<<<----",
                        true);
                    
                }
                catch (InterruptedException e1)
                {
                    LogUtils.e(TAG,
                        "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check interrupt happened... This may be caused by load url success at WebChromeClient::onProgressChanged() when onProgress==100... <<<<----");
                    
                    e1.printStackTrace();
                }
                
                try
                {
                    Thread.sleep(minumConnectTime);
                }
                catch (InterruptedException e)
                {
                    LogUtils.e(TAG,
                        "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check interrupt happened... This is caused when wait in the minumConnectTime for piwik to get the connection information<<<<----");
                    
                    e.printStackTrace();
                }
            }
            
            long endTime = System.currentTimeMillis();
            
            if (endTime - startTime >= (defaultTimeoutWait + minumConnectTime))
            {
                LogUtils.e(TAG,
                    "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check timeout exception triggered...(endTime - startTime)=["
                        + (endTime - startTime) + "] url=[ " + load_URL + " ] <<<<----");
                
                // May be it's time out here....
                load_Status = LOAD_TIMEOUT;
            }
            else
            {
                LogUtils.e(TAG, "---->>>> UserBehaviorSurveyWebview::Load_Timeout_Check ((endTime - startTime)=["
                    + (endTime - startTime) + "] < WEBVIEW_DEFAULT_TIMEOUT[" + (defaultTimeoutWait + minumConnectTime)
                    + "])... url = [ " + load_URL + " ]<<<<----");
                
                load_Status = LOAD_FINISHED;
            }
            
            if (null != completeURL && completeURL.contains(DISCONNECT_URL))
            {
                LogUtils.e(TAG, "the completeURL.contains(" + DISCONNECT_URL
                    + "), so the interrupt here may be caused by disconnectFromServer() fuc");
            }
            else
            {
                handler.sendEmptyMessage(WebViewManagerService.LOAD_FINISHED);
            }
            
        }
    }
    
    public PiwikTrackerBehaviorBean getBehavior()
    {
        return behavior;
    }
    
    public void setBehavior(PiwikTrackerBehaviorBean behavior)
    {
        this.behavior = behavior;
    }
    
    public String getSurveyPath()
    {
        return behavior.getSurveyPathNew();
    }
}
