/*
 * �� �� ��:  DBMail.java
 * ��    Ȩ:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  gtsong
 * �޸�ʱ��:  2012-9-29
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.db;

import android.net.Uri;
import android.provider.BaseColumns;

/**
 * <mail database.>
 * 
 * @author  gtsong
 * @version  [�汾��, 2012-9-29]
 * @see  [�����/����]
 * @since  [��Ʒ/ģ��汾]
 */
public class DBMail implements BaseColumns
{
    public static final String TABLE_MAIL_NAME = "db_mail_table";
    public static final String MAIL_AUTHORITY = "com.pbi.dvb.provider.MailProvider";
    
    private DBMail(){}
    public static final Uri CONTENT_URI = Uri.parse("content://"+MAIL_AUTHORITY+"/"+TABLE_MAIL_NAME);
    
    public static final int ITEM = 1;
    public static final int ITEM_ID = 2;
    
    public static final String CONTENT_TYPE ="vnd.android.cursor.dir/vnd.dvb.mail";
    public static final String CONTENT_ITEM_TYPE="vnd.android.cursor.item/vnd.dvb.mail";
    
    public static final String MAIL_ID = "m_id";
    public static final String MAIL_INDEX = "m_index";
    public static final String MAIL_TYPE = "m_type";
    public static final String MAIL_STATUS = "m_status";
    public static final String MAIL_CLASS = "m_class";
    public static final String MAIL_PRIORITY = "m_priority";
    public static final String MAIL_PERIOD = "m_period";
    public static final String MAIL_FROM = "m_from";
    public static final String MAIL_TITLE = "m_title";
    public static final String MAIL_POST_TIME = "m_time";
    public static final String MAIL_CONTENT = "m_content";
    
    
}
