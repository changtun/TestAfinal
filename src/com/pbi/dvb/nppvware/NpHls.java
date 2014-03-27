package com.pbi.dvb.nppvware;

import android.webkit.WebView;
import android.util.Log;


public class NpHls
{
	protected static final String TAG = "NpHls";
	private WebView mWebView;
    public NpHls(WebView wv)
    {
    	mWebView = wv;
		mWebView.addJavascriptInterface(this, "Hls");
		Log.d(TAG, "New NpHls");
		HlsRegisterCallback(this);
    }
	private void HlsCallBackToJs(int type)
	{
		Log.d(TAG, "javascript:HlsCallBack(" + type +  " )");
		mWebView.loadUrl("javascript:HlsCallBack(" + type + " )");
	}
	public native int HlsRegisterCallback(Object obj);
}
