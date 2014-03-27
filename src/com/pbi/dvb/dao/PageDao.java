package com.pbi.dvb.dao;

import java.util.List;

public interface PageDao
{
    
    /**
     * <�õ�������Ŀ����>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract int getCount();
    
    /**
     * <�õ���ǰ�ǵڼ�ҳ>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract int getCurrentPage();
    
    /**
     * <�õ�������ҳ>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract int getPageNum();
    
    /**
     * <ÿҳ�Ĵ�С>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract int getPerPage();
    
    /**
     * <������nҳ>
     * @param n
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void gotoPage(int n);
    
    /**
     * <�ж��Ƿ������һҳ>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract boolean hasNextPage();
    
    /**
     * <�ж��Ƿ����ǰһҳ>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract boolean hasPrepage();
    
    /**
     * <��һҳ>
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void headPage();
    
    /**
     * <���һҳ>
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void lastPage();
    
    /**
     * <��һҳ>
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void nextPage();
    
    /**
     * <ǰһҳ>
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void prePage();
    
    /**
     * <����ÿҳ��ʾ��Ŀ>
     * @param pageSize
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void setPerPageSize(int pageSize);
    
    /**
     * <�õ���ǰ�б�>
     * @return
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract List<?> getCurrentList();
    
    /**
     * <��ʼ���б�>
     * @param list
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void initList(List<?> list);
    
    /**
     * <���õ�ǰҳ>
     * @param currentPage
     * @see [�ࡢ��#��������#��Ա]
     */
    public abstract void setCurrentPage(int currentPage);
    
}