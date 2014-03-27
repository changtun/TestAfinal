package com.pbi.dvb.adapter;

import java.util.List;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import com.pbi.dvb.R;
import com.pbi.dvb.domain.MailBean;

public class MailAdapter extends BaseAdapter{
	private LayoutInflater inflater;
	private List<MailBean> emailList;	
	private int index;// 选中Item的位置
	
	private Context context;
	private int pageNum;
	private int pageSize;

	public MailAdapter(Context context, int pageNum, int pageSize) {
		this.context = context;
		this.pageNum = pageNum;
		this.pageSize = pageSize;
		this.inflater = LayoutInflater.from(context);
	}

	public MailAdapter(Context context, int pageNum, int pageSize, List<MailBean> emailList) {
		this.context = context;
		this.pageNum = pageNum;
		this.pageSize = pageSize;
		this.inflater = LayoutInflater.from(context);
		this.emailList = emailList;
	}
	
	public void setEmailList(List<MailBean> emailList) {
		this.emailList = emailList;
	}

	public void setIndex(int index) {
		this.index = index;
	}
	
	public void setPageNum(int pageNum) {
		this.pageNum = pageNum;
	}

	public void setPageSize(int pageSize) {
		this.pageSize = pageSize;
	}

	public int getCount() {
	    if (null !=emailList)
        {
	        return emailList.size();
        }
	    return 0;
	}

	public Object getItem(int position) {
		return emailList.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) {
		View view = null;
		ViewHolder holder = null;
		if (null == convertView) {
			view = inflater.inflate(R.layout.ca_email_item, null);
			holder = new ViewHolder();
			holder.email_item_id = (TextView) view.findViewById(R.id.email_item_id);
			holder.email_item_num = (TextView) view.findViewById(R.id.email_item_num);
			holder.email_item_status = (TextView) view.findViewById(R.id.email_item_status);
			holder.email_item_title = (TextView) view.findViewById(R.id.email_item_title);
			holder.email_item_time = (TextView) view.findViewById(R.id.email_item_time);
			view.setTag(holder);
		} else {
			view = convertView;
			holder = (ViewHolder) view.getTag();
		}
		
		MailBean bean = emailList.get(position);
		int index = (pageNum - 1) * pageSize + position + 1;//该条记录在集合中的索引值，再+1 ---> ”序号“
//		Log.i("CaEmail", "---------------------- mailAdapter : index，pageSize，pageNum = " + index + " , " + pageSize + " , " + pageNum);
		String emailNum = String.valueOf(index).length()==2 ? String.valueOf(index) : "0"+String.valueOf(index);
		String emailStatus = "";
		if(bean.getMailStatus() == 0){
			emailStatus = context.getResources().getString(R.string.email_unread);
		}else{
			emailStatus = context.getResources().getString(R.string.email_read);
		}
		
		holder.email_item_id.setText(String.valueOf(bean.getMailId()));
		holder.email_item_num.setText(emailNum);
		holder.email_item_status.setText(emailStatus);
		holder.email_item_title.setText(bean.getMailTitle());
		holder.email_item_time.setText(bean.getMailTime());
		return view;
	}
	
	class ViewHolder {
		TextView email_item_id;
		TextView email_item_num;
		TextView email_item_status;
		TextView email_item_title;
		TextView email_item_time;
	}
}
