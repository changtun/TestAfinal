/*
 * �� �� ��:  CustomToast.java
 * ��    Ȩ:  PBI Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��:  
 * �޸�ʱ��:  2012-12-21
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
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
 * <һ�仰���ܼ���> <������ϸ����>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-12-21]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public class CustomToast
{
    private Context context;
    
    /**
     * <Ĭ�Ϲ��캯��>
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
     * @see [�ࡢ��#��������#��Ա]
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
