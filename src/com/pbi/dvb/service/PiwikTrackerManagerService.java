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

import com.pbi.dvb.aidl.PiwikTrackerManagerServiceAIDL;
import com.pbi.dvb.aidl.PiwikUserBehaviorSurveyAIDL;
import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.domain.UserBehaviorSurvey;
import com.pbi.dvb.piwik_tracker.BasePiwikTracker;
import com.pbi.dvb.piwik_tracker.impl.Continuous_Piwik_Tracker;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PiwikTrackerManager;
import com.pbi.dvb.view.ContinuousUserBehaviorSurveyWebview;

public class PiwikTrackerManagerService extends Service
{
    private static final String TAG = "PiwikTrackerManagerService";
    
    protected static final int LOAD_URL = 0;
    
    protected static final int LOAD_FINISHED = 1;
    
    private ServiceConnection piwikUserBehaviorSurveyConnection;
    
    private PiwikUserBehaviorSurveyAIDL piwikBehaviorSurveyService = null;
    
    protected boolean isServiceConnected = false;
    
    public PiwikTrackerBehaviorBean currentBehavior = null;
    
    private SendBehaviorToServer behaviorSender = null;
    
    private PiwikTrackerManager piwikTrackerManager = null;
    
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
                            case UserBehaviorSurvey.PUSH_BEHAVIOR:
                            {
                                LogUtils.e(TAG,
                                    "PiwikTrackerManagerService::dispatchMessage::PUSH_BEHAVIOR::LOAD_URL url = "
                                        + currentBehavior.getSurveyPathNew());
                                
                                BasePiwikTracker tracker = null;
                                
                                if (currentBehavior.isUnInterruptable())
                                {
                                    tracker = piwikTrackerManager.getFirstAvailableContinuousTracker(currentBehavior);
                                    
                                    if (tracker == null)
                                    {
                                        throw new RuntimeException(
                                            "---->>>> PiwikTrackerManagerService.piwikTrackerManager::getFirstAvailableContinuousTracker() Can not get a ContinuousUserBehaviorSurveyWebview from the PiwikTrackerManager <<<<----");
                                    }
                                }
                                else
                                {
                                    tracker = piwikTrackerManager.getFirstAvailableDiscreteTracker(currentBehavior);
                                    
                                    if (tracker == null)
                                    {
                                        LogUtils.e(TAG, "can not get a discrete tracker...");
                                    }
                                }
                                
                                if (tracker != null)
                                {
                                    // webView.loadUrl("http://61.58.63.24/DCMS/");
                                    tracker.connectToServer();
                                }
                                
                                break;
                            }
                            
                            case UserBehaviorSurvey.DISCONNECT_FROM_SERVER:
                            {
                                LogUtils.e(TAG,
                                    "PiwikTrackerManagerService::dispatchMessage::DISCONNECT_FROM_SERVER::LOAD_URL url = "
                                        + currentBehavior.getSurveyPathNew());
                                
                                List<Continuous_Piwik_Tracker> continuousPiwikTracks =
                                    piwikTrackerManager.getAllOnDisplayContinuousTracker(currentBehavior);
                                
                                int unAvailableForDisconnect = 0;
                                
                                if (continuousPiwikTracks != null)
                                {
                                    for (Continuous_Piwik_Tracker piwikTracker : continuousPiwikTracks)
                                    {
                                        
                                        if (!piwikTracker.isAvailable())
                                        {
                                            
                                            LogUtils.e(TAG, "start to disconnect a behavior from server:: "
                                                + piwikTracker.getPageTrackURL());
                                            
                                            unAvailableForDisconnect++;
                                            
                                            piwikTracker.disconnectFromServer();
                                            
                                        }
                                        
                                    }
                                    
                                    piwikTrackerManager.clearContinuousConnectionPool();
                                    
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
                    LogUtils.e(TAG, "unsupport message in handler... msg.what = " + msg.what);
                    
                    break;
                }
            }
        };
    };
    
    @Override
    public IBinder onBind(Intent intent)
    {
        return new PiwikTrackerManagerBinder();
    }
    
    private class PiwikTrackerManagerBinder extends PiwikTrackerManagerServiceAIDL.Stub
    {
        
        @Override
        public void sendSignal()
            throws RemoteException
        {
            LogUtils.e(TAG, "receive the signal from PiwikUserBehaviorSurveyService, start to interrupt");
            
            behaviorSender.interrupt();
            
        }
    }
    
    private void connect_To_PiwikUserBehaviorSurveyService()
    {
        LogUtils.e(TAG, "PiwikTrackerManagerService::connect_To_PiwikUserBehaviorSurveyService() run...");
        
        Intent piwikBehaviorSurveyServiceIntent =
            new Intent(this, com.pbi.dvb.service.PiwikUserBehaviorSurveyService.class);
        
        piwikUserBehaviorSurveyConnection = new ServiceConnection()
        {
            @Override
            public void onServiceDisconnected(ComponentName name)
            {
            }
            
            @Override
            public void onServiceConnected(ComponentName name, IBinder service)
            {
                LogUtils.e(TAG,
                    "PiwikTrackerManagerService::init_Service_Connection has connected to the PiwikUserBehaviorSurveyService");
                
                piwikBehaviorSurveyService = PiwikUserBehaviorSurveyAIDL.Stub.asInterface(service);
                
                if (piwikBehaviorSurveyService != null)
                {
                    isServiceConnected = true;
                }
                else
                {
                    LogUtils.e(TAG,
                        "Notice!!! Error!!!!!!   PiwikTrackerManagerService.connect_To_PiwikUserBehaviorSurveyService, the piwikBehaviorSurveyService == null...");
                }
                
                behaviorSender.interrupt();
                
            }
        };
        
        bindService(piwikBehaviorSurveyServiceIntent, piwikUserBehaviorSurveyConnection, Context.BIND_AUTO_CREATE);
    }
    
    private void disconnect_From_UserBehaviorSurveyService()
    {
        LogUtils.e(TAG, "PiwikTrackerManagerService::disconnect_From_UserBehaviorSurveyService() run...");
        
        if (piwikUserBehaviorSurveyConnection != null && piwikBehaviorSurveyService != null)
        {
            unbindService(piwikUserBehaviorSurveyConnection);
            
            isServiceConnected = false;
            
            piwikUserBehaviorSurveyConnection = null;
            
            piwikBehaviorSurveyService = null;
        }
    }
    
    @Override
    public void onCreate()
    {
        super.onCreate();
        
        LogUtils.e(TAG, "PiwikTrackerManagerService::onCreate run()");
        
        connect_To_PiwikUserBehaviorSurveyService();
        
        if (behaviorSender == null)
        {
            behaviorSender = new SendBehaviorToServer();
            
            behaviorSender.start();
        }
        else
        {
            behaviorSender.setRunFlag(false);
            
            LogUtils.e(TAG,
                "WebViewManagerService::onCreate, the behaviorSender is not null. intertupt the original one, start to interrupt");
            
            behaviorSender.interrupt();
            
            behaviorSender = null;
            
            behaviorSender = new SendBehaviorToServer();
            
            behaviorSender.start();
        }
        
        piwikTrackerManager = new PiwikTrackerManager();
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        
        LogUtils.e(TAG, "PiwikTrackerManagerService::onStartCommand run()");
        
        return super.onStartCommand(intent, flags, startId);
        
    }
    
    private class SendBehaviorToServer extends Thread
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
                
                LogUtils.e(TAG, "PiwikTrackerManagerService.SendBehaviorToServer run...");
                
                // if the WebView has finished his work, it will set the
                // currentBehavior to be null.
                if (currentBehavior == null && isServiceConnected)
                {
                    
                    LogUtils.e(TAG,
                        "PiwikTrackerManagerService::currentBehavior = null, behaviorSurveyService connected");
                    
                    // get the behavior information from the service
                    try
                    {
                        if (piwikBehaviorSurveyService == null)
                        {
                            LogUtils.e(TAG, "PiwikTrackerManagerService:: behaviorSurveyService == null");
                        }
                        else
                        {
                            LogUtils.e(TAG, "PiwikTrackerManagerService:: behaviorSurveyService != null");
                        }
                        
                        currentBehavior = piwikBehaviorSurveyService.generateActionOne();
                        
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
                        handler.sendEmptyMessage(LOAD_URL);
                    }
                    else
                    {
                        LogUtils.d(TAG, "currentBehavior == null");
                    }
                }
                else
                {
                    LogUtils.e(TAG, "currentBehavior != null or behaviorSurveyService is not connected");
                }
                
                try
                {
                    LogUtils.d(TAG, "before sleep  isInterrupted() = " + isInterrupted(), true);
                    
                    Thread.sleep(Integer.MAX_VALUE);
                    
                    LogUtils.d(TAG, "after sleep", true);
                }
                catch (InterruptedException e)
                {
                    e.printStackTrace();
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
