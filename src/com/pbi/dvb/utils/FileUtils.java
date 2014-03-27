/*
 * 文 件 名:  FileUtils.java
 * 版    权:  Beijing Jaeger Communication Electronic Technology Co., Ltd.Copyright YYYY-YYYY,  All rights reserved
 * 描    述:  <描述>
 * 修 改 人:  
 * 修改时间:  2013-6-4
 * 跟踪单号:  <跟踪单号>
 * 修改单号:  <修改单号>
 * 修改内容:  <修改内容>
 */
package com.pbi.dvb.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * <读写文件的工具类>
 * 
 * @author 姓名 工号
 * @version [版本号, 2013-6-4]
 * @see [相关类/方法]
 * @since [产品/模块版本]
 */
public class FileUtils
{
    private static final String TAG = "FileUtils";
    
    /**
     * <以行为单位读取文件>
     * 
     * @param fileName
     * @return 返回文件內容的集合
     * @see [类、类#方法、类#成员]
     */
    public static List<String> readFileByLines(String fileName)
    {
        File file = new File(fileName);
        BufferedReader reader = null;
        List<String> list = new ArrayList<String>();
        
        if (file.exists())
        {
            try
            {
                LogUtils.printLog(1, 3, TAG, "--->>>Read File :: " + fileName);
                
                reader = new BufferedReader(new FileReader(file));
                String tempString = null;
                int line = 1;
                // 一次读入一行，直到读入null为文件结束
                while ((tempString = reader.readLine()) != null)
                {
                    list.add(tempString);
                    line++;
                }
                
                LogUtils.printLog(1, 3, TAG, "--->>>File line numbers  :: " + line);
                reader.close();
            }
            catch (IOException e)
            {
                LogUtils.printLog(1, 5, TAG, "--->>> read file by lines fail ---");
                e.printStackTrace();
            }
            finally
            {
                if (reader != null)
                {
                    try
                    {
                        reader.close();
                    }
                    catch (IOException e1)
                    {
                        LogUtils.printLog(1, 5, TAG, "--->>> read file by lines fail ---");
                        e1.printStackTrace();
                    }
                }
            }
        }
        else
        {
            LogUtils.printLog(1, 3, TAG, "---->>> dvb_config not found---");
            try
            {
                file.createNewFile();
                initFile(fileName);
            }
            catch (IOException e)
            {
                
                LogUtils.printLog(1, 5, TAG, "--->>> create file fail ---");
                e.printStackTrace();
            }
        }
        
        return list;
    }
    
    /**
     * <写文件>
     * 
     * @param filename
     * @param data 写入内容
     * @see [类、类#方法、类#成员]
     */
    public static void writeFile(String filename, List<String> data)
    {
        try
        {
            File file = new File(filename);
            if (file.exists())
            {
                LogUtils.printLog(1, 3, TAG, "--->>> dvb_config exist !!----");
                BufferedWriter bWriter = new BufferedWriter(new FileWriter(file));
                
                for (int i = 0; i < data.size(); i++)
                {
                    LogUtils.printLog(1, 3, TAG, "--->>> new data content ---" + data.get(i));
                    bWriter.write(data.get(i));
                    bWriter.newLine();
                }
                
                bWriter.close();
            }
            else
            {
                LogUtils.printLog(1, 3, TAG, "---->>> dvb_config not found---");
                try
                {
                    file.createNewFile();
                    initFile(filename);
                }
                catch (IOException e)
                {
                    
                    LogUtils.printLog(1, 5, TAG, "--->>> create file fail ---");
                    e.printStackTrace();
                }
            }
        }
        
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    private static void initFile(String filename)
    {
        List<String> data = new ArrayList<String>();
        data.add("## TunerBandWidth = 0(6M), 1 = (8M), other(8M)");
        data.add("TunerBandWidth=1.");
        data.add("## SearchBat = 0(ON), 1(OFF), other(OFF)");
        data.add("SearchBAT=0.");
        data.add("## DefaultCharacterEncode = 0(GBK), 1(BIG5), 2(UNICODE), 3(UTF8), other(0)");
        data.add("DefaultCharacterEncode=2.");
        
        if (null != data)
        {
            FileWriter fw = null;
            BufferedWriter bw = null;
            try
            {
                fw = new FileWriter(filename, true); // 创建FileWriter对象，用来写入字符流
                bw = new BufferedWriter(fw); // 将缓冲对文件的输出
                
                // String line = System.getProperty("line.separator"); //获取换行(跨平台)
                // StringBuffer sb = new StringBuffer();
                for (int i = 0; i < data.size(); i++)
                {
                    bw.write(data.get(i));
                    bw.newLine();
                }
                bw.flush(); // 刷新该流的缓冲
                bw.close();
                fw.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
                try
                {
                    bw.close();
                    fw.close();
                }
                catch (IOException e1)
                {
                    e1.printStackTrace();
                }
            }
        }
    }
}
