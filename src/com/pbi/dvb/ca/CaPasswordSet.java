package com.pbi.dvb.ca;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.NativeCA.caPinCode;
import com.pbi.dvb.global.Config;

public class CaPasswordSet extends BaseActivity {
	private EditText oldPwEdit;
	private EditText newPwEdit;
	private EditText newPwEditAgain;
	private Button saveButton;
	private String TAG = "CaPasswordSet";
	private int pwdlength = 4;
	private LinearLayout pwdSetLL;
	
	private Handler caHandler = new Handler() {
		public void handleMessage(Message msg) {
			Log.i(TAG, "CaPasswordSet!  msg.what = " + msg.what);
			
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
				Log.i(TAG, "CaPasswordSet!  msg.arg1 = " + msg.arg1);
				if (msg.arg1 == 1) {
					showToast(getResources().getString(R.string.password_fail));
				} 
				break;

			case Config.CA_MSG_PIN_CHANGE:
				Log.i(TAG, "CaPasswordSet!  msg.arg1 = " + msg.arg1);				
				if (msg.arg1 == 0) {
					showToast(getResources().getString(R.string.password_success));
				}
				break;

			}
		}
	};

	private void showErrorInfo() {
		errorTV.setVisibility(View.VISIBLE);
		pwdSetLL.setVisibility(View.GONE);
	}
	
	private void showEditView(){
		pwdSetLL.setVisibility(View.VISIBLE);
		errorTV.setVisibility(View.GONE);
	}
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.ca_password_set);

		pwdSetLL = (LinearLayout) findViewById(R.id.pwd_set_ll);
		errorTV = (TextView) findViewById(R.id.error_tv);
		oldPwEdit = (EditText) findViewById(R.id.password_old);
		newPwEdit = (EditText) findViewById(R.id.password_new);
		newPwEditAgain = (EditText) findViewById(R.id.password_new_again);
		saveButton = (Button) findViewById(R.id.save_pw_modify);

		saveButton.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				if (pwdSetLL.getVisibility() == View.VISIBLE) {
					Log.i(TAG, "pwdSetLL Visibility = visibility");
					savePwModify();
				} else {
					Log.i(TAG, "pwdSetLL Visibility = gone");
					CaPasswordSet.this.finish();
				}
			}
		});

		messageCa.setHandler(caHandler, this);
		nativeCA.CaMessageInit(messageCa);

		if (cardExist) {
			showEditView();
		} else {
			showErrorInfo();
		}
	}
	
	private void savePwModify() {
		String oldPw = oldPwEdit.getText().toString();
		String newPw = newPwEdit.getText().toString();
		String newPwAgain = newPwEditAgain.getText().toString();

		Log.i(TAG, "CaPasswordSet! oldPw = " + oldPw);
		Log.i(TAG, "CaPasswordSet! newPw = " + newPw);

		if ("".equals(oldPw) || oldPw == null || oldPw.length() != pwdlength) {
			showToast(getResources().getString(R.string.password_message));
			return;
		}

		if (!newPw.equals(newPwAgain)) {
			showToast(getResources().getString(R.string.password_new_error));
			return;
		}

		if ("".equals(newPw) || newPw == null || newPw.length() != pwdlength) {
			showToast(getResources().getString(R.string.password_message));
			return;
		}

		caPinCode capin = nativeCA.new caPinCode(Integer.valueOf(oldPw), Integer.valueOf(newPw)); 
		nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_CHANGE_PIN_QUERY, capin,0);
		
		Log.i(TAG, "CaPasswordSet! SendMessageToCA.. change pin ");
	}

}
