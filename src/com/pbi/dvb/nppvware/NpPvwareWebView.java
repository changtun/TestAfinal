package com.pbi.dvb.nppvware;

import android.content.Context;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;

import com.pbi.dvb.utils.LogUtils;

public class NpPvwareWebView extends WebView
{
    public NpPvwareWebView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }
    
    @Override
    public void addView(View child)
    {
        LogUtils.printLog(1, 3, "NpPvwareWebView", "--->>> addView !!!");
        LogUtils.printLog(1, 3, "NpPvwareWebView", "--->>> who parent1 " + this.getParent().getClass().getName());
        LogUtils.printLog(1, 3, "NpPvwareWebView", "--->>> who parent2 " + this.getParent().getParent().getClass().getName());
        LogUtils.printLog(1, 3, "NpPvwareWebView", "--->>>  child name " +child.getClass().getName());
        
        ViewGroup vp = (ViewGroup)this.getParent();
        vp.addView(child,0);
    }
    
    @Override
    public void removeView(View view)
    {
        LogUtils.printLog(1, 3, "NpPvwareWebView", "--->>> removeView !!!");
         ViewGroup vp = (ViewGroup)this.getParent();
         vp.removeView(view);
    }
}
