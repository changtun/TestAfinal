/**
 * Piwik - Open source web analytics
 * 
 * @license released under BSD License http://www.opensource.org/licenses/bsd-license.php
 * @link http://piwik.org/docs/tracking-api/
 *
 * @category Piwik
 * @package PiwikTracker
 */
package com.pbi.dvb.piwik_tracker.impl;

import javax.servlet.http.HttpServletRequest;

import com.pbi.dvb.piwik_tracker.BasePiwikTracker;
import com.pbi.dvb.piwik_tracker.PiwikException;

/**
 * Piwik - Open source web analytics
 * 
 * Client to record visits, page views, Goals, in a Piwik server. For more information, see
 * http://piwik.org/docs/tracking-api/
 * 
 * released under BSD License http://www.opensource.org/licenses/bsd-license.php
 * 
 * @link http://piwik.org/docs/tracking-api/
 * @author Martin Fochler, Klaus Pfeiffer, Bernhard Friedreich
 */
public class Discrete_Piwik_Tracker extends BasePiwikTracker
{
    
    /**
     * 
     * @param apiUrl
     * @throws PiwikException
     */
    public Discrete_Piwik_Tracker(final String apiUrl)
        throws PiwikException
    {
        super(apiUrl);
    }
    
    /**
     * Builds a PiwikTracker object, used to track visits, pages and Goal conversions for a specific website, by using
     * the Piwik Tracking API.
     * 
     * @param idSite Id of the site to be tracked
     * @param apiUrl points to URL of the tracker server
     * @param request
     * @throws PiwikException
     */
    public Discrete_Piwik_Tracker(final int idSite, final String apiUrl, final HttpServletRequest request)
        throws PiwikException
    {
        super(idSite, apiUrl, request);
    }
    
    @Override
    public boolean connectToServer()
    {
        boolean result = super.connectToServer();
        
        disconnectFromServer();
        
        return result;
    }
}
