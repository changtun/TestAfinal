package com.pbi.dvb.dvbinterface;

import android.content.Context;
//import android.os.PBIManager;
import com.pbi.dvb.utils.LogUtils;
import com.android.pbi.frontPanel.FrontPanelManager;
import android.os.PBIManager;

public class NativeDrive
{
    private static boolean LoadLibFlag = true;
    
    private static Context context;
    
    private PBIManager pbiManager;
    
    private FrontPanelManager front;
    static
    {
        if (LoadLibFlag)
        {
            try
            {
                System.loadLibrary("dvbadapt");
                System.loadLibrary("dvbcore");
                System.loadLibrary("trcas");
                System.loadLibrary("dvbiconv");
                System.loadLibrary("dvbcore-jni");
                
                System.loadLibrary("dvbnppv");
                
                LoadLibFlag = false;
            }
            catch (UnsatisfiedLinkError ule)
            {
                System.out.println("Could not load library: " + ule);
            }
            catch (SecurityException se)
            {
                System.out.println("Encountered a security issue when loading library: " + se);
            }
        }
    }
    
    public native int pvwareDRVInit();
    
    public native int CAInit();
    
    public native int DrvPlayerInit();
    
    public native int DrvPlayerUnInit();
    
    public native int DrvPmocSetWakeUp(PmocWakeUp WakeUp);
    
    // public native void DrvFPClear();
    
    //pvr 
    //  public native int plus (int x, int y);
    public native void surfaceCreated(int npp, Object surface);
    public native void surfaceChanged(int npp, int format, int width, int height);
    public native void surfaceDestroyed(int npp);
    
    public NativeDrive()
    {
        super();
        // pbiManager = (PBIManager)context.getSystemService(Context.PBI_SERVICE);//new PBIManager();
        pbiManager = new PBIManager(this.context);
        front = (FrontPanelManager)pbiManager.getManager("frontPanel");
    }
    
    public NativeDrive(Context context)
    {
        super();
        this.context = context;
    }
    
    public void DisPlay(String str)
    {
        // int i = pbiManager.display(str);
        front.display(str);
    }
    
    public class PmocWakeUp
    {
        /*
         * reserved param
         * 
         * int u32IrPmocNum; int u32IrPowerKey0 [6]; int u32IrPowerKey1 [6]; int u32KeypadPowerKey;
         */
        // second.
        private int u32WakeUpTime;
        
        public int getU32WakeUpTime()
        {
            return u32WakeUpTime;
        }
        
        public void setU32WakeUpTime(int u32WakeUpTime)
        {
            this.u32WakeUpTime = u32WakeUpTime;
        }
        
    }
}
