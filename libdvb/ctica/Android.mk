LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SHARED_LIBRARIES := \
                            libutils \
                            libc \
                            libdvbadapt \
                            libdvbcore \
                            liblog

LOCAL_LDFLAGS := \
		$(LOCAL_PATH)/src/ctilib/CTI-CAcore-lib-Amlogic-20131213.a \
		$(LOCAL_PATH)/../../am_adp/libam_adp.so


LOCAL_SRC_FILES := \
				src/ctios.c \
				src/ctisc.c \
				src/ctidmux.c \
				src/ctinvm.c \
				src/ctisys.c	\
				##am_smc_test.c

LOCAL_MODULE := libtrcas

base := $(LOCAL_PATH)/../../../../../

LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/../dvbadapt/include \
                    $(LOCAL_PATH)/../dvbcore/include  \
                    $(LOCAL_PATH)/include \
			$(LOCAL_PATH)/../../include/am_adp \
			$(base)/kernel/include/linux/dvb \
			$(base)/kernel/include/linux/amports \




##include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)