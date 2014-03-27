package com.pbi.dvb.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
/**
 * <����ҳ�����б������>
 * @author  gtsong
 * @version  [�汾��, 2012-5-16]
 * @see  [�����/����]
 * @since  [��Ʒ/ģ��汾]
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
