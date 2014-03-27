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
/// @brief Defines the primary data types according to the target platform. All data 
/// types defined in this file should be overwritten by users according to 
/// their own target platform.
/// 
//$Revision: 255 $ 
//$Date: 2008-02-06 10:37:01 -0500 (Wed, 06 Feb 2008) $
// 
#ifndef bspdatadef_h_h
#define bspdatadef_h_h

//#define AVL_CPLUSPLUS		///< Used to switch the C++ and C compiler. Comment the macro AVL_CPLUSPLUS if you use C compiler

typedef  char AVL_char;		///< 8 bit signed char data type.
typedef  unsigned char AVL_uchar;	///< 8 bit unsigned char data type.

typedef  short AVL_int16;	///< 16 bit signed int data type.
typedef  unsigned short AVL_uint16;	///< 16 bit unsigned int data type.

typedef  int AVL_int32;		///< 32 bit signed int data type.
typedef  unsigned int AVL_uint32;	///< 32 bit unsigned int data type.

typedef  char * AVL_pchar;	///< pointer to an 8 bit signed char data type.
typedef  unsigned char * AVL_puchar; ///< pointer to an 8 bit unsigned char data type.

typedef  short * AVL_pint16;	///< pointer to a 16 bit signed int data type.
typedef  unsigned short * AVL_puint16;	///< pointer to a 16 bit unsigned int data type.

typedef  int * AVL_pint32;	///< pointer to a 32 bit signed int data type.
typedef  unsigned int * AVL_puint32; ///< pointer to a 32 bit unsigned int data type.

/* BEGIN: Modified by zhwu, 2012/10/18 */
//typedef unsigned char AVL_semaphore; 	///< semaphore data type.
//typedef unsigned char * AVL_psemaphore;		///< pointer to a semaphore data type.
typedef unsigned int AVL_semaphore;
typedef unsigned int * AVL_psemaphore;
/* END:   Modified by zhwu, 2012/10/18 */


#endif

