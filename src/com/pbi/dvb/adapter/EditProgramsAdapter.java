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

public class EditProgramsAdapter extends BaseAdapter
{
    protected static final String TAG = "EditProgramsAdapter";

    private LayoutInflater inflater;
    
    private List<ServiceInfoBean> channelList;
    
    public EditProgramsAdapter(Context context)
    {
        inflater = LayoutInflater.from(context);
    }
    
    public void setChannelList(List<ServiceInfoBean> channelList)
    {
        this.channelList = channelList;
    }
    
    public int getCount()
    {
        return channelList.size();
    }
    
    public Object getItem(int position)
    {
        return channelList.get(position);
    }
    
    public long getItemId(int position)
    {
        return position;
    }
    
    public View getView(final int position, View convertView, ViewGroup parent)
    {
        View view = inflater.inflate(R.layout.item_edit_content, null);
        TextView tvCno = (TextView)view.findViewById(R.id.tv_channel_contentNo);
        TextView tvCname = (TextView)view.findViewById(R.id.tv_channel_contentName);
        ImageView ivFav = (ImageView)view.findViewById(R.id.iv_channel_fav);
//        ImageView ivMov = (ImageView)view.findViewById(R.id.iv_channel_mov);
        ImageView ivLock = (ImageView)view.findViewById(R.id.iv_channel_lock);
        ImageView ivDel = (ImageView)view.findViewById(R.id.iv_channel_del);
        
        tvCno.setText(channelList.get(position).getLogicalNumber() + "");
        tvCname.setText(channelList.get(position).getChannelName());
        
        if (channelList.get(position).getFavFlag() == 1)
        {
            ivFav.setVisibility(View.VISIBLE);
        }
/*        if (channelList.get(position).getMoveFlag() == 1)
        {
            ivMov.setVisibility(View.VISIBLE);
        }*/
        if (channelList.get(position).getLockFlag() == 1)
        {
            ivLock.setVisibility(View.VISIBLE);
        }
        if (channelList.get(position).getDelFlag() == 1)
        {
            ivDel.setVisibility(View.VISIBLE);
        }
        
        return view;
    }
    
}
