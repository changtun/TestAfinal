package com.pbi.dvb.dvbinterface;

public class Tuner
{
    public native int getTunerStrengthPercent(int tunerId);
    public native int getTunerQualityPercent(int tunerId);
    public native void getTunerCablePercent(DVBCore_Cab_Desc_t Cable, CablePercent Percent);
    
    /**
     * <DVB-C Tuner参数>
     * @author  gtsong
     * @version  [版本号, 2012-7-13]
     * @see  [相关类/方法]
     * @since  [产品/模块版本]
     */
    public class DVBCore_Cab_Desc_t
    {
        public char u8TunerId;
        public int u32Freq;
        public int u32SymbRate;
        public int eMod;
        public int eFEC_Inner;
        public int eFEC_Outer;
        
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
        
    }
    
    public class CablePercent
    {
        public int Strength;
        public int Quality;
        public int getStrength()
        {
            return Strength;
        }
        public void setStrength(int strength)
        {
            Strength = strength;
        }
        public int getQuality()
        {
            return Quality;
        }
        public void setQuality(int quality)
        {
            Quality = quality;
        }
        
    }


    
}
