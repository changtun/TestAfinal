package com.pbi.dvb.dvbinterface;
import com.pbi.dvb.dvbinterface.MsgTimeUpdate;

public class Nativetime
{	
	public native int TimeUpdateInit(MsgTimeUpdate msg);

	public native int TimeMessageUnint();
}

