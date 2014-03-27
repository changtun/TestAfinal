package com.pbi.dvb.pvrinterface.impl;

import android.content.Context;
import android.util.Log;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.pvrinterface.PvrSetTaskCallback;
import com.pbi.dvb.utils.LogUtils;

public class SetNextTaskCallBack extends PvrSetTaskCallback {

	private static final String TAG = "SetNextTaskCallBack";

	private PvrDao pvrDao = null;

	public SetNextTaskCallBack(Context context, Integer... skipID) {

		pvrDao = new PvrDaoImpl(context);

		super.skipID = skipID;

	}

	@Override
	public PvrBean get_First_Record_Task() {

		PvrBean returnBean = null;

		if (pvrDao != null) {

			if (skipID == null) {

				Log.e(TAG,
						"SetNextTaskCallBack::get_First_Record_Task  the skip == null");

			} else {

				Log.e(TAG,
						"SetNextTaskCallBack::get_First_Record_Task  the skipID = "
								+ skipID);

			}

			returnBean = pvrDao.get_First_Record_Task(skipID);

		} else {

			LogUtils.e(
					TAG,
					"SetNextTaskCallBack::get_First_Record_Task  The pvrDao must not be null here!!!!!It's a runtime error..!!!");

		}

		return returnBean;
	}

}