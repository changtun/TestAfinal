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


#ifndef ITUNER_H
#define ITUNER_H

#include "AVL63X1_include.h"

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

struct Tuner_Status
{
	AVL_uint16 m_uiReset;
	AVL_uint16 m_uiLock;
	AVL_uint16 m_uiALBC;
	AVL_uint16 m_uiAGC;
};

struct AVL_Tuner
{
	AVL_uint16 m_uiSlaveAddress;		///< The Tuner slave address. It is the write address of the tuner device. In particular it is an 8-bit address, with the LSB set to zero. The Availink device does not support 10-bit I2C addresses. 
	AVL_uint16 m_uiI2C_BusClockSpeed_kHz;		///< The clock speed of the I2C bus that is dedicated to tuner control.  The units are kHz.
	AVL_uint32 m_uiFrequency_Hz;	///< The tuned frequency in units of 100kHz.
	struct Tuner_Status m_sStatus;
	void * m_pParameters;				///< A pointer to the tuner's customized parameters baseband gain, etc.
	
	struct AVL63X1_Chip * m_pAVL_Chip;	///< A pointer to the Availink device connected to the Tuner.
	AVL63X1_ErrorCode (* m_pInitializeFunc)(struct AVL_Tuner *);	 	///< A pointer to the tuner initialization function.
	AVL63X1_ErrorCode (* m_pGetLockStatusFunc)(struct AVL_Tuner *); 	///< A pointer to the tuner GetLockStatus function.
	AVL63X1_ErrorCode (* m_pLockFunc)(struct AVL_Tuner *);			///< A pointer to the tuner Lock function.
};

	#ifdef AVL_CPLUSPLUS
}
	#endif
	
#endif

