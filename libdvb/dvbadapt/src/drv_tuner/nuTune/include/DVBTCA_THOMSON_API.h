/*****************************************
File: DVBTCA_THOMSON_API.h
*******************************************/
#ifndef _DVBTCA_THOMSON_API_H
#define _DVBTCA_THOMSON_API_H


#include "drv_tuner.h"

#ifdef __cplusplus 
extern "C" { 
#endif 

/* BEGIN: Added by zhwu, 2013/3/6 */
typedef struct
{

	U32                 FrequencyKhz;
	U32                 SymbolRate;
	U32                 Modulation;
	U32 		        FecRate;
	U32 			  	FecType;
	tPI_Modulation  	Polarity;
	U32                 LnbState;
	U32                 LockState;
	U32                 SignalQuality;
	U32                 SignalStrength;
	U32                 BitErrorRate;

}Tun_Info_t;
/* END:   Added by zhwu, 2013/3/6 */
extern TUNER_ErrorCode_t DVBTCA_THOMSON_DrvInit(U8 Tuner_ID);
extern TUNER_ErrorCode_t DRV_Tuner_ThomsonSet(ts_src_info_t *pScanInfo);
extern U8 		THOMSON_DrvModeChange(ts_src_info_t *pScanInfo);
extern void 	Tuner_GetFELockThomson(U8 * tRetLock );
extern void		Tuner_GetFEInfoThomson(Tun_Info_t *ptTunerInfo);
extern U32 		Tuner_GetChannelThomson(void);




#ifdef __cplusplus 
} 
#endif 
#endif


