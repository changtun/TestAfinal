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
/// @brief Declares functions for the receiver component.  The functions are used to configure receiver components, to lock channels,
/// and to retrieve the status of the receiver.
/// @details Although it is possible to call these functions from as many threads as the user wishes, Availink recommends putting
/// all "control" functions in one single thread to avoid conflicting results. For instance, suppose that thread A requests that the
/// receiver lock to a channel using 4QAM modulation.  Suppose that thread B requests the receiver to lock to a channel using 16QAM
/// modulation.  Both thread complete; however, the locked modulation type will be uncertain.
///
//$Revision: 534 $
//$Date: 2011-08-09 $
//
#ifndef AVL63X1_API_H
#define AVL63X1_API_H

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

	///*--------------------IRx Module--------------------*///
	
	/// Reads the current BER.  This function is provided for BER testing.  The BER measurement can only be performed if the transmitter is generating either an LFSR-15 or an LFSR-23 data pattern.  The BER measurement cannot be performed with a signal that is carrying video data.
	///
	/// @param pErrorStats Stores the variables used in the BER calculation.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the BER is being read.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the BER is successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_GetBER( struct AVL63X1_ErrorStats *pErrorStats , const struct AVL63X1_Chip * pAVL_Chip );


	/// Reads back the current spectrum inversion status.
	///
	/// @param pSpectrumInversion Carries back the current spectrum inversion status.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the spectrum inversion status is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the spectrum inversion status is successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip);

	/// Get the times which chip retry to lock the specified channel.
	/// @param puiNumLockRetries Pointer to a variable in which the function returns the times chip retry to lock the channel.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which lock status is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the lock status has been successfully retrieved.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_GENERAL_FAIL if current chip working mode is not DTMB.
	/// @remarks This function should be called after the function AVL63X1_LockChannel to determine how many times chip retry to lock channel, and this can be used to determine whether this channel can be locked or not.
	AVL63X1_ErrorCode AVL63X1_GetLockRetries(AVL_puint16 puiNumLockRetries,const struct AVL63X1_Chip * pAVL_Chip );


	/// Checks the current AVL63X1 device lock status.  The function reports whether channel lock has been achieved as well as the number of times that lock has been attempted thus far.
	///
	/// @param puiLockStatus Pointer to a variable in which the function returns the channel lock status.  If the AVL63X1 device has locked to the channel, the function writes a 1 to the pointer contents.  If the AVL63X1 device has not locked to the channel, the function writes a 0 to the pointer contents.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which lock status is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the lock status has been successfully retrieved.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function should be called after the function AVL63X1_LockChannel to determine whether the AVL63X1 device was able to lock to the channel.  The user may poll the lock status by calling this function repeatedly.  It is recommended that the number of lock retries returned by the function be used to determine whether a channel is present.  For example, if the number of lock retries exceeds ten, the user may conclude that a carrier is either not present or is of insufficient quality for demodulation.
    AVL63X1_ErrorCode AVL63X1_GetLockStatus( AVL_puint16 puiLockStatus, const struct AVL63X1_Chip * pAVL_Chip );


	/// Reads the current PER.
	///
	/// @param pErrorStats Pointer to the structure which stores the variables used in the PER calculation.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the PER is being read.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the PER is successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_GetPER( struct AVL63X1_ErrorStats *pErrorStats, const struct AVL63X1_Chip * pAVL_Chip );


	/// Reads the carrier frequency offset.
	///
	/// @param piFreqOffsetHz Carries back the frequency offset of the carrier in units of Hz.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the carrier frequency offset is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the carrier frequency offset is successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function should be called after the AVL63X1 device is locked to the input signal.
    AVL63X1_ErrorCode AVL63X1_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip );


	/// Retrieves the signal information parameters of the received signal.
	///
	/// @param pSignalInfo Pointer to an object to which the functions returns the detected signal information.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which signal information is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the signal information has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C problem.
	/// @remarks This function can be called after the receiver has locked to a channel, and it is particularly useful if the lock operation has been performed in automatic mode.  Once channel lock is established, this function may be called to read the signal parameters that have been automatically detected by the AVL63X1 device.
    AVL63X1_ErrorCode AVL63X1_GetSignalInfo(void *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip );


	/// Gets the UHF/VHF input signal level.
	///
	/// @param puiSignalLevel Pointer to a variable in which the function returns the signal level as a value ranging from 0 to 65535, with 0 corresponding to the weakest signal representation and 65535 corresponding to the strongest signal representation.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the UHF/VHF input signal level is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the signal level has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function actually returns back the value of the analog AGC gain. The corresponding  input power in dbm varies depending on the tuner. The user needs to derive the relationship between AGC gain value and UHF/VHF input signal level in dBm from the tuner data sheet or through measurement.
    AVL63X1_ErrorCode AVL63X1_GetStrength(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip );


	/// Reads the current SNR estimate.
	///
	/// @param puiSNR_db Pointer to a variable in which the function returns the current estimated SNR value. The value is scaled by 100. For example, reading back decimal 2578 means that the SNR estimate is 25.78 db.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the SNR is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the SNR has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks It takes a few seconds for the chip to calculate a stable SNR value after the FEC locks. While the SNR calculation is stabilizing, the function reports an SNR of 0.
    AVL63X1_ErrorCode AVL63X1_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip );


	/// Initializes the receiver.
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the receiver is being initialized.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the receiver has been successfully initialized.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// @remarks This function must be called before all other functions declared in this interface.
    AVL63X1_ErrorCode AVL63X1_Initialize(struct AVL63X1_Chip * pAVL_Chip);


	/// Locks to a channel.  The channel may be locked using one of two modes as specified by the enumLockMode input parameter.
	///
	/// @param enumLockMode The channel lock mode.  See the description for ::AVL63X1_LockMode.
	/// @param psChannel Holds all of the parameters needed by the AVL63X1 device to lock to the input signal.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the lock information and lock command have been successfully sent to the AVL63X1 device.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_GENERAL_FAIL if the user is attempting to lock to the channel in manual mode and the signal information specified in psChannel is invalid.
	/// Return ::AVL63X1_EC_RUNNING if the lock command could not be sent to the AVL63X1 device because the AVL63X1 device is still processing a previous command.
	/// @remarks The function GetLockStatus can be called after this function to determine whether the AVL63X1 device has successfully locked to the channel.
    AVL63X1_ErrorCode AVL63X1_LockChannel( enum AVL63X1_LockMode enumLockMode, const void * psChannel );


	/// Configures the MPEG output mode.
	///
	/// @param demod_mode Indicates the requested standard.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the MPEG output mode is being configured.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the MPEG output mode has been configured.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_SetMPEG_Mode(const struct AVL63X1_Chip * pAVL_Chip );


	/// Sets the polarity of the analog AGC. This value should be set according to the specific tuner voltage/gain characteristics.
	///
	/// @param enumAGC_Pola The polarity of the analog AGC. Refer to ::AVL63X1_AnalogAGC_Pola.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the analog AGC polarity is being configured.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the analog AGC polarity has been configured.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_SetAnalogAGC_Pola( enum AVL63X1_AnalogAGC_Pola enumAGC_Pola, const struct AVL63X1_Chip * pAVL_Chip );


	/// Resets the BER statistics and starts the BER measurement.  The BER measurement can only be performed if the transmitter is generating either an LFSR-15 or an LFSR-23 data pattern.
	///
	/// @param pAVL_Chip Pointer to the AVL63X1_Chip object for which the BER measurement is being started.
	/// @param pErrorStats Pointer to the object which stores the variables used in the BER calculation.
	/// @param enumTestPattern The BER test pattern being generated by the test transmitter.
	/// @param enumLFSR_InvertFb Indicates whether the feedback bit of the LFSR used in the test pattern generation is inverted.
	/// @param puiBER_Sync Pointer to a variable in which the function returns an indication of whether the receiver has synchronized to the test pattern generated by the transmitter.  (0 - Not synchronized, 1 - Synchronized).
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the commands to start the BER measurement have been successfully sent to the AVL2108.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_StartBER( struct AVL63X1_Chip * pAVL_Chip, struct AVL63X1_ErrorStats * pErrorStats, enum AVL63X1_TestPattern enumTestPattern, enum AVL63X1_LFSR_FbBit enumLFSR_InvertFb, AVL_puint16 puiBER_Sync);


	/// Resets the PER statistics and starts the PER measurement.
	///
	/// @param pAVL_Chip Pointer to the AVL63X1_Chip object for which the PER measurement is being started.
	/// @param pErrorStats Pointer to the object which stores the variables used in the PER calculation.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the PER measurement has been started.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_StartPER( struct AVL63X1_Chip * pAVL_Chip, struct AVL63X1_ErrorStats * pErrorStats);


	/// Calculates the signal quality of the received signal.
	///
	/// @param puiSignal_Quality 		Pointer to a variable in which the function returns the Signal Quality value. This Signal Quality value ranges between 0 and 100.
	/// @param iNormalization_Factor	Normalization Factor. Value can be 4, 5 or 6. If any other value is fed to the function, the function initializes it to a default of 4.
	/// @param pAVL_Chip 				Pointer to the ::AVL63X1_Chip object for which signal information is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the signal information has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C problem.
	/// @remarks This function can be called after the receiver has locked to a channel.
    AVL63X1_ErrorCode AVL63X1_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip );


	/// Configures the AVL63X1 device to drive the MPEG output interface.
	/// 
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object which is to drive the MPEG output interface.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the device has been successfully configured to drive the MPEG output interface.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_DriveMpegOutput(enum AVL63X1_Switch         enumOn_Off, const struct AVL63X1_Chip * pAVL_Chip);


	/// Configures the AVL63X1 device to output the IF AGC signal.
	/// 
	/// @param pAVL_Chip A pointer to the ::AVL63X1_Chip object which is to output an IF AGC signal.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the device has been successfully configured to output an IF AGC signal.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_DriveIFAGC(enum AVL63X1_Switch         enumOn_Off,   const struct AVL63X1_Chip * pAVL_Chip );


	/// Disables MPEG continous clock mode. 
	/// 
	/// @param pAVL_Chip A pointer to the ::AVL63X1_Chip object for which MPEG continous clock mode is being disabled.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if MPEG continous clock mode is successfully disabled.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_DisableMpegContinuousMode(const struct AVL63X1_Chip * pAVL_Chip);


	/// Enables MPEG continous clock mode.  
	/// 
	/// @param uiByteClockFreq_Hz The desired MPEG output byte clock frequency .  Units of Hz.
	/// @param enumMpegMode Specifies the MPEG mode, either serial or parallel.
	/// @param pAVL_Chip A pointer to the ::AVL63X1_Chip object for which MPEG continous clock mode is being enabled.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if MPEG continous clock mode is successfully enabled.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_EnableMpegContinuousMode(const struct AVL63X1_Chip * pAVL_Chip, AVL_uint32 uiByteClockFreq_Hz) ;


	/// Reads back the scatter data from the AVL63X1 device.
	///
	/// @param puiIQ_Data Carries back the complex scatter data samples.  The 16 MSBs of each sample contain the I component and the 16 LSBs of each sample contain the Q component.
	/// @param puiSize As an input, specifies the number of IQ pairs to read; as an output, specifies the number of IQ pairs that were read.  The maximum number of IQ pairs that may be read is 256.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the scatter data is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the scatter data has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_GENERAL_FAIL if the received signal is a single carrier signal.
	/// @remarks A maximum of 256/512 complex samples (IQ pairs) may be read by calling this function.  If the caller requests more than 256/512 complex samples, the function returns only 256/512 complex samples.
    AVL63X1_ErrorCode AVL63X1_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip );



	///*--------------------IBase Module--------------------*///
    
	/// Change Demod operational mode 
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the mode is being changed.
	/// @param demod_mode Indicates the requested standard to which the device's mode is required to be changed .
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if test has run.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_SwitchMode(struct AVL63X1_Chip * pAVL_Chip, AVL_uchar demod_mode);



	/// Places the AVL63X1 device in sleep mode.  In sleep mode, all hardware in the dig core is hold in reset.
	/// 
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip which is being placed in sleep mode.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the operational command has been sent to the device.
	/// Return ::AVL63X1_EC_Running if the command could not be sent because the device is still processing a previous command.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_Sleep( struct AVL63X1_Chip * pAVL_Chip );


	/// Wakes the AVL63X1 device from sleep mode.  Upon waking from sleep, the SDK need to boot the cpu again before locking to any channel.
	/// 
	/// @param pAVLChip Pointer to the ::AVL63X1_Chip which is being waken from sleep.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the operational command has been sent to the device.
	/// Return ::AVL63X1_EC_Running if the command could not be sent because the device is still processing a previous command.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_Wakeup( struct AVL63X1_Chip * pAVL_Chip );


	/// Checks whether the AVL63X1 device has completed its initialization procedure.
	///
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the status is being checked.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if AVL63X1 device initialization is complete.
	/// Return ::AVL63X1_EC_GENERAL_FAIL if the AVL63X1 device has not successfully booted.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_CheckChipReady( struct AVL63X1_Chip * pAVL_Chip );


	/// Obtains the AVL63X1 device version information.
	///
	/// @param pVerInfo Returns the version information. Refer to ::AVL63X1_VerInfo for details.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the version information is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the version information has been read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
    AVL63X1_ErrorCode AVL63X1_GetVersion( struct AVL63X1_VerInfo * pVerInfo, const struct AVL63X1_Chip * pAVL_Chip );



	///*--------------------II2C_Repeater Module--------------------*///
	
	/// Reads data back from the tuner via the I2C repeater module in both firmware and hardware.  This function is used with tuners which insert a stop
	/// bit between messages.
	/// 
	/// @param ucSlaveAddr The slave address of the tuner device. Please note that the Availink device only supports a 7 bit slave address.
	/// @param pucBuff Pointer to a buffer in which to store the data read from the tuner.
	/// @param uiSize The number of bytes to read from the tuner.  The maximum value is 20.
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object for which data is being read from the tuner.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the data has been read from the tuner.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_Running if the read commmand could not be sent to the Availink device because the device is still processing  a previous command.
	/// Return ::AVL63X1_EC_GeneralFail if uiSize is larger than 20.
	/// @remarks This function will trigger a I2C read operation. It is used with tuners which insert a stop bit between messages.  The read position (or device internal address) can be determined by calling ::AVL63X1_II2CRepeater_WriteData.
	AVL63X1_ErrorCode AVL63X1_II2C_Repeater_ReadData(  AVL_uchar ucSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip );


	/// Writes data to the tuner via the I2C repeater module.
	/// 
	/// @param ucSlaveAddr The slave address of the tuner device. Please note that the Availink device only supports a 7 bit slave address.
	/// @param ucBuff Pointer to the buffer which contains the data to be sent to the tuner.
	/// @param uiSize The number of bytes to be sent to the tuner.  The maximum value is 17.
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object for which data is being sent to the tuner.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the send command has been sent to the Availink device.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_Running if the send command could not be sent to the Availink device because the device is still processing a previous command.
	/// Return ::AVL63X1_EC_GeneralFail if \a uiSize is larger than 17.
	/// @remarks The internal register address is buried in the buffer to which ucBuff points. This function is a nonblocking function.  Call ::AVL63X1_II2CRepeater_GetOPStatus to determine if the write operation is complete.
	AVL63X1_ErrorCode AVL63X1_II2C_Repeater_WriteData(  AVL_uchar ucSlaveAddr, const AVL_puchar ucBuff, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip );


	/// Reads data back from the tuner via the I2C repeater.  This function is used with tuners which do not insert a stop
	/// bit between messages.
	/// 
	/// @param ucSlaveAddr The slave address of the tuner device. Please note that the Availink device only supports a 7 bit slave address.
	/// @param pucBuff Pointer to the buffer in which to store the read data.
	/// @param ucRegAddr The address of the register being read.
	/// @param uiSize The number of bytes to read from the tuner.  The maximum value is 20.
	/// @param pAVLChip A pointer to the ::AVL63X1_Chip object for which data is being read from the tuner.
	/// 
	/// @return ::AVL63X1_ErrorCode, 
	/// Return ::AVL63X1_EC_OK if the data has been read from the tuner.
	/// Return ::AVL63X1_EC_I2CFail if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_Running if the read command could not be sent to the Availink device because the device is still processing a previous command.
	/// Return ::AVL63X1_EC_GeneralFail if \a uiSize is larger than 20.
	/// @remarks This function will trigger a I2C read operation. It is used with tuners which do not insert a stop bit between messages.
	AVL63X1_ErrorCode AVL63X1_II2C_Repeater_ReadData_Multi(  AVL_uchar ucSlaveAddr, AVL_puchar pucBuff, AVL_uchar ucRegAddr, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip );
    
	/// Reads back the BER before Reed-Solomon Code from the AVL63X1 device.
	///
	/// @param puiBER_BeforeRS Carries back the BER before Reed-Solomon Code.
	/// @param pAVL_Chip Pointer to the ::AVL63X1_Chip object for which the BER is being retrieved.
	///
	/// @return ::AVL63X1_ErrorCode,
	/// Return ::AVL63X1_EC_OK if the scatter data has been successfully read.
	/// Return ::AVL63X1_EC_I2C_FAIL if there is an I2C communication problem.
	/// Return ::AVL63X1_EC_GENERAL_FAIL if the received signal is a single carrier signal.
	AVL63X1_ErrorCode AVL63X1_DVBC_GetBER_BeforeRS( AVL_puint32 puiBER_BeforeRS,const struct AVL63X1_Chip * pAVL_Chip );
    
	#ifdef AVL_CPLUSPLUS
}
	#endif

#endif

