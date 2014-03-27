package com.pbi.dvb.aidl;

import com.pbi.dvb.domain.PiwikTrackerBehaviorBean;

interface PiwikUserBehaviorSurveyAIDL {

	void pushBehavior(int type, boolean unInterruptableFlag, inout String[] tags);

    PiwikTrackerBehaviorBean generateActionOne() ;
}
