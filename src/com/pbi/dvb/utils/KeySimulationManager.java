package com.pbi.dvb.utils;

import android.os.RemoteException;
import android.os.SystemClock;
import android.os.ServiceManager;
import android.view.IWindowManager;
import android.view.KeyEvent;

public class KeySimulationManager {

	public static void sendHomeKey() {

		sendSimulationKey(KeyEvent.KEYCODE_HOME);

	}

	public static void sendSuspendLKey() {

		sendSimulationKey(KeyEvent.KEYCODE_POWER);

	}

	private static void sendSimulationKey(int keyCode) {

		try {

			Thread.sleep(100);

			long now = SystemClock.uptimeMillis();

			KeyEvent keyDown = new KeyEvent(now, now, KeyEvent.ACTION_DOWN,
					keyCode, 0);

			IWindowManager wm = IWindowManager.Stub.asInterface(ServiceManager
					.getService("window"));
			wm.injectKeyEvent(keyDown, false);

			KeyEvent keyUp = new KeyEvent(now, now, KeyEvent.ACTION_UP,
					keyCode, 0);
			wm.injectKeyEvent(keyUp, false);

			Thread.sleep(1000);

		} catch (InterruptedException e) {

			e.printStackTrace();

		} catch (RemoteException e) {

			e.printStackTrace();

		}

	}

}
