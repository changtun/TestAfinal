##
##
## Copyright 2008, The Android Open Source Project
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
base := $(LOCAL_PATH)/../../../../..


LOCAL_SRC_FILES := \
	control/nporuntime.cpp \
	control/npolibpv.cpp \
	pvshell.cpp \
	pvplugin.cpp \
	npandroid.cpp \
	RenderingThread.cpp \
	animation/AnimationPlugin.cpp \
	animation/AnimationThread.cpp \
	audio/AudioPlugin.cpp \
	background/BackgroundPlugin.cpp \
	form/FormPlugin.cpp \
	navigation/NavigationPlugin.cpp \
	paint/PaintPlugin.cpp \
	video/VideoPlugin.cpp \
	jni-bridge.cpp \
	log/anpdebug.c \
	codechar/codechar.c \
	adapt/npdvb.c \
	adapt/nphls.c \
	adapt/npepg.c \
	adapt/npsystem.c \
	adapt/npinstall.c \
	adapt/npdatabase.c \
	adapt/cJSON.c	\

LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE) \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/animation \
	$(LOCAL_PATH)/audio \
	$(LOCAL_PATH)/background \
	$(LOCAL_PATH)/form \
	$(LOCAL_PATH)/navigation \
	$(LOCAL_PATH)/paint \
	$(LOCAL_PATH)/log \
	$(LOCAL_PATH)/video \
	$(LOCAL_PATH)/adapt \
	$(LOCAL_PATH)/codechar \
	external/webkit/Source/WebCore/bridge \
	external/webkit/Source/WebCore/plugins \
	external/webkit/Source/WebCore/platform/android/JavaVM \
	external/webkit/Source/WebKit/android/plugins \
	external/skia/include/core  \
	external/sqlite/dist \
	$(base)/../../../../include \
	$(LOCAL_PATH)/../../libdvb/dvbadapt/include \
	$(LOCAL_PATH)/../../libdvb/dvbcore/include \
	$(LOCAL_PATH)/../../libdvb/dvbcore/src/dal \
	$(LOCAL_PATH)/../../libdvb/ctica/include \
	$(LOCAL_PATH)/../../libdvb/libiconv/include \
	$(LOCAL_PATH)/../../include/am_adp \
	$(base)/kernel/include/linux/dvb \
	$(base)/kernel/include/linux/amports \
	
LOCAL_SHARED_LIBRARIES := \
	liblog \
	libnativehelper \
	libandroid \
	libutils \
	libcutils \
	libEGL \
	libGLESv2 \
	libskia \
	libsqlite \
	libdvbcore \
	libdvbadapt \
	libdvbiconv \
  libtrcas \
	libsqlite \

LOCAL_LDLIBS := -llog -liconv
LOCAL_CFLAGS += -fvisibility=hidden 
LOCAL_PRELINK_MODULE:=false

LOCAL_MODULE:= libdvbnppv

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

