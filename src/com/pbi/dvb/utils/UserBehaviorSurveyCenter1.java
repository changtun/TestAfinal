package com.pbi.dvb.utils;

import java.util.ArrayList;
import java.util.List;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;

import com.pbi.dvb.aidl.UserBehaviorSurveyAIDL;
import com.pbi.dvb.domain.UserBehaviorSurvey;
import com.pbi.dvb.service.UserBehaviorSurveyService;

public class UserBehaviorSurveyCenter1 extends Thread
{
    
    private static final String TAG = "UserBehaviorSurveyCenter";
    
    private Context context;
    
    private UserBehaviorSurveyAIDL behaviorSurveyService = null;
    
    private ServiceConnection behaviorSurveyConnection;
    
    private boolean isServiceConnected = false;
    
    private static List<UserBehaviorSurvey> behaviorList = null;
    
    private boolean runFlag = true;
    
    public UserBehaviorSurveyCenter1(Context context)
    {
        
        this.context = context;
        
        if (behaviorList == null)
        {
            
            synchronized (this)
            {
                
                if (behaviorList == null)
                {
                    
                    behaviorList = new ArrayList<UserBehaviorSurvey>();
                    
                }
            }
        }
    }
    
    private void connect_To_UserBehaviorSurvey()
    {
        LogUtils.e(TAG, "connect_To_UserBehaviorSurvey() run...");
        
        Intent behaviorSurveyServiceIntent = new Intent(context, UserBehaviorSurveyService.class);
        
        behaviorSurveyConnection = new ServiceConnection()
        {
            
            @Override
            public void onServiceDisconnected(ComponentName name)
            {
            }
            
            @Override
            public void onServiceConnected(ComponentName name, IBinder service)
            {
                
                behaviorSurveyService = UserBehaviorSurveyAIDL.Stub.asInterface(service);
                
                isServiceConnected = true;
                
                interrupt();
                
            }
        };
        
        context.bindService(behaviorSurveyServiceIntent, behaviorSurveyConnection, Context.BIND_AUTO_CREATE);
        
    }
    
    private void disconnect_From_UserBehaviorSurvey()
    {
        LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey() run...");
        
        if (behaviorSurveyConnection != null && behaviorSurveyService != null)
        {
            context.unbindService(behaviorSurveyConnection);
            
            isServiceConnected = false;
            
            behaviorSurveyConnection = null;
            
            behaviorSurveyService = null;
        }
        else
        {
            if (behaviorSurveyConnection == null)
            {
                LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey::behaviorsurveyconnection == null");
            }
            
            if (behaviorSurveyService == null)
            {
                LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey::behaviorSurveyService == null");
            }
        }
    }
    
    public void pushAction(String project, String action, boolean unInterrupted, Integer type, String... tags)
    {
        
        synchronized (this)
        {
            LogUtils.e(TAG, "UserBehaviorSurveyCenter::pushAction project = " + project + " action = " + action
                + " unInterrupted = " + unInterrupted + " type = " + type);
            
            UserBehaviorSurvey behaviorSurvey = new UserBehaviorSurvey();
            
            behaviorSurvey.setProject(project);
            behaviorSurvey.setAction(action);
            behaviorSurvey.setUnInterruptable(unInterrupted);
            behaviorSurvey.setType(type);
            
            if (tags.length == 1)
            {
                
                behaviorSurvey.setTag1(tags[0]);
                
            }
            else if (tags.length == 2)
            {
                
                behaviorSurvey.setTag1(tags[0]);
                
                behaviorSurvey.setTag2(tags[1]);
                
            }
            else if (tags.length == 3)
            {
                
                behaviorSurvey.setTag1(tags[0]);
                
                behaviorSurvey.setTag2(tags[1]);
                
                behaviorSurvey.setTag3(tags[2]);
            }
            
            if (behaviorSurvey.isUnInterruptable())
            {
                behaviorList.add(0, behaviorSurvey);
            }
            else
            {
                behaviorList.add(behaviorSurvey);
            }
            
            interrupt();
            
        }
        
    }
    
    public UserBehaviorSurvey popAction()
    {
        
        synchronized (this)
        {
            
            if (behaviorList != null && behaviorList.size() > 0)
            {
                
                return behaviorList.remove(0);
                
            }
            
            return null;
        }
        
    }
    
    @Override
    public void run()
    {
        super.run();
        
        connect_To_UserBehaviorSurvey();
        
        while (runFlag)
        {
            try
            {
                LogUtils.e(TAG, "UserBehaviorSurveyCenter.run() in while run...");
                
                if (isServiceConnected)
                {
                    
                    // The util has connected to the UserBehaviorService
                    
                    // get a behavior from the behavior list
                    UserBehaviorSurvey behavior = popAction();
                    
                    // push the behavior to the service according to the type of
                    // behavior
                    if (behavior != null)
                    {
                        
                        LogUtils.e(TAG,
                            "UserBehaviorSurveyCenter::pushAction to server, surveyPath = " + behavior.getSurveyPath());
                        
                        synchronized (this)
                        {
                            if (behaviorSurveyService != null)
                            {
                                behaviorSurveyService.pushBehavior(behavior.getProject(),
                                    behavior.getAction(),
                                    behavior.getType(),
                                    behavior.isUnInterruptable(),
                                    behavior.getTags());
                            }
                            else
                            {
                                LogUtils.e(TAG,
                                    "behaviorSurveyService == null, skip pushBehavior to server... the behavior list's size = "
                                        + behaviorList.size());
                            }
                            
                        }
                        
                    }
                    else
                    {
                        
                        Thread.sleep(Integer.MAX_VALUE);
                        
                    }
                    
                }
                else
                {
                    // The util has not connected to the service , let's wait
                    // sometime
                    Thread.sleep(Integer.MAX_VALUE);
                }
                
            }
            catch (RemoteException e)
            {
                e.printStackTrace();
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }
            finally
            {
                // if you run the thread and stop it immediately, then the service does not connect to service
                // but you try to unbind it , this will cause a force quit...
                // if (!runFlag && behaviorSurveyConnection != null && behaviorSurveyService != null)
                // {
                // LogUtils.e(TAG, "UserBehaviorSurveyCenter.run().finally() run...");
                //
                // disconnect_From_UserBehaviorSurvey();
                // }
            }
            
        }
        
        LogUtils.e(TAG, "UserBehaviorSurveyCenter.run() jump out of while...");
        
    }
    
    public void stopTask()
    {
        LogUtils.e(TAG, "UserBehaviorSurveyCenter.run().stopTask() run...");
        
        runFlag = false;
        
        synchronized (this)
        {
            if (isServiceConnected && behaviorSurveyConnection != null && behaviorSurveyService != null)
            {
                LogUtils.e(TAG, "UserBehaviorSurveyCenter.run().finally() run...");
                
                disconnect_From_UserBehaviorSurvey();
            }
        }
        
        interrupt();
    }
}
