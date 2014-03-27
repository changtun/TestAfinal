/*
 * File Name:  PvrPlayList.java
 * Copyright:  Beijing Jaeger Communication Electronic Technology Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * Descriptions:  <Descriptions>
 * Changed By:  gtsong
 * Changed Time:  2013-12-19
 * Changed Content:  <Changed Content>
 */
package com.pbi.dvb;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.app.Dialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ListView;
import android.widget.TextView;

import com.pbi.dvb.adapter.PvrPlayAdapter;
import com.pbi.dvb.domain.MountInfoBean;
import com.pbi.dvb.domain.PvrFileBean;
import com.pbi.dvb.dvbinterface.NativePlayer;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.receiver.UsbStateReceiver;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.utils.StorageUtils;
public class PvrPlayList extends Activity
{
    private String TAG = "PvrPlayList";
    
    /** declare usb stated receiver */
    private UsbStateReceiver usbStateReceiver;
    
    private PvrPlayAdapter adapter;
    
    private ArrayList<PvrFileBean> pvrFileLists;
    
    private Dialog cDialog;
    
    private Dialog noDeviceDialog;
    
    private int selection;
    
    private ListView listView;
    
    private View itemView;
    
    private int index = 0;
    
    private String pvrPath = "";
    
    private static final String pvr = "pvr";
    
    private int screenMode = 0;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.pvr_play_list);
        adapter = new PvrPlayAdapter(PvrPlayList.this);
        listView = (ListView)findViewById(R.id.pvr_play_listview);
        listView.setOnItemClickListener(new OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                itemView = view;
                index = position;
                startPvrOrderInfos();
            }
        });
        
        listView.setOnItemSelectedListener(new OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2, long arg3)
            {
                selection = arg2;
            }
            
            @Override
            public void onNothingSelected(AdapterView<?> arg0)
            {
            }
        });
        
    }
    
    protected void onStart()
    {
        super.onStart();
        //screenMode = CommonUtils.getScreenMode();
        usbStateReceiver = new UsbStateReceiver(this, mHandler);
        usbStateReceiver.registerUsbReceiver();
        LoadListView();
    }
    
    protected void onRestart()
    {
        super.onRestart();
        listView.setSelection(index);
        listView.requestFocus(index);
    }
    
    protected void onStop()
    {
        //CommonUtils.setScreenMode(screenMode);
        super.onStop();
        unregisterReceiver(usbStateReceiver);
    }
    
    private void LoadListView()
    {
        // check the hdd;
        StorageUtils util = new StorageUtils(this);
        int check = util.checkHDDSpace();
        switch (check)
        {
            case 0:
            case 1:
                MountInfoBean mountInfoBean = util.getMobileHDDInfo();
                if (null != mountInfoBean)
                {
                    String recFilePath = mountInfoBean.getPath();
                    Log.i(TAG, "mountInfoBean.getPath() = " + recFilePath);// "/mnt/sda/sda1"
                    pvrPath = recFilePath + "/" + pvr;
                    File pvrFile = new File(pvrPath);
                    Log.i(TAG, "pvrFile.exists() = " + pvrFile.exists());
                    if (!pvrFile.exists())
                    {
                        pvrFile.mkdirs();
                    }
                    initListView(pvrPath);
                }
                break;
            case 2:
                mHandler.sendEmptyMessage(UsbStateReceiver.USB_UNMOUNT);
                break;
        }
    }
    
    public void initListView(String pvrPath)
    {
        File[] files = new File(pvrPath).listFiles();
        if (null == files)
        {
            return;
        }
        StringBuilder sb = new StringBuilder();
        pvrFileLists = new ArrayList<PvrFileBean>();
        int index = 1;
        for (File file : files)
        {
            if (!file.isDirectory())
            {
                sb.setLength(0);
                sb = getPostfix(file.getName());
                if ("ts".equals(sb.toString()) && file.length() > 0)
                {
                    Log.i(TAG, "initListView , file.getName() = " + file.getName());
                    if (!"time.ts".equals(file.getName()))
                    {
                        PvrFileBean fileBean = new PvrFileBean(file.getName(), index++);
                        pvrFileLists.add(fileBean);
                    }
                }
            }
        }
        
        adapter.setList(pvrFileLists);
        listView.setAdapter(adapter);
        listView.invalidate();
    }
    
    private StringBuilder getPostfix(String filename)
    {
        int position = filename.lastIndexOf(".");
        if (position >= 0)
            return new StringBuilder(filename.substring(position + 1));
        else
            return new StringBuilder("");
    }
    
    public void startPvrOrderInfos()
    {
        //CommonUtils.setScreenMode(1);// 0:NORMAL 1:FULLSTRETCH 2:RATIO4_3 3:RATIO16_9
        
        Intent intent = new Intent(this, PvrPlay.class);
        TextView textView = (TextView)itemView.findViewById(R.id.pvr_play_name);
        intent.putExtra("absolutePath", pvrPath + "/" + textView.getText().toString());
        intent.putExtra("fileName", textView.getText().toString());
        startActivity(intent);
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
//        if (keyCode == Config.DELETE)
       if (keyCode == Config.KEY_SWITCH)
        {
            if (pvrFileLists.size() <= 0)
            {
                return false;
            }
            
            cDialog =
                DialogUtils.twoButtonsDialogCreate(PvrPlayList.this,
                    getString(R.string.pvr_delete_file_tips),
                    new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            removePvrFile();
                            
                            if (null != cDialog)
                            {
                                cDialog.dismiss();
                            }
                        }
                    },
                    new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            if (null != cDialog)
                            {
                                cDialog.dismiss();
                            }
                            
                        }
                    });
        }
        
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            finish();
        }
        return false;
    }
    
    private void removePvrFile()
    {
        PvrFileBean itemBean;
        if (pvrFileLists.size() > 0)
        {
            itemBean = pvrFileLists.get(selection);
        }
        else
        {
            return;
        }
        
        // delete the item in the page.
        pvrFileLists.remove(selection);
        adapter.setList(pvrFileLists);
        adapter.notifyDataSetChanged();
        
        // delete the file in the storage
        StorageUtils util = new StorageUtils(PvrPlayList.this);
        MountInfoBean mountInfoBean = util.getMobileHDDInfo();
        if (null == mountInfoBean)
        {
            return;
        }
        
        // set ts file's name.
        if (null == itemBean)
        {
            return;
        }
        String filePath = mountInfoBean.getPath() + "/pvr/" + itemBean.getName();
        LogUtils.printLog(1, 3, TAG, "------pvr file path------->>>" + filePath);
        
        File file = new File(filePath);
        if (file.exists())
        {
            NativePlayer dvbPlayer = NativePlayer.getInstance();
            //dvbPlayer.DVBPlayerInit(null);
            
            // combination the delete content.
            int index = filePath.indexOf(".");
            String name = filePath.substring(0, index); // add "."
            
            LogUtils.printLog(1, 3, TAG, "------delete content------->>>" + name + ".t*");
            dvbPlayer.DVBPlayerPvrRemoveFile(name + ".ts"); // contain .ts and .config
            dvbPlayer.DVBPlayerPvrRemoveFile(name + ".ts.config"); // contain .ts and .config
            int i = 1;
            file = new File(filePath + ".[" + i + "]");
            while(file.exists())
            {
            	dvbPlayer.DVBPlayerPvrRemoveFile(filePath + ".[" + i + "]");
				i++;
				file = new File(filePath + ".[" + i + "]");
            }
        }
    }
    
    private Handler mHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case UsbStateReceiver.USB_MOUNT:
                    LogUtils.printLog(1, 3, TAG, "--->>> mount external device success!");
                    if(null != noDeviceDialog)
                    {
                        noDeviceDialog.dismiss();
                    }
                    LoadListView();
                    break;
                case UsbStateReceiver.USB_UNMOUNT:
                    LogUtils.printLog(1, 3, TAG, "--->>> remove external device success!");
                    if(null == noDeviceDialog)
                    {
                        noDeviceDialog = DialogUtils.noButtonDialogCreate(PvrPlayList.this,getString(R.string.hdd_no));
                        return;
                    }
                    noDeviceDialog.show();
                    
                    if(null == pvrFileLists)
                    {
                        return;
                    }
                    pvrFileLists.clear();
                    adapter.setList(pvrFileLists);
                    adapter.notifyDataSetChanged();
                    break;
            
            }
        }
        
    };
}
