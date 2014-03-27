########################
# MODULE : sys_osapi #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
ST20_TARGETS := drv_osapi.lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
HISI_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))

ST20_OBJS :=

ifeq "$(DVD_OS)" "OS20"
  ST20_OBJS += os_os20.tco
else	
  ifeq "$(DVD_OS)" "OS21"
    ST20_OBJS += os_os21.tco
  else	
    ifeq "$(DVD_OS)" "LINUX"
      ST20_OBJS += os_linux.tco
   endif
  endif
endif

ST40_OBJS := $(call ST20OBJ_TO_ST40OBJ,$(ST20_OBJS))
ST200_OBJS := $(call ST20OBJ_TO_ST200OBJ,$(ST20_OBJS))
HISI_OBJS := $(call ST20OBJ_TO_LINUXOBJ,$(ST20_OBJS))

ST20_EXPORTS :=
ST40_EXPORTS :=
HISI_EXPORTS :=

# Sub-libs :
#--------------------------
# Origianl
SUBDIRS :=

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
#end of file

