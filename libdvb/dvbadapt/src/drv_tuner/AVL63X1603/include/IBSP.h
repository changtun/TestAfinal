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
/// @brief Defines the BSP functions which users need to implement. 
/// @details These BSP functions are called by SDK API functions. This file also defines some hardware related macros which also need to be 
/// customized by users according to their hardware platform. Most of the functions declared here should 
/// NOT be called by users' applications explicitly.  Exceptions are ::AVL63X1_IBSP_Initialize and ::AVL63X1_IBSP_Dispose, which may be
/// directly called by users' applications.
///
//$Revision: 355 $ 
//$Date: 2008-04-21 16:12:24 -0400 (Mon, 21 Apr 2008) $
// 
#ifndef IBSP_h_h
	#define IBSP_h_h

	#include "AVL63X1_include.h"

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

	#define MAX_II2C_READ_SIZE	64		///< The maximum number of bytes the back end chip can handle in a single I2C read operation. This value must >= 2.
	#define MAX_II2C_WRITE_SIZE	64		///< The maximum number of bytes the back end chip can handle in a single I2C write operation. This value must >= 8.

	/// Implements a time delay of uiDelay_ms milliseconds.
	/// 
	/// @param uiDelay_ms: The time delay in units of milliseconds.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the time delay has been implemented. 
	AVL63X1_ErrorCode AVL63X1_IBSP_Delay( AVL_uint32 uiDelay_ms );

	/// Destroys all of the resources which were allocated in AVL63X1_IBSP_Initialize and during BSP operations.
	/// 
	/// @remarks This function is never called inside the SDK. The user can redeclare the function to any prototype.
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the dispose operation was successful. 
	AVL63X1_ErrorCode AVL63X1_IBSP_Dispose(void);

	/// Performs an I2C read operation.
	/// 
	/// @param uiSlaveAddr The I2C device's slave address.  This can be the address of either the AVL DTMB device or the tuner.  This slave address does not include the least significant R/W bit.  Depending on the user's hardware, the user may need to shift the slave address to the left by one bit. 
	/// @param pucBuff The buffer which contains the read data.
	/// @param puiSize As an input, specifies the number of bytes to read. As an output, specifies the number of bytes that have been read. If there is an error, the function sets this value to 0.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful. 
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// @remarks This function should perform a direct I2C read operation without having to initially write the device’s internal address.  The SDK automatically handles writing the device internal address.
	AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Read( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_puint16 puiSize );

	/// Performs an I2C write operation.
	/// 
	/// @param uiSlaveAddr The I2C device's slave address.  This can be the address of either the AVL DTMB device or the tuner.  This slave address does not include the least significant R/W bit.  Depending on the user's hardware, the user may need to shift the slave address to the left by one bit.
	/// @param pucBuff The buffer which contains the write data.  
	/// @param puiSize As an input, specifies the number of bytes to write. As an output, specifes the number of bytes that have been written.  If the I2C write fails, the function sets this value to 0.  
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the I2C write operation is successful. 
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Write( AVL_uint16 uiSlaveAddr,  AVL_puchar pucBuff, AVL_puint16 puiSize );

	/// Performs any steps which are necessary before any BSP operations can be performed.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the initialization is successful. 
	/// Return ::AVL63X1_EC_GENERAL_FAIL if the BSP initialization fails. 
	/// @remarks This function is never called inside the SDK. The user can redeclare this function to any prototype.
	AVL63X1_ErrorCode AVL63X1_IBSP_Initialize(void);

	/// Initializes a semaphore object.
	/// 
	/// @param pSemaphore Pointer to the ::AVL_semaphore object which is being initialized.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the semaphore initialization is successful. 
	/// @remarks All of the semaphore objects should be initialized with 1 as the maximum count and the initialized state should be signaled. That is, after initialization, the first query should succeed.
	AVL63X1_ErrorCode AVL63X1_IBSP_InitSemaphore( AVL_psemaphore pSemaphore ); 

	/// Releases the semaphore.
	/// 
	/// @param pSemaphore Pointer to the ::AVL_semaphore object that is being released.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the semaphore is successfully released. 
	AVL63X1_ErrorCode AVL63X1_IBSP_ReleaseSemaphore( AVL_psemaphore pSemaphore );

	/// Queries the semaphore. if the semaphore is held by others, then the function should be blocked until the semaphore is available.
	/// 
	/// @param pSemaphore Pointer to the ::AVL_semaphore object that is being queried.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the wait operation was successful. 
	AVL63X1_ErrorCode AVL63X1_IBSP_WaitSemaphore( AVL_psemaphore pSemaphore );         

      AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Write_Reset(  AVL_uint16 uiSlaveAddr );


	#ifdef AVL_CPLUSPLUS
}
	#endif
#endif
