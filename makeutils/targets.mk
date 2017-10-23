# -*-Mode: makefile;-*-

# ======================================================================
#
# TARGET LOOP IMPLENTATION
# ------------------------
# This effectively implements the looping of the target. 
# Given the limited capabilities of make, this works in a somewhat
# awkward way.  Fortunately, almost all of this awkwardness is hidden
# within this file. The only user responsibility is to skip everything
# that comes after including this file if the variable CONTINUE is
# not defined.
#
# A single tag is peeled off of the command line variable TAG or TAGS
# (either is accepted) and the original Makefile is recursively called
# back with the CONTINUE variable set.  This continues until the list
# is exhausted.
# ======================================================================


# -----------------------------------------------------------------------
# See if target or targets (accept either) is present on the command line
# -----------------------------------------------------------------------
ifndef target
  ifdef targets
   TARGET = $(targets)
  endif
else
   TARGET = $(target)
endif

ALL_TARGET_X86_64 := x86_64-gen-linux \
                     x86_64-avx-linux \
                     x86_64-avx2-linux 


ifdef TARGET

   # -------------------------------------------------------------
   # If have specified to build for all x86 targets, expand to all
   # -------------------------------------------------------------
   TARGET := $(subst x86_64-linux,$(ALL_TARGET_X86_64),$(TARGET))

else

   # -----------------------------
   # Default to all X86_64 targets
   # -----------------------------
   TARGET := $(ALL_TARGET_X86_64)

endif


# ---------------------------------------------------
# Change comma separated list to space separated list
# ---------------------------------------------------
space  :=
space  += 
comma  :=,
TARGET := $(subst $(comma),$(space),$(TARGET))


ifneq ($(words $(TARGET)),1)

   makefile = $(firstword $(MAKEFILE_LIST))

   ifeq ($(MAKECMDGOALS),)
      # -- Nothing specified, default to all
      # -- This ensures that a per target rule gets generated
      goals := all
   else
      # -- Set recursed goal list to filter out the non-recursed goals
      goals := $(MAKECMDGOALS)
   endif


   # -------------------------------------------------------
   # -- Only trigger the per target makes for the first goal
   # -------------------------------------------------------
   first_goal = $(word 1,$(goals))
   $(word 1,$(goals)) :
	@echo -e "Building for these targets: $(TARGET) $^ \n"
	@$(foreach t,$(TARGET), $(MAKE) --no-print-directory -f $(makefile)\
           target=$(t) $(MAKECMDGOALS);)

   # -- Nop the remaining goals
   $(filter-out $(first_goal),$(goals)):
	@true

else
   # -- Only one target defined, just process it
   CONTINUE := 1
endif
