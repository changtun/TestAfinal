package com.pbi.dvb.pvrinterface.impl;

import java.util.Calendar;
import java.util.Date;

import android.os.Handler;
import android.util.Log;

import com.pbi.dvb.service.PvrBackgroundService;

public class PvrTimerTaskUtil extends Thread {

	private static final String TAG = "PvrTimerTaskUtil";

	private long milli_Delay = 0;

	private boolean runFlag = true;

	private long startTime = 0;

	private Handler handler = null;

	private int msg_Type;

	public PvrTimerTaskUtil(Handler handler, long milli_Delay, int msg_Type) {

		this.milli_Delay = milli_Delay;

		this.handler = handler;

		this.msg_Type = msg_Type;
	}

	@Override
	public void run() {

		startTime = Calendar.getInstance().getTimeInMillis();

		if (runFlag) {

			try {

				Log.e(TAG, "before running task, the delayStart = "
						+ new Date().toLocaleString());
				Thread.sleep(milli_Delay);
				Log.e(TAG,
						"after running task, the delayStart = "
								+ new Date().toLocaleString());

			} catch (InterruptedException e) {

				e.printStackTrace();

			}
		}

		Log.e(TAG, "Total delay time = "
				+ (Calendar.getInstance().getTimeInMillis() - startTime)
				+ " milli_Delay = " + milli_Delay);

		if (runFlag) {

			// handler.sendEmptyMessage(PvrBackgroundService.RECORD_FINISHED);
			handler.sendEmptyMessage(msg_Type);

			runFlag = false;

		} else {

			Log.e(TAG, "runFlag = false");

		}
	}

	public boolean isRunFlag() {
		return runFlag;
	}

	public void setRunFlag(boolean runFlag) {
		this.runFlag = runFlag;
	}

	public void stopTask() {

		setRunFlag(false);

	}
}