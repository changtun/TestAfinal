package com.pbi.dvb.utils;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.view.View;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.view.ContinuousUserBehaviorSurveyWebview;
import com.pbi.dvb.view.DiscreteUserBehaviorSurveyWebview;
import com.pbi.dvb.view.UserBehaviorSurveyWebview;

public class WebViewManager
{
    
    private static final String TAG = "WebViewManager";
    
    private List<UserBehaviorSurveyWebview> continuousConn = null;
    
    private List<UserBehaviorSurveyWebview> discreteConn = null;
    
    private final Integer SUGGESTED_MAX_CONTINUOUS_WEBVIEWS = 5;
    
    private Handler handler;
    
    private Context context;
    
    public WebViewManager(Context context, Handler handler)
    {
        
        continuousConn = new LinkedList<UserBehaviorSurveyWebview>();
        
        discreteConn = new LinkedList<UserBehaviorSurveyWebview>();
        
        this.context = context;
        
        this.handler = handler;
        
    }
    
    public UserBehaviorSurveyWebview getFirstAvailableDiscreteWebView(PiwikTrackerBehaviorBean behavior)
    {
        
        final int discreteConnSize = getDiscreteWebViewSize();
        
        if (discreteConnSize < 0)
        {
            
            throw new RuntimeException(
                "---->>>> UserBehaviorSurveyCenter::WebViewDisplayManager::getFirstAvailableDiscreteWebView() getDiscreteWebViewSize() < 0... <<<<----");
            
        }
        
        UserBehaviorSurveyWebview discreteWebView = null;
        
        if (discreteConnSize == 0)
        {
            
            discreteWebView = new DiscreteUserBehaviorSurveyWebview(context, behavior, handler);
            
            // 锟斤拷锟斤拷JavaScript锟斤拷支锟斤拷...锟斤拷锟斤拷为true锟斤拷,
            // 使锟矫碉拷前WebView锟斤拷锟截碉拷锟角革拷URL锟斤拷址锟斤拷应锟斤拷锟斤拷页锟叫碉拷JS锟脚憋拷锟结被执锟斤拷
            discreteWebView.getSettings().setJavaScriptEnabled(true);
            
            // 锟斤拷WebView锟叫的癸拷锟斤拷锟斤拷取锟斤拷
            discreteWebView.setVerticalScrollBarEnabled(false);
            discreteWebView.setHorizontalScrollBarEnabled(false);
            
            // 锟斤拷锟斤拷锟斤拷锟斤拷趴锟斤拷锟斤拷锟饺★拷锟叫э拷锟斤拷"setBackgroundColor(0);"锟斤拷效???
            discreteWebView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
            
            // 锟矫憋拷锟斤拷效锟斤拷锟斤拷失
            discreteWebView.setBackgroundColor(0);
            
            // 锟斤拷锟斤拷锟絎ebView锟斤拷透锟斤拷锟饺憋拷锟�.5f
            discreteWebView.setAlpha(0.5f);
            
            discreteConn.add(discreteWebView);
            
        }
        else if (discreteConn.get(0).isAvailable())
        {
            
            discreteWebView = discreteConn.get(0);
            
            discreteWebView.setBehavior(behavior);
        }
        
        return discreteWebView;
    }
    
    public UserBehaviorSurveyWebview getFirstAvailableContinuousWebView(PiwikTrackerBehaviorBean behavior)
    {
        
        UserBehaviorSurveyWebview availableContinuousWebView = null;
        
        for (int i = 0; continuousConn != null && i < continuousConn.size(); i++)
        {
            LogUtils.e(TAG,
                "UserBehaviorSurveyCenter::WebViewDisplayManager::getFirstAvailableContinuousWebView before continuousConn.get(i).isAvailable()  the i = ["
                    + i + "] the continous.size = [" + continuousConn.size() + "]");
            
            if (continuousConn.get(i).isAvailable())
            {
                availableContinuousWebView = continuousConn.get(i);
                
                availableContinuousWebView.setBehavior(behavior);
                
                break;
            }
        }
        
        if (availableContinuousWebView == null)
        {
            LogUtils.e(TAG, "New a new UserBehaviorSurveyWebview for use...url = [" + behavior.getSurveyPathNew() + "]");
            
            availableContinuousWebView = new ContinuousUserBehaviorSurveyWebview(context, behavior, handler);
            
            // 锟斤拷锟斤拷JavaScript锟斤拷支锟斤拷...锟斤拷锟斤拷为true锟斤拷,
            // 使锟矫碉拷前WebView锟斤拷锟截碉拷锟角革拷URL锟斤拷址锟斤拷应锟斤拷锟斤拷页锟叫碉拷JS锟脚憋拷锟结被执锟斤拷
            availableContinuousWebView.getSettings().setJavaScriptEnabled(true);
            
            // 锟斤拷WebView锟叫的癸拷锟斤拷锟斤拷取锟斤拷
            availableContinuousWebView.setVerticalScrollBarEnabled(false);
            availableContinuousWebView.setHorizontalScrollBarEnabled(false);
            
            // 锟斤拷锟斤拷锟斤拷锟斤拷趴锟斤拷锟斤拷锟饺★拷锟叫э拷锟斤拷"setBackgroundColor(0);"锟斤拷效???
            availableContinuousWebView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
            
            // 锟矫憋拷锟斤拷效锟斤拷锟斤拷失
            availableContinuousWebView.setBackgroundColor(0);
            
            // 锟斤拷锟斤拷锟絎ebView锟斤拷透锟斤拷锟饺憋拷锟�.0f, 全透锟斤拷, 锟斤拷图锟斤拷锟缴硷拷
            availableContinuousWebView.setAlpha(0.0f);
            
            continuousConn.add(availableContinuousWebView);
        }
        
        return availableContinuousWebView;
    }
    
    public void clearContinuousConnectionPool()
    {
        if (continuousConn.size() <= SUGGESTED_MAX_CONTINUOUS_WEBVIEWS)
        {
            return;
        }
        
        for (int i = 0; i < continuousConn.size(); i++)
        {
            if (continuousConn.get(i).isAvailable())
            {
                continuousConn.remove(i);
            }
        }
    }
    
    public List<ContinuousUserBehaviorSurveyWebview> getAllOnDisplayContinuousWebView(
        PiwikTrackerBehaviorBean markBehavior)
    {
        List<ContinuousUserBehaviorSurveyWebview> onDisplayContinuousWebViews = null;
        
        LogUtils.e(TAG, "continuousConn.size() = " + continuousConn.size());
        
        for (int i = 0; continuousConn != null && i < continuousConn.size(); i++)
        {
            ContinuousUserBehaviorSurveyWebview webView = (ContinuousUserBehaviorSurveyWebview)continuousConn.get(i);
            
            PiwikTrackerBehaviorBean checkBehavior = webView.getBehavior();
            
            if (!webView.isAvailable() && checkBehavior.checkBehaviorTheSame(markBehavior))
            {
                LogUtils.e(TAG, "The behavior on WindowManager is the same == with the behavior exptected to remove");
                
                LogUtils.e(TAG, "The behavior on WindowManager:: " + checkBehavior.toString());
                
                LogUtils.e(TAG, "The behavior expected to remove:: " + markBehavior.toString());
                
                if (onDisplayContinuousWebViews == null)
                {
                    onDisplayContinuousWebViews = new ArrayList<ContinuousUserBehaviorSurveyWebview>();
                }
                
                onDisplayContinuousWebViews.add(webView);
            }
            else
            {
                // LogUtils.e(
                // TAG,
                // "The ContinuousWebView here is available or The behavior on WindowManager is not not not the same != with the behavior exptected to remove");
                //
                // LogUtils.e(TAG, "The behavior on WindowManager:: "
                // + checkBehavior.toString());
                //
                // LogUtils.e(TAG, "The behavior expected to remove:: "
                // + markBehavior.toString());
            }
        }
        
        LogUtils.e(TAG, "the total continuous WebViews on screen is "
            + (onDisplayContinuousWebViews == null ? 0 : onDisplayContinuousWebViews.size()));
        
        return onDisplayContinuousWebViews;
    }
    
    public int getDiscreteWebViewSize()
    {
        return discreteConn == null ? -1 : discreteConn.size();
    }
    
    public int getContinuousWebViewSize()
    {
        return continuousConn == null ? -1 : continuousConn.size();
    }
    
}
