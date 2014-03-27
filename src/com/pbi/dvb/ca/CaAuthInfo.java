package com.pbi.dvb.ca;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import com.pbi.dvb.R;
import com.pbi.dvb.dvbinterface.MessageCa;
import com.pbi.dvb.dvbinterface.NativeCA;
import com.pbi.dvb.dvbinterface.MessageCa.ppidInfo;
import com.pbi.dvb.dvbinterface.NativeCA.caPinCode;
import com.pbi.dvb.global.Config;

public class CaAuthInfo extends BaseActivity
{
    
    private ListView caAuthListLv;
    
    private int selectPos;
    
    private String pinPwd;
    
    private ArrayList<ppidInfo> list = new ArrayList<MessageCa.ppidInfo>();
    
    private LinearLayout authListLL;
    
    private Handler caHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Log.i(TAG, "CaAuthInfo! msg.what = " + msg.what);
            
            switch (msg.what)
            {
                case Config.CA_MSG_CARD_Status:
                    int status = msg.arg1;
                    if (status == 1)
                    {
                        showErrorInfo();
                    }
                    else
                    {
                        showAuthListView();
                    }
                    break;
                
                case Config.CA_MSG_PPID_LIST:
                    int count = msg.arg1;
                    Log.i(TAG, "CaAuthInfo! count = " + count);
                    
                    list.clear();
                    if (count != 0)
                    {
                        Bundle bundle = (Bundle)msg.obj;
                        Log.i(TAG, "CaAuthInfo! iPPid = " + ((ppidInfo)bundle.getSerializable("2000")).iPPid);
                        // 获取CA Info ... ...
                        for (int i = 0; i < count; i++)
                        {
                            String key = String.valueOf(2000 + i);
                            list.add((ppidInfo)bundle.getSerializable(key));
                        }
                    }
                    
                    initPPID();
                    break;
                
                case Config.CA_MSG_PIN_CHECK:
                    int result = msg.arg1;
                    Log.i(TAG, "CaAuthInfo! msg.arg1 = " + msg.arg1);
                    if (result == 0)
                    {
                        Intent intent = new Intent(CaAuthInfo.this, CaAuthDetail.class);// ...
                        Bundle bundle2 = new Bundle();
                        int id = list.get(selectPos).iPPid;
                        bundle2.putInt("id", id);
                        bundle2.putInt("pinPwd", Integer.valueOf(pinPwd));
                        intent.putExtras(bundle2);
                        CaAuthInfo.this.startActivity(intent);
                    }
                    else
                    {
                        showToast(getResources().getString(R.string.ca_dialog_pwd_error));
                    }
                    break;
                
                case Config.CA_MSG_PPID_NOTIFY:
                    Log.i(TAG, "CaAuthInfo! msg.arg1 = " + msg.arg1);
                    if (msg.arg1 == 1)
                    {
                        showToast(getResources().getString(R.string.ca_ppid_get_fail));
                    }
                    break;
            
            }
        }
    };
    
    private void showErrorInfo()
    {
        errorTV.setVisibility(View.VISIBLE);
        authListLL.setVisibility(View.GONE);
    }
    
    private void showAuthListView()
    {
        authListLL.setVisibility(View.VISIBLE);
        errorTV.setVisibility(View.GONE);
        
        nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_PPID_QUERY, null, 0);
    }
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.ca_auth_info);
        authListLL = (LinearLayout)findViewById(R.id.auth_list_ll);
        errorTV = (TextView)findViewById(R.id.error_tv);
        caAuthListLv = (ListView)findViewById(R.id.ca_auth_info_lv);
        
        caAuthListLv.setOnItemClickListener(new OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                showPinPwdDialog();
                selectPos = position;
            }
        });
        
        messageCa.setHandler(caHandler, this);
        nativeCA.CaMessageInit(messageCa);
        
        if (cardExist)
        {
            showAuthListView();
        }
        else
        {
            showErrorInfo();
        }
    }
    
    @Override
    protected void onRestart()
    {
        super.onRestart();
        nativeCA.CaMessageInit(messageCa);
        messageCa.setHandler(caHandler, this);
    }
    
    @Override
    public void onDialogDone(String tag, boolean confirm, String message)
    {
        Log.i(TAG, "CaAuthInfo!. onDialogDone(), pwd = " + message);
        pinPwd = message;
        
        if (confirm)
        {// 验证密码。。。
            caPinCode capin = nativeCA.new caPinCode(Integer.valueOf(message), Integer.valueOf(message));
            nativeCA.SendMessageToCA(NativeCA.CA_CLIENT_CHECK_PIN_QUERY, capin, 4);
        }
    }
    
    private void initPPID()
    {
        Log.i(TAG, "CaAuthInfo!. initPPID() = ");
        
        List<HashMap<String, Object>> data = new ArrayList<HashMap<String, Object>>();
        
        for (int i = 0; i < list.size(); i++)
        {
            ppidInfo ppid = list.get(i);
            HashMap<String, Object> item = new HashMap<String, Object>();
            item.put("caPpidNo", "" + (i + 1));
            item.put("caPpidId", "" + InfoMapUtils.convertHexString(ppid.iPPid));
            item.put("caPpidName", ppid.sLabel);
            item.put("caPpidStatus", InfoMapUtils.matchPpidStatus(ppid.iUsedStatus, CaAuthInfo.this));
            data.add(item);
        }
        
        SimpleAdapter adapter =
            new SimpleAdapter(CaAuthInfo.this, data, R.layout.ca_auth_item, new String[] {"caPpidNo", "caPpidId",
                "caPpidName", "caPpidStatus"}, new int[] {R.id.ca_ppid_no, R.id.ca_ppid_id, R.id.ca_ppid_name,
                R.id.ca_ppid_status});
        
        caAuthListLv.setAdapter(adapter);
        caAuthListLv.invalidate();
    }
    
}
