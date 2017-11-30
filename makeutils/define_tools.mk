ifeq ($(HOST_OS),linux)
else
   ifeq ($(HOST_OS),darwin)
   else
       $(error ERROR:Can only compile on linux/darwin OSs, not $(HOST_OS))
   endif
endif


# ---------------------------------------------------------------
# SO_LDFLAGS
# 
# Defines the linker flags needed/used to build a shareable image
# ---------------------------------------------------------------
ifeq ($(HOST_OS),linux)
   SO_LDFLAGS :=  -shared                                         \
                  -Wl,--hash-style=gnu                            \
                  -Wl,--no-undefined                              \
                  -Wl,--allow-shlib-undefined                     \
                  -Wl,--unresolved-symbols=ignore-in-shared-libs
else
ifeq ($(HOST_OS),darwin)
   SO_LDFLAGS :=  -dynamiclib
endif
endif
# ---------------------------------------------------------------


ifeq ($(HOST_CPU),arm_CA9)

   # -----------------------------------
   # Building on the embedded target
   # -----------------------------------
   ifneq ($(TARGET_CPU),armCA9)

        # -----------------------------------
        # Target can only be the host machine
        # -----------------------------------
        $(error "Cannot cross build for <$(HOST)> for target <$(TARGET)>")

   endif

   #---------------------------------------------------------------
   # Tools for the embedded environment on the embedded environment
   #---------------------------------------------------------------
   AS  := as
   CC  := g++
   CXX := g++
   LD  := g++


else 

   ifeq ($(TARGET_CPU),arm_CA9)

      # -----------------------
      # Cross development tools
      # -----------------------
      ifeq ($(TARGET_OS),linux)

        CC  := arm-linux-gnueabihf-gcc
        CXX := arm-linux-gnueabihf-g++
        LD  := arm-linux-gnueabihf-g++
        AS  := arm-linux-gnueabihf-as

	CCFLAGS += -std=c++0x

      else

        AS  := arm-rtems4.11-as
        CC  := arm-rtems4.11-gcc
        CXX := arm-rtems4.11-g++
        LD  := $(CXX)

	link_script := $(RCE_RTEMS_SDK)/lib/shareable.ld
	task_stub   := $(RCE_RTEMS_SDK)/lib/libtaskstub.a
        LDFLAGS     := -Wl,--hash-style=gnu                            \
                       -Wl,--no-undefined                              \
                       -Wl,--allow-shlib-undefined                     \
                       -Wl,--unresolved-symbols=ignore-in-shared-libs  \
                        -shared -nostdlib -nostartfiles                \
                        -e Task_Entry                                  \
                        -Wl,-z,max-page-size=4096                      \
                        -Wl,-T $(link_script) \
                        $(task_stub)


      endif

   else

      # ------------
      # Native tools
      # ------------
      CC  ?= g++
      CXX ?= g++
      LD  ?= $(CXX)
      AS  ?= as

   endif

endif


ifeq ($(TARGET_CPU),arm_CA9)

   arm_stdflags  =  -Wall                          \
                    -Wno-psabi                     \
                    -fno-zero-initialized-in-bss   \
                    -march=armv7-a                 \
                    -mtune=cortex-a9               \
                    -mcpu=cortex-a9                \
                    -mfpu=neon                     \


   CCDEFINES    := -DARM -D __ARM_PCS_VFP 
   CPPFLAGS     := 

   CPPFLAGS_DEF := 
   CFLAGS       := -Wall -Werror $(arm_stdflags) -g -std=gnu99
   CXXFLAGS     := -Wall -Werror $(arm_stdflags) -g -std=c++0x

   ifeq ($(TARGET_OS),rtems)
        CFLAGS    += -fPIC
        XXXCFLAGS += -fPIC
   endif


else

   ifeq ($(TARGET_CPU),x86_64)

      CCDEFINES   :=
      CPPFLAGS    := 
      CFLAGS      := -Wall -Werror -std=gnu99
      CXXFLAGS    := -Wall -Werror -pedantic -std=c++0x


     ifeq ($(TARGET_OPT),avx2)
        CFLAGS   += -march=core-avx2 -mtune=core-avx2
        CXXFLAGS += -march=core-avx2 -mtune=core-avx2
     else
       ifeq ($(TARGET_OPT),avx)
          CFLAGS   += -march=core-avx-i -mtune=core-avx-i -mavx
          CXXFLAGS += -march=core-avx-i -mtune=core-avx-i -mavx
       endif
     endif


    # ---------------------------------------------------------
    #
    # !!! - KLUDGE - !!!
    # 2017.11.02 --- jjr 
    # ------------------
    #
    # The clang compiler on the MAC has stricter error checking
    # than the stock gcc 6.3 compiler. In one particular case 
    # the 'fix' is not compatiable with the standard gcc 
    # compilers.  This is a work around.
    #
    # Not happy with this, it is way too ad hoc.  It also moves
    # knowledge to this makefile making it difficult for others
    # to use their own build system. 
    #
    # This particular fix addresses unused class members. 
    # These are used to pad and reserve space in classes. 
    # clang flags these as errors. Adding an 
    #
    #    __attribute__((unused))
    #
    # tells clang to ignore the error.  Unfortunately straight
    # gcc does not allow this.  (I tried to make the -D
    # to be the above string, but trying to get parens through
    # the make rules was way too daunting, so compromised and
    # just passed in a 'unused' or blank string.   
    # ---------------------------------------------------------
    ifeq ($(TARGET_OS),darwin)

       # ----------------------------------------
       # -- Check if this is the clang compiler
       #    Note that the versioning is on stderr
       # ----------------------------------------
       cmp_version := $(shell $(CXX) -v 2>&1 |  grep clang)

	// ---------------------------------------------------
	// 2017.11.30 -- jjr
	// Change the test for an empty string from ,"") -< ,)
        // per Tom Junks request. I guess this is the correct
        // way to test for an empty string.
        // ---------------------------------------------------
       ifneq ($(cmp_version),)

          # ------------------------------------------------
          # This is the clang compiler, define the attribute
          # ------------------------------------------------
          CPPFLAGS += -DCLASS_MEMBER_UNUSED=unused

       else
          CPPFLAGS += -DCLASS_MEMBER_UNUSED= 
       endif
    else
          CPPFLAGS += -DCLASS_MEMBER_UNUSED= 
    endif

  else

     ifeq ($(TARGET_CPU),x86_32)
       $(error ERROR: Building for x86_32 targets not yet supported)
     else
       $(error ERROR: Unknown target = $(TARGET))
     endif

  endif

endif
