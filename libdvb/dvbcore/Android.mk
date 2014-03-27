LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SHARED_LIBRARIES := \
                            libutils \
                            libc \
                            libdvbadapt \
                            liblog	

LOCAL_LDLIBS := -llog
LOCAL_LDFLAGS := \
			$(LOCAL_PATH)/../curl/curl-7.26.0/libcurl.so \
			$(LOCAL_PATH)/../../am_adp/libam_adp.so \
			$(LOCAL_PATH)/../../pbi_tuner/libdvbtuner.so \

LOCAL_SRC_FILES := \
                   src/util/crc.c

LOCAL_SRC_FILES += \
                   src/psi_si/table.c \
                   src/psi_si/descriptor.c \
                   src/psi_si/nit.c \
                   src/psi_si/pat.c \
                   src/psi_si/bat.c \
                   src/psi_si/sdt.c \
                   src/psi_si/pmt.c \
                   src/psi_si/tdt_tot.c

LOCAL_SRC_FILES += \
                   src/installation/installation.c

LOCAL_SRC_FILES += \
                   src/background/background.c
LOCAL_SRC_FILES += \
                   src/systime/systime.c\
                   src/systime/bcd.c
LOCAL_SRC_FILES += \
                   src/epg/epg.c\
                   src/epg/epg_common.c\
                   src/epg/gmem.c\
                   src/epg/gtree.c\
                   src/epg/
LOCAL_SRC_FILES += \
                   src/database/usm_unicode_gb.c

LOCAL_SRC_FILES += \
                   src/dvbplayer/dvbplayer.c \
                   src/dvbplayer/tlock.c \
                   src/dvbplayer/dvbplayer_proc.c \
                   src/dvbplayer/tmsgqueue.c \
                   src/dvbplayer/dvbplayer_av.c \
				   src/dvbplayer/dvbplayer_pvr.c \
                   src/dvbplayer/dvbplayer_callbackandpmt.c 
			   
LOCAL_SRC_FILES += \
                   src/dvb_download/dvb_download.c \
                   src/dal/anpdebug.c \
                   src/dal/dal.c \
                   src/dal/dal_common.c \
                   src/dal/dal_core.c \
                   src/dal/dal_download.c \
                   src/dal/dal_hw_player.c \
									 src/dal/dal_inject.c \
									 src/dal/dal_message.c \
									 src/dal/dal_parse.c \
									 src/dal/dal_sw_player.c \
									 src/dal/dal_wrap_linux.c \
									 src/dal/dal_hls.c \
									 src/dal/dal_timeshift.c \
									 src/dal/ring_buffer.c 

LOCAL_SRC_FILES += \
                   src/sys_debug/sys_debug.c 
                   
LOCAL_SRC_FILES += \
                   src/mempool/mempool.c \
                   src/mempool/sf_sdlist.c 
                   			   
LOCAL_MODULE := libdvbcore

base := $(LOCAL_PATH)/../../../../../

LOCAL_C_INCLUDES += \
			$(base)/kernel/include/linux/dvb \
			$(base)/kernel/include/linux/amports \
                    $(LOCAL_PATH)/../dvbadapt/include \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/src/psi_si \
                    $(LOCAL_PATH)/src/dvbplayer\
			$(LOCAL_PATH)../trcas/include \
			$(LOCAL_PATH)/../../include/am_adp \
			$(LOCAL_PATH)/../../include/am_mw \
										$(LOCAL_PATH)/../curl 


include $(BUILD_SHARED_LIBRARY)
