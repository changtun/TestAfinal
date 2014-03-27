package com.pbi.dvb.view;

import java.util.List;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.TVChannelPlay;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.LogUtils;

public class EpgDetailDialog extends Dialog
{
    
    private static final String TAG = "EpgDetailDialog";
    
    private TextView tv_ScrollText;
    private TextView tv_ScrollText_Title;
    
    private EpgDetailDialogCallBack callBack = null;
    
    private int clickCount = 0;
    
    private List<String> msgList;
    
    private int dialogLayout;
    
    private Context context;
    
    private final static int UPDATE_EPG_CONTENT = 0;
    /*2014-02-13 10:31:09 by gtsong, declear handler, send message to TVChannelPlay */
    private Handler mHandler;
    /*2014-02-13 10:31:09 */
    private Handler handler = new Handler()
    {
        @Override
        public void dispatchMessage(Message msg)
        {
            super.dispatchMessage(msg);
            
            switch (msg.what)
            {
                case UPDATE_EPG_CONTENT:
                {
                    LogUtils.e(TAG, "view.EpgDetailDialog.onKeyDown:: UPDATE_EPG_CONTENT  clickCount = " + clickCount);
                    
                    clickCount++;
                    
                    if (null != msgList && clickCount <= msgList.size())
                    {
                        LogUtils.e(TAG, "view.EpgDetailDialog.onKeyDown:: clickCount = " + clickCount + " msgList.get("
                            + (clickCount - 1) + ") = " + msgList.get(clickCount - 1));
                        
                        String unknownContent = context.getResources().getString(R.string.epg_detail_no_content);
                        
                        String content = null;
                        
                        if (null == msgList.get(clickCount - 1) || "".equals(msgList.get(clickCount - 1)))
                        {
                            LogUtils.e(TAG,
                                "view.EpgDetailDialog.onKeyDown::  null == msgList.get(clickCount or \"\".equals(msgList.get(clickCount))");
                            
                            content = unknownContent;
                        }
                        else
                        {
                            content = msgList.get(clickCount - 1);
                        }
                        
                        LogUtils.e(TAG, "view.EpgDetailDialog.onKeyDown:: content = " + content);
                        
                        switch (clickCount)
                        {
                            case 1:
                                tv_ScrollText_Title.setText(context.getString(R.string.epg_current_name));
                                // send message to TVChannelPlay to show Banner again.
                                if(null != mHandler)
                                {
                                    mHandler.sendEmptyMessage(Config.SHOW_BANNER_MESSAGE);
                                }
                                break;
                            
                            case 2:
                                tv_ScrollText_Title.setText(context.getString(R.string.epg_next_name));
                                break;
                        }
                        
                        tv_ScrollText.setText(content);
                        
                        tv_ScrollText.setTextColor(Color.GREEN);
                        
                        tv_ScrollText.invalidate();
                    }
                    else
                    {
                        clickCount = 0;
                        msgList.clear();
                        dismiss();
                        // when dialog dismiss, send message to TVChannelPlay
                        if(null != mHandler)
                        {
                            mHandler.sendEmptyMessage(Config.HIDE_BANNER_MESSAGE);
                        }

                    }
                    
                    if (null != callBack)
                    {
                        if (null != msgList && clickCount >= msgList.size())
                        {
                            callBack.onShowKey();
                        }
                    }
                    
                    break;
                }
            }
        }
    };
    
    public EpgDetailDialog(Context context, int dialogLayout, List<String> msgList)
    {
        super(context);
        
        this.context = context;
        
        this.msgList = msgList;
        
        this.dialogLayout = dialogLayout;
    }
    
    public EpgDetailDialog(Context context, int dialogLayout, List<String> msgList,Handler handler)
    {
        super(context);
        this.context = context;
        this.msgList = msgList;
        this.dialogLayout = dialogLayout;
        this.mHandler = handler;
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        LogUtils.e(TAG, "EpgActivity.EpgDetialDialog.onCreate run...");
        
        super.onCreate(savedInstanceState);
        
        setContentView(dialogLayout);
    }
    
    @Override
    protected void onStart()
    {
        super.onStart();
        LogUtils.e(TAG, "EpgActivity.EpgDetialDialog:: view.EpgDetailDialog.onStart run...");
        clickCount = 0;
        init_View();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        
        LogUtils.e(TAG, "EpgActivity.EpgDetialDialog:: view.EpgDetailDialog.onStop run...");
        clickCount = 0;
        
        msgList.clear();
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == Config.KEY_SHOW && event.getRepeatCount() == 0)
        {
            handler.sendEmptyMessage(UPDATE_EPG_CONTENT);
        }
        
        return super.onKeyDown(keyCode, event);
    }
    
    private void init_View()
    {
        LogUtils.e(TAG, "EpgActivity.EpgDetialDialog.init_View run...");
        
        if (null == tv_ScrollText)
        {
            tv_ScrollText = (TextView)findViewById(R.id.tv_ScrollText);
        }
        if(null == tv_ScrollText_Title)
        {
            tv_ScrollText_Title = (TextView)this.findViewById(R.id.tv_ScrollText_title);
        }
        
        handler.sendEmptyMessage(UPDATE_EPG_CONTENT);
    }
    
    public interface EpgDetailDialogCallBack
    {
        public abstract void onShowKey();
    }
    
    public void setCallBack(EpgDetailDialogCallBack callBack)
    {
        this.callBack = callBack;
    }
    
    public void setNewEpgContent(List<String> extendInfos)
    {
        if (null != msgList)
        {
            msgList.clear();
            
            msgList = null;
        }
        
        msgList = extendInfos;
        
        clickCount = 0;
    }
    
}
