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
/// @brief Defines data structures, enums, global macros for the AVL63X1 device.
/// 

#ifndef AVL63X1_H
#define AVL63X1_H

#include "AVL63X1_include.h"

	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

	typedef AVL_uint16 AVL63X1_ErrorCode;		// Defines the error code. 

	#define AVL63X1_EC_OK						0	// There is no error.
	#define AVL63X1_EC_GENERAL_FAIL				1	// A general failure has occurred.
	#define AVL63X1_EC_I2C_FAIL					2	// An I2C operation failed during communication with the AVL63X1 through the BSP.
	#define AVL63X1_EC_I2C_REPEATER_FAIL		4	// An error ocurred during communication between AVL63X1 I2C master and tuner. This error code is defined by enum AVL63X1_MessageType_II2C_Repeater_Status.													
	#define AVL63X1_EC_RUNNING					8	// The AVL63X1 device is busy processing a previous receiver/i2c repeater command.
	#define AVL63X1_EC_BSP_ERROR1				16	// A user defined error code for reporting BSP errors.
	#define AVL63X1_EC_BSP_ERROR2				32	// A user defined error code for reporting BSP errors.
	#define AVL63X1_EC_TIMEOUT					64	// Operation failed in a given time period

	#define TUNER_NUM							 2  	// the maximum number of tuner is 2 

	// Defines the I2C slave address options for the AVL63X1 device.
	typedef enum AVL63X1_ChipSlaveAddr
	{
		AVL63X1_SA_0 = 0x14, 		// =0x14 
		AVL63X1_SA_1 = 0x15, 		// =0x15
	}AVL63X1_ChipSlaveAddr;

	// Defines the AVL63X1 device spectrum inversion mode
	typedef enum AVL63X1_SpectrumInversion
	{
		SPECTRUM_NOT_INVERTED = 0,	// =0
		SPECTRUM_INVERTED = 1		// =1
		//SPECTRUM_AUTO = 2			// =2
	}AVL63X1_SpectrumInversion;

	// Defines the channel lock mode.
	typedef enum AVL63X1_LockMode
	{
		LOCK_MODE_AUTO = 0,			// =0
		LOCK_MODE_MANUAL = 1		// =1
	}AVL63X1_LockMode;

	// Defines the polarity of the analog AGC control signal.  It must be set to match the requirement of the tuner being used.
	// The default value in the AVL63X1 device is AGC_INVERTED
	typedef enum AVL63X1_AnalogAGC_Pola
	{
		AGC_NORMAL = 0,	        	// = 0  Normal. Used for a tuner whose gain increases with increased AGC voltage.
		AGC_INVERTED = 1			// = 1  Inverted. Used for tuner whose gain decreases with increased AGC voltage.
	}AVL63X1_AnalogAGC_Pola;

	// Defines the AVL63X1 device running levels.
	typedef enum AVL63X1_RunningLevel
	{
		AVL63X1_RL_HALT =  0,		// = 0 Halt running level.
		AVL63X1_RL_NORMAL = 2		// = 2 Normal running level.
	}AVL63X1_RunningLevel;

	// Defines the type of signal being input to the AVL63X1 device.  The default value in the AVL63X1 device is \a RX_COMPLEX_BASEBAND
	typedef enum AVL63X1_RxInput
	{
		RX_REAL_IF_I = 0,			// = 0 The input signal is a real IF signal routed to the AVL63X1 device I channel input.
		RX_REAL_IF_Q = 1,			// = 1 The input signal is a real IF signal routed to the AVL63X1 device Q channel input.
		RX_COMPLEX_BASEBAND = 2		// = 2 The input signal is a complex baseband signal.
	}AVL63X1_RxInput;

  
	// Defines the MPEG clock output mode.  This is configured to match the desired capture edge for the MPEG decoder device receiving the AVL63X1 device MPEG output signals.
	// The default value in the AVL63X1 device is AVL63X1_MPCM_RISING.
	typedef enum AVL63X1_MPEG_ClockMode
	{
		AVL63X1_MPCM_FALLING = 0,	//  =0  MPEG data should be sampled on the falling edge of the clock.
		AVL63X1_MPCM_RISING = 1		//  =1  MPEG data should be sampled on the rising edge of the clock.
	}AVL63X1_MPEG_ClockMode;
	
	// Defines the test pattern being used for BER/PER measurements.
	typedef enum AVL63X1_TestPattern
	{
		AVL63X1_TEST_LFSR_15 = 0,	// =0
		AVL63X1_TEST_LFSR_23 = 1	// =1
	}AVL63X1_TestPattern;

	// Defines whether the feeback bit of the LFSR used to generate the BER/PER test pattern is inverted.
	typedef enum AVL63X1_LFSR_FbBit
	{
		AVL63X1_LFSR_FB_NOT_INVERTED = 0,	// =0
		AVL63X1_LFSR_FB_INVERTED = 1		// =1
	}AVL63X1_LFSR_FbBit;

	// Defines SDRAM configuration type(SIP or NON-SIP). The type of SRDAM impacts the phase delays in DDLL block.
	typedef enum AVL63X1_SDRAM_Type
	{
		SIP = 0,
		NON_SIP = 1
	}AVL63X1_SDRAM_Type;
	
	// The MPEG output mode. The default value in the AVL63X1 device chip is \a AVL63X1_MPM_PARALLEL
	typedef enum AVL63X1_MPEG_Mode
	{
		AVL63X1_MPM_PARALLEL = 0,			//=0  Output parallel MPEG data
		AVL63X1_MPM_SERIAL = 1				// =1  Output serial MPEG data via D7.
	}AVL63X1_MPEG_Mode;	

    // Defines MPEG Error Bit(Disable or Enable). 
	typedef enum AVL63X1_MPEG_ErrorBit
	{
		MPEG_ERROR_BIT_DISABLE = 0,
		MPEG_ERROR_BIT_ENABLE
	}AVL63X1_MPEG_ErrorBit;

	// The MPEG output format. The default value in the AVL63X1 device is \a AVL63X1_MPF_TS
	typedef enum AVL63X1_MpegFormat
	{
		AVL63X1_MPF_TS = 0,					// = 0  Transport stream format.
		AVL63X1_MPF_TSP = 1					// = 1  Transport stream plus parity format.
	}AVL63X1_MpegFormat;

	// Holds the MPEG configuration information.
	typedef struct AVL63X1_MPEG_Info
	{
		enum AVL63X1_MPEG_Mode m_mpm;
		enum AVL63X1_MPEG_ClockMode m_mpcm;
		enum AVL63X1_MpegFormat m_mpfm_DTMB_mode;
		enum AVL63X1_MpegFormat m_mpfm_DVBC_mode;
		enum AVL63X1_MPEG_ErrorBit m_MPEG_ErrorBit;
	}AVL63X1_MPEG_Info ;

	// Defines II2C Repeater Type. 
	typedef enum AVL63X1_II2C_Repeater_Type
	{
		MESSAGE_TYPE = 0x0, 
		BYTE_TYPE = 0x1,
		BIT_TYPE = 0x2
	}AVL63X1_II2C_Repeater_Type;

	// Defines error codes for Message type II2C repeater. 
	typedef enum AVL63X1_MessageType_II2C_Repeater_Status
	{
		I2CM_STATUS_OK = 0,
		I2CM_STATUS_BAD_OPCODE,
		I2CM_STATUS_SLAW_NACK,
		I2CM_STATUS_DATA_NACK,
		I2CM_STATUS_ARB_LOST,
		I2CM_STATUS_SLAR_NACK,
		I2CM_STATUS_BUS_ERROR,
		I2CM_STATUS_RD_MAX_BYTES,
		I2CM_STATUS_SI_NOT_SET,
		I2CM_STATUS_DATA_ACK_ON2ndLASTBYTE,
		I2CM_STATUS_PREV_STOP_NOTSENT,
		I2CM_STATUS_UNKNOWN
	}AVL63X1_MessageType_II2C_Repeater_Status;	

	// Defines the ON/OFF options for the AVL63X1 device.
	typedef enum AVL63X1_Switch
	{
		AVL63X1_ON, 
		AVL63X1_OFF
	}AVL63X1_Switch;
	
	// The Availink version structure.
	typedef struct AVL_VerInfo {
		AVL_uchar m_Major;					// The major version number
		AVL_uchar m_Minor;					// The minor version number
		AVL_uint16 m_Build;					// The build version number
	}AVL_VerInfo;
	
    // Stores AVL63X1 device version information.
	typedef struct AVL63X1_VerInfo
	{
		struct AVL_VerInfo m_Chip;			// Hardware version information. Should be X.X.X. 
		struct AVL_VerInfo m_API;			// SDK version information.
		struct AVL_VerInfo m_Patch;			// The version of the internal patch.
	} AVL63X1_VerInfo;

	// Used to configure the AVL PART device SDRAM controller. 
	typedef struct AVL63X1_SDRAM_Conf
	{
		AVL_uint16 m_SDRAM_TRC;				// SDRAM active to active command period in ns. 
		AVL_uint16 m_SDRAM_TXSR;			// SDRAM exit self-refresh to active command period in ns.
		AVL_uint16 m_SDRAM_TRCAR;			// SDRAM auto refresh period in ns.  Minimum time between two auto refresh commands.
		AVL_uint16 m_SDRAM_TWR;				// SDRAM write recovery time in SDRAM clock cycles.  The delay from the last data in to the next precharge command.  Valid range is 1 to 4 clock cycles.
		AVL_uint16 m_SDRAM_TRP;				// SDRAM precharge period in ns.
		AVL_uint16 m_SDRAM_TRCD;			// SDRAM minimum delay between active and read/write commands in ns.
		AVL_uint16 m_SDRAM_TRAS;			// SDRAM minimum delay between active and precharge commands in ns.
		AVL_uint16 m_SDRAM_CAS_Latency;		// SDRAM delay between read command and availability of first data in SDRAM clock cycles.  Valid range is 1 to 4 clock cycles.
	}AVL63X1_SDRAM_Conf;

    // Defines input signal information
	typedef struct AVL63X1_InputSignal_Info
	{
		enum AVL63X1_RxInput m_RxInputType_DTMB_mode;
		enum AVL63X1_RxInput m_RxInputType_DVBC_mode;
		AVL_int32 m_InputFrequency_Hz_DTMB_mode;
		AVL_int32 m_InputFrequency_Hz_DVBC_mode;
	}AVL63X1_InputSignal_Info;

    // Stores the MPEG interface configuration parameters
	typedef struct AVL63X1_MPEG_Interface_Configuration
	{
		struct AVL63X1_MPEG_Info m_MPEG_Info_DTMB_mode;
		struct AVL63X1_MPEG_Info m_MPEG_Info_DVBC_mode;
		enum AVL63X1_MPEG_ErrorBit m_MPEG_ErrorBit;
	}AVL63X1_MPEG_Interface_Configuration;
	
	// Stores the tuners' parameters which are relative to repeater
	typedef struct AVL63X1_II2C_Repeater_Info
	{
		AVL_uint16 m_TunerAddress;
		AVL_uint16 m_RepeaterClockKHz;
		AVL_uint32 m_uiRepeaterMode;
	}AVL63X1_II2C_Repeater_Info;
	
    // The structure stores the data and flags associated with the AVL63X1 device.
	typedef struct AVL63X1_Chip
	{
		enum AVL63X1_ChipSlaveAddr m_SlaveAddr;		    				// Seven bit I2C slave address of the device. 
		AVL_uchar m_current_demod_mode;			        				// Indicates the current standard mode that the device is configured to operate in. It is set each time a lock channel operation is requested on the part-supported standard.
        	AVL_uint16 m_TunerAddress;										// Current tuner address
        	AVL_puchar m_pInitialData;                      				// Store the start address of patch array
		AVL_uint32 m_DemodFrequency_Hz;									// The demodulator internal clock frequency in Hz. 
		AVL_uint32 m_SDRAMFrequency_Hz;									// The SDRAM clock frequency in Hz. 
		AVL_uint32 m_FECFrequency_Hz;									// The FEC clock frequency in Hz. 
		AVL_uint32 m_RefFrequency_Hz;									// The reference clock frequency in units of Hz.
		AVL_uint32 m_RxInput;											// Stores the information of the type of the signal being input to the device when operating for a particular standard.	
		AVL_semaphore m_semRx;											// A semaphore used to protect the receiver command channel.
		AVL_semaphore m_semI2CRepeater;		    						// A semaphore used to protect the I2C repeater channel.
		AVL_semaphore m_semI2CRepeater_r;	    						// A semaphore used to protect the I2C repeater channel data reading.
		AVL_int32 m_iInputFrequencyHz;									// The center frequency of the device's input signal in units of Hz.  If a baseband tuner is used, the value is zero.  If an IF tuner is used, the value is equal to the IF output frequency of the tuner.  This parameter is set by the function AVL_IRx_Initialize.
		struct	AVL63X1_II2C_Repeater_Info m_RepeaterInfo[TUNER_NUM]; 	// Array used to keep the tuners' parameters which are relative to repeater
		struct 	AVL63X1_SDRAM_Conf * m_pSDRAM_Conf;						// Stores the SDRAM configuration structure information for the chip.
		struct 	AVL63X1_MPEG_Info m_MPEG_Info;							// Stores the MPEG configuration structure information  for the whole chip.
		struct 	AVL63X1_InputSignal_Info m_InputSignal_Info;			// The info of signal which is inputted into demod
	}AVL63X1_Chip;

	// Used to configure the AVL PART device's internal PLL. The SDK provides a PLL configuration array in IBSP.c. The user may remove unused elements in that array to save memory. Availink strongly advises against users changing the PLL setup values themselves. Please contact Availink if there is a requirment to do that.
	typedef struct AVL63X1_PLL_Conf
	{
		AVL_uchar m_PLL_RefClkDivVal;			// PLL reference clock divider value
		AVL_uchar m_PLL_FbClkDivVal;			// PLL feedback clock divider value
		AVL_uchar m_PLL_od;						// PLL output divider value
		AVL_uchar m_PLL_od2;					// PLL output divider 2
		AVL_uchar m_PLL_od3;					// PLL output divider 3
		AVL_uint32 m_DDLL_out_phase;			// DDLL SDRAM OC Clock Phase Adjust value
		AVL_uint32 m_DDLL_read_phase;			// DDLL Read Clock Phase Adj Register
		AVL_uint32 m_RefFrequency_Hz;			// The reference clock frequency in units of Hz.
		AVL_uint32 m_DemodFrequency_Hz;			// The internal demod clock frequency in units of Hz.
		AVL_uint32 m_SDRAM_Frequency_Hz;		// The SDRAM clock frequency in units of Hz.
		AVL_uint32 m_FEC_Frequency_Hz;			// The FEC clock frequency in units of Hz.
		AVL63X1_SDRAM_Type m_SDRAM_Type;        // SDRAM configuration type
	}AVL63X1_PLL_Conf ;

    // Stores an unsigned 64-bit integer
	typedef struct AVL63X1_uint64
	{
		AVL_uint32 m_HighWord;					// The most significant 32-bits of the unsigned 64-bit integer
		AVL_uint32 m_LowWord;					// The least significant 32-bits of the unsigned 64-bit integer
	}AVL63X1_uint64;

	// Stores a signed 64-bit integer
	typedef struct AVL63X1_int64
	{
		AVL_int32 m_HighWord;					// The most significant 32-bits of the signed 64-bit integer
		AVL_uint32 m_LowWord;					// The least significant 32-bits of the signed 64-bit integer
	}AVL63X1_int64;	
	
	// Contains variables for storing error statistics used in the BER and PER calculations.
    typedef  struct AVL63X1_ErrorStats
	{
		AVL_uint16 m_LFSR_Sync;					// Indicates whether the receiver is synchronized with the transmitter generating the BER test pattern.
		AVL_uint16 m_LostLock;					// Indicates whether the receiver has lost lock since the BER/PER measurement was started.
		struct AVL63X1_uint64 m_SwCntNumBits;	// A software counter which stores the number of bits which have been received.
		struct AVL63X1_uint64 m_SwCntBitErrors;	// A software counter which stores the number of bit errors which have been detected.
		struct AVL63X1_uint64 m_NumBits;		// The total number of bits which have been received.
		struct AVL63X1_uint64 m_BitErrors;		// The total number of bit errors which have been detected.
		struct AVL63X1_uint64 m_SwCntNumPkts;	// A software counter which stores the number of packets which have been received.
		struct AVL63X1_uint64 m_SwCntPktErrors;	// A software counter which stores the number of packet errors which have been detected.
		struct AVL63X1_uint64 m_NumPkts;		// The total number of packets which have been received.
		struct AVL63X1_uint64 m_PktErrors;		// The total number of packet errors which have been detected.
		AVL_uint32 m_BER;						// The bit error rate scaled by 1e9.
		AVL_uint32 m_PER;						// The packet error rate scaled by 1e9.
	}AVL63X1_ErrorStats;

    // The generic Availink Channel structure
	// The users will not need to populate this structure. 
	// It is used by the highest layer of API interface internally to route to specific SDK functions as per the demod mode configuration.
	typedef struct AVL_Channel
	{
		AVL_uchar m_demod_mode;     		// This parameter is used to identify the desired standard mode to operate the demod in.
		AVL_uchar m_channel_id;     		// This parameter is used to identify the demod channel in operation incase of a  multi-channel supported part.
		AVL_uint32 m_ConfigOptions; 		// This parameter is presently not used, but is reserved for future configuration options.  
		AVL63X1_Chip *pAVLChip;     		//Chip object
	}AVL_Channel;

	#ifdef AVL_CPLUSPLUS
	}
	#endif

#endif

