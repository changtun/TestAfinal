package com.pbi.dvb.adapter;

import com.pbi.dvb.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class PvrSpinnerAdapter  extends BaseAdapter {

	private String[] dataArray;
	private LayoutInflater inflater;

	public PvrSpinnerAdapter(Context context, String[] dataArray) {
		this.inflater = LayoutInflater.from(context);
		this.dataArray = dataArray;
	}

	public int getCount() {
		return dataArray.length;
	}

	public Object getItem(int position) {
		return dataArray[position];
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		View view = null;
		ViewHolder holder = null;

		if (null == convertView) {
			view = inflater.inflate(R.layout.pvr_spinner_item, null);
			holder = new ViewHolder();
			holder.textView = (TextView) view.findViewById(R.id.pvr_spinner_item);
			view.setTag(holder);
		} else {
			view = convertView;
			holder = (ViewHolder) view.getTag();
		}
		holder.textView.setText(dataArray[position]);
		return view;
	}

	class ViewHolder {
		TextView textView;
	}


}
