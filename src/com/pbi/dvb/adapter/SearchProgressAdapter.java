package com.pbi.dvb.adapter;

import java.util.List;

import com.pbi.dvb.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class SearchProgressAdapter extends BaseAdapter
{
    private LayoutInflater inflater;
    private List<String> programNames;
    
    public void setProgramNames(List<String> programNames)
    {
        this.programNames = programNames;
    }

    public SearchProgressAdapter(Context context)
    {
        super();
        this.inflater = LayoutInflater.from(context);
    }

    public int getCount()
    {
        return programNames.size();
    }
    
    public Object getItem(int position)
    {
        return programNames.get(position);
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
            view = inflater.inflate(R.layout.item_show_program, null);
            holder = new ViewHolder();
            holder.tvName = (TextView)view.findViewById(R.id.tv_program_show_item);
            view.setTag(holder);
        }
        else
        {
            view = convertView;
            holder = (ViewHolder)view.getTag();
        }
        holder.tvName.setText(programNames.get(position));
        return view;
    }
    
    class ViewHolder
    {
        TextView tvName;
    }
    
}
