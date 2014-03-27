package com.pbi.dvb.ca;

import java.util.List;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ListView;
import android.widget.TextView;

import com.pbi.dvb.DVBBaseActivity;
import com.pbi.dvb.EpgActivity;
import com.pbi.dvb.R;
import com.pbi.dvb.adapter.MailAdapter;
import com.pbi.dvb.dao.MailDao;
import com.pbi.dvb.dao.PageDao;
import com.pbi.dvb.dao.impl.MailDaoImpl;
import com.pbi.dvb.dao.impl.PageDaoImpl;
import com.pbi.dvb.domain.MailBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;

public class CaEmail extends DVBBaseActivity
{
    private String TAG = "CaEmail";
    
    private TextView receivedTView;
    
    private TextView remainTView;
    
    private ListView listView;
    
    private MailDao mailDao;
    
    private List<MailBean> emailBeans;
    
    private MailAdapter mailAdapter;
    
    private long receivedCount;
    
    private View deleteView;
    
    private static final int EMAIL_SPACE = 100; // 邮件空间大小
    
    // 分页
    private List<MailBean> currentList;
    
    private PageDao pd;
    
    private int mailIndex = 0;
    
    private final int pageSize = 10;
    
    private int currentPosition;
    
    private int currentPageNum;
    
    private int flag = 1;
    
    private boolean notEmpty;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ca_email);
        
        receivedTView = (TextView)findViewById(R.id.ca_email_received);
        remainTView = (TextView)findViewById(R.id.ca_email_space);
        listView = (ListView)findViewById(R.id.ca_email_lv);
        
        // 分页初始化
        mailIndex = 0; // 在邮件集合中的索引(0,1,...)
        currentPageNum = mailIndex / pageSize + 1; // 列表当前显示的页数(1,2,...)
        currentPosition = mailIndex % pageSize; // 列表项在当前页中的位置(0,1,...)
        
        mailDao = new MailDaoImpl(CaEmail.this);
        refreshCurEmailView(currentPageNum);
        
        listView.setOnItemSelectedListener(new OnItemSelectedListener()
        {
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
            {
                deleteView = view;
                currentPosition = position;// 该记录在当前页中的索引
            }
            
            public void onNothingSelected(AdapterView<?> parent)
            {
            }
        });
        
        listView.setOnItemClickListener(new OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                // 1.修改邮件状态
                TextView emailIdTextView = (TextView)deleteView.findViewById(R.id.email_item_id);
                int emailId = Integer.valueOf(emailIdTextView.getText().toString());
                mailDao.updateMailStatus(emailId, 1);
                // 2.刷新列表
                refreshCurEmailView(currentPageNum);
                // 3.查看该项详细信息
                Intent intent = new Intent(CaEmail.this, CaEmailDetail.class);
                Bundle bundle = new Bundle();
                bundle.putInt("emailId", emailId);
                intent.putExtras(bundle);
                CaEmail.this.startActivity(intent);
            }
        });
        
    }
    
    private void refreshCurEmailView(int curPageNum)
    {
        
        receivedCount = mailDao.getMailAccounts();
        receivedTView.setText(String.valueOf(receivedCount));
        
        if (receivedCount == 0)
        {
            notEmpty = false;
        }
        else
        {
            notEmpty = true;
        }
        
        if (receivedCount > EMAIL_SPACE)
        {// 邮件已满,只显示其中的30条记录
            emailBeans = mailDao.getPartMailInfo(EMAIL_SPACE);
            remainTView.setText("0");
        }
        else
        {// 邮件未满
            emailBeans = mailDao.getMailInfo();
            remainTView.setText(String.valueOf(EMAIL_SPACE - receivedCount));
        }
        
        // 分页
        pd = new PageDaoImpl(emailBeans, pageSize);
        
        // 自定义适配器，初始化当前页(curPageNum)数据
        pd.setCurrentPage(curPageNum);
        currentList = (List<MailBean>)pd.getCurrentList();
        mailAdapter = new MailAdapter(this, curPageNum, pageSize);
        mailAdapter.setEmailList(currentList);
        
        listView.setAdapter(mailAdapter);
        listView.setSelection(currentPosition);
        listView.requestFocus(currentPosition);
        
        if (receivedCount > EMAIL_SPACE)
        {// 邮件已满,提示还有未显示的邮件
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(getResources().getString(R.string.email_full_front)
                + String.valueOf(receivedCount - EMAIL_SPACE) + getResources().getString(R.string.email_full_behind));
            
            // 确认按钮
            builder.setPositiveButton(R.string.ca_ok, new OnClickListener()
            {
                public void onClick(DialogInterface dialog, int which)
                {
                }
            });
            builder.show();
        }
    }
    
    public boolean dispatchKeyEvent(KeyEvent event)
    {
        if (KeyEvent.ACTION_DOWN == event.getAction())
        {
            if (event.getKeyCode() == Config.PAGE_UP)
            {
                if (notEmpty)
                {
                    // 向上翻页键
                    Log.i(TAG, "--------- Config.PAGE_UP Clicked-------------");
                    if (pd.getCurrentPage() != 1)
                    {
                        pd.prePage();
                        currentList = (List<MailBean>)pd.getCurrentList();
                        currentPageNum--;
                        mailAdapter.setPageNum(currentPageNum);
                        mailAdapter.setEmailList(currentList);
                        
                        listView.setAdapter(mailAdapter);
                        listView.setSelection(currentPosition);
                        listView.requestFocus(currentPosition);
                    }
                    else
                    {
                        pd.lastPage();
                        currentList = (List<MailBean>)pd.getCurrentList();
                        currentPageNum = pd.getPageNum();
                        mailAdapter.setPageNum(currentPageNum);
                        mailAdapter.setEmailList(currentList);
                        
                        listView.setAdapter(mailAdapter);
                        listView.setSelection(currentPosition);
                        listView.requestFocus(currentPosition);
                    }
                }
            }
            else if (event.getKeyCode() == Config.PAGE_DOWN)
            {
                if (notEmpty)
                {
                    // 向下翻页键
                    Log.i(TAG, "---------Config.PAGE_DOWN Clicked------------");
                    if (pd.getCurrentPage() != pd.getPageNum())
                    {
                        pd.nextPage();
                        currentList = (List<MailBean>)pd.getCurrentList();
                        currentPageNum++;
                        mailAdapter.setPageNum(currentPageNum);
                        mailAdapter.setEmailList(currentList);
                        
                        listView.setAdapter(mailAdapter);
                        listView.setSelection(currentPosition);
                        listView.requestFocus(currentPosition);
                    }
                    else
                    {
                        pd.headPage();
                        currentList = (List<MailBean>)pd.getCurrentList();
                        currentPageNum = 1;
                        mailAdapter.setPageNum(currentPageNum);
                        mailAdapter.setEmailList(currentList);
                        
                        listView.setAdapter(mailAdapter);
                        listView.setSelection(currentPosition);
                        listView.requestFocus(currentPosition);
                    }
                }
            }
        }
        return super.dispatchKeyEvent(event);
    }
    
    protected void onUserLeaveHint()
    {
        super.onUserLeaveHint();
        finish();
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {// 返回键
            finish();
        }
        else if (keyCode == Config.KEY_EPG && event.getRepeatCount() == 0)
        {
            String superPwd = android.provider.Settings.System.getString(getContentResolver(), "password");
            if (null == superPwd)
            {
                superPwd = Config.PROGRAMME_LOCK;
            }
            CommonUtils.skipActivity(this, EpgActivity.class, "superPwd", superPwd, -1);
        }
//        else if (keyCode == Config.KEY_SEARCH && event.getRepeatCount() == 0)
//        {
//            CommonUtils.skipActivity(this, TVChannelSearch.class, -1);
//        }
        else if (keyCode == Config.KEY_BLUE)
        {
            if (emailBeans.size() != 0)
            {
                String title = getResources().getString(R.string.email_delete_one);
                TextView emailIdTextView = (TextView)deleteView.findViewById(R.id.email_item_id);
                int emailId = Integer.valueOf(emailIdTextView.getText().toString());
                String emailName = ((TextView)deleteView.findViewById(R.id.email_item_title)).getText().toString();
                doDeleteEmail(title, emailName, emailId, "0");
            }
        }
        else if (keyCode == Config.KEY_GREEN)
        {
            if (emailBeans.size() != 0)
            {
                String title = getResources().getString(R.string.email_delete_all);
                doDeleteEmail(title, null, -1, "1");
            }
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_UP)
        {
            if (notEmpty)
            {
                // 上
                Log.i(TAG, "---------up Clicked : serviceList.hasFocus() ------------------");
                --flag;
                mailAdapter = new MailAdapter(this, currentPageNum, pageSize);
                if (pd.getCurrentPage() == 1 && flag < 1)
                {
                    pd.lastPage();
                    currentList = (List<MailBean>)pd.getCurrentList();
                    currentPageNum = pd.getPageNum();
                    mailAdapter.setPageNum(currentPageNum);
                    mailAdapter.setEmailList(currentList);
                    mailAdapter.notifyDataSetChanged();
                    flag = 1;
                    listView.setAdapter(mailAdapter);
                    listView.setSelection(currentList.size());
                    listView.requestFocus(currentList.size());
                }
                if (flag < 1)
                {
                    pd.prePage();
                    currentList = (List<MailBean>)pd.getCurrentList();
                    flag = 1;
                    currentPageNum--;
                    mailAdapter.setPageNum(currentPageNum);
                    mailAdapter.setEmailList(currentList);
                    mailAdapter.notifyDataSetChanged();
                    listView.setAdapter(mailAdapter);
                    listView.setSelection(currentList.size());
                    listView.requestFocus(currentList.size());
                }
            }
        }
        else if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN)
        {
            if (notEmpty)
            {
                // 下
                Log.i(TAG, "---------down Clicked ：email , listView.hasFocus() -----------");
                ++flag;
                if (pd.getCurrentPage() == pd.getPageNum() && flag > 1)
                {
                    pd.headPage();
                    currentList = (List<MailBean>)pd.getCurrentList();
                    flag = 1;
                    currentPageNum = 1;
                    mailAdapter.setPageNum(currentPageNum);
                    mailAdapter.setEmailList(currentList);
                    mailAdapter.notifyDataSetChanged();
                    listView.setSelection(1);
                    listView.requestFocus(1);
                }
                if (flag > 1)
                {
                    pd.nextPage();
                    currentList = (List<MailBean>)pd.getCurrentList();
                    flag = 1;
                    currentPageNum++;
                    mailAdapter.setPageNum(currentPageNum);
                    mailAdapter.setEmailList(currentList);
                    mailAdapter.notifyDataSetChanged();
                    listView.setSelection(1);
                    listView.requestFocus(1);
                }
                listView.setAdapter(mailAdapter);
            }
        }
        return false;
    }
    
    public void doDeleteEmail(String title, String message, final int id, final String flag)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);// 创建AlertDialog.Builder对象
        // builder.setIcon(R.drawable.question);//设置对话框图标
        builder.setTitle(title);// 设置对话框标题
        builder.setMessage(message);// 设置对话框正文消息
        
        // 确认按钮
        builder.setPositiveButton(R.string.ca_ok, new OnClickListener()
        {
            public void onClick(DialogInterface dialog, int which)
            {
                if ("0".equals(flag))
                {
                    // Log.i("CaEmail", "---builder.setPositiveButton------- flag = " + flag);
                    mailDao.deleteMailById(id);
                    // mailDao.updateMailStatus(id, 2);
                    refreshCurEmailView(currentPageNum);
                }
                else
                {
                    // Log.i("CaEmail", "---builder.setPositiveButton------- flag = " + flag);
                    mailDao.deleteMail();
                    refreshCurEmailView(currentPageNum);
                }
                
            }
        });
        
        // 取消按钮
        builder.setNegativeButton(R.string.ca_cancel, new OnClickListener()
        {
            public void onClick(DialogInterface dialog, int which)
            {
            }
        });
        builder.show();
        
    }
    
}
