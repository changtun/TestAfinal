package com.pbi.dvb.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.pbi.dvb.global.Config;

/**
 * 需要在布局文件中添加 android:ellipsize="marquee" android:singleLine="true"； 在布局文件引用本view时,paddingLeft,paddingRigh都必须为0;
 * 
 * @author
 * @version [版本号, 2013-5-30]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class CustomMarqueeText extends TextView implements Runnable
{
    private static final String TAG = "CustomMarqueeText";
    
    private int startPosition;
    
    private int currentScrollX;// 当前滚动的位置
    
    private boolean isStop = false;
    
    private int textWidth;
    
    private boolean isMeasure = false; // 获取文字宽度的标志位
    
    private int measureTime = 8;
    
    private int speed = Config.CA_OSD_LOW_SPEED;// 文字的滚动速度
    
    private int repeatTime; // 设置滚动次数
    
    public CustomMarqueeText(Context context)
    {
        super(context);
    }
    
    public CustomMarqueeText(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }
    
    public CustomMarqueeText(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }
    
    @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        // if (!isMeasure)
        if (measureTime > 0)
        {
            // 文字宽度只需获取一次就可以了
            getTextWidth();
            // isMeasure = true;
            
            measureTime--;
            Log.e(TAG, "--->>> text width  ::  " + textWidth);
        }
    }
    
    /**
     * 获取文字宽度,得到文字长度的像素值
     */
    private void getTextWidth()
    {
        Paint paint = this.getPaint();
        String str = this.getText().toString();
        textWidth = (int)paint.measureText(str);
    }
    
    @Override
    public void run()
    {
        // Log.i(TAG, "--->>> current scroll position ::  " +currentScrollX);
        scrollTo(currentScrollX, 0);
        currentScrollX += speed;// 滚动速度
        
        if (isStop)
        {
            return;
        }
        // if (getScrollX() <= -(this.getWidth()))
        // Log.i(TAG, "--->>> current scroll position ::  " + getScrollX());
        // Log.i(TAG, "--->>> scroll distance ::  " + textWidth);
        if (getScrollX() >= textWidth)// 从最右边滚出
        {
            scrollTo(currentScrollX, 0);
            currentScrollX = -startPosition; // 从最左边滚入
            
            repeatTime--;
            
        }
        
        if (repeatTime <= 0)
        {
            stopScroll();
        }
        
        this.removeCallbacks(this);
        postDelayed(this, 10);
    }
    
    // 开始滚动
    public void startScroll()
    {
        isStop = false;
        this.removeCallbacks(this);
        post(this);
    }
    
    // 停止滚动
    private void stopScroll()
    {
        isStop = true;
        this.setVisibility(View.INVISIBLE);
    }
    
    // 从头开始滚动
    private void startFor0()
    {
        currentScrollX = -startPosition;
        startScroll();
    }
    
    public void setStartPosition(int startPosition)
    {
        this.startPosition = startPosition;
        
        // 初始化当前滚动位置,取初始位置的相反数，实现文字从右侧滚入的效果
        this.currentScrollX = -startPosition;
    }
    
    public void setSpeed(int speed)
    {
        this.speed = speed;
    }
    
    public void setRepeatTime(int repeatTime)
    {
        this.repeatTime = repeatTime;
    }
    
    public int getRepeatTime()
    {
        return repeatTime;
    }
    
    @Override
    public boolean isFocused()
    {
        return true;
    }
    
    @Override
    protected void onFocusChanged(boolean focused, int direction, Rect previouslyFocusedRect)
    {
        if (focused)
        {
            super.onFocusChanged(focused, direction, previouslyFocusedRect);
        }
    }
    
    @Override
    public void onWindowFocusChanged(boolean focused)
    {
        if (focused)
        {
            super.onWindowFocusChanged(focused);
        }
    }
    
}
