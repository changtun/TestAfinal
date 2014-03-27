package com.pbi.dvb.dvbinterface;

public class NativeCA
{
    public static final int CA_CLIENT_CAT_QUERY = 0x02;
    public static final int CA_CLIENT_PMT_QUERY = 0x03;
    public static final int CA_CLIENT_CHANGE_PROG = 0x07;
    public static final int CA_CLIENT_STOP_PROG_QUERY = 0x04;
    public static final int CA_CLIENT_CHECK_PIN_QUERY = 0x05;
    public static final int CA_CLIENT_CHANGE_PIN_QUERY = 0x06;
    public static final int CA_CLIENT_IPPV_QUERY = 0x08;
    public static final int CA_CLIENT_SC_INFO_QUERY = 0x0c;
    public static final int CA_CLIENT_PPID_QUERY = 0x0f;
    public static final int CA_CLIENT_PPID_INFO_QUERY = 0x0e;
    public static final int CA_CLIENT_PPV_LIST_INFO_QUERY = 0x14;
    public static final int CA_CLIENT_ANTI_RECORD_QUERY = 0x15;
    public static final int CA_CLIENT_SET_PARENTAL_QUERY = 0x16;
    public static final int CA_CLIENT_USER_VIEW_QUERY = 0x19;
    public static final int CA_CLIENT_PARENTAL_CHANGE_QUERY = 0x1a;
    public static final int CA_CLIENT_NIT_QUERY = 0x0a;
    public static final int CA_CLIENT_CHANGE_PID_QUERY = 0x0d;
    
    public class CaData /**/
    {
        private String SCnumber;/* 智能卡号 */
        private String SCversion;/* 智能卡版本号 */
        private String CAcellVersion;/* ca版本 */
        private int rate;/* 可观看级�?*/
        
        public void setSCnumber(String value)
        {
            SCnumber = value;
        }
        
        public String getSCnumber()
        {
            return SCnumber;
        }
        
        public void setSCversion(String value)
        {
            SCversion = value;
        }
        
        public String getSCversion()
        {
            return SCversion;
        }
        
        public void setCAcellVersion(String value)
        {
            CAcellVersion = value;
        }
        
        public String getCAcellVersion()
        {
            return CAcellVersion;
        }
        
        public void setrate(int value)
        {
            rate = value;
        }
        
        public int getrate()
        {
            return rate;
        }
        
    }
    
    public class caIppProduct/* IPP 授权信息 */
    {
        private char prodcuttype;/* 0:ippv,1:ippt */
        private int ulRunningNum; /* 流水�?*/
        private short startYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private char startMonth; /* 1 to 12 */
        private char startDay; /* 1 to 31 */
        private char startHour; /* 0 to 23 */
        private char startMinute; /* 0 to 59 */
        private char startSecond; /* 0 to 59 */
        private short endYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private char endMonth; /* 1 to 12 */
        private char endDay; /* 1 to 31 */
        private char endHour; /* 0 to 23 */
        private char endMinute; /* 0 to 59 */
        private char endSecond; /* 0 to 59 */
        
        public void setprodcuttype(char value)
        {
            prodcuttype = value;
        }
        
        public char getprodcuttype()
        {
            return prodcuttype;
        }
        
        public void setulRunningNum(int value)
        {
            ulRunningNum = value;
        }
        
        public int getulRunningNum()
        {
            return ulRunningNum;
        }
        
        public void setstartYear(short value)
        {
            startYear = value;
        }
        
        public short getstartYear()
        {
            return startYear;
        }
        
        public void setstartMonth(char value)
        {
            startMonth = value;
        }
        
        public char getstartMonth()
        {
            return startMonth;
        }
        
        public void setstartDay(char value)
        {
            startDay = value;
        }
        
        public char getstartDay()
        {
            return startDay;
        }
        
        public void setstartHour(char value)
        {
            startHour = value;
        }
        
        public char getstartHour()
        {
            return startHour;
        }
        
        public void setstartMinute(char value)
        {
            startMinute = value;
        }
        
        public char getstartMinute()
        {
            return startMinute;
        }
        
        public void setstartSecond(char value)
        {
            startSecond = value;
        }
        
        public char getstartSecond()
        {
            return startSecond;
        }
        
        public void setendYear(short value)
        {
            endYear = value;
        }
        
        public short getendYear()
        {
            return endYear;
        }
        
        public void setendMonth(char value)
        {
            endMonth = value;
        }
        
        public char getendMonth()
        {
            return endMonth;
        }
        
        public void setendDay(char value)
        {
            endDay = value;
        }
        
        public char getendDay()
        {
            return endDay;
        }
        
        public void setendHour(char value)
        {
            endHour = value;
        }
        
        public char getendHour()
        {
            return endHour;
        }
        
        public void setendMinute(char value)
        {
            endMinute = value;
        }
        
        public char getendMinute()
        {
            return endMinute;
        }
        
        public void setendSecond(char value)
        {
            endSecond = value;
        }
        
        public char getendSecond()
        {
            return endSecond;
        }
        
    }
    
    public class IppProductList
    {
        public short totalnum;
        public caIppProduct[] ProList;
        
        public void settotalnum(short value)
        {
            totalnum = value;
        }
        
        public short gettotalnum()
        {
            return totalnum;
        }
        
        public caIppProduct[] getIppProList()
        {
            return ProList;
        }
        
        public void setIppProList(caIppProduct[] infoList)
        {
            this.ProList = infoList;
        }
        
        public void BulidArrIPP(short number, IppProductList list)
        {
            int ii = 0;
            
            System.out.println("BulidArrIPP===========" + number + "====");
            
            caIppProduct[] Infos = new caIppProduct[number];
            
            list.setIppProList(Infos);
        }
        
        public void BulidIPPlist(short number, IppProductList list)
        {
            int ii = 0;
            
            System.out.println("===========" + number + "====");
            
            for (ii = 0; ii < number; ii++)
            {
                list.ProList[ii] = new caIppProduct();
            }
        }
        
    }
    
    public class caIPPRecord /* IPP 消费纪录，对应IPPV和IPPT */
    {
        private char bStateFlag;/* Charging or consume flag, 1:存入; 0:支出. */
        private short ulExchbYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private char ulExchbMonth; /* 1 to 12 */
        private char ulExchbDay; /* 1 to 31 */
        private char ulExchbHour; /* 0 to 23 */
        private char ulExchbMinute; /* 0 to 59 */
        private char ulExchbSecond; /* 0 to 59 */
        private int ulExchRunningNum; /* 流水�?*/
        private int ulExchValue; /* 金额 */
        private char bContentLen; /* 内容信息，菜单不用显�?*/
        private String pbContent; /* 内容信息，菜单不用显�?*/
        
        public void setbStateFlag(char value)
        {
            bStateFlag = value;
        }
        
        public char getbStateFlag()
        {
            return bStateFlag;
        }
        
        public void setulExchbYear(short value)
        {
            ulExchbYear = value;
        }
        
        public short getulExchbYear()
        {
            return ulExchbYear;
        }
        
        public void setulExchbMonth(char value)
        {
            ulExchbMonth = value;
        }
        
        public char getulExchbMonth()
        {
            return ulExchbMonth;
        }
        
        public void setulExchbDay(char value)
        {
            ulExchbDay = value;
        }
        
        public char getulExchbDay()
        {
            return ulExchbDay;
        }
        
        public void setulExchbHour(char value)
        {
            ulExchbHour = value;
        }
        
        public char getulExchbHour()
        {
            return ulExchbHour;
        }
        
        public void setulExchbMinute(char value)
        {
            ulExchbMinute = value;
        }
        
        public char getulExchbMinute()
        {
            return ulExchbMinute;
        }
        
        public void setulExchbSecond(char value)
        {
            ulExchbSecond = value;
        }
        
        public char getulExchbSecond()
        {
            return ulExchbSecond;
        }
        
        public void setulExchRunningNum(int value)
        {
            ulExchRunningNum = value;
        }
        
        public int getulExchRunningNum()
        {
            return ulExchRunningNum;
        }
        
        public void setulExchValue(int value)
        {
            ulExchValue = value;
        }
        
        public int getulExchValue()
        {
            return ulExchValue;
        }
        
        public void setbContentLen(char value)
        {
            bContentLen = value;
        }
        
        public char getbContentLen()
        {
            return bContentLen;
        }
        
        public void setpbContent(String value)
        {
            pbContent = value;
        }
        
        public String getpbContent()
        {
            return pbContent;
        }
        
    }
    
    public class IppRecordList
    {
        public short totalnum;
        public caIPPRecord[] RList;
        
        public void settotalnum(short value)
        {
            totalnum = value;
        }
        
        public short gettotalnum()
        {
            return totalnum;
        }
        
        public caIPPRecord[] getIppRList()
        {
            return RList;
        }
        
        public void setIppRList(caIPPRecord[] infoList)
        {
            this.RList = infoList;
        }
        
        public void BulidArrP(short number, IppRecordList list)
        {
            int ii = 0;
            
            System.out.println("BulidArrIPP===========" + number + "====");
            
            caIPPRecord[] Infos = new caIPPRecord[number];
            
            list.setIppRList(Infos);
        }
        
        public void BulidRlist(short number, IppRecordList list)
        {
            int ii = 0;
            
            System.out.println("===========" + number + "====");
            
            for (ii = 0; ii < number; ii++)
            {
                list.RList[ii] = new caIPPRecord();
            }
        }
        
    }
    
    public class caEntitle /* IPP 授权信息 */
    {
        private short startYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private char startMonth; /* 1 to 12 */
        private char startDay; /* 1 to 31 */
        private char startHour; /* 0 to 23 */
        private char startMinute; /* 0 to 59 */
        private char startSecond; /* 0 to 59 */
        private short endYear; /* for example 1998, bYear[0]=19, bYear[1]=98 */
        private char endMonth; /* 1 to 12 */
        private char endDay; /* 1 to 31 */
        private char endHour; /* 0 to 23 */
        private char endMinute; /* 0 to 59 */
        private char endSecond; /* 0 to 59 */
        private int pulRemain; /* 剩余时间 */
        private int index; /* 索引 */
        private int status;/*0:正常�?:参数错误�?:没有授权�?:授权受限 4:授权关闭,5:授权暂停。上层在处理的时候，不会�?,2两种值，不用显示*/
        
        public void setstartYear(short value)
        {
            startYear = value;
        }
        
        public short getstartYear()
        {
            return startYear;
        }
        
        public void setstartMonth(char value)
        {
            startMonth = value;
        }
        
        public char getstartMonth()
        {
            return startMonth;
        }
        
        public void setstartDay(char value)
        {
            startDay = value;
        }
        
        public char getstartDay()
        {
            return startDay;
        }
        
        public void setstartHour(char value)
        {
            startHour = value;
        }
        
        public char getstartHour()
        {
            return startHour;
        }
        
        public void setstartMinute(char value)
        {
            startMinute = value;
        }
        
        public char getstartMinute()
        {
            return startMinute;
        }
        
        public void setstartSecond(char value)
        {
            startSecond = value;
        }
        
        public char getstartSecond()
        {
            return startSecond;
        }
        
        public void setendYear(short value)
        {
            endYear = value;
        }
        
        public short getendYear()
        {
            return endYear;
        }
        
        public void setendMonth(char value)
        {
            endMonth = value;
        }
        
        public char getendMonth()
        {
            return endMonth;
        }
        
        public void setendDay(char value)
        {
            endDay = value;
        }
        
        public char getendDay()
        {
            return endDay;
        }
        
        public void setendHour(char value)
        {
            endHour = value;
        }
        
        public char getendHour()
        {
            return endHour;
        }
        
        public void setendMinute(char value)
        {
            endMinute = value;
        }
        
        public char getendMinute()
        {
            return endMinute;
        }
        
        public void setendSecond(char value)
        {
            endSecond = value;
        }
        
        public char getendSecond()
        {
            return endSecond;
        }
        
        public void setpulRemain(int value)
        {
            pulRemain = value;
        }
        
        public int getpulRemain()
        {
            return pulRemain;
        }
        
        public void setindex(int value)
        {
            index = value;
        }
        
        public int getindex()
        {
            return index;
        }
        
        public void setstatus(int value)
        {
            status = value;
        }
        
        public int getstatus()
        {
            return status;
        }
        
    }
    
    public class IppEntitleList
    {
        public short totalnum;
        public caEntitle[] EList;
        
        public void settotalnum(short value)
        {
            totalnum = value;
        }
        
        public short gettotalnum()
        {
            return totalnum;
        }
        
        public caEntitle[] getEList()
        {
            return EList;
        }
        
        public void setEList(caEntitle[] infoList)
        {
            this.EList = infoList;
        }
        
        public void BulidArrE(short number, IppEntitleList list)
        {
            int ii = 0;
            
            System.out.println("BulidArrIPP===========" + number + "====");
            
            caEntitle[] Infos = new caEntitle[number];
            
            list.setEList(Infos);
        }
        
        public void BulidElist(short number, IppEntitleList list)
        {
            int ii = 0;
            
            System.out.println("===========" + number + "====");
            
            for (ii = 0; ii < number; ii++)
            {
                list.EList[ii] = new caEntitle();
            }
        }
        
    }
    
    public class caEpuseInfo /* 钱包信息 */
    {
        private int ulCashValue;
        private int ulCreditValue;
        private int wRecordIndex;
        
        public void setulCashValue(int value)
        {
            ulCashValue = value;
        }
        
        public int getulCashValue()
        {
            return ulCashValue;
        }
        
        public void setulCreditValue(int value)
        {
            ulCreditValue = value;
        }
        
        public int getulCreditValue()
        {
            return ulCreditValue;
        }
        
        public void setwRecordIndex(int value)
        {
            wRecordIndex = value;
        }
        
        public int getwRecordIndex()
        {
            return wRecordIndex;
        }
        
    }
    
    public class IppEpuseList
    {
        public short totalnum;
        public caEpuseInfo[] EPUList;
        
        public void settotalnum(short value)
        {
            totalnum = value;
        }
        
        public short gettotalnum()
        {
            return totalnum;
        }
        
        public caEpuseInfo[] getEList()
        {
            return EPUList;
        }
        
        public void setEList(caEpuseInfo[] infoList)
        {
            this.EPUList = infoList;
        }
        
        public void BulidArrE(short number, IppEpuseList list)
        {
            int ii = 0;
            
            System.out.println("BulidArrIPP===========" + number + "====");
            
            caEpuseInfo[] Infos = new caEpuseInfo[number];
            
            list.setEList(Infos);
        }
        
        public void BulidElist(short number, IppEpuseList list)
        {
            int ii = 0;
            
            System.out.println("===========" + number + "====");
            
            for (ii = 0; ii < number; ii++)
            {
                list.EPUList[ii] = new caEpuseInfo();
            }
        }
        
    }
    
	public class caPinCode
	{
		int oldCode;
		int newCode;
		public caPinCode(int oldCode, int newCode)
		{
			this.oldCode = oldCode;
			this.newCode = newCode;
		}
	}
	
	public class caIppvBuy
	{
		int iFLag;
		int iProgramNo;
		int pinCode;
		public caIppvBuy(int iFLag, int iProgramNo, int pinCode)
		{
			this.iFLag = iFLag;
			this.iProgramNo = iProgramNo;
			this.pinCode = pinCode;
		}
	}

	public class caPPidInFo
	{
		int iPinCode;
		int iPPID;
		public caPPidInFo(int iPinCode, int iPPID)
		{
			this.iPinCode = iPinCode;
			this.iPPID = iPPID;
		}
	}

	public class caParental
	{
		int iPinCode; /*卡密码*/
		int iParentalRate; 
		public caParental(int iPinCode, int iParentalRate)
		{
			this.iPinCode = iPinCode;
			this.iParentalRate = iParentalRate;
		}
	}

	public class caPPVList
	{
		int iType;
		int iPPID;
		int iPage;
		public caPPVList(int iType, int  iPage, int iPPID)
		{
			this.iType = iType;
			this.iPage = iPage;
			this.iPPID = iPPID;
		}
	}

	public class caUserView
	{
		public int iType;
		public int iPinCode;
		public int iFLag;
		public int iStatus;
		public int iStartCh;
		public int iEndCh;

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
    
    public native int CaMessageInit(MessageCa msgCA);
    
    /* CA 信息菜单 */
    /* 返回�?-1,取值错误，都不显示 */
    /* 返回�?-2,没有插入智能卡，但是ca库版本号可以读到 */
    public native int GetCaData(CaData data);/* 获取CA信息 包括卡号库版本号�?*/
    
    /* IPP 消费纪录，对应IPPV和IPPT */
    /* index: 0~99 */
    /* 返回�?-1,取值错误，读数据失�?*/
    /* 返回�?-2,没有插入智能卡，无法读取数据 */
    /* 返回�?0 成功 */
    public native int GetIPPRecord(IppRecordList ipp);/* 获取ipp 纪录 */
    
    /* IPP 授权信息 */
    /* index: 0~19 */
    /* 返回�?-1,取值错误，读数据失�?*/
    /* 返回�?-2,没有插入智能卡，无法读取数据 */
    /* 返回�?0 成功 */
    public native int GetIPPProduct(IppProductList ipp);/* 获取IPP */
    
    /* IPP 授权信息 */
    /* INDEX: 0~319 */
    /* 返回�?-1,取值错误，读数据失�?*/
    /* 返回�?-2,没有插入智能卡，无法读取数据 */
    /* 返回�?0 成功 */
    public native int GetEntitle(IppEntitleList eipp);/* 获取授权信息 */
    
    public native int GetEntitleByPage(IppEntitleList eipp, int pagenum, int getnumber);
    
    public native int GetEntitleTotals();
    
    /* 钱包信息 */
    /* 返回�?-1,取值错误，读数据失�?*/
    /* 返回�?-2,没有插入智能卡，无法读取数据 */
    /* 返回�?0 成功 */
    public native int GetEpuse(IppEpuseList ipp);/* 获取钱包信息 */
    
    /* 获取当前观看级别 */
    // 0 成功 -2: 未插入智能卡，其他值失�?
    /* level :0~10 */
    public native int SetRating(String pbPin, int bPinLen, int bRating);/* 修改观看级别 */
    
    public native int caParentalCtrlUnlock(String pbPin, int bPinLen);/* 输入密码后，可以调用这个函数解除父母锁控�?*/
    
    /* 修改智能卡密�?*/
    // 0 成功 -2: 未插入智能卡，其他值失�?
    public native int ChangePin(String pbOldPin, String pbNewPin, int bPinLen);/* 修改智能卡密�?*/
    
    /* 获取智能卡状�?1:卡插入，0;卡拔出，==========================*/
    public native int CaGetScStatus();
    
    public native int factoryResetTest();
    
    /**
     * 
     * @param type 消息类型
     * @param data 根据消息类型的不同，发送不同的实例
     * @param arg1 扩展参数
     * @return 
     * @see [类、类#方法、类#成员]
     */
    public native int SendMessageToCA(int type, Object data, int arg1);
}
