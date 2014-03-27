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



///
/// @file
/// @brief This file include the remaining SDK header files located under different sub-directories and paths
/// 

#ifndef AVL63X1_INCLUDE_H
#define AVL63X1_INCLUDE_H
	
	#include "bspdatadef.h"
    
	#include "AVL63X1.h"
	#include "AVL63X1_globals.h"
	#include "AVL63X1_Internal.h"
	#include "AVL63X1_API.h"
	#include "AVL_DTMB.h"
	#include "AVL_DVBC.h"
	#include "IBSP.h"
	#include "pbitrace.h"

    #ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

	#define AVL_min(x,y) (((x) < (y)) ? (x) : (y))
	#define AVL_max(x,y) (((x) < (y)) ? (y) : (x))
	#define AVL_floor(a) (((a) == (int)(a))? ((int)(a)) : (((a) < 0)? ((int)((a)-1)) : ((int)(a))))
	#define AVL_ceil(a)  (((a) == (int)(a))? ((int)(a)) : (((a) < 0)? ((int)(a)) : ((int)((a)+1))))
	#define AVL_abs(a)  (((a)>0) ? (a) : (-(a)))
	
	/// The following macros define the different standards supported by the demod.
	/// They should be used to appropriately set the m_demod_mode member of the desired channel structure 
	/// to configure the demod to operate in one of the several supported standard modes.   	
	#define AVL_DEMOD_MODE_DTMB     0
	#define AVL_DEMOD_MODE_DVBC     1
	#define AVL_DEMOD_MODE_DVBTx    2
	#define AVL_DEMOD_MODE_DVBSx    3
	
	
	/// Chunk two bytes \a uidata in to \a pBuff.
	/// 
	/// @param uidata The input 2 bytes data.
	/// @param pBuff The destination buffer, at least 2 bytes length.
	/// 
	/// @remarks This function is used to eliminates the big endian and little endian problem.
	void Chunk16(AVL_uint16 uidata, AVL_puchar pBuff);

	/// Composes a ::AVL_uint16 from two bytes in a AVL_uchar array.
	/// 
	/// @param pBuff  The buffer has at least 2 bytes data.
	/// 
	/// @return AVL_uint16
	/// @remarks This function is used to eliminates the big endian and little endian problem.
	AVL_uint16 DeChunk16(const AVL_puchar pBuff);

	/// Chunk four bytes \a uidata in to \a pBuff.
	/// 
	/// @param uidata The input 3 bytes data.
	/// @param pBuff The destination buffer, at least 3 bytes length.
	/// 
	/// @remarks This function is used to eliminates the big endian and little endian problem.
	void Chunk32(AVL_uint32 uidata, AVL_puchar pBuff);

	/// Composes a ::AVL_uint16 from four bytes in a AVL_uchar array.
	/// 
	/// @param pBuff  The buffer has at least 4 bytes data.
	/// 
	/// @return AVL_uint32
	/// @remarks This function is used to eliminates the big endian and little endian problem.
	AVL_uint32 DeChunk32(const AVL_puchar pBuff);

	/// Chunk 3 byte of \a uiaddr into the \a pBuff
	/// 
	/// @param uiaddr The address. Only the three LSB bytes will be used.
	/// @param pBuff The destination buffer, at lease three bytes length.
	/// 
	/// @remarks This function is used to eliminates the big endian and little endian problem.
	void ChunkAddr(AVL_uint32 uiaddr, AVL_puchar pBuff);

	#ifdef AVL_CPLUSPLUS
}
	#endif


#endif

