# -*-Mode: makefile;-*-


# ======================================================================
#
# ProtoDune Data Access Library Methods
# -------------------------------------
#
#
# ======================================================================



# ======================================================================
#
# HISTORY
# -------
#
#     DATE   WHO WHAT
# ---------- --- ----------------------------------------------------------- 
# 2017.12.19 jjr Bump version of libprotodune-dam to 0.0.4. This version 
#                accepts the interleaved ADC byte streams.
# 2017.11.30 jjr Bump version.  The code did not change, but there was a
#                correction to the makeutil/define_tool.mk to correctly
#                handle an empty string (which was only on Mac builds).
# 2017.07.27 jjr Created
#
# ======================================================================





# ======================================================================
# BOILERPLATE
# -----------------------------------------------------------
# Extract the project root from the make file stem
# This assumes the directory structure is
#
#    <prj-stem>/source/<branch>/<package-name>/make/Makefile
# -----------------------------------------------------------
MAKEUTILS_DIR := $(shell                                                          \
                   cd $(dir $(word 1, $(MAKEFILE_LIST)))../../../../makeutils; \
                   pwd)
include $(MAKEUTILS_DIR)/prologue.mk

# --------------------------
#    Check if should process
# --------------------------
ifdef CONTINUE
# ======================================================================




# ======================================================================
# --- USER DEFINED STUFF
# ----------------------------------------------------------------------


# -----------------------------------
#   Project name, informational only
# -----------------------------------
PRJNAME  := protodune-dam-lib
# -----------------------------------


# -----------------------------------------------
#   Global include path
#   This path will be used for every compilation
#   While a common thing to do, it is up to the
#   user to 'opt-in'
# -----------------------------------------------
INCLUDES := $(INCLUDE_DIR)
# -----------------------------------------------

# ------------------
# ADD optional flags
# ------------------
optflags := $(OPT)
ifeq ($(optflags),)

     # ------------------------------------
     # If production build, do optimization
     # ------------------------------------
     ifneq ($(PROD),)
       optflags = -O2
      endif

endif


CXXFLAGS += $(optflags)
CFLAGS   += $(optflags)


# ---------------------------------
# -- armCA9-linux only constituents
# ---------------------------------
ifeq ($(TARGET), armCA9-linux)
   # -- None
endif
# ------------------------




#$(info Makefile::TARGET       = $(TARGET))
#$(info Makefile::TARGET_CPU   = $(TARGET_CPU))
#$(info Makefile::TARGET_OS    = $(TARGET_OS))
#$(info Makefile::TARGET_OPT   = $(TARGET_OPT))
#$(info Makefile::MAKECMDGOALS  = $(MAKECMDGOALS))

# -----------------------------------------
# -- x86_64 specific constituents
# -----------------------------------------
ifeq ($(TARGET_CPU),x86_64)

  ifeq ($(TARGET_OS),linux)
      dam-lib := $(PKG_BLD_LIBDIR)/libprotodune-dam.so
  else
      dam-lib := $(PKG_BLD_LIBDIR)/libprotodune-dam.dylib
  endif

  # -- Only generic stuff

  PdReaderTest_SRCDIR       := $(PKG_CC_ROOT)/ptd
  PdReaderTest_CCSRCFILES   := PdReaderTest.cc
  PdReaderTest__CPPFLAGS    := -g
  PdReaderTest_LDFLAGS      := $(dam-lib)
  PdReaderTest_ALIAS        := PdReaderTest
  EXECUTABLES               += PdReaderTest

  PdWibFrameTest_SRCDIR     := $(PKG_CC_ROOT)/ptd
  PdWibFrameTest_CCSRCFILES := PdWibFrameTest.cc
  PdWibFrameTest__CPPFLAGS  := -g
  PdWibFrameTest_LDFLAGS    := $(dam-lib)
  PdWibFrameTest_ALIAS      := PdWibFrameTest
  EXECUTABLES               += PdWibFrameTest


  PdWibFrameExtract_SRCDIR     := $(PKG_CC_ROOT)/ptd
  PdWibFrameExtract_CCSRCFILES := PdWibFrameExtract.cc
  PdWibFrameExtract__CPPFLAGS  := -g
  PdWibFrameExtract_LDFLAGS    := $(dam-lib)
  PdWibFrameExtract_ALIAS      := PdWibFrameExtract
  EXECUTABLES                  += PdWibFrameExtract


#  capabilities_SRCDIR      := $(PKG_CC_ROOT)/src
#  capabilities_CSRCFILES   := capabilities.c
#  capabilities_ALIAS       := capabilities
#  EXECUTABLES              += capabilities

#endif
endif

# ---------------------------
# SHAREABLE: libprotodune-dam
# ---------------------------

libprotodune-dam_CCSRCFILES := Headers.cc             \
                               DataFragmentUnpack.cc  \
                               TpcFragmentUnpack.cc   \
                               TpcStreamUnpack.cc     \
                               DataFragment.cc        \
                               Identifier.cc          \
                               Originator.cc          \
                               Data.cc                \
                               TpcFragment.cc         \
                               TpcStream.cc           \
                               TpcRanges.cc           \
                               TpcToc.cc              \
                               TpcPacket.cc           \
                               TpcCompressed.cc       \
                               AP-Decode.cc           \
                               WibFrame.cc

libprotodune-dam__CCFLAGS   := -g
libprotodune-dam__CXXFLAGS  := -g
libprotodune-dam_ALIAS      := protodune-dam
libprotodune-dam_VERSION    := 1.0.2
SHAREABLES                  += libprotodune-dam

# ----------------------------------------------------------
#
#
# ====================================================================== *



# ======================================================================
# BOILERPLATE
# ---------------------------------------------------------------------
include $(MAKEUTILS_DIR)/epilogue.mk
# ======================================================================
endif
