package com.pbi.dvb.nppvware.Player;

import java.io.IOException;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;
import android.view.SurfaceHolder;

import com.pbi.dvb.nppvware.NpPvwareActivity;

public class MediaPlayerCore implements MediaPlayer.OnBufferingUpdateListener, MediaPlayer.OnPreparedListener,
    MediaPlayer.OnCompletionListener, MediaPlayer.OnErrorListener
{
    
    SurfaceHolder mHolder = null;
    
    private MediaPlayer mPlayer = null;
    
    public MediaPlayerCore(SurfaceHolder holder)
    {
        mHolder = holder;
    }
    
    public void onPrepared(MediaPlayer mp)
    {
        // TODO Auto-generated method stub
        // int iWidth = mPlayer.getVideoWidth();
        // int iHeight = mPlayer.getVideoHeight();
        //
        // if(iWidth != 0 && iHeight != 0)
        // mHolder.setFixedSize(iWidth, iHeight);
        
        Log.d("pxwang_debug", "onPrepared");
        mPlayer.start();
        NpPvwareActivity.Event_PlayPrepared();
    }
    
    public MediaPlayer getPlayer()
    {
        return mPlayer;
    }
    
    @Override
    public void onCompletion(MediaPlayer mp)
    {
        // TODO Auto-generated method stub
        if (mPlayer != null)
        {
            Log.d("pxwang_debug", "onCompletion			");
            mPlayer.release();
            mPlayer = null;
        }
        NpPvwareActivity.Event_PlayComplete();
    }
    
    public boolean onError(MediaPlayer mp, int x, int y)
    {
        
        // mPlayer.stop();
        try
        {
            mPlayer.reset();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        NpPvwareActivity.Event_PlayError();
        Log.d("pxwang_debug", "onError         ");
        return false;
    }
    
    public void onBufferingUpdate(MediaPlayer mp, int percent)
    {
        // TODO Auto-generated method stub
        Log.d("pxwang_debug", "onBufferingUpdate         ");
    }
    
    public synchronized void onSurfaceChanged(SurfaceHolder holder)
    {
        mHolder = holder;
    }
    
    
    public synchronized void playVideo(SurfaceHolder holder, String szUrl)
    {
        
        if (mPlayer == null)
        {
            Log.d("pxwang_debug", "playVideo			mPlayer === NULL");
            
            mPlayer = new MediaPlayer();
            mPlayer.setOnBufferingUpdateListener(this);
            mPlayer.setOnPreparedListener(this);
            mPlayer.setOnCompletionListener(this);
            mPlayer.setOnErrorListener(this);
            mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
        }
        
        if (mPlayer != null)
        {
            try
            {
                if (mPlayer.isPlaying())
                    mPlayer.reset();
                
                mPlayer.setDataSource(szUrl);
                mPlayer.setDisplay(holder);
                mPlayer.prepareAsync();
                
            }
            catch (IllegalArgumentException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            catch (IllegalStateException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            catch (IOException e)
            {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }
    
    public synchronized void release()
    {
        if (mPlayer != null)
        {
            Log.d("pxwang_debug", "release			");
            mPlayer.release();
            mPlayer = null;
        }
    }
    
    public synchronized void stopVideo()
    {
        if (mPlayer != null)
        {
            Log.d("pxwang_debug", "stopVideo	111		");
            // mPlayer.stop();
            mPlayer.release();
            Log.d("pxwang_debug", "release			");
            mPlayer = null;
        }
    }
    
    public synchronized void pauseVideo()
    {
        if (mPlayer != null)
        {
            mPlayer.pause();
        }
    }
    
    public synchronized void resumevideo()
    {
        if (mPlayer != null)
        {
            mPlayer.start();
        }
    }
    
    public synchronized void seekto(String milsec)
    {
        int val = Integer.valueOf(milsec).intValue();
        if (mPlayer != null)
        {
            mPlayer.seekTo(val);
        }
    }
    
    public synchronized boolean isplaying()
    {
        boolean play_flag = false;
        if (mPlayer != null)
        {
            play_flag = mPlayer.isPlaying();
        }
        return play_flag;
    }
    
    public synchronized int getvideowidth()
    {
        int width = 0;
        if (mPlayer != null)
        {
            width = mPlayer.getVideoWidth();
        }
        return width;
    }
    
    public synchronized int getvideoheight()
    {
        int height = 0;
        if (mPlayer != null)
        {
            height = mPlayer.getVideoHeight();
        }
        return height;
    }
    
    public synchronized int getcurrentposition()
    {
        int current = 0;
        if (mPlayer != null)
        {
            current = mPlayer.getCurrentPosition();
        }
        return current;
    }
    
    public synchronized int getduration()
    {
        int total = 0;
        if (mPlayer != null)
        {
            total = mPlayer.getDuration();
        }
        return total;
    }
    
    public synchronized void setlooping(String loop)
    {
        int val = Integer.valueOf(loop).intValue();
        boolean set_val = false;
        if (val != 0)
        {
            set_val = true;
        }
        if (mPlayer != null)
        {
            mPlayer.setLooping(set_val);
        }
    }
    
    public synchronized boolean islooping()
    {
        boolean llop_flag = false;
        if (mPlayer != null)
        {
            llop_flag = mPlayer.isLooping();
        }
        return llop_flag;
    }
    
    public synchronized void setvolume(String volume)
    {
        int val = Integer.valueOf(volume).intValue();
        if (mPlayer != null)
        {
            mPlayer.setVolume(val, val);
        }
    }
    
}
