package com.pbi.dvb.ca;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.MessageCa.smartCardInfo;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.global.Config;

public class CaStateInfo extends BaseActivity {

	private ListView caStateLv;
	private String[] contents = new String[23];

	private smartCardInfo scinfo = null;

	private Handler caHandler = new Handler() {
		public void handleMessage(Message msg) {
			Log.i(TAG, "CaStateInfo! msg.what = " + msg.what);

			switch (msg.what) {
			case Config.CA_MSG_CARD_Status:
				int status = msg.arg1;
				if(status == 1){
					showErrorInfo();
				}else{
					showStateInfo();
				}
				break;
				
			case Config.CA_MSG_STATE_INFO:
				Bundle bundle = (Bundle) msg.obj;
				scinfo = (smartCardInfo) bundle.getSerializable("scinfo");
				Log.i(TAG, "CaStateInfo! iScStatus = " + scinfo.iScStatus);

				if(scinfo != null){
					// 获取CA Info ... ...
					int i = 0;
					contents[i++] = InfoMapUtils.matchStatus(scinfo.iScStatus, CaStateInfo.this);
					contents[i++] = InfoMapUtils.matchUsedStatus(scinfo.iUsedStartus, CaStateInfo.this);
					contents[i++] = InfoMapUtils.matchType(scinfo.iSCType, CaStateInfo.this);
					contents[i++] = scinfo.sCardNum;
					contents[i++] = scinfo.sCardVer;
					contents[i++] = scinfo.sCardType;// card label
					contents[i++] = InfoMapUtils.convertTwoHexString(scinfo.iSysId);
					contents[i++] = InfoMapUtils.convertTwoHexString(scinfo.iEcmSysId);
					contents[i++] = InfoMapUtils.convertTwoHexString(scinfo.iEmmSysId);
					contents[i++] = scinfo.sIsSueTime.replace('/', '-');//yyyy-mm-dd
					contents[i++] = scinfo.sExpireTime.replace('/', '-');//yyyy-mm-dd
					contents[i++] = InfoMapUtils.matchLockControl(scinfo.iParentRating, CaStateInfo.this);
					contents[i++] = "" + scinfo.iGroupCtrl;
					contents[i++] = "" + scinfo.iAntiMoveCtrl;
					contents[i++] = InfoMapUtils.matchMatch(scinfo.iSCMatch, CaStateInfo.this);
					contents[i++] = InfoMapUtils.matchStbMatch(scinfo.iSTBMatch, CaStateInfo.this);

					if (scinfo.iSCType == 1) {
						String symd = "" + scinfo.iStartYear + "-" 
										+ fillZero(scinfo.iStartMonth) + "-" 
										+ fillZero(scinfo.iStartDay);
						String shms = "" + fillZero(scinfo.iStartHour) + ":" 
										+ fillZero(scinfo.iStartMinute) + ":" 
										+ fillZero(scinfo.iStartSecond);
						contents[i++] = "" + symd + " " + shms;
						contents[i++] = "" + scinfo.iWordPeriod;
						contents[i++] = "" + scinfo.lMoutherUa;
					}
					
					Log.i(TAG, "CaStateInfo! stateInfo length = " + i);

					initWidget();
				}
				break;
			}
		}
	};

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.ca_state_info);
		caStateLv = (ListView) findViewById(R.id.ca_state_lv);
		errorTV = (TextView) findViewById(R.id.error_tv);

		messageCa.setHandler(caHandler, this);
		nativeCA.CaMessageInit(messageCa);
		nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_SC_INFO_QUERY, null, 0);// 取智能卡信息
		
		if(cardExist){
			showStateInfo();
		}else{
			showErrorInfo();
		}
	}

	private void showErrorInfo() {
		errorTV.setVisibility(View.VISIBLE);
		caStateLv.setVisibility(View.GONE);
	}

	private void showStateInfo() {
		caStateLv.setVisibility(View.VISIBLE);
		errorTV.setVisibility(View.GONE);
	}

	private void initWidget() {

		List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();
		String[] titles = getResources().getStringArray(R.array.ca_state_array);
		Log.i(TAG, "CaStateInfo!. initWidget(), titles.length = " + titles.length);

		int length = titles.length;
		for (int i = 0; i < length; i++) {
			HashMap<String, Object> item = new HashMap<String, Object>();
			item.put("caInfoTitle", titles[i]);
			item.put("caInfoContent", contents[i]);
			data.add(item);
		}

		if (scinfo.iSCType == 1) {
			String[] sonTitles = getResources().getStringArray(
					R.array.ca_state_son_array);
			int l = sonTitles.length;
			for (int i = 0; i < l; i++) {
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put("caInfoTitle", sonTitles[i]);
				item.put("caInfoContent", contents[length + i]);
				data.add(item);
			}
		}

		SimpleAdapter adapter = new SimpleAdapter(CaStateInfo.this, data,
				R.layout.ca_info_item, new String[] { "caInfoTitle",
						"caInfoContent" }, new int[] { R.id.ca_info_title,
						R.id.ca_info_content });

		caStateLv.setAdapter(adapter);
		caStateLv.setSelection(0);
		caStateLv.requestFocus(0);
		caStateLv.invalidate();
	}

}
