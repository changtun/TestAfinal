package com.pbi.dvb.pvrinterface.impl;

import android.content.Context;

import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.pvrinterface.PvrSetTaskCallback;

public class TaskCompleteCallBack extends PvrSetTaskCallback {

	private PvrDao pvrDao = null;

	public TaskCompleteCallBack(Context context) {

		pvrDao = new PvrDaoImpl(context);

	}

	@Override
	public PvrBean get_First_Record_Task() {

		return pvrDao.get_First_Record_Task((Integer[]) null);

	}

}