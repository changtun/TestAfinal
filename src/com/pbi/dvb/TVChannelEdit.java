package com.pbi.dvb;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import android.app.Activity;
import android.app.Dialog;
import android.content.ContentProviderOperation;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ListView;

import com.pbi.dvb.adapter.EditProgramsAdapter;
import com.pbi.dvb.dao.PageDao;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.PageDaoImpl;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.db.DBService;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.global.PasswordCallback;
import com.pbi.dvb.utils.CommonUtils;
import com.pbi.dvb.utils.DialogUtils;
import com.pbi.dvb.utils.LogUtils;
import com.pbi.dvb.view.CustomProgressDialog;

public class TVChannelEdit extends DVBBaseActivity
{
    private static final String TAG = "TVChannelEdit";
    
    private ListView lvEditContent;
    private ServiceInfoDao serviceDao;
    private EditProgramsAdapter adapter;
    
    private List<ServiceInfoBean> channelList;
    private List<ServiceInfoBean> currentList = new LinkedList<ServiceInfoBean>();
    
    private PageDao pageDao;
    private ServiceInfoBean sBean;
    
    private int listViewSize = 10;
    private int currentPage = 1;
    private int flag = 1;
    private int selection;
    private boolean isDeal = false;
    
    private Dialog passwordDialog;
    private CustomProgressDialog dialog;
    private Dialog cDialog;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        passwordDialog = DialogUtils.passwordDialogCreate(this, new PasswordCallback()
        {
            
            @Override
            public void passwordCorrectEvent()
            {
                setContentView(R.layout.channel_edit);
                initWidget();
            }
        });
        
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        if (null != passwordDialog)
        {
            passwordDialog.dismiss();
        }
        if (null != cDialog)
        {
            cDialog.dismiss();
        }
        if (null != dialog)
        {
            dialog.dismiss();
        }
        
        releaseResource();
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    private void initWidget()
    {
        serviceDao = new ServiceInfoDaoImpl(this);
        lvEditContent = (ListView)this.findViewById(R.id.lv_channel_edit);
        lvEditContent.setDivider(null);
        
        adapter = new EditProgramsAdapter(this);
        channelList = serviceDao.getAllChannelInfo(true);
        pageDao = new PageDaoImpl(channelList, listViewSize);
        if (null != channelList)
        {
            setCurrentList();
        }
    }
    
    @SuppressWarnings("unchecked")
    private void setCurrentList()
    {
        pageDao.setCurrentPage(currentPage);
        currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
        
        adapter.setChannelList(currentList);
        lvEditContent.setAdapter(adapter);
        lvEditContent.setSelection(0);
        lvEditContent.requestFocus(0);
        lvEditContent.setOnItemSelectedListener(new ChannelItemSelectedListener());
    }
    
    private void startProgressDialog()
    {
        if (null == dialog)
        {
            dialog = CustomProgressDialog.createDialog(this, R.layout.progress_dialog);
            dialog.setMessage(getString(R.string.SaveProgram));
        }
        dialog.show();
    }
    
    @SuppressWarnings("unchecked")
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        LogUtils.printLog(1, 3, TAG, "dispatchKeyEvent:: +on start!!");
        if (KeyEvent.ACTION_DOWN == event.getAction())
        {
            /*-------------向上翻页---------------*/
            if (event.getKeyCode() == Config.PAGE_UP)
            {
                if(pageDao.getCurrentPage()!=1)
                {
                    pageDao.prePage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage--;
                }
            }
            /*-------------向下翻页---------------*/
            if (event.getKeyCode() == Config.PAGE_DOWN)
            {
                if(pageDao.getCurrentPage() != pageDao.getPageNum())
                {
                    pageDao.nextPage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage++;
                }
            }
        }
        return super.dispatchKeyEvent(event);
    }
    
    @SuppressWarnings("unchecked")
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        /*-------------处理Epg---------------*/
        if (keyCode == Config.KEY_EPG && event.getRepeatCount() == 0)
        {
            String superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
            if (null == superPwd)
            {
                superPwd = Config.SUPER_PASSWORD;
            }
            CommonUtils.skipActivity(this, EpgActivity.class, "superPwd", superPwd, -1);
        }
//        if (keyCode == Config.KEY_SEARCH && event.getRepeatCount() == 0)
//        {
//            CommonUtils.skipActivity(this, TVChannelSearch.class, -1);
//        }
        /*-------------退出处理---------------*/
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0)
        {
            cDialog =
                DialogUtils.twoButtonsDialogCreate(this,
                    getString(R.string.Save_tips),
                    new PositiveButtonListener(),
                    new NegativeButtonListener());
        }
        /*-------------设置喜爱---------------*/
        if (keyCode == Config.KEY_RED)
        {
            Log.i(TAG, "------Red Key-Fav-------");
            sBean = currentList.get(selection);
            int favFlag = sBean.getFavFlag();
            if (favFlag == 1)
            {
                sBean.setFavFlag(0);
            }
            else
            {
                sBean.setFavFlag(1);
            }
            currentList.remove(selection);
            currentList.add(selection, sBean);
            adapter.setChannelList(currentList);
            adapter.notifyDataSetChanged();
            
            isDeal = true;
            sBean = null;
        }
        /*-------------设置全部刪除---------------*/
        if (keyCode == Config.KEY_GREEN)
        {
            Log.i(TAG, "------Green Key-Del all-------");
            for (ServiceInfoBean serviceBean : channelList)
            {
                if (serviceBean.getDelFlag() == 0)
                {
                    serviceBean.setDelFlag(1);
                }
                else
                {
                    serviceBean.setDelFlag(0);
                }
            }
            
            adapter.setChannelList(currentList);
            adapter.notifyDataSetChanged();
            isDeal = true;
        }
        /*-------------设置加锁---------------*/
        if (keyCode == Config.KEY_YELLOW)
        {
            Log.i(TAG, "------Yellow Key-Lock-------");
            sBean = currentList.get(selection);
            int lockFlag = sBean.getLockFlag();
            if (lockFlag == 1)
            {
                sBean.setLockFlag(0);
            }
            else
            {
                sBean.setLockFlag(1);
            }
            currentList.remove(selection);
            currentList.add(selection, sBean);
            adapter.setChannelList(currentList);
            adapter.notifyDataSetChanged();
            
            isDeal = true;
            sBean = null;
        }
        /*-------------设置删除---------------*/
        if (keyCode == Config.KEY_BLUE)
        {
            Log.i(TAG, "------Blue Key-Del-------");
            sBean = currentList.get(selection);
            int delFlag = sBean.getDelFlag();
            if (delFlag == 1)
            {
                sBean.setDelFlag(0);
            }
            else
            {
                sBean.setDelFlag(1);
            }
            currentList.remove(selection);
            currentList.add(selection, sBean);
            adapter.setChannelList(currentList);
            adapter.notifyDataSetChanged();
            
            isDeal = true;
            sBean = null;
        }
        /*-------------向上---------------*/
        if (keyCode == KeyEvent.KEYCODE_DPAD_UP && event.getRepeatCount() == 0)
        {
            Log.i(TAG, "---------UP Key Clicked!!-------------------------");
            --flag;
            if (flag < 1)
            {
                if (pageDao.getCurrentPage() == 1)
                {
                    pageDao.lastPage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage = pageDao.getPageNum();
                    
                    if (null != currentList && currentList.size() > 0)
                    {
                        lvEditContent.setSelection(currentList.size() -1);
                        lvEditContent.requestFocus(currentList.size() -1);
                    }
                    flag = 1;
                }
                else
                {
                    pageDao.prePage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage -- ;
                    
                    if (null != currentList && currentList.size() > 0)
                    {
                        lvEditContent.setSelection(currentList.size() -1);
                        lvEditContent.requestFocus(currentList.size() -1);
                    }
                    flag = 1;
                }
            }
        }
        /*-------------向下---------------*/
        if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN && event.getRepeatCount() == 0)
        {
            Log.i(TAG, "---------Down Key Clicked!!-------------------------");
            ++flag;
            if (flag > 1)
            {
                if (pageDao.getCurrentPage() == pageDao.getPageNum())
                {
                    pageDao.headPage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage = 1;
                    lvEditContent.setSelection(0);
                    lvEditContent.requestFocus(0);
                    flag = 1;
                }
                else
                {
                    pageDao.nextPage();
                    currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                    adapter.setChannelList(currentList);
                    adapter.notifyDataSetChanged();
                    currentPage ++ ;
                    
                    lvEditContent.setSelection(0);
                    lvEditContent.requestFocus(0);
                    flag = 1;
                }
            }
        }
        return false;
    }
    
    class ChannelItemSelectedListener implements OnItemSelectedListener
    {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            selection = position;
        }
        
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
    }
    
    public void releaseResource()
    {
        lvEditContent = null;
        serviceDao = null;
        pageDao = null;
        sBean = null;
        
        adapter = null;
        
        channelList = null;
        currentList = null;
    }
    
    class PositiveButtonListener implements OnClickListener
    {
        public void onClick(View v)
        {
            if (null != cDialog)
            {
                cDialog.dismiss();
            }
            new ChannelEditProcess().execute();
        }
    }
    
    class NegativeButtonListener implements OnClickListener
    {
        public void onClick(View v)
        {
            if (null != cDialog)
            {
                cDialog.dismiss();
                finish();
            }
        }
    }
    
    class ChannelEditProcess extends AsyncTask<Void, Void, Void>
    {
        protected void onPreExecute()
        {
            startProgressDialog();
        }
        
        @SuppressWarnings("unchecked")
        protected Void doInBackground(Void... params)
        {
            List<ServiceInfoBean> delList = new ArrayList<ServiceInfoBean>();
            ArrayList<ContentProviderOperation> ops = new ArrayList<ContentProviderOperation>();
            
            // 删除标记项
            if (isDeal)
            {
                Log.i(TAG, "-----------Before-Del--ChannelList--Size---------->>>" + channelList.size());
                for (ServiceInfoBean serviceInfoBean : channelList)
                {
                    int delTag = serviceInfoBean.getDelFlag();
                    if (delTag == 1)
                    {
                        delList.add(serviceInfoBean);
                    }
                }
                
                for (int i = 0; i < delList.size(); i++)
                {
                    ops.add(serviceDao.clearServiceDataByChannelNumber(delList.get(i).getChannelNumber()));
                }
                channelList.remove(delList);
                
                if (channelList.size() > 0)
                {
                    Log.i(TAG, "-----------After-Del--ChannelList--Size---------->>>" + channelList.size());
                    for (int i = 0; i < channelList.size(); i++)
                    {
                        ServiceInfoBean infoBean = channelList.get(i);
                        ops.add(serviceDao.updateServiceSettings(infoBean, infoBean.getChannelNumber()));
                    }
                }
                try
                {
                    getContentResolver().applyBatch(DBService.SERVICE_AUTHORITY, ops);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                
                delList.clear();
                currentList = (List<ServiceInfoBean>)pageDao.getCurrentList();
                adapter.setChannelList(currentList);
                isDeal = false;
            }
            return null;
        }
        
        protected void onPostExecute(Void result)
        {
            if (null != dialog)
            {
                dialog.dismiss();
            }
            finish();
        }
    }
    
}
