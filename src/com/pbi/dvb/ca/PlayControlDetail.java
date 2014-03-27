package com.pbi.dvb.ca;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnKeyListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.adapter.PvrSpinnerAdapter;
import com.pbi.dvb.dvbinterface.MessageCa.userViewInfo;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caUserView;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.PvrUtils;

public class PlayControlDetail extends BaseActivity {
	
	private String pinPwd;
	private int requestType;
	private userViewInfo uvinfo;
	
	// mode set 
	private TextView modeTextView;
	private ListView modeListView;
	private String[] modeArray;
	private PvrSpinnerAdapter modeAdapter;
	private PopupWindow modePopupWindow;
	
	// control status 
	private TextView statusTextView;
	private ListView statusListView;
	private String[] statusArray;
	private PvrSpinnerAdapter statusAdapter;
	private PopupWindow statusPopupWindow;
	
	// channel start
//	private TextView sChannelTextView;
//	private ListView sChannelListView;
//	private String[] sChannelArray;
//	private PvrSpinnerAdapter sChannelAdapter;
//	private PopupWindow sChannelPopupWindow;
	
	// channel end
//	private TextView eChannelTextView;
//	private ListView eChannelListView;
//	private String[] eChannelArray;
//	private PvrSpinnerAdapter eChannelAdapter;
//	private PopupWindow eChannelPopupWindow;
	
	private EditText sChannelEdit;
	private EditText eChannelEdit;
	
	private EditText sDateEdit;
	private EditText sTimeEdit;
	private EditText eDateEdit;
	private EditText eTimeEdit;
	
	private Button saveBtn;
	
	private String mModeStr;
	private String mStatusStr;
	private String mStartChannel;
	private String mEndChannel;
	private String mStartDate;
	private String mEndDate;
	private String mStartTime;
	private String mEndTime;

	private LinearLayout playDetailLL;
	
	private TextWatcher sDateEditWatcher = new TextWatcher() {
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			validateDate(sDateEdit);
		}

		public void afterTextChanged(Editable arg0) {
		}

		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
		}
	};
	
	private TextWatcher eDateEditWatcher = new TextWatcher() {
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			validateDate(eDateEdit);
		}

		public void afterTextChanged(Editable arg0) {
		}

		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
		}
	};
	
	private void validateDate(EditText et) {
		// 2012-12-09
		StringBuffer text = new StringBuffer(et.getText().toString());
		int index = text.length();
		switch (index) {
		case 4:
			text.append("-");
			et.setText(text.toString());
			et.setSelection(5);
			break;
		case 7:
			text.append("-");
			et.setText(text.toString());
			et.setSelection(8);
			break;
		default:
			break;
		}
	}

	private TextWatcher sTimeEditWatcher = new TextWatcher() {
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			validateTime(sTimeEdit);
		}

		public void afterTextChanged(Editable arg0) {
		}

		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
		}
	};
	
	private TextWatcher eTimeEditWatcher = new TextWatcher() {
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			validateTime(eTimeEdit);
		}

		public void afterTextChanged(Editable arg0) {
		}

		public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
				int arg3) {
		}
	};

	private void validateTime(EditText et) {
		// HH:mm
		StringBuffer text = new StringBuffer(et.getText().toString());
		int index = text.length();
		switch (index) {
		case 2:
			text.append(":");
			et.setText(text.toString());
			et.setSelection(3);
			break;
		default:
			break;
		}
	}
	
	private Handler caHandler = new Handler() {
		public void handleMessage(Message msg) {
			Log.i(TAG, "PlayControlDetail! msg.what = " + msg.what);
			
			switch (msg.what) {
			case Config.CA_MSG_CARD_Status:
				int status = msg.arg1;
				if(status == 1){
					showErrorInfo();
				}else{
					showEditView();
				}
				break;
			
			case Config.CA_MSG_PLAY_CONTROL_QUERY:
				Bundle queryBundle = (Bundle)msg.obj;
				uvinfo = (userViewInfo)queryBundle.getSerializable("uvinfo");
				Log.i(TAG, "CaStateInfo! sCardNum = " + uvinfo.iType);
				initValue();
				break;
			
			case Config.CA_MSG_PLAY_CONTROL_MODIFY:
				Log.i(TAG, "CA_MSG_PLAY_CONTROL_MODIFY, msg.arg1 = " + msg.arg1);
				if (msg.arg1 == 0) {
					showToast(getResources().getString(R.string.ca_control_info_success));
					PlayControlDetail.this.finish();
				}else if(msg.arg1 == 1){
				} else {
					showToast(getResources().getString(R.string.ca_control_info_fail));
				}
				break;
			}
		}

		private void initValue() {
			Log.i(TAG, "initValue() uvinfo.iFlag = " + uvinfo.iFlag);
//			status： 控制状态
//			 0，固定时间段 准播
//			 1，指定时间段 准播
//			 2，固定时间段 禁播
//			 3，指定时间段 禁播
//			modeArray[]: 0,准播; 1,禁播
//			statusArray[]: 0,固定时间段; 1,指定时间段
			switch (requestType) {
			case 3:
				modeTextView.setText(getResources().getString(R.string.ca_control_mode3));
				break;
			case 2:
				if(uvinfo.iFlag == 0){
					modeTextView.setText(getResources().getString(R.string.ca_control_mode3));
					break;
				}
			default:
				if(uvinfo.iStatus == 0 || uvinfo.iStatus == 1){
					modeTextView.setText(modeArray[0]);
				}else if(uvinfo.iStatus == 2 || uvinfo.iStatus == 3){
					modeTextView.setText(modeArray[1]);
				}
				break;
			}
			
			if(uvinfo.iStatus %2 == 0){//固定时间
				statusTextView.setText(statusArray[0]);
				setViewIsFocus(sDateEdit, false);
				setViewIsFocus(eDateEdit, false);
			}else{//指定时间
				statusTextView.setText(statusArray[1]);
				setViewIsFocus(sDateEdit, true);
				setViewIsFocus(eDateEdit, true);
			}
			
			sChannelEdit.setText(String.valueOf(uvinfo.iStartChannel));
			eChannelEdit.setText(String.valueOf(uvinfo.iEndChannel));
			
			String symd = "" + uvinfo.iStartYear + "-" + fillZero(uvinfo.iStartMonth) + "-" + fillZero(uvinfo.iStartDay);
			String shms = "" + fillZero(uvinfo.iStartHour) + ":" + fillZero(uvinfo.iStartMinute);
			sDateEdit.setText(symd);
			sTimeEdit.setText(shms);
			Log.i(TAG, "initValue()，shms =" + shms);
			
			String eymd = "" + uvinfo.iEndYear + "-" + fillZero(uvinfo.iEndMonth) + "-" + fillZero(uvinfo.iEndDay);
			String ehms = "" + fillZero(uvinfo.iEndHour) + ":" + fillZero(uvinfo.iEndMinute);
			eDateEdit.setText(eymd);
			eTimeEdit.setText(ehms);
			
			if (requestType == 2 || requestType == 3) {
				forbidGetFocus();
			}
		}
	};

	private void showErrorInfo() {
		errorTV.setVisibility(View.VISIBLE);
		playDetailLL.setVisibility(View.GONE);
	}
	
	private void showEditView(){
		playDetailLL.setVisibility(View.VISIBLE);
		errorTV.setVisibility(View.GONE);

		caUserView cuv = nativeCA.new caUserView();
		cuv.iType = 0;
		cuv.iPinCode = Integer.valueOf(pinPwd);
		nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_USER_VIEW_QUERY, cuv, 0);
	}
	
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.ca_playcontrol_detail);

		playDetailLL = (LinearLayout) findViewById(R.id.play_detail_ll);
		errorTV = (TextView) findViewById(R.id.error_tv);

		Intent intent = getIntent();
		pinPwd = intent.getExtras().getString("pinPwd");
		requestType = intent.getExtras().getInt("requestType", -1);

		Log.i(TAG, "pinPwd = " + pinPwd);
		Log.i(TAG, "requestType = " + requestType);

		initWidget();

		messageCa.setHandler(caHandler, this);
		nativeCA.CaMessageInit(messageCa);

		if (cardExist) {
			showEditView();
			if (requestType == 2 || requestType == 3) {
				forbidGetFocus();
			}
		} else {
			showErrorInfo();
		}
	}
	
	private void forbidGetFocus(){
		Log.i(TAG, "forbidGetFocus(). requestType = " + requestType);
		setViewIsFocus(modeTextView, false);
		setViewIsFocus(statusTextView, false);
		setViewIsFocus(sChannelEdit, false);
		setViewIsFocus(eChannelEdit, false);
		setViewIsFocus(sDateEdit, false);
		setViewIsFocus(sTimeEdit, false);
		setViewIsFocus(eDateEdit, false);
		setViewIsFocus(eTimeEdit, false);
	}
	
	private void initWidget() {
		// mode set 
		modeTextView = (TextView) this.findViewById(R.id.control_mode);
		
		if (requestType == 3) {
			Log.i(TAG, "modeTextView =++++++++++++++++++++++++++++++++++++++ ");
			modeTextView.setText(getResources().getString(R.string.ca_control_mode3));
		} else {
			Log.i(TAG, "modeTextView =-------------------------------------- ");
			setViewIsFocus(modeTextView, true);
			modeTextView.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
			modeTextView.setOnClickListener(new TextViewOnClickListener());

			modeListView = (ListView) this.getLayoutInflater().inflate(R.layout.listview_qam, null);
			modeArray = new String[]{getResources().getString(R.string.ca_control_mode1),
					getResources().getString(R.string.ca_control_mode2)};
			modeAdapter = new PvrSpinnerAdapter(PlayControlDetail.this, modeArray);
			modeListView.setAdapter(modeAdapter);
			modeListView.setDivider(null);
			modeListView.setOnItemClickListener(new ListViewOnItemClickListener());
			modeListView.setOnItemSelectedListener(new ListViewOnItemSelectedListener());
		}

		// control status 
		statusTextView = (TextView) this.findViewById(R.id.control_status);
		setViewIsFocus(statusTextView, true);
		statusTextView.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
		statusTextView.setOnClickListener(new TextViewOnClickListener());

		statusListView = (ListView) this.getLayoutInflater().inflate(R.layout.listview_qam, null);
		statusArray = new String[]{getResources().getString(R.string.ca_control_status1),
				getResources().getString(R.string.ca_control_status2)};
		statusAdapter = new PvrSpinnerAdapter(PlayControlDetail.this, statusArray);
		statusListView.setAdapter(statusAdapter);
		statusListView.setDivider(null);
		statusListView.setOnItemClickListener(new ListViewOnItemClickListener());
		statusListView.setOnItemSelectedListener(new ListViewOnItemSelectedListener());

		// channel start
/*		sChannelTextView = (TextView) this.findViewById(R.id.control_s_channel);
		setViewIsFocus(sChannelTextView, true);
		sChannelTextView.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
		sChannelTextView.setOnClickListener(new TextViewOnClickListener());

		sChannelListView = (ListView) this.getLayoutInflater().inflate(
				R.layout.listview_qam, null);
		sChannelArray = new String[]{getResources().getString(R.string.ca_control_channel_one),
				getResources().getString(R.string.ca_control_channel_two),
				getResources().getString(R.string.ca_control_channel_three),
				getResources().getString(R.string.ca_control_channel_four)};
		sChannelAdapter = new PvrSpinnerAdapter(PlayControlDetail.this, sChannelArray);
		sChannelListView.setAdapter(sChannelAdapter);
		sChannelListView.setDivider(null);
		sChannelListView.setOnItemClickListener(new ListViewOnItemClickListener());
		sChannelListView.setOnItemSelectedListener(new ListViewOnItemSelectedListener());
*/
		
		// channel end
/*		eChannelTextView = (TextView) this.findViewById(R.id.control_e_channel);
		setViewIsFocus(eChannelTextView, true);
		eChannelTextView.setOnFocusChangeListener(new TextViewOnFocusChangeListener());
		eChannelTextView.setOnClickListener(new TextViewOnClickListener());

		eChannelListView = (ListView) this.getLayoutInflater().inflate(
				R.layout.listview_qam, null);
		eChannelArray = new String[]{getResources().getString(R.string.ca_control_channel_one),
				getResources().getString(R.string.ca_control_channel_two),
				getResources().getString(R.string.ca_control_channel_three),
				getResources().getString(R.string.ca_control_channel_four)};
		eChannelAdapter = new PvrSpinnerAdapter(PlayControlDetail.this, eChannelArray);
		eChannelListView.setAdapter(eChannelAdapter);
		eChannelListView.setDivider(null);
		eChannelListView.setOnItemClickListener(new ListViewOnItemClickListener());
		eChannelListView.setOnItemSelectedListener(new ListViewOnItemSelectedListener());
*/		

		sChannelEdit = (EditText) findViewById(R.id.control_s_channel);
		sChannelEdit.setOnKeyListener(new EditTextOnKeyListener());
		sChannelEdit.setInputType(InputType.TYPE_NULL);
		sChannelEdit.setText("0");
		
		eChannelEdit = (EditText) findViewById(R.id.control_e_channel);
		eChannelEdit.setOnKeyListener(new EditTextOnKeyListener());
		eChannelEdit.setInputType(InputType.TYPE_NULL);
		eChannelEdit.setText("0");

		//start date
		sDateEdit = (EditText) findViewById(R.id.control_s_date);
		sDateEdit.addTextChangedListener(sDateEditWatcher);
		sDateEdit.setOnKeyListener(new EditTextOnKeyListener());
		sDateEdit.setInputType(InputType.TYPE_NULL);

		//start time
		sTimeEdit = (EditText) findViewById(R.id.control_s_time);
		sTimeEdit.addTextChangedListener(sTimeEditWatcher);
		sTimeEdit.setOnKeyListener(new EditTextOnKeyListener());
		sTimeEdit.setInputType(InputType.TYPE_NULL);

		//end date
		eDateEdit = (EditText) findViewById(R.id.control_e_date);
		eDateEdit.addTextChangedListener(eDateEditWatcher);
		eDateEdit.setOnKeyListener(new EditTextOnKeyListener());
		eDateEdit.setInputType(InputType.TYPE_NULL);

		//end time
		eTimeEdit = (EditText) findViewById(R.id.control_e_time);
		eTimeEdit.addTextChangedListener(eTimeEditWatcher);
		eTimeEdit.setOnKeyListener(new EditTextOnKeyListener());
		eTimeEdit.setInputType(InputType.TYPE_NULL);

		saveBtn = (Button) this.findViewById(R.id.save_play_control);
		saveBtn.setOnFocusChangeListener(new SaveBtnOnFocusChangeListener());
		saveBtn.setOnClickListener(new SaveBtnOnClickListener());

	}
	
	public void setViewIsFocus(View view, boolean visible) {
		view.setFocusable(visible);
		view.setClickable(visible);
	}

	/*************************************************************************************/
	
	class TextViewOnFocusChangeListener implements OnFocusChangeListener {
		@Override
		public void onFocusChange(View v, boolean hasFocus) {
			switch (v.getId()) {
			case R.id.control_mode:
				if (hasFocus) {
					modeTextView.setBackgroundResource(R.drawable.qam_focused);
					modeTextView.setTextColor(white);
				} else {
					modeTextView.setBackgroundResource(R.drawable.qam_normal);
					modeTextView.setTextColor(black);
				}
				break;
			case R.id.control_status:
				if (hasFocus) {
					statusTextView.setBackgroundResource(R.drawable.qam_focused);
					statusTextView.setTextColor(white);
				} else {
					statusTextView.setBackgroundResource(R.drawable.qam_normal);
					statusTextView.setTextColor(black);
				}
				break;
/*			case R.id.control_s_channel:
				if (hasFocus) {
					sChannelTextView.setBackgroundResource(R.drawable.qam_focused);
					sChannelTextView.setTextColor(white);
				} else {
					sChannelTextView.setBackgroundResource(R.drawable.qam_normal);
					sChannelTextView.setTextColor(black);
				}
				break;
			case R.id.control_e_channel:
				if (hasFocus) {
					eChannelTextView.setBackgroundResource(R.drawable.qam_focused);
					eChannelTextView.setTextColor(white);
				} else {
					eChannelTextView.setBackgroundResource(R.drawable.qam_normal);
					eChannelTextView.setTextColor(black);
				}
				break;*/
			default:
				break;
			}
		}
		
	}

	class TextViewOnClickListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.control_mode:
				if (null == modePopupWindow) {
					int width = modeTextView.getWidth();
					int height = LayoutParams.WRAP_CONTENT;

					modePopupWindow = new PopupWindow(modeListView, width, height, true);
					modePopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
				}
				if (!modePopupWindow.isShowing()) {
					modePopupWindow.showAsDropDown(modeTextView, 0, -5);
				} else {
					modePopupWindow.dismiss();
				}
				break;
			case R.id.control_status:
				if (null == statusPopupWindow) {
					int width = statusTextView.getWidth();
					int height = LayoutParams.WRAP_CONTENT;

					statusPopupWindow = new PopupWindow(statusListView, width, height, true);
					statusPopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
				}
				if (!statusPopupWindow.isShowing()) {
					statusPopupWindow.showAsDropDown(statusTextView, 0, -5);
				} else {
					statusPopupWindow.dismiss();
				}
				break;
/*			case R.id.control_s_channel:
				if (null == sChannelPopupWindow) {
					int width = sChannelTextView.getWidth();
					int height = LayoutParams.WRAP_CONTENT;

					sChannelPopupWindow = new PopupWindow(sChannelListView, width, height, true);
					sChannelPopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
				}

				if (!sChannelPopupWindow.isShowing()) {
					sChannelPopupWindow.showAsDropDown(sChannelTextView, 0, -5);
				} else {
					sChannelPopupWindow.dismiss();
				}
				break;
			case R.id.control_e_channel:
				if (null == eChannelPopupWindow) {
					int width = eChannelTextView.getWidth();
					int height = LayoutParams.WRAP_CONTENT;

					eChannelPopupWindow = new PopupWindow(eChannelListView, width, height, true);
					eChannelPopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
				}

				if (!eChannelPopupWindow.isShowing()) {
					eChannelPopupWindow.showAsDropDown(eChannelTextView, 0, -5);
				} else {
					eChannelPopupWindow.dismiss();
				}
				break;*/
			default:
				break;
			}
		}
	}

	/*************************************************************************************/

	class ListViewOnItemClickListener implements OnItemClickListener {
		@Override
		public void onItemClick(AdapterView<?> parent, View view, int position,
				long id) {
			if(parent == modeListView){
				modeTextView.setText(modeArray[position]);
				modeTextView.setBackgroundResource(R.drawable.qam_focused);
				modeTextView.setTextColor(white);
				modePopupWindow.dismiss();
			}else if (parent == statusListView) {
				statusTextView.setText(statusArray[position]);
				statusTextView.setBackgroundResource(R.drawable.qam_focused);
				statusTextView.setTextColor(white);
				statusPopupWindow.dismiss();
				
				//当为固定时间时，日期不起作用，参考时间，结束要大于开始
				if(position == 0){
					setViewIsFocus(sDateEdit, false);
					setViewIsFocus(eDateEdit, false);
				}else{
					setViewIsFocus(sDateEdit, true);
					setViewIsFocus(eDateEdit, true);
				}
			}
/*			else if(parent == sChannelListView){
				sChannelTextView.setText(sChannelArray[position]);
				sChannelTextView.setBackgroundResource(R.drawable.qam_focused);
				sChannelTextView.setTextColor(white);
				sChannelPopupWindow.dismiss();
			}else if(parent == eChannelListView){
				eChannelTextView.setText(eChannelArray[position]);
				eChannelTextView.setBackgroundResource(R.drawable.qam_focused);
				eChannelTextView.setTextColor(white);
				eChannelPopupWindow.dismiss();
			}*/
		}
	}

	class ListViewOnItemSelectedListener implements OnItemSelectedListener {
		@Override
		public void onItemSelected(AdapterView<?> parent, View view,
				int position, long id) {
			if (parent == modeListView) {
				modeTextView.setBackgroundResource(R.drawable.qam_normal);
				modeTextView.setTextColor(black);
			}else if (parent == statusListView) {
				statusTextView.setBackgroundResource(R.drawable.qam_normal);
				statusTextView.setTextColor(black);
			}
/*			else if(parent == sChannelListView){
				sChannelTextView.setBackgroundResource(R.drawable.qam_normal);
				sChannelTextView.setTextColor(black);
			}else if(parent == eChannelListView){
				eChannelTextView.setBackgroundResource(R.drawable.qam_normal);
				eChannelTextView.setTextColor(black);
			}*/
		}

		@Override
		public void onNothingSelected(AdapterView<?> parent) {
		}
	}

	/*************************************************************************************/
	
	class EditTextOnKeyListener implements OnKeyListener {
		public boolean onKey(View view, int keyCode, KeyEvent event) {
			if (keyCode == KeyEvent.KEYCODE_DEL || keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
				if (view == sDateEdit) {
					sDateEdit.setText(null);
				} else if (view == sTimeEdit) {
					sTimeEdit.setText(null);
				} else if (view == eDateEdit) {
					eDateEdit.setText(null);
				} else if (view == eTimeEdit) {
					eTimeEdit.setText(null);
				} else if (view == sChannelEdit) {
					sChannelEdit.setText(null);
				} else if (view == eChannelEdit) {
					eChannelEdit.setText(null);
				}
			}
			return false;
		}
	}

	/*************************************************************************************/

	class SaveBtnOnFocusChangeListener implements OnFocusChangeListener {
		public void onFocusChange(View v, boolean hasFocus) {
			if(playDetailLL.getVisibility() == View.VISIBLE){
				if (hasFocus) {
					saveBtn.setBackgroundResource(R.drawable.button_search_light);
					saveBtn.setTextColor(white);
				} else {
					saveBtn.setBackgroundResource(R.drawable.button_search);
					saveBtn.setTextColor(black);
				}
			}
			
		}
	}

	class SaveBtnOnClickListener implements OnClickListener {
		public void onClick(View v) {
			if(playDetailLL.getVisibility() == View.VISIBLE){
				switch (requestType) {
				case 1:
					Log.i(TAG, "SaveBtnOnClickListener , requestType = " + requestType);
					savePlayControlInfo();
					break;
				case 2:
					Log.i(TAG, "SaveBtnOnClickListener , requestType = " + requestType);
					PlayControlDetail.this.finish();
					break;
				case 3:
					Log.i(TAG, "SaveBtnOnClickListener , requestType = " + requestType);
					savePlayControlInfo();
					break;
				default:
					break;
				}
			}
		}

		private void savePlayControlInfo() {
			Log.i(TAG, "savePlayControlInfo() , requestType = " + requestType);

			mModeStr = modeTextView.getText().toString();
			mStatusStr = statusTextView.getText().toString();
			
			mStartChannel = sChannelEdit.getText().toString();
			mEndChannel= eChannelEdit.getText().toString();

			Log.i(TAG, "savePlayControlInfo() , mStartDate = " + sDateEdit.getText().toString());
			Log.i(TAG, "savePlayControlInfo() , mEndDate = " + eDateEdit.getText().toString());

			mStartDate = sDateEdit.getText().toString();
			mStartTime = sTimeEdit.getText().toString() + ":00";
			mEndDate = eDateEdit.getText().toString();
			mEndTime = eTimeEdit.getText().toString() + ":00";
			
			if(requestType == 1){
				if(mStatusStr.equals(statusArray[0])){//固定时间
					if (!validateTimeValue(mStartTime, mEndTime)) {
						return;
					}
				}else{//指定时间
					if (!validateDateValue(mStartDate + " " + mStartTime, mEndDate + " " + mEndTime)) {
						return;
					}
				}
				
				Log.i(TAG, "savePlayControlInfo() , mStartChannel = " + mStartChannel);
				Log.i(TAG, "savePlayControlInfo() , mEndChannel = " + mEndChannel);

				if(mStartChannel == null || "".equals(mStartChannel)){
					showToast(getResources().getString(R.string.ca_control_channel_info));
					return;
				}
				
				if(mEndChannel == null || "".equals(mEndChannel)){
					showToast(getResources().getString(R.string.ca_control_channel_info));
					return;
				}
				
				int sChannelId = Integer.valueOf(mStartChannel);
				int eChannelId = Integer.valueOf(mEndChannel);
				if (sChannelId > eChannelId) {
					showToast(getResources().getString(R.string.ca_control_channel_info));
					return;
				}
			}
			
			sendCaMsg();
		}
	}
	
	public void sendCaMsg(){
		caUserView cuv = nativeCA.new caUserView(); 
		cuv.iType = 1;
		
		if (requestType == 3) {
			cuv.iFLag = 0;// 取消设置
		} else {
			cuv.iFLag = 1;// 准禁播设置

			int modeId = -1;
			if (mModeStr.equals(modeArray[0])) {
				modeId = 0;
			} else if (mModeStr.equals(modeArray[1])) {
				modeId = 1;
			}

			int statusId = -1;
			if (mStatusStr.equals(statusArray[0])) {
				statusId = 0;
			} else if (mStatusStr.equals(statusArray[1])) {
				statusId = 1;
			}

			if (modeId == 0 && statusId == 0) {
				cuv.iStatus = 0;
			} else if (modeId == 0 && statusId == 1) {
				cuv.iStatus = 1;
			} else if (modeId == 1 && statusId == 0) {
				cuv.iStatus = 2;
			} else if (modeId == 1 && statusId == 1) {
				cuv.iStatus = 3;
			}
		}

//		status： 控制状态		(modeId, statusId)
//		 0，固定时间段 准播(0,0)
//		 1，指定时间段 准播(0,1)
//		 2，固定时间段 禁播(1,0)
//		 3，指定时间段 禁播(1,1)
//		modeArray[]: 0,准播; 1,禁播
//		statusArray[]: 0,固定时间段; 1,指定时间段
		
		cuv.iStartCh = Integer.valueOf(mStartChannel);
		cuv.iEndCh = Integer.valueOf(mEndChannel);
		
		String[] symd = mStartDate.split("-");
		cuv.iStartYear = Integer.valueOf(symd[0]);
		cuv.iStartMonth = Integer.valueOf(symd[1]);
		cuv.iStartDay = Integer.valueOf(symd[2]);
		
		String[] shms = mStartTime.split(":");
		cuv.iStartHour = Integer.valueOf(shms[0]);
		cuv.iStartMinute = Integer.valueOf(shms[1]);
		cuv.iStartSecond = Integer.valueOf("00");
		
		String[] eymd = mEndDate.split("-");
		cuv.iEndYear = Integer.valueOf(eymd[0]);
		cuv.iEndMonth = Integer.valueOf(eymd[1]);
		cuv.iEndDay = Integer.valueOf(eymd[2]);
		
		String[] ehms = mEndTime.split(":");
		cuv.iEndHour = Integer.valueOf(ehms[0]);
		cuv.iEndMinute = Integer.valueOf(ehms[1]);
		cuv.iEndSecond = Integer.valueOf("00");
		
		cuv.iPinCode = Integer.valueOf(pinPwd);
		
		nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_USER_VIEW_QUERY, cuv, 0);
	}
	
	//固定时间:参考时间，结束要大于开始
	public boolean validateTimeValue(String stime, String etime) {
		if (!"".equals(mStartTime) && !PvrUtils.validateHMS(mStartTime)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		}
		if (!"".equals(mEndTime) && !PvrUtils.validateHMS(mEndTime)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		}
		
		Calendar ca = Calendar.getInstance();
		int year = ca.get(Calendar.YEAR);
		int month = ca.get(Calendar.MONTH) + 1;
		int day = ca.get(Calendar.DATE);
		String ymd = year + "-" + fillZero(month) + "-" + fillZero(day) + " ";

		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date startDate;
		Date endDate;
		long result = 0;
		try {
			startDate = df.parse(ymd + stime);
			endDate = df.parse(ymd + etime);
			result = endDate.getTime() - startDate.getTime();// 毫秒
		} catch (ParseException e) {
			e.printStackTrace();
		}

		if (result > 0) {
			return true;
		} else {
			showToast(getResources().getString( R.string.ca_control_time_info));
			return false;
		}

	}
	
	//指定时间:开始和结束时间的时差不能超过45天。开始年份大于2000.
	public boolean validateDateValue(String sDateTime, String eDateTime){
		if (!"".equals(mStartDate) && !PvrUtils.validateYMD(mStartDate)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		} 
		if (!"".equals(mEndDate) && !PvrUtils.validateYMD(mEndDate)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		} 
		if (!"".equals(mStartTime) && !PvrUtils.validateHMS(mStartTime)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		}
		if (!"".equals(mEndTime) && !PvrUtils.validateHMS(mEndTime)) {
			showToast(getResources().getString(R.string.pvr_save_fail_format));
			return false;
		}
		
		int sYear = Integer.valueOf(sDateTime.substring(0, 4));
		Log.i(TAG, "start Year = " + sYear);
		if (sYear <= 2000) {
			showToast(getResources().getString( R.string.ca_control_datetime_info));
			return false;
		}
		
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date startDate;
		Date endDate;
		long result = 0;
		long compare = 0;
		try {
			startDate = df.parse(sDateTime);
			endDate = df.parse(eDateTime);
			result = endDate.getTime() - startDate.getTime();// 毫秒
			compare = df.parse("2013-05-15 00:00:00").getTime() - df.parse("2013-04-01 00:00:00").getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		
		if (result <= compare) {
			return true;
		} else {
			showToast(getResources().getString( R.string.ca_control_datetime_info));
			return false;
		}
		
	}
}
