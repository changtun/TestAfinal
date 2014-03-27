########################
# MODULE : driver  #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
COMPONENT_NAME := driver

ST20_TARGETS := $(COMPONENT_NAME).lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
HISI_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))

ST20_EXPORTS :=
ST40_EXPORTS :=
HISI_EXPORTS :=

#define the include path for a module
#--------------------------------------
MODULE_INCLUDE :=#
ifneq "$(MODULE_INCLUDE)" ""
  VPATH += $(MODULE_INCLUDE)
  export VPATH MODULE_INCLUDE
endif

# Pick up standars targets
#-------------------------
include $(DVD_MAKE)/defrules.mak

# No component in the top-level : run make in ./src
#---------------------------------------------------
FULLDIR := $(DVD_BUILD_DIR)/src/objs/$(OBJECT_DIRECTORY)
$($(ARCHITECTURE)_TARGETS): $(FULLDIR)/$($(ARCHITECTURE)_TARGETS)
	$(CP) $(subst $(BAD_SLASH),$(GOOD_SLASH),$<) $@ > $(NULL)

$(FULLDIR)/$($(ARCHITECTURE)_TARGETS): FORCE
	@$(ECHO) Entering SRC directory	
	$(MAKE) -C $(DVD_BUILD_DIR)/src


# Default rule
#-------------
FORCE:

# Clean the lib
#--------------
clean: subdir_clean
	@$(ECHO) Cleaning $($(ARCHITECTURE)_TARGETS)
	-$(RM) $($(ARCHITECTURE)_TARGETS)

subdir_clean:
	$(MAKE) -C $(DVD_BUILD_DIR)/src clean

else

include $(DVD_MAKE)/builddir.mak

endif
#end of file


