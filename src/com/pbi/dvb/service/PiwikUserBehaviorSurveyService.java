package com.pbi.dvb.service;

import java.util.LinkedList;
import java.util.List;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;

import com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL;
import com.pbi.dvb.aidl.PiwikUserBehaviorSurveyAIDL;
import com.pbi.dvb.aidl.WebViewManagerServiceAIDL;
import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.utils.LogUtils;

public class PiwikUserBehaviorSurveyService extends Service
{
    
    private static final String TAG = "PiwikUserBehaviorSurveyService";
    
    private static final int SEND_SIGNAL_TO_WEBVIEWMANAGER = 0;
    
    private static List<PiwikTrackerBehaviorBean> continuousBehaviorList = null;
    
    private static List<PiwikTrackerBehaviorBean> discreteBehaviorList = null;
    
    private PiwikUserBehaviorSurveyBinder behaviorSurveyBinder;
    
    private PiwikTrackerManagerServiceAIDL piwikTrackerManagerServiceAIDL = null;
    
    private boolean isServiceConnected = false;
    
    private Handler wv_Handler = new Handler()
    {
        
        public void dispatchMessage(Message msg)
        {
            
            super.dispatchMessage(msg);
            
            switch (msg.what)
            {
                case SEND_SIGNAL_TO_WEBVIEWMANAGER:
                {
                    
                    if (!isServiceConnected)
                    {
                        
                        int tryCount = 0;
                        
                        while (!isServiceConnected)
                        {
                            
                            tryCount++;
                            
                            try
                            {
                                
                                Thread.sleep(500);
                                
                            }
                            catch (InterruptedException e)
                            {
                                
                                e.printStackTrace();
                                
                            }
                            
                            if (tryCount >= 3)
                            {
                                break;
                            }
                            
                        }
                        
                    }
                    
                    if (isServiceConnected)
                    {
                        try
                        {
                            LogUtils.e(TAG,
                                "PiwikUserBeahaviorSurveyService has connected to the PiwikTrackerManagerService, sendSignal() for do action now...");
                            
                            piwikTrackerManagerServiceAIDL.sendSignal();
                        }
                        catch (RemoteException e)
                        {
                            e.printStackTrace();
                        }
                    }
                    else
                    {
                        LogUtils.e(TAG, "can not connected to the WebViewManagerService", true);
                    }
                    
                    break;
                }
            }
        }
    };
    
    private ServiceConnection piwikTrackerManagerConnection;
    
    private void connect_To_WebViewManagerService()
    {
        
        LogUtils.e(TAG, "PiwikUserBehaviorSurveyService::init_Service_Connection() run...");
        
        Intent piwikTrackerManagerServiceIntent =
            new Intent(PiwikUserBehaviorSurveyService.this, com.pbi.dvb.service.PiwikTrackerManagerService.class);
        
        piwikTrackerManagerConnection = new ServiceConnection()
        {
            @Override
            public void onServiceDisconnected(ComponentName name)
            {
            }
            
            @Override
            public void onServiceConnected(ComponentName name, IBinder service)
            {
                piwikTrackerManagerServiceAIDL = PiwikTrackerManagerServiceAIDL.Stub.asInterface(service);
                
                isServiceConnected = true;
                
                try
                {
                    piwikTrackerManagerServiceAIDL.sendSignal();
                }
                catch (RemoteException e)
                {
                    e.printStackTrace();
                }
                
                LogUtils.e(TAG, "PiwikUserBehaviorSurveyService::init_Service_Connection::onServiceConnected", true);
            }
        };
        
        boolean bindFlag =
            bindService(piwikTrackerManagerServiceIntent, piwikTrackerManagerConnection, Context.BIND_AUTO_CREATE);
        
        LogUtils.e(TAG, "PiwikUserBehaviorSurveyService::bindFlag = " + bindFlag);
    }
    
    private void disconnect_From_WebViewManagerService()
    {
        if (piwikTrackerManagerConnection != null && piwikTrackerManagerServiceAIDL != null)
        {
            unbindService(piwikTrackerManagerConnection);
            
            isServiceConnected = false;
            
            piwikTrackerManagerConnection = null;
            
            piwikTrackerManagerServiceAIDL = null;
        }
    }
    
    @Override
    public IBinder onBind(Intent intent)
    {
        
        if (behaviorSurveyBinder == null)
        {
            synchronized (this)
            {
                if (behaviorSurveyBinder == null)
                {
                    behaviorSurveyBinder = new PiwikUserBehaviorSurveyBinder();
                }
            }
        }
        
        return behaviorSurveyBinder;
    }
    
    @Override
    public void onCreate()
    {
        super.onCreate();
        
        LogUtils.e(TAG, "PiwikUserBehaviorSurveyService::onCreate() run");
        
        if (continuousBehaviorList == null)
        {
            synchronized (this)
            {
                if (continuousBehaviorList == null)
                {
                    continuousBehaviorList = new LinkedList<PiwikTrackerBehaviorBean>();
                }
            }
        }
        
        if (discreteBehaviorList == null)
        {
            synchronized (this)
            {
                if (discreteBehaviorList == null)
                {
                    discreteBehaviorList = new LinkedList<PiwikTrackerBehaviorBean>();
                }
            }
        }
        
        connect_To_WebViewManagerService();
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        
        LogUtils.e(TAG, "PiwikUserBehaviorSurveyService::onStartCommand() run");
        
        return super.onStartCommand(intent, flags, startId);
        
    }
    
    private class PiwikUserBehaviorSurveyBinder extends PiwikUserBehaviorSurveyAIDL.Stub
    {
        
        // When you get the behavior from the behavior list...
        // You need to check if the WebView in the WindowManagerList is available...
        // Because the discrete behavior should send one by one...
        // So the short term behavior list's size should always be 1...
        // It means that when you a discrete behavior is added into the short term
        // behavior list...
        // You should check if the short term WebView is available...
        // If it's not available, you should just skipped it this time...
        // Notice:: You should synchronize this to
        // public List<UserBehaviorSurveyWebview> getAction()
        @Override
        public PiwikTrackerBehaviorBean generateActionOne()
            throws RemoteException
        {
            
            synchronized (this)
            {
                PiwikTrackerBehaviorBean behavior = null;
                
                if (null != continuousBehaviorList && continuousBehaviorList.size() > 0)
                {
                    LogUtils.e(TAG, "the continuousBehaviorList's size = " + continuousBehaviorList.size()
                        + " return a new behavior to WebViewManagerService");
                    
                    behavior = continuousBehaviorList.remove(0);
                }
                else if (null != discreteBehaviorList && discreteBehaviorList.size() > 0)
                {
                    LogUtils.e(TAG, "the discreteBehaviorList's size = " + discreteBehaviorList.size()
                        + " return a new behavior to WebViewManagerService");
                    
                    behavior = discreteBehaviorList.remove(0);
                }
                
                return behavior;
                
            }
            
        }
        
        @Override
        public void pushBehavior(int type, boolean unInterruptableFlag, String[] tags)
            throws RemoteException
        {
            synchronized (this)
            {
                PiwikTrackerBehaviorBean behavior = new PiwikTrackerBehaviorBean();
                
                behavior.setType(type);
                behavior.setUnInterruptable(unInterruptableFlag);
                
                if (null != tags && tags.length % 2 != 0)
                {
                    LogUtils.e(TAG, "Error!!! the tags.length is not a even number");
                }
                
                for (int i = 0; null != tags && i < tags.length;)
                {
                    if (i + 2 > tags.length)
                    {
                        LogUtils.e(TAG, "Error!!!!!!! when get the customerVarBean, the variables is not a pair...");
                        
                        break;
                    }
                    
                    behavior.addCustomerVar(tags[i++], tags[i++]);
                }
                
                // // when get a disconnect behavior, we delete all the same behavior in the behavior list
                // // because at this time , the other behavior has no meaning for the behavior survey...
                // if (UserBehaviorSurvey.DISCONNECT_FROM_SERVER == behavior.getType())
                // {
                // for (int i = 0; continuousBehaviorList != null && i < continuousBehaviorList.size(); i++)
                // {
                // PiwikTrackerBehaviorBean checkSurvey = continuousBehaviorList.get(i);
                //
                // if (behavior.checkBehaviorTheSame(checkSurvey))
                // {
                // continuousBehaviorList.remove(i);
                //
                // i--;
                // }
                // }
                // }
                
                if (behavior.isUnInterruptable())
                {
                    continuousBehaviorList.add(behavior);
                }
                else
                {
                    discreteBehaviorList.add(behavior);
                }
                
                if (continuousBehaviorList.size() == 1 || discreteBehaviorList.size() == 1)
                {
                    wv_Handler.sendEmptyMessage(SEND_SIGNAL_TO_WEBVIEWMANAGER);
                }
                
                LogUtils.e(TAG,
                    "---->>>> UserBehaviorSurveyBinder::UserBehaviorSurveyBinder::pushBehavior() surveyPath = "
                        + behavior.getSurveyPathNew() + " <<<<----");
            }
        }
    }
    
    @Override
    public void onDestroy()
    {
        super.onDestroy();
        
        disconnect_From_WebViewManagerService();
    }
    
}