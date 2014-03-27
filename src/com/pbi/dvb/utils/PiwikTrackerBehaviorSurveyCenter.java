package com.pbi.dvb.utils;

import java.util.ArrayList;
import java.util.List;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;

import com.pbi.dvb.aidl.PiwikUserBehaviorSurveyAIDL;
import com.pbi.dvb.domain.CustomerVarBean;
import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;
import com.pbi.dvb.domain.UserBehaviorSurvey;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.service.PiwikUserBehaviorSurveyService;

public class PiwikTrackerBehaviorSurveyCenter extends Thread
{
    private static final String TAG = "PiwikTrackerBehaviorSurveyCenter";
    
    private Context context;
    
    private PiwikUserBehaviorSurveyAIDL piwikUserBehaviorSurveyService = null;
    
    private ServiceConnection piwikUserBehaviorSurveyConnection;
    
    private boolean isServiceConnected = false;
    
    private static List<PiwikTrackerBehaviorBean> continuousPiwikTraclerList = null;
    
    private static List<PiwikTrackerBehaviorBean> discretePiwikTrackerList = null;
    
    private boolean runFlag = true;
    
    public PiwikTrackerBehaviorSurveyCenter(Context context)
    {
        
        this.context = context;
        
        if (continuousPiwikTraclerList == null)
        {
            synchronized (this)
            {
                if (continuousPiwikTraclerList == null)
                {
                    continuousPiwikTraclerList = new ArrayList<PiwikTrackerBehaviorBean>();
                }
            }
        }
        
        if (discretePiwikTrackerList == null)
        {
            synchronized (this)
            {
                if (discretePiwikTrackerList == null)
                {
                    discretePiwikTrackerList = new ArrayList<PiwikTrackerBehaviorBean>();
                }
            }
        }
        
    }
    
    private void connect_To_UserBehaviorSurvey()
    {
        LogUtils.e(TAG, "connect_To_UserBehaviorSurvey() run...");
        
        Intent piwikUserBehaviorSurveyServiceIntent = new Intent(context, PiwikUserBehaviorSurveyService.class);
        
        piwikUserBehaviorSurveyConnection = new ServiceConnection()
        {
            @Override
            public void onServiceDisconnected(ComponentName name)
            {
            }
            
            @Override
            public void onServiceConnected(ComponentName name, IBinder service)
            {
                piwikUserBehaviorSurveyService = PiwikUserBehaviorSurveyAIDL.Stub.asInterface(service);
                
                isServiceConnected = true;
                
                interrupt();
            }
        };
        
        context.bindService(piwikUserBehaviorSurveyServiceIntent,
            piwikUserBehaviorSurveyConnection,
            Context.BIND_AUTO_CREATE);
    }
    
    private void disconnect_From_UserBehaviorSurvey()
    {
        LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey() run...");
        
        if (piwikUserBehaviorSurveyConnection != null && piwikUserBehaviorSurveyService != null)
        {
            context.unbindService(piwikUserBehaviorSurveyConnection);
            
            isServiceConnected = false;
            
            piwikUserBehaviorSurveyConnection = null;
            
            piwikUserBehaviorSurveyService = null;
        }
        else
        {
            if (piwikUserBehaviorSurveyConnection == null)
            {
                LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey::behaviorsurveyconnection == null");
            }
            
            if (piwikUserBehaviorSurveyService == null)
            {
                LogUtils.e(TAG, "disconnect_From_UserBehaviorSurvey::behaviorSurveyService == null");
            }
        }
    }
    
    public void startProgram(int logicalNumber)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_VIEW_LENGTH);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER);
        var.setValue(logicalNumber + "");
        
        vars.add(var);
        
        pushAction(true, UserBehaviorSurvey.PUSH_BEHAVIOR, vars);
    }
    
    public void changeChannel(int logicalNumber)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_VIEW_LENGTH);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER);
        var.setValue(logicalNumber + "");
        
        vars.add(var);
        
        pushAction(true, UserBehaviorSurvey.DISCONNECT_FROM_SERVER, vars);
    }
    
    public void startTimeShift(int logicalNumber)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START);
        
        vars.add(var);
        
        pushAction(false, UserBehaviorSurvey.DISCONNECT_FROM_SERVER, vars);
    }
    
    public void pauseTimeShift(int logicalNumber)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_PAUSE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER);
        var.setValue(logicalNumber + "");
        
        vars.add(var);
        
        pushAction(false, UserBehaviorSurvey.PUSH_BEHAVIOR, vars);
    }
    
    public void stopTimeShift(int logicalNumber)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER);
        var.setValue(logicalNumber + "");
        
        vars.add(var);
        
        pushAction(false, UserBehaviorSurvey.DISCONNECT_FROM_SERVER, vars);
    }
    
    public void changeSpeedTimeShift(int logicalNumber, int speed)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_VALUE_START_PLAY);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_TAG_NAME_LOGICAL_NUMBER);
        var.setValue(logicalNumber + "");
        
        vars.add(var);
        
        var = null;
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_SHIFT_ACTION_NAME_TIME_SHIFT_SPEED);
        var.setValue(speed + "");
        
        vars.add(var);
        
        pushAction(false, UserBehaviorSurvey.PUSH_BEHAVIOR, vars);
    }
    
    public void sendSignalQuality(String signal_Frequency, String signal_Symbol, String signal_Quality,
        String signal_Level)
    {
        List<CustomerVarBean> vars = new ArrayList<CustomerVarBean>();
        
        CustomerVarBean var = null;
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_SIGNAL_QUALITY);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_FREQUENCY);
        var.setValue(signal_Frequency + "");
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_SYMBOL);
        var.setValue(signal_Symbol + "");
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_QUALITY);
        var.setValue(signal_Quality + "");
        
        pushAction(false, UserBehaviorSurvey.PUSH_BEHAVIOR, vars);
        
        vars = new ArrayList<CustomerVarBean>();
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_PROJECT_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_PROJECT_VALUE);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_GLOBAL_ACTION_NAME);
        var.setValue(Config.USER_BEHAVIOR_SURVEY_ZAPP_ACTION_VALUE_SIGNAL_QUALITY);
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_FREQUENCY);
        var.setValue(signal_Frequency + "");
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_SYMBOL);
        var.setValue(signal_Symbol + "");
        
        vars.add(var);
        
        var = new CustomerVarBean();
        
        var.setName(Config.USER_BEHAVIOR_SURVEY_ZAPP_TAG_NAME_SIGNAL_LEVEL);
        var.setValue(signal_Level + "");
        
        pushAction(false, UserBehaviorSurvey.PUSH_BEHAVIOR, vars);
    }
    
    private void pushAction(boolean unInterrupted, Integer type, List<CustomerVarBean> vars)
    {
        synchronized (this)
        {
            PiwikTrackerBehaviorBean behaviorBean = new PiwikTrackerBehaviorBean();
            
            for (int i = 0; null != vars && i < vars.size(); i++)
            {
                CustomerVarBean var = vars.get(i);
                
                behaviorBean.addCustomerVar(var.getName(), var.getValue());
            }
            
            behaviorBean.setUnInterruptable(unInterrupted);
            behaviorBean.setType(type);
            
            if (behaviorBean.isUnInterruptable())
            {
                continuousPiwikTraclerList.add(behaviorBean);
            }
            else
            {
                discretePiwikTrackerList.add(behaviorBean);
            }
            
            interrupt();
            
        }
        
    }
    
    public PiwikTrackerBehaviorBean popAction()
    {
        
        synchronized (this)
        {
            
            if (continuousPiwikTraclerList != null && continuousPiwikTraclerList.size() > 0)
            {
                
                return continuousPiwikTraclerList.remove(0);
                
            }
            
            if (null != discretePiwikTrackerList && discretePiwikTrackerList.size() > 0)
            {
                return discretePiwikTrackerList.remove(0);
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
                    synchronized (this)
                    {
                        // get a behavior from the behavior list
                        PiwikTrackerBehaviorBean behavior = popAction();
                        
                        // push the behavior to the service according to the type of
                        // behavior
                        if (behavior != null)
                        {
                            if (piwikUserBehaviorSurveyService != null)
                            {
                                piwikUserBehaviorSurveyService.pushBehavior(behavior.getType(),
                                    behavior.isUnInterruptable(),
                                    behavior.tranlateCustomerVarsToArray());
                            }
                            else
                            {
                                LogUtils.e(TAG,
                                    "behaviorSurveyService == null, skip pushBehavior to server... the continuousPiwikTraclerList size = "
                                        + continuousPiwikTraclerList.size() + " discretePiwikTrackerList's size = "
                                        + discretePiwikTrackerList.size());
                            }
                        }
                        else
                        {
                            Thread.sleep(Integer.MAX_VALUE);
                        }
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
            if (isServiceConnected && piwikUserBehaviorSurveyConnection != null
                && piwikUserBehaviorSurveyService != null)
            {
                LogUtils.e(TAG, "UserBehaviorSurveyCenter.run().finally() run...");
                
                disconnect_From_UserBehaviorSurvey();
            }
        }
        
        interrupt();
    }
}
