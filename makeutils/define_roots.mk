
#
# Directory structure is
#
# <stem>/<pkg-name)/<branch>/source/
#                                   cc/
#                                     make/
#                                     <pkg-name>/
#                                      src/
#                           /binary/<pkg-name>/dep
#                                             /obj
#                           /export/<pkg-name>/lib
#                           /export/<pkg-name>/bin
#


PKG_SOURCE_ROOT := $(shell cd $(dir $(word 1, $(MAKEFILE_LIST)))/../..; pwd)

PKG_BASE_DIR    := $(shell cd $(PKG_SOURCE_ROOT)/..; pwd)
PKG_BRANCH      := $(notdir $(PKG_BASE_DIR))
PKG_NAME        := $(notdir $(shell cd $(PKG_BASE_DIR)/; pwd))
PKG_BINARY_ROOT := $(PKG_BASE_DIR)/binary/$(TARGET)
PKG_CC_ROOT     := $(PKG_SOURCE_ROOT)/cc
PKG_INCDIR      := $(PKG_SOURCE_ROOT)/cc
PKG_SRCDIR      := $(PKG_SOURCE_ROOT)/cc/src
PKG_MAKEDIR     := $(PKG_SOURCE_ROOT)/cc/make
PKG_EXPORT_ROOT := $(PKG_BASE_DIR)/export/$(TARGET)
PKG_EXP_LIBDIR  := $(PKG_EXPORT_ROOT)/lib
PKG_EXP_BINDIR  := $(PKG_EXPORT_ROOT)/bin


PKG_ROOT          := $(shell cd $(PKG_BASE_DIR)/..; pwd)
PRJ_BUILD_ROOT    := $(PKG_ROOT)/install/$(TARGET)

INCLUDE_DIR       := $(PRJ_BUILD_ROOT)/include
PKG_BLD_INCDIR    := $(INCLUDE_DIR)/$(PKG_NAME)
PKG_BLD_BINDIR    := $(PRJ_BUILD_ROOT)/bin
PKG_BLD_LIBDIR    := $(PRJ_BUILD_ROOT)/lib
PKG_BLD_INCTARGET := $(PKG_BLD_INCDIR)/target


ifdef   PRINT_ROOTS
  $(info PACKAGE           ROOT: $(PKG_ROOT))
  $(info PACKAGE           NAME: $(PKG_NAME))
  $(info PACKAGE         BRANCH: $(PKG_BRANCH))
  $(info PACKAGE BASE       DIR: $(PKG_BASE_DIR))
  $(info PACKAGE MAKE       DIR: $(PKG_MAKEDIR))
  $(info PACKAGE SOURCE    ROOT: $(PKG_SOURCE_ROOT))
  $(info PACKAGE BINARY    ROOT: $(PKG_BINARY_ROOT))
  $(info PACKAGE EXPORT    ROOT: $(PKG_EXPORT_ROOT))
  $(info PACKAGE SRC        DIR: $(PKG_SRCDIR))
  $(info PACKAGE INC        DIR: $(PKG_INCDIR))
  $(info BUILD             ROOT: $(PRJ_BUILD_ROOT))
  $(info BUILD   INC        DIR: $(PKG_BLD_INCDIR))
  $(info BUILD   INC TARGET DIR: $(PKG_BLD_INCTARGET))
  $(info BUILD   BINARY     DIR: $(PKG_BLD_BINDIR))
  $(info BUILD   LIBRARY    DIR: $(PKG_BLD_LIBDIR))
endif


ifdef BUILD

# -----------------------------------------------
# Return a reference to the list of specified SOs 
# using their names as defined by the Makefile
# -----------------------------------------------
define use_pkg_so
$(foreach x,$1,$($x_SO))
endef

# -----------------------------------------------
# Return a reference to the list of specified SOs 
# by constructing its path in the export library.
# -----------------------------------------------
define use_exp_so
$(foreach x,$1,$(PKG_EXP_LIBDIR)/$x)
endef


# -----------------------------------------------
# Return a reference to the list of specified SOs 
# by constructing its path in the build library.
# -----------------------------------------------
define use_bld_so
$(foreach x,$1,$(PRJ_BUILD_ROOT)/$x)
endef


# -----------------------------------------------
# Return a reference to the list of specified SOs 
# by constructing its path in the an external dir
# -----------------------------------------------
define use_ext_so
$(foreach x,$2,$1/$x)
endef


# -----------------------------------------------
# Return a reference to the list of specified ROs 
# using their names as defined by the Makefile
#
# Since ROs are not exportable, there is only the
# internal name.
# -----------------------------------------------

define use_ro
$(foreach x,$1,$($x_RO))
endef


endif
