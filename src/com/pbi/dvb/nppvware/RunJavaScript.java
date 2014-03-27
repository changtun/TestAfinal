package com.pbi.dvb.nppvware;

import android.os.Handler;
import android.util.Log;

final public class RunJavaScript 
{
	private NpPvwareActivity mNotify;
	private Handler mHandler;
	
	public RunJavaScript(NpPvwareActivity notify)
	{
		mNotify = notify;
		mHandler = new Handler();
	}

	public void play(final String szTexts)
	{		
		mHandler.post(new Runnable()
		{
			public void run() 
			{				
				mNotify.playVideoCommandFormJS(szTexts);
			}
		});
	}

	public void stop()
	{
		
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.stopVideoCommandFormJS();
			}
		});
	}

	public void pause()
	{
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.pauseVideoCommandFormJS();
			}
		});
	}

	public void resume()
	{
		
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.resumeVideoCommandFormJS();
			}
		});
	}

	public void seekto(final int milsec)
	{		
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.seektoVideoCommandFormJS(milsec);
			}
		});
	}

	public boolean isplaying()
	{
		return mNotify.isplayingVideoCommandFormJS();
	}

	public int getvideowidth()
	{		
		int wid = 800;
		wid = mNotify.getvideowidthVideoCommandFormJS();
		return wid;
	}

	public int getvideoheight()
	{		
		return mNotify.getvideoheightVideoCommandFormJS();
	}

	public int getcurrentposition()
	{
		return mNotify.getcurrentpositionVideoCommandFormJS();
	}

	public int getduration()
	{
		return mNotify.getdurationVideoCommandFormJS();
	}

//OK
	public void setlooping(final int loop_flag)
	{		
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.setloopingVideoCommandFormJS(loop_flag);
			}
		});
	}

	public boolean islooping()
	{
		return mNotify.isloopingVideoCommandFormJS();
	}

	public void setvolume(final int vol_val)
	{
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				mNotify.setvolumeVideoCommandFormJS(vol_val);
			}
		});
	}


	
	
	public void runOnAndroidJavaScriptOtherAPK(final String[] szTexts)
	{
		mHandler.post(new Runnable()
		{
			public void run() 
			{
				//mNotify.callAtherAPK();
			}
		});
	}
}
