/*
 * 文 件 名:  ItemAdapter.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd.Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  
 * 修改时间:  2013-6-28
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.adapter;

import java.util.ArrayList;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.PvrFileBean;

/**
 * <一句话功能简述> <功能详细描述>
 * 
 * @author 姓名 工号
 * @version [版本号, 2013-6-28]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class PvrPlayAdapter extends BaseAdapter
{
    private ArrayList<PvrFileBean> list;
    
    private LayoutInflater inflater;
    
    public PvrPlayAdapter(Context context)
    {
        super();
        this.inflater = LayoutInflater.from(context);
    }
    
    public void setList(ArrayList<PvrFileBean> list)
    {
        this.list = list;
    }
    
    /**
     * 重载方法
     * 
     * @return
     */
    @Override
    public int getCount()
    {
        return list.size();
    }
    
    /**
     * 重载方法
     * 
     * @param position
     * @return
     */
    @Override
    public Object getItem(int position)
    {
        if(null != list)
        {
            return list.get(position);
        }
        return null;
    }
    
    /**
     * 重载方法
     * 
     * @param position
     * @return
     */
    @Override
    public long getItemId(int position)
    {
        return position;
    }
    
    /**
     * 重载方法
     * 
     * @param position
     * @param convertView
     * @param parent
     * @return
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {
        View view = null;
        ViewHolder holder = null;
        
        if (null == convertView)
        {
            view = inflater.inflate(R.layout.pvr_play_item, null);
            holder = new ViewHolder();
            holder.tvNum = (TextView)view.findViewById(R.id.pvr_play_num);
            holder.tvFileName = (TextView)view.findViewById(R.id.pvr_play_name);
            view.setTag(holder);
        }
        else
        {
            view = convertView;
            holder = (ViewHolder)view.getTag();
        }
        
        if(null != list)
        {
            holder.tvNum.setText(list.get(position).getNumber() + "");
            holder.tvFileName.setText(list.get(position).getName());
        }
        
        return view;
    }
    
}

class ViewHolder
{
    TextView tvNum;
    
    TextView tvFileName;
}
