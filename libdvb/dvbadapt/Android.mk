LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
## nuTUNE is DVB-T, AVL63X1 is DVB-C
##TUNER_TYPE := AVL63X1

##LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SHARED_LIBRARIES := \
				libutils \
				libc \
				liblog \
				libcutils 

LOCAL_LDLIBS := -llog
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../am_adp/libam_adp.so \
					$(LOCAL_PATH)/../../pbi_tuner/libdvbtuner.so 

LOCAL_SRC_FILES := \
				src/drv_osapi/os_linux.c \
				src/drv_avctrl/drv_avctrl.c \
				src/drv_filter/drv_filter.c \
				src/drv_api/drv_api.c \
				src/drv_api/file_config.c \
				src/drv_pvr/drv_pvr.c \
				src/drv_pvr/drv_pvr_player.c \
				src/drv_pmoc/drv_pmoc.c \
				src/drv_api/system_info.c
				
LOCAL_MODULE := libdvbadapt

base := $(LOCAL_PATH)/../../../../../

LOCAL_C_INCLUDES += \
				$(base)/kernel/include/linux/dvb \
				$(base)/kernel/include/linux/amports \
				$(LOCAL_PATH)/../../include/am_adp \
				$(LOCAL_PATH)/include 
				
##include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)