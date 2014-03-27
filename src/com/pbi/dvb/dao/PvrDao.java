/*
 * �� �� ��:  PvrDao.java
 * ��    Ȩ:  Huawei Technologies Co., Ltd. Copyright YYYY-YYYY,  All rights reserved
 * ��    ��:  <����>
 * �� �� ��: 
 * �޸�ʱ��:  2012-12-1
 * ���ٵ���:  <���ٵ���>
 * �޸ĵ���:  <�޸ĵ���>
 * �޸�����:  <�޸�����>
 */
package com.pbi.dvb.dao;

import java.util.List;

import com.pbi.dvb.domain.PvrBean;

/**
 * <һ�仰���ܼ���> <������ϸ����>
 * 
 * @author ���� ����
 * @version [�汾��, 2012-12-1]
 * @see [�����/����]
 * @since [��Ʒ/ģ��汾]
 */
public interface PvrDao
{
    public abstract int add_Record_Task(PvrBean task);
    
    public abstract PvrBean get_First_Record_Task(Integer... skipID);
    
    public abstract PvrBean get_Record_Task_By_ID(Integer _ID);
    
    public abstract int update_Record_Task(PvrBean task);
    
    public abstract int delete_Record_Task(PvrBean task);
    
    public abstract int delete_Record_Task_By_ID(Integer pvr_ID);
    
    public abstract List<PvrBean> get_All_Record_Task();
    
    public abstract List<PvrBean> get_All_Illegal_Tasks();
    
    public abstract PvrBean get_First_Close_Task();
    
    public abstract int get_Count_Of_Conflict_Task(PvrBean pvrBean);
    
    public abstract List<Integer> getIdOfConflictTasks(PvrBean pvrBean);
    
    public abstract int get_Count_Of_Unclose_Task();
    
    public abstract void init_Pvr_Database(int new_Item_Count);
    
    public abstract void filter_All_Task();
    
    public abstract void insert_REAL_TIME_Task();
    
    public abstract PvrBean get_REAL_TIME_Task();
    
    public abstract void cancelTaskByIDS(List<Integer> ids);
    
    /* Check if the task has exists... */
    public abstract List<Integer> checkTaskExists(int serviceId, long getstartdate, long lstart, long lend);
    
}
