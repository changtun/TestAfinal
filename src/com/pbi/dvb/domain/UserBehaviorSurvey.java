package com.pbi.dvb.domain;

import android.os.Parcel;
import android.os.Parcelable;

import com.pbi.dvb.utils.LogUtils;

public class UserBehaviorSurvey implements Parcelable
{
    
    private static final String TAG = null;
    
    public static final int PUSH_BEHAVIOR = 0;
    
    public static final int DISCONNECT_FROM_SERVER = 1;
    
    private String project = null;
    
    private String action = null;
    
    private String tag1 = null;
    
    private String tag2 = null;
    
    private String tag3 = null;
    
    private boolean unInterruptable = false;
    
    private Integer type = null;
    
    public Integer getType()
    {
        return type;
    }
    
    public void setType(Integer type)
    {
        this.type = type;
    }
    
    public UserBehaviorSurvey()
    {
        project = null;
        
        action = null;
        
        tag1 = null;
        
        tag2 = null;
        
        tag3 = null;
        
        unInterruptable = false;
    }
    
    public boolean checkBehaviorTheSame(UserBehaviorSurvey behavior)
    {
        if (project != null && !project.equals(behavior.getProject()))
        {
            LogUtils.e(TAG, "11111111");
            
            return false;
        }
        else if (project == null && behavior.getProject() != null)
        {
            LogUtils.e(TAG, "2222222222222222");
            return false;
        }
        
        if (action != null && !action.equals(behavior.getAction()))
        {
            LogUtils.e(TAG, "3333333333");
            return false;
        }
        else if (action == null && behavior.getAction() != null)
        {
            LogUtils.e(TAG, "444444444444");
            return false;
        }
        
        if (tag1 != null && behavior.getTag1() != null && !tag1.equals(behavior.getTag1()))
        {
            LogUtils.e(TAG, "55555555555555");
            return false;
        }
        else if (tag1 == null && (behavior.getTag1() != null))
        {
            LogUtils.e(TAG, "666666666666");
            return false;
        }
        
        if (tag2 != null && behavior.getTag2() != null && !tag2.equals(behavior.getTag2()))
        {
            LogUtils.e(TAG, "7777777777");
            return false;
        }
        else if (tag2 == null && (behavior.getTag2() != null))
        {
            LogUtils.e(TAG, "888888888888888");
            return false;
        }
        
        if (tag3 != null && behavior.getTag3() != null && !tag3.equals(behavior.getTag3()))
        {
            LogUtils.e(TAG, "999999999999");
            return false;
        }
        else if (tag3 == null && (behavior.getTag3() != null))
        {
            LogUtils.e(TAG, "101010101010");
            return false;
        }
        
        return true;
    }
    
    public boolean isUnInterruptable()
    {
        return unInterruptable;
    }
    
    public void setUnInterruptable(boolean unInterruptable)
    {
        this.unInterruptable = unInterruptable;
    }
    
    public String getProject()
    {
        return project;
    }
    
    public void setProject(String project)
    {
        this.project = project;
    }
    
    public String getAction()
    {
        return action;
    }
    
    public void setAction(String action)
    {
        this.action = action;
    }
    
    public String getTag1()
    {
        return tag1;
    }
    
    public void setTag1(String tag1)
    {
        this.tag1 = tag1;
    }
    
    public String getTag2()
    {
        return tag2;
    }
    
    public void setTag2(String tag2)
    {
        this.tag2 = tag2;
    }
    
    public String getTag3()
    {
        return tag3;
    }
    
    public void setTag3(String tag3)
    {
        this.tag3 = tag3;
    }
    
    // The url here should be like "http://192.168.0.206/dcms"
    public String getCallbackURL(String url)
    {
        
        if (url == null)
        {
            
            return null;
            
        }
        else if ("about:blank".equals(url))
        {
            
            return url;
            
        }
        
        String result = url;
        
        if (url.endsWith("/"))
        {
            
            result = url.substring(0, url.lastIndexOf("/"));
            
        }
        
        result += getSurveyPath();
        
        return result;
    }
    
    public String getSurveyPath()
    {
        
        String result = "";
        
        result += getPathStr(project);
        result += getPathStr(action);
        result += getPathStr(tag1);
        result += getPathStr(tag2);
        result += getPathStr(tag3);
        
        return result;
    }
    
    private String getPathStr(String str)
    {
        
        if (str != null)
        {
            
            return ("/" + str);
            
        }
        else
        {
            
            return "";
            
        }
    }
    
    public String[] getTags()
    {
        
        if (tag3 == null)
        {
            
            if (tag2 == null)
            {
                
                if (tag1 == null)
                {
                    
                    return null;
                    
                }
                else
                {
                    
                    return new String[] {tag1};
                    
                }
                
            }
            else
            {
                
                return new String[] {tag1, tag2};
                
            }
            
        }
        else
        {
            
            return new String[] {tag1, tag2, tag3};
            
        }
    }
    
    @Override
    public int describeContents()
    {
        return 0;
    }
    
    @Override
    public void writeToParcel(Parcel parcel, int i)
    {
        
        parcel.writeString(project);
        parcel.writeString(action);
        
        byte interruptOption = 0;
        
        if (unInterruptable)
        {
            
            interruptOption = 1;
            
        }
        else
        {
            
            interruptOption = 0;
            
        }
        
        parcel.writeByte(interruptOption);
        parcel.writeInt(type);
        parcel.writeString(tag1);
        parcel.writeString(tag2);
        parcel.writeString(tag3);
    }
    
    public static final Parcelable.Creator<UserBehaviorSurvey> CREATOR = new Creator<UserBehaviorSurvey>()
    {
        @Override
        public UserBehaviorSurvey createFromParcel(Parcel parcel)
        {
            
            UserBehaviorSurvey behavior = new UserBehaviorSurvey();
            
            behavior.setProject(parcel.readString());
            behavior.setAction(parcel.readString());
            
            byte interruptOption = parcel.readByte();
            
            if (interruptOption > 0)
            {
                
                behavior.setUnInterruptable(true);
                
            }
            else
            {
                
                behavior.setUnInterruptable(false);
                
            }
            
            behavior.setType(parcel.readInt());
            behavior.setTag1(parcel.readString());
            behavior.setTag2(parcel.readString());
            behavior.setTag2(parcel.readString());
            
            return behavior;
            
        }
        
        @Override
        public UserBehaviorSurvey[] newArray(int i)
        {
            
            return new UserBehaviorSurvey[i];
            
        }
    };
    
    @Override
    public String toString()
    {
        return "UserBehaviorSurvey{" + "project='" + project + '\'' + ", action='" + action + '\'' + ", tag1='" + tag1
            + '\'' + ", tag2='" + tag2 + '\'' + ", tag3='" + tag3 + '\'' + ", unInterruptable=" + unInterruptable
            + ", type=" + type + '}';
    }
}
