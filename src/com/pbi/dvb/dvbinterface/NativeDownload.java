package com.pbi.dvb.dvbinterface;

//import android.net.Uri;
//import android.content.Context;	
//import android.content.ContentResolver; 


public class NativeDownload
{
	//private static final String TAG = "NativeDownload";
	public native int downloadMessageInit(MessageDownload msg);
	public native int downloadMessageUnInit();
	public native int downloadStart( );
	public native int downloadGetStatus( );
}


