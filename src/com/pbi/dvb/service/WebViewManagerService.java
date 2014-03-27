package com.pbi.dvb.service;

import java.util.List;

import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.RemoteException;

import com.pbi.dvb.aidl.UserBehaviorSurveyAIDL;
import com.pbi.dvb.aidl.WebViewManagerServiceAIDL;
import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.WebViewManager;
import com.pbi.dvb.view.ContinuousUserBehaviorSurveyWebview;
import com.pbi.dvb.view.UserBehaviorSurveyWebview;

public class WebViewManagerService extends Service
{
    private static final String TAG = "WebViewManagerService";
    
    private UserBehaviorSurveyAIDL behaviorSurveyService = null;
    
    private boolean isServiceConnected = false;
    
    private WebViewManager webViewManager;
    
    private static final int LOAD_URL = 0;
    
    public static final int LOAD_FINISHED = 1;
    
    private PiwikTrackerBehaviorBean currentBehavior = null;
    
    private SendBehaviorToServerThread behaviorSender = null;
    
    private WebViewManagerBinder webViewManagerBinder = null;
    
    private Handler handler = new Handler()
    {
        public void dispatchMessage(android.os.Message msg)
        {
            switch (msg.what)
            {
                case LOAD_URL:
                {
                    if (currentBehavior != null)
                    {
                        switch (currentBehavior.getType())
                        {
                            case PiwikTrackerBehaviorBean.PUSH_BEHAVIOR:
                            {
                                LogUtils.e(TAG,
                                    "WebViewManagerService::dispatchMessage::PUSH_BEHAVIOR::LOAD_URL url = "
                                        + currentBehavior.getSurveyPathNew());
                                
                                UserBehaviorSurveyWebview webView = null;
                                
                                if (currentBehavior.isUnInterruptable())
                                {
                                    webView = webViewManager.getFirstAvailableContinuousWebView(currentBehavior);
                                    
                                    if (webView == null)
                                    {
                                        throw new RuntimeException(
                                            "---->>>> UserBehaviorSurveyCenter::getAction() Can not get a ContinuousUserBehaviorSurveyWebview from the WebViewDisplayManager <<<<----");
                                    }
                                }
                                else
                                {
                                    webView = webViewManager.getFirstAvailableDiscreteWebView(currentBehavior);
                                    
                                    if (webView == null)
                                    {
                                        LogUtils.e(TAG, "can not get a discrete WebView...");
                                    }
                                }
                                
                                if (webView != null)
                                {
                                    webView.clearCache(true);
                                    
                                    webView.loadUrl("http://61.58.63.24/DCMS/");
                                }
                                
                                break;
                            }
                            
                            case PiwikTrackerBehaviorBean.DISCONNECT_FROM_SERVER:
                            {
                                
                                LogUtils.e(TAG,
                                    "WebViewManagerService::dispatchMessage::DISCONNECT_FROM_SERVER::LOAD_URL url = "
                                        + currentBehavior.getSurveyPathNew());
                                
                                List<ContinuousUserBehaviorSurveyWebview> behaviorWebViews =
                                    webViewManager.getAllOnDisplayContinuousWebView(currentBehavior);
                                
                                int unAvailableForDisconnect = 0;
                                
                                if (behaviorWebViews != null)
                                {
                                    
                                    for (ContinuousUserBehaviorSurveyWebview behaviorWebView : behaviorWebViews)
                                    {
                                        
                                        if (!behaviorWebView.isAvailable())
                                        {
                                            
                                            LogUtils.e(TAG, "start to disconnect a behavior from server:: "
                                                + behaviorWebView);
                                            
                                            unAvailableForDisconnect++;
                                            
                                            behaviorWebView.disconnectFromServer();
                                            
                                        }
                                        
                                    }
                                    
                                    webViewManager.clearContinuousConnectionPool();
                                    
                                }
                                
                                LogUtils.e(TAG,
                                    "during execute the DISCONNECT_FROM_SERVER, the number of view to disconnect from server = "
                                        + unAvailableForDisconnect);
                                
                                currentBehavior = null;
                                
                                handler.sendEmptyMessage(LOAD_FINISHED);
                                
                                break;
                            }
                        }
                        
                    }
                    
                    break;
                }
                
                case LOAD_FINISHED:
                {
                    synchronized (this)
                    {
                        currentBehavior = null;
                        
                        LogUtils.e(TAG, "LOAD_FINISHED, start to interrupt");
                        
                        behaviorSender.interrupt();
                    }
                }
                
                default:
                {
                    break;
                }
            }
        }
        
    };
    
    private ServiceConnection userBehaviorSurveyConnection;
    
    @Override
    public IBinder onBind(Intent arg0)
    {
        
        if (webViewManagerBinder == null)
        {
            synchronized (this)
            {
                if (webViewManagerBinder == null)
                {
                    webViewManagerBinder = new WebViewManagerBinder();
                }
            }
        }
        
        return webViewManagerBinder;
    }
    
    private class WebViewManagerBinder extends WebViewManagerServiceAIDL.Stub
    {
        @Override
        public void sendSignal()
            throws RemoteException
        {
            LogUtils.e(TAG, "receive the signal from UserBehaviorSurveyService, start to interrupt");
            
            behaviorSender.interrupt();
            
            // throw new RuntimeException("We don't expect the sendSignal to be called here...");
        }
    }
    
    private void connect_To_UserBehaviorSurveyService()
    {
        new Thread()
        {
            public void run()
            {
                LogUtils.e(TAG, "WebViewManagerService::connect_To_UserBehaviorSurveyService() run...");
                
                Intent behaviorSurveyServiceIntent =
                    new Intent(WebViewManagerService.this, com.pbi.dvb.service.UserBehaviorSurveyService.class);
                
                userBehaviorSurveyConnection = new ServiceConnection()
                {
                    @Override
                    public void onServiceDisconnected(ComponentName name)
                    {
                    }
                    
                    @Override
                    public void onServiceConnected(ComponentName name, IBinder service)
                    {
                        LogUtils.e(TAG,
                            "WebViewManagerService::init_Service_Connection has connected to the UserBehaviorSurveyService");
                        
                        behaviorSurveyService = UserBehaviorSurveyAIDL.Stub.asInterface(service);
                        
                        isServiceConnected = true;
                        
                        behaviorSender.interrupt();
                    }
                };
                
                bindService(behaviorSurveyServiceIntent, userBehaviorSurveyConnection, Context.BIND_AUTO_CREATE);
            };
        }.start();
    }
    
    private void disconnect_From_UserBehaviorSurveyService()
    {
        LogUtils.e(TAG, "WebViewManagerService::disconnect_From_UserBehaviorSurveyService() run...");
        
        if (userBehaviorSurveyConnection != null && behaviorSurveyService != null)
        {
            unbindService(userBehaviorSurveyConnection);
            
            isServiceConnected = false;
            
            userBehaviorSurveyConnection = null;
            
            behaviorSurveyService = null;
        }
    }
    
    @Override
    public void onCreate()
    {
        
        LogUtils.e(TAG, "WebViewManagerService::onCreate run()");
        
        connect_To_UserBehaviorSurveyService();
        
        if (behaviorSender == null)
        {
            behaviorSender = new SendBehaviorToServerThread();
            
            behaviorSender.start();
        }
        else
        {
            behaviorSender.setRunFlag(false);
            
            LogUtils.e(TAG,
                "WebViewManagerService::onCreate, the behaviorSender is not null. intertupt the original one, start to interrupt");
            
            behaviorSender.interrupt();
            
            behaviorSender = null;
            
            behaviorSender = new SendBehaviorToServerThread();
            
            behaviorSender.start();
        }
        
        webViewManager = new WebViewManager(this, handler);
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        LogUtils.e(TAG, "WebViewManagerService::onStartCommand run()");
        
        return super.onStartCommand(intent, flags, startId);
    }
    
    private class SendBehaviorToServerThread extends Thread
    {
        private boolean runFlag = true;
        
        private void setRunFlag(boolean runFlag)
        {
            this.runFlag = runFlag;
        }
        
        @Override
        public void run()
        {
            super.run();
            
            while (runFlag)
            {
                LogUtils.e(TAG, "WebViewManagerService.SendBehaviorToServerThread run...");
                
                // if the WebView has finished his work, it will set the
                // currentBehavior to be null.
                if (currentBehavior == null && isServiceConnected)
                {
                    LogUtils.e(TAG, "WebViewManagerService::currentBehavior = null, behaviorSurveyService connected");
                    
                    // get the behavior information from the service
                    try
                    {
                        if (behaviorSurveyService == null)
                        {
                            LogUtils.e(TAG, "WebViewManagerService:: behaviorSurveyService == null");
                        }
                        else
                        {
                            LogUtils.e(TAG, "WebViewManagerService:: behaviorSurveyService != null");
                        }
                        
                        currentBehavior = behaviorSurveyService.generateActionOne();
                        
                        LogUtils.e(TAG, "after generateActionOne(), the currentBehavior = " + currentBehavior);
                        
                    }
                    catch (RemoteException e)
                    {
                        e.printStackTrace();
                    }
                    
                    // if the service tells that he has a behavior need to be sended
                    // here checkBehavior will be true and currentBehavior shouldn't
                    // be null!
                    if (currentBehavior != null)
                    {
                        LogUtils.e(TAG,
                            "currentBehavior != null currentBehavior's survey path = "
                                + currentBehavior.getSurveyPathNew());
                        
                        handler.sendEmptyMessage(LOAD_URL);
                    }
                    else
                    {
                        LogUtils.e(TAG, "currentBehavior == null");
                    }
                }
                else
                {
                    LogUtils.e(TAG, (currentBehavior == null ? "currentBehavior == null " : "currentBehavior != null ")
                        + (isServiceConnected ? "behaviorSurveyService has been connected "
                            : "behaviorSurveyService has no connected "));
                }
                
                try
                {
                    LogUtils.d(TAG, "before sleep  isInterrupted() = " + isInterrupted(), true);
                    
                    Thread.sleep(Integer.MAX_VALUE);
                    
                    LogUtils.d(TAG, "after sleep", true);
                }
                catch (InterruptedException e)
                {
                }
            }
        }
    }
    
    @Override
    public void onDestroy()
    {
        super.onDestroy();
        
        disconnect_From_UserBehaviorSurveyService();
    }
}
