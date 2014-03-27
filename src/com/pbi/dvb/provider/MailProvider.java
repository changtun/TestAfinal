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

import com.pbi.dvb.db.DBMail;
import com.pbi.dvb.db.DBopenHelper;

public class MailProvider extends ContentProvider
{
    private DBopenHelper dBOpenHelper;
    private static final UriMatcher mMatcher;
    
    static 
    {
        mMatcher = new UriMatcher(UriMatcher.NO_MATCH);
        mMatcher.addURI(DBMail.MAIL_AUTHORITY, DBMail.TABLE_MAIL_NAME, DBMail.ITEM);
        mMatcher.addURI(DBMail.MAIL_AUTHORITY, DBMail.TABLE_MAIL_NAME+"/#", DBMail.ITEM_ID);
    }
    
    public boolean onCreate()
    {
        dBOpenHelper = new DBopenHelper(this.getContext());
        return true;
    }
    
    public String getType(Uri uri)
    {
        switch (mMatcher.match(uri))
        {
            case DBMail.ITEM:
                return DBMail.CONTENT_TYPE;
            case DBMail.ITEM_ID:
                return DBMail.CONTENT_ITEM_TYPE;
            default:
                throw new IllegalArgumentException("Unknown URI: " + uri);
        }
    }
    
    public Uri insert(Uri uri, ContentValues values)
    {
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        long rowId;
        if (mMatcher.match(uri) != DBMail.ITEM)
        {
            throw new IllegalArgumentException("Unknown URI:"+uri);
        }
        rowId = db.insert(DBMail.TABLE_MAIL_NAME, DBMail.MAIL_ID, values);
        if (rowId > 0)
        {
            Uri noteUri = ContentUris.withAppendedId(DBMail.CONTENT_URI, rowId);
            getContext().getContentResolver().notifyChange(noteUri, null);
            return noteUri;
        }
        throw new SQLException("Fail to insert row into "+uri);
    }
    
    public int delete(Uri uri, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        int count;
        switch (mMatcher.match(uri))
        {
            case DBMail.ITEM:
                count = db.delete(DBMail.TABLE_MAIL_NAME, selection, selectionArgs);
                break;
            case DBMail.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count =
                    db.delete(DBMail.TABLE_MAIL_NAME, DBMail.MAIL_ID + "=" + id
                        + (!TextUtils.isEmpty(selection) ? "AND (" + selection + ")" : ""), selectionArgs);
                break;
            
            default:
                throw new SQLException("Fail to delete row into + " + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }

    public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder)
    {
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        Cursor cursor;
        switch (mMatcher.match(uri))
        {
            case DBMail.ITEM:
                cursor = db.query(DBMail.TABLE_MAIL_NAME, projection, selection, selectionArgs, null, null, sortOrder);
                break;
            case DBMail.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                cursor = db.query(DBMail.TABLE_MAIL_NAME, projection, DBMail.MAIL_ID +"="+id +(!TextUtils.isEmpty(selection) ? "AND(" + selection +")" : ""), selectionArgs, null, null, sortOrder);
                break;
            default:
                throw new IllegalArgumentException("Fail to query row into " + uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return cursor;
    }
    
    public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs)
    {
        SQLiteDatabase db = dBOpenHelper.getReadableDatabase();
        int count;
        switch (mMatcher.match(uri))
        {
            case DBMail.ITEM:
                count = db.update(DBMail.TABLE_MAIL_NAME, values, selection, selectionArgs);
                break;
            case DBMail.ITEM_ID:
                String id = uri.getPathSegments().get(1);
                count =db.update(DBMail.TABLE_MAIL_NAME,
                        values,
                        DBMail.MAIL_ID + "=" + id + (!TextUtils.isEmpty(selection) ? "AND(" + selection + ")":""),
                        selectionArgs);
                break;
            default:
                throw new IllegalArgumentException("Fail to update row into "+uri);
        }
        getContext().getContentResolver().notifyChange(uri, null);
        return count;
    }
    
}
























