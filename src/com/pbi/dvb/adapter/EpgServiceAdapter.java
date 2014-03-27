package com.pbi.dvb.adapter;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.ServiceInfoBean;

public class EpgServiceAdapter extends BaseAdapter
{
    
    private LayoutInflater inflater;
    
    private List<ServiceInfoBean> tvProgramList;
    
    private int index;// 选中Item的位置
    
    public EpgServiceAdapter(Context context)
    {
        this.inflater = LayoutInflater.from(context);
    }
    
    public EpgServiceAdapter(Context context, List<ServiceInfoBean> tvProgramList)
    {
        this.inflater = LayoutInflater.from(context);
        this.tvProgramList = tvProgramList;
    }
    
    public void setTvProgramList(List<ServiceInfoBean> tvProgramList)
    {
        this.tvProgramList = tvProgramList;
    }
    
    public void setIndex(int index)
    {
        this.index = index;
    }
    
    public int getCount()
    {
        return tvProgramList.size();
    }
    
    public Object getItem(int position)
    {
        return tvProgramList.get(position);
    }
    
    public long getItemId(int position)
    {
        return position;
    }
    
    public View getView(int position, View convertView, ViewGroup parent)
    {
        View view = null;
        ViewHolder holder = null;
        if (null == convertView)
        {
            view = inflater.inflate(R.layout.epg_service_item, null);
            holder = new ViewHolder();
            holder.epg_service = (TextView)view.findViewById(R.id.epg_service);
            view.setTag(holder);
        }
        else
        {
            view = convertView;
            holder = (ViewHolder)view.getTag();
        }
        
        holder.epg_service.setText(tvProgramList.get(position).getLogicalNumber() + " "
            + tvProgramList.get(position).getChannelName());
        return view;
    }
    
    class ViewHolder
    {
        TextView epg_service;
    }
}
