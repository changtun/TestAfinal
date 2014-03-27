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
/// @brief Defines data structures, enums, global macros for the DVB-C mode.
/// 

#ifndef aVL_DVBC_H
#define aVL_DVBC_H


	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

#define AVL_DVBC_status_regs_base (0x000001a0)
#define AVL_DVBC_config_regs_base (0x00000558)
	
#define rs_AVL_DVBC_lock_status              (AVL_DVBC_status_regs_base + 0x00000000)
#define rs_AVL_DVBC_mode_status              (AVL_DVBC_status_regs_base + 0x00000004)
#define rs_AVL_DVBC_demod_lock_indicator     (AVL_DVBC_status_regs_base + 0x00000008)
#define rs_AVL_DVBC_constellation_mode       (AVL_DVBC_status_regs_base + 0x00000008)
#define rs_AVL_DVBC_patch_ver                (AVL_DVBC_status_regs_base + 0x00000010)
#define rs_AVL_DVBC_carrier_freq_offset_Hz   (AVL_DVBC_status_regs_base + 0x00000014)
#define rs_AVL_DVBC_demod_status             (AVL_DVBC_status_regs_base + 0x0000001C)
#define rs_AVL_DVBC_aagc_gain                (AVL_DVBC_status_regs_base + 0x00000048)
#define rs_AVL_DVBC_timing_offset            (AVL_DVBC_status_regs_base + 0x00000044)
#define rs_AVL_DVBC_snr_dB_x100              (AVL_DVBC_status_regs_base + 0x0000000e)
#define rs_AVL_DVBC_berBeforRS_xE9           (AVL_DVBC_status_regs_base + 0x0000004c)
#define rs_AVL_DVBC_berAfterRS_xE9           (AVL_DVBC_status_regs_base + 0x00000050)
	
		
#define rc_AVL_DVBC_symbol_rate_Hz           (AVL_DVBC_config_regs_base + 0x00000000)
#define rc_AVL_DVBC_sample_rate_Hz           (AVL_DVBC_config_regs_base + 0x00000004)
#define rc_AVL_DVBC_demod_clk                (AVL_DVBC_config_regs_base + 0x00000008)
#define rc_AVL_DVBC_dmd_clk_Hz               (AVL_DVBC_config_regs_base + 0x00000008)
#define rc_AVL_DVBC_sdram_clk_Hz             (AVL_DVBC_config_regs_base + 0x0000000c)
#define rc_AVL_DVBC_i2cm_config              (AVL_DVBC_config_regs_base + 0x00000010)
#define rc_AVL_DVBC_mpeg_control_1           (AVL_DVBC_config_regs_base + 0x00000014)
#define rc_AVL_DVBC_mpeg_control_2           (AVL_DVBC_config_regs_base + 0x00000018)
#define rc_AVL_DVBC_mpeg_control_3           (AVL_DVBC_config_regs_base + 0x0000001c)
#define rc_AVL_DVBC_ts0_tsp1                 (AVL_DVBC_config_regs_base + 0x0000001b)
#define rc_AVL_DVBC_mpeg_ts_error_bit_en     (AVL_DVBC_config_regs_base + 0x00000020)
#define rc_AVL_DVBC_ddc_input_select         (AVL_DVBC_config_regs_base + 0x00000024)
#define rc_AVL_DVBC_tuner_type               (AVL_DVBC_config_regs_base + 0x00000024)
#define rc_AVL_DVBC_input_format             (AVL_DVBC_config_regs_base + 0x00000025)
#define rc_AVL_DVBC_spectrum_invert          (AVL_DVBC_config_regs_base + 0x00000026)
#define rc_AVL_DVBC_input_select             (AVL_DVBC_config_regs_base + 0x00000027)
#define rc_AVL_DVBC_if_freq_Hz               (AVL_DVBC_config_regs_base + 0x00000028)
#define rc_AVL_DVBC_tuner_used               (AVL_DVBC_config_regs_base + 0x00000040)
#define rc_AVL_DVBC_qam_mode                 (AVL_DVBC_config_regs_base + 0x0000002c)
#define rc_AVL_DVBC_if_agc_control           (AVL_DVBC_config_regs_base + 0x00000044)
#define rc_AVL_DVBC_ifagc_ref                (AVL_DVBC_config_regs_base + 0x00000044)
#define rc_AVL_DVBC_ifagc_dcm                (AVL_DVBC_config_regs_base + 0x00000046)
#define rc_AVL_DVBC_ifagc_pol                (AVL_DVBC_config_regs_base + 0x00000047)
#define rc_AVL_DVBC_fec_clk_Hz               (AVL_DVBC_config_regs_base + 0x00000050)
#define rc_AVL_DVBC_digital0_analog1         (AVL_DVBC_config_regs_base + 0x00000054)
#define rc_AVL_DVBC_get_btr_crl              (AVL_DVBC_config_regs_base + 0x00000080)
#define rc_AVL_DVBC_qam_mode_scan_control    (AVL_DVBC_config_regs_base + 0x00000090)
	
#define rc_AVL_DVBC_mpeg_control_5                          (AVL_DVBC_config_regs_base + 0x00000094)
#define rc_AVL_DVBC_cntns_pkt_para_rate_frac_n_32bit        (AVL_DVBC_config_regs_base + 0x00000098)
#define rc_AVL_DVBC_cntns_pkt_para_rate_frac_d_32bit        (AVL_DVBC_config_regs_base + 0x0000009c)
#define rc_AVL_DVBC_cntns_pkt_seri_rate_frac_n_32bit        (AVL_DVBC_config_regs_base + 0x000000a0)
#define rc_AVL_DVBC_cntns_pkt_seri_rate_frac_d_32bit        (AVL_DVBC_config_regs_base + 0x000000a4)
		
		
#define ADDRESS_INC    4 //register address increase 4 for each word
#define BASE_ADDR_CITRINE_EQUALIZER      (0x0005a000*ADDRESS_INC)
#define rs_AVL_DVBC_equalizer_mse        (BASE_ADDR_CITRINE_EQUALIZER + (0x6 * ADDRESS_INC))
	
	
#define rs_AVL_DVBC_total_words              0x00149430                                        
#define rs_AVL_DVBC_total_uncorr_words       0x00149434   
#define rs_AVL_DVBC_errstat_clear            0x0014942c
	
#define rs_AVL_DVBC_scatter_data             0x002edd70


	/// Defines the modulation mode of the received DVB-C signal.
	typedef enum AVL_DVBC_ModulationMode
	{
		RX_DVBC_16QAM = 0,      ///< =0
		RX_DVBC_32QAM = 1,      ///< =1
		RX_DVBC_64QAM = 2,      ///< =2
		RX_DVBC_128QAM = 3,     ///< =3
		RX_DVBC_256QAM = 4      ///< =4
	}AVL_DVBC_ModulationMode;

	/// Holds the received DVB-C signal information parameters.
	typedef struct AVL_DVBC_SignalInfo
	{
		enum AVL_DVBC_ModulationMode m_Modulation;		///< The modulation mode of the signal. \sa ::AVL_DVBC_ModulationMode.
	}AVL_DVBC_SignalInfo;
	
	/// Holds the DVB-C channel information.The structure contains the parameters used for locking to a channel.
	typedef struct AVL_DVBC_Channel
	{
		AVL_uchar m_demod_mode;                             /// <Indicates the desired standard mode to operate the demod in as per the specified channel.
		                                                    ///< Should be set to one of the four macros defining different supported standards by an Availink part in AVL63X1_include.h	
		AVL_uchar m_channel_id;                             ///< Defines the channel identifier to specify a particular demod channel in operation incase of multi-channel supported part.
		AVL_uint32 m_ConfigOptions;                         ///< This parameter is presently not used, but is reserved for future configuration options.  			
		AVL63X1_Chip *pAVLChip;                             ///< Defines a pointer to the Availink chip structure that will be used to operate upon the specified channel.				
		AVL_uint32 m_uiFrequency_kHz;                       ///< The carrier frequency of the channel in units of kHz.
		AVL_uint32 m_uiSymbolRate_Hz;                       ///< The symbol rate in units of Hz. 
		struct AVL_DVBC_SignalInfo m_SignalInfo;            ///< The signal information.
		enum AVL63X1_SpectrumInversion m_SpectrumInversion; ///< The spectrum inversion setting. Controls whether the spectrum is inverted.
	}AVL_DVBC_Channel;

	AVL63X1_ErrorCode AVL_DVBC_Halt( struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_Wakeup( struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetRunningLevel( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip );


	AVL63X1_ErrorCode AVL_DVBC_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip);
	AVL63X1_ErrorCode AVL_DVBC_GetLockStatus( AVL_puint16 puiLockStatus, AVL_puint16 puiNumLockRetries, const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetSignalInfo(struct AVL_DVBC_SignalInfo *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetSignalLevel(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DVBC_Initialize(struct AVL63X1_Chip * pAVL_Chip);
	AVL63X1_ErrorCode AVL_DVBC_LockChannel( struct AVL63X1_Chip * pAVL_Chip, enum AVL63X1_LockMode enumLockMode, const struct AVL_DVBC_Channel * psChannel );
	AVL63X1_ErrorCode AVL_DVBC_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip );


#ifdef AVL_CPLUSPLUS
}
	#endif
#endif

