########################
# MODULE :Avx_tuner  #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
ST20_TARGETS := AVL63X1.lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
HISI_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))

ST20_OBJS := IBSP.tco AVL63X1_API.tco AVL63X1_fw.tco AVL63X1_Internal.tco\
			AVL.tco AVL_DTMB.tco AVL_DVBC.tco SemcoMxL601.tco SemcoMxL601_OEM_Drv.tco\
			SemcoMxL601_TunerApi.tco SemcoMxL601_TunerCfg.tco SemcoMxL601_TunerSpurTable.tco\
			AVL_tuner.tco #tuner_test.tco LockSignalAuto.tco
ST40_OBJS := $(call ST20OBJ_TO_ST40OBJ,$(ST20_OBJS))
ST200_OBJS := $(call ST20OBJ_TO_ST200OBJ,$(ST20_OBJS))
HISI_OBJS := $(call ST20OBJ_TO_LINUXOBJ,$(ST20_OBJS))

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




