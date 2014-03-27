package com.pbi.dvb.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
/**
 * <搜索页下拉列表填充器>
 * @author  gtsong
 * @version  [版本号, 2012-5-16]
 * @see  [相关类/方法]
 * @since  [产品/模块版本]
 */
public class QamAdapter extends BaseAdapter
{
    
    private String[] qamArray;
    private LayoutInflater inflater;
    
    public QamAdapter(Context context, String[] qamArray)
    {
        this.inflater = LayoutInflater.from(context);
        this.qamArray = qamArray;
    }
    
    public int getCount()
    {
        return qamArray.length;
    }
    
    public Object getItem(int position)
    {
        return qamArray[position];
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
            view = inflater.inflate(R.layout.item_qam, null);
            holder = new ViewHolder();
            holder.tvQam = (TextView)view.findViewById(R.id.tv_qam_item);
            view.setTag(holder);
        }
        else
        {
            view = convertView;
            holder = (ViewHolder)view.getTag();
        }
        holder.tvQam.setText(qamArray[position]);
        return view;
    }
    
    class ViewHolder
    {
        TextView tvQam;
    }
    
}
