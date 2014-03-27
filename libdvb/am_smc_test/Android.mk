LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= am_smc_test.c

LOCAL_MODULE:= am_smc_test

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS+=-DANDROID -DAMLINUX -DCHIP_8226M -DLINUX_DVB_FEND
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/am_adp $(LOCAL_PATH)/../../android/ndk/include
				
LOCAL_SHARED_LIBRARIES := liblog libc
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../am_adp/libam_adp.so

include $(BUILD_EXECUTABLE)

