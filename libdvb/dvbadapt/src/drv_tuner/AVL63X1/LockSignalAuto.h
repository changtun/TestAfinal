/*
 *           Copyright 2007-2012 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */



#ifndef LOCK_SIGNAL_AUTO_H
#define LOCK_SIGNAL_AUTO_H

#include "AVL63X1_include.h"
#include "SemcoMxL601.h"

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif


	/*---------------------------------*
	 |     user interface functions    |
	 *---------------------------------*/
	AVL63X1_ErrorCode AVL63X1_init(void);
	AVL63X1_ErrorCode demod_init(void);
	AVL63X1_ErrorCode tuner_init(void);
	AVL63X1_ErrorCode lock_channel(struct AVL_Tuner *ptuner);
	AVL63X1_ErrorCode lock_tuner(struct AVL_Tuner *ptuner);
	AVL63X1_ErrorCode get_lock_status(int *lock_status);
	AVL63X1_ErrorCode get_signal_info(AVL_DVBC_Channel *dvbc_channel);
	AVL63X1_ErrorCode get_signal_strength(unsigned short *strength);
	AVL63X1_ErrorCode get_signal_quality(int *quality);
	AVL63X1_ErrorCode get_per(float *per);
	AVL63X1_ErrorCode get_snr(float *snr);
	AVL63X1_ErrorCode get_ber_beforeRS(float *ber);

	/*----------------------------------------*
	 |    application auxiliary functions     |
	 *----------------------------------------*/
	void display_signal_info(void *pSignalInfo);
/*
	AVL63X1_ErrorCode get_per(void);
	AVL63X1_ErrorCode AVL63X1_halt(void);
	AVL63X1_ErrorCode AVL63X1_sleep(void);
	AVL63X1_ErrorCode AVL63X1_wakeup(void);
	AVL63X1_ErrorCode get_signal_snr(void);
*/


	#ifdef AVL_CPLUSPLUS
}
	#endif
#endif
