/*
 * Copyright 2009, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package com.pbi.dvb.nppvware.plugins;

import com.pbi.dvb.nppvware.NpPvwareActivity;
import com.pbi.dvb.nppvware.Player.MediaPlayerCore;

import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;

public class PaintSurface extends SurfaceView {

    static {
        //needed for jni calls
        //System.loadLibrary("nppvwareplugin");
    }

    private final int npp;

    private boolean validNPP = true;
    private Object nppLock = new Object();
    public static SurfaceHolder mHolder = null;

//    private PaintThread		mThread = null;
    private MediaPlayerCore	mPlayer = null;
    private Handler mHandleController = null; 
    
    public static final int g_MSG_PlayerCtrl_Play = 0;	
	public static final int g_MSG_PlayerCtrl_Stop = 1;
    public static final int g_MSG_PlayerCtrl_Release = 2;
    public static final int g_MSG_PlayerCtrl_Pause = 3;
	public static final int g_MSG_PlayerCtrl_Resume = 4;
	public static final int g_MSG_PlayerCtrl_Seekto = 5;
	public static final int g_MSG_PlayerCtrl_IsPlaying = 6;
    public static final int g_MSG_PlayerCtrl_GetVideoWidth = 7;
	public static final int g_MSG_PlayerCtrl_GetVideoHeight = 8;
	public static final int g_MSG_PlayerCtrl_GetCurrentPosition = 9;
	public static final int g_MSG_PlayerCtrl_GetDuration = 10;
	public static final int g_MSG_PlayerCtrl_SetLooping = 11;
	public static final int g_MSG_PlayerCtrl_IsLooping = 12;
	public static final int g_MSG_PlayerCtrl_SetVolume = 13;

    public PaintSurface(Context context, int NPP, int width, int height) {
        super(context);

        this.npp = NPP;

        mPlayer = new MediaPlayerCore(getHolder());

        
        
this.getHolder().setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        this.getHolder().setFormat(257);//PixelFormat.VIDEO_HOLE
        //this.getHolder().setFormat(PixelFormat.RGBX_8888);

        this.getHolder().addCallback(new Callback() {
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                synchronized (nppLock) {
                    if (validNPP) {
                        //nativeSurfaceChanged(npp, format, width, height);
                    }
                }
                
                mHolder = holder;                
                mPlayer.onSurfaceChanged(holder);
            }

            private void initSurface(SurfaceHolder h)
            {
                Canvas c = null;
                try
                {
                    c = h.lockCanvas();
                }
                finally
                {
                    if (c != null)
                        h.unlockCanvasAndPost(c);
                }
            }

            public void surfaceCreated(SurfaceHolder holder) {
                synchronized (nppLock) {
                    if (validNPP) {
                        //nativeSurfaceCreated(npp);
                    }
                    
                    initSurface(holder);
                    mHolder = holder;
                    mHandleController = new Handler()
                    {
						public void handleMessage(Message msg) 
						{
							// TODO Auto-generated method stub
							switch(msg.what)
							{
							case g_MSG_PlayerCtrl_Play:
								{
						    		Log.d("dw_plugin", "***********recieve g_MSG_PlayerCtrl_Play***********");
				                    mPlayer.playVideo(mHolder, (String)msg.obj);							    		
								}
								break;								
							case g_MSG_PlayerCtrl_Stop:
								{
				                    mPlayer.stopVideo();							    		
								}
								break;	
							case g_MSG_PlayerCtrl_Release:
								{
									mPlayer.release();
								}
								break;
							case g_MSG_PlayerCtrl_Pause:
								{
									mPlayer.pauseVideo();
								}
								break;								
							case g_MSG_PlayerCtrl_Resume:
								{
									mPlayer.resumevideo();
								}
								break;
							case g_MSG_PlayerCtrl_Seekto:
								{
									mPlayer.seekto((String)msg.obj);
								}
								break;
							
							case g_MSG_PlayerCtrl_SetLooping:
								{
									mPlayer.setlooping((String)msg.obj);
								}
								break;
							
							case g_MSG_PlayerCtrl_SetVolume:
								{
									mPlayer.setvolume((String)msg.obj);
								}
								break;
							}
                }
                    };
                }
                NpPvwareActivity.setMediaController(mHandleController, mPlayer);
                Log.d("dongwei", "**************paint surface surfaceCreated****************");
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                synchronized (nppLock) {
                    if (validNPP) {
                        //nativeSurfaceDestroyed(npp);
                    }
                    
                    if(mHandleController != null)
                    {
                    	mHandleController = null;
                    }
                }
            }
        });

        // sets the plugin's surface to a fixed size
        this.getHolder().setFixedSize(width, height);

        // ensure that the view system is aware that we will be drawing
        //this.setWillNotDraw(false);
    }

    // called by JNI
    private void invalidateNPP() {
        synchronized (nppLock) {
            validNPP = false;
        }
    }

    //private native void nativeSurfaceCreated(int npp);
    //private native void nativeSurfaceChanged(int npp, int format, int width, int height);
    //private native void nativeSurfaceDestroyed(int npp);
}
