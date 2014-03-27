package com.pbi.dvb.aidl;

interface OTADownloaderServiceAIDL {

	boolean try_Lock_OTA_IP() ;
	boolean try_Lock_OTA(int lockMask) ;
	boolean unlock_OTA_IP() ;
	boolean unlock_OTA(int lockMask) ;

}
