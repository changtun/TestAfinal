package com.pbi.dvb.service;

import android.app.Service;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.os.IBinder;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.PvrDialogManager;

public class PvrInitService extends Service
{
    
    private static final String TAG = "PvrInitService";
    
    @Override
    public IBinder onBind(Intent intent)
    {
        
        return null;
        
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        
        init_Pvr_Database();
        
        return super.onStartCommand(intent, flags, startId);
        
    }
    
    private void init_Pvr_Database()
    {
        
        for (int i = 0; i < 3; i++)
        {
            
            ContentResolver resolver = getContentResolver();
            
            Cursor cursor = resolver.query(DBPvr.CONTENT_URI, new String[] {"count(*)"}, null, null, null);
            
            if (cursor != null && cursor.moveToNext())
            {
                
                int count = cursor.getInt(0);
                
                cursor.close();
                
                LogUtils.e(TAG, "In PvrInitService:: Need to insert " + (DBPvr.MAX_RECORD_ITEMS - count)
                    + " items... count = " + count);
                
                if (count < DBPvr.MAX_RECORD_ITEMS && count >= 0)
                {
                    
                    LogUtils.e(TAG, "Begin to init_Pvr_Database");
                    
                    PvrDao pvrDao = new PvrDaoImpl(PvrInitService.this);
                    
                    pvrDao.init_Pvr_Database(DBPvr.MAX_RECORD_ITEMS - count);
                    
                }
                else if (count >= DBPvr.MAX_RECORD_ITEMS)
                {
                    LogUtils.e(TAG, "---->>>> <<<<---");
                    
                    // stopSelf();
                    
                    return;
                    
                }
                
            }
            else if (cursor != null)
            {
                
                // cursor.close();
                
                LogUtils.e(TAG, "-------->>>> The cursor can not moveToNext...Maybe the result is null <<<<--------");
                
            }
            else
            {
                
                LogUtils.e(TAG, "-------->>>> The cursor is null <<<<--------");
                
            }
        }
        
        show_Init_Error_Dialog();
    }
    
    private void show_Init_Error_Dialog()
    {
        
        View pvr_Init_Error = View.inflate(PvrInitService.this, R.layout.pvr_check_box_with_one_button, null);
        
        TextView tv_showTaskInvalid = (TextView)pvr_Init_Error.findViewById(R.id.tv_showTaskInvalid);
        
        tv_showTaskInvalid.setText("Notice: The initialization of pvr module encounter some error.\nPlease try to restart the Set-top box for normal use.");
        
        Button bt_Ok = (Button)tv_showTaskInvalid.findViewById(R.id.bt_Ok);
        
        bt_Ok.setTag(pvr_Init_Error);
        
        bt_Ok.setOnClickListener(new OnClickListener()
        {
            
            @Override
            public void onClick(View v)
            {
                
                View view = (View)v.getTag();
                
                PvrDialogManager.getInstance().removeView(PvrInitService.this, view);
                
                stopSelf();
                
            }
        });
        
        PvrDialogManager.getInstance().adjustView(PvrInitService.this,
            tv_showTaskInvalid,
            R.id.tv_showTaskInvalid,
            R.id.ll_buttons);
        
        PvrDialogManager.getInstance().addView(PvrInitService.this, pvr_Init_Error);
        
    }
}
