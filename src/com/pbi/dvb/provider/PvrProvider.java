/*
 * 文 件 名:  PvrProvider.java
 * 版    权:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  gtsong
 * 修改时间:  2012-12-1
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.provider;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.db.DBopenHelper;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author 姓名 工号
 * @version [版本号, 2012-12-1]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class PvrProvider extends ContentProvider
{
    // private DBopenHelper dBOpenHelper;
    private DBopenHelper dBOpenHelper;
    
    private static final UriMatcher mMatcher;
    
    private static final String TAG = "PvrProvider";
    
    static
    {
        mMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        mMatcher.addURI(DBPvr.PVR_AUTHORITY, DBPvr.TABLE_PVR_NAME, DBPvr.ITEM);
        mMatcher.addURI(DBPvr.PVR_AUTHORITY, DBPvr.TABLE_PVR_NAME + "/#", DBPvr.ITEM_ID);
    }
    
    public boolean onCreate()
    {
        
        Log.e(TAG, "PvrProvider onCreate");
        
        // dBOpenHelper = new DBopenHelper(this.getContext());
        dBOpenHelper = new DBopenHelper(this.getContext());
        
        return true;
        
    }
    
    public String getType(Uri uri)
    {
        
        switch (mMatcher.match(uri))
        {
        
            case DBPvr.ITEM:
                
                return DBPvr.CONTENT_TYPE;
                
            case DBPvr.ITEM_ID:
                
                return DBPvr.CONTENT_ITEM_TYPE;
                
            default:
                
                throw new IllegalArgumentException("Unknown URI: " + uri);
                
        }
    }
    
    public Uri insert(Uri uri, ContentValues values)
    {
        
        Log.e(TAG, "insert begin");
        
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        
        long rowId;
        
        if (mMatcher.match(uri) != DBPvr.ITEM)
        {
            throw new IllegalArgumentException("Unknown URI:" + uri);
        }
        
        rowId = db.insert(DBPvr.TABLE_PVR_NAME, DBPvr.PVR_ID, values);
        
        if (rowId > 0)
        {
            
            Uri noteUri = ContentUris.withAppendedId(DBPvr.CONTENT_URI, rowId);
            
            getContext().getContentResolver().notifyChange(noteUri, null);
            getContext().getContentResolver().notifyChange(uri, null);
            
            Log.e(TAG, "add success");
            Log.e(TAG, "insert success");
            
            return noteUri;
            
        }
        
        Log.e(TAG, "insert error");
        
        throw new SQLException("Fail to insert row into " + uri);
    }
    
    public int delete(Uri uri, String selection, String[] selectionArgs)
    {
        
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        
        int count;
        
        switch (mMatcher.match(uri))
        {
        
            case DBPvr.ITEM:
                
                count = db.delete(DBPvr.TABLE_PVR_NAME, selection, selectionArgs);
                
                break;
            
            case DBPvr.ITEM_ID:
                
                String id = uri.getPathSegments().get(1);
                
                count =
                    db.delete(DBPvr.TABLE_PVR_NAME, DBPvr.PVR_ID + "=" + id
                        + (!TextUtils.isEmpty(selection) ? "AND (" + selection + ")" : ""), selectionArgs);
                
                break;
            
            default:
                
                throw new SQLException("Fail to delete row into + " + uri);
        }
        
        getContext().getContentResolver().notifyChange(uri, null);
        
        return count;
    }
    
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs)
    {
        
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        
        int count;
        
        switch (mMatcher.match(uri))
        {
        
            case DBPvr.ITEM:
                count = db.update(DBPvr.TABLE_PVR_NAME, values, selection, selectionArgs);
                
                break;
            
            case DBPvr.ITEM_ID:
                
                String id = uri.getPathSegments().get(1);
                
                count =
                    db.update(DBPvr.TABLE_PVR_NAME, values, DBPvr.PVR_ID + "=" + id
                        + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")" : ""), selectionArgs);
                
                break;
            
            default:
                
                throw new IllegalArgumentException("Fail to update row into " + uri);
        }
        
        getContext().getContentResolver().notifyChange(uri, null);
        
        return count;
    }
    
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder)
    {
        
        // Log.e(TAG, "query begin");
        
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        
        Cursor cursor;
        
        switch (mMatcher.match(uri))
        {
        
            case DBPvr.ITEM:
                
                // Log.e(TAG, "in DBPvr.ITEM");
                
                cursor = db.query(DBPvr.TABLE_PVR_NAME, projection, selection, selectionArgs, null, null, sortOrder);
                break;
            
            case DBPvr.ITEM_ID:
                
                // Log.e(TAG, "In DBPvr.ITEM_ID");
                
                String id = uri.getPathSegments().get(1);
                
                cursor =
                    db.query(DBPvr.TABLE_PVR_NAME,
                        projection,
                        DBPvr.PVR_ID + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")" : ""),
                        selectionArgs,
                        null,
                        null,
                        sortOrder);
                
                break;
            
            default:
                
                throw new IllegalArgumentException("Fail to query row into " + uri);
                
        }
        
        // getContext().getContentResolver().notifyChange(uri, null);
        
        // Log.e(TAG, "query end");
        
        return cursor;
    }
}