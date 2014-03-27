package com.pbi.dvb.dvbinterface;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import com.pbi.dvb.R;
import com.pbi.dvb.utils.CommonUtils;

public class NativeEpg
{
    
    /**
     * 
     * <��Ŀ�ۿ����𣬿����ڸ���epg��ʱ��ʹ��>
     * 
     * @version [�汾��, 2012-9-5]
     * @see [�����?����]
     * @since [��Ʒ/ģ��汾]
     */
    public class peventRate
    {
        private int rate;
        
        public void setRate(int value)
        {
            rate = value;
        }
        
        public int getRate()
        {
            return rate;
        }
        
    }
    
    /**
     * 
     * <ȡevent ��ϸ��Ϣ������epg��ϸ��Ϣ��ʾ>
     * 
     * @version [�汾��, 2012-9-5]
     * @see [�����?����]
     * @since [��Ʒ/ģ��汾]
     */
    public class extentinfo
    {
        private String exinfo;
        
        public void setextentinfo(String value)
        {
            exinfo = value;
        }
        
        public String getextentinfo()
        {
            return exinfo;
        }
        
    }
    
    /**
     * 
     * <java��ȡeventʱ��ʱ��Ҫ���뵱ǰʱ��>
     * 
     * @version [�汾��, 2012-9-5]
     * @see [�����?����]
     * @since [��Ʒ/ģ��汾]
     */
    public class EpgTime
    {
        private short utc_year;
        private short utc_month;
        private short utc_date;
        private short utc_hour;
        private short utc_minute;
        private short utc_second;
        
        public void setutcyear(short value)
        {
            utc_year = value;
        }
        
        public short getutcyear()
        {
            return utc_year;
        }
        
        public void setutcmonth(short value)
        {
            utc_month = value;
        }
        
        public short getutcmonth()
        {
            return utc_month;
        }
        
        public void setutcdate(short value)
        {
            utc_date = value;
        }
        
        public short getutcdate()
        {
            return utc_date;
        }
        
        public void setutchour(short value)
        {
            utc_hour = value;
        }
        
        public short getutchour()
        {
            return utc_hour;
        }
        
        public void setutcminute(short value)
        {
            utc_minute = value;
        }
        
        public short getutcminute()
        {
            return utc_minute;
        }
        
        public void setutcsecond(short value)
        {
            utc_second = value;
        }
        
        public short getutcsecond()
        {
            return utc_second;
        }
        
    }
    
    public class EpgEventInfo
    {
        private short event_id;

        /* epg �˵�,banner���˵���ʾ�� */
        private short start_year;
        private short start_month;
        private short start_date;
        private short start_hour;
        private short start_minute;
        private short start_second;
        
        /* epg �˵�,banner���˵���ʾ�� */
        private short end_year;
        private short end_month;
        private short end_date;
        private short end_hour;
        private short end_minute;
        private short end_second;
        
        /* high 8bit is nibble1 and nibble2, low 8bit is user_nibble */
        private short content_nibble_level;
        private int content_nibble1;
        private int content_nibble2;
        private int user_nibble;
        private char name_length;
        private int rate;/* �ۿ��ȼ��� */
        
        /* epg �˵�,banner���˵���ʾ�� */
        private String event_name;
        private char short_length;
        
        /* ��ʱ�ò��� */
        private String short_event;

        public void seteventid(short value)
        {
            event_id = value;
        }
        
        public short geteventid()
        {
            return event_id;
        }
        
        public void setstartyear(short value)
        {
            start_year = value;
        }
        
        public short getstartyear()
        {
            return start_year;
        }
        
        public void setstartmonth(short value)
        {
            start_month = value;
        }
        
        public short getstartmonth()
        {
            return start_month;
        }
        
        public void setstartdate(short value)
        {
            start_date = value;
        }
        
        public short getstartdate()
        {
            return start_date;
        }
        
        public void setstarthour(short value)
        {
            start_hour = value;
        }
        
        public short getstarthour()
        {
            return start_hour;
        }
        
        public void setstartminute(short value)
        {
            start_minute = value;
        }
        
        public short getstartminute()
        {
            return start_minute;
        }
        
        public void setstartsecond(short value)
        {
            start_second = value;
        }
        
        public short getstartsecond()
        {
            return start_second;
        }
        
        public void setendyear(short value)
        {
            end_year = value;
        }
        
        public short getendyear()
        {
            return end_year;
        }
        
        public void setendmonth(short value)
        {
            end_month = value;
        }
        
        public short getendmonth()
        {
            return end_month;
        }
        
        public void setenddate(short value)
        {
            end_date = value;
        }
        
        public short getenddate()
        {
            return end_date;
        }
        
        public void setendhour(short value)
        {
            end_hour = value;
        }
        
        public short getendhour()
        {
            return end_hour;
        }
        
        public void setendminute(short value)
        {
            end_minute = value;
        }
        
        public short getendminute()
        {
            return end_minute;
        }
        
        public void setendsecond(short value)
        {
            end_second = value;
        }
        
        public short getendsecond()
        {
            return end_second;
        }
        
        public void setcontentnibble(short value)
        {
            content_nibble_level = value;
        }
        
        public int getcontentnibble()
        {
            return content_nibble1;
        }
        
        public void setnamelen(char value)
        {
            name_length = value;
        }
        
        public char getnamelen()
        {
            return name_length;
        }
        
        public void seteventname(String value)
        {
            event_name = value;
        }
        
        public String geteventname()
        {
            return event_name;
        }
        
        // java add these methods.
        public char getShort_length()
        {
            return short_length;
        }
        
        public void setShort_length(char short_length)
        {
            this.short_length = short_length;
        }
        
        public String getShort_event()
        {
            return short_event;
        }
        
        public void setShort_event(String short_event)
        {
            this.short_event = short_event;
        }
        
        public void setrate(int value)
        {
            rate = value;
        }
        
        public int getrate()
        {
            return rate;
        }

        public HashMap<Integer, Integer> getRate()
        {
            HashMap<Integer, Integer> map = new HashMap<Integer, Integer>();
            int picId = -1;
            int textId = -1;
            
            if (rate == 0)
            {
                picId = R.drawable.epg_rate_green;
                textId = R.string.rate_normal;
            }
            else if (rate >= 1 && rate <= 6)
            {
                picId = R.drawable.epg_rate_blue;
                textId = R.string.rate_protect;
            }
            else if (rate >= 7 && rate <= 12)
            {
                picId = R.drawable.epg_rate_yellow;
                textId = R.string.rate_assist;
            }
            else if (rate >= 13 && rate <= 14)
            {
                picId = R.drawable.epg_rate_red;
                textId = R.string.rate_adult;
            }
            else if (rate >= 15)
            {
                picId = R.drawable.epg_rate_pink;
                textId = R.string.rate_special;
            }
           
            if (picId == -1 && textId == -1)
            {
                return null;
            }
            else
            {
                map.put(picId, textId);
                return map;
            }
        }
        
		public String getStartTime()
		{
			String bucket = CommonUtils.alignCharacter(start_hour) + ":" + CommonUtils.alignCharacter(start_minute);
//			if (bucket.equals("00:00"))
//            {
//                bucket = null;
//            }
			return bucket;
		}
        
        private double getEventDuration()
        {
            return getDuration(end_hour + ":" + end_minute + ":" + end_second, start_hour + ":" + start_minute + ":"+ start_second);
        }
        
        /**
         * 
         * <get current event play progress.>
         * @param currentTime HH:MM:SS
         * @return
         * @see [�ࡢ��#��������#��Ա]
         */
        public int getPlayProgress(String currentTime)
        {
            int progress = 0;
            double duration = getEventDuration();
            long playDuration = getDuration(currentTime,start_hour+":"+start_minute+":"+start_second);
            if (duration != 0)
            {
                progress = (int)((playDuration / duration) * 100);
            }
            return progress;
        }
        
    }
    
    private long getDuration(String startTime, String endTime)
    {
        DateFormat df = new SimpleDateFormat("HH:mm:ss");
        Date start = null;
        Date end = null;
        try
        {
            start = df.parse(startTime);
            end = df.parse(endTime);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        long bucket = end.getTime() - start.getTime();
        return bucket;
    }
    
    public class EPGOnePage
    {
        public short totalnum; /* epgˢ��ҳ���ʱ�򷵻صķ���������¼����� */
        
        public EpgEventInfo[] epgList;
        
        public void settotalnum(short value)
        {
            totalnum = value;
        }
        
        public short gettotalnum()
        {
            return totalnum;
        }
        
        public EpgEventInfo[] getEpgList()
        {
            return epgList;
        }
        
        public void setEpgList(EpgEventInfo[] epgList)
        {
            this.epgList = epgList;
        }
        
        @SuppressWarnings("unused")
        public void BulidArrPointer(short number, EPGOnePage onepage)
        {
            int ii = 0;
            
            System.out.println("BulidArrPointer===========" + number + "====");
            
            EpgEventInfo[] epgEventInfos = new EpgEventInfo[number];
            onepage.setEpgList(epgEventInfos);
        }
        
        public void BulidEpglist(short number, EPGOnePage onepage)
        {
            int ii = 0;
            
            System.out.println("===========" + number + "====");
            
            for (ii = 0; ii < number; ii++)
            {
                onepage.epgList[ii] = new EpgEventInfo();
            }
        }
        
    }
    
    public native int EpgMessageInit(NativeEpgMsg msgEPG);
    
/**
     * banner����Ϣȡ��ǰ����event
     * @param serviceid
     * @param tsid
     * @param ornetowrkid Ҫ��ȡ�¼���Ƶ����Ϣ
     * @param pevent �����Ͳ����ȡ����һ���¼���Ϣ����jni��ȡ�����ݵ���ʵ����
     * @return ������0ʱ��ʾ�ɹ�
     * @see [�ࡢ��#��������#��Ա]
 */
    public native int GetPevent(short serviceid, short tsid, short ornetowrkid, EpgEventInfo pevent);
    
    /**
     * 
     * banner����Ϣȡ��һ������event
     * 
     * @param serviceid
     * @param tsid
     * @param ornetowrkid Ҫ��ȡ�¼���Ƶ����Ϣ
     * @param fevent �����Ͳ����ȡ�ĵ�ǰ�¼���Ϣ����jni��ȡ�����ݵ���ʵ����
     * @see [�ࡢ��#��������#��Ա]
     */
    public native int GetFevent(short serviceid, short tsid, short ornetowrkid, EpgEventInfo fevent);
    
    /**
     * 
     * epg�˵�ʹ��,��ȡĳ���ĳ��event��Ϣ
     * 
     * @param serviceid
     * @param tsid
     * @param ornetowrkid Ƶ����Ϣ
     * @param curdate ��ǰ�����?��ʼ��0�ǵ��죬1�ڶ��죬��������
     * @param number Ҫ��ȡ �ĵڼ����¼�
     * @param eventinfo �����Ͳ���Ҫ��ȡ���¼���Ϣ����jni��ȡ�����ݵ���ʵ����
     * @param utctime �����Ͳ����뵱ǰ��ϵͳʱ�䣬 ����ȡ����ϵͳʱ�� �򴫵ݲ���ȫ��Ϊ0,��ǰʱ��������ʱ��Ϊ׼
     * @see [�ࡢ��#��������#��Ա]
     */
    public native void GetEventInfoByIndex(short serviceid, short tsid, short ornetowrkid, int curdate, int number,
        EpgEventInfo eventinfo, EpgTime utctime);
    
    /**
     * java�ϲ�ͨ������ӿڵ�ǰ��ϵͳʱ�䣬���ʱ���Ǵ������л�ȡ�� ����ͨ��ǰʱ����¼��Ŀ�ʼ����ʱ�����ó���ǰ�Ľ�Ŀ���Ž��
     * 
     * @param utctime
     * @see [�ࡢ��#��������#��Ա]
     */
    public native void GetUtcTime(EpgTime utctime);
    
    /**
     * ��ȡ��ǰ�ۿ���Ŀ�ļ��� ����ֵ0��Ч
     * 
     * @param serviceid
     * @param tsid
     * @param ornetowrkid
     * @param Ratedata
     * @see [�ࡢ��#��������#��Ա]
     */
    public native void GetRate(short serviceid, short tsid, short ornetowrkid, peventRate Ratedata);
    
    public native void GetOneEventExtentInfo(short serviceid, short tsid, short ornetowrkid, char flag, char cur_date,
        short number, extentinfo data);
    
    /**
     * EPG �˵�ʹ�ã���ȡĳҳ��epg�¼��б�
     * 
     * @param serv_id
     * @param ts_id
     * @param on_id Ƶ����Ϣ
     * @param cur_date
     * @param number Ҫ��ȡ �¼��б�ĵڼ������?
     * @param one_page_max һ��ҳ����ʾ�����¼�����ʱ��ʹ�ã��ȱ���
     * @param peventlist ��ǰƵ����epg �¼���Ϣ������ʵ�������Լ���Ӧ�� �¼���Ϣ
     * @see [�ࡢ��#��������#��Ա]
     */
    public native void GetEventListInfo(short serv_id, short ts_id, short on_id, int cur_date, short number,
        short one_page_max, EPGOnePage peventlist);
    

    public native void GetEventListInfoByNibble(short serv_id, short ts_id, short on_id, int cur_date, short number,
        short one_page_max, EPGOnePage peventlist,short nibble);	
    
}
