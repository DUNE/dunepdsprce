# -*-Mode: makefile;-*-

# ======================================================================
#
#  PURPOSE
#  -------
#  Defines the host and target cpu and operating system. 
#  The host cpu and os are gotten from uname, wherea the target cpu and
#  os are specified by the variable TARGET.  If TARGET is not specified
#  it is defaulted to the host
# ======================================================================




export HOST TARGET

HOST_CPU := $(shell uname -m)
HOST_OS  := $(shell uname -s)

ifeq ($(HOST_OS),Linux)
   HOST_OS := linux
endif

ifeq ($(HOST_CPU),armv7l)
   HOST_CPU = arm_CA9
endif


HOST := $(HOST_CPU)-$(HOST_OS)
#$(info HOST = $(HOST))


# ---------------------------------------------
# Define and verify the host/target combination
# ---------------------------------------------
VALID_TARGET_SET := x86_32-linux x86_64-linux arm_CA9-linux arm_CA9-rtems
ifdef TARGET


   # ----------------------------
   # Check if have a valid target
   # ----------------------------
  ifeq ($(filter $(TARGET),$(VALID_TARGET_SET)),) 
    $(info  Error: Invalid target, TARGET=<$(TARGET)> is not one of $(VALID_TARGET_SET))
    $(error Quitting)
  endif

  # --- Change fields of the target to whitespace delimited list
  space :=
  space += 
  tmp = $(subst -,$(space),$(TARGET))
  TARGET_CPU := $(word 1,$(tmp))
  TARGET_OS  := $(word 2,$(tmp))

else

  # ----------------------------------------------
  # TARGET not define, default to the host machine
  # ----------------------------------------------
  TARGET = $(HOST_CPU)-$(HOST_OS)
  TARGET_CPU = $(HOST_CPU)
  TARGET_OS  = $(HOST_OS)

endif
