package com.pbi.dvb.utils;

import java.util.LinkedList;
import java.util.List;

import android.content.Context;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;

import com.pbi.dvb.R;

//The class may should be designed as a static and single instance..
//Inside the class, we can control if there can exists more than one dialog in the system
//Or like the implementation this moment, there can just has one dialog in the screen at one time.

//If the user stop record task at the same time when the task should be finished by system..
//Then it will create two dialog at the same time...
//And in this situation...We should close one of them... And when close the dialog, we should stop the count down task
//So like a suspend dialog...We should design a dialog class who can automatically manage it's count down task....
public class PvrDialogManager
{
    
    private static final String TAG = "PvrDialogManager";
    
    private static List<View> dialogList = new LinkedList<View>();
    
    private static PvrDialogManager pvrDiaManager = null;
    
    private PvrDialogManager()
    {
    }
    
    public static PvrDialogManager getInstance()
    {
        if (pvrDiaManager == null)
        {
            synchronized (PvrDialogManager.class)
            {
                if (pvrDiaManager == null)
                {
                    pvrDiaManager = new PvrDialogManager();
                }
            }
        }
        
        return pvrDiaManager;
    }
    
    public synchronized void addView(Context context, View view)
    {
        
        if (view != null)
        {
            
            WindowManager mWM = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            
            WindowManager.LayoutParams dialogParams = new WindowManager.LayoutParams();
            
            dialogParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
            dialogParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
            dialogParams.flags = WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;
            // mParams.format = PixelFormat.TRANSLUCENT;
            dialogParams.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;
            dialogParams.setTitle("Toast11111111111");
            
            dialogList.add(view);
            
            mWM.addView(view, dialogParams);
            
        }
        else
        {
            
            Log.e(TAG, "The view is null while trying to attach a view to the WindowManager");
            
        }
        
    }
    
    public synchronized void addView(Context context, View view, WindowManager.LayoutParams dialogParams)
    {
        
        if (view != null)
        {
            WindowManager mWM = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            
            dialogList.add(view);
            
            mWM.addView(view, dialogParams);
            
        }
        else
        {
            
            Log.e(TAG, "The view is null while trying to attach a view to the WindowManager");
            
        }
        
    }
    
    public synchronized void removeView(Context context, View view)
    {
        if (view != null)
        {
            WindowManager mWM = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            if (dialogList.contains(view))
            {
                dialogList.remove(view);
                mWM.removeView(view);
            }
        }
        else
        {
            Log.e(TAG, "The view is null while trying to dettach a view to the WindowManager");
        }
        
    }
    
    public void adjustView(Context context, View checkDialog, int... idArray)
    {
        
        // int ll_Max_Width = getWidthOfDialog(checkDialog,
        // R.id.tv_showPvrTitle,
        // R.id.ll_buttons);
        
        int ll_Max_Width = getWidthOfDialog(checkDialog, idArray);
        
        Log.e(TAG, "getWidthOfDialog()::ll_Max_Width = " + ll_Max_Width);
        
        WindowManager mWM = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
        
        Display disPlay = mWM.getDefaultDisplay();
        
        DisplayMetrics outMetrics = new DisplayMetrics();
        
        disPlay.getMetrics(outMetrics);
        
        int disWidth = outMetrics.widthPixels;
        
        Log.e(TAG, "before adjust disWidth = " + disWidth);
        
        disWidth = disWidth * 3 / 5;
        
        Log.e(TAG, "after adjust disWidth = " + disWidth);
        
        if (ll_Max_Width > disWidth)
        {
            ll_Max_Width = disWidth;
        }
        
        Log.e(TAG, "adjustByDisWidth::ll_Max_Width = " + ll_Max_Width);
        LinearLayout ll_dialog = (LinearLayout)checkDialog.findViewById(R.id.ll_dialog);
        LayoutParams dialog_Params = null;
        
        if (ll_dialog != null)
        {
            dialog_Params = (LayoutParams)ll_dialog.getLayoutParams();
            dialog_Params.width = ll_Max_Width + dip2px(context, 40) * 2;
            ll_dialog.setLayoutParams(dialog_Params);
        }
    }
    
    private int getWidthOfDialog(View checkDialog, int... idArray)
    {
        
        int maxWidth = 0;
        
        int mWidth = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
        int mHeight = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
        
        View view = null;
        
        for (int i = 0; i < idArray.length; i++)
        {
            
            view = checkDialog.findViewById(idArray[i]);
            
            if (view != null)
            {
                
                view.measure(mWidth, mHeight);
                
                int width = view.getMeasuredWidth();
                
                Log.e(TAG, i + " width = " + width);
                
                if (width > maxWidth)
                {
                    
                    maxWidth = width;
                    
                }
            }
            
        }
        
        return maxWidth;
        
    }
    
    public static int sp2px(Context context, int textSize)
    {
        
        float fontSacle = context.getResources().getDisplayMetrics().scaledDensity;
        
        return (int)(textSize * fontSacle + 0.5f);
        
    }
    
    public static int dip2px(Context context, float dipValue)
    {
        
        final float scale = context.getResources().getDisplayMetrics().density;
        
        return (int)(dipValue * scale + 0.5f);
    }
    
}
