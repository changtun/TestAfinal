
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. 
*
* File name : drv_api.c
* Description : Includes all application functions used to initialize the driver module.
              	This allows to be used in the main function for initializing driver module .
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2010/02/05         Created                                 Zheng Dong sheng
******************************************************************************/
/*******************************************************/
/*              Includes				                  			     */
/*******************************************************/
#include "drv_api.h"
#include "pbitrace.h"

/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/

/*******************************************************/
/*               Private Types						   */
/*******************************************************/

/*******************************************************/
/*               Private Constants                     */
/*******************************************************/

/*******************************************************/
/*               Global Variables					   */
/*******************************************************/

/*******************************************************/
/*               Private Variables (static)			   */
/*******************************************************/

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/

/*******************************************************/
/*              Private Functions				 	  */
/*******************************************************/

/*******************************************************/
/*              Public Functions					   */
/*******************************************************/

//extern void App_KeyNotify(S32 send_key_value, S32 device);


/******************************************************************************
* Function : Pvware_DRV_Init
* parameters :	
*                   nothing
* Return :	
*		nothing
* Description :
*		The function is to init the driver.
*
* Author : Zheng Dong sheng	2010/02/05
******************************************************************************/
int g_DrivesInitFlag = 0;
SYS_DRV_ErrorCode_t Pvware_DRV_Init( void )
{
	int iRet = 0;

    if( 1 == g_DrivesInitFlag )
    {
       return SYS_DRV_NO_ERROR;
    }
    
	OS_API_Init();
#if 0
	iRet = DRV_I2C_Open();
    if( 0 != iRet )
    {
        pbierror("%s  DRV_I2C_Open  error!\n", __FUNCTION__ );
    }
#endif	
	ConfigInit();
    
	iRet = DRV_Tuner_Init();
	if( 0 != iRet )
	{
		pbierror("%s  DRV_Tuner_Init  error!\n", __FUNCTION__ );
	}

        /* The DRV_AV_Init before SFILTER_Init */
	iRet = SFILTER_Init();
	if( 0 != iRet )
	{
    	pbierror("%s  SFILTER_Init  error!\n", __FUNCTION__ );
	}
    
    iRet = DRV_AV_Init();
    if( 0 != iRet )
    {
        PBIDEBUG("DRV_AV_Init error!");
    }

    /* PVR INIT */
    iRet = SFILTER_PVR_Init();
    if( 0 != iRet )
    {
    	pbierror("%s  SFILTER_PVR_Init  error!\n", __FUNCTION__ );
    }
    
    iRet = DRV_PVR_Init();
	if( 0 != iRet )
	{
    	pbierror("%s  DRV_PVR_Init  error!\n", __FUNCTION__ );
	}
    //CtrlTunerLockLed(1);
    //CtrlTunerLockLed(1);
    //CtrlTunerLockLed(1);

    g_DrivesInitFlag = 1;


	return SYS_DRV_NO_ERROR;
}
SYS_DRV_ErrorCode_t DrvPlayerInit( int SleepFlag )
{
    int iRet = 0;
    if( 0 == SleepFlag )
    {
        sleep(2);
        PBIDEBUG("Sleep Time");
        return 10;
    }

    DRV_AVCtrl_UnInit();
    
    SFILTER_Reset();
    
    iRet = DRV_AVCtrl_Init(DEMUX_0);
    if( 0 != iRet )
    {
	pbierror("%s  DRV_AVCtrl_Init  error!\n", __FUNCTION__ );
    }

	return SYS_DRV_NO_ERROR;
}

SYS_DRV_ErrorCode_t DrvPlayerUnInit()
{
    S32 sRet = 0;
    
    PBIDEBUG("DRV_AVCtrl_UnInit");
    sRet = DRV_AVCtrl_UnInit();
    if( 0 != sRet )
    {
        PBIDEBUG("DRV_AVCtrl_UnInit error!");
    }
    
    return SYS_DRV_NO_ERROR;
}

SYS_DRV_ErrorCode_t DrvPmSetWakeUp(SYS_DRV_PMOC_WKUP_T tPmocWkup)
{
    SYS_DRV_ErrorCode_t ErrRet = SYS_DRV_NO_ERROR;

    //ErrRet = Drv_Pmoc_Init();
    if( SYS_DRV_NO_ERROR != ErrRet )
    {
    	PBIDEBUG("Drv_Pmoc_Init  error!");
       return SYS_DRV_ERROR;
    }
    
    //ErrRet = Drv_Pmoc_SetWakeUpAttr(&tPmocWkup);
    if( SYS_DRV_NO_ERROR != ErrRet )
    {
        return SYS_DRV_ERROR;
    }
    
    return ErrRet;
}