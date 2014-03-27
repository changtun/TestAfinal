########################
# MODULE : dvbplayer  #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
ST20_TARGETS := dvbplayer.lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
HISI_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))

ST20_OBJS := dvbplayer.tco tlock.tco dvbplayer_proc.tco tmsgqueue.tco dvbplayer_av.tco  \
		dvbplayer_callbackandpmt.tco 

ST40_OBJS := $(call ST20OBJ_TO_ST40OBJ,$(ST20_OBJS))
ST200_OBJS := $(call ST20OBJ_TO_ST200OBJ,$(ST20_OBJS))
HISI_OBJS := $(call ST20OBJ_TO_LINUXOBJ,$(ST20_OBJS))

ST20_EXPORTS :=
ST40_EXPORTS :=
HISI_EXPORTS :=

# Sub-libs :
#--------------------------
# Origianl
SUBDIRS :=#

# Pick up standars targets
include $(DVD_MAKE)/defrules.mak

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
dvbplayer.tco: dvbplayer.c
dvbplayer_av.tco: dvbplayer_av.c dvbplayer_av.h
dvbplayer_callbackandpmt.tco: dvbplayer_callbackandpmt.c dvbplayer_callbackandpmt.h
tmsgqueue.tco: tmsgqueue.c tmsgqueue.h
tlock.tco: tlock.c tlock.h
dvbplayer_proc.tco: dvbplayer_proc.c dvbplayer_proc.h
#end of file




