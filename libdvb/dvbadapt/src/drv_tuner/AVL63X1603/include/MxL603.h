#ifndef MxL603_H
#define MxL603_H

	#include "MxL603_TunerApi.h"
	#include "MxL603_OEM_Drv.h"
	#include "ITuner.h"

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

		AVL63X1_ErrorCode MxL603_Initialize(struct AVL_Tuner *pTuner);
		AVL63X1_ErrorCode MxL603_GetStatus(struct AVL_Tuner *pTuner);
		AVL63X1_ErrorCode MxL603_Lock(struct AVL_Tuner *pTuner);

	#ifdef AVL_CPLUSPLUS
}
	#endif

#endif
