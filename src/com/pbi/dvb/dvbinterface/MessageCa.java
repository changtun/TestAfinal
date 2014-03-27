package com.pbi.dvb.dvbinterface;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.pbi.dvb.dao.MailDao;
import com.pbi.dvb.dao.impl.MailDaoImpl;
import com.pbi.dvb.domain.MailBean;
import com.pbi.dvb.global.Config;
import com.pbi.dvb.utils.LogUtils;

public class MessageCa
{
    private static final String TAG = "MessageCa";
    
    public class maildata
    {
        private int bStatus;/* 1: Bmail数据有效 0：无Bmail */
        private String bMsgFrom;/* from 数据内容 */
        private String bMsgTitle;/* 标题 */
        private String bMsgData;/* 内容 */
        private int bType;
        
        /* 以下是邮件发送时�?*/
        private int sMsgYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private int sMsgMonth; /* 1 to 12 */
        private int sMsgDay; /* 1 to 31 */
        private int sMsgHour; /* 0 to 23 */
        private int sMsgMinute; /* 0 to 59 */
        private int sMsgSecond; /* 0 to 59 */
        
        /* 以下是邮件创建时�?*/
        private int sCreateYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private int sCreateMonth; /* 1 to 12 */
        private int sCreateDay; /* 1 to 31 */
        private int sCreateHour; /* 0 to 23 */
        private int sCreateMinute; /* 0 to 59 */
        private int sCreateSecond; /* 0 to 59 */
        
	public void logcat()
	{
		Log.i(TAG, "maildata:" );
		Log.i(TAG, "bType:" + bType);
		Log.i(TAG, "bStatus:" + bStatus);
		Log.i(TAG, "bMsgTitle:" + bMsgTitle);
		Log.i(TAG, "bMsgData:" + bMsgData);
		Log.i(TAG, "Send:" + sMsgYear + "-" +sMsgMonth + "-" +sMsgDay + " | " +sMsgHour + ":" +sMsgMinute + ":" +sMsgSecond  );
		Log.i(TAG, "Create:" + sCreateYear + "-" +sCreateMonth + "-" +sCreateDay + " | " +sCreateHour + ":" +sCreateMinute + ":" +sCreateSecond  );
		
	}
        public void setbStatus(int bStatus)
        {
            this.bStatus = bStatus;
        }

        public void setbMsgFrom(String bMsgFrom)
        {
            this.bMsgFrom = bMsgFrom;
        }

        public void setbMsgTitle(String bMsgTitle)
        {
            this.bMsgTitle = bMsgTitle;
        }

        public void setbMsgData(String bMsgData)
        {
            this.bMsgData = bMsgData;
        }

        public void setbType(int bType)
        {
            this.bType = bType;
        }

        public void setsCreateYear(int sCreateYear)
        {
            this.sCreateYear = sCreateYear;
        }

        public void setsCreateMonth(int sCreateMonth)
        {
            this.sCreateMonth = sCreateMonth;
        }

        public void setsCreateDay(int sCreateDay)
        {
            this.sCreateDay = sCreateDay;
        }

        public void setsCreateHour(int sCreateHour)
        {
            this.sCreateHour = sCreateHour;
        }

        public void setsCreateMinute(int sCreateMinute)
        {
            this.sCreateMinute = sCreateMinute;
        }

        public void setsCreateSecond(int sCreateSecond)
        {
            this.sCreateSecond = sCreateSecond;
        }

        public String getbMsgTitle()
        {
            return bMsgTitle;
        }
        
        public String getbMsgData()
        {
            return bMsgData;
        }
        
        public String getbMsgFrom()
        {
            return bMsgFrom;
        }
        
        public int getbType()
        {
            return bType;
        }
        
        public int getbStatus()
        {
            return bStatus;
        }
        
        public String getMailTime()
        {
            return sCreateYear + "-" + sCreateMonth + "-" + sCreateDay + "  " + alignCharacter(sCreateHour) + ":"
                + alignCharacter(sCreateMinute) + ":" + alignCharacter(sCreateSecond);
        }
        
        private  String alignCharacter(int character)
        {
            if (character < 10)
            {
                return "0" + character;
            }
            return "" + character;
        }
    }
    
    public class ippnotifymsg implements Serializable
    {
        private int iChannelId;/*当前IPPV节目所属的节目编号*/
        private int iStatus;/*0 允许购买节目�?1 读卡错误，所有数据都置1 2卡内余额不足，不能购�?*/
        private int iIppId;/*当前IPPV节目所属的节目提供商的ID*/
        private int iType_p;/*1 支持按节目点播方�?0 不支�?*/
        private int iType_t;/*1 支持按时间点播方�?0 不支�?*/
        private int iIppvNum; /* IPPV节目的标识ID，转换为10进制数据进行显示*/
        private int iPrice_p;/*按节目点播IPPV的价格，单位为分，如果为0 ，表示不支持按节目方式点�*/
        private int iPrice_t;/*按时间点播IPPV的价格，单位为分，如果为0 ，表示不支持按节目方式点�*/
        private String sIppidName;/*当前IPPV节目所属的节目提供商的名字*/
        
        public void setiChannelId(int value)
        {
            iChannelId = value;
        }
        
        public int getiChannelId()
        {
            return iChannelId;
        }
        
        public void setiStatus(int value)
        {
            iStatus = value;
        }
        
        public int getiStatus()
        {
            return iStatus;
        }
        
        public void setiIppId(int value)
        {
            iIppId = value;
        }
        
        public int getiIppId()
        {
            return iIppId;
        }
        
        public void setiType_p(int value)
        {
            iType_p = value;
        }
        
        public int getiiType_p()
        {
            return iType_p;
        }
        
        public void setiType_t(int value)
        {
            iType_t = value;
        }
        
        public int getiiType_t()
        {
            return iType_t;
        }
        
        public void setiIppvNum(int value)
        {
            iIppvNum = value;
        }
        
        public int getiIppvNum()
        {
            return iIppvNum;
        }
        
        public void setiPrice_p(int value)
        {
            iPrice_p = value;
        }
        
        public int getiPrice_p()
        {
            return iPrice_p;
        }
        
        public void setiPrice_t(int value)
        {
            iPrice_t = value;
        }
        
        public int getiPrice_t()
        {
            return iPrice_t;
        }
        
        public void setsIppidName(String value)
        {
            sIppidName = value;
        }
        
        public String getsIppidName()
        {
            return sIppidName;
        }

       
    }
    
    public class osdDisplayStatus
    {
        private int iAvailBit;/*显示方式有效定义*/
        private int iLoopNum; /*滚动字幕的完整显示次�*/
        private int iLoopInterval;/*字幕不同显示次数之间的时间间�*/
        private int iLoopSpeed;/*字幕显示速度*/
        private int iOsdLx;/*字幕显示区域左上角坐标的X值（最�?920�*/
        private int iOsdLy;/*字幕显示区域左上角坐标的Y值（最�?280�*/
        private int iOsdRx;/*字幕显示区域右下角坐标的X值（最�?920�*/
        private int iOsdRy;/*字幕显示区域右下角坐标的Y值（最�?280�*/
        private int iFontColor;/*字幕文字颜色*/
        private int iFontSize;/*字幕的字�*/
        private int iBackColor;/*字幕背景颜色*/
        
        private void show()
        {
            LogUtils.printLog(1, 3, TAG, "--------OSD Screen Show!!-----------");
            LogUtils.printLog(1, 3, TAG, "--->>> iAvailBit  " + iAvailBit);
            LogUtils.printLog(1, 3, TAG, "--->>> iLoopNum  " +iLoopNum);
            LogUtils.printLog(1, 3, TAG, "--->>> iLoopInterval  " + iLoopInterval);
            LogUtils.printLog(1, 3, TAG, "--->>> iLoopSpeed  "+iLoopSpeed);
            LogUtils.printLog(1, 3, TAG, "--->>> iOsdLx  " + iOsdLx);
            LogUtils.printLog(1, 3, TAG, "--->>> iOsdLy  " + iOsdLy);
            LogUtils.printLog(1, 3, TAG, "--->>> iOsdRx  "+ iOsdRx);
            LogUtils.printLog(1, 3, TAG, "--->>> iOsdRy  " +iOsdRy);
            LogUtils.printLog(1, 3, TAG, "--->>> iFontColor  " + iFontColor);
            LogUtils.printLog(1, 3, TAG, "--->>> iFontSize  " + iFontSize);
            LogUtils.printLog(1, 3, TAG, "--->>> iBackColor  " + iBackColor);
        }
    }
    
    public class smartCardInfo implements Serializable
    {
    	public int iScStatus;
    	public int iUsedStartus;
    	public int iSysId;
    	public int iEcmSysId;
    	public int iEmmSysId;
    	public int iParentRating;
    	public int iGroupCtrl;
    	public int iAntiMoveCtrl;
    	public int iSCType;
    	public int iSCMatch;
    	public int iSTBMatch;
    	public String sCardNum;
    	public String sCardVer;
    	public String sCardType;
    	public String sIsSueTime;
    	public String sExpireTime;
    	public int iWordPeriod;
    	public long lMoutherUa;
    	public int iStartYear;
			public int iStartMonth;
			public int iStartDay;
			public int iStartHour;
			public int iStartMinute;
			public int iStartSecond;
    	//public String sVerifyStartTime;
        
        public void setsCardNum(String value)
        {
            sCardNum = value;
        }
        
        public void setsCardVer(String value)
        {
            sCardVer = value;
        }
        
        public void setsCardType(String value)
        {
            sCardType = value;
        }
        
        public void setsIsSueTime(String value)
        {
            sIsSueTime = value;
        }
        
        public void setsExpireTime(String value)
        {
            sExpireTime = value;
        }        
        
        public void logcat()
        {
            Log.i(TAG, "smartCardInfo:");
            Log.i(TAG, "iScStatus:" + iScStatus);
            Log.i(TAG, "iUsedStartus:" + iUsedStartus);
            Log.i(TAG, "iSysId:" + iSysId);
            Log.i(TAG, "iEcmSysId:" + iEcmSysId);
            Log.i(TAG, "iEmmSysId:" + iEmmSysId);
            Log.i(TAG, "iParentRating:" + iParentRating);
            Log.i(TAG, "iGroupCtrl:" + iGroupCtrl);
            Log.i(TAG, "iAntiMoveCtrl:" + iAntiMoveCtrl);
            Log.i(TAG, "iSCType:" + iSCType);
            Log.i(TAG, "iSCMatch:" + iSCMatch);
            Log.i(TAG, "iSTBMatch:" + iSTBMatch);
            Log.i(TAG, "sCardNum:" + sCardNum);
            Log.i(TAG, "sCardVer:" + sCardVer);
            Log.i(TAG, "sCardType:" + sCardType);
            Log.i(TAG, "sIsSueTime:" + sIsSueTime);
            Log.i(TAG, "sExpireTime:" + sExpireTime);
            if (iSCType == 1)
            {
                Log.i(TAG, "tis is childcard:");
                Log.i(TAG, "iWordPeriod:" + iWordPeriod);
                Log.i(TAG, "lMoutherUa:" + lMoutherUa);
                Log.i(TAG, "sVerifyStartTime:" + iStartYear + "-" +iStartMonth + "-" +iStartDay + " | " +iStartHour + ":" +iStartMinute + ":" +iStartSecond  );
            }
            
        }
        
    }
    
    public class ppidListInfo
    {
        private int iCount;
        ArrayList<ppidInfo> pListInfo = null;
        
        public void clearListInfo()
        {
            pListInfo.clear();
        }
        
        public void setpListInfo(ppidInfo pInfo)
        {
        	if(pListInfo == null)
        	{
        		pListInfo = new ArrayList<ppidInfo>();
        	}
            pListInfo.add(pInfo);
        }
    }
    
    public class ppidInfo implements Serializable
    {
    	public int iPPid;
    	public int iUsedStatus;
    	public int iCreaditDeposit;
    	public int[] iProgRight = null;
    	public String sCreditDate;
    	public String sLabel;
    	public String sStartDate;
    	public String sEndDate;
        
        public ppidInfo() {
			super();
			// TODO Auto-generated constructor stub
			iProgRight = new int[8];
		}

		public void setsCreditDate(String value)
        {
            sCreditDate = value;
        }
        
        public void setsLabel(String value)
        {
            sLabel = value;
        }
        
        public void setsStartDate(String value)
        {
            sStartDate = value;
        }
        
        public void setsEndDate(String value)
        {
            sEndDate = value;
        }
        
        public void setsiProgRight(int i, int value)
        {
        	if(iProgRight == null)
        	{
        		iProgRight = new int[8];
        	}
            iProgRight[i] = value;
        }
    }
    
    public class userViewInfo implements Serializable
    {
        public int iType;
        public int iPinCode;
        public int iFlag;
        public int iStartChannel;
        public int iEndChannel;
        public int iStatus;
        public int iStartYear;
        public int iStartMonth;
        public int iStartDay;
        public int iStartHour;
        public int iStartMinute;
        public int iStartSecond;
        public int iEndYear;
        public int iEndMonth;
        public int iEndDay;
        public int iEndHour;
        public int iEndMinute;
        public int iEndSecond;
    }
    
    public class proSkipInfo
    {
        private int iProNum; /*如果信息中的prognumber与当前播放节目不同，表示命令为过期命�*/
        private int iStatus; /* 0：强制跳�?2：用户选择跳转，根据用户选择，执行节目切换，同时显示字符串内�*/
        private int iType; /* 0 按频道跳�?1 ：按频点跳转*/
        private int iNetidOrFrq; /*type = 0 net_work_id  type =1 频点频率，单位MHZ*/
        private int iTsidOrMod;/*type = 0 ts_id type =1  调制方式*/
        private int iSrvidOrSyb;/*type = 0 service_id type =1  符号率，单位ksps*/
        private int iEsId;/*type = 0 es_id通常�?  type =1  service_id*/
        private int iMsgLen;
        private String sMsg; /*字符串内�*/
        
        public String getsMsg()
        {
            return sMsg;
        }

        public int getiProNum()
        {
            return iProNum;
        }

        public int getiStatus()
        {
            return iStatus;
        }

        public int getiType()
        {
            return iType;
        }

        public int getiNetidOrFrq()
        {
            return iNetidOrFrq;
        }

        public int getiTsidOrMod()
        {
            return iTsidOrMod;
        }

        public int getiSrvidOrSyb()
        {
            return iSrvidOrSyb;
        }

        public int getiEsId()
        {
            return iEsId;
        }

        public void setsMsg(String value)
        {
            sMsg = value;
        }
        
        public void logcat()
        {
            Log.i(TAG, "proSkipInfo:" );
            Log.i(TAG, "iProNum:" + iProNum );
            Log.i(TAG, "iStatus:" + iStatus );
            Log.i(TAG, "iType:" + iType );
            Log.i(TAG, "iNetidOrFrq:" + iNetidOrFrq );
            Log.i(TAG, "iTsidOrMod:" + iTsidOrMod );
            Log.i(TAG, "iSrvidOrSyb:" + iSrvidOrSyb );
            Log.i(TAG, "iEsId:" + iEsId );
            Log.i(TAG, "iMsgLen:" + iMsgLen );
            Log.i(TAG, "sMsg:" + sMsg );
        }
    }
    
    public class ppvnumber
    {
    	public int iStartNum;
    	public int iEndNum;
    }
    
    public class ppvlistinfo implements Serializable
    {
    	public int iPage;
    	public int iProgCount;
        
        public ArrayList<ppvnumber> aPPvNum = null;
        
        public void setaPPvNum(ppvnumber ppvNum)
        {
        	if(aPPvNum == null)
        	{
        		aPPvNum = new ArrayList<ppvnumber>();
        	}
            aPPvNum.add(ppvNum);
        }
    }
    
    public class ippvlistinfo implements Serializable
    {
    	public int iPage;
    	public int iProgCount;
    	public int[] iIppvNum = null;
        
        public void setiIppvNum(int i, int value)
        {
        	if(iIppvNum == null)
        	{
        		iIppvNum = new int[32];
        	}
            iIppvNum[i] = value;
        }
    }
    
    private Handler handler;
    private Context context;
    public static final int SET_PARENTAL_QUERY_REPLY= 22; /* 设置父母锁控制级别请求应�*/
    public static final int CHECK_PIN_QUERY_REPLY = 5;
    public static final int CHANGE_PIN_QUERY_REPLY =6;
    public static final int SC_INFO_NOTIFY = 12;
    public static final int USER_VIEW_INFO_NOTIFY = 34;
    
	public static final int  SUBTITLE_NOTIFY = 0x12; //subtitle osd  status = 0 custom style, status= 1 system style.
	public static final int  PPID_NOTIFY = 0x0d;
	public static final int  PPID_INFO_NOTIFY = 0x0e;
	public static final int  PROG_SKIP_NOTIFY = 0x2a;
	public static final int  PPV_LIST_INFO_NOTIFY = 0x2d; //ippv message,
	public static final int  IPPV_LIST_INFO_NOTIFY = 0x2e;//ippv message,
	public static final int  IPPV_QUERY_REPLY = 0x08; //ippv purchase message.

    public void setHandler(Handler handler, Context context)
    {
        this.handler = handler;
        this.context = context;
    }
    
    public MessageCa()
    {
        super();
    }
    
    public MessageCa(Handler handler, Context context)
    {
        super();
        this.handler = handler;
        this.context = context;
    }
    
    public void CaServiceMessage(int Code, int ProgramNum, int Index)
    {
        Log.i(TAG, "--------------CA Message has benn Here!!!--------------------" + Code);
        switch (Code)
        {
            case 0:
                sendEmptyMessage(Config.CTI_CA_E00);
                break;
            case 1:
                sendEmptyMessage(Config.CTI_CA_E01);
                break;
            case 2:
                sendEmptyMessage(Config.CTI_CA_E02);
                break;
            case 3:
                sendEmptyMessage(Config.CTI_CA_E03);
                break;
            case 4:
                sendEmptyMessage(Config.CTI_CA_E04);
                break;
            case 5:
                sendEmptyMessage(Config.CTI_CA_E05);
                break;
            case 6:
                sendEmptyMessage(Config.CTI_CA_E06);
                break;
            case 7:
                sendEmptyMessage(Config.CTI_CA_E07);
                break;
            case 8:
                sendEmptyMessage(Config.CTI_CA_E08);
                break;
            case 10:
                sendEmptyMessage(Config.CTI_CA_E10);
                break;
            case 11:
                sendEmptyMessage(Config.CTI_CA_E11);
                break;
            case 12:
                sendEmptyMessage(Config.CTI_CA_E12);
                break;
            case 13:
                sendEmptyMessage(Config.CTI_CA_E13);
                break;
            case 14:
                sendEmptyMessage(Config.CTI_CA_E14);
                break;
            case 15:
                sendEmptyMessage(Config.CTI_CA_E15);
                break;
            case 16:
                sendEmptyMessage(Config.CTI_CA_E16);
                break;
            case 17:
                sendEmptyMessage(Config.CTI_CA_E17);
                break;
            case 18:
                sendEmptyMessage(Config.CTI_CA_E18);
                break;
            case 19:
                sendEmptyMessage(Config.CTI_CA_E19);
                break;
            case 20:
                sendEmptyMessage(Config.CTI_CA_E20);
                break;
            case 21:
                sendEmptyMessage(Config.CTI_CA_E21);
                break;
            case 22:
                sendEmptyMessage(Config.CTI_CA_E22);
                break;
            case 23:
                sendEmptyMessage(Config.CTI_CA_E23);
                break;
            case 24:
                sendEmptyMessage(Config.CTI_CA_E24);
                break;
            case 25:
                sendEmptyMessage(Config.CTI_CA_E25);
                break;
            case 26:
                sendEmptyMessage(Config.CTI_CA_E26);
                break;
            case 27:
                sendEmptyMessage(Config.CTI_CA_E27);
                break;
            case 28:
                sendEmptyMessage(Config.CTI_CA_E28);
                break;
            case 29:
                sendEmptyMessage(Config.CTI_CA_E29);
                break;
            case 30:
                sendEmptyMessage(Config.CTI_CA_E30);
                break;
            case 40:
                sendEmptyMessage(Config.CTI_CA_E40);
                break;
            case 41:
                sendEmptyMessage(Config.CTI_CA_E41);
                break;
            case 42:
                sendEmptyMessage(Config.CTI_CA_E42);
                break;
            case 43:
                sendEmptyMessage(Config.CTI_CA_E43);
                break;
            case 44:
                sendEmptyMessage(Config.CTI_CA_E44);
                break;
            case 45:
                sendEmptyMessage(Config.CTI_CA_E45);
                break;
            case 46:
                sendEmptyMessage(Config.CTI_CA_E46);
                break;
            case 47:
                sendEmptyMessage(Config.CTI_CA_E47);
                break;
            case 50:
                sendEmptyMessage(Config.CTI_CA_E50);
                break;
            case 51:
                sendEmptyMessage(Config.CTI_CA_E51);
                break;
            case 52:
                sendEmptyMessage(Config.CTI_CA_E52);
                break;
            case 53:
                sendEmptyMessage(Config.CTI_CA_E53);
                break;
            case 54:
                sendEmptyMessage(Config.CTI_CA_E54);
                break;
            case 55:
                sendEmptyMessage(Config.CTI_CA_E55);
                break;
            case 56:
                sendEmptyMessage(Config.CTI_CA_E56);
                break;
            case 57:
                sendEmptyMessage(Config.CTI_CA_E57);
                break;
        }
    }
    
    public void CaStatusMessage(int type, int status)
    {
        Log.i(TAG, "--->>CaStatusMessage--------------------type:" + type + " status :" + status);
        switch (type)
        {
            case PROG_SKIP_NOTIFY:
                Message switchMsg = new Message();
                switchMsg.what = Config.CA_FORCE_SWITCH;
                Bundle bundle = new Bundle();
                bundle.putInt("ca_force_status", 1);//status = 1;
                switchMsg.obj = bundle;
                handler.sendMessage(switchMsg);
                break;
            
            case IPPV_QUERY_REPLY:
                Message ippMsg = new Message();
                ippMsg.what = Config.CA_IPPV_QUERY_REPLY;
                ippMsg.obj = status;
                sendMessage(ippMsg);
                break;
            
            case SET_PARENTAL_QUERY_REPLY://成人级别
                LogUtils.printLog(1, 3, TAG, "SET_PARENTAL_QUERY_REPLY"+SET_PARENTAL_QUERY_REPLY);
                Message parentalMsg = new Message();
                parentalMsg.what = Config.CA_PARENTAL_REPLY;
                parentalMsg.obj = status;
                parentalMsg.arg1 = status;
                sendMessage(parentalMsg);
                break;
            case CHECK_PIN_QUERY_REPLY://校验密码
                LogUtils.printLog(1, 3, TAG, "CHECK_PIN_QUERY_REPLY"+CHECK_PIN_QUERY_REPLY);;
                Message pinCheck = new Message();
                pinCheck.what = Config.CA_MSG_PIN_CHECK;
                pinCheck.arg1 = status;
                sendMessage(pinCheck);
                break;
            case CHANGE_PIN_QUERY_REPLY://修改密码
                LogUtils.printLog(1, 3, TAG, "CHANGE_PIN_QUERY_REPLY"+CHANGE_PIN_QUERY_REPLY);
                Message pinChange = new Message();
                pinChange.what = Config.CA_MSG_PIN_CHANGE;
                pinChange.arg1 = status;
                sendMessage(pinChange);
                break;
                
            case SC_INFO_NOTIFY:
                LogUtils.printLog(1, 3, TAG, "SC_INFO_NOTIFY"+SC_INFO_NOTIFY);
                Message scMsg = new Message();
                scMsg.what = Config.CA_MSG_CARD_Status;
                scMsg.arg1 = status;
                sendMessage(scMsg);
                break;
            case USER_VIEW_INFO_NOTIFY:
                LogUtils.printLog(1, 3, TAG, "USER_VIEW_INFO_NOTIFY"+USER_VIEW_INFO_NOTIFY);
                Message viewMsg = new Message();
                viewMsg.what = Config.CA_MSG_PLAY_CONTROL_MODIFY;
                viewMsg.arg1 = status;
                sendMessage(viewMsg);
                break;
            case PPID_NOTIFY:
                LogUtils.printLog(1, 3, TAG, "PPID_NOTIFY"+PPID_NOTIFY);
                Message ppidNotify = new Message();
                ppidNotify.what = Config.CA_MSG_PPID_NOTIFY;
                ppidNotify.arg1 = status;
                sendMessage(ppidNotify);
            	
            	break;
            case PPID_INFO_NOTIFY:
                LogUtils.printLog(1, 3, TAG, "PPID_INFO_NOTIFY"+PPID_INFO_NOTIFY);
                Message ppidDetailNotify = new Message();
                ppidDetailNotify.what = Config.CA_MSG_PPID_DETAIL_NOTIFY;
                ppidDetailNotify.arg1 = status;
                sendMessage(ppidDetailNotify);
            	
            	break;
        }
    }
    
/**
 * 
 * <控制OSD屏显方式>
 * <功能详细描述>
 * @param osdDisplay
 * @see [类、类#方法、类#成员]
 */
    public void CaOsdDisplayMessage(osdDisplayStatus osdDisplay)
    {
        Log.i(TAG, "--->>CaOsdDisplayMessage--------------------");
        LogUtils.printLog(1, 5, TAG, "****************************************************************");
        LogUtils.printLog(1, 5, TAG, "****************************************************************");
        LogUtils.printLog(1, 5, TAG, "******************OSD Display!!**********************");
        LogUtils.printLog(1, 5, TAG, "****************************************************************");
        LogUtils.printLog(1, 5, TAG, "****************************************************************");
        LogUtils.printLog(1, 5, TAG, "****************************************************************");
        osdDisplay.show();
    }
    
    /**
     * 
     * <智能卡信�?
     * <功能详细描述>
     * 显示智能卡基本信息内�?
     * @param scinfo
     * @see [类、类#方法、类#成员]
     */
    public void CaSCInfoMessage(smartCardInfo scinfo)
    {
        Log.i(TAG, "--->>CaSCInfoMessage--------------------");
        scinfo.logcat();

    	Bundle bundle = new Bundle();
    	bundle.putSerializable("scinfo",scinfo);
    	
    	Message msg = new Message();
    	msg.what = Config.CA_MSG_STATE_INFO;
    	msg.obj = bundle;
    	
    	sendMessage(msg);
    }
    
    /**
     * 
     * <智能卡内PPID信息>
     * <功能详细描述>
     * @param pList
     * @see [类、类#方法、类#成员]
     */
    public void CaPPidListMessage(ppidListInfo pList)
    {
        Log.i(TAG, "--->>CaPPidListMessage--------------------");
        Message msg = new Message();
        msg.what = Config.CA_MSG_PPID_LIST;    	
    	msg.arg1 = pList.iCount;
    	
    	if(pList.iCount != 0){
    		Bundle bundle = new Bundle();
        	
        	ArrayList<ppidInfo> list = pList.pListInfo;
    		Iterator<ppidInfo> itr = list.iterator();
        	int key = 2000;
        	while(itr.hasNext()){
            	bundle.putSerializable(""+ key, itr.next());
            	key++;
        	}
        	msg.obj = bundle;
    	}
    	
    	sendMessage(msg);
    }
    
    /**
     * 
     * <智能卡PPID详细信息>
     * <功能详细描述>
     * @param pList
     * @see [类、类#方法、类#成员]
     */
    public void CaPPidInfoMessage(ppidInfo pList)
    {
        Log.i(TAG, "--->>CaPPidInfoMessage--------------------");

		Bundle bundle = new Bundle();
		bundle.putSerializable("pList", pList);

		Message msg = new Message();
		msg.what = Config.CA_MSG_PPID_DETAIL;
		msg.obj = bundle;
		sendMessage(msg);
    }
    
    /**
     * 
     * <PPV信息>
     * <功能详细描述>
     * @param ppvlist
     * @see [类、类#方法、类#成员]
     */
    public void CaPPVListMessage(ppvlistinfo ppvlist)
    {
        Log.i(TAG, "--->>CaPPVListMessage--------------------");
        Log.i(TAG, "--->>CaPPVListMessage--------------------"+ ppvlist.iPage + "|" + ppvlist.iProgCount );
		Bundle bundle = new Bundle();
		bundle.putSerializable("ppvList", ppvlist);

		Message msg = new Message();
		msg.what = Config.CA_MSG_PPV_LIST;
		msg.obj = bundle;
		sendMessage(msg);
    }
    
    /**
     * 
     * <IPPV信息>
     * <功能详细描述>
     * @param ippvlist
     * @see [类、类#方法、类#成员]
     */
    public void CaIPPVListMessage(ippvlistinfo ippvlist)
    {
        Log.i(TAG, "--->>CaIPPVListMessage--------------------");

		Bundle bundle = new Bundle();
		bundle.putSerializable("ippvList", ippvlist);
		if(ippvlist.iIppvNum == null)
		{
			Log.i(TAG, "--->>iIppvNum is null--------------------");
		}
		else
		{
			Log.i(TAG, "--->>iIppvNum:length" + ippvlist.iIppvNum.length);
			Log.i(TAG, "--->>iIppvNum:" + ippvlist.iIppvNum[0]);
		}
		Message msg = new Message();
		msg.what = Config.CA_MSG_IPPV_LIST;
		msg.obj = bundle;
		sendMessage(msg);
    }
    
    
    /**
     * 
     * <邮件Bmail>
     * <功能详细描述>
      * �?机顶盒收到新的Bmail后应在屏幕的右上角或左上角显示新邮件提示标识，通知用户收到邮件
     * �?如果邮件箱中的邮件都已阅读，屏幕上的新邮件提示标识应被清除；
     * ③机顶盒应正确完整地显示Bmail内容，包括邮件发送者、主题、内容等。能够支持常见的中英文字符、标点符号�?数字序号，支持换行、翻页、英文的分词等；
    
     * @param mail
     * @see [类、类#方法、类#成员]
     */
    public void emailNotify(maildata mail)
    {
        Log.i(TAG, "--->>emailNotify--------------------");
        MailDao mDao = new MailDaoImpl(context);
        
        MailBean mailBean = new MailBean();
        mailBean.setMailType(mail.getbType());
        mailBean.setMailTitle(mail.getbMsgTitle());
        mailBean.setMailTime(mail.getMailTime());
        mailBean.setMailContent(mail.getbMsgData());
        mailBean.setMailFrom(mail.getbMsgFrom());
        
        int status = mail.getbStatus();
        if (status == 1)
        {
            mDao.addMailInfo(mailBean);
            sendEmptyMessage(Config.CA_MAIL);
        }
    }
    
    /**
     * 
     * <IPP节目购买>
     * <功能详细描述>
     * @param ippdata
     * @see [类、类#方法、类#成员]
     */
    public void ippNotify(ippnotifymsg ippdata)
    {
        Log.i(TAG, "--->>ippNotify--------------------");
        Bundle data = new Bundle();
        data.putSerializable("ippv_data", ippdata);
        
        Message ippMsg = new Message();
        ippMsg.what = Config.CA_IPP_NOTICE;
        ippMsg.obj = data;
        sendMessage(ippMsg);
        
    }
    
    /**
     * 
     * <准禁播控�?
     * <功能详细描述>
     * @see [类、类#方法、类#成员]
     */
	public void CaUseViewMessage(userViewInfo uvinfo) {
		Log.i(TAG, "--->>CaUseViewMessage--------------------");
		LogUtils.printLog(1, 3, TAG, "CaUseViewMessage");

		Bundle bundle = new Bundle();
		bundle.putSerializable("uvinfo", uvinfo);

		Message msg = new Message();
		msg.what = Config.CA_MSG_PLAY_CONTROL_QUERY;
		msg.obj = bundle;

		sendMessage(msg);
	}
    
    /**
     * 
     * <频道跳转控制>
     * <功能详细描述>
     * 1.频道跳转方式分为两种：①按频道跳转；②按频点跳转
     * 2.根据命令中的参数，执行方式分为：
     *  ①强制执行，机顶盒切换到指定
     * @param pSkip
     * @see [类、类#方法、类#成员]
     */
    public void enhancechNotify(proSkipInfo pSkip)
    {
        Log.i(TAG, "--->>enhancechNotify--------------------");
        pSkip.logcat();
        
        Message switchMsg = new Message();
        switchMsg.what = Config.CA_FORCE_SWITCH;
        Bundle bundle = new Bundle();
        bundle.putInt("ca_logical_number", pSkip.getiProNum());
        bundle.putInt("ca_netid_or_freq", pSkip.getiNetidOrFrq());
        bundle.putInt("ca_tsid_or_mod", pSkip.getiTsidOrMod());
        bundle.putInt("ca_serviceid_or_symb", pSkip.getiSrvidOrSyb());
        bundle.putString("ca_message", pSkip.getsMsg());
        
        int status = pSkip.getiStatus();
        int type = pSkip.getiType();
        
        bundle.putInt("ca_force_status", status);
        // 强制跳转
        if (status == 0)
        {
            LogUtils.printLog(1, 3, TAG, "--->>>Ca Enhance Switch!!------");
            if (type == 0)
            {
                bundle.putInt("ca_force_type", 0);
                switchMsg.obj = bundle;
                sendMessage(switchMsg);
            }
            else
            {
                bundle.putInt("ca_force_type", 1);
                bundle.putInt("ca_force_esid", pSkip.getiEsId());
                switchMsg.obj = bundle;
                sendMessage(switchMsg);
            }
        }
        // 用户选择跳转
        else if (status == 2)
        {
            LogUtils.printLog(1, 3, TAG, "--->>>Ca User Choose Switch!!------");
            if (type == 0)
            {
                bundle.putInt("ca_force_type", 0);
                switchMsg.obj = bundle;
                sendMessage(switchMsg);
            }
            else
            {
                bundle.putInt("ca_force_type", 1);
                bundle.putInt("ca_force_esid", pSkip.getiEsId());
                switchMsg.obj = bundle;
                sendMessage(switchMsg);
            }
        }
    }

    /**
     * 
     * <指纹显示>
     * <功能详细描述>
     * @param type
     * @param str
     * @see [类、类#方法、类#成员]
     */
    public void FpNotify(int type, String str)
    {
        Log.i(TAG, "--->>FpNotify--------------------type:" + type + "str:" + str);
        Message fpMsg = new Message();
        Bundle bundle = new Bundle();
        bundle.putInt("cti_fp_type", type);
        bundle.putString("cti_fp_str", str);
        fpMsg.obj = bundle;
        fpMsg.what = Config.CA_FP;
        sendMessage(fpMsg);
    }
    
    public void osdNotify(maildata mail)
    {
        Log.i(TAG, "--------------osdNotify--------");
        //OSD 数据有效
        if (mail.getbStatus() == 1)
        {
            Message osdMsg = new Message();
            osdMsg.what = Config.CA_OSD_SHOW;
            osdMsg.obj = mail.getbMsgData();
            sendMessage(osdMsg);
        }
    }
    
    public class ipppurchasedata
    {
    }
    
    /* 判断是否需要显示到期提�?*/
    /* 返回值定�? 0,不需要显示；1:需要显示； */
    public native int CheckReminder();
    
    /* 购买IPP节目,0正确 */
    public native int PurchaseIpp(ipppurchasedata data, String pin);
    
    /* 从母卡中读取喂养数据，用于喂养流�?*/
    /* 返回�?0 成功，非0，失�?*/
    public native int ReadFeedDataFromMaster();
    
    /* 将喂养数据写入子�?*/
    /* 返回�?0 成功，非0，失�?*/
    public native int WriteFeedDataToSlaver();
    
    /* 得到当前的ca的errcode,对应E00~E36 */
    public native int GetLastErrcode();
    
    /* 上层传入当前的状�?*/
    public native int SetReminderStatus(int status);
    
    private void sendEmptyMessage(int what)
    {
        if (null == handler)
        {
            LogUtils.printLog(1, 5, TAG, "--->>> handler is null !!");
        }
        else
        {
            handler.sendEmptyMessage(what);
        }
    }
    
    private void sendMessage(Message msg)
    {
        if (null == handler)
        {
            LogUtils.printLog(1, 5, TAG, "--->>> handler is null !!");
        }
        else
        {
            handler.sendMessage(msg);
        }
    }
    
}

