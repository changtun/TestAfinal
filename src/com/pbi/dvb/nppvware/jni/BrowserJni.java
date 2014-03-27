package com.pbi.dvb.nppvware.jni;

public class BrowserJni 
{
	static
	{
//		System.loadLibrary("dvbnppv");
	}

	//	public native int plus (int x, int y);
	public native void surfaceCreated(int npp, Object surface);
	public native void surfaceChanged(int npp, int format, int width, int height);
	public native void surfaceDestroyed(int npp);
	
	public BrowserJni()
	{
		
	}
}
