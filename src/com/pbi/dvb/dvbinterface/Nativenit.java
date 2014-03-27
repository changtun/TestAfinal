package com.pbi.dvb.dvbinterface;
import com.pbi.dvb.dvbinterface.MessageNit;
import com.pbi.dvb.dvbinterface.MessageNitDownload;

public class Nativenit
{	
	public native int NitMessageInit(MessageNit msg);

	public native int NitMessageUnint();

	public native int DLMessageInit(MessageNitDownload msg);
	public native int DLMessageUnint();
}
