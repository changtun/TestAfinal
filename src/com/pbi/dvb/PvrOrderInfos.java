package com.pbi.dvb;

import java.util.Calendar;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.util.TypedValue;
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
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import com.pbi.dvb.adapter.PvrSpinnerAdapter;
import com.pbi.dvb.dao.PvrDao;
import com.pbi.dvb.dao.ServiceInfoDao;
import com.pbi.dvb.dao.impl.PvrDaoImpl;
import com.pbi.dvb.dao.impl.ServiceInfoDaoImpl;
import com.pbi.dvb.db.DBPvr;
import com.pbi.dvb.domain.PvrBean;
import com.pbi.dvb.domain.ServiceInfoBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.PvrUtils;

public class PvrOrderInfos extends Activity
{
    private String tag = "PvrOrderInfos";
    
    private ColorStateList black;
    
    private ColorStateList white;
    
    private PvrDao pvrDao;
    
    private PvrBean globalBean = null;
    
    // 预约类型：关、一次、一周、每周、每工作日
    private TextView modeTextView;
    
    private ListView modeListView;
    
    private String[] modeArray;
    
    private PvrSpinnerAdapter modeAdapter;
    
    private PopupWindow modePopupWindow;
    
    // 唤醒模式：录影、播放
    private TextView wakeTextView;
    
    private ListView wakeListView;
    
    private String[] wakeArray;
    
    private PvrSpinnerAdapter wakeAdapter;
    
    private PopupWindow wakePopupWindow;
    
    // 唤醒频道
    private TextView serviceTextView;
    
    private String[] selectedService;
    
    // 日期：播放模式是‘每周’，选择星期几
    private TextView weekdateTextView;
    
    private ListView weekdateListView;
    
    private String[] weekdateArray;
    
    private PvrSpinnerAdapter weekdateAdapter;
    
    private PopupWindow weekdatePopupWindow;
    
    // 日期：默认是年月日
    private EditText dateEditText;
    
    // 开始时间
    private EditText timeEditText;
    
    // 时长
    private EditText lengthEditText;
    
    private StringBuilder inputStr;
    
    // 预约结束后，是否待机：是、否，取值分别是0、1。播放模式，只能是否；录影模式，默认为是，可以修改。
    private TextView standbyView;
    
    private ListView standbyListView;
    
    private String[] standbyArray;
    
    private PvrSpinnerAdapter standbyAdapter;
    
    private PopupWindow standbyPopupWindow;
    
    private Button pvrSaveBtn;
    
    private TextWatcher dateTextWatcher = new TextWatcher()
    {
        public void onTextChanged(CharSequence s, int start, int before, int count)
        {
            // 2012-12-09
            StringBuffer text = new StringBuffer(dateEditText.getText().toString());
            int index = text.length();
            switch (index)
            {
                case 4:
                    text.append("-");
                    dateEditText.setText(text.toString());
                    dateEditText.setSelection(5);
                    break;
                case 7:
                    text.append("-");
                    dateEditText.setText(text.toString());
                    dateEditText.setSelection(8);
                    break;
                default:
                    break;
            }
        }
        
        public void afterTextChanged(Editable arg0)
        {
        }
        
        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
        {
        }
    };
    
    private TextWatcher timeTextWatcher = new TextWatcher()
    {
        public void onTextChanged(CharSequence s, int start, int before, int count)
        {
            // HH:mm
            StringBuffer text = new StringBuffer(timeEditText.getText().toString());
            int index = text.length();
            switch (index)
            {
                case 2:
                    text.append(":");
                    timeEditText.setText(text.toString());
                    timeEditText.setSelection(3);
                    break;
                default:
                    break;
            }
        }
        
        public void afterTextChanged(Editable arg0)
        {
        }
        
        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
        {
        }
    };
    
    private TextWatcher lengthTextWatcher = new TextWatcher()
    {
        public void onTextChanged(CharSequence s, int start, int before, int count)
        {
            // HH:mm
            StringBuffer text = new StringBuffer(lengthEditText.getText().toString());
            int index = text.length();
            switch (index)
            {
                case 2:
                    text.append(":");
                    lengthEditText.setText(text.toString());
                    lengthEditText.setSelection(3);
                    break;
                default:
                    break;
            }
        }
        
        public void afterTextChanged(Editable arg0)
        {
        }
        
        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
        {
        }
    };
    
    private AlertDialog taskConflictDialog;
    
    private AlertDialog taskTimeInvalidDialog;
    
    /***********************************************************/
    
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.pvr_main_one);
        
        globalBean = new PvrBean();
        inputStr = new StringBuilder();
        
        parserLastProgram();
        
        initWidget();
        showOrderInfo();
        
    }
    
    private void parserLastProgram()
    {
        
        SharedPreferences spChannel = getSharedPreferences(Config.DVB_LASTPLAY_SERVICE, 8);
        String tvServiceName = spChannel.getString("tv_serviceName", null);
        int tvServiceId = spChannel.getInt("tv_serviceId", 0);
        int tvNumber = spChannel.getInt("tv_logicalNumber", 0);
        
        ServiceInfoDao serInfoDao = new ServiceInfoDaoImpl(this);
        ServiceInfoBean tBean = serInfoDao.getChannelInfoByServiceId(tvServiceId, tvNumber);
        
        List<ServiceInfoBean> tvList = serInfoDao.getTVChannelInfo();
        
        int logicalNumber = -1;
        if (tvList.size() != 0)
        {
            // invalid tv program.
            if (null == tBean || null == tvServiceName)
            {
                ServiceInfoBean tvBean = tvList.get(0);
                logicalNumber = tvBean.getLogicalNumber();
            }
            else
            {
                logicalNumber = tvNumber;
            }
        }
        selectedService =
            new String[] {String.valueOf(logicalNumber), PvrUtils.getNameByLogicalNum(this, logicalNumber)};
    }
    
    /**
     * 初始化页面布局中的控件及资源
     */
    private void initWidget()
    {
        Resources resources = getBaseContext().getResources();
        black = resources.getColorStateList(R.drawable.black);
        white = resources.getColorStateList(R.drawable.white);
        
        pvrDao = new PvrDaoImpl(this);
        
        /* 预约类型：关、一次、一周、每周、每工作日 */
        modeTextView = (TextView)this.findViewById(R.id.pvr_mode);
        setViewIsFocus(modeTextView, true);
        modeTextView.setOnFocusChangeListener(new PvrTextViewOnFocusChangeListener());
        modeTextView.setOnClickListener(new PvrTextViewOnClickListener());
        
        modeListView = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        modeArray = PvrUtils.getModeArray(this);
        modeAdapter = new PvrSpinnerAdapter(PvrOrderInfos.this, modeArray);
        modeListView.setAdapter(modeAdapter);
        modeListView.setDivider(null);
        modeListView.setOnItemClickListener(new PvrListViewOnItemClickListener());
        modeListView.setOnItemSelectedListener(new PvrListViewOnItemSelectedListener());
        
        /* 唤醒模式：录影、播放 */
        wakeTextView = (TextView)this.findViewById(R.id.pvr_wake);
        setViewIsFocus(wakeTextView, true);
        wakeTextView.setOnFocusChangeListener(new PvrTextViewOnFocusChangeListener());
        wakeTextView.setOnClickListener(new PvrTextViewOnClickListener());
        
        wakeListView = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        wakeArray = PvrUtils.getWakeupArray(this);
        wakeAdapter = new PvrSpinnerAdapter(PvrOrderInfos.this, wakeArray);
        wakeListView.setAdapter(wakeAdapter);
        wakeListView.setDivider(null);
        wakeListView.setOnItemClickListener(new PvrListViewOnItemClickListener());
        wakeListView.setOnItemSelectedListener(new PvrListViewOnItemSelectedListener());
        
        /* 預約結束后待機：否、是 */
        standbyView = (TextView)this.findViewById(R.id.pvr_standby);
        setViewIsFocus(standbyView, true);
        standbyView.setOnFocusChangeListener(new PvrTextViewOnFocusChangeListener());
        standbyView.setOnClickListener(new PvrTextViewOnClickListener());
        
        standbyListView = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        standbyArray = PvrUtils.getStandbyArray(this);
        standbyAdapter = new PvrSpinnerAdapter(PvrOrderInfos.this, standbyArray);
        standbyListView.setAdapter(standbyAdapter);
        standbyListView.setDivider(null);
        standbyListView.setOnItemClickListener(new PvrListViewOnItemClickListener());
        standbyListView.setOnItemSelectedListener(new PvrListViewOnItemSelectedListener());
        
        /* 唤醒频道 */
        serviceTextView = (TextView)findViewById(R.id.pvr_service);
        setViewIsFocus(serviceTextView, true);
        serviceTextView.setOnFocusChangeListener(new PvrTextViewOnFocusChangeListener());
        serviceTextView.setOnClickListener(new PvrTextViewOnClickListener());
        
        /* 日期 -- 星期几 */
        weekdateTextView = (TextView)findViewById(R.id.pvr_week_date);
        weekdateTextView.setOnFocusChangeListener(new PvrTextViewOnFocusChangeListener());
        weekdateTextView.setOnClickListener(new PvrTextViewOnClickListener());
        
        weekdateListView = (ListView)this.getLayoutInflater().inflate(R.layout.listview_qam, null);
        weekdateArray = PvrUtils.getWeekdateArray(this);
        weekdateAdapter = new PvrSpinnerAdapter(PvrOrderInfos.this, weekdateArray);
        weekdateListView.setAdapter(weekdateAdapter);
        weekdateListView.setDivider(null);
        weekdateListView.setOnItemClickListener(new PvrListViewOnItemClickListener());
        weekdateListView.setOnItemSelectedListener(new PvrListViewOnItemSelectedListener());
        
        /* 日期 -- 年月日 */
        dateEditText = (EditText)findViewById(R.id.pvr_set_date);
        dateEditText.addTextChangedListener(dateTextWatcher);
        dateEditText.setOnKeyListener(new PvrEditTextOnKeyListener());
        dateEditText.setInputType(InputType.TYPE_NULL);
        
        /* 开始时间 */
        timeEditText = (EditText)findViewById(R.id.pvr_start_time);
        timeEditText.addTextChangedListener(timeTextWatcher);
        timeEditText.setOnKeyListener(new PvrEditTextOnKeyListener());
        timeEditText.setInputType(InputType.TYPE_NULL);
        
        /* 时长 */
        lengthEditText = (EditText)findViewById(R.id.pvr_time_length);
        lengthEditText.addTextChangedListener(lengthTextWatcher);
        lengthEditText.setOnKeyListener(new PvrEditTextOnKeyListener());
        lengthEditText.setInputType(InputType.TYPE_NULL);
        
        pvrSaveBtn = (Button)this.findViewById(R.id.pvr_save);
        pvrSaveBtn.setOnFocusChangeListener(new SaveBtnOnFocusChangeListener());
        pvrSaveBtn.setOnClickListener(new SaveBtnOnClickListener());
        
    }
    
    /**
     * 预约已满时，弹出对话框
     */
    public void showOrderFullDialog(String msg)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(getResources().getString(R.string.pvr_title));
        builder.setMessage(msg);
        builder.setPositiveButton(R.string.ca_ok, new DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface arg0, int arg1)
            {
                PvrOrderInfos.this.finish();
            }
        });
        builder.show();
    }
    
    /**
     * 显示预约详细信息
     */
    private void showOrderInfo()
    {
        Intent intent = getIntent();
        String flag = intent.getStringExtra("flag");
        
        if ("list_pvr".equals(flag))
        {
            String pvrId = intent.getStringExtra("pvrId");
            if (!"".equals(pvrId) || pvrId != null)
            {
                globalBean = pvrDao.get_Record_Task_By_ID(Integer.valueOf(pvrId));
                Log.i(tag, "list : globalBean.getPvrId() = " + globalBean.getPvrId());
            }
            
        }
        else if ("epg_pvr".equals(flag))
        {
            globalBean = pvrDao.get_First_Close_Task();
            
            if (globalBean == null)
            {
                weekdateTextView.setVisibility(View.GONE);
                showOrderFullDialog(getResources().getString(R.string.pvr_orderfull_info));
                return;
            }
            else
            {
                Log.i(tag, "epg : globalBean.getPvrId() = " + globalBean.getPvrId());
                Log.i(tag, "epg : eventYMD = " + intent.getStringExtra("eventYMD"));
                globalBean.setPvrMode(1);
                globalBean.setPvrWakeMode(0);
                globalBean.setServiceId(intent.getIntExtra("serviceId", 0));
                globalBean.setLogicalNumber(intent.getIntExtra("logicalNum", 0));
                // globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                globalBean.setPvrSetDate(PvrUtils.convertFromYMD(intent.getStringExtra("eventYMD")));
                globalBean.setPvrStartTime(PvrUtils.convertFromHM(intent.getStringExtra("startTime")));
                globalBean.setPvrRecordLength(PvrUtils.convertMSFromHM(intent.getStringExtra("length")));
                globalBean.setPvr_is_sleep(0);
            }
        }
        
        if (globalBean != null)
        {
            modeTextView.setText(PvrUtils.getNameByModeId(this, globalBean.getPvrMode()));
            if (globalBean.getPvrMode() != 0)
            {
                selectedService[0] = String.valueOf(globalBean.getLogicalNumber());
                selectedService[1] = PvrUtils.getNameByLogicalNum(this, globalBean.getLogicalNumber());
                
                wakeTextView.setText(PvrUtils.getNameByWakeupId(this, globalBean.getPvrWakeMode()));
                serviceTextView.setText(selectedService[1]);
                timeEditText.setText(PvrUtils.getHMFromLong(globalBean.getPvrStartTime()));
                lengthEditText.setText(PvrUtils.getHMFormMS(globalBean.getPvrRecordLength()));
                standbyView.setText(PvrUtils.getNameByStandbyId(this, globalBean.getPvr_is_sleep()));
                
                if (globalBean.getPvrMode() == 3)
                {
                    weekdateTextView.setText(PvrUtils.getNameByWeekday(this, globalBean.getPvrWeekDate()));
                    dateEditText.setText(null);
                    
                    weekdateTextView.setVisibility(View.VISIBLE);
                    dateEditText.setVisibility(View.GONE);
                }
                else
                {
                    weekdateTextView.setText(null);
                    dateEditText.setText(PvrUtils.getYMDFromLong(globalBean.getPvrSetDate()));
                    
                    weekdateTextView.setVisibility(View.GONE);
                    dateEditText.setVisibility(View.VISIBLE);
                }
            }
            else
            {
                wakeTextView.setText(null);
                serviceTextView.setText(null);
                timeEditText.setText(null);
                lengthEditText.setText(null);
                dateEditText.setText(null);
                weekdateTextView.setText(null);
                weekdateTextView.setVisibility(View.GONE);
                dateEditText.setVisibility(View.VISIBLE);
                standbyView.setText(null);
                showOrHideView(PvrUtils.getNameByModeId(this, globalBean.getPvrMode()), null);
            }
        }
        else
        {
            Log.i(tag, "globalBean is null");
        }
        
    }
    
    public void showOrHideView(String modeName, String wakeName)
    {
        if (modeName != null)
        {
            if (modeName.equals(modeArray[0]))
            {// 关
                setViewIsFocus(wakeTextView, false);
                setViewIsFocus(serviceTextView, false);
                setViewIsFocus(timeEditText, false);
                setViewIsFocus(lengthEditText, false);
                setViewIsFocus(standbyView, false);
                
                wakeTextView.setText(null);
                serviceTextView.setText(null);
                timeEditText.setText(null);
                lengthEditText.setText(null);
                standbyView.setText(null);
                
                setViewIsFocus(dateEditText, false);
                dateEditText.setText(null);
                dateEditText.setVisibility(View.VISIBLE);
                
                setViewIsFocus(weekdateTextView, false);
                weekdateTextView.setText(null);
                weekdateTextView.setVisibility(View.GONE);
            }
            else if (modeName.equals(modeArray[3]))
            {// 每周
                setViewIsFocus(wakeTextView, true);
                setViewIsFocus(serviceTextView, true);
                setViewIsFocus(timeEditText, true);
                setViewIsFocus(lengthEditText, true);
                setViewIsFocus(standbyView, true);
                
                setViewIsFocus(weekdateTextView, true);
                weekdateTextView.setVisibility(View.VISIBLE);
                
                setViewIsFocus(dateEditText, false);
                dateEditText.setText(null);
                dateEditText.setVisibility(View.GONE);
                initItemValue();
            }
            else
            {
                setViewIsFocus(wakeTextView, true);
                setViewIsFocus(serviceTextView, true);
                setViewIsFocus(timeEditText, true);
                setViewIsFocus(lengthEditText, true);
                setViewIsFocus(standbyView, true);
                
                setViewIsFocus(dateEditText, true);
                dateEditText.setVisibility(View.VISIBLE);
                
                setViewIsFocus(weekdateTextView, false);
                weekdateTextView.setText(null);
                weekdateTextView.setVisibility(View.GONE);
                initItemValue();
            }
        }
        
        if (wakeName != null)
        {
            if (wakeName.equals(wakeArray[1]))
            {
                lengthEditText.setText("00:00");
                setViewIsFocus(lengthEditText, false);
                
                standbyView.setText(PvrUtils.getNameByStandbyId(this, 1));
                setViewIsFocus(standbyView, false);
            }
            else
            {
                lengthEditText.setText("02:00");
                setViewIsFocus(lengthEditText, true);
                
                standbyView.setText(PvrUtils.getNameByStandbyId(this, 0));
                setViewIsFocus(standbyView, true);
            }
        }
        
    }
    
    public void initItemValue()
    {
        wakeTextView.setText(wakeArray[0]);
        serviceTextView.setText(selectedService[1]);
        timeEditText.setText(PvrUtils.getCurHM());
        lengthEditText.setText("02:00");
        weekdateTextView.setText(weekdateArray[0]);
        dateEditText.setText(PvrUtils.getCurYMD());
        standbyView.setText(standbyArray[0]);
    }
    
    public void setViewIsFocus(View view, boolean visible)
    {
        view.setFocusable(visible);
        view.setClickable(visible);
    }
    
    /****** TextView的点击和焦点变化的监听事件 *********************/
    
    class PvrTextViewOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View view, boolean hasFocus)
        {
            if (view == modeTextView)
            {// pvr mode
                if (hasFocus)
                {
                    modeTextView.setBackgroundResource(R.drawable.qam_focused);
                    modeTextView.setTextColor(white);
                }
                else
                {
                    modeTextView.setBackgroundResource(R.drawable.qam_normal);
                    modeTextView.setTextColor(black);
                }
            }
            else if (view == wakeTextView)
            {// pvr wake
                if (hasFocus)
                {
                    wakeTextView.setBackgroundResource(R.drawable.qam_focused);
                    wakeTextView.setTextColor(white);
                }
                else
                {
                    wakeTextView.setBackgroundResource(R.drawable.qam_normal);
                    wakeTextView.setTextColor(black);
                }
            }
            else if (view == serviceTextView)
            {// service name
                if (hasFocus)
                {
                    serviceTextView.setBackgroundResource(R.drawable.qam_focused);
                    serviceTextView.setTextColor(white);
                }
                else
                {
                    serviceTextView.setBackgroundResource(R.drawable.qam_normal);
                    serviceTextView.setTextColor(black);
                }
            }
            else if (view == weekdateTextView)
            {// weekdate
                if (hasFocus)
                {
                    weekdateTextView.setBackgroundResource(R.drawable.qam_focused);
                    weekdateTextView.setTextColor(white);
                }
                else
                {
                    weekdateTextView.setBackgroundResource(R.drawable.qam_normal);
                    weekdateTextView.setTextColor(black);
                }
            }
            else if (view == standbyView)
            {// pvr standby
                if (hasFocus)
                {
                    standbyView.setBackgroundResource(R.drawable.qam_focused);
                    standbyView.setTextColor(white);
                }
                else
                {
                    standbyView.setBackgroundResource(R.drawable.qam_normal);
                    standbyView.setTextColor(black);
                }
            }
            
        }
    }
    
    class PvrTextViewOnClickListener implements OnClickListener
    {
        public void onClick(View view)
        {
            if (view == modeTextView)
            {// pvr mode
                if (null == modePopupWindow)
                {
                    initModePopupWindow();
                }
                if (!modePopupWindow.isShowing())
                {
                    modePopupWindow.showAsDropDown(modeTextView, 0, -5);
                }
                else
                {
                    modePopupWindow.dismiss();
                }
            }
            else if (view == wakeTextView)
            {// pvr wake
                if (null == wakePopupWindow)
                {
                    initWakePopupWindow();
                }
                if (!wakePopupWindow.isShowing())
                {
                    wakePopupWindow.showAsDropDown(wakeTextView, 0, -5);
                }
                else
                {
                    wakePopupWindow.dismiss();
                }
            }
            else if (view == serviceTextView)
            {// service name
                showServiceDialog();
            }
            else if (view == weekdateTextView)
            {// weekdate
                if (null == weekdatePopupWindow)
                {
                    initWeekdatePopupWindow();
                }
                if (!weekdatePopupWindow.isShowing())
                {
                    weekdatePopupWindow.showAsDropDown(weekdateTextView, 0, -5);
                }
                else
                {
                    weekdatePopupWindow.dismiss();
                }
            }
            else if (view == standbyView)
            {// pvr standby
                if (null == standbyPopupWindow)
                {
                    initStandbyPopupWindow();
                }
                if (!standbyPopupWindow.isShowing())
                {
                    standbyPopupWindow.showAsDropDown(standbyView, 0, -5);
                }
                else
                {
                    standbyPopupWindow.dismiss();
                }
            }
        }
    }
    
    /********* ListView的点击和焦点变化的监听事件 ******************/
    
    class PvrListViewOnItemClickListener implements OnItemClickListener
    {
        public void onItemClick(AdapterView<?> parent, View view, int position, long id)
        {
            if (parent == modeListView)
            {
                modeTextView.setText(modeArray[position]);
                modeTextView.setBackgroundResource(R.drawable.qam_focused);
                modeTextView.setTextColor(white);
                modePopupWindow.dismiss();
                showOrHideView(modeArray[position], null);
            }
            else if (parent == wakeListView)
            {
                wakeTextView.setText(wakeArray[position]);
                wakeTextView.setBackgroundResource(R.drawable.qam_focused);
                wakeTextView.setTextColor(white);
                wakePopupWindow.dismiss();
                showOrHideView(null, wakeArray[position]);
            }
            else if (parent == weekdateListView)
            {
                weekdateTextView.setText(weekdateArray[position]);
                weekdateTextView.setBackgroundResource(R.drawable.qam_focused);
                weekdateTextView.setTextColor(white);
                weekdatePopupWindow.dismiss();
            }
            else if (parent == standbyListView)
            {
                standbyView.setText(standbyArray[position]);
                standbyView.setBackgroundResource(R.drawable.qam_focused);
                standbyView.setTextColor(white);
                standbyPopupWindow.dismiss();
            }
        }
    }
    
    class PvrListViewOnItemSelectedListener implements OnItemSelectedListener
    {
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
        {
            if (parent == modeListView)
            {
                modeTextView.setBackgroundResource(R.drawable.qam_normal);
                modeTextView.setTextColor(black);
            }
            else if (parent == wakeListView)
            {
                wakeTextView.setBackgroundResource(R.drawable.qam_normal);
                wakeTextView.setTextColor(black);
            }
            else if (parent == weekdateListView)
            {
                weekdateTextView.setBackgroundResource(R.drawable.qam_normal);
                weekdateTextView.setTextColor(black);
            }
            else if (parent == standbyListView)
            {
                standbyView.setBackgroundResource(R.drawable.qam_normal);
                standbyView.setTextColor(black);
            }
        }
        
        public void onNothingSelected(AdapterView<?> parent)
        {
        }
    }
    
    /********* PopupWindow ********************/
    
    private void initModePopupWindow()
    {// pvr mode
        int width = modeTextView.getWidth();
        int height = LayoutParams.WRAP_CONTENT;
        
        modePopupWindow = new PopupWindow(modeListView, width, height, true);
        modePopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
    }
    
    private void initWakePopupWindow()
    {// pvr wake
        int width = wakeTextView.getWidth();
        int height = LayoutParams.WRAP_CONTENT;
        
        wakePopupWindow = new PopupWindow(wakeListView, width, height, true);
        wakePopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
    }
    
    private void initWeekdatePopupWindow()
    {// weekdate
        int width = weekdateTextView.getWidth();
        int height = LayoutParams.WRAP_CONTENT;
        
        weekdatePopupWindow = new PopupWindow(weekdateListView, width, height, true);
        weekdatePopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
    }
    
    private void initStandbyPopupWindow()
    {// pvr standby
        int width = standbyView.getWidth();
        int height = LayoutParams.WRAP_CONTENT;
        
        standbyPopupWindow = new PopupWindow(standbyListView, width, height, true);
        standbyPopupWindow.setBackgroundDrawable(new ColorDrawable(0xA0838383));
    }
    
    /*************** service name list *******************************/
    
    public void showServiceDialog()
    {
        List<ServiceInfoBean> serviceList = PvrUtils.getServiceInfoList(this);
        int size = serviceList.size();
        final String[] data = new String[size];
        for (int i = 0; i < size; i++)
        {
            ServiceInfoBean bean = serviceList.get(i);
            data[i] = bean.getLogicalNumber() + " " + bean.getChannelName();
        }
        
        new AlertDialog.Builder(this).setTitle(R.string.pvr_service_name)
            .setItems(data, new DialogInterface.OnClickListener()
            {
                public void onClick(DialogInterface dialog, int which)
                {
                    int index = data[which].indexOf(" ");
                    selectedService[0] = data[which].substring(0, index);
                    selectedService[1] = data[which].substring(index + 1);
                    serviceTextView.setText(selectedService[1]);
                }
            })
            .show();
    }
    
    /******** EditText *********************/
    
    class PvrEditTextOnKeyListener implements OnKeyListener
    {
        public boolean onKey(View v, int keyCode, KeyEvent event)
        {
            if (keyCode == KeyEvent.KEYCODE_DEL || keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
            {
                clearEditText(v);
            }
            return false;
        }
    }
    
    public void clearEditText(View view)
    {
        if (view == dateEditText)
        {
            inputStr.setLength(0);
            dateEditText.setText(null);
        }
        else if (view == timeEditText)
        {
            inputStr.setLength(0);
            timeEditText.setText(null);
        }
        else if (view == lengthEditText)
        {
            inputStr.setLength(0);
            lengthEditText.setText(null);
        }
    }
    
    /***************************************/
    
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {
        if (keyCode == KeyEvent.KEYCODE_BACK)
        {// 返回键
            finish();
        }
        return false;
    }
    
    /****** 保存设置信息 ****************************/
    
    class SaveBtnOnFocusChangeListener implements OnFocusChangeListener
    {
        public void onFocusChange(View v, boolean hasFocus)
        {
            if (hasFocus)
            {
                pvrSaveBtn.setBackgroundResource(R.drawable.button_search_light);
                pvrSaveBtn.setTextColor(white);
            }
            else
            {
                pvrSaveBtn.setBackgroundResource(R.drawable.button_search);
                pvrSaveBtn.setTextColor(black);
            }
        }
    }
    
    class SaveBtnOnClickListener implements OnClickListener
    {
        public void onClick(View v)
        {
            saveOrderInfo();
        }
    }
    
    private void saveOrderInfo()
    {
        String mode = modeTextView.getText().toString();
        if ("".equals(mode) || mode == null)
        {
            Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
            return;
        }
        else
        {
            globalBean.setPvrMode(PvrUtils.getIdByModeName(this, mode));
        }
        
        if (!mode.equals(modeArray[0]))
        {
            String wakeup = wakeTextView.getText().toString();
            if ("".equals(wakeup) || wakeup == null)
            {
                Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
                return;
            }
            else
            {
                globalBean.setPvrWakeMode(PvrUtils.getIdByWakeupName(this, wakeup));
            }
            
            String serviceName = serviceTextView.getText().toString();
            if ("".equals(serviceName) || serviceName == null)
            {
                Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
                return;
            }
            else
            {
                globalBean.setLogicalNumber(Integer.valueOf(selectedService[0]));
                globalBean.setServiceId(PvrUtils.getServiceIdByLogicalNum(this, Integer.valueOf(selectedService[0])));
            }
            
            String startTime = timeEditText.getText().toString();
            if ("".equals(startTime) || startTime == null)
            {
                Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
                return;
            }
            else
            {
                if (!PvrUtils.validateHM(startTime))
                {
                    Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_format), Toast.LENGTH_SHORT)
                        .show();
                    return;
                }
                else
                {
                    globalBean.setPvrStartTime(PvrUtils.convertFromHM(startTime));
                }
            }
            
            String lengthTime = lengthEditText.getText().toString();
            if ("".equals(lengthTime) || lengthTime == null)
            {
                Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
                return;
            }
            else
            {
                if (!PvrUtils.validateHM(lengthTime))
                {
                    Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_format), Toast.LENGTH_SHORT)
                        .show();
                    return;
                }
                else
                {
                    globalBean.setPvrRecordLength(PvrUtils.convertMSFromHM(lengthTime));
                }
            }
            
            String standby = standbyView.getText().toString();
            if ("".equals(standby) || standby == null)
            {
                Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT).show();
                return;
            }
            else
            {
                globalBean.setPvr_is_sleep(PvrUtils.getIdByStandbyName(this, standby));
                Log.i(tag, "standby = " + standby);
            }
            
            if (mode.equals(modeArray[3]))
            {
                String weekdate = weekdateTextView.getText().toString();
                if ("".equals(weekdate) || weekdate == null)
                {
                    Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT)
                        .show();
                    return;
                }
                else
                {
                    globalBean.setPvrWeekDate(PvrUtils.getWeekdateByName(this, weekdate));
                    globalBean.setPvrSetDate(PvrUtils.convertFromYMD(PvrUtils.getCurYMD()));
                }
            }
            else
            {
                String setDate = dateEditText.getText().toString();
                if ("".equals(setDate) || setDate == null)
                {
                    Toast.makeText(this, getResources().getString(R.string.pvr_save_fail_null), Toast.LENGTH_SHORT)
                        .show();
                    return;
                }
                else
                {
                    if (!PvrUtils.validateYMD(setDate))
                    {
                        Toast.makeText(this,
                            getResources().getString(R.string.pvr_save_fail_format),
                            Toast.LENGTH_SHORT).show();
                        return;
                    }
                    else
                    {
                        globalBean.setPvrSetDate(PvrUtils.convertFromYMD(setDate));
                        Log.i(tag, "PvrUtils.getWeekFromYMD(setDate) = " + PvrUtils.getWeekFromYMD(setDate));
                        globalBean.setPvrWeekDate(PvrUtils.getWeekFromYMD(setDate));
                    }
                }
            }
            
        }
        else
        {
            // globalBean.setPvrWakeMode(null);
            // globalBean.setLogicalNumber(null);
            // globalBean.setServiceId(null);
            // globalBean.setPvrSetDate(null);
            // globalBean.setPvrWeekDate(null);
            // globalBean.setPvrStartTime(null);
            // globalBean.setPvrRecordLength(null);
        }
        
        Calendar midNight = Calendar.getInstance();
        
        long currentTime = midNight.getTimeInMillis();
        
        midNight.set(Calendar.HOUR_OF_DAY, 0);
        midNight.set(Calendar.MINUTE, 0);
        midNight.set(Calendar.SECOND, 0);
        midNight.set(Calendar.MILLISECOND, 0);
        
        currentTime = currentTime - midNight.getTimeInMillis();
        
        if (globalBean.getPvrMode() == DBPvr.DO_ONCE_MODE && globalBean.getPvrStartTime() < currentTime
            && globalBean.getPvrSetDate() == midNight.getTimeInMillis())
        {
            
            if (taskTimeInvalidDialog == null)
            {
                
                AlertDialog.Builder taskTimeInvalidDialogBuilder = new AlertDialog.Builder(this);
                
                View view = View.inflate(this, R.layout.pvr_check_box_with_one_button, null);
                
                TextView tv_showTaskInvalid = (TextView)view.findViewById(R.id.tv_showTaskInvalid);
                
                tv_showTaskInvalid.setTextSize(TypedValue.COMPLEX_UNIT_SP, 30);
                
                tv_showTaskInvalid.setText(R.string.pvr_time_invalid);
                
                Button bt_Ok = (Button)view.findViewById(R.id.bt_Ok);
                
                bt_Ok.setOnClickListener(new OnClickListener()
                {
                    
                    @Override
                    public void onClick(View v)
                    {
                        
                        taskTimeInvalidDialog.dismiss();
                        
                    }
                });
                
                taskTimeInvalidDialogBuilder.setView(view);
                
                taskTimeInvalidDialog = taskTimeInvalidDialogBuilder.create();
                
            }
            
            taskTimeInvalidDialog.show();
            
        }
        else
        {
            
            // if (checkTask(globalBean))
            if (globalBean.checkTaskConfict(PvrOrderInfos.this))
            {
                
                Log.e(tag, "Update the record task for OK button");
                
                pvrDao.update_Record_Task(globalBean);
                
                finish();
                
            }
            else
            {
                
                Log.e(tag, "conflict task");
                
                if (taskConflictDialog == null)
                {
                    
                    AlertDialog.Builder taskConflictDialogBuilder = new AlertDialog.Builder(this);
                    
                    View view = View.inflate(this, R.layout.pvr_check_box_with_one_button, null);
                    
                    TextView tv_showTaskInvalid = (TextView)view.findViewById(R.id.tv_showTaskInvalid);
                    
                    tv_showTaskInvalid.setTextSize(TypedValue.COMPLEX_UNIT_SP, 30);
                    
                    tv_showTaskInvalid.setText(R.string.pvr_time_conflict);
                    
                    Button bt_Ok = (Button)view.findViewById(R.id.bt_Ok);
                    
                    bt_Ok.setOnClickListener(new OnClickListener()
                    {
                        
                        @Override
                        public void onClick(View v)
                        {
                            
                            taskConflictDialog.dismiss();
                            
                        }
                    });
                    
                    taskConflictDialogBuilder.setView(view);
                    
                    taskConflictDialog = taskConflictDialogBuilder.create();
                }
                
                taskConflictDialog.show();
            }
        }
    }
    
    // private boolean checkTask(PvrBean pvrBean)
    // {
    // // Return ture means the task has no conflict with the items in database
    // // Return false means that you can not insert this bean into database
    //
    // Log.e(tag, "pvrBean = " + pvrBean.toString());
    //
    // boolean result = false;
    //
    // if (pvrDao == null)
    // {
    //
    // pvrDao = new PvrDaoImpl(this);
    //
    // }
    //
    // int conflict = pvrDao.get_Count_Of_Conflict_Task(pvrBean);
    //
    // Log.e(tag, "There are total " + conflict + " conflict tasks");
    //
    // if (conflict == 0)
    // {
    //
    // result = true;
    //
    // }
    //
    // return result;
    // }
    
}
