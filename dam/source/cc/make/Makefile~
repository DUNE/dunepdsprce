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

# ------------------------------------------
# -- x86_64-linux generic only constituents
# ---------------------000000---------------
ifeq ($(TARGET_CPU),x86_64)
#ifeq ($(TARGET_OPT),gen)

  # -- Only generic stuff

  reader_SRCDIR            := $(PKG_CC_ROOT)/ptd
  reader_CCSRCFILES        := reader.cc
  reader__CPPFLAGS         := -g
  reader_LDFLAGS           := $(PKG_BLD_LIBDIR)/libprotodune-dam.so
  reader_ALIAS             := reader
  EXECUTABLES              += reader

  wibFrame_test_SRCDIR     := $(PKG_CC_ROOT)/ptd
  wibFrame_test_CCSRCFILES := wibFrame_test.cc
  wibFrame_test__CPPFLAGS  := -g
  wibFrame_test_LDFLAGS    := $(PKG_BLD_LIBDIR)/libprotodune-dam.so
  wibFrame_test_ALIAS      := wibFrame_test
  EXECUTABLES              += wibFrame_test


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
                               WibFrame.cc

libprotodune-dam__CCFLAGS   := -g
libprotodune-dam__CPPFLAGS  := -g
libprotodune-dam_ALIAS      := protodune-dam
libprotodune-dam_VERSION    := 0.0.1
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
