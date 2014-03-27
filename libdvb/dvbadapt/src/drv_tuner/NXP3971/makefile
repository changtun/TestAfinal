########################
# MODULE :Avx_tuner  #
########################

DVD_MAKE_VERSION := 2
ifdef IN_OBJECT_DIR

include $(DVD_MAKE)/generic.mak

# Define the targets and exported files
#--------------------------------------
ST20_TARGETS := AVL63X1_603.lib
ST40_TARGETS := $(call ST20LIB_TO_ST40LIB,$(ST20_TARGETS))
HISI_TARGETS := $(call ST20LIB_TO_LINUXLIB,$(ST20_TARGETS))

ST20_OBJS := tmbslTDA10025.tco tmbslTDA10025Instance.tco tmbslTDA10025_Cfg.tco\
		tmbslHCDP.tco tmbslHCDP_Advanced.tco tmbslHCDP_Instance.tco tmbslHCDP_Reg.tco \
		 tmbslTDA18250.tco tmddTDA18250Instance.tco tmbslTDA18250Instance.tco tmddTDA18250.tco \
		 System_Lo_Detuning.tco tmsysOM3971Instance.tco tmsysOM3971.tco tmbslOM3971Main.tco 
		 #nim_tda10025.tco tun_tda18250.tco
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




