package com.pbi.dvb.dao.impl;

import java.util.ArrayList;
import java.util.List;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;

import com.pbi.dvb.dao.MailDao;
import com.pbi.dvb.db.DBMail;
import com.pbi.dvb.domain.MailBean;

public class MailDaoImpl implements MailDao
{
    private Context context;
    public MailDaoImpl(Context context)
    {
        super();
        this.context = context;
    }
    
    public void addMailInfo(MailBean mailBean)
    {
        ContentValues values = new ContentValues();
        values.put(DBMail.MAIL_INDEX, mailBean.getMailIndex());
        values.put(DBMail.MAIL_TYPE, mailBean.getMailType());
        values.put(DBMail.MAIL_STATUS, mailBean.getMailStatus());
        values.put(DBMail.MAIL_CLASS, mailBean.getMailClass());
        values.put(DBMail.MAIL_PRIORITY, mailBean.getMailPriority());
        values.put(DBMail.MAIL_PERIOD, mailBean.getMailPeriod()); 
        values.put(DBMail.MAIL_FROM, mailBean.getMailFrom()); 
        values.put(DBMail.MAIL_TITLE, mailBean.getMailTitle());
        values.put(DBMail.MAIL_POST_TIME, mailBean.getMailTime());
        values.put(DBMail.MAIL_CONTENT, mailBean.getMailContent());
        
        context.getContentResolver().insert(DBMail.CONTENT_URI, values);
    }
    
    public List<MailBean> getMailInfo()
    {
        List<MailBean> listMails = new ArrayList<MailBean>();
        MailBean mailBean;
        String selection = DBMail.MAIL_TYPE+"!= 2 " ; //get undelete mails.
        String order = DBMail.MAIL_POST_TIME+" desc";
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, null, selection, null, order);
        while (cursor.moveToNext())
        {
            int idIndex =  cursor.getColumnIndex(DBMail.MAIL_ID);
            int index = cursor.getColumnIndex(DBMail.MAIL_INDEX);
            int typeIndex =  cursor.getColumnIndex(DBMail.MAIL_TYPE);
            int statusIndex =  cursor.getColumnIndex(DBMail.MAIL_STATUS);
            int classIndex =  cursor.getColumnIndex(DBMail.MAIL_CLASS);
            int priorityIndex =  cursor.getColumnIndex(DBMail.MAIL_PRIORITY);
            int periodIndex =  cursor.getColumnIndex(DBMail.MAIL_PERIOD);
            int fromIndex =  cursor.getColumnIndex(DBMail.MAIL_FROM);
            int titleIndex = cursor.getColumnIndex(DBMail.MAIL_TITLE);
            int timeIndex =  cursor.getColumnIndex(DBMail.MAIL_POST_TIME);
            int contentIndex =  cursor.getColumnIndex(DBMail.MAIL_CONTENT);
            
            int mailId = cursor.getInt(idIndex);
            int mailIndex = cursor.getInt(index);
            int mailType = cursor.getInt(typeIndex);
            Integer mailStatus = cursor.getInt(statusIndex);
            int mailClass = cursor.getInt(classIndex);
            int mailPriority = cursor.getInt(priorityIndex);
            int mailPeriod = cursor.getInt(periodIndex);
            String mailFrom = cursor.getString(fromIndex);
            String mailTitle = cursor.getString(titleIndex);
            String mailTime = cursor.getString(timeIndex);
            String mailContent = cursor.getString(contentIndex);
            
            mailBean = new MailBean(mailId, mailIndex,mailType, mailStatus, mailClass, mailPriority,mailPeriod, mailFrom,mailTitle, mailTime, mailContent);
            listMails.add(mailBean);
        }
        cursor.close();
        return listMails;
    }
    
    public List<MailBean> getPartMailInfo(int number)
    {
        List<MailBean> listMails = new ArrayList<MailBean>();
        String selection = DBMail.MAIL_TYPE+"!= 2 " ;
        MailBean mailBean;
        String order =DBMail.MAIL_POST_TIME+" desc  limit 0,"+number;
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, null, selection, null, order);
        while (cursor.moveToNext())
        {
            int idIndex =  cursor.getColumnIndex(DBMail.MAIL_ID);
            int index = cursor.getColumnIndex(DBMail.MAIL_INDEX);
            int typeIndex =  cursor.getColumnIndex(DBMail.MAIL_TYPE);
            int statusIndex =  cursor.getColumnIndex(DBMail.MAIL_STATUS);
            int classIndex =  cursor.getColumnIndex(DBMail.MAIL_CLASS);
            int priorityIndex =  cursor.getColumnIndex(DBMail.MAIL_PRIORITY);
            int periodIndex =  cursor.getColumnIndex(DBMail.MAIL_PERIOD);
            int fromIndex =  cursor.getColumnIndex(DBMail.MAIL_FROM);
            int titleIndex = cursor.getColumnIndex(DBMail.MAIL_TITLE);
            int timeIndex =  cursor.getColumnIndex(DBMail.MAIL_POST_TIME);
            int contentIndex =  cursor.getColumnIndex(DBMail.MAIL_CONTENT);
            
            int mailId = cursor.getInt(idIndex);
            int mailIndex = cursor.getInt(index);
            int mailType = cursor.getInt(typeIndex);
            Integer mailStatus = cursor.getInt(statusIndex);
            int mailClass = cursor.getInt(classIndex);
            int mailPriority = cursor.getInt(priorityIndex);
            int mailPeriod = cursor.getInt(periodIndex);
            String mailFrom = cursor.getString(fromIndex);
            String mailTitle = cursor.getString(titleIndex);
            String mailTime = cursor.getString(timeIndex);
            String mailContent = cursor.getString(contentIndex);
            
            mailBean = new MailBean(mailId, mailIndex,mailType, mailStatus, mailClass, mailPriority, mailPeriod,mailFrom, mailTitle, mailTime, mailContent);
            listMails.add(mailBean);
        }
        cursor.close();
        return listMails;
    }
    
    public MailBean getMailInfoById(int mailId)
    {
        MailBean mailBean = null;
        String selection = DBMail.MAIL_ID+"= "+mailId;
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, null, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            int index =  cursor.getColumnIndex(DBMail.MAIL_INDEX);
            int typeIndex =  cursor.getColumnIndex(DBMail.MAIL_TYPE);
            int statusIndex =  cursor.getColumnIndex(DBMail.MAIL_STATUS);
            int classIndex =  cursor.getColumnIndex(DBMail.MAIL_CLASS);
            int priorityIndex =  cursor.getColumnIndex(DBMail.MAIL_PRIORITY);
            int periodIndex =  cursor.getColumnIndex(DBMail.MAIL_PERIOD);
            int fromIndex =  cursor.getColumnIndex(DBMail.MAIL_FROM);
            int titleIndex = cursor.getColumnIndex(DBMail.MAIL_TITLE);
            int timeIndex =  cursor.getColumnIndex(DBMail.MAIL_POST_TIME);
            int contentIndex =  cursor.getColumnIndex(DBMail.MAIL_CONTENT);
            
            int mailIndex = cursor.getInt(index);
            int mailType = cursor.getInt(typeIndex);
            Integer mailStatus = cursor.getInt(statusIndex);
            int mailClass = cursor.getInt(classIndex);
            int mailPriority = cursor.getInt(priorityIndex);
            int mailPeriod = cursor.getInt(periodIndex);
            String mailFrom = cursor.getString(fromIndex);
            String mailTitle = cursor.getString(titleIndex);
            String mailTime = cursor.getString(timeIndex);
            String mailContent = cursor.getString(contentIndex);
           
            mailBean = new MailBean(mailId, mailIndex,mailType, mailStatus, mailClass, mailPriority, mailPeriod, mailFrom,mailTitle, mailTime, mailContent);
        }
        cursor.close();
        return mailBean;
    }
    
    public void deleteMailById(int mailId)
    {
        String where = DBMail.MAIL_ID+"= "+mailId;
        context.getContentResolver().delete(DBMail.CONTENT_URI, where, null);
    }
    
    public  void deleteMail()
    {
        String where = DBMail.MAIL_TYPE+" != 2";
        context.getContentResolver().delete(DBMail.CONTENT_URI, where, null);
//        ContentValues values = new ContentValues();
//        values.put(DBMail.MAIL_STATUS, 2);
//        context.getContentResolver().update(DBMail.CONTENT_URI, values, null, null);
    }
    
    public void updateMailStatus(int mailId,int status)
    {
        String where = DBMail.MAIL_ID+"= "+mailId;
        ContentValues values = new ContentValues();
        values.put(DBMail.MAIL_STATUS, status);
        context.getContentResolver().update(DBMail.CONTENT_URI, values, where, null);
    }

    public long getMailAccounts()
    {
        long amount = 0;
        String[] projection = new String[] {"count(*)"};
        String selection = DBMail.MAIL_STATUS +" != 2";
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, projection, null, null, null);
        
        if (cursor.moveToFirst())
        {
            amount= cursor.getInt(0);
        }
        cursor.close();
        return amount;
    }

    public int getMailIdByIdIndex(int mailIndex)
    {
        int mailId = 0;
        String[] projection = new String[] {DBMail.MAIL_ID};
        String selection = DBMail.MAIL_INDEX+"= "+mailIndex;
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, projection, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            int index =  cursor.getColumnIndex(DBMail.MAIL_ID);
            mailId = cursor.getInt(index);
        }
        cursor.close();
        return mailId;
    }
    
    public long  getUnreadMailInfo()
    {
        long amount = 0;
        String[] projection = new String[] {"count(*)"};
        String selection = DBMail.MAIL_STATUS +"=0 ";
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, projection, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            amount= cursor.getInt(0);
        }
        cursor.close();
        return amount;
    }
    
    
    public  void updateMailInfo(int mailId,MailBean mBean)
    {
        String where = DBMail.MAIL_ID+"= "+mailId;
        ContentValues values = new ContentValues();
        values.put(DBMail.MAIL_INDEX,mBean.getMailIndex());
        values.put(DBMail.MAIL_STATUS,mBean.getMailStatus());
        values.put(DBMail.MAIL_CLASS,mBean.getMailClass());
        values.put(DBMail.MAIL_PRIORITY,mBean.getMailPriority());
        values.put(DBMail.MAIL_PERIOD,mBean.getMailPeriod());
        values.put(DBMail.MAIL_FROM,mBean.getMailFrom());
        values.put(DBMail.MAIL_TITLE,mBean.getMailTitle());
        values.put(DBMail.MAIL_POST_TIME,mBean.getMailTime());
        values.put(DBMail.MAIL_CONTENT,mBean.getMailContent());
        context.getContentResolver().update(DBMail.CONTENT_URI, values, where, null);
    }
    
    public  MailBean getMailInfoByType2()
    {
        MailBean mailBean = null;
        String selection = DBMail.MAIL_TYPE+"=2 ";
        Cursor cursor = context.getContentResolver().query(DBMail.CONTENT_URI, null, selection, null, null);
        
        if (cursor.moveToFirst())
        {
            int idIndex =  cursor.getColumnIndex(DBMail.MAIL_INDEX);
            int index =  cursor.getColumnIndex(DBMail.MAIL_INDEX);
            int typeIndex =  cursor.getColumnIndex(DBMail.MAIL_TYPE);
            int statusIndex =  cursor.getColumnIndex(DBMail.MAIL_STATUS);
            int classIndex =  cursor.getColumnIndex(DBMail.MAIL_CLASS);
            int priorityIndex =  cursor.getColumnIndex(DBMail.MAIL_PRIORITY);
            int periodIndex =  cursor.getColumnIndex(DBMail.MAIL_PERIOD);
            int fromIndex =  cursor.getColumnIndex(DBMail.MAIL_FROM);
            int titleIndex = cursor.getColumnIndex(DBMail.MAIL_TITLE);
            int timeIndex =  cursor.getColumnIndex(DBMail.MAIL_POST_TIME);
            int contentIndex =  cursor.getColumnIndex(DBMail.MAIL_CONTENT);
            
            int mailId = cursor.getInt(idIndex);
            int mailIndex = cursor.getInt(index);
            int mailType = cursor.getInt(typeIndex);
            Integer mailStatus = cursor.getInt(statusIndex);
            int mailClass = cursor.getInt(classIndex);
            int mailPriority = cursor.getInt(priorityIndex);
            int mailPeriod = cursor.getInt(periodIndex);
            String mailFrom = cursor.getString(fromIndex);
            String mailTitle = cursor.getString(titleIndex);
            String mailTime = cursor.getString(timeIndex);
            String mailContent = cursor.getString(contentIndex);
           
            mailBean = new MailBean(mailId, mailIndex,mailType, mailStatus, mailClass, mailPriority, mailPeriod,mailFrom,mailTitle,mailTime, mailContent);
        }
        cursor.close();
        return mailBean;
    }
    
}
