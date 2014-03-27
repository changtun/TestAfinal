package com.pbi.dvb.nppvware;

import android.util.Log;
import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebView;

public class NpPvwareChromeClient extends WebChromeClient
{
    public boolean onJsAlert(WebView view, String url, String message, JsResult result)
    {
        Log.d("pvcloud", "donwgei: onJsAlert=" + message);
        return false;
    }
}
