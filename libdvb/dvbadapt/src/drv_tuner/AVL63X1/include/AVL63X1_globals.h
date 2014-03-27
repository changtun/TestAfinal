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
/// @brief Defines registers address for the AVL63X1 device.
/// 

#ifndef AVL63X1_GLOBALS_H
#define AVL63X1_GLOBALS_H
	
#define AVL63X1_config_regs_base (0x00000204)

#define rs_AVL63X1_core_ready_word				    0x000000a0
#define rc_AVL63X1_c306_top_srst				    0x00110840
#define rs_AVL63X1_current_active_mode				0x200

#define rc_AVL63X1_reset_register				    0x00100000
#define rc_AVL63X1_pll_reg_mode					    0x00100004
#define rc_AVL63X1_pll_clkr						    0x00100080
#define rc_AVL63X1_pll_clkf						    0x001000c0
#define rc_AVL63X1_pll_od						    0x00100100
#define rc_AVL63X1_pll_od2						    0x00100140
#define rc_AVL63X1_pll_od3						    0x00100180
#define rc_AVL63X1_pll_bwadj					    0x001001c0
#define rc_AVL63X1_pll_rst_lock_indicators		    0x00100200
#define rs_AVL63X1_pll_lock_status				    0x00100240

#define rc_AVL63X1_dll_init						    0x00100008
#define rc_AVL63X1_dll_dly_control_lin				0x00100014	
#define rc_AVL63X1_dll_out_phase				    0x00100018
#define rc_AVL63X1_dll_rd_phase					    0x0010001c

#define rc_AVL63X1_cpu_ddc_srst						0x00110084
#define rc_AVL63X1_cpu_valu_srst					0x00110088

#define rc_AVL63X1_nordic_adc_control				0x00104000
#define rc_AVL63X1_adc_reg_capture					0x00104004
#define rc_AVL63X1_mission0_adcbypass1				0x00108000

#define rs_AVL63X1_rom_ver						    0x00040000


#define rc_AVL63X1_esm_cntrl				        0x00149004
#define rs_AVL63X1_byte_num					        0x0014900C
#define rs_AVL63X1_ber_err_cnt				        0x00149008
#define rs_AVL63X1_packet_err_cnt			        0x00149010
#define rs_AVL63X1_packet_num				        0x00149014

#define rs_AVL63X1_rx_stdout_fifo					0x00114000 

#define rc_AVL63X1_command			        		(AVL63X1_config_regs_base + 0x00000000)
#define rs_AVL63X1_sdram_test_return        		(AVL63X1_config_regs_base + 0x00000004)
#define rs_AVL63X1_sdram_test_result        		(AVL63X1_config_regs_base + 0x00000008)
#define rc_AVL63X1_sdram_config             		(AVL63X1_config_regs_base + 0x0000000c)
#define rc_AVLPRT_sdram_col_addr_width      		(AVL63X1_config_regs_base + 0x0000000d)
#define rc_AVL63X1_sdram_row_addr_width     		(AVL63X1_config_regs_base + 0x0000000e)
#define rc_AVL63X1_sdram_bank_addr_width    		(AVL63X1_config_regs_base + 0x0000000f)
#define rc_AVL63X1_sdram_timing_config_0    		(AVL63X1_config_regs_base + 0x00000010)
#define rc_AVL63X1_sdram_t_rc               		(AVL63X1_config_regs_base + 0x00000010)
#define rc_AVL63X1_sdram_t_xsr              		(AVL63X1_config_regs_base + 0x00000011)
#define rc_AVL63X1_sdram_t_rcar             		(AVL63X1_config_regs_base + 0x00000012)
#define rc_AVL63X1_sdram_t_wr               		(AVL63X1_config_regs_base + 0x00000013)
#define rc_AVL63X1_sdram_timing_config_1    		(AVL63X1_config_regs_base + 0x00000014)
#define rc_AVL63X1_sdram_t_rp               		(AVL63X1_config_regs_base + 0x00000014)
#define rc_AVL63X1_sdram_t_rcd              		(AVL63X1_config_regs_base + 0x00000015)
#define rc_AVL63X1_sdram_t_ras              		(AVL63X1_config_regs_base + 0x00000016)
#define rc_AVL63X1_sdram_cas_latency        		(AVL63X1_config_regs_base + 0x00000017)
#define rc_AVL63X1_acquisition_control      		(AVL63X1_config_regs_base + 0x00000018)
#define rc_AVL63X1_fixed_pn_phase           		(AVL63X1_config_regs_base + 0x00000018)
#define rc_AVL63X1_tps_settle_frames        		(AVL63X1_config_regs_base + 0x00000019)
#define rc_AVL63X1_bypass_time_deint        		(AVL63X1_config_regs_base + 0x0000001a)
#define rc_AVL63X1_get_config_from_tps      		(AVL63X1_config_regs_base + 0x0000001b)
#define rc_AVL63X1_i2cm_repeater_type       		(AVL63X1_config_regs_base + 0x0000001c)
#define rc_AVL63X1_fw_image_info            		(AVL63X1_config_regs_base + 0x00000020)
#define rc_AVL63X1_fw_download_method       		(AVL63X1_config_regs_base + 0x00000021)
#define rc_AVL63X1_demod_mode               		(AVL63X1_config_regs_base + 0x00000022)
#define rc_AVL63X1_copy_config_area         		(AVL63X1_config_regs_base + 0x00000023)
#define rc_AVL63X1_dtmb_compressed_ram_start_addr 	(AVL63X1_config_regs_base + 0x00000024)
#define rc_AVL63X1_dtmb_uncompressed_ram_start_addr (AVL63X1_config_regs_base + 0x00000028)
#define rc_AVL63X1_dvbc_compressed_ram_start_addr 	(AVL63X1_config_regs_base + 0x0000002c)
#define rc_AVL63X1_dvbc_uncompressed_ram_start_addr (AVL63X1_config_regs_base + 0x00000030)
#define rc_AVL63X1_ADC_test_Config1         		(AVL63X1_config_regs_base + 0x00000034)
#define rc_AVL63X1_ADC_I0_Q1_test_sel       		(AVL63X1_config_regs_base + 0x00000036)
#define rc_AVL63X1_adc_swing_rng            		(AVL63X1_config_regs_base + 0x00000037)
#define rc_AVL63X1_ADC_test_Config2         		(AVL63X1_config_regs_base + 0x00000038)
#define rc_AVL63X1_ADC_CI_threshold         		(AVL63X1_config_regs_base + 0x00000038)
#define rc_AVL63X1_ADC_SNDR_threshold       		(AVL63X1_config_regs_base + 0x0000003a)
#define rc_AVL63X1_ADC_test_status_reg1     		(AVL63X1_config_regs_base + 0x0000003c)
#define rc_AVL63X1_SFDR_100dB_Q             		(AVL63X1_config_regs_base + 0x0000003c)
#define rc_AVL63X1_SNDR_100dB_Q             		(AVL63X1_config_regs_base + 0x0000003e)
#define rc_AVL63X1_ADC_test_status_reg2     		(AVL63X1_config_regs_base + 0x00000040)
#define rc_AVL63X1_SNDR_100dB_I             		(AVL63X1_config_regs_base + 0x00000040)
#define rc_AVL63X1_SNR_100dB_Q              		(AVL63X1_config_regs_base + 0x00000042)
#define rc_AVL63X1_ADC_test_status_reg3     		(AVL63X1_config_regs_base + 0x00000044)
#define rc_AVL63X1_SNR_100dB_I              		(AVL63X1_config_regs_base + 0x00000044)
#define rc_AVL63X1_SFDR_100dB_I             		(AVL63X1_config_regs_base + 0x00000046)
#define rc_AVL63X1_Q_PEAK                   		(AVL63X1_config_regs_base + 0x00000048)
#define rc_AVL63X1_I_PEAK                  			(AVL63X1_config_regs_base + 0x0000004c)
#define rc_AVL63X1_CI_100dB                 		(AVL63X1_config_regs_base + 0x00000050)
#define rc_AVL63X1_chip_id_status           		(AVL63X1_config_regs_base + 0x00000054)
#define rc_AVL63X1_sdram_type               		(AVL63X1_config_regs_base + 0x00000057)


#define rc_AVL63X1_i2cm_cmd_addr                    0x00000800
#define rc_AVL63X1_i2cm_rsp_addr                    0x00000814
#define rs_AVL63X1_i2cm_status_addr			        0x00000828	

#define rc_AVL63X1_tuner_i2c_cntrl				    0x00118004 
#define rc_AVL63X1_tuner_i2c_clk_div			    0x00118008 
#define rc_AVL63X1_intr_clr_reg				        0x0011001C 
#define rc_AVL63X1_tuner_i2c_srst				    0x00118000 
#define rc_AVL63X1_tuner_i2cm_con				    0x00118560 
#define rc_AVL63X1_tuner_i2c_read_cnt			    0x00118010 
#define rs_AVL63X1_tuner_i2c_error_flag		        0x00118014
#define rc_AVLAPRT_tuner_hw_i2c_bit_rpt_clk_div		0x00118018
#define rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl		0x0011801C


#define rc_AVL63X1_mpeg_bus_tri_enb			        0x00108030	
#define rc_AVL63X1_rfagc_tri_enb			        0x00108034
#define rc_AVL63X1_mpeg_cntrl				        0x00149000
#define rc_AVL63X1_cntns_pkt_para_rate_frac_n		0x0014909c
#define rc_AVL63X1_cntns_pkt_para_rate_frac_d		0x001490a0
#define rc_AVL63X1_pkt_seri_rate_frac_n		        0x00149018
#define rc_AVL63X1_pkt_seri_rate_frac_d     		0x0014901c

#endif
