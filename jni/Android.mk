#############################################################
#
# libdvbcore-jni
#
#############################################################

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SHARED_LIBRARIES := \
                          liblog \
                          libhardware \
                          libandroid_runtime \
                          libnativehelper \
                          libdvbcore \
                          libdvbadapt \
                          libdvbiconv \
                          libtrcas
LOCAL_LDFLAGS := $(LOCAL_PATH)/../pbi_tuner/libdvbtuner.so

LOCAL_SRC_FILES := \
										logit.c \
										native_installation.cpp \
										native_player.cpp \
										native_drive.cpp \
                   	tuner.cpp \
										installation_message.cpp \
										installation_manage.cpp \
										player_message.cpp \
										player_manage.cpp \
                   	jnistaple.cpp  \
                   	ca_manage.cpp \
                   	ca_message.cpp \
                   	native_epg.cpp \
                   	pbi_coding.cpp \
					frontpanel.cpp \
                    message_nitupdate.cpp \
                    pvr_message.cpp \
                    pvr_manage.cpp \
		    native_system_info.cpp \
										native_epg_message.cpp\
										native_ca.cpp\
										msg_timeUpdate.cpp\
                    message_nit_download.cpp \
                    download_message.cpp \
                    native_download.cpp \
				    com_pbi_dvb_domain_ConfigBean.cpp
                    
                   

LOCAL_MODULE := libdvbcore-jni

LOCAL_LDLIBS := -llog -liconv

base := $(LOCAL_PATH)/../../../../

LOCAL_C_INCLUDES += \
					$(JNI_H_INCLUDE) \
					$(base)/frameworks/base/include \
				    $(base)/system/core/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp_base/mpi/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp_base/common/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp_base/ha_codec/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp_base/mpi/drv/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp/graphics/fb/include \
                    $(base)/device/hisilicon/godbox/driver/sdk/msp_base/ecs/include \
                    $(base)/dalvik/libnativehelper/include/nativehelper \
                    $(LOCAL_PATH)/../libdvb/dvbcore/include \
                    $(LOCAL_PATH)/../libdvb/dvbadapt/include \
                    $(LOACL_PATH)/../libdvb/libiconv/include \
                    $(LOCAL_PATH)/../libdvb/ctica/include \
			$(LOCAL_PATH)/../include/am_adp \
			$(LOCAL_PATH)/../include/am_mw \
			$(base)/kernel/include/linux/dvb \
			$(base)/kernel/include/linux/amports \



LOCAL_CFLAGS += -fPIC -O2
LOCAL_CFLAGS += -DTARGET_ANDROID -DUSE_LOGIT=1 -DPACKAGE_NAME=com_pbi_dvb_DVB -DPACKAGE_DIR=\"com/pbi/dvb/DVB\"

include $(BUILD_SHARED_LIBRARY)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))

