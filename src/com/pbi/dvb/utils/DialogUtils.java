/*
 * File Name:  DialogUtils.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2014-01-08
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb.utils;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Handler;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.Window;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.TextView;

import com.pbi.dvb.ChannelSearchProgress;
import com.pbi.dvb.R;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.global.PasswordCallback;
import com.pbi.dvb.pvrinterface.impl.TickCountDownUtil;
import com.pbi.dvb.view.DVBNoButtonsDialog;
import com.pbi.dvb.view.PasswordDialog;

/**
 * 
 * <Functional overview> <Functional Details>
 * 
 * @author
 * @version [Version Number, 2014-1-9]
 * @see [Relevant Class/Method]
 * @since [Product/Module Version]
 */
public class DialogUtils
{
    protected static final String TAG = "DialogUtils";
    
    public static Dialog passwordDialogCreate(final Context context, final PasswordCallback passwordCallback)
    {
        final Dialog dialog = new Dialog(context);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        
        LayoutInflater inflater = LayoutInflater.from(context);
        final View view = inflater.inflate(R.layout.dialog_password, null);
        final TextView tv = (TextView)view.findViewById(R.id.tv_pwd_tips);
        final TextView tvPwd = (TextView)view.findViewById(R.id.tv_pwd);
        
        tvPwd.findFocus();
        tvPwd.setFocusable(true);
        tvPwd.setFocusableInTouchMode(true);
        
        final StringBuilder passwordInput = new StringBuilder();
        tvPwd.setOnKeyListener(new View.OnKeyListener()
        {
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event)
            {
                StringBuilder pStar = new StringBuilder();
                /*-------------Press the digital key-----------------*/
                if (keyCode >= KeyEvent.KEYCODE_0 && keyCode <= KeyEvent.KEYCODE_9)
                {
                    if (event.getAction() == MotionEvent.ACTION_DOWN)
                    {
                        passwordInput.append(keyCode - KeyEvent.KEYCODE_0);
                    }
                    
                    if (null != passwordInput)
                    {
                        for (int i = 0; i < passwordInput.length(); i++)
                        {
                            pStar.append("*");
                            tvPwd.setText(pStar);
                        }
                    }
                    
                    if (passwordInput.length() == 4)
                    {
                        pStar.delete(0, pStar.length());
                        
                        String superPwd =
                            android.provider.Settings.System.getString(context.getContentResolver(), "password");
                        if (null == superPwd)
                        {
                            superPwd = Config.PROGRAMME_LOCK;
                        }
                        // process password
                        if (passwordInput.toString().equalsIgnoreCase(superPwd)
                            || passwordInput.toString().equalsIgnoreCase(Config.SUPER_PASSWORD))
                        {
                            if (null != dialog)
                            {
                                dialog.dismiss();
                            }
                            passwordCallback.passwordCorrectEvent();
                        }
                        else
                        {
                            tv.setText(context.getString(R.string.password_wrong_tips));
                            Animation shake = AnimationUtils.loadAnimation(context, R.anim.shake);
                            tvPwd.startAnimation(shake);
                            tvPwd.setText(null);
                            
                            passwordInput.delete(0, passwordInput.length());
                            pStar.delete(0, pStar.length());
                        }
                    }
                    return true;
                }
                /*-------------Press the del key-----------------*/
                if (keyCode == Config.KEY_FAST_CHANGE)
                {
                    passwordInput.delete(0, passwordInput.length());
                    pStar.delete(0, pStar.length());
                    tvPwd.setText(null);
                    tv.setText(context.getString(R.string.password_stb));
                    return true;
                }
                /*-------------Press the back key-----------------*/
                else
                {
                    Intent intent = new Intent(Intent.ACTION_MAIN);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);// 注意
                    intent.addCategory(Intent.CATEGORY_HOME);
                    context.startActivity(intent);
                    return false;
                }
            }
        });
        
        dialog.setContentView(view);
        dialog.show();
        return dialog;
        
    }
    
    public static Dialog twoButtonsDialogCreate(final Context context, String message,
        android.view.View.OnClickListener posButtonListener, android.view.View.OnClickListener negButtonListener)
    {
        AlertDialog.Builder builder = new Builder(context);
        Dialog cDialog = builder.create();
        cDialog.show();
        
        // set remote left/right function
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        
        Window window = cDialog.getWindow();
        window.setContentView(R.layout.dialog_with_buttons);
        TextView tvMessage = (TextView)window.findViewById(R.id.tv_dialog_notice);
        final Button btOK = (Button)window.findViewById(R.id.bt_search_ok);
        final Button btCancel = (Button)window.findViewById(R.id.bt_search_cancel);
        
        tvMessage.setText(message);
        btOK.setOnFocusChangeListener(new OnFocusChangeListener()
        {
            
            @Override
            public void onFocusChange(View v, boolean hasFocus)
            {
                if (hasFocus)
                {
                    btOK.setTextColor(context.getResources().getColor(R.color.white_color));
                    btCancel.setTextColor(context.getResources().getColor(R.color.black));
                    
                    btOK.setBackgroundResource(R.drawable.select_s_focus);
                    btCancel.setBackgroundResource(R.drawable.select_s);
                }
                
            }
        });
        btCancel.setOnFocusChangeListener(new OnFocusChangeListener()
        {
            
            @Override
            public void onFocusChange(View v, boolean hasFocus)
            {
                if (hasFocus)
                {
                    btCancel.setTextColor(context.getResources().getColor(R.color.white_color));
                    btOK.setTextColor(context.getResources().getColor(R.color.black));
                    
                    btOK.setBackgroundResource(R.drawable.select_s);
                    btCancel.setBackgroundResource(R.drawable.select_s_focus);
                }
                
            }
        });
        
        btOK.setOnClickListener(posButtonListener);
        btCancel.setOnClickListener(negButtonListener);
        
        return cDialog;
    }
    
    public static Dialog noButtonsDialogCreate(final Context context, Handler handler, String message)
    {
        DVBNoButtonsDialog cDialog = new DVBNoButtonsDialog(context, R.layout.dialog_one_button, handler);
        cDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        cDialog.show();
        cDialog.setMessage(message);
        return cDialog;
    }
	
	 public static Dialog noButtonDialogCreate(final Context context, String message)
    {
        AlertDialog.Builder builder = new Builder(context);
        Dialog cDialog = builder.create();
        cDialog.show();
        
        Window window = cDialog.getWindow();
        window.setContentView(R.layout.dialog_no_button);
        TextView tvMessage = (TextView)window.findViewById(R.id.tv_nb_dialog_message);
        tvMessage.findFocus();
        tvMessage.setFocusable(true);
        tvMessage.setFocusableInTouchMode(true);
        tvMessage.setOnKeyListener(new View.OnKeyListener()
        {
            
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event)
            {
                return true;
            }
        });
        tvMessage.setText(message);
        return cDialog;
    }
    
    public static Dialog passwordDialogCreate(Context context,String message,Handler handler,boolean isCaLock)
    {
        PasswordDialog dialog = new PasswordDialog(context, message, handler);
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialog.show();
        dialog.setMessage(message);
        if(isCaLock)
        {
            dialog.setCaLock(isCaLock);
        }
        return dialog;
    }
    
    public static Dialog twoButtonsDialogWithTimeCreate(final Context context, String message,TickCountDownUtil mCountDown,
        android.view.View.OnClickListener posButtonListener, android.view.View.OnClickListener negButtonListener)
    {
        if(null != mCountDown)
        {
            mCountDown.start();
        }
        
        mContext = context;
        
        AlertDialog.Builder builder = new Builder(context);
        cDialog = builder.create();
        cDialog.show();
        
        // set remote left/right function
        CommonUtils.setLeftRightFunction(Config.SWITCH_TO_LEFT_RIGHT_CONTROL);
        
        Window window = cDialog.getWindow();
        window.setContentView(R.layout.dialog_with_buttons);
        TextView tvMessage = (TextView)window.findViewById(R.id.tv_dialog_notice);
        btOK = (Button)window.findViewById(R.id.bt_search_ok);
        final Button btCancel = (Button)window.findViewById(R.id.bt_search_cancel);
        
        tvMessage.setText(message);
        btOK.setOnFocusChangeListener(new OnFocusChangeListener()
        {
            
            @Override
            public void onFocusChange(View v, boolean hasFocus)
            {
                if (hasFocus)
                {
                    btOK.setTextColor(context.getResources().getColor(R.color.white_color));
                    btCancel.setTextColor(context.getResources().getColor(R.color.black));
                    
                    btOK.setBackgroundResource(R.drawable.select_s_focus);
                    btCancel.setBackgroundResource(R.drawable.select_s);
                }
                
            }
        });
        btCancel.setOnFocusChangeListener(new OnFocusChangeListener()
        {
            @Override
            public void onFocusChange(View v, boolean hasFocus)
            {
                if (hasFocus)
                {
                    btCancel.setTextColor(context.getResources().getColor(R.color.white_color));
                    btOK.setTextColor(context.getResources().getColor(R.color.black));
                    
                    btOK.setBackgroundResource(R.drawable.select_s);
                    btCancel.setBackgroundResource(R.drawable.select_s_focus);
                }
                
            }
        });
        
        btOK.setOnClickListener(posButtonListener);
        btCancel.setOnClickListener(negButtonListener);
        
        return cDialog;
        
    }
    
    static Button btOK =null;
    public static final int UPDATE_BUTTON = 100;
    private static Context mContext;
    public static int curNitCode;
    private static Dialog cDialog;
    
    public static Handler handler = new Handler()
    {
        public void handleMessage(android.os.Message msg)
        {
            switch (msg.what)
            {
                case UPDATE_BUTTON:
                    btOK.setText(String.valueOf(msg.arg1));
                    if(msg.arg1 == 0)
                    {
                        //
                        if(null != cDialog)
                        {
                            cDialog.dismiss();
                        }
                        
                        // update the new version code.
                        Editor edit = mContext.getSharedPreferences("dvb_nit", 8).edit();
                        edit.putInt("version_code", curNitCode);
                        edit.commit();
                        
                        // package tp infomation.
                        Bundle bundle = new Bundle();
                        bundle.putInt("SearchMode", Config.Install_Search_Type_eAUTO_SEARCH);
                        Intent intent = new Intent(mContext, ChannelSearchProgress.class);
                        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                        intent.putExtras(bundle);
                        mContext.startActivity(intent);
                    }
                    break;
            }
        };
    };
    
}
