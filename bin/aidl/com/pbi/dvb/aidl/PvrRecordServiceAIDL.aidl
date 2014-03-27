package com.pbi.dvb.aidl;

interface PvrRecordServiceAIDL {

	void startRecord(int pvrID);
	
	void stopRecord() ;

    int getCurrentTunerFreq() ;
    
    int getCurrentTunerSymbol() ;
    
    int getCurrentTunerQAM() ;
    
    int[] getTunerInfo() ;
}
