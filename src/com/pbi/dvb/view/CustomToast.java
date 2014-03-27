/*
 * 文 件 名:  CustomToast.java
 * 版    权:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  
 * 修改时间:  2012-12-21
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.view;

import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.R;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author 姓名 工号
 * @version [版本号, 2012-12-21]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class CustomToast
{
    private Context context;
    
    /**
     * <默认构造函数>
     */
    public CustomToast(Context context)
    {
        super();
        this.context = context;
    }
    
    /**
     * 
     * <show custom toast. time 1 Toast.LONG 0 Toast.SHORT>
     * @param content
     * @param time
     * @see [类、类#方法、类#成员]
     */
    public void show(String content, int time)
    {
        LayoutInflater inflater = LayoutInflater.from(context);
        View view = inflater.inflate(R.layout.custom_toast, null);
        
        TextView tvContent = (TextView)view.findViewById(R.id.tv_toast);
        tvContent.setText(content);
        
        Toast toast = new Toast(context);
        toast.setGravity(Gravity.CENTER, 0, 0);
        
        if (time == 0)
        {
            toast.setDuration(Toast.LENGTH_SHORT);
        }
        else
        {
            toast.setDuration(Toast.LENGTH_SHORT);
        }
        toast.setView(view);
        toast.show();
    }
}
