package com.pbi.dvb.dao;

import java.util.List;

public interface PageDao
{
    
    /**
     * <得到所有条目总数>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract int getCount();
    
    /**
     * <得到当前是第几页>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract int getCurrentPage();
    
    /**
     * <得到共多少页>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract int getPageNum();
    
    /**
     * <每页的大小>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract int getPerPage();
    
    /**
     * <跳到第n页>
     * @param n
     * @see [类、类#方法、类#成员]
     */
    public abstract void gotoPage(int n);
    
    /**
     * <判断是否存在下一页>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract boolean hasNextPage();
    
    /**
     * <判断是否存在前一页>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract boolean hasPrepage();
    
    /**
     * <第一页>
     * @see [类、类#方法、类#成员]
     */
    public abstract void headPage();
    
    /**
     * <最后一页>
     * @see [类、类#方法、类#成员]
     */
    public abstract void lastPage();
    
    /**
     * <下一页>
     * @see [类、类#方法、类#成员]
     */
    public abstract void nextPage();
    
    /**
     * <前一页>
     * @see [类、类#方法、类#成员]
     */
    public abstract void prePage();
    
    /**
     * <设置每页显示数目>
     * @param pageSize
     * @see [类、类#方法、类#成员]
     */
    public abstract void setPerPageSize(int pageSize);
    
    /**
     * <得到当前列表>
     * @return
     * @see [类、类#方法、类#成员]
     */
    public abstract List<?> getCurrentList();
    
    /**
     * <初始化列表>
     * @param list
     * @see [类、类#方法、类#成员]
     */
    public abstract void initList(List<?> list);
    
    /**
     * <设置当前页>
     * @param currentPage
     * @see [类、类#方法、类#成员]
     */
    public abstract void setCurrentPage(int currentPage);
    
}