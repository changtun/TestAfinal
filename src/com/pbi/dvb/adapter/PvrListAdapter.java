package com.pbi.dvb.adapter;

import java.util.List;

import com.pbi.dvb.R;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.utils.PvrUtils;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class PvrListAdapter extends BaseAdapter{
	private LayoutInflater inflater;
	private List<PvrBean> pvrList;
	private Context context;

	public PvrListAdapter(Context context) {
		this.context = context;
		this.inflater = LayoutInflater.from(context);
	}

	public PvrListAdapter(Context context, List<PvrBean> pvrList) {
		this.context = context;
		this.inflater = LayoutInflater.from(context);
		this.pvrList = pvrList;
	}
	
	public void setEmailList(List<PvrBean> pvrList) {
		this.pvrList = pvrList;
	}	

	public int getCount() {
		if (null != pvrList) {
			return pvrList.size();
		}
		return 0;
	}

	public Object getItem(int position) {
		return pvrList.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		View view = null;
		ViewHolder holder = null;
		if (null == convertView) {
			view = inflater.inflate(R.layout.pvr_main_item, null);
			holder = new ViewHolder();
			holder.pvrNumber = (TextView) view.findViewById(R.id.pvr_number);
			holder.pvrMode = (TextView) view.findViewById(R.id.pvr_mode);
			holder.pvrServiceName = (TextView) view.findViewById(R.id.pvr_service_name);
			holder.pvrSetDate = (TextView) view.findViewById(R.id.pvr_set_date);
			holder.pvrWeekdate = (TextView) view.findViewById(R.id.pvr_weekdate);
			holder.pvrStartTime = (TextView) view.findViewById(R.id.pvr_start_time);
			holder.pvrTimeLength = (TextView) view.findViewById(R.id.pvr_time_length);
			view.setTag(holder);
		} else {
			view = convertView;
			holder = (ViewHolder) view.getTag();
		}
		
		PvrBean bean = pvrList.get(position);

		holder.pvrNumber.setText(String.valueOf(bean.getPvrId()));
		holder.pvrMode.setText(PvrUtils.getNameByModeId(context, bean.getPvrMode()));
		if(bean.getPvrMode() != 0){
			if(bean.getPvrMode() == 3){
				holder.pvrWeekdate.setText(PvrUtils.getNameByWeekday(context, bean.getPvrWeekDate()));
				holder.pvrWeekdate.setVisibility(View.VISIBLE);
				holder.pvrSetDate.setVisibility(View.GONE);
			}else{
				holder.pvrSetDate.setText(PvrUtils.getYMDFromLong(bean.getPvrSetDate()));
				holder.pvrSetDate.setVisibility(View.VISIBLE);
				holder.pvrWeekdate.setVisibility(View.GONE);
			}
			holder.pvrServiceName.setText(PvrUtils.getNameByLogicalNum(context, bean.getLogicalNumber()));
			holder.pvrStartTime.setText(PvrUtils.getHMFromLong(bean.getPvrStartTime()));
			holder.pvrTimeLength.setText(PvrUtils.getHMFormMS(bean.getPvrRecordLength()));
		}else{
			holder.pvrServiceName.setText(null);
			holder.pvrStartTime.setText(null);
			holder.pvrTimeLength.setText(null);
			holder.pvrSetDate.setText(null);
			holder.pvrWeekdate.setText(null);
			holder.pvrWeekdate.setVisibility(View.GONE);
			holder.pvrSetDate.setVisibility(View.VISIBLE);
		}
		
		return view;
	}
	
	class ViewHolder {
		TextView pvrNumber;
		TextView pvrMode;
		TextView pvrServiceName;
		TextView pvrSetDate;
		TextView pvrWeekdate;
		TextView pvrStartTime;
		TextView pvrTimeLength;
	}
}

