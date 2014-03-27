package com.pbi.dvb.ca;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.MessageCa.ippvlistinfo;
import com.pbi.dvb.dvbinterface.MessageCa.ppidInfo;
import com.pbi.dvb.dvbinterface.MessageCa.ppvlistinfo;
import com.pbi.dvb.dvbinterface.MessageCa.ppvnumber;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caPPVList;
import com.pbi.dvb.dvbinterface.NativeCA.caPPidInFo;
import com.pbi.dvb.dvbinterface.NativeCA.caPinCode;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.CommonUtils;

public class CaAuthDetail extends BaseActivity {

	private String myTag = "CaAuthDetail ";
	private int pinPwd;
	private int authId;
	private ListView caPpidInfoLv;
	private ppidInfo info = null;
	private String[] contents = new String[10];

	private int curPpvPage = 0;
	private boolean isShowPPV = false;
	private boolean isShowIPPV = false;
	private ListView caPPVLv;
	private ListView caIPPVLv;

	private Handler caHandler = new Handler() {
		public void handleMessage(Message msg) {
			Log.i(TAG, myTag + " msg.what = " + msg.what);

			switch (msg.what) {
			case Config.CA_MSG_CARD_Status:
				if (msg.arg1 == 1) {
					showErrorInfo();
				} else {
//					showAuthDetailView(false);
		            CommonUtils.skipActivity(CaAuthDetail.this, CaAuthInfo.class, Intent.FLAG_ACTIVITY_CLEAR_TOP);
		            CaAuthDetail.this.finish();
				}
				break;

			case Config.CA_MSG_PIN_CHECK:
				int result = msg.arg1;
				Log.i(TAG, myTag + " msg.arg1 = " + msg.arg1);
				if (result == 0) {
					showAuthDetailView(true);
				} else {
					showToast(getResources().getString(R.string.ca_dialog_pwd_error));
				}
				break;

			case Config.CA_MSG_PPID_DETAIL:
				Bundle bundle = (Bundle) msg.obj;
				info = (ppidInfo) bundle.getSerializable("pList");
				Log.i(TAG, myTag + " iPPid = " + info.iPPid);

				int i = 0;
				contents[i++] = InfoMapUtils.convertHexString(info.iPPid);
				contents[i++] = info.sLabel;
				contents[i++] = InfoMapUtils.matchPpidStatus(info.iUsedStatus, CaAuthDetail.this);
				contents[i++] = String.valueOf((float) info.iCreaditDeposit / 100);
				contents[i++] = info.sCreditDate.replace('/', '-');

				int[] arr = info.iProgRight;
				StringBuilder sb = new StringBuilder();
				for (int j = 0; j < arr.length; j++) {
					sb.append("<" + j + ">" + Integer.toHexString(arr[j]));
				}
				contents[i++] = sb.toString();

				contents[i++] = info.sStartDate.replace('/', '-');
				contents[i++] = info.sEndDate.replace('/', '-');
				
				initPPIDDetail();
				break;

			case Config.CA_MSG_PPID_DETAIL_NOTIFY:
				Log.i(TAG, myTag + " msg.arg1 = " + msg.arg1);
				if (msg.arg1 == 1) {
					showToast(getResources().getString(R.string.ca_ppid_get_fail));
				}
				break;

			case Config.CA_MSG_PPV_LIST:
				Bundle ppv = (Bundle) msg.obj;
				ppvlistinfo ppvList = (ppvlistinfo) ppv.getSerializable("ppvList");
				ArrayList<ppvnumber> ppvNum = ppvList.aPPvNum;

				showPPVInfo(ppvList.iProgCount, ppvNum);
				break;

			case Config.CA_MSG_IPPV_LIST:
				Bundle ippv = (Bundle) msg.obj;
				ippvlistinfo ippvList = (ippvlistinfo) ippv.getSerializable("ippvList");
				int[] ippvNum = ippvList.iIppvNum;

				showIPPVInfo(ippvList.iProgCount, ippvNum);
				break;

			}
		}
	};

	private void showErrorInfo() {
		errorTV.setVisibility(View.VISIBLE);
		caPpidInfoLv.setVisibility(View.GONE);
		caPPVLv.setVisibility(View.GONE);
		caIPPVLv.setVisibility(View.GONE);
		isShowPPV = false;
		isShowIPPV = false;
	}

	private void showAuthDetailView(boolean validate) {
		if (validate) {
			Log.i(TAG, myTag + "send get auth detail info msg ... ... ...");
			caPPidInFo ppidInfo = nativeCA.new caPPidInFo(pinPwd, authId);
			nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PPID_INFO_QUERY, ppidInfo, 4);
		} else {
			showPinPwdDialog();
		}
	}

	private void showPPVInfo(int iProgCount, ArrayList<ppvnumber> ppvNum) {
		Log.i(TAG, myTag + "ppv :  iProgCount = " + iProgCount);
		
		List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();

		String formatStr = getResources().getString(
				R.string.ca_ppv_ippv_pageInfo);
		String str = String.format(formatStr, String.valueOf(curPpvPage + 1),
				String.valueOf(iProgCount));

		HashMap<String, Object> pageInfo = new HashMap<String, Object>();
		pageInfo.put("caPpvIppvItem", str);
		data.add(pageInfo);

		if (ppvNum != null) {
			Iterator<ppvnumber> ppvItr = ppvNum.iterator();
			while (ppvItr.hasNext()) {
				ppvnumber ppvItem = ppvItr.next();

				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put("caPpvIppvItem", ppvItem.iStartNum + "_" + ppvItem.iEndNum);
				data.add(item);
				Log.i(TAG, myTag + "ppv :  iStartNum = " + ppvItem.iStartNum
						+ ", iEndNum = " + ppvItem.iEndNum);
			}
		}

		SimpleAdapter adapter = new SimpleAdapter(CaAuthDetail.this, data,
				R.layout.ca_auth_ppv_ippv, new String[] { "caPpvIppvItem" },
				new int[] { R.id.ca_ppv_ippv_item });
		caPPVLv.setAdapter(adapter);
		caPPVLv.invalidate();

		errorTV.setVisibility(View.GONE);
		caPpidInfoLv.setVisibility(View.GONE);
		caPPVLv.setVisibility(View.VISIBLE);
		caIPPVLv.setVisibility(View.GONE);
		isShowPPV = true;
		isShowIPPV = false;
	}

	private void showIPPVInfo(int iProgCount, int[] ippvNum) {
		Log.i(TAG, myTag + "ippv :  iProgCount = " + iProgCount);

		List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();

		String formatStr = getResources().getString(
				R.string.ca_ppv_ippv_pageInfo);
		String str = String.format(formatStr, String.valueOf(0 + 1),
				String.valueOf(iProgCount));

		HashMap<String, Object> pageInfo = new HashMap<String, Object>();
		pageInfo.put("caPpvIppvItem", str);
		data.add(pageInfo);

		if (ippvNum != null) {
			for (int index = 0; index < iProgCount; index++) {
				HashMap<String, Object> item = new HashMap<String, Object>();
				item.put("caPpvIppvItem", Integer.valueOf(ippvNum[index]));
				data.add(item);
				Log.i(TAG, myTag + "ppv :  ippvNum[index] = " + ippvNum[index]);
			}
		}

		SimpleAdapter adapter = new SimpleAdapter(CaAuthDetail.this, data,
				R.layout.ca_auth_ppv_ippv, new String[] { "caPpvIppvItem" },
				new int[] { R.id.ca_ppv_ippv_item });

		caIPPVLv.setAdapter(adapter);
		caIPPVLv.invalidate();

		errorTV.setVisibility(View.GONE);
		caPpidInfoLv.setVisibility(View.GONE);
		caPPVLv.setVisibility(View.GONE);
		caIPPVLv.setVisibility(View.VISIBLE);
		isShowPPV = false;
		isShowIPPV = true;
	}

	@Override
	public void onDialogDone(String tag, boolean confirm, String message) {
		pinPwd = Integer.valueOf(message);

		if (confirm) {// 验证密码。。。
			caPinCode capin = nativeCA.new caPinCode(Integer.valueOf(message), Integer.valueOf(message));
			nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_CHECK_PIN_QUERY, capin, 4);
		}
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.ca_auth_detail);
		caPpidInfoLv = (ListView) findViewById(R.id.ca_auth_lv);
		errorTV = (TextView) findViewById(R.id.error_tv);
		caPPVLv = (ListView) findViewById(R.id.ca_ppv_lv);
		caIPPVLv = (ListView) findViewById(R.id.ca_ippv_lv);

		Intent intent = getIntent();
		authId = intent.getExtras().getInt("id");
		pinPwd = intent.getExtras().getInt("pinPwd");
		Log.i(TAG, myTag + " ca ppid id = " + authId);

		messageCa.setHandler(caHandler, this);
		nativeCA.CaMessageInit(messageCa);

		if (cardExist) {
			showAuthDetailView(true);
		} else {
			showErrorInfo();
		}
	}

	private void initPPIDDetail() {
		List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();

		String[] ppidInfoDetail = getResources().getStringArray(
				R.array.ca_ppid_detailed_array);

		int length = ppidInfoDetail.length;
		for (int i = 0; i < length; i++) {
			HashMap<String, Object> item = new HashMap<String, Object>();
			item.put("caInfoTitle", ppidInfoDetail[i]);
			item.put("caInfoContent", contents[i]);// ca info
			data.add(item);
		}

		SimpleAdapter adapter = new SimpleAdapter(CaAuthDetail.this, data,
				R.layout.ca_info_item, new String[] { "caInfoTitle",
						"caInfoContent" }, new int[] { R.id.ca_info_title,
						R.id.ca_info_content });

		caPpidInfoLv.setAdapter(adapter);
		caPpidInfoLv.invalidate();
		
		errorTV.setVisibility(View.GONE);
		caPpidInfoLv.setVisibility(View.VISIBLE);
		caPPVLv.setVisibility(View.GONE);
		caIPPVLv.setVisibility(View.GONE);
		isShowPPV = false;
		isShowIPPV = false;
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if (KeyEvent.ACTION_DOWN == event.getAction()) {
			Log.i(TAG, myTag + "dispatchKeyEvent() ... ... keyCode = " + event.getKeyCode());
			
			if (event.getKeyCode() == Config.PAGE_UP) {
				Log.i(TAG, myTag + "dispatchKeyEvent() ... ... isShowPPV = " + isShowPPV);
				if (isShowPPV) {
					Log.i(TAG, myTag + "PAGE_UP ... ... curPpvPage = " + curPpvPage);
					curPpvPage = curPpvPage == 0 ? 0 : curPpvPage - 1;
					sendGetPPVMsg(curPpvPage);
					Log.i(TAG, myTag + "after PAGE_UP ... ... curPpvPage = " + curPpvPage);
				}
			} else if (event.getKeyCode() == Config.PAGE_DOWN) {
				Log.i(TAG, myTag + "dispatchKeyEvent() ... ... isShowPPV = " + isShowPPV);
				if (isShowPPV) {
					Log.i(TAG, myTag + "PAGE_DOWN ... ... curPpvPage = " + curPpvPage);
					curPpvPage = curPpvPage == 7 ? 7 : curPpvPage + 1;
					sendGetPPVMsg(curPpvPage);
					Log.i(TAG, myTag + "after PAGE_DOWN ... ... curPpvPage = " + curPpvPage);
				}
			}
		}
		return super.dispatchKeyEvent(event);
	}
	
	private void sendGetPPVMsg(int pageNum){
		Log.i(TAG, myTag + "send get ppv msg ... ... pageNum = " + pageNum);
		caPPVList cappv = nativeCA.new caPPVList(8, pageNum, authId);
		nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PPV_LIST_INFO_QUERY, cappv, 4);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		super.onKeyDown(keyCode, event);
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			CaAuthDetail.this.finish();
		} else if (keyCode == Config.KEY_BLUE && event.getRepeatCount() == 0) {// PPV,共8页
			Log.i(TAG, myTag + "ppv blue key click ... ... ...");
			if (isShowPPV) {
				hidePPVInfo();
			} else {
				sendGetPPVMsg(curPpvPage);
			}
			
		} else if (keyCode == Config.KEY_GREEN && event.getRepeatCount() == 0) {// IPPV,共1页
			Log.i(TAG, myTag + "ippv green key click ... ... ...");
			if (isShowIPPV) {
				hideIPPVInfo();
			} else {
				Log.i(TAG, myTag + "send get ippv msg ... ...  ");
				caPPVList cappv = nativeCA.new caPPVList(1, 0, authId);
				nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PPV_LIST_INFO_QUERY, cappv, 4);
			}
			
		}
		return false;
	}

	private void hidePPVInfo() {
		errorTV.setVisibility(View.GONE);
		caPpidInfoLv.setVisibility(View.VISIBLE);
		caPPVLv.setVisibility(View.GONE);
		caIPPVLv.setVisibility(View.GONE);
		isShowPPV = false;
		isShowIPPV = false;
	}

	private void hideIPPVInfo() {
		errorTV.setVisibility(View.GONE);
		caPpidInfoLv.setVisibility(View.VISIBLE);
		caPPVLv.setVisibility(View.GONE);
		caIPPVLv.setVisibility(View.GONE);
		isShowPPV = false;
		isShowIPPV = false;
	}

}
