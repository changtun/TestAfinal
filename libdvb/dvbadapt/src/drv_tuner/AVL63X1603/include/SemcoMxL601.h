/********************************************************************
* SemcoMxL601.h
* MxL601 Header Files
* By WillowAndi 2011/11/14
*********************************************************************/

#ifndef _SEMCO_MXL601_H
#define _SEMCO_MXL601_H

#include <stdio.h>
#include "SemcoMxL601_TunerApi.h"
#include "SemcoMxL601_OEM_Drv.h"
#include "ITuner.h"

#if defined( __cplusplus )
extern "C"                     /* Use "C" external linkage                  */
{
#endif

/* Define the configuration information structure */
typedef struct _MxL601ConfigInfo{
    int nMode;       /* a used system mode */
    int nBandWidth;  /* bandwidth          */ 
    int nSelIF;      /* IF                 */
    int bIFPath;     /* IF path            */
    int bIFInv;      /* IF polarity        */
    int nXtalCap;    /* Xtal cap           */
    int nGainLevel;  /* Gain level         */
    int nTOP;        /* TOP                */
    int nType;       /* AGC Type : Internal or External */
    int bPower3_3;   /* Power Type          */
    int nSelAGC;     /* AGC1 or AGC2        */
}MxL601ConfigInfo, *lpMxL601ConfigInfo;


/* Initialize MxL601 */
AVL63X1_ErrorCode SemcoMxL601_Initialize(struct AVL_Tuner *pTuner);
/* Soft Reset */
AVL63X1_ErrorCode SemcoMxL601_SoftReset(struct AVL_Tuner *pTuner);
/* Set IF */
AVL63X1_ErrorCode SemcoMxL601_SetIF(struct AVL_Tuner * pTuner);
/* Set AGC */
AVL63X1_ErrorCode SemcoMxL601_SetAGC(struct AVL_Tuner * pTuner);
/* Set Application Mode */
AVL63X1_ErrorCode SemcoMxL601_SetMode(struct AVL_Tuner * pTuner);
/* Set 1/2RF Performance  */
AVL63X1_ErrorCode SemcoMxL601_SetRFPerformance(struct AVL_Tuner * pTuner);
/* Set Xtal and Power voltage */
AVL63X1_ErrorCode SemcoMxL601_SetXtalPower(struct AVL_Tuner * pTuner);
/* Set Frequency */
AVL63X1_ErrorCode SemcoMxL601_Lock(struct AVL_Tuner * pTuner);
/* Get Lock Status */
AVL63X1_ErrorCode SemcoMxL601_GetLockStatus(struct AVL_Tuner * pTuner);
/* Get Power Level */
float SemcoMxL601_GetPowerLevel(struct AVL_Tuner * pTuner);
/* Get Chip Information */
void SemcoMxL601_GetChipInfo(struct AVL_Tuner * pTuner, int *lpChipId, int *lpChipVer);

#if defined( __cplusplus )
}
#endif

#endif
