package com.pbi.dvb.factorytest;

import com.pbi.dvb.R;

import android.app.Activity;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class AvPlayer extends Activity
{
    private MediaPlayer player;
    
    private SurfaceView svFace;
    
    private String url;
    
    private SurfaceHolder holder;
    
    private int mVideoWidth;
    
    private int mVideoHeight;
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.player);
        Bundle data = this.getIntent().getExtras();
        url = data.getString("url");
        if (url == null && "".equals(url))
        {
            finish();
        }
        player = new MediaPlayer();
        svFace = (SurfaceView)findViewById(R.id.vodView);
        holder = svFace.getHolder();
        holder.setType(svFace.getHolder().SURFACE_TYPE_PUSH_BUFFERS);
        holder.addCallback(new SurfaceHolder.Callback()
        {
            public void surfaceDestroyed(SurfaceHolder holder)
            {
                if (player != null)
                {
                    player.release();
                }
            }
            
            public void surfaceCreated(SurfaceHolder holder)
            {
                prepare();
            }
            
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
            {
                // createDialog();
            }
        });
    }
    
    public void prepare()
    {
        try
        {
            player.reset();
            player.setAudioStreamType(AudioManager.STREAM_MUSIC);
            player.setDisplay(svFace.getHolder());
            player.setDataSource(url);
            player.prepareAsync();
            player.setOnPreparedListener(new MediaPlayer.OnPreparedListener()
            {
                public void onPrepared(MediaPlayer mp)
                {
                    
                    int maxProgress = player.getDuration();
                    
                    maxProgress /= 1000;
                    int minute = maxProgress / 60;
                    int hour = minute / 60;
                    int second = maxProgress % 60;
                    minute %= 60;
                    
                    Log.d("AvPlayer", "setOnPreparedListener");
                    play();
                }
            });
            
            player.setOnCompletionListener(new MediaPlayer.OnCompletionListener()
            {
                public void onCompletion(MediaPlayer mp)
                {
                    if (player != null)
                    {
                        // player.stop();
                        player.release();
                    }
                    Log.d("AvPlayer", "setOnCompletionListener");
                    finish();
                }
            });
            
            player.setOnBufferingUpdateListener(new MediaPlayer.OnBufferingUpdateListener()
            {
                public void onBufferingUpdate(MediaPlayer mp, int percent)
                {
                    
                }
            });
            
            player.setOnErrorListener(new MediaPlayer.OnErrorListener()
            {
                public boolean onError(MediaPlayer mp, int what, int extra)
                {
                    if (player != null)
                    {
                        player.release();
                        player = null;
                        
                    }
                    Log.d("AvPlayer", "setOnErrorListener");
                    finish();
                    return true;
                }
            });
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
    
    @Override
    protected void onStop()
    {
        // TODO Auto-generated method stub
        super.onStop();
        if (player != null)
        {
            player.release();
            player =null;
        }
    }
    
    public void play()
    {
        try
        {
            mVideoWidth = player.getVideoWidth();
            mVideoHeight = player.getVideoHeight();
            if (mVideoWidth != 0 && mVideoHeight != 0)
            {
                holder.setFixedSize(mVideoWidth, mVideoHeight);
            }
            player.start();
            // player.stop();
            // this.finish();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
