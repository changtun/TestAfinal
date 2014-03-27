package com.pbi.dvb.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.View;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.utils.LogUtils;

public class DiscreteUserBehaviorSurveyWebview extends UserBehaviorSurveyWebview
{
    
    protected static final String TAG = "DiscreteUserBehaviorSurveyWebview";
    
    private Handler handler;
    
    private ControlDoubleLoadWebViewClient webViewClient;
    
    public DiscreteUserBehaviorSurveyWebview(Context context, PiwikTrackerBehaviorBean behavior, Handler handler)
    {
        
        super(context, behavior);
        
        this.handler = handler;
        
        if (behavior == null)
        {
            throw new RuntimeException("The UserBehaviorSurvey passed in can not be null...");
        }
        
        this.behavior = behavior;
        
        WebChromeClient chromClient = new WebChromeClient()
        {
            @Override
            public void onProgressChanged(WebView view, int newProgress)
            {
                if (newProgress >= 100)
                {
                    LogUtils.e(TAG,
                        "---->>>> DiscrettinuousUserBehaviorSurveyWebview::chromClient::onProgressChanged() loadUrl success...cess...newProgress=="
                            + newProgress + "... url:: " + view.getUrl() + " <<<<----");
                    
                    timeout_Check.interrupt();
                    
                    super.onProgressChanged(view, newProgress);
                }
                else
                {
                    LogUtils.e(TAG,
                        "---->>>> DiscreteUserBehaviorSurveyWebview::chromClient::onProgressChanged() loadUrl:: "
                            + view.getUrl() + "     newProgress = " + newProgress + " <<<<----");
                }
            }
        };
        
        setWebChromeClient(chromClient);
        
        webViewClient = new ControlDoubleLoadWebViewClient();
        
        setWebViewClient(webViewClient);
        
        setOnKeyListener(new OnKeyListener()
        {
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event)
            {
                return false;
            }
        });
        
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
                "---->>>> DiscreteUserBehaviorSurveyWebview::webViewClient::onPageStarted() start loading url:: " + url
                    + " <<<<----");
            
            if (executeTime == 0)
            {
                LogUtils.e(TAG, "DiscreteUserBehaviorSurveyWebview::the executeTime == 0... = " + executeTime
                    + " so handle this start load right now!!!...");
                
                executeTime++;
                
                timeout_Check = null;
                
                timeout_Check = new Load_Timeout_Check(DiscreteUserBehaviorSurveyWebview.this.handler, url);
                
                timeout_Check.start();
                
                load_Status = LOAD_STARTED;
                
                super.onPageStarted(view, url, favicon);
            }
            else
            {
                LogUtils.e(TAG, "DiscreteUserBehaviorSurveyWebview::the executeTime != 0... executeTime = "
                    + executeTime + " so skipped this start load...");
                
                executeTime++;
            }
        }
        
        @Override
        public void onPageFinished(WebView view, String url)
        {
            LogUtils.e(TAG,
                "---->>>> DiscreteUserBehaviorSurveyWebview::webViewClient::onPageFinished() finished loading url:: "
                    + url + " <<<<----");
            
            super.onPageFinished(view, url);
        }
        
        public void setExecuteTime(int executeTime)
        {
            this.executeTime = executeTime;
        }
        
    }
    
    public void setBehavior(PiwikTrackerBehaviorBean behavior)
    {
        this.behavior = behavior;
    }
    
    @Override
    public void loadUrl(String url)
    {
        LogUtils.e(TAG, "---->>>> DiscreteUserBehaviorSurveyWebview::loadUrl() " + behavior.getCallbackURL(url)
            + " <<<<----");
        
        load_Status = CALL_LOAD_FUNCTION;
        
        webViewClient.setExecuteTime(0);
        
        super.clearCache(true);
        super.loadUrl(behavior.getCallbackURL(url));
    }
}
