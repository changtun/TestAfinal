package com.pbi.dvb.adapter;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.ServiceInfoBean;

public class PlayProgramAdapter extends BaseAdapter
{
    private LayoutInflater inflater;
    
    private List<ServiceInfoBean> tvProgramList;
    
    // 选中Item的位置
    private int index;
    
    public void setTvProgramList(List<ServiceInfoBean> tvProgramList)
    {
        this.tvProgramList = tvProgramList;
    }
    
    public void setIndex(int index)
    {
        this.index = index;
    }
    
    public PlayProgramAdapter(Context context)
    {
        this.inflater = LayoutInflater.from(context);
    }
    
    public PlayProgramAdapter(Context context, List<ServiceInfoBean> tvProgramList)
    {
        this.inflater = LayoutInflater.from(context);
        this.tvProgramList = tvProgramList;
    }
    
    public int getCount()
    {
        if (null != tvProgramList)
        {
            return tvProgramList.size();
        }
        return 0;
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
        View view = inflater.inflate(R.layout.item_play_programs, null);
        ViewHolder holder = new ViewHolder();
        
        holder.tvServiceId = (TextView)view.findViewById(R.id.tv_play_serviceid);
        holder.tvChannelNumber = (TextView)view.findViewById(R.id.tv_play_program_number);
        holder.tvChannelName = (TextView)view.findViewById(R.id.tv_play_program_name);
//        holder.ivCa = (ImageView)view.findViewById(R.id.iv_ca_tag);
        holder.ivFav = (ImageView)view.findViewById(R.id.iv_fav_tag);
        holder.ivLock = (ImageView)view.findViewById(R.id.iv_lock_tag);
        view.setTag(holder);
        
        // 设置Item显示的状态
        if (index == position)
        {
            view.findViewById(R.id.ll_play_program_item).setSelected(true);
        }
        else
        {
            view.findViewById(R.id.ll_play_program_item).setSelected(false);
        }
        
        ServiceInfoBean serviceInfoBean = tvProgramList.get(position);
        holder.tvServiceId.setText(serviceInfoBean.getServiceId()+"");
        holder.tvChannelName.setText(serviceInfoBean.getChannelName());
        holder.tvChannelNumber.setText(serviceInfoBean.getLogicalNumber() + "");
//        if (tvProgramList.get(position).getCaMode() == 0)
//        {
//            holder.ivCa.setVisibility(View.VISIBLE);
//        }
        if (tvProgramList.get(position).getFavFlag() == 1)
        {
            holder.ivFav.setVisibility(View.VISIBLE);
        }
        if (tvProgramList.get(position).getLockFlag() == 1)
        {
            holder.ivLock.setVisibility(View.VISIBLE);
        }
        return view;
    }
    
    class ViewHolder
    {
        TextView tvServiceId;
        TextView tvChannelNumber;
        TextView tvChannelName;
        
//        ImageView ivCa;
        ImageView ivFav;
        ImageView ivLock;
    }
}
