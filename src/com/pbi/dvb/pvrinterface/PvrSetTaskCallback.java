package com.pbi.dvb.pvrinterface;

import com.pbi.dvb.domain.PvrBean;

public abstract class PvrSetTaskCallback {

	protected Integer[] skipID = null;

	public abstract PvrBean get_First_Record_Task();

	public Integer[] getSkipID() {
		return skipID;
	}

}