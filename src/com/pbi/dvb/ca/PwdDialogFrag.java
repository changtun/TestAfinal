package com.pbi.dvb.ca;

import com.pbi.dvb.R;

import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;

public class PwdDialogFrag extends DialogFragment implements
		View.OnClickListener {

	private EditText et;

	public static PwdDialogFrag newInstance() {
		PwdDialogFrag pdf = new PwdDialogFrag();
		return pdf;
	}

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		
		this.setCancelable(true);
		int style = DialogFragment.STYLE_NORMAL, theme = 0;
		setStyle(style, theme);
	}

	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle icicle) {
		
		View v = inflater.inflate(R.layout.ca_pin_dialog, container, false); 
		et = (EditText) v.findViewById(R.id.pin_pwd_et);

		Button dismissBtn = (Button) v.findViewById(R.id.btn_dismiss);
		dismissBtn.setOnClickListener(this);

		Button saveBtn = (Button) v.findViewById(R.id.btn_save);
		saveBtn.setOnClickListener(this);
		
		return v;
	}

	public void onClick(View v) {
		PwdDialogListener cal = (PwdDialogListener) getActivity();
		if (v.getId() == R.id.btn_save) {
			cal.onDialogDone(this.getTag(), true, et.getText().toString());
			dismiss();
			return;
		}
		if (v.getId() == R.id.btn_dismiss) {
			cal.onDialogDone(this.getTag(), false, null);
			dismiss();
			return;
		}
	}
}
