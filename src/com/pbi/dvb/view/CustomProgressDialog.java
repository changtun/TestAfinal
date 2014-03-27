package com.pbi.dvb.view;

import android.app.Dialog;
import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.view.Gravity;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.utils.LogUtils;

public class CustomProgressDialog extends Dialog
{
    private static final String TAG = "CustomProgressDialog";
    
    private static CustomProgressDialog customProgressDialog = null;
    
    public CustomProgressDialog(Context context)
    {
        super(context);
    }
    
    public CustomProgressDialog(Context context, int theme)
    {
        super(context, theme);
    }
    
    public static CustomProgressDialog createDialog(Context context, int layoutResId)
    {
        customProgressDialog = new CustomProgressDialog(context, R.style.CustomProgressDialog);
        customProgressDialog.setContentView(layoutResId);
        customProgressDialog.getWindow().getAttributes().gravity = Gravity.CENTER;
        
        return customProgressDialog;
    }
    
    public void onWindowFocusChanged(boolean hasFocus)
    {
        if (customProgressDialog == null)
        {
            return;
        }
        
        View view = customProgressDialog.findViewById(R.id.iv_loading);
        
        if (null == view)
        {
            LogUtils.e(TAG, "view.CustomProgressDialog.onWindowFocusChanged:: null == view...");
            
            return;
        }
        
        // ImageView imageView = (ImageView)customProgressDialog.findViewById(R.id.iv_loading);
        ImageView imageView = (ImageView)view;
        
        AnimationDrawable animationDrawable = (AnimationDrawable)imageView.getBackground();
        
        animationDrawable.start();
    }
    
    /**
     * 
     * set progress dialog title
     * 
     * @param strTitle
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public CustomProgressDialog setTitile(String strTitle)
    {
        return customProgressDialog;
    }
    
    /**
     * set progress dialog message.
     * 
     * @param strMessage
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public CustomProgressDialog setMessage(String strMessage)
    {
        TextView tvMsg = (TextView)customProgressDialog.findViewById(R.id.tv_dialog_msg);
        
        if (tvMsg != null)
        {
            tvMsg.setText(strMessage);
        }
        
        return customProgressDialog;
    }
}