package com.pbi.dvb.dvbinterface;

import android.net.Uri;
import android.content.Context;	
import android.content.ContentResolver; 


public class NativeSystemInfo
{
	private String SoftWareID;
	
	native public void SoftWareInit(String software);

	public void SystemInfoInit(Context context)
	{
		this.setSoftWareID(context);
	}

	private void setSoftWareID(Context context)
    {	
    	
		
    	ContentResolver cr = context.getContentResolver(); 
		SoftWareID = cr.getType(Uri.parse("content://com.pbi.pbiprovider/pbi/1"));
		System.out.println("SoftWareID:"+SoftWareID);
		SoftWareInit(SoftWareID);
    }


}

