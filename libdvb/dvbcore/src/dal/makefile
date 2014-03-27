########################
# MODULE : dal #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
ST20_TARGETS := dal.lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
LINUX_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))


ST20_OBJS := dal.tco dal_hw_player.tco dal_sw_player.tco \
             dal_core.tco dal_download.tco dal_parse.tco dal_inject.tco dal_wrap_linux.tco dal_message.tco \
             ring_buffer.tco flv_parse.tco flv_list_play.tco dal_mms.tco dal_common.tco

ST40_OBJS := $(call ST20OBJ_TO_ST40OBJ,$(ST20_OBJS))
ST200_OBJS := $(call ST20OBJ_TO_ST200OBJ,$(ST20_OBJS))
LINUX_OBJS := $(call ST20OBJ_TO_LINUXOBJ,$(ST20_OBJS))

ST20_EXPORTS :=
ST40_EXPORTS :=
LINUX_EXPORTS :=

# Sub-libs :
#--------------------------
# Origianl
SUBDIRS :=#

# Pick up standars targets
include $(DVD_MAKE)/defrules.mak

INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DevROOT)/src/platform/stapp/playrec)
INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DevROOT)/src/platform/opensource/curl-7.21.3/lib)
INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DevROOT)/src/platform/opensource/curl-7.21.3/include)
INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(DevROOT)/src/platform/opensource/libmms-0.6.2)

INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SDKROOT)/opensource/include)
INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SDKROOT)/opensource/include/glib-2.0)
INCLUDES += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SDKROOT)/opensource/include/glib-2.0/include)

# Build lib and obj
#------------------
$($(ARCHITECTURE)_TARGETS): $($(ARCHITECTURE)_OBJS) $(SUBDIR_LIBS)
	@$(ECHO) Building library $@
	$(BUILD_LIBRARY)

clean: $(SUBDIR_CLEAN)
	@$(ECHO) Cleaning $($(ARCHITECTURE)_TARGETS)
ifneq ($(strip $($(ARCHITECTURE)_OBJS)), )
	-$(RM) $($(ARCHITECTURE)_OBJS)
endif	
	-$(RM) $($(ARCHITECTURE)_TARGETS)

else

include $(DVD_MAKE)/builddir.mak

endif

# Local dependancies

#end of file


