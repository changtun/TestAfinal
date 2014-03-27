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

import com.pbi.dvb.db.DBTp;
import com.pbi.dvb.db.DBopenHelper;

/**
 * 
 * <TP information Provider>
 * 
 * @author  gtsong
 * @version  [版本号, 2012-7-19]
 * @see  [相关类/方法]
 * @since  [产品/模块版本]
 */
public class TPProvider extends ContentProvider
{
    private DBopenHelper dbOpenHelper;
    
    public static final UriMatcher tMatcher;
    
    static
    {
        tMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        tMatcher.addURI(DBTp.TP_AUTHORITY, DBTp.TABLE_TP_NAME, DBTp.ITEM);
        tMatcher.addURI(DBTp.TP_AUTHORITY, DBTp.TABLE_TP_NAME + "/#", DBTp.ITEM_ID);
    }
    
    public boolean onCreate()
    {
        dbOpenHelper = new DBopenHelper(this.getContext());
        return true;
    }
    
    public String getType(Uri uri)
    {
        switch (tMatcher.match(uri))
        {
            case DBTp.ITEM:
                return DBTp.CONTENT_TYPE;
            case DBTp.ITEM_ID:
                return DBTp.CONTENT_ITEM_TYPE;
            default:
                throw new IllegalArgumentException("Unknown URI:" + uri);
        }
    }
    
    public Uri insert(Uri uri, ContentValues values)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        long rowId;
        if (tMatcher.match(uri) != DBTp.ITEM)
        {
            throw new IllegalArgumentException("UnKnown URI" + uri);
        }
        rowId = db.insert(DBTp.TABLE_TP_NAME, DBTp.TP_ID, values);
        if (rowId > 0)
        {
            Uri noteUri = ContentUris.withAppendedId(DBTp.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(noteUri, null);
            return noteUri;
        }
        throw new SQLException("Failed to insert row to uri:" + uri);
    }
    
    public int delete(Uri uri, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        int count;
        switch (tMatcher.match(uri))
        {
            case DBTp.ITEM:
                count = db.delete(DBTp.TABLE_TP_NAME, selection, selectionArgs);
                break;
            case DBTp.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count =db.delete(DBTp.TABLE_TP_NAME, 
                    DBTp.TP_ID + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")" : ""), 
                    selectionArgs);
                break;
            default:
                throw new IllegalArgumentException("Fail to delete to uri :" + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }
    
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        int count;
        switch (tMatcher.match(uri))
        {
            case DBTp.ITEM:
                count = db.update(DBTp.TABLE_TP_NAME, values, selection, selectionArgs);
                break;
            case DBTp.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count =db.update(DBTp.TABLE_TP_NAME, 
                    values, 
                    DBTp.TP_ID + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")" : ""), 
                    selectionArgs);
                break;
            default:
                throw new IllegalArgumentException("Failed to update to uri :" + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }
    
    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder)
    {
        SQLiteDatabase db = dbOpenHelper.getReadableDatabase();
        Cursor cursor;
        switch (tMatcher.match(uri))
        {
            case DBTp.ITEM:
                cursor = db.query(DBTp.TABLE_TP_NAME, projection, selection, selectionArgs, null, null, sortOrder);
                break;
            case DBTp.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                cursor =db.query(DBTp.TABLE_TP_NAME,
                        projection,
                        DBTp.TP_ID + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")" : ""),
                        selectionArgs,
                        null,
                        null,
                        sortOrder);
                break;
            default:
                throw new IllegalArgumentException("Failed to query to uri:" + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return cursor;
    }
    
}
