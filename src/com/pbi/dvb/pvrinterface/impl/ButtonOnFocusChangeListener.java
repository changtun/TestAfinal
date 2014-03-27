package com.pbi.dvb.pvrinterface.impl;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.widget.Button;

import com.pbi.dvb.R;

public class ButtonOnFocusChangeListener implements OnFocusChangeListener {

	private ColorStateList black;
	private ColorStateList white;

	public ButtonOnFocusChangeListener(Context context) {

		Resources resources = context.getResources();

		black = resources.getColorStateList(R.drawable.black);
		white = resources.getColorStateList(R.drawable.white);

	}

	@Override
	public void onFocusChange(View v, boolean hasFocus) {

		Button button = (Button) v;

		if (hasFocus) {

			v.setBackgroundResource(R.drawable.button_search_light);

			button.setTextColor(white);

		} else {

			v.setBackgroundResource(R.drawable.button_search);

			button.setTextColor(black);

		}

	}
}