package com.pbi.dvb.provider;

import java.util.ArrayList;

import com.pbi.dvb.db.DBService;
import com.pbi.dvb.db.DBopenHelper;

import android.content.ContentProvider;
import android.content.ContentProviderOperation;
import android.content.ContentProviderResult;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.OperationApplicationException;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.text.TextUtils;
/**
 * 
 * <Service Information Provider>
 * 
 * @author  gtsong
 * @version  [版本号, 2012-7-19]
 * @see  [相关类/方法]
 * @since  [产品/模块版本]
 */
public class ServiceProvider extends ContentProvider
{
    private DBopenHelper dbOpenHelper;
    
    private static final UriMatcher sMatcher;
    
    static
    {
        sMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        sMatcher.addURI(DBService.SERVICE_AUTHORITY, DBService.TABLE_SERVICE_NAME, DBService.ITEM);
        sMatcher.addURI(DBService.SERVICE_AUTHORITY, DBService.TABLE_SERVICE_NAME + "/#", DBService.ITEM_ID);
    }
    
    public boolean onCreate()
    {
        dbOpenHelper = new DBopenHelper(this.getContext());
        return true;
    }
    
    public String getType(Uri uri)
    {
        switch (sMatcher.match(uri))
        {
            case DBService.ITEM:
                return DBService.CONTENT_TYPE;
            case DBService.ITEM_ID:
                return DBService.CONTENT_ITEM_TYPE;
            default:
                throw new IllegalArgumentException("Unknown URI:" + uri);
        }
    }
    
    public Uri insert(Uri uri, ContentValues values)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        long rowId;
        if (sMatcher.match(uri) != DBService.ITEM)
        {
            throw new IllegalArgumentException("Unknown URI:" + uri);
        }
        rowId = db.insert(DBService.TABLE_SERVICE_NAME, DBService.CHANNEL_NUMBER, values);
        if (rowId > 0)
        {
            Uri noteUri = ContentUris.withAppendedId(DBService.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(noteUri, null);
            return noteUri;
        }
        throw new SQLException("Failed to insert row into " + uri);
    }
    
    public int delete(Uri uri, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        int count;
        switch (sMatcher.match(uri))
        {
            case DBService.ITEM:
                count = db.delete(DBService.TABLE_SERVICE_NAME, selection, selectionArgs);
                break;
            case DBService.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count = db.delete(DBService.TABLE_SERVICE_NAME,
                        DBService.CHANNEL_NUMBER + "=" + id + (!TextUtils.isEmpty(selection) ? " AND (" + selection + ")":""),
                        selectionArgs);
                break;
            default:
                throw new IllegalArgumentException("Failed to delete row into:" + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }
    
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        int count;
        switch (sMatcher.match(uri))
        {
            case DBService.ITEM:
                count = db.update(DBService.TABLE_SERVICE_NAME, values, selection, selectionArgs);
                break;
            case DBService.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count =db.update(DBService.TABLE_SERVICE_NAME,
                        values,
                        DBService.CHANNEL_NUMBER + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")":""),
                        selectionArgs);
                break;
            default:
                throw new IllegalArgumentException("Failed to update row into:" + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }
    
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        Cursor cursor;
        switch (sMatcher.match(uri))
        {
            case DBService.ITEM:
                cursor = db.query(DBService.TABLE_SERVICE_NAME,
                    projection,
                    selection, 
                    selectionArgs, 
                    null, 
                    null,
                    sortOrder);
                break;
            case DBService.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                cursor = db.query(DBService.TABLE_SERVICE_NAME, 
                    projection, 
                    DBService.CHANNEL_NUMBER +"="+ id + (!TextUtils.isEmpty(selection) ? "AND("+ selection + ")":""), 
                    selectionArgs, 
                    null, 
                    null, 
                    sortOrder);
                break;
            default:
                throw new IllegalArgumentException("Failed to query row to "+uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return cursor;
    }
    
    /**
     * 重载方法
     * @param operations
     * @return
     * @throws OperationApplicationException
     */
    public ContentProviderResult[] applyBatch(ArrayList<ContentProviderOperation> operations)
        throws OperationApplicationException
    {
        SQLiteDatabase db = dbOpenHelper.getWritableDatabase();
        db.beginTransaction();
        try
        {
            ContentProviderResult[] results = super.applyBatch(operations);
            db.setTransactionSuccessful();
            return results;
        }
        finally
        {
            db.endTransaction();
        }
    }
    
}
