package com.pbi.dvb.aidl;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;

interface UserBehaviorSurveyAIDL {

	void pushBehavior(String project, String action, int type, boolean unInterruptableFlag, inout String[] tags);
	
	void pushBehaviorNew(int type, boolean unInterruptableFlag, inout String[] tags);
	
    PiwikTrackerBehaviorBean generateActionOne() ;
}
