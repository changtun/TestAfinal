package com.pbi.dvb.dao.impl;

import java.util.List;

import android.util.Log;

import com.pbi.dvb.dao.PageDao;

/**
 * <分页>
 * 
 * @author gtsong
 * @version [版本号, 2012-6-7]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class PageDaoImpl implements PageDao
{
    private static final String TAG = "PageDao";
    
    // 每页显示条目的数目
    private int perPageSize = 24;
    
    // 当前是第几页
    private int currentPage = 1;
    
    // 共多少页
    private int pageNum = 1;
    
    // 所有条目数
    private int count;
    
    private List<?> list;
    
    private List<?> currentList;
    
    public PageDaoImpl()
    {
    }
    
    public PageDaoImpl(List<?> list, int perPageSize)
    {
        this.list = list;
        if (perPageSize >= 1)
        {
            this.perPageSize = perPageSize;
        }
        if (list != null && list.size() > this.perPageSize)
        {
            this.currentList = list.subList(0, this.perPageSize - 1);
        }
        this.count = this.list.size();
        if (this.count != 0 && this.count % this.perPageSize == 0)
        {
            this.pageNum = this.count / this.perPageSize;
        }
        else
        {
            this.pageNum = this.count / this.perPageSize + 1;
        }
    }
    
    public int getCount()
    {
        return this.count;
    }
    
    public int getCurrentPage()
    {
        return this.currentPage;
    }
    
    public int getPageNum()
    {
        return this.pageNum;
    }
    
    public int getPerPage()
    {
        return this.perPageSize;
    }
    
    public void gotoPage(int n)
    {
        if (n > this.pageNum)
        {
            n = this.pageNum;
            this.currentPage = n;
        }
        else
        {
            this.currentPage = n;
        }
    }
    
    public boolean hasNextPage()
    {
        this.currentPage++;
        if (this.currentPage <= this.pageNum)
        {
            return true;
        }
        else
        {
            this.currentPage = this.pageNum;
        }
        return false;
    }
    
    public boolean hasPrepage()
    {
        this.currentPage--;
        if (this.currentPage > 0)
        {
            return true;
        }
        else
        {
            this.currentPage = 1;
        }
        return false;
    }
    
    public void headPage()
    {
        this.currentPage = 1;
    }
    
    public void lastPage()
    {
        this.currentPage = this.pageNum;
    }
    
    public void nextPage()
    {
        this.hasNextPage();
    }
    
    public void prePage()
    {
        this.hasPrepage();
    }
    
    public void setPerPageSize(int pageSize)
    {
        this.perPageSize = pageSize;
    }
    
    public List<?> getCurrentList()
    {
        Log.i(TAG, "------------this.currentPage ---------" + this.currentPage);
        Log.i(TAG, "------------this.pageNum ---------" + this.pageNum);
        Log.i(TAG, "------------currentPage-1 ---------" + (this.currentPage - 1));
        Log.i(TAG, "------------this.perPageSize ---------" + (this.perPageSize));
        Log.i(TAG, "------------this.currentPage * (this.perPageSize) ---------" + this.currentPage* (this.perPageSize));
        int startPos = (this.currentPage - 1) * this.perPageSize;
        if (this.currentPage == this.pageNum)
        {
            if (startPos < list.size())
            {
                this.currentList = this.list.subList(startPos, list.size());
            }
        }
        else
        {
            int endPos = this.currentPage * (this.perPageSize);
            if (startPos <endPos && endPos < list.size())
            {
                this.currentList = this.list.subList(startPos, endPos);
            }
        }
        return this.currentList;
    }
    
    public void initList(List<?> list)
    {
        this.list = list;
        if (this.perPageSize <= 0)
        {
            this.perPageSize = 12;
        }
        this.count = this.list.size();
        if (this.count % this.perPageSize == 0)
        {
            this.pageNum = this.count / this.perPageSize;
        }
        else
        {
            this.pageNum = this.count / this.perPageSize + 1;
        }
    }
    
    public void setCurrentPage(int currentPage)
    {
        this.currentPage = currentPage;
    }
}
