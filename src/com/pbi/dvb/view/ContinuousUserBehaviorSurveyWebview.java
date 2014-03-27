package com.pbi.dvb.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.service.WebViewManagerService;
import com.pbi.dvb.utils.LogUtils;

public class ContinuousUserBehaviorSurveyWebview extends UserBehaviorSurveyWebview
{
    protected static final String TAG = "ContinuousUserBehaviorSurveyWebview";
    
    private Handler handler;
    
    private ControlDoubleLoadWebViewClient webViewClient;
    
    private WebChromeClient chromClient;
    
    public ContinuousUserBehaviorSurveyWebview(Context context, PiwikTrackerBehaviorBean behavior, Handler handler)
    {
        super(context, behavior);
        
        this.handler = handler;
        
        if (behavior == null)
        {
            throw new RuntimeException("The UserBehaviorSurvey passed in can not be null...");
        }
        
        this.behavior = behavior;
        
        chromClient = new WebChromeClient()
        {
            @Override
            public void onProgressChanged(WebView view, int newProgress)
            {
                if (newProgress >= 100)
                {
                    LogUtils.e(TAG,
                        "---->>>> ContinuousUserBehaviorSurveyWebview::chromClient::onProgressChanged() loadUrl success... url:: "
                            + view.getUrl() + " <<<<----");
                    
                    if (null != completeURL && !completeURL.contains(DISCONNECT_URL))
                    {
                        ContinuousUserBehaviorSurveyWebview.this.handler.sendEmptyMessage(WebViewManagerService.LOAD_FINISHED);
                    }
                    else if (null != completeURL && null != timeout_Check)
                    {
                        timeout_Check.interrupt();
                    }
                    else
                    {
                        if (null == completeURL)
                        {
                            LogUtils.e(TAG, "ERROR!!!    The completeURL is null here...");
                        }
                        else
                        {
                            LogUtils.e(TAG, "The completeURL = " + completeURL + " here...");
                        }
                        
                        if (null == timeout_Check)
                        {
                            LogUtils.e(TAG, "The timeout_Check is null here... ");
                        }
                        else
                        {
                            LogUtils.e(TAG, "The timeout_Check is not not null here... ");
                        }
                    }
                    
                    super.onProgressChanged(view, newProgress);
                }
                else
                {
                    LogUtils.e(TAG,
                        "---->>>> ContinuousUserBehaviorSurveyWebview::chromClient::onProgressChanged() loadUrl::newProgress = "
                            + newProgress + "    " + view.getUrl() + "<<<<----");
                }
            }
        };
        
        setWebChromeClient(chromClient);
        
        webViewClient = new ControlDoubleLoadWebViewClient();
        
        setWebViewClient(webViewClient);
    }
    
    private class ControlDoubleLoadWebViewClient extends WebViewClient
    {
        int executeTime = 0;
        
        @Override
        public void onReceivedError(WebView view, int errorCode, String description, String failingUrl)
        {
            LogUtils.e(TAG, "in WebViewClient::onReceivedError errorCode = " + errorCode + " description = "
                + description + " failingUrl = " + failingUrl);
            
            super.onReceivedError(view, errorCode, description, failingUrl);
        }
        
        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon)
        {
            LogUtils.e(TAG,
                "---->>>> ContinuousUserBehaviorSurveyWebview::webViewClient::onPageStarted() start loading url:: "
                    + url + " <<<<----");
            
            if (executeTime == 0)
            {
                LogUtils.e(TAG, "ContinuousUserBehaviorSurveyWebview::the executeTime == 0... = " + executeTime
                    + " so handle this start load right now!!!...");
                
                executeTime++;
                
                if (null != completeURL && !completeURL.contains(DISCONNECT_URL))
                {
                    LogUtils.e(TAG, "completeURL = " + completeURL + " start the timeout control thread");
                    
                    load_Status = LOAD_STARTED;
                    
                    if (timeout_Check != null)
                    {
                        LogUtils.e(TAG,
                            "ContinuousUserBehaviorSurveyWebview::the timeout_Check is not null... interrupt it...");
                        
                        timeout_Check.interrupt();
                    }
                    
                    timeout_Check = null;
                    
                    timeout_Check = new Load_Timeout_Check(ContinuousUserBehaviorSurveyWebview.this.handler, url);
                    
                    timeout_Check.setDefaultTimeoutWait(Integer.MAX_VALUE);
                    
                    timeout_Check.start();
                }
                else
                {
                    if (null == completeURL)
                    {
                        LogUtils.e(TAG,
                            "Error!!! the completeURL is null here, this will cause the ContinuousUserBehaviorSurveyWebview never be free!!!");
                    }
                    else
                    {
                        LogUtils.e(TAG,
                            "The completeURL contains "
                                + DISCONNECT_URL
                                + ", completeURL = "
                                + completeURL
                                + " so do not start the status control thread and do not change the status here... Please check other progress callback in WebView for status changing...");
                    }
                }
                
                super.onPageStarted(view, url, favicon);
            }
            else
            {
                LogUtils.e(TAG, "ContinuousUserBehaviorSurveyWebview::the executeTime != 0... executeTime = " + executeTime
                    + " so skipped this start load...");
                
                executeTime++;
            }
        }
        
        @Override
        public void onPageFinished(WebView view, String url)
        {
            
            LogUtils.e(TAG,
                "---->>>> ContinuousUserBehaviorSurveyWebview::webViewClient::onPageFinished() finished loading url:: "
                    + url + " <<<<----");
            
            super.onPageFinished(view, url);
        }
        
        public void setExecuteTime(int executeTime)
        {
            this.executeTime = executeTime;
        }
    }
    
    @Override
    public void loadUrl(String url)
    {
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        LogUtils.e(TAG, "loadUrl!!!!!!!!!!!!!!!!!!!");
        
        LogUtils.e(TAG, "---->>>> ContinuousUserBehaviorSurveyWebview::loadUrl() " + behavior.getCallbackURL(url)
            + " <<<<----");
        
        completeURL = null;
        
        if (url != null && url.contains(DISCONNECT_URL))
        {
            completeURL = url;
            
            // Maybe we should change the load_Status here...
        }
        else
        {
            completeURL = behavior.getCallbackURL(url);
            
            load_Status = CALL_LOAD_FUNCTION;
        }
        
        webViewClient.setExecuteTime(0);
        
        super.clearCache(true);
        super.loadUrl(completeURL);
        
        LogUtils.e(TAG, "loadUrl***********************");
        LogUtils.e(TAG, "loadUrl***********************");
        LogUtils.e(TAG, "loadUrl***********************");
        LogUtils.e(TAG, "loadUrl***********************");
        LogUtils.e(TAG, "loadUrl***********************");
        LogUtils.e(TAG, "loadUrl***********************");
    }
    
    public void disconnectFromServer()
    {
        
        // if (timeout_Check != null) {
        //
        // timeout_Check.interrupt();
        //
        // }
        
        loadUrl(DISCONNECT_URL);
        
    }
}
