package com.pbi.dvb.ca;

import com.pbi.dvb.R;
import com.pbi.dvb.ca.PlayControlDetail.SaveBtnOnClickListener;
import com.pbi.dvb.ca.PlayControlDetail.SaveBtnOnFocusChangeListener;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caPinCode;
import com.pbi.dvb.global.Config;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class PlayControlMain extends BaseActivity {
	
	private Button setBtn;
	private Button queryBtn;
	private Button cancelBtn;
	private Button backBtn;
	private LinearLayout playMainLL;

	private String pinPwd;
	private int requestType;

	private Handler caHandler = new Handler() {
		public void handleMessage(Message msg) {
			Log.i(TAG, "PlayControl!  msg.what = " + msg.what);
			
			switch (msg.what) {
			case Config.CA_MSG_CARD_Status:
				int status = msg.arg1;
				if(status == 1){
					showErrorInfo();
				}else{
					showEditView();
				}
				break;
			
			case Config.CA_MSG_PIN_CHECK:
				int result = msg.arg1;
				Log.i(TAG, "PlayControl! msg.arg1 = " + msg.arg1);
				if(result==0){
					Intent intent = new Intent(PlayControlMain.this, PlayControlDetail.class);// ...
		            Bundle bundle = new Bundle();
		            bundle.putString("pinPwd", pinPwd);
		            bundle.putInt("requestType", requestType);
		            intent.putExtras(bundle);
		            PlayControlMain.this.startActivity(intent);
				}else {
					showToast(getResources().getString(R.string.ca_dialog_pwd_error));
				}
				break;
			}
		}
	};

	private void showErrorInfo() {
		errorTV.setVisibility(View.VISIBLE);
		playMainLL.setVisibility(View.GONE);
	}
	
	private void showEditView(){
		playMainLL.setVisibility(View.VISIBLE);
		errorTV.setVisibility(View.GONE);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.ca_playcontrol_main);

		playMainLL = (LinearLayout) findViewById(R.id.play_main_ll);
		errorTV = (TextView) findViewById(R.id.error_tv);
		setBtn = (Button) findViewById(R.id.control_set_btn);
		queryBtn = (Button) findViewById(R.id.control_query_btn);
		cancelBtn = (Button) findViewById(R.id.control_cancel_btn);

		messageCa.setHandler(caHandler, this);
		nativeCA.CaMessageInit(messageCa);

		setBtn.setOnClickListener(new BtnOnClickListener());
		queryBtn.setOnClickListener(new BtnOnClickListener());
		cancelBtn.setOnClickListener(new BtnOnClickListener());

		setBtn.setOnFocusChangeListener(new BtnOnFocusChangeListener());
		queryBtn.setOnFocusChangeListener(new BtnOnFocusChangeListener());
		cancelBtn.setOnFocusChangeListener(new BtnOnFocusChangeListener());

		backBtn = (Button) this.findViewById(R.id.play_control);
		backBtn.setOnFocusChangeListener(new OnFocusChangeListener() {
			@Override
			public void onFocusChange(View v, boolean hasFocus) {
				if (hasFocus) {
					backBtn.setBackgroundResource(R.drawable.button_search_light);
					backBtn.setTextColor(white);
				} else {
					backBtn.setBackgroundResource(R.drawable.button_search);
					backBtn.setTextColor(black);
				}

			}
		});
		backBtn.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				PlayControlMain.this.finish();
			}
		});
		
		if (cardExist) {
			showEditView();
		} else {
			showErrorInfo();
		}
	}
	
	@Override
	protected void onRestart() {
		super.onRestart();
		nativeCA.CaMessageInit(messageCa);
		messageCa.setHandler(caHandler, this);
	}
	
	class BtnOnFocusChangeListener implements OnFocusChangeListener {
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				switch (v.getId()) {
				case R.id.control_set_btn:
					btnHasFocus(setBtn);
					break;
				case R.id.control_query_btn:
					btnHasFocus(queryBtn);
					break;
				case R.id.control_cancel_btn:
					btnHasFocus(cancelBtn);
					break;
				default:
					break;
				}
			} else {
				switch (v.getId()) {
				case R.id.control_set_btn:
					btnNotHasFocus(setBtn);
					break;
				case R.id.control_query_btn:
					btnNotHasFocus(queryBtn);
					break;
				case R.id.control_cancel_btn:
					btnNotHasFocus(cancelBtn);
					break;
				default:
					break;
				}
			}
		}
	}	
	
	private void btnHasFocus(Button btn) {
		btn.setBackgroundResource(R.drawable.button_search_light);
		btn.setTextColor(white);
	}

	private void btnNotHasFocus(Button btn) {
		btn.setBackgroundResource(R.drawable.button_search);
		btn.setTextColor(black);
	}
	
	private class BtnOnClickListener implements  OnClickListener{
		public void onClick(View v) {

			showPinPwdDialog();
			
			switch (v.getId()) {
			case R.id.control_set_btn:
				requestType = request_set;
				break;
			case R.id.control_query_btn:
				requestType = request_query;
				break;
			case R.id.control_cancel_btn:
				requestType = request_cancel;
				break;
			default:
				break;
			}
		}
	}

	@Override
	public void onDialogDone(String tag, boolean confirm, String message) {
		Log.i(TAG, "PlayControl!. onDialogDone(), pwd = " + message);
		pinPwd = message;
		
		//test
//		if("0000".equals(message)){
//			Intent intent = new Intent(PlayControlMain.this, PlayControlDetail.class);// ...
//            Bundle bundle = new Bundle();
//            bundle.putString("pinPwd", pinPwd);
//            bundle.putInt("requestType", requestType);
//            intent.putExtras(bundle);
//            PlayControlMain.this.startActivity(intent);
//		}
		

		if (confirm) {// 楠岃瘉瀵嗙爜銆傘�銆�
			caPinCode capin = nativeCA.new caPinCode(Integer.valueOf(message),
					Integer.valueOf(message));
			nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_CHECK_PIN_QUERY, capin, 4);
		}
	}
}
