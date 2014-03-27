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
/// @brief Defines data structures, enums, global macros for the DTMB mode.
/// 

#ifndef AVL_DTMB_H
#define AVL_DTMB_H
	
	#ifdef AVL_CPLUSPLUS
extern "C" {
	#endif

#define AVL_DTMB_config_regs_base				   (0x00000300)
#define AVL_DTMB_status_regs_base				   (0x000000a4)
	
#define AVL_DTMB_SDRAM_BASE_ADDR					0x00400000
	
#define rc_AVL_DTMB_sdram_control					0x003FF000
	
#define rs_AVL_DTMB_scatter_data					0x002f1880
	
#define rs_AVL_DTMB_aagc_gain						0x0016004C
		
#define rs_AVL_DTMB_cfo_freq_est					0x001600C0
		
#define rc_AVL_DTMB_symbol_rate_Hz           (AVL_DTMB_config_regs_base + 0x00000000)
#define rc_AVL_DTMB_sample_rate_Hz           (AVL_DTMB_config_regs_base + 0x00000004)
#define rc_AVL_DTMB_if_agc_control           (AVL_DTMB_config_regs_base + 0x00000008)
#define rc_AVL_DTMB_ifagc_dcm                (AVL_DTMB_config_regs_base + 0x00000008)
#define rc_AVL_DTMB_ifagc_trk_gain           (AVL_DTMB_config_regs_base + 0x00000009)
#define rc_AVL_DTMB_ifagc_acq_gain           (AVL_DTMB_config_regs_base + 0x0000000a)
#define rc_AVL_DTMB_ifagc_pol                (AVL_DTMB_config_regs_base + 0x0000000b)
#define rc_AVL_DTMB_ifagc_ref                (AVL_DTMB_config_regs_base + 0x0000000c)
#define rc_AVL_DTMB_dig_agc_control          (AVL_DTMB_config_regs_base + 0x00000010)
#define rc_AVL_DTMB_dagc_ref                 (AVL_DTMB_config_regs_base + 0x00000010)
#define rc_AVL_DTMB_dagc_trk_gain            (AVL_DTMB_config_regs_base + 0x00000012)
#define rc_AVL_DTMB_dagc_acq_gain            (AVL_DTMB_config_regs_base + 0x00000013)
#define rc_AVL_DTMB_dc_comp_control          (AVL_DTMB_config_regs_base + 0x00000014)
#define rc_AVL_DTMB_dc_comp_enable           (AVL_DTMB_config_regs_base + 0x00000015)
#define rc_AVL_DTMB_dc_comp_trk_gain         (AVL_DTMB_config_regs_base + 0x00000016)
#define rc_AVL_DTMB_dc_comp_acq_gain         (AVL_DTMB_config_regs_base + 0x00000017)
#define rc_AVL_DTMB_iq_comp_control          (AVL_DTMB_config_regs_base + 0x00000018)
#define rc_AVL_DTMB_iq_comp_enable           (AVL_DTMB_config_regs_base + 0x00000019)
#define rc_AVL_DTMB_iq_comp_trk_gain         (AVL_DTMB_config_regs_base + 0x0000001a)
#define rc_AVL_DTMB_iq_comp_acq_gain         (AVL_DTMB_config_regs_base + 0x0000001b)
#define rc_AVL_DTMB_btr_bandwidth            (AVL_DTMB_config_regs_base + 0x0000001c)
#define rc_AVL_DTMB_btr_trk_lpbw             (AVL_DTMB_config_regs_base + 0x0000001c)
#define rc_AVL_DTMB_btr_acq_lpbw             (AVL_DTMB_config_regs_base + 0x0000001e)
#define rc_AVL_DTMB_ddc_input_select         (AVL_DTMB_config_regs_base + 0x00000020)
#define rc_AVL_DTMB_tuner_type               (AVL_DTMB_config_regs_base + 0x00000020)
#define rc_AVL_DTMB_input_format             (AVL_DTMB_config_regs_base + 0x00000021)
#define rc_AVL_DTMB_spectrum_invert          (AVL_DTMB_config_regs_base + 0x00000022)
#define rc_AVL_DTMB_input_select             (AVL_DTMB_config_regs_base + 0x00000023)
#define rc_AVL_DTMB_nom_carrier_freq_Hz      (AVL_DTMB_config_regs_base + 0x00000024)
#define rc_AVL_DTMB_equalizer_control        (AVL_DTMB_config_regs_base + 0x00000028)
#define rc_AVL_DTMB_lms_limit_threshold      (AVL_DTMB_config_regs_base + 0x00000028)
#define rc_AVL_DTMB_noise_var_limit_mode     (AVL_DTMB_config_regs_base + 0x0000002a)
#define rc_AVL_DTMB_equalizer_mode           (AVL_DTMB_config_regs_base + 0x0000002b)
#define rc_AVL_DTMB_equalizer_loop_gain      (AVL_DTMB_config_regs_base + 0x0000002c)
#define rc_AVL_DTMB_equalizer_lpbw_trk       (AVL_DTMB_config_regs_base + 0x0000002c)
#define rc_AVL_DTMB_equalizer_lpbw_acq      (AVL_DTMB_config_regs_base + 0x0000002e)
#define rc_AVL_DTMB_cfo_diff_control         (AVL_DTMB_config_regs_base + 0x00000030)
#define rc_AVL_DTMB_bypass_cfo_estimation    (AVL_DTMB_config_regs_base + 0x00000030)
#define rc_AVL_DTMB_cfo_peak_thrshld         (AVL_DTMB_config_regs_base + 0x00000031)
#define rc_AVL_DTMB_cfo_loop_max             (AVL_DTMB_config_regs_base + 0x00000032)
#define rc_AVL_DTMB_cfo_valid_max            (AVL_DTMB_config_regs_base + 0x00000033)
#define rc_AVL_DTMB_cfo_coh_control          (AVL_DTMB_config_regs_base + 0x00000034)
#define rc_AVL_DTMB_cfo_coh_loop_max         (AVL_DTMB_config_regs_base + 0x00000036)
#define rc_AVL_DTMB_cfo_coh_valid_max        (AVL_DTMB_config_regs_base + 0x00000037)
#define rc_AVL_DTMB_demod_clk                (AVL_DTMB_config_regs_base + 0x00000038)
#define rc_AVL_DTMB_dmd_clk_Hz               (AVL_DTMB_config_regs_base + 0x00000038)
#define rc_AVL_DTMB_sdram_clk_Hz             (AVL_DTMB_config_regs_base + 0x0000003c)
#define rc_AVL_DTMB_frm_adj_cntrl            (AVL_DTMB_config_regs_base + 0x00000040)
#define rc_AVL_DTMB_frm_adj_late_target      (AVL_DTMB_config_regs_base + 0x00000040)
#define rc_AVL_DTMB_frm_adj_early_target     (AVL_DTMB_config_regs_base + 0x00000041)
#define rc_AVL_DTMB_frm_adj_step_trk         (AVL_DTMB_config_regs_base + 0x00000042)
#define rc_AVL_DTMB_frm_adj_step_acq         (AVL_DTMB_config_regs_base + 0x00000043)
#define rc_AVL_DTMB_cfo_loop_gain_acq        (AVL_DTMB_config_regs_base + 0x00000044)
#define rc_AVL_DTMB_cfo_loop_lpbw_acq        (AVL_DTMB_config_regs_base + 0x00000044)
#define rc_AVL_DTMB_cfo_loop_g1_gain_acq     (AVL_DTMB_config_regs_base + 0x00000046)
#define rc_AVL_DTMB_cfo_loop_g2_gain_acq     (AVL_DTMB_config_regs_base + 0x00000047)
#define rc_AVL_DTMB_cfo_loop_gain_trk        (AVL_DTMB_config_regs_base + 0x00000048)
#define rc_AVL_DTMB_cfo_loop_lpbw_trk        (AVL_DTMB_config_regs_base + 0x00000048)
#define rc_AVL_DTMB_cfo_loop_g1_gain_trk     (AVL_DTMB_config_regs_base + 0x0000004a)
#define rc_AVL_DTMB_cfo_loop_g2_gain_trk     (AVL_DTMB_config_regs_base + 0x0000004b)
#define rc_AVL_DTMB_demod_cntrl_0            (AVL_DTMB_config_regs_base + 0x0000004c)
#define rc_AVL_DTMB_fbs_limit_scl            (AVL_DTMB_config_regs_base + 0x0000004c)
#define rc_AVL_DTMB_sounding_limit_scl       (AVL_DTMB_config_regs_base + 0x0000004d)
#define rc_AVL_DTMB_sounding_phs_correction  (AVL_DTMB_config_regs_base + 0x0000004e)
#define rc_AVL_DTMB_tone_rmv_enable          (AVL_DTMB_config_regs_base + 0x0000004f)
#define rc_AVL_DTMB_mpeg_control_1           (AVL_DTMB_config_regs_base + 0x00000050)
#define rc_AVL_DTMB_mpeg_seq                 (AVL_DTMB_config_regs_base + 0x00000050)
#define rc_AVL_DTMB_serial_outpin_sel        (AVL_DTMB_config_regs_base + 0x00000051)
#define rc_AVL_DTMB_mpeg_posedge             (AVL_DTMB_config_regs_base + 0x00000052)
#define rc_AVL_DTMB_mpeg_serial              (AVL_DTMB_config_regs_base + 0x00000053)
#define rc_AVL_DTMB_mpeg_control_2           (AVL_DTMB_config_regs_base + 0x00000054)
#define rc_AVL_DTMB_pkt_err_pol              (AVL_DTMB_config_regs_base + 0x00000054)
#define rc_AVL_DTMB_pkt_lfsr_en              (AVL_DTMB_config_regs_base + 0x00000055)
#define rc_AVL_DTMB_pkt_int_pattern          (AVL_DTMB_config_regs_base + 0x00000056)
#define rc_AVL_DTMB_ts0_tsp1                 (AVL_DTMB_config_regs_base + 0x00000057)
#define rc_AVL_DTMB_i2cm_config              (AVL_DTMB_config_regs_base + 0x00000058)
#define rc_AVL_DTMB_i2cm_time_out            (AVL_DTMB_config_regs_base + 0x00000058)
#define rc_AVL_DTMB_i2cm_speed_kHz           (AVL_DTMB_config_regs_base + 0x0000005a)
#define rc_AVL_DTMB_frameid_config           (AVL_DTMB_config_regs_base + 0x0000005c)
#define rc_AVL_DTMB_frameid_config_confirmation_times (AVL_DTMB_config_regs_base + 0x0000005c)
#define rc_AVL_DTMB_frameid_config_retry_times (AVL_DTMB_config_regs_base + 0x0000005e)
#define rc_AVL_DTMB_single_carrier_pll_loop_bw (AVL_DTMB_config_regs_base + 0x00000060)
#define rc_AVL_DTMB_sc_pll_g2_rshift         (AVL_DTMB_config_regs_base + 0x00000060)
#define rc_AVL_DTMB_sc_pll_g2_scl            (AVL_DTMB_config_regs_base + 0x00000061)
#define rc_AVL_DTMB_sc_pll_g1_rshift         (AVL_DTMB_config_regs_base + 0x00000062)
#define rc_AVL_DTMB_sc_pll_g1_scl            (AVL_DTMB_config_regs_base + 0x00000063)
#define rc_AVL_DTMB_tone_cmp_rshft           (AVL_DTMB_config_regs_base + 0x00000064)
#define rc_AVL_DTMB_tone_cmp_loop_rshft      (AVL_DTMB_config_regs_base + 0x00000067)
#define rc_AVL_DTMB_equalizer_control_2      (AVL_DTMB_config_regs_base + 0x00000068)
#define rc_AVL_DTMB_max_delay_spread         (AVL_DTMB_config_regs_base + 0x00000068)
#define rc_AVL_DTMB_equalizer_wait_frames    (AVL_DTMB_config_regs_base + 0x0000006a)
#define rc_AVL_DTMB_mpeg_ratio_num_scl       (AVL_DTMB_config_regs_base + 0x0000006c)
#define rc_AVL_DTMB_demod_cntrl_1            (AVL_DTMB_config_regs_base + 0x00000070)
#define rc_AVL_DTMB_num_spectrum_invert_tries (AVL_DTMB_config_regs_base + 0x00000070)
#define rc_AVL_DTMB_plot_data_type           (AVL_DTMB_config_regs_base + 0x00000071)
#define rc_AVL_DTMB_boundary_symbol_erasures (AVL_DTMB_config_regs_base + 0x00000072)
#define rc_AVL_DTMB_demod_profile            (AVL_DTMB_config_regs_base + 0x00000073)
#define rc_AVL_DTMB_mc_eq_mode_det           (AVL_DTMB_config_regs_base + 0x00000074)
#define rc_AVL_DTMB_noise_var_rshft          (AVL_DTMB_config_regs_base + 0x00000074)
#define rc_AVL_DTMB_mobile_static_det_en     (AVL_DTMB_config_regs_base + 0x00000075)
#define rc_AVL_DTMB_mc_eq_switch_en          (AVL_DTMB_config_regs_base + 0x00000076)
#define rc_AVL_DTMB_max_delay_margin         (AVL_DTMB_config_regs_base + 0x00000077)
#define rc_AVL_DTMB_mpeg_control_4           (AVL_DTMB_config_regs_base + 0x00000078)
#define rc_AVL_DTMB_mpeg_ts_error_bit_en     (AVL_DTMB_config_regs_base + 0x00000078)
#define rc_AVL_DTMB_mpeg_refclk_mode         (AVL_DTMB_config_regs_base + 0x00000079)
#define rc_AVL_DTMB_mpeg_rate_adjust_step    (AVL_DTMB_config_regs_base + 0x0000007a)
#define rc_AVL_DTMB_sc_cfo_hypothetical_cntrl (AVL_DTMB_config_regs_base + 0x0000007c)
#define rc_AVL_DTMB_static_equalizer_lpbw_acq (AVL_DTMB_config_regs_base + 0x0000007c)
#define rc_AVL_DTMB_static_equalizer_lpbw_trk (AVL_DTMB_config_regs_base + 0x0000007d)
#define rc_AVL_DTMB_sc_hypothetical_wait_frames (AVL_DTMB_config_regs_base + 0x0000007e)
#define rc_AVL_DTMB_dfe_cntrl                (AVL_DTMB_config_regs_base + 0x00000080)
#define rc_AVL_DTMB_enable_new_var_est       (AVL_DTMB_config_regs_base + 0x00000082)
#define rc_AVL_DTMB_enable_dfe               (AVL_DTMB_config_regs_base + 0x00000083)
#define rc_AVL_DTMB_fec_clk                  (AVL_DTMB_config_regs_base + 0x00000084)
#define rc_AVL_DTMB_fec_clk_Hz               (AVL_DTMB_config_regs_base + 0x00000084)
#define rc_AVL_DTMB_mpeg_control_3           (AVL_DTMB_config_regs_base + 0x00000088)
#define rc_AVL_DTMB_cont_mpegclk_en          (AVL_DTMB_config_regs_base + 0x0000008b)
#define rc_AVL_DTMB_cntns_pkt_para_rate_frac_n (AVL_DTMB_config_regs_base + 0x0000008c)
#define rc_AVL_DTMB_cntns_pkt_para_rate_frac_d (AVL_DTMB_config_regs_base + 0x00000090)
#define rc_AVL_DTMB_cntns_pkt_seri_rate_frac_n (AVL_DTMB_config_regs_base + 0x00000094)
#define rc_AVL_DTMB_cntns_pkt_seri_rate_frac_d (AVL_DTMB_config_regs_base + 0x00000098)
#define rc_AVL_DTMB_mode_detect_control      (AVL_DTMB_config_regs_base + 0x00000100)
#define rc_AVL_DTMB_mc_decision_equalizer_lpbw_trk (AVL_DTMB_config_regs_base + 0x00000100)
#define rc_AVL_DTMB_pn_indx_var              (AVL_DTMB_config_regs_base + 0x00000101)
#define rc_AVL_DTMB_mode_detect_retry_max    (AVL_DTMB_config_regs_base + 0x00000103)
#define rc_AVL_DTMB_time_deint_mode          (AVL_DTMB_config_regs_base + 0x00000104)
#define rc_AVL_DTMB_time_deint_test_mode     (AVL_DTMB_config_regs_base + 0x00000107)
#define rc_AVL_DTMB_acq_circular_buffer_timeout (AVL_DTMB_config_regs_base + 0x00000108)
#define rc_AVL_DTMB_acq_cb_timeout           (AVL_DTMB_config_regs_base + 0x00000108)
#define rc_AVL_DTMB_tps_valid_times          (AVL_DTMB_config_regs_base + 0x0000010c)
#define rc_AVL_DTMB_fec_lock_notification_frames (AVL_DTMB_config_regs_base + 0x0000010c)
#define rc_AVL_DTMB_reacq_after_tps_confirm  (AVL_DTMB_config_regs_base + 0x0000010e)
#define rc_AVL_DTMB_max_tps_code_fec_lock_attempts (AVL_DTMB_config_regs_base + 0x0000010f)
#define rc_AVL_DTMB_software_watch_dog       (AVL_DTMB_config_regs_base + 0x00000110)
#define rc_AVL_DTMB_swd_threshold            (AVL_DTMB_config_regs_base + 0x00000110)
#define rc_AVL_DTMB_demod_init_delay         (AVL_DTMB_config_regs_base + 0x00000114)
#define rc_AVL_DTMB_mobile_static_mode_det_control1 (AVL_DTMB_config_regs_base + 0x00000118)
#define rc_AVL_DTMB_mobile_det_threshold_minus (AVL_DTMB_config_regs_base + 0x00000118)
#define rc_AVL_DTMB_mobile_det_threshold_plus (AVL_DTMB_config_regs_base + 0x00000119)
#define rc_AVL_DTMB_mobile_static_det_window (AVL_DTMB_config_regs_base + 0x0000011a)
#define rc_AVL_DTMB_mobile_static_mode_det_control2 (AVL_DTMB_config_regs_base + 0x0000011c)
#define rc_AVL_DTMB_static_lms_limit_threshold (AVL_DTMB_config_regs_base + 0x0000011c)
#define rc_AVL_DTMB_mobile_off_frms          (AVL_DTMB_config_regs_base + 0x0000011e)
#define rc_AVL_DTMB_initial_mobile_on_frms   (AVL_DTMB_config_regs_base + 0x0000011f)
#define rc_AVL_DTMB_mobile_mc_equalizer_loop_gain (AVL_DTMB_config_regs_base + 0x00000120)
#define rc_AVL_DTMB_mc_mb_equalizer_lpbw_trk (AVL_DTMB_config_regs_base + 0x00000120)
#define rc_AVL_DTMB_mc_mb_equalizer_lpbw_acq (AVL_DTMB_config_regs_base + 0x00000122)
#define rc_AVL_DTMB_mobile_sc_equalizer_loop_gain (AVL_DTMB_config_regs_base + 0x00000124)
#define rc_AVL_DTMB_sc_mb_equalizer_lpbw_trk (AVL_DTMB_config_regs_base + 0x00000124)
#define rc_AVL_DTMB_sc_mb_equalizer_lpbw_acq (AVL_DTMB_config_regs_base + 0x00000126)
#define rc_AVL_DTMB_mb_lms_limit_control     (AVL_DTMB_config_regs_base + 0x00000128)
#define rc_AVL_DTMB_mc_mb_lms_limit_threshold (AVL_DTMB_config_regs_base + 0x00000128)
#define rc_AVL_DTMB_sc_mb_lms_limit_threshold (AVL_DTMB_config_regs_base + 0x0000012a)
#define rc_AVL_DTMB_mb_fbs_limit_control     (AVL_DTMB_config_regs_base + 0x0000012c)
#define rc_AVL_DTMB_static_fbs_limit_scl     (AVL_DTMB_config_regs_base + 0x0000012c)
#define rc_AVL_DTMB_initial_mobile_cont_det_frms (AVL_DTMB_config_regs_base + 0x0000012d)
#define rc_AVL_DTMB_sc_mb_fbs_limit_scl      (AVL_DTMB_config_regs_base + 0x0000012e)
#define rc_AVL_DTMB_mc_mb_fbs_limit_scl      (AVL_DTMB_config_regs_base + 0x0000012f)
#define rc_AVL_DTMB_mc_frm_adj_cntrl         (AVL_DTMB_config_regs_base + 0x00000130)
#define rc_AVL_DTMB_mc_frm_adj_late_target   (AVL_DTMB_config_regs_base + 0x00000130)
#define rc_AVL_DTMB_mc_frm_adj_early_target  (AVL_DTMB_config_regs_base + 0x00000131)
#define rc_AVL_DTMB_sc_frm_adj_late_target   (AVL_DTMB_config_regs_base + 0x00000132)
#define rc_AVL_DTMB_sc_frm_adj_early_target  (AVL_DTMB_config_regs_base + 0x00000133)
#define rc_AVL_DTMB_frt_echo_det_cntrl       (AVL_DTMB_config_regs_base + 0x00000134)
#define rc_AVL_DTMB_static_lms_thresh_scl    (AVL_DTMB_config_regs_base + 0x00000138)
#define rc_AVL_DTMB_static_lms_thresh_lshift_m3 (AVL_DTMB_config_regs_base + 0x00000139)
#define rc_AVL_DTMB_static_lms_thresh_lshift_m2 (AVL_DTMB_config_regs_base + 0x0000013a)
#define rc_AVL_DTMB_static_lms_thresh_lshift_m1 (AVL_DTMB_config_regs_base + 0x0000013b)
#define rc_AVL_DTMB_mobile_lms_thresh_scl    (AVL_DTMB_config_regs_base + 0x0000013c)
#define rc_AVL_DTMB_mobile_lms_thresh_lshift_m3 (AVL_DTMB_config_regs_base + 0x0000013d)
#define rc_AVL_DTMB_mobile_lms_thresh_lshift_m2 (AVL_DTMB_config_regs_base + 0x0000013e)
#define rc_AVL_DTMB_mobile_lms_thresh_lshift_m1 (AVL_DTMB_config_regs_base + 0x0000013f)
#define rc_AVL_DTMB_minvar_cntrl             (AVL_DTMB_config_regs_base + 0x00000140)
#define rc_AVL_DTMB_samps_per_seg            (AVL_DTMB_config_regs_base + 0x00000142)
#define rc_AVL_DTMB_cci_impls_cntrl          (AVL_DTMB_config_regs_base + 0x00000144)
#define rc_AVL_DTMB_cci_T_scl                (AVL_DTMB_config_regs_base + 0x00000144)
#define rc_AVL_DTMB_impls_T_scl              (AVL_DTMB_config_regs_base + 0x00000146)
#define rc_AVL_DTMB_carr_tone_rmv_config     (AVL_DTMB_config_regs_base + 0x00000148)
#define rc_AVL_DTMB_carr_tone_rmv_rshft      (AVL_DTMB_config_regs_base + 0x0000014a)
#define rc_AVL_DTMB_carr_tone_rmv_en         (AVL_DTMB_config_regs_base + 0x0000014b)
#define rc_AVL_DTMB_m1_static_lms_lpbw       (AVL_DTMB_config_regs_base + 0x0000014c)
#define rc_AVL_DTMB_m1_static_lms_lpbw_trk   (AVL_DTMB_config_regs_base + 0x0000014c)
#define rc_AVL_DTMB_m1_static_lms_lpbw_acq   (AVL_DTMB_config_regs_base + 0x0000014e)
#define rc_AVL_DTMB_m2_static_lms_lpbw       (AVL_DTMB_config_regs_base + 0x00000150)
#define rc_AVL_DTMB_m2_static_lms_lpbw_trk   (AVL_DTMB_config_regs_base + 0x00000150)
#define rc_AVL_DTMB_m2_static_lms_lpbw_acq   (AVL_DTMB_config_regs_base + 0x00000152)
#define rc_AVL_DTMB_m3_static_lms_lpbw       (AVL_DTMB_config_regs_base + 0x00000154)
#define rc_AVL_DTMB_m3_static_lms_lpbw_trk   (AVL_DTMB_config_regs_base + 0x00000154)
#define rc_AVL_DTMB_m3_static_lms_lpbw_acq   (AVL_DTMB_config_regs_base + 0x00000156)
#define rc_AVL_DTMB_mobile_static_det_config (AVL_DTMB_config_regs_base + 0x00000158)
#define rc_AVL_DTMB_adapt_eq_tm_sw_threshold (AVL_DTMB_config_regs_base + 0x00000159)
#define rc_AVL_DTMB_mobile_on_frms           (AVL_DTMB_config_regs_base + 0x0000015a)
#define rc_AVL_DTMB_mobile_cont_det_frms     (AVL_DTMB_config_regs_base + 0x0000015b)
#define rc_AVL_DTMB_dynamic_lms_lpbw_cntrl   (AVL_DTMB_config_regs_base + 0x0000015c)
#define rc_AVL_DTMB_enable_mobile_auto_det_by_corr (AVL_DTMB_config_regs_base + 0x0000015e)
#define rc_AVL_DTMB_enable_dynamic_lpbw      (AVL_DTMB_config_regs_base + 0x0000015f)
#define rc_AVL_DTMB_m1_mobile_lms_lpbw       (AVL_DTMB_config_regs_base + 0x00000160)
#define rc_AVL_DTMB_m1_mob_lms_lpbw_trk      (AVL_DTMB_config_regs_base + 0x00000160)
#define rc_AVL_DTMB_m1_mob_lms_lpbw_acq      (AVL_DTMB_config_regs_base + 0x00000162)
#define rc_AVL_DTMB_m2_mobile_lms_lpbw       (AVL_DTMB_config_regs_base + 0x00000164)
#define rc_AVL_DTMB_m2_mob_lms_lpbw_trk      (AVL_DTMB_config_regs_base + 0x00000164)
#define rc_AVL_DTMB_m2_mob_lms_lpbw_acq      (AVL_DTMB_config_regs_base + 0x00000166)
#define rc_AVL_DTMB_m3_mobile_lms_lpbw       (AVL_DTMB_config_regs_base + 0x00000168)
#define rc_AVL_DTMB_m3_mob_lms_lpbw_trk      (AVL_DTMB_config_regs_base + 0x00000168)
#define rc_AVL_DTMB_m3_mob_lms_lpbw_acq      (AVL_DTMB_config_regs_base + 0x0000016a)
#define rc_AVL_DTMB_corr_converge_cntrl      (AVL_DTMB_config_regs_base + 0x0000016c)
#define rc_AVL_DTMB_corr_converg_frames      (AVL_DTMB_config_regs_base + 0x0000016e)
#define rc_AVL_DTMB_TPS_det_ctrl and FEC erasure control (AVL_DTMB_config_regs_base + 0x00000170)
#define rc_AVL_DTMB_enable_fec_deep_null_erasure (AVL_DTMB_config_regs_base + 0x00000170)
#define rc_AVL_DTMB_tps_frame_accum_no       (AVL_DTMB_config_regs_base + 0x00000172)
#define rc_AVL_DTMB_tps_confirmation_times   (AVL_DTMB_config_regs_base + 0x00000173)
#define rc_AVL_DTMB_dc_comp_init             (AVL_DTMB_config_regs_base + 0x00000174)
#define rc_AVL_DTMB_dc_comp_init_q           (AVL_DTMB_config_regs_base + 0x00000174)
#define rc_AVL_DTMB_dc_comp_init_i           (AVL_DTMB_config_regs_base + 0x00000176)
#define rc_AVL_DTMB_iq_comp_init             (AVL_DTMB_config_regs_base + 0x00000178)
#define rc_AVL_DTMB_iq_comp_init_phs         (AVL_DTMB_config_regs_base + 0x00000178)
#define rc_AVL_DTMB_iq_comp_init_amp         (AVL_DTMB_config_regs_base + 0x0000017a)
#define rc_AVL_DTMB_fec_control              (AVL_DTMB_config_regs_base + 0x0000017c)
#define rc_AVL_DTMB_fec_lock_leak            (AVL_DTMB_config_regs_base + 0x0000017c)
#define rc_AVL_DTMB_fec_lock_wait_frames     (AVL_DTMB_config_regs_base + 0x0000017d)
#define rc_AVL_DTMB_num_valid_mpeg_syncs     (AVL_DTMB_config_regs_base + 0x0000017e)
#define rc_AVL_DTMB_ldpc_dec_pc_stopping     (AVL_DTMB_config_regs_base + 0x0000017f)
#define rc_AVL_DTMB_fec_tracking_control     (AVL_DTMB_config_regs_base + 0x00000180)
#define rc_AVL_DTMB_fec_lock_threshold       (AVL_DTMB_config_regs_base + 0x00000180)
#define rc_AVL_DTMB_standard_config          (AVL_DTMB_config_regs_base + 0x00000184)
#define rc_AVL_DTMB_standard                 (AVL_DTMB_config_regs_base + 0x00000184)
#define rc_AVL_DTMB_dvbs_dss_fec_init_sel_pkt_number (AVL_DTMB_config_regs_base + 0x00000188)
#define rc_AVL_DTMB_fec_max_frame_chk        (AVL_DTMB_config_regs_base + 0x0000018c)
#define rc_AVL_DTMB_fec_min_indx_repetitions (AVL_DTMB_config_regs_base + 0x00000190)
#define rc_AVL_DTMB_fec_lock_control         (AVL_DTMB_config_regs_base + 0x00000194)
#define rc_AVL_DTMB_dvbs_fec_lock_threshold  (AVL_DTMB_config_regs_base + 0x00000194)
#define rc_AVL_DTMB_dvbs_fec_lock_leak       (AVL_DTMB_config_regs_base + 0x00000196)
#define rc_AVL_DTMB_fec_lock_control2        (AVL_DTMB_config_regs_base + 0x00000198)
#define rc_AVL_DTMB_fec_min_indx_repetitions_cm (AVL_DTMB_config_regs_base + 0x00000198)
#define rc_AVL_DTMB_fec_max_frame_chk_cm     (AVL_DTMB_config_regs_base + 0x0000019a)
#define rc_AVL_DTMB_fec_lock_control3        (AVL_DTMB_config_regs_base + 0x0000019c)
#define rc_AVL_DTMB_dvbs_fec_fcnt_step       (AVL_DTMB_config_regs_base + 0x0000019c)
#define rc_AVL_DTMB_dvbs_fec_max_trial_for_same_phase (AVL_DTMB_config_regs_base + 0x0000019e)
#define rc_AVL_DTMB_if_freq_Hz               (AVL_DTMB_config_regs_base + 0x000001a0)
#define rc_AVL_DTMB_sdram_test_config        (AVL_DTMB_config_regs_base + 0x000001a4)
#define rc_AVL_DTMB_test_addr_bus            (AVL_DTMB_config_regs_base + 0x000001a5)
#define rc_AVL_DTMB_test_data_bus            (AVL_DTMB_config_regs_base + 0x000001a6)
#define rc_AVL_DTMB_test_content             (AVL_DTMB_config_regs_base + 0x000001a7)
#define rc_AVL_DTMB_dagc_cntrl               (AVL_DTMB_config_regs_base + 0x000001a8)
#define rc_AVL_DTMB_mf_rshift                (AVL_DTMB_config_regs_base + 0x000001a9)
#define rc_AVL_DTMB_aaf_rshift               (AVL_DTMB_config_regs_base + 0x000001aa)
#define rc_AVL_DTMB_dagc_midrshft            (AVL_DTMB_config_regs_base + 0x000001ab)
#define rc_AVL_DTMB_fec_rom_cntrl            (AVL_DTMB_config_regs_base + 0x000001ac)
#define rc_AVL_DTMB_fec_sync_adjust          (AVL_DTMB_config_regs_base + 0x000001ac)
#define rc_AVL_DTMB_extra_num_valid_mpeg_syncs (AVL_DTMB_config_regs_base + 0x000001ad)
#define rc_AVL_DTMB_fec_lock_wait_extra_frames (AVL_DTMB_config_regs_base + 0x000001ae)
#define rc_AVL_DTMB_sdram_clks_per_rd_seq    (AVL_DTMB_config_regs_base + 0x000001af)
		
#define rs_AVL_DTMB_lock_status              (AVL_DTMB_status_regs_base + 0x00000000)
#define rs_AVL_DTMB_lost_lock                (AVL_DTMB_status_regs_base + 0x00000001)
#define rs_AVL_DTMB_fec_lock                 (AVL_DTMB_status_regs_base + 0x00000002)
#define rs_AVL_DTMB_frame_lock               (AVL_DTMB_status_regs_base + 0x00000003)
#define rs_AVL_DTMB_mode_status              (AVL_DTMB_status_regs_base + 0x00000004)
#define rs_AVL_DTMB_interleaver_mode         (AVL_DTMB_status_regs_base + 0x00000004)
#define rs_AVL_DTMB_carrier_mode             (AVL_DTMB_status_regs_base + 0x00000005)
#define rs_AVL_DTMB_fec_rate                 (AVL_DTMB_status_regs_base + 0x00000006)
#define rs_AVL_DTMB_header_mode              (AVL_DTMB_status_regs_base + 0x00000007)
#define rs_AVL_DTMB_modulation_status        (AVL_DTMB_status_regs_base + 0x00000008)
#define rs_AVL_DTMB_tps_code_status          (AVL_DTMB_status_regs_base + 0x00000008)
#define rs_AVL_DTMB_fixed_header             (AVL_DTMB_status_regs_base + 0x00000009)
#define rs_AVL_DTMB_nr_mode                  (AVL_DTMB_status_regs_base + 0x0000000a)
#define rs_AVL_DTMB_constellation_mode       (AVL_DTMB_status_regs_base + 0x0000000b)
#define rs_AVL_DTMB_channel_status           (AVL_DTMB_status_regs_base + 0x0000000c)
#define rs_AVL_DTMB_delay_spread             (AVL_DTMB_status_regs_base + 0x0000000c)
#define rs_AVL_DTMB_snr_dB_x100              (AVL_DTMB_status_regs_base + 0x0000000e)
#define rs_AVL_DTMB_frame_strt               (AVL_DTMB_status_regs_base + 0x00000010)
#define rs_AVL_DTMB_frm_strt_indx            (AVL_DTMB_status_regs_base + 0x00000012)
#define rs_AVL_DTMB_demod_state_reg          (AVL_DTMB_status_regs_base + 0x00000014)
#define rs_AVL_DTMB_demod_state              (AVL_DTMB_status_regs_base + 0x00000014)
#define rs_AVL_DTMB_buffer_status            (AVL_DTMB_status_regs_base + 0x0000003c)
#define rs_AVL_DTMB_buffer_slack_slack       (AVL_DTMB_status_regs_base + 0x0000003c)
#define rs_AVL_DTMB_buffer_slack             (AVL_DTMB_status_regs_base + 0x0000003e)
#define rs_AVL_DTMB_debug_reg_1              (AVL_DTMB_status_regs_base + 0x00000040)
#define rs_AVL_DTMB_acq_status               (AVL_DTMB_status_regs_base + 0x00000044)
#define rs_AVL_DTMB_cfo_acc                  (AVL_DTMB_status_regs_base + 0x00000048)
#define rs_AVL_DTMB_patch_ver                (AVL_DTMB_status_regs_base + 0x0000004c)
#define rs_AVL_DTMB_carrier_freq_bin_Hz      (AVL_DTMB_status_regs_base + 0x00000050)
#define rs_AVL_DTMB_num_tps_code_fec_lock_attempts (AVL_DTMB_status_regs_base + 0x00000054)
#define rs_AVL_DTMB_non_coh_coarse_cfo       (AVL_DTMB_status_regs_base + 0x00000058)
#define rs_AVL_DTMB_coh_coarse_cfo           (AVL_DTMB_status_regs_base + 0x0000005c)
#define rs_AVL_DTMB_single_head_corr_cfo     (AVL_DTMB_status_regs_base + 0x00000060)
#define rs_AVL_DTMB_total_coarse_cfo         (AVL_DTMB_status_regs_base + 0x00000064)
#define rs_AVL_DTMB_demod_lock_status        (AVL_DTMB_status_regs_base + 0x00000068)
#define rs_AVL_DTMB_acq_retry_nums           (AVL_DTMB_status_regs_base + 0x00000068)
#define rs_AVL_DTMB_num_lock_retries         (AVL_DTMB_status_regs_base + 0x0000006a)
#define rs_AVL_DTMB_tps_detection_stats      (AVL_DTMB_status_regs_base + 0x0000006c)
#define rs_AVL_DTMB_num_valid_tps_detected   (AVL_DTMB_status_regs_base + 0x0000006c)
#define rs_AVL_DTMB_num_decode_tps_calls     (AVL_DTMB_status_regs_base + 0x0000006e)
#define rs_AVL_DTMB_cci_detection_status     (AVL_DTMB_status_regs_base + 0x00000070)
#define rs_AVL_DTMB_cci_exist                (AVL_DTMB_status_regs_base + 0x00000071)
#define rs_AVL_DTMB_impulse_exist            (AVL_DTMB_status_regs_base + 0x00000073)
#define rs_AVL_DTMB_static_mobile_detection_status (AVL_DTMB_status_regs_base + 0x00000074)
#define rs_AVL_DTMB_static_eq_mode           (AVL_DTMB_status_regs_base + 0x00000076)
#define rs_AVL_DTMB_curr_mobile1_static0     (AVL_DTMB_status_regs_base + 0x00000077)
#define rs_AVL_DTMB_dfe_status               (AVL_DTMB_status_regs_base + 0x00000078)
#define rs_AVL_DTMB_dfe_start                (AVL_DTMB_status_regs_base + 0x0000007b)
#define rs_AVL_DTMB_fw_bch_blk_counter       (AVL_DTMB_status_regs_base + 0x0000007c)
#define rs_AVL_DTMB_pre_bch_blk_counter      (AVL_DTMB_status_regs_base + 0x0000007c)
#define rs_AVL_DTMB_fw_cntrl                 (AVL_DTMB_status_regs_base + 0x00000080)
#define rs_AVL_DTMB_demod_status             (AVL_DTMB_status_regs_base + 0x00000080)
#define rs_AVL_DTMB_bch_blk_cnt_det_times    (AVL_DTMB_status_regs_base + 0x00000081)
#define rs_AVL_DTMB_pre_ddc_frame_counter    (AVL_DTMB_status_regs_base + 0x00000082)
#define rs_AVL_DTMB_noise_var_status         (AVL_DTMB_status_regs_base + 0x00000084)
#define rs_AVL_DTMB_noise_var                (AVL_DTMB_status_regs_base + 0x00000084)
#define rs_AVL_DTMB_fw_cntrl_2               (AVL_DTMB_status_regs_base + 0x00000088)
#define rs_AVL_DTMB_mobile_off_frms_sw       (AVL_DTMB_status_regs_base + 0x00000088)
#define rs_AVL_DTMB_spectrum_flipflop_retry  (AVL_DTMB_status_regs_base + 0x00000089)
#define rs_AVL_DTMB_ch_relock_flag           (AVL_DTMB_status_regs_base + 0x0000008a)
#define rs_AVL_DTMB_spectrum_flipflop        (AVL_DTMB_status_regs_base + 0x0000008b)
#define rs_AVL_DTMB_mean_min_seg_var_status  (AVL_DTMB_status_regs_base + 0x0000008c)
#define rs_AVL_DTMB_mean_min_seg_var         (AVL_DTMB_status_regs_base + 0x0000008c)
#define rs_AVL_DTMB_channel_correlation_status (AVL_DTMB_status_regs_base + 0x00000090)
#define rs_AVL_DTMB_channel_corr             (AVL_DTMB_status_regs_base + 0x00000092)
#define rs_AVL_DTMB_dvbc_modulation_status   (AVL_DTMB_status_regs_base + 0x00000094)
#define rs_AVL_DTMB_dvbc_constellation_mode  (AVL_DTMB_status_regs_base + 0x00000097)
#define rs_AVL_DTMB_carrier_freq_offset_Hz   (AVL_DTMB_status_regs_base + 0x00000098)
#define rs_AVL_DTMB_fec_loop_status          (AVL_DTMB_status_regs_base + 0x0000009c)
#define rs_AVL_DTMB_loop                     (AVL_DTMB_status_regs_base + 0x0000009c)
#define rs_AVL_DTMB_loop_max                 (AVL_DTMB_status_regs_base + 0x0000009e)
#define rs_AVL_DTMB_fec_sync_occurance       (AVL_DTMB_status_regs_base + 0x000000a0)
#define rs_AVL_DTMB_max_occ                  (AVL_DTMB_status_regs_base + 0x000000a0)
#define rs_AVL_DTMB_min_occurance            (AVL_DTMB_status_regs_base + 0x000000a2)
#define rs_AVL_DTMB_fec_trials               (AVL_DTMB_status_regs_base + 0x000000a4)
#define rs_AVL_DTMB_indx_trial               (AVL_DTMB_status_regs_base + 0x000000a4)
#define rs_AVL_DTMB_max_trial                (AVL_DTMB_status_regs_base + 0x000000a6)
#define rs_AVL_DTMB_fec_fcnt                 (AVL_DTMB_status_regs_base + 0x000000a8)
#define rs_AVL_DTMB_fcnt_step                (AVL_DTMB_status_regs_base + 0x000000a8)
#define rs_AVL_DTMB_fcnt_step_temp           (AVL_DTMB_status_regs_base + 0x000000aa)
#define rs_AVL_DTMB_fec_sync_status          (AVL_DTMB_status_regs_base + 0x000000ac)
#define rs_AVL_DTMB_acq_state                (AVL_DTMB_status_regs_base + 0x000000ac)
#define rs_AVL_DTMB_dvbs_dss_fec_lock        (AVL_DTMB_status_regs_base + 0x000000ae)
#define rs_AVL_DTMB_fec_sync_status2         (AVL_DTMB_status_regs_base + 0x000000b0)
#define rs_AVL_DTMB_sync_indx                (AVL_DTMB_status_regs_base + 0x000000b0)
#define rs_AVL_DTMB_sync_indx_unused         (AVL_DTMB_status_regs_base + 0x000000b2)
#define rs_AVL_DTMB_fec_err_accum            (AVL_DTMB_status_regs_base + 0x000000b4)

	
	/// Defines the carrier mode of the received DTMB signal.
	typedef enum AVL_DTMB_CarrierMode
	{
		RX_MULTI_CARRIER = 0,	///< =0
		RX_SINGLE_CARRIER = 1	///< =1
	}AVL_DTMB_CarrierMode;

	/// Defines the frame header mode of the received DTMB signal.
	typedef enum AVL_DTMB_FrameHeaderMode
	{
		RX_PN420 = 1,	///< =1
		RX_PN595 = 2,	///< =2
		RX_PN945 = 3	///< =3
	}AVL_DTMB_FrameHeaderMode;

	/// Defines the symbol interleave mode of the received DTMB signal.
	typedef enum AVL_DTMB_InterleaveMode
	{
		RX_INTERLEAVE_240 = 0,	///< =0
		RX_INTERLEAVE_720 = 1	///< =1
	}AVL_DTMB_InterleaveMode;

	/// Defines whether NR (Nordstrom Robinson) mapping is used for modulation mode 4QAM. 	
	typedef enum AVL_DTMB_NR_Mode
	{
		RX_NR_OFF = 0,		///< =0 NR mapping off
		RX_NR_ON = 1		///< =1 NR mapping on
	}AVL_DTMB_NR_Mode;

    /// Defines the FEC rate of the received DTMB signal.
	typedef enum AVL_DTMB_FEC_Rate
	{
		RX_FEC_2_5 = 0,		///< =0
		RX_FEC_3_5 = 1,		///< =1
		RX_FEC_4_5 = 2		///< =2
	}AVL_DTMB_FEC_Rate;

	/// Defines the modulation mode of the received DTMB signal.
	typedef enum AVL_DTMB_ModulationMode
	{
		RX_DTMB_4QAM = 0,		///< =0
		RX_DTMB_16QAM = 1,		///< =1
		RX_DTMB_32QAM = 2,	    ///< =2
		RX_DTMB_64QAM = 3,		///< =3
	}AVL_DTMB_ModulationMode;

	/// Holds the received DTMB signal information parameters.
	typedef struct AVL_DTMB_SignalInfo
	{
		enum AVL_DTMB_CarrierMode m_CarrierMode;		///< The carrier mode of the signal.  \sa::AVL_DTMB_CarrierMode.
		enum AVL_DTMB_FEC_Rate m_CodeRate;				///< The FEC rate of the signal. \sa ::AVL_DTMB_FEC_Rate.
		enum AVL_DTMB_ModulationMode m_Modulation;		///< The modulation mode of the signal. \sa ::AVL_DTMB_ModulationMode.
		enum AVL_DTMB_NR_Mode m_MappingMode;			///< The NR mapping mode of the signal. \sa ::AVL_DTMB_NR_Mode.
		enum AVL_DTMB_InterleaveMode m_InterleaveMode;	///< The interleave mode of the signal. \sa ::AVL_DTMB_InterleaveMode.
		enum AVL_DTMB_FrameHeaderMode m_HeaderMode;	///< The frame header mode of the signal. \sa ::AVL_DTMB_FrameHeaderMode.
	}AVL_DTMB_SignalInfo;
	
    /// Holds the DTMB channel information. The structure contains the parameters used for locking to a channel.
    typedef struct AVL_DTMB_Channel
    {
    	AVL_uchar m_demod_mode;			                        ///<Indicates the desired standard mode to operate the demod in as per the specified channel.
    															///< Should be set to one of the four macros defining different supported standards by an Availink part in AVL63X1_include.h
    	AVL_uchar m_channel_id;									///< Defines the channel identifier to specify a particular demod channel in operation incase of multi-channel supported part.
    	AVL_uint32 m_ConfigOptions;		                        ///< This parameter is presently not used, but is reserved for future configuration options.  
    	AVL63X1_Chip *pAVLChip;								    ///< Defines a pointer to the Availink chip structure that will be used to operate upon the specified channel.				
    	AVL_uint32 m_uiFrequency_kHz;						    ///< The carrier frequency of the channel in units of kHz.
    	struct AVL_DTMB_SignalInfo m_SignalInfo;				///< The signal information.
    	enum AVL63X1_SpectrumInversion m_SpectrumInversion;	///< The spectrum inversion setting. Controls whether the spectrum is inverted.
    }AVL_DTMB_Channel;


	AVL63X1_ErrorCode AVL_DTMB_Halt( struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DTMB_Wakeup( struct AVL63X1_Chip * pAVL_Chip );
	AVL63X1_ErrorCode AVL_DTMB_GetRunningLevel( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip );


	#define AUTO_MODE					0x1
	#define MOBILE_MODE 				0x2
	#define STATIC_MODE 				0x3
	#define MAX_LOCK_RETRIES			0x2

    AVL63X1_ErrorCode AVL_DTMB_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip);
    AVL63X1_ErrorCode AVL_DTMB_GetLockStatus( AVL_puint16 puiLockStatus, AVL_puint16 puiNumLockRetries, const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_GetSignalInfo(struct AVL_DTMB_SignalInfo *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_GetSignalLevel(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_Initialize(struct AVL63X1_Chip * pAVL_Chip);
    AVL63X1_ErrorCode AVL_DTMB_LockChannel( struct AVL63X1_Chip * pAVL_Chip, enum AVL63X1_LockMode enumLockMode, const struct AVL_DTMB_Channel * psChannel );
    AVL63X1_ErrorCode AVL_DTMB_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip );
    AVL63X1_ErrorCode AVL_DTMB_SetModeforDTMB( AVL_uint32 uiSetMode,const struct AVL63X1_Chip * pAVL_Chip );

	
	#ifdef AVL_CPLUSPLUS
}
	#endif
#endif

