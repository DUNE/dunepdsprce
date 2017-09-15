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


# ---------------------------------
# -- armCA9-linux only constituents
# ---------------------------------
ifeq ($(TARGET), armCA9-linux)
   # -- None
endif
# ------------------------



# ---------------------------------
# -- x86_64-linux only constituents
# ---------------------000000------
ifeq ($(TARGET),x86_64-linux)

  # -- None

  reader_SRCDIR     := $(PKG_CC_ROOT)/ptd
  reader_CCSRCFILES := reader.cc
  reader__CPPFLAGS  := -g -mavx2
  reader_LDFLAGS    := $(PKG_BLD_LIBDIR)/libprotodune-dam.so
  reader_ALIAS      := reader
  EXECUTABLES       += reader

  wibFrame_test_SRCDIR     := $(PKG_CC_ROOT)/ptd
  wibFrame_test_CCSRCFILES := wibFrame_test.cc
  wibFrame_test__CPPFLAGS  := -g -mavx2
  wibFrame_test_LDFLAGS    := $(PKG_BLD_LIBDIR)/libprotodune-dam.so
  wibFrame_test_ALIAS      := wibFrame_test
  EXECUTABLES              += wibFrame_test

endif
# ------------------------


# ------------------------
# -- Common to all tags
# ------------------------




# -----------------------------------------------------------
# SHAREABLE: libprotodune-dam
# ---------------------------

libprotodune-dam_CCSRCFILES := Headers.cc             \
                               DataFragmentUnpack.cc  \
                               TpcFragmentUnpack.cc   \
                               TpcStreamUnpack.cc     \
                               TpcStream.cc           \
                               TpcRecords.cc          \
                               WibFrame.cc

libprotodune-dam__CCFLAGS   := -g
libprotodune-dam__CPPFLAGS  := -g
libprotodune-dam_ALIAS      := protodune-dam
libprotodune-dam_VERSION    := 0.0.0
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
