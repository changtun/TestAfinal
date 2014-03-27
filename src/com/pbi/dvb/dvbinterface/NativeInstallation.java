package com.pbi.dvb.dvbinterface;


public class NativeInstallation extends Tuner
{
    private static final String TAG = "NativeInstallation";
    public native int installationInit();
    public native int installationStart(Install_Cfg_t tCfg);
    public native int installationStop();
    public native int installationGetSearchRet(Install_Search_Result_t ptRet);
    public native int installationFreeSearchRet();
    public native int installationDeinit();
    public native int installMessageInit(MessageInstallation obj);
    
    /**
     * <超时时间实体>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    public class Install_Timeout_t
    {
        // tuner 锁频超时时间
        public int u32TunerTimeout;
        // NIT actual 超时时间
        public int u32NITActualTimeout;
        // NIT other 超时时间
        public int u32NITOtherTimeout;
        // BAT 超时时间
        public int u32BATTimeout;
        // PAT 超时时间
        public int u32PATTimeout;
        // PMT 超时时间
        public int u32PMTTimeout;
        // SDT actual 超时时间
        public int u32SDTActualTimeout;
        // SDT other 超时时间
        public int u32SDTOtherTimeout;
        
        public Install_Timeout_t()
        {
            super();
            this.u32TunerTimeout = 1000;
            this.u32NITActualTimeout = 20000;
            this.u32NITOtherTimeout = 20000;
            this.u32BATTimeout = 10000;
            this.u32PATTimeout = 5000;
            this.u32PMTTimeout = 5000;
            this.u32SDTActualTimeout = 10000;
            this.u32SDTOtherTimeout = 10000;
        }
        
        
    }
    
    /**
     * <C类型Tuner的描述信息>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
  /*  public class DVBCore_Cab_Desc_t
    {
        private char u8TunerId;
        private int u32Freq;
        private int u32SymbRate;
        private int eMod;
        private int eFEC_Inner;
        private int eFEC_Outer;
        
        public char getU8TunerId()
        {
            return u8TunerId;
        }
        
        public void setU8TunerId(char u8TunerId)
        {
            this.u8TunerId = u8TunerId;
        }
        
        public int getU32Freq()
        {
            return u32Freq;
        }
        
        public void setU32Freq(int u32Freq)
        {
            this.u32Freq = u32Freq;
        }
        
        public int getU32SymbRate()
        {
            return u32SymbRate;
        }
        
        public void setU32SymbRate(int u32SymbRate)
        {
            this.u32SymbRate = u32SymbRate;
        }
        
        public int geteMod()
        {
            return eMod;
        }
        
        public void seteMod(int eMod)
        {
            this.eMod = eMod;
        }
        
        public int geteFEC_Inner()
        {
            return eFEC_Inner;
        }
        
        public void seteFEC_Inner(int eFEC_Inner)
        {
            this.eFEC_Inner = eFEC_Inner;
        }
        
        public int geteFEC_Outer()
        {
            return eFEC_Outer;
        }
        
        public void seteFEC_Outer(int eFEC_Outer)
        {
            this.eFEC_Outer = eFEC_Outer;
        }
        
    }*/
    
    /**
     * <搜索参数实体>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    public class Install_Cfg_t
    {
        public int eSearchType;
        public Install_Timeout_t ptTimeout;
        public int u32MainTpNum;
        // C835为 eDVBCORE_SIGNAL_SOURCE_DVBC 根据不同Tuner类型，写入不同值，后面的参数定义不同的类
        public int eSignalSource;
        public DVBCore_Cab_Desc_t[] puMainTpDescList;
        public int u32NormalTpNum;
        public DVBCore_Cab_Desc_t[] puNormalTpDescList;
		public int u8SearchLimitFlag; /* 搜索限制 0：正常搜索  1：無限制搜索 */
        
        public int geteSearchType()
        {
            return eSearchType;
        }
        
        public void seteSearchType(int eSearchType)
        {
            this.eSearchType = eSearchType;
        }
        
        public Install_Timeout_t getPtTimeout()
        {
            return ptTimeout;
        }
        
        public void setPtTimeout(Install_Timeout_t ptTimeout)
        {
            this.ptTimeout = ptTimeout;
        }
        
        public int getU32MainTpNum()
        {
            return u32MainTpNum;
        }
        
        public void setU32MainTpNum(int u32MainTpNum)
        {
            this.u32MainTpNum = u32MainTpNum;
        }
        
        public int geteSignalSource()
        {
            return eSignalSource;
        }
        
        public void seteSignalSource(int eSignalSource)
        {
            this.eSignalSource = eSignalSource;
        }
        
        public DVBCore_Cab_Desc_t[] getPuMainTpDescList()
        {
            return puMainTpDescList;
        }
        
        public void setPuMainTpDescList(DVBCore_Cab_Desc_t[] puMainTpDescList)
        {
            this.puMainTpDescList = puMainTpDescList;
        }
        
        public int getU32NormalTpNum()
        {
            return u32NormalTpNum;
        }
        
        public void setU32NormalTpNum(int u32NormalTpNum)
        {
            this.u32NormalTpNum = u32NormalTpNum;
        }
        
        public DVBCore_Cab_Desc_t[] getPuNormalTpDescList()
        {
            return puNormalTpDescList;
        }
        
        public void setPuNormalTpDescList(DVBCore_Cab_Desc_t[] puNormalTpDescList)
        {
            this.puNormalTpDescList = puNormalTpDescList;
        }
        
    }
    
    /**
     * <TP 实体>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    public class Install_Tp_t
    {
        public int u16NetId;
        public int u16TS_Id;
        public int u16ON_Id;
        public int eSignalSource;
        public DVBCore_Cab_Desc_t uTunerDesc;
        
        public int getU16NetId()
        {
            return u16NetId;
        }
        
        public void setU16NetId(int u16NetId)
        {
            this.u16NetId = u16NetId;
        }
        
        public int getU16TS_Id()
        {
            return u16TS_Id;
        }
        
        public void setU16TS_Id(int u16ts_Id)
        {
            u16TS_Id = u16ts_Id;
        }
        
        public int getU16ON_Id()
        {
            return u16ON_Id;
        }
        
        public void setU16ON_Id(int u16on_Id)
        {
            u16ON_Id = u16on_Id;
        }
        
        public int geteSignalSource()
        {
            return eSignalSource;
        }
        
        public void seteSignalSource(int eSignalSource)
        {
            this.eSignalSource = eSignalSource;
        }
        
        public DVBCore_Cab_Desc_t getuTunerDesc()
        {
            return uTunerDesc;
        }
        
        public void setuTunerDesc(DVBCore_Cab_Desc_t uTunerDesc)
        {
            this.uTunerDesc = uTunerDesc;
        }
        
    }
    
    /**
     * <Service 实体>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    public class Install_Serv_t
    {
        public int u16ServId;
        
        public int u16PMTPid;
		
        public int u16LogicalNum; 
        
        public char u8ServType;
        
        public char u8FreeCA_Mode;
        
        public String sServName;
        
        public Install_Tp_t ptTpDesc;
        
        public int getU16ServId()
        {
            return u16ServId;
        }
        
        public void setU16ServId(int u16ServId)
        {
            this.u16ServId = u16ServId;
        }

	public int getu16LogicalNum()
	{
	    return u16LogicalNum;
	}

	public void setu16LogicalNum(int u16LogicalNum)
	{
	    this.u16LogicalNum = u16LogicalNum;
	}

		
        public int getU16PMTPid()
        {
            return u16PMTPid;
        }
        
        public void setU16PMTPid(int u16pmtPid)
        {
            u16PMTPid = u16pmtPid;
        }
        
        public char getU8ServType()
        {
            return u8ServType;
        }
        
        public void setU8ServType(char u8ServType)
        {
            this.u8ServType = u8ServType;
        }
        
        public char getU8FreeCA_Mode()
        {
            return u8FreeCA_Mode;
        }
        
        public void setU8FreeCA_Mode(char u8FreeCA_Mode)
        {
            this.u8FreeCA_Mode = u8FreeCA_Mode;
        }
        
        public String getsServName()
        {
            return sServName;
        }
        
        public void setsServName(String sServName)
        {
            this.sServName = sServName;
        }
        
        public Install_Tp_t getPtTpDesc()
        {
            return ptTpDesc;
        }
        
        public void setPtTpDesc(Install_Tp_t ptTpDesc)
        {
            this.ptTpDesc = ptTpDesc;
        }
        
    }
    
    /**
     * <搜索结果实体>
     * 
     * @author gtsong
     * @version [版本号, 2012-7-11]
     * @see [相关类/方法]
     * @since [产品/模块版本]
     */
    public class Install_Search_Result_t
    {
        public int u32TpNum;
        
        public Install_Tp_t[] ptTpList;
        
        public int u32ServNum;
        
        public Install_Serv_t[] ptServList;
        
        public void BulidTpServList(Install_Search_Result_t tISR)
        {
            int ii = 0;
            
            System.out.println("===========" + tISR.u32TpNum + "====" + tISR.u32ServNum + "==============");
            if (0 < tISR.u32TpNum)
            {
                tISR.ptTpList = new Install_Tp_t[tISR.u32TpNum];
                
                for (ii = 0; ii < tISR.u32TpNum; ii++)
                {
                    tISR.ptTpList[ii] = new Install_Tp_t();
                    tISR.ptTpList[ii].uTunerDesc = new DVBCore_Cab_Desc_t();
                }
                
            }
            
            if (0 < tISR.u32ServNum)
            {
                tISR.ptServList = new Install_Serv_t[tISR.u32ServNum];
                
                for (ii = 0; ii < tISR.u32ServNum; ii++)
                {
                    tISR.ptServList[ii] = new Install_Serv_t();
                    tISR.ptServList[ii].ptTpDesc = new Install_Tp_t();
                    tISR.ptServList[ii].ptTpDesc.uTunerDesc = new DVBCore_Cab_Desc_t();
                }
            }
        }
        
        public int getU32TpNum()
        {
            return u32TpNum;
        }
        
        public void setU32TpNum(int u32TpNum)
        {
            this.u32TpNum = u32TpNum;
        }
        
        public Install_Tp_t[] getPtTpList()
        {
            return ptTpList;
        }
        
        public void setPtTpList(Install_Tp_t[] ptTpList)
        {
            this.ptTpList = ptTpList;
        }
        
        public int getU32ServNum()
        {
            return u32ServNum;
        }
        
        public void setU32ServNum(int u32ServNum)
        {
            this.u32ServNum = u32ServNum;
        }
        
        public Install_Serv_t[] getPtServList()
        {
            return ptServList;
        }
        
        public void setPtServList(Install_Serv_t[] ptServList)
        {
            this.ptServList = ptServList;
        }
    }
    
}
