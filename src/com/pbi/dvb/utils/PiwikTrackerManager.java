package com.pbi.dvb.utils;

import java.util.LinkedList;
import java.util.List;

import android.content.Context;
import android.os.Handler;
import android.view.View;

import com.pbi.dvb.domain.CustomerVarBean;
import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.piwik_tracker.PiwikException;
import com.pbi.dvb.piwik_tracker.impl.Continuous_Piwik_Tracker;
import com.pbi.dvb.piwik_tracker.impl.Discrete_Piwik_Tracker;
import com.pbi.dvb.view.DiscreteUserBehaviorSurveyWebview;
import com.pbi.dvb.view.UserBehaviorSurveyWebview;

public class PiwikTrackerManager
{
    public final String REMOTE_URL = "http://61.58.63.24/DCMS/";
    
    private static final String TAG = "PiwikTrackerManager";
    
    private List<Continuous_Piwik_Tracker> continuousConn = null;
    
    private List<Discrete_Piwik_Tracker> discreteConn = null;
    
    private final Integer SUGGESTED_MAX_CONTINUOUS_WEBVIEWS = 5;
    
    public PiwikTrackerManager()
    {
        continuousConn = new LinkedList<Continuous_Piwik_Tracker>();
        
        discreteConn = new LinkedList<Discrete_Piwik_Tracker>();
    }
    
    public List<Continuous_Piwik_Tracker> getAllOnDisplayContinuousTracker(PiwikTrackerBehaviorBean currentBehavior)
    {
        return null;
    }
    
    public Continuous_Piwik_Tracker getFirstAvailableContinuousTracker(PiwikTrackerBehaviorBean currentBehavior)
    {
        return null;
    }
    
    public Discrete_Piwik_Tracker getFirstAvailableDiscreteTracker(PiwikTrackerBehaviorBean currentBehavior)
    {
        final int discreteConnSize = getDiscretePiwikTrackerSize();
        
        if (discreteConnSize < 0)
        {
            throw new RuntimeException(
                "---->>>> UserBehaviorSurveyCenter::WebViewDisplayManager::getFirstAvailableDiscreteWebView() getDiscreteWebViewSize() < 0... <<<<----");
        }
        
        Discrete_Piwik_Tracker discretePiwikTracker = null;
        
        if (discreteConnSize == 0)
        {
            try
            {
                discretePiwikTracker = new Discrete_Piwik_Tracker(REMOTE_URL);
                
                List<CustomerVarBean> vars = currentBehavior.getCustomerVars();
                
                for (int i = 0; null != vars && i < vars.size(); i++)
                {
                    CustomerVarBean var = vars.get(i);
                    
                    discretePiwikTracker.setVisitorCustomVariable(var.getName(), var.getValue());
                }
                
                discreteConn.add(discretePiwikTracker);
            }
            catch (PiwikException e)
            {
                // TODO: the format of url passed in is not correct...
                e.printStackTrace();
            }
        }
        else if (discreteConn.get(0).isAvailable())
        {
            discretePiwikTracker = discreteConn.get(0);
        }
        
        return discretePiwikTracker;
    }
    
    private int getDiscretePiwikTrackerSize()
    {
        return discreteConn == null ? 0 : discreteConn.size();
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
}
