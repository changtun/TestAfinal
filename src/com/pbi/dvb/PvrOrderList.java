package com.pbi.dvb;

import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.TextView;

import com.pbi.dvb.adapter.PvrListAdapter;
import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.domain.PvrBean;

public class PvrOrderList extends Activity
{
	private String tag = "PvrOrderList";

	private PvrDao pvrDao;
	private ListView listView;
	private View itemView;
	private int index = 0;

	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		setContentView(R.layout.pvr_main_list);
		listView = (ListView) findViewById(R.id.orderlist);

		pvrDao = new PvrDaoImpl(this);
		initListView();

		listView.setOnItemClickListener(new OnItemClickListener()
		{
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id)
			{
				itemView = view;
				index = position;
				startPvrOrderInfos();
			}
		});
		
	}

	public void initListView()
	{
		List<PvrBean> pvrList = pvrDao.get_All_Illegal_Tasks();

		if (!pvrList.isEmpty())
		{
			PvrListAdapter adapter = new PvrListAdapter(this, pvrList);

			listView.setAdapter(adapter);
			listView.setSelection(0);
			listView.requestFocus(0);
		} else
		{
			Log.i(tag, "pvrList is null");
		}
	}

	public void startPvrOrderInfos()
	{
		Intent intent = new Intent(this, PvrOrderInfos.class);
		TextView textView = (TextView) itemView.findViewById(R.id.pvr_number);
		intent.putExtra("flag", "list_pvr");
		intent.putExtra("pvrId", textView.getText().toString());
		startActivity(intent);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (keyCode == KeyEvent.KEYCODE_BACK)
		{// 返回键
			finish();
		}
		return false;
	}

	protected void onRestart()
	{
		super.onRestart();
		initListView();
		listView.setSelection(index);
		listView.requestFocus(index);
	}
}
