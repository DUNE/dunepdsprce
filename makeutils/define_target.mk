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




export HOST_OS HOST_CPU TARGET ECHO_OPT TARGETS_DEFINED


# -------------------------------------------------------
# Do not execute if the targets have already been define
# -------------------------------------------------------
ifndef TARGETS_DEFINED

HOST_CPU := $(shell uname -m)
HOST_OS  := $(shell uname -s)

#$(info HOST CPU/OS = $(HOST_CPU), $(HOST_OS))


ifeq ($(HOST_OS),Linux)
   HOST_OS  := linux
   ECHO_OPT := -e
else
   ifeq ($(HOST_OS),Darwin)
       HOST_OS  := darwin
       ECHO_OPT :=  
    endif
endif

ifeq ($(HOST_CPU),armv7l)
   HOST_CPU  = arm_CA9
   ECHO_OPT := -e
endif


# ---------------------------------------------
# Define and verify the host/target combination
# ---------------------------------------------
ifeq ($(HOST_OS),darwin)
VALID_TARGET_SET := x86_64-gen-darwin          \
                    x86_64-avx-darwin          \
                    x86_64-avx2-darwin

DEFAULT_TARGET_SET := darwin


else
VALID_TARGET_SET := x86_32-linux               \
                    x86_64-gen-linux           \
                    x86_64-avx-linux           \
                    x86_64-avx2-linux linux    \
                    arm_CA9-linux arm_CA9-rtems

DEFAULT_TARGET_SET := linux

endif


#$(info TARGET = $(TARGET))
#$(info VALID_TARGET_SET = $(VALID_TARGET_SET))

ifndef TARGET
    TARGET := $(VALID_TARGET_SET)
else

   # ----------------------------
   # Check if have a valid target
   # ----------------------------
   ifeq ($(TARGET),DEFAULT_TARGET)
       TARGET := $(VALID_TARGET_SET)
   else

     ifeq ($(filter $(TARGET),$(VALID_TARGET_SET)),)
        $(info  Error: Invalid target, TARGET=<$(TARGET)> is not one of $(VALID_TARGET_SET))
        $(error Quitting)
     endif

   endif

endif

TARGETS_DEFINED := 1

endif


# --- Change fields of the target to whitespace delimited list
space :=
space += 
tmp   := $(subst -,$(space),$(TARGET))

TARGET_CPU := $(word 1,$(tmp))
TARGET_OPT := $(word 2,$(tmp))
TARGET_OS  := $(word 3,$(tmp))

