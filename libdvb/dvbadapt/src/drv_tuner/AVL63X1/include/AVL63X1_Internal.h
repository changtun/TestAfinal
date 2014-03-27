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
/// @brief This interface declares functions for the initialization, control, and information access capabilities for the AVL63X1 device from a host processor
///

#ifndef AVL63X1_IBASE_H
#define AVL63X1_IBASE_H

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif
	///IBase Module
	#define OP_RX_IDLE              0x00
	#define OP_RX_LD_DEFAULT        0x01
	#define OP_RX_ACQUIRE           0x02
	#define OP_RX_HALT              0x03
	#define OP_RX_CMD_DEBUG         0x04	
	#define OP_RX_SDRAM_TEST        0x05 
	#define OP_RX_SLEEP             0x06
	#define OP_RX_WAKE_UP           0x07
	#define OP_RX_INIT_SDRAM        0x08
	#define OP_RX_INIT_ADC          0x09
	#define OP_RX_CHANGE_MODE       0x0A

	///II2C_Repeater Module
	#define I2CM_CMD_LENGTH   0x14
	#define I2CM_RSP_LENGTH   0x14
	#define OP_I2CM_NOOP      0x00
	#define OP_I2CM_WRITE     0x01
	#define OP_I2CM_READ      0x02
	
	#define AVL63X1_API_VER_MAJOR	0x02
	#define AVL63X1_API_VER_MINOR	0x02
	#define AVL63X1_API_VER_BUILD	0x00

	///
	/// Declaration of the global PLL configuration array variable. The variable must be defined in IBSP.c
	extern const struct AVL63X1_PLL_Conf pll_conf[];

	///
	/// Declaration of the global SDRAM configuration array variable. The variable must be defined in IBSP.c
	extern struct AVL63X1_SDRAM_Conf sdram_conf;

	///
	/// Declaration of the size of the global PLL configuration array variable.
	extern const AVL_uint16 pll_array_size;

	/// Initialize an ::AVL63X1_Chip object. Users first define an object of the ::AVL63X1_chip, and then pass the address of the object to this function. This function will initialize semaphores for the object and fill in the I2C slave address.
	/// 
	/// @param pAVL63X1_ChipObject     Pointer to the ::AVL63X1_Chip object to be initialized.
	/// 
	/// @return ::AVL63X1_ErrorCode, Returns ::AVL63X1_EC_OK if everything is OK.
	/// @remarks This function will initialize the semaphores without any checking. It is the user's responsibility to make sure that each object is only initialized once. Otherwise, there may be a semaphore resource leak.
	AVL63X1_ErrorCode Init_ChipObject_63X1(struct AVL63X1_Chip * pAVL63X1_ChipObject);
	

	///*--------------------IRx Module--------------------*///

	/// Initializes the receiver.
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the receiver is being initialized.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the receiver has been successfully initialized.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function must be called before all other functions declared in this interface.
	AVL63X1_ErrorCode IRx_Initialize_63X1(struct AVL63X1_Chip * pAVL_Chip);

	/// Adds a 32-bit unsigned integer to a 64-bit unsigned integer.  Stores the result in a 64-bit unsigned integer.
	///
	/// @param pSum Contains the 64-bit addend.  Also carries back the resulting sum.
	/// @param uiAddend Contains the 32-bit addend.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	void Add32To64(struct AVL63X1_uint64 *pSum, AVL_uint32 uiAddend);

	/// Divides two 64-bit unsigned integers.  Stores the result in a 64-bit unsigned integer.
	///
	/// @param y Contains the 64-bit divisor.  Also carries back the result.
	/// @param x Contains the 64-bit dividend.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	AVL_uint32 Divide64(struct AVL63X1_uint64 y, struct AVL63X1_uint64 x);


	/// Compares two 64-bit unsigned integers to determine whether the first integer is greater than or equal to the second integer.
	///
	/// @param a Number which is compared.
	/// @param b Number against which the comparison takes place.
	///
	/// @return :: Returns 1 if a >= b, 0 otherwise.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	AVL_uint32 GreaterThanOrEqual64(struct AVL63X1_uint64 a, struct AVL63X1_uint64 b);

	/// Subtracts a 64-bit unsigned integer (the subtrahend) from another 64-bit unsigned integer (the minuend).  Stores the result in a 64-bit unsigned integer.
	///
	/// @param pA Contains the 64-bit minuend.  This is the number from which the other input is subtracted.  The contents of pA must be larger than b.
	/// @param b Contains the 64-bit subtrahend.  Also stores the result of the subtraction operation.  This is the number which is subtracted from the other input.  Must be smaller than the contents of pA.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	void Subtract64(struct AVL63X1_uint64 *pA, struct AVL63X1_uint64 b);


	/// Multiplies two 32-bit unsigned integers.  Stores the result in a 64-bit unsigned integer.
	///
	/// @param pDst Carries back the 64-bit product of the multiplication.
	/// @param m1 Contains one of the 32-bit factors to be used in the multiplication.
	/// @param m2 Contains the other 32-bit factor to be used in the multiplication.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	void Multiply32(struct AVL63X1_uint64 *pDst, AVL_uint32 m1, AVL_uint32 m2);

	/// Shifts a 32-bit unsigned integer left by 16 bits and then adds the result to a 64-bit unsigned integer.  Stores the sum in a 64-bit unsigned integer.
	///
	/// @param pDst Contains the 64-bit addend.  Also carries back the resulting sum.
	/// @param a Contains the 32-bit input which is shifted and added to the other addend.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	void AddScaled32To64(struct AVL63X1_uint64 *pDst, AVL_uint32 a);

	/// Multiplies a 32-bit signed integer by a 16-bit signed integer.  Stores the result in a 64-bit signed integer.
	///
	/// @param pY Carries back the 64-bit product of the multiplication.
	/// @param a Contains the 32-bit factor to be used in the multiplication.
	/// @param b Contains the 16-bit factor to be used in the multiplication.
	///
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	void Multiply32By16(struct AVL63X1_int64 *pY, AVL_int32 a, AVL_int16 b);

	///*--------------------IBase Module--------------------*///

	/// Sets up the PLL.
	///
	/// @param pPLL_Conf  Pointer to the PLL configuration
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the PLL is being configured.
	///
	/// @return ::AVL63X1_ErrorCode, Returns ::AVL63X1_EC_OK if PLL configuration commands are sent successfully; Returns AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call it directly.
	AVL63X1_ErrorCode IBase_SetPLL_63X1(const struct AVL63X1_PLL_Conf * pPLL_Conf, struct AVL63X1_Chip * pAVL_Chip );

	/// Verifies that the last operational command sent to the receiver has completed.
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the command status is being determined.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the receiver has processed the last command sent to it.
	/// Return ::AVL63X1_EC_RUNNING if the AVL63X1 device is still processing the last command sent to it.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function is an 'internal' function. Availink does not recommend that users call it directly.
	AVL63X1_ErrorCode IBase_GetRxOP_Status_63X1(const struct AVL63X1_Chip * pAVL_Chip );

	/// Initializes the AVL63X1 device. This function loads program data into the chip and boots the chip.
	///
	/// @param pPLL_Conf  Pointer to the ::AVL63X1_PLL_Conf object which the function uses to initialize the PLL.
	/// @param pSDRAM_Conf Pointer to the ::AVL63X1_SDRAM_Conf object which the function uses to initialize the SDRAM controller.
	/// @param pMPEG_Info Pointer to the AVL63X1_MPEG_Info structure which the function uses to capture the back end decoder based MPEG configuration information in the chip structure for future reference.
	/// @param pInputSignal_Info Pointer to the AVL63X1_InputSignal_Info structure which the function uses to capture the type and the centre frequency of the input signal for each standard inside the chip structure for future reference.
	/// @param pInitialData Pointer to the buffer which holds the program data to be downloaded to the chip.
	/// @param demod_mode Indicates the desired standard to boot the device in.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object which is being initialized.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if all initialization data was successfully sent to the AVL63X1 device.
	/// Return ::AVL63X1_EC_RUNNING if the AVL63X1 device did not process the initialization command.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	AVL63X1_ErrorCode IBase_Initialize_63X1( const struct AVL63X1_PLL_Conf * pPLL_Conf, struct AVL63X1_SDRAM_Conf * pSDRAM_Conf, struct AVL63X1_Chip * pAVL_Chip );

	/// Sends an operational command to the AVL63X1 device.
	///
	/// @param ucOpCmd  The operation command code (OP_RX_xxx) defined in AVL63X1_IBase.h.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object to which the command is being sent.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the command has been sent to the AVL63X1 device.
	/// Return ::AVL63X1_EC_RUNNING if the command could not be sent because the AVL63X1 device is still processing a previous command.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call this function directly.
	AVL63X1_ErrorCode IBase_SendRxOP_63X1(AVL_uchar ucOpCmd, struct AVL63X1_Chip * pAVL_Chip );

	/// Tests the SDRAM 
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the SDRAM is being tested.
	/// @param puiTestResult Pointer to a variable in which the SDRAM test result will be written by the function.  If the all three tests passed, then this value is zero.  This value is set to a non-zero value incase any of the three tests fail.
	/// @param puiTestPattern Pointer to a variable in which the SDRAM test patton will be written by the function. 0- incase all three tests pass and a specific pattern incase one of them fails
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if test has run.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	AVL63X1_ErrorCode IBase_TestSDRAM_63X1(struct AVL63X1_Chip * pAVL_Chip, AVL_puint32 puiTestResult, AVL_puint32 puiTestPattern);

	/// Converts a value from units of nanoseconds to units of SDRAM clock periods.
	///
	/// @param SDRAM_ClkFrequency_Hz The SDRAM clock frequency in units of Hz.
	/// @param Value_ns The input value to convert.
	///
	/// @return ::AVL_uint32,
	/// Return ::The value in units of SDRAM clock periods.
	/// @remarks This function is an 'internal' function. Availink does not recommend that the user call this function directly.
	AVL_uint32 ConvertNsToSDRAM_Clocks(AVL_uint32 SDRAM_ClkFrequency_Hz, AVL_uint16 Value_ns);

	///*--------------------II2C Repeater Module--------------------*///

	/// Select a particular type of I2C repeater
	/// 
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object for which I2C repeater operation in firmware status is being queried.
	/// @param uiSetMode The mode value to set in the II2C repeater
	///
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the last I2C repeater operation is complete.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Repeater_SelectMode_63X1(AVL_uint32 uiSetMode , const struct AVL63X1_Chip * pAVLChip );

	/// Initializes the I2C repeater.
	/// 
	/// @param uiTunerSlaveAddr The I2C slave address of the tuner
	/// @param uiI2C_BusClock_kHz The clock speed of the I2C bus between the tuner and the AVL63X1 device.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the I2C repeater is being initialized.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if I2C repeater initialization is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// @remarks This function must be called before any other function in this interface and before any IBSP I2C functions are called to read or write data from or to the tuner.
	AVL63X1_ErrorCode II2C_Repeater_Initialize_63X1(AVL_uint16 uiTunerSlaveAddr, AVL_uint16 uiI2C_BusClock_kHz, struct AVL63X1_Chip * pAVL_Chip);

	/// Find II2C Repeater Info enty by address
	/// 
	/// @param uiTunerSlaveAddr The I2C slave address of the tuner
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the I2C repeater is being initialized.
	/// @param pIndex output parameters.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if I2C repeater initialization is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// @remarks This function must be called before any other function in this interface and before any IBSP I2C functions are called to read or write data from or to the tuner.
	AVL63X1_ErrorCode II2C_Repeater_FindByAddress_63X1(  AVL_uchar ucSlaveAddr, struct AVL63X1_Chip * pAVL_Chip, AVL_puint32 pIndex);

	/// Checks whether the tuner I2C address that was configured by calling the function AVL63X1_II2C_Repeater_Initialize fro I2C repeater in hardware matches the actual tuner I2C address.  
	/// 
	/// @param puiTunerAddrMismatch Pointer to a variable in which the result of the address check is placed.  If the configured tuner address matches the actual tuner address, the contents of the pointer is set to zero.  If there is an address mismatch, the contents of the pointer is set to 1.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the configured tuner address is being checked against the actual tuner address.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the tuner address status has been checked.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Repeater_CheckTunerAddress_63X1(AVL_puint32 puiTunerAddrMismatch, struct AVL63X1_Chip * pAVL_Chip);

	/// Configures the I2C repeater in hardware with the number of bytes that are to be read from the tuner.  This function must called each time a tuner read transaction is to be performed 
	/// using I2C repeater in hardware. The function must be called before AVL63X1_II2CRepeater_ReadData fucntion is called.  This function does not actually perform the tuner read operation.  
	/// 
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the I2C repeater in hardware is being configured for a read transaction.
	/// @param ucNumBytes The number of bytes to be read from the tuner
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the I2C repeater has been successfully configured with the number of read bytes.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Repeater_SetReadBytes_63X1(struct AVL63X1_Chip * pAVL_Chip, AVL_uchar ucNumBytes);
	///*--------------------II2C Module--------------------*///

	/// Initializes the I2C interface.
	/// 
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the I2C sempahores are successfully initialized.
	/// Return ::AVL63X1_EC_BSP_ERROR1 or AVL63X1_EC_BSP_ERROR2, depending on the user implementation, if the semaphores cannot be initialized.
	/// @remarks This is an internal function. This function must be called before any I2C operations. The SDK already calls this function in ::Init_AVL63X1_ChipObject, so the user does not need to worry about calling this function.
	AVL63X1_ErrorCode II2C_Initialize_63X1(void);

	/// @endcond

	/// Reads uiSize bytes from the AVL63X1 device at internal address uiOffset.  The read data is stored in pucBuff.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiOffset  The AVL63X1 device internal address at which the read operation is to be performed.  
	/// @param pucBuff  Pointer to an array in which the function returns the data read from the AVL63X1 device.  
	/// @param uiSize Specifies the number of bytes to read.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// @remarks The function first performs an I2C write to tell the AVL63X1 device the internal address at which to perform the read.
	AVL63X1_ErrorCode II2C_Read_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiOffset, AVL_puchar pucBuff, AVL_uint16 uiSize);

	/// Reads back an 8 bit unsigned integer from the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the read operation is to be performed.
	/// @param puiData Pointer to a variable in which the function returns the read8 bit unsigned integer. 
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Read8_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puchar puiData );

	/// Reads back a 16 bit unsigned integer from the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the read operation is to be performed.
	/// @param puiData Pointer to a variable in which the function returns the read16 bit unsigned integer. 
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Read16_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint16 puiData );

	/// Reads a 32 bit unsigned integer from the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the read operation is to be performed.
	/// @param puiData Pointer to a variable in which the function returns the read16 bit unsigned integer. 
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Read32_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint32 puiData );

	/// Reads uiSize bytes from the AVL63X1 device.  Stores the read data in pucBuff.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param pucBuff  Pointer to a buffer in which the function returns the read data.  
	/// @param uiSize The number of bytes to read.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the read operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_ReadDirect_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize);

	/// Performs an direct I2C write operation.
	/// 
	/// @param uiSlaveAddr  The slave address of the device to which the write operation is being performed.
	/// @param pucBuff  Pointer to an array which contains the data to be written to the target device. 
	/// @param uiSize Specifies the number of bytes to be sent to the target device. 
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the write operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_WriteDirect_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize);

	/// Performs an I2C write operation to the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param pucBuff  Pointer to an array which contains the data to be written to the AVL63X1 device. 
	/// @param uiSize Specifies the number of bytes to be sent to the AVL63X1 device.  The number of bytes must be greater than or equal to 3.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the write operation is successful.
	/// Return ::AVL63X1_GENERAL_FAIL if the write size is invalid.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Write_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize);

	/// Writes an 8 bit unsigned integer to the AVL63X1 device at the internal address specified by uiAddr.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the write operation is to be performed.
	/// @param ucData The data to be written to the AVL63X1 device.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the write operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Write8_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uchar ucData );

	/// Writes a 16 bit unsigned integer to the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the write operation is to be performed.  
	/// @param uiData The data to be written to the AVL63X1 device.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the write operation is successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Write16_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint16 uiData );

	/// Writes a 32 bit unsigned integer to the AVL63X1 device.
	/// 
	/// @param uiSlaveAddr  The AVL63X1 device slave address.
	/// @param uiAddr  The AVL63X1 device internal address at which the write operation is to be performed.  
	/// @param uiData The data to be written to the AVL63X1 device.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the write operation was successful.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	AVL63X1_ErrorCode II2C_Write32_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint32 uiData );
	///II2C_Repeater Module
	/// Sends an I2C repeater operational command to the Availink device. This function is internally called by SDK functions 
	/// AVL63X1_II2CRepeater_ReadData and AVL63X1_ErrorCode AVL63X1_II2CRepeater_WriteData when using I2C repeater module in firmware. 
	/// 
	/// @param pBuff Pointer to the array which contains the operational command and its parameters.
	/// @param ucSize The number of command related bytes in the array to which pBuff points.
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object to which an I2C repeater operational command is being sent.
	/// 
	/// Return ::AVL63X1_EC_OK if the I2C repeater operational command has been sent to the Availnk device.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_Running if the command could not be sent to the Availink device because the device is still processing a previous command.
	AVL63X1_ErrorCode II2C_Repeater_SendOP_63X1(AVL_puchar pBuff, AVL_uchar ucSize, struct AVL63X1_Chip * pAVLChip );

	/// Checks if the last I2C repeater operation in firmware is finished. This function is internally called by SDK functions 
	/// AVL63X1_II2CRepeater_ReadData and AVL63X1_ErrorCode AVL63X1_II2CRepeater_WriteData when using I2C repeater module in firmware. 
	/// 
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object for which I2C repeater operation in firmware status is being queried.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the last I2C repeater operation is complete.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_Running if the Availink device is still processing the last I2C repeater operation in firmware.
	AVL63X1_ErrorCode II2C_Repeater_GetOPStatus_63X1(const struct AVL63X1_Chip * pAVLChip );

	/// Halts AVL63X1 device signal processing.  This function places the AVL63X1 device into the AVL63X1_RL_HALT running level.  To place the AVL63X1 device into the AVL63X1_RL_NORMAL running level, the user must call the function LockChannel.
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which signal processing is being halted.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the AVL63X1 device has been placed in the halt running level.
	/// Return ::AVL63X1_EC_RUNNING if the halt command could not be sent to the AVL63X1 device because the AVL63X1 device was still processing a previous command.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	AVL63X1_ErrorCode IBase_Halt_63X1( struct AVL63X1_Chip * pAVL_Chip );

	/// Gets the running level of the AVL63X1 device.
	///
	/// @param pRunningLevel Pointer to a variable which the funciton uses to return the AVL63X1 device running level.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the running level is being read.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the running level has been read successfully.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	AVL63X1_ErrorCode IBase_GetRunningLevel_63X1( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip );

	#ifdef AVL_CPLUSPLUS
}
	#endif

#endif

