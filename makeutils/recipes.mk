# -*-Mode: makefile;-*-

# ======================================================================
#
#  THE PROBLEM
#  -----------
#  The original makefile had problems with the -include *.d directive.
#  This directive implicitly caused all the *.d targets to be updated.
#  Normally, at worst, for example, in the case of executing a clean
#  target, this is just a waste of time. However in the case that the
#  the .d cannot be remade, say if you have pulled a new version from
#  the repository and the new version no longer includes one of the 
#  source files, the make file will abort execution because it cannot 
#  remake the .d.  One could go in and by hand do the clean, but this
#  sort of defeats the purpose of having a clean target. It should hide
#  these sort of details.
#
#  THE STANDARD SOLUTION
#  ---------------------
#  As one might expect, this is not the first time this problem has been
#  encountered and there is a recommended solution which uses the make
#  variable MAKECMDGOALS This variable gives the list of goals as
#  preented on the command line. Here is the suggested solution
#
#  ifneq ($(MAKECMDGOALS), clean)
#   -include *.d
#  endif
#
#  This works like a champ if one types
#
#  $ gmake clean
#
#  The -include is not executed, thus avoiding the problem. However if
#  one updates a package to a new release, one of the more common make
#  commands might be
#
#  $ gmake clean all
#
#  Cleaning up the old stuff, then rebuilding everything. In this case
#  MAKECMDGOALS is now "clean all" and this does not match "clean", so
#  the protection afforded by the if is lost.
#
#  THE SOLUTION
#  ------------
#  The chosen solution is to recurse the make iff the target is not
#  clean and, only in the recursed make do the -include *.d. The clean
#  target was changed from deleting each individual file in the 
#  dependency directory to just deleting the contents of the dependency
#  directory. Personally, I think this is a cleaner solution, lessening
#  the effects of the files in a new release being a different set than
#  the previous release.
#
#  The implementation uses the internal BUILD phase variable to distinguish
#  whether one is in the top level or recursed make. This tactic means
#  only one makefile is needed. Thus this makefile is broken into three
#  sections
#        1. The stuff that is common to the top level execution
#           and the recursed execution
#        2. The stuff specific to the top level execution
#        3. The stuff specific to the recursed executions
#
# ----------------------------------------------------------------------
#
#
# HOW THE CLEAN IS DONE
# ---------------------
# There are two ways to do the clean
#  1) Get a list of all the make products and remove them one at a time
#  2) Remove everything in the product's directories
#
# I've opted for the #2. Many times the Makefile's list of products 
# changes. If a new one is added either method is fine. However if a
# product is deleted, it and its remenants products do not get removed.
# 
# The downside of this is that with only directories and their contents
# being removed, one must have implicit knowledge of these directories.
# Given the semi-formal nature of the directory structure, this is well
# defined and well contained (i.e there are only a few, so generating
# the list is relatively easy.
#
# ======================================================================





# ======================================================================
#
# SECTION: Common to all levels
#
# ----------------------------------------------------------------------


# ----------------------
# Disable implicit rules
# ----------------------
.SUFFIXES:
#-----------------------



# ----------------------------------------------------------------------
#
# VERSION CHECK
# -------------
#
# The driving requirement is the 'eval' function which was not supported
# until version 3.81 of make.  
#
# NOTE:
# Many examples on the WEB use an '=' when using 'define'. This is buggy
# in 3.81. It was fixed in 3.82.  Since using 'define' without the '='
# means the same thing, this file avoids it usage, sacrificing a bit of
# clearity to use an older version of make.
# ----------------------------------------------------------------------
MIN_VERSION := 3.81
VERSION_OK  := $(filter $(MIN_VERSION), \
               $(firstword $(sort $(MAKE_VERSION) $(MIN_VERSION))))
# -----------------------------------------------------------------



# -----------------------------------------------------------
# VERSION_OK will be blank if this version of make is too old
# otherwise  will be MIN_VERSION is okay
# -----------------------------------------------------------
ifeq ($(VERSION_OK),)
   space :=
   space += 
   spc   := $(space)$(space)$(space)$(space)$(space)$(space)
   $(info ERROR: Make version $(MAKE_VERSION) is insufficent)
   $(info $(spc) Minimum required version is $(MIN_VERSION))
   $(error Aborting..)
endif
# ----------------------------------------------------------------------



binaryfile = $(subst $(PKG_BINARY_ROOT)/,,$(1))
exportfile = $(subst $(PKG_EXPORT_ROOT)/,,$(1))
buildfile  = $(subst $(PRJ_BUILD_ROOT)/,,$(1))

binarylist = $(subst $(space),\n                                  . ,$(foreach dir,$(1),$(call binaryfile,$(dir))))
exportlist = $(subst $(space),\n                                  . ,$(foreach dir,$(1),$(call exportfile,$(dir))))
buildlist  = $(subst $(space),\n                                  . ,$(foreach dir,$(1),$(call buildfile,$(dir))))


# ======================================================================





# ======================================================================
# SECTION: Definition Phase, i.e. not BUILDING
# --------------------------------------------
#
# These are definitions needed to implement the targets (currently
# clean and help that get executed during non BUILD phase. Some of these
# definitions are also needed in the BUILD phase 1, so they are exported.
#
# Only those defintions needed for non-BUILD phase targets should be 
# defined here.
#
# ----------------------------------------------------------------------
ifndef BUILD

.PHONY: all clean help

.DEFAULT_GOAL := all

space  := 
space  +=
comma  := ,
extract_1 = $(patsubst $1:%,%,$(filter-out $(patsubst $1:%,,$2),$2))
extract   = $(subst $(comma),$(space),$(call extract_1,all,$2) $(call extract_1,$1,$2))


# --------------------------------------------
# Extract the name of the original 
# make file so that it can be re-invoked
# ---------------------------------------
makefile      = $(firstword $(MAKEFILE_LIST))
# --------------------------------------------


# ------------------------------------------------------------
# Convenience methods to print the constituent directory lists
# ------------------------------------------------------------
pkg_blddir            = $(subst $(PRJ_BUILD_ROOT)/,,$(1))
pkg_depdirs          := $(wildcard $(PKG_BINARY_ROOT)/*/dep)
pkg_objdirs          := $(wildcard $(PKG_BINARY_ROOT)/*/obj)
pkg_depdirs_list     := $(call binarylist,$(pkg_depdirs))
pkg_objdirs_list     := $(call binarylist,$(pkg_objdirs))
# ------------------------------------------------------------



# --------------------------------------------------
# -- If return to constituent qualified lib and bin 
# -- Then -> $(PKG_EXPORT_ROOT)/*/(lib,bin)
# --------------------------------------------------
pkg_libdirs      := $(PKG_EXPORT_ROOT)/lib
pkg_bindirs      := $(PKG_EXPORT_ROOT)/bin
pkg_libdirs_list := $(call exportlist,$(pkg_libdirs))
pkg_bindirs_list := $(call exportlist,$(pkg_bindirs))
# ------------------------------------------------------------



# ------------------------------------------------------------
# Taylor the options for phase (BUILD or non-BUILD)
MAKEOPTIONS  = --no-print-directory
# ------------------------------------------------------------


# -------------------------------------------------------------------
# Taylor the differed goals
# -------------------------
ifeq ($(MAKECMDGOALS),)
   # -- Nothing specified, default to all
   deferred_goals := all
else
   # -- Set recursed goal list to filter out the non-recursed goals
   deferred_goals := $(filter-out clean help,$(MAKECMDGOALS))
endif
# -------------------------------------------------------------------



# ---------------------------------------------------------------------------
# Only trigger the recursed make for the first goal
# -------------------------------------------------
first_goal := $(word 1,$(deferred_goals))
$(first_goal): 
	@$(MAKE) -r -f $(makefile) $(MAKEOPTIONS) BUILD=1 $(deferred_goals)
# ---------------------------------------------------------------------------


# ------------------------------------------------------------------------
# -- Have to neuter the remaining goals to prevent noisy print out
# -- I've tried 
#      - Not putting in anything  -> generates target is up-to-date
#      - Using a empty recipe (;) -> generates Nothing to be done
#
# -- Settled on just returning 'true' as the recipe.
#      - This is crude since it demands a shell to be spawned.
#        but taking this as a better alternative than a misleading message
# ------------------------------------------------------------------------
$(filter-out $(first_goal),$(deferred_goals)):
	@true


#copy_includes:
#	@$(MAKE) -f $(MAKEFILE_LIST) $(MAKEOPTIONS) $@

#make_directories:
#	@$(MAKE) -f $(MAKEFILE_LIST) $(MAKEOPTIONS) $@
# ------------------------------------------------------------



# ------------------------------------------------------------
# TARGET: help
#
# Provides rudimentary explanation of the various targets that
# are standardly available
# ------------------------------------------------------------
help:
	@echo $(ECHO_OPT)                                                     \
        "Targets are"                                                         \
      "\n"                                                                    \
      "\n    all........................ Build all targets"                   \
      "\n    clean...................... Clean all targets"                   \
      "\n"                                                                    \
      "\n    make_directories........... Creates the output directories"      \
      "\n    copy_includes...............Copies the public include files"     \
      "\n"                                                                    \
      "\n    help....................... Print all targets"                   \
      "\n    print_tools................ Print the compiler/load tools used"  \
      "\n    print_flags................ Print the various flags"             \
      "\n    print_dependencyfiles...... Print the list of dependency files"  \
      "\n    print_directories.......... Print the input & output directories"\
      "\n"


# ------------------------------------------------------------
# TARGET: clean
#
# Removes the produced files by removing the contents of the
# directories they live in. This contrasts with removing the
# products that the driving Makefile would produce. The 
# problem with this latter approach is that if a product is
# removed from the driving Makefile, it is no longer known
# and remanents of it can remain in these directories.
#
# The exception to this rule is the build directories. 
# Because the products from various packages are mixed 
# (effectively the package structure is flattened), there is
# no package speccific directory whose contents can be 
# targetted for removal. The tact taken here is to take
# advantabe of the fact that these are symbolic links which
# point back to package's export directory.  Using this a
# list of all the files that point back to this package's
# directory is used as the removal targets.
#
# Note that the directories themselves are not removed.
# ------------------------------------------------------------


# ---------------------------------------------------
# Since this may be a relatively expensive operation,
# it is only done if necessary.
# ---------------------------------------------------
ifeq ($(filter $(MAKECMDGOALS),clean),clean)

  # -----------------------------------------------------------------------
  # 
  # Procedure
  #   clean_links  <target_directory>,<parent_directory>
  #
  #   This is just a convenience procedure so to get the real path
  #   of the parent directory is a form that can be used for comparison.
  #   It is also somewhat of an optimization, avoiding doing this 
  #   evaluation for every file.
  #
  #   clean_links_ <target_directory>,<parent_direcoory-real-path>
  #
  #   This is workhorse procedure.
  #   The following obtuse line can be broken as
  #
  #     wildcard     $1/*   find all the files in the target directory
  #     dir $(realpath $f)  get the actual directory of the file
  #     if (filter a,$2)     check if actual == parent directory
  #     $f,)                if so, add file to the list, otherwise skip
  # -----------------------------------------------------------------------
  clean_links  = $(call clean_links_,$1,$(realpath $2)/)
  clean_links_ = $(foreach f,$(wildcard $1/*),\
                 $(if $(filter $(dir $(realpath $f)),$2),$f,))
  files_bin := $(call clean_links,$(PKG_BLD_BINDIR),$(PKG_EXP_BINDIR))
  files_lib := $(call clean_links,$(PKG_BLD_LIBDIR),$(PKG_EXP_LIBDIR))
  #$(info files_bin = $(files_bin))
  #$(info files_lib = $(files_lib))

endif

clean:
	@echo $(ECHO_OPT) "Start clean of package ........... $(PKG_NAME) - $(TARGET)\n" \
	                  "   Clean files in binary dep dir.. $(pkg_depdirs_list)"
	@rm -rf $(addsuffix /*,$(pkg_depdirs))
	@echo $(ECHO_OPT) "    Clean files in binary obj dir.. $(pkg_objdirs_list)"
	@rm -rf $(addsuffix /*,$(pkg_objdirs))
	@echo $(ECHO_OPT) "    Clean files in export lib dir.. $(pkg_libdirs_list)"
	@rm -rf $(addsuffix /*,$(pkg_libdirs))
	@echo $(ECHO_OPT) "    Clean files in export bin dir.. $(pkg_bindirs_list)"
	@rm -rf $(addsuffix /*,$(pkg_bindirs))
	@echo $(ECHO_OPT) "    Clean files in build  inc dir.. $(call pkg_blddir,$(PKG_BLD_INCDIR))"
	@rm -rf $(addsuffix /*,$(PKG_BLD_INCDIR))
	@echo $(ECHO_OPT) "    Clean files in build  lib dir.. $(call buildlist,$(call pkg_blddir,$(files_lib)))"
	@rm -rf $(files_lib)
	@echo $(ECHO_OPT) "    Clean files in build  bin dir.. $(call buildlist,$(call pkg_blddir,$(files_bin)))"
	@rm -rf $(file_bin)
	@echo $(ECHO_OPT) "End   clean of project ........... $(PKG_NAME) - $(TARGET)\n"
# ------------------------------------------------------------



# ======================================================================
else
# ======================================================================



# ----------------------------------------------------------------------
# SECTION: BUILD PHASE
# --------------------
#
# This is where the real action happens.  Much of this is 
# defining various lists of files and target and the canned
# recipes that do the actual building for the products.
#
# The current list of products are
#
# WHAT             USES                              EXTENSION
# ---------------- ----------------------------      ---------
# Static  Objects  Executables,static libraries             .o
# Library Objects  Shared libraries (PIC code)             .lo
# Shared  Objects  Link with other .so's and .exe's        .so
# Executables      Runnable image                       <none>
# Dependencies     C and C++ include dependencies          .d
# Build products   These invert the package tree
#                  structure, organizing by product
#                  type, not by package
#
# I've tries to use standard UNIX file extensions.
# 
# These are generally wrapped in a foreach loop construct
# which uses the 'eval' function to define the pattern rules
# for a specific pconstituent's products.
#
# ----------------------------------------------------------------------
DBG_DEP_ASM  := $(if $(strip $(dep_asm)  $(dep)  $(dbg)),,@)
DBG_DEP_C    := $(if $(strip $(dep_asm)  $(dep)  $(dbg)),,@)
DBG_DEP_CC   := $(if $(strip $(dep_asm)  $(dep)  $(dbg)),,@)
DBG_CMP_ASM  := $(if $(strip $(cmp_asm)  $(cmp)  $(dbg)),,@)
DBG_CMP_C    := $(if $(strip $(cmp_c)    $(cmp)  $(dbg)),,@)
DBG_CMP_CC   := $(if $(strip $(cmp_cc)   $(cmp)  $(dbg)),,@)
DBG_LINK_RO  := $(if $(strip $(link_ro)  $(link) $(dbg)),,@)
DBG_LINK_SO  := $(if $(strip $(link_so)  $(link) $(dbg)),,@)
DBG_LINK_EXE := $(if $(strip $(link_exe) $(link) $(dbg)),,@)


CONSTITUENTS := $(EXECUTABLES) $(RELOCATABLES) $(SHAREABLES)
export CONSTITUENTS

pkgfile     = $(subst $(PKG_SOURCE_ROOT)/,"",$(1))
objfile     = $(patsubst %.d,%.o, $(subst /dep/,/obj/,$(1)))
includefile = $(subst $(PRJ_BUILD_ROOT)/include/,,$(1))

derive = $(addprefix $(1)/,                            \
         $(addsuffix $(3),                             \
         $(basename $(notdir $(2)))))


# ------------------------------------------------------------------
# Compose the build roots and subdirectories for each constituent
# ------------------------------------------------------------------
$(foreach c,$(CONSTITUENTS),$(eval $c_BINARYROOT := $(PKG_BINARY_ROOT)/$c))
$(foreach c,$(CONSTITUENTS),$(eval $c_DEPDIR     := $($c_BINARYROOT)/dep))
$(foreach c,$(CONSTITUENTS),$(eval $c_OBJDIR     := $($c_BINARYROOT)/obj))


$(foreach c,$(CONSTITUENTS),$(eval $c_EXPORTROOT := $(PKG_EXPORT_ROOT)))
$(foreach c,$(CONSTITUENTS),$(eval $c_LIBDIR     := $($c_EXPORTROOT)/lib))
$(foreach c,$(CONSTITUENTS),$(eval $c_BINDIR     := $($c_EXPORTROOT)/bin))
# ------------------------------------------------------------------


# ------------------------------------------------------------
# Compose the list of all constituent roots and subdirectories
# ------------------------------------------------------------
PKG_BINARYROOTS := $(foreach c,$(CONSTITUENTS),$($c_BINARYROOT))
PKG_DEPDIRS     := $(foreach c,$(CONSTITUENTS),$($c_DEPDIR))
PKG_OBJDIRS     := $(foreach c,$(CONSTITUENTS),$($c_OBJDIR))
PKG_EXPORTROOTS := $(foreach c,$(CONSTITUENTS),$($c_EXPORTROOT))
PKG_LIBDIRS     := $(foreach c,$(CONSTITUENTS),$($c_LIBDIR))
PKG_BINDIRS     := $(foreach c,$(CONSTITUENTS),$($c_BINDIR))
# ------------------------------------------------------------


# ----------------------------------------
# Define the names of output products
# This is not too important for executables
# But, relocatables and shareables can be
# referenced as part of the builds
# ----------------------------------------
$(foreach x,$(EXECUTABLES), $(eval $x_EXE := $($x_BINDIR)/$x))
$(foreach r,$(RELOCATABLES),$(eval $r_RO  := $($r_OBJDIR)/$r.ro))


# ------------------------------------------------------------------
# There are 3 output products for a shareable
#   lib/libX.so.M.m.u  - The name of the shareable object
#   lib/libX.so.M.m    - A symbolic link that points to the so
#   lib/libX.so.M      - A symbolic link that points to the shareable
#
#   
#   libX.so.M          - SONAME used in the link
#
# Typically he SONAME is qualified by the major version number,
# because only shareables with the same major version number are 
# backwardly compatible.
# ------------------------------------------------------------------
space := 
space += 


ifeq ($(TARGET_OS),linux)
# -------------------------------------------------------------------------
#
# Assume building shareable xx, version 1.2.3 in directory /lib/
#
# xx_BASENAME = xx.so               -- Temporary
# xx_Mmp      = 1 2 3               -- Space delimited version, temporary
# xx_SONAME   = xx.so.1             -- SONAME used in linking the shareable
# xx_SO       = /lib/xx.so          -- Symbolic link -> /lib/libxx.so.1.2.3
# xx_SO_M     = /lib/xx.so.1        -- Symbolic link -> /lib/libxx.so.1.2.3
# xx_SO_Mm    = /lib/xx.so.1.2      -- Symbolic link -> /lib/libxx.so.1.2.3
# xx_SO_Mmp   = /lib/xx.so.1.2.3    -- Actual shareable image
# -------------------------------------------------------------------------
$(foreach s,$(SHAREABLES), $(eval $s_BASENAME := $s.so))
$(foreach s,$(SHAREABLES), $(eval $s_Mmp      := $(subst .,$(space),$($s_VERSION))))
$(foreach s,$(SHAREABLES), $(eval $s_SONAME   := $($s_BASENAME).$(word 1,$($s_Mmp))))
$(foreach s,$(SHAREABLES), $(eval $s_SO       := $($s_LIBDIR)/$($s_BASENAME)))
$(foreach s,$(SHAREABLES), $(eval $s_SO_M     := $($s_LIBDIR)/$($s_SONAME)))
$(foreach s,$(SHAREABLES), $(eval $s_SO_Mm    := $($s_SO_M).$(word 2,$($s_Mmp))))
$(foreach s,$(SHAREABLES), $(eval $s_SO_Mmp   := $($s_SO_Mm).$(word 3,$($s_Mmp))))

$(foreach s,$(SHAREABLES), $(eval $s_BLD_SO     := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                   $(notdir $($s_SO)))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_SO_M   := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                   $(notdir $($s_SO_M)))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_SO_Mm  := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                   $(notdir $($s_SO_Mm)))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_SO_Mmp := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                   $(notdir $($s_SO_Mmp)))))

$(foreach x,$(EXECUTABLES), $(eval $x_BLD_EXE   := $(addprefix $(PKG_BLD_BINDIR)/,\
                                                   $(notdir $($x_EXE)))))

PKG_EXP_SOS  := $(foreach s,$(SHAREABLES),  $($s_SO_Mmp) $($s_SO) \
                                            $($s_SO_M) $($s_SO_Mm))

PKG_BLD_SOS  := $(foreach s,$(SHAREABLES), $($s_BLD_SO)    $($s_BLD_SO_M) \
                                           $($s_BLD_SO_Mm) $($s_BLD_SO_Mmp))

else

ifeq ($(TARGET_OS),darwin)
# -------------------------------------------------------------------------
#
# Assume building dylib xx, version 1.2.3 in directory /lib/
#
# xx_Mmp       = 1 2 3               -- Space delimited version, temporary
# xx_M         = 1                   -- The major version
# xx_m         = 2                   -- The minor version
# xx_p         = 3                   -- The patch release
# xx_Mm        = 2.3                 -- The minor.patch values

# xx_DYLIBNAME = libxx.1.dylib       -- NAME used in linking the dylib
# xx_COMVER    = 2                   -- Compatibility version, just the minor
# xx_CURVER    = 2                   -- Current version, just the minor

# xx_DYLIB     = libxx.dylib        
# xx_M_DYLIB   = libxx.1.dylib     
# xx_Mm_DYLIB  = libxx.1.2.dylib 
# xx_Mmp_DYLIB = libxx.1.2.3.dylib    


# xx_DYLIB_Mmp = lib/xx.1.2.3.dylib -- Actual shareable image
# -------------------------------------------------------------------------

# --------------------------------------------------
# Break the version number into its component pieces
# --------------------------------------------------
$(foreach s,$(SHAREABLES), $(eval $s_Mmp       := $(subst .,$(space),$($s_VERSION))))
$(foreach s,$(SHAREABLES), $(eval $s_M         := $(word 1,$($s_Mmp))))
$(foreach s,$(SHAREABLES), $(eval $s_m         := $(word 2,$($s_Mmp))))
$(foreach s,$(SHAREABLES), $(eval $s_p         := $(word 3,$($s_Mmp))))
$(foreach s,$(SHAREABLES), $(eval $s_Mm        := $($s_M).$($s_m)))

# ---------------------------
# Form the strings needed for
#   -install_name
#   -compatibility_version
#   -current-version
# ---------------------------
$(foreach s,$(SHAREABLES), $(eval $s_DYLIBNAME := $s.$($s_M).dylib))
$(foreach s,$(SHAREABLES), $(eval $s_COMVER    := $($s_m)))
$(foreach s,$(SHAREABLES), $(eval $s_CURVER    := $($s_m)))

# ------------------------------------------
# Form the base names of the
#   - unversioned dylib
#   - major             version of the dylib
#   - major.minor       version of the dylib
#   - major.minor.patch version of the dylib
# ------------------------------------------
$(foreach s,$(SHAREABLES), $(eval $s_DYLIB     := $s.dylib))
$(foreach s,$(SHAREABLES), $(eval $s_M_DYLIB   := $s.$($s_M).dylib))
$(foreach s,$(SHAREABLES), $(eval $s_Mm_DYLIB  := $s.$($s_M).$($s_m).dylib))
$(foreach s,$(SHAREABLES), $(eval $s_Mmp_DYLIB := $s.$($s_M).$($s_m).$($s_p).dylib))

# ------------------------------------------------
# Form the names of the dylib in the lib directory
# ------------------------------------------------
$(foreach s,$(SHAREABLES), $(eval  $s_LIB_DYLIB     := $($s_LIBDIR)/$($s_DYLIB)))
$(foreach s,$(SHAREABLES), $(eval  $s_LIB_M_DYLIB   := $($s_LIBDIR)/$($s_M_DYLIB)))
$(foreach s,$(SHAREABLES), $(eval  $s_LIB_Mm_DYLIB  := $($s_LIBDIR)/$($s_Mm_DYLIB)))
$(foreach s,$(SHAREABLES), $(eval  $s_LIB_Mmp_DYLIB := $($s_LIBDIR)/$($s_Mmp_DYLIB)))


# --------------------------------------------------
# Form the names of the dylib in the build directory
#  -- The name of the BLD -> install directory
#     just haven't gotten to making this change
# --------------------------------------------------
$(foreach s,$(SHAREABLES), $(eval $s_BLD_DYLIB     := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                      $($s_DYLIB))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_M_DYLIB   := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                      $($s_M_DYLIB))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_Mm_DYLIB  := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                      $($s_Mm_DYLIB))))
$(foreach s,$(SHAREABLES), $(eval $s_BLD_Mmp_DYLIB := $(addprefix $(PKG_BLD_LIBDIR)/,\
                                                      $($s_Mmp_DYLIB))))

$(foreach x,$(EXECUTABLES), $(eval $x_BLD_EXE   := $(addprefix $(PKG_BLD_BINDIR)/,\
                                                   $(notdir $($x_EXE)))))

# ------------------------------------------------------------------------------
# Form the list of all librarys to be exported -- these are in its lib directory
# and those in the BLD (install) directories   -- these are in its lib directory5D
# ------------------------------------------------------------------------------
PKG_EXP_SOS  := $(foreach s,$(SHAREABLES),  $($s_LIB_DYLIB)    $($s_LIB_M_DYLIB)  \
                                            $($s_LIB_Mm_DYLIB) $($s_LIB_Mmp_DYLIB))

PKG_BLD_SOS  := $(foreach s,$(SHAREABLES),  $($s_BLD_DYLIB)    $($s_BLD_M_DYLIB) \
                                            $($s_BLD_Mm_DYLIB) $($s_BLD_Mmp_DYLIB))

else

$(foreach s,$(SHAREABLES), $(eval $s_BASENAME := $s.so))
$(foreach s,$(SHAREABLES), $(eval $s_SO       := $($s_LIBDIR)/$($s_BASENAME)))
$(foreach s,$(SHAREABLES), $(eval $s_SONAME   := $($s_NAMESPACE):$($s_BASENAME)))

PKG_EXP_SOS  := $(foreach s,$(SHAREABLES), $($s_SO))
PKG_BLD_SOS  := $(foreach s,$(SHAREABLES), $($s_BLD_SO))

PKG_EXP_EXES := $(foreach x,$(EXECUTABLES), $($x_EXE))
PKG_EXP_ROS  := $(foreach r,$(RELOCATABLES),$($r_RO))

endif
endif


PKG_BLD_EXES := $(addprefix $(PKG_BLD_BINDIR)/,$(EXECUTABLES))


# ======================================================================
#
# BUILD INCLUDE DIRECTORY
# -----------------------
# This populates the packages include directory in the target specific
# build area. Two lists of are composed.
#
# A list of non-target specific files 
# A list of target specific directories
#
# Rules/recipes that establish the symbolic links to these files are 
# also defined
#
# ----------------------------------------------------------------------


# --------------------------------------------------------
# Get the set of target specific directories in the
# include file directory.
# --------------------------------------------------------
PKG_SRC_INCTARGET  := $(wildcard $(PKG_INCDIR)/$(PKG_NAME)/target/$(TARGET))
#$(info PKG_SRC_INCTARGET = $(PKG_SRC_INCTARGET))
# --------------------------------------------------------

# ----------------------------------------
# Create the /target directory name
# This is actually the 
# -------------------------------------------------------------------
#$(info PKG_BLD_INCTARGET = $(PKG_BLD_INCTARGET))
#$(info PKG_SRC_INCTARGET = $(PKG_SRC_INCTARGET))

ifneq ($(PKG_SRC_INCTARGET),)
   PKG_BLD_INCTARGETS := $(PKG_BLD_INCTARGET) $(PKG_BLD_INCTARGET)/target
   #$(info PKG_BLD_INCTARGETS = $(PKG_BLD_INCTARGETS))
   PKG_BLD_INCTARGETS := $(filter-out $(PKG_BLD_INCTARGETS),\
                         $(wildcard $(PKG_BLD_INCTARGETS)))
else 
   PKG_BLD_INCTARGETS :=
endif
#$(info PKG_BLD_INCTARGETS = $(PKG_BLD_INCTARGETS))
# ------------------------------------------------------------------


# -------------------------------------------
# Find all the files in the include directory
# -------------------------------------------
PKG_SRC_INCFILES := $(wildcard $(PKG_INCDIR)/$(PKG_NAME)/*)
PKG_SRC_INCFILES := $(filter-out $(wildcard $(PKG_INCDIR)/$(PKG_NAME)/*~),\
                                 $(PKG_SRC_INCFILES))
#$(info PKG_SRC_INCFILES      = $(PKG_SRC_INCFILES))
# -------------------------------------------


# --------------------------------------
# Remove any target specific directories
# --------------------------------------
PKG_SRC_INCS := $(filter-out $(PKG_SRC_INCTARGET) ,$(PKG_SRC_INCFILES))
#$(info PKG_SRC_INCS = $(PKG_SRC_INCS))
# ---------------------------------

# -------------------------------
# Create the output include names
# -------------------------------
PKG_BLD_INCS := $(patsubst $(PKG_INCDIR)/$(PKG_NAME)/%, \
                           $(PKG_BLD_INCDIR)/%,           \
                           $(PKG_SRC_INCS))
#$(info PKG_BLD_INCS = $(PKG_BLD_INCS))
# -------------------------------



# ------------------------------------------------------------
# Remove any existing include files from the build destination
# ------------------------------------------------------------
PKG_BLD_INCS := $(filter-out $(wildcard $(PKG_BLD_INCS)),$(PKG_BLD_INCS))
#$(info PKG_BLD_INCS(new) = $(PKG_BLD_INCS))
# ------------------------------------------------------------
#$(info PKG_BLD_INCDIR = $(PKG_BLD_INCDIR))
$(PKG_BLD_INCDIR)/target : $(PKG_INCDIR)/$(PKG_NAME)//target/$(TARGET)
	@echo "    Create build inc dir link. $(call includefile, $@)"
	@ln -s $^ $@


$(PKG_BLD_INCDIR)/% : $(PKG_INCDIR)/$(PKG_NAME)/%
	@echo "    Create build inc link..... $(call includefile, $@)"
	@ln -s $^ $@

# ----------------------------------------------
# This is an order-only rule to ensure that the
# build include directory is created before it 
# is populated
# ----------------------------------------------
$(PKG_BLD_INCS) $(PKG_BLD_INCTARGETS) : | $(PKG_BLD_INCDIR)
# ----------------------------------------------

# ======================================================================





# ======================================================================
#
# LISTS:
# These generate various lists of files, for example, dependency and
# object files.  The lists have granularity that range from the list
# of files for a given constituent's C++ files to lists that include
# a both an givne constituents C++ and C files, to lists including 
# the C++ and C files for all constituents.
#
# <constituent>_CCDPEFILES: 
# The dependency files for a constituent's C++ source files
#
# <constituent>_CCOBJFILES: 
# The object files for a constituent's C++ source files
$
# <constituent>_CDPEFILES: 
# The dependency files for a constituent's C source files
#
# <constituent>_COBJFILES: 
# The object files for a constituent's C source files
#
# <constituent>_DEPFILES: 
# The dependency files for a constituent's C++ and C source files
#
# <constituent>_OBJFILES: 
# The object files for a constituent's C++ and C source files

# ======================================================================



# ----------------------------------------------------------------------
# CANNED RECIPE: _FLAGS
# ---------------------
# Captures the syntax of modular versions of the various flags passed
# to tools, e.g. the preprocessor, compilers, linker. The flags are
# composed of 3 pieces capturing 4 scopes
#
# A constituent file scope    <constituent_<file>_CXXFLAGS
# A constituent      scope    <constituent>__CXXFLAGS
# A file             scope    <file>__CXXFLAGS
# A package wide     scope    CXXFLAGS
#
# They are added in this order to the command line. For the most part
# this is done as the most restrictive first.  One could argue whether
# constituent which should come first 
#   -> <file>_flags or <constituent>_flags
#
# ----------------------------------------------------------------------
define _FLAGS
 $($2__$(*F)___$1) $($2__$1) $($(*F)___$1) $($1)
endef
# ----------------------------------------------------------------------


# ----------------------------------------------------------------------
# CANNED RECIPE: _CPPFLAGS
# -------------------------
# Captures the full preprocessor flags.  This consists of the user
# defined CPPFLAGS + INCLUDES.  The INCLUDES could be bundled with the
# CPPFLAGS, but it seemed convenient to allow the include directories 
# not to include the -I.  Of course the user is free to put -I <inc-dir>
# in the CPPFLAGS.  The INCLUDES syntax is encouraged, but can also be
# be considered just a convenience.
# ----------------------------------------------------------------------
define _CPPFLAGS
-I $($1_SRCDIR)/ $(foreach i,$(call _FLAGS,INCLUDES,$1),-I $i) $(call _FLAGS,CPPFLAGS,$1)
endef
# ----------------------------------------------------------------------


# ----------------------------------------------------------------------
# SOURCE DIRECTORY ASSIGNMENT
# ---------------------------
# If the source directory of a constituent is not defined, 
# The PKG_SRCDIR is assigned as the default
# ----------------------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
            $(eval $c_SRCDIR := $(if $($c_SRCDIR),$($c_SRCDIR),$(PKG_SRCDIR))))
# ----------------------------------------------------------------------


# -----------------------------------------------------------
# LIST: C++ dependency file list for a given constituent
#
# USAGE: 
# Generate a variable, one for each constituent,
# prefixed by the name of the constituent, 
# containing the list of C++ dependency files for 
# that consistutent.
#
# INPUT:
# The list of C++ source files for the constituent.
#
# OUTPUT:
# Vriables listing the constituent's dependency files
#
# EXAMPLE:
# c1_CCDEPFILES = file1.d file2.d
#
# where
#  c1               : The constituents name
#  file1.cc file2.cc: The constituents C++ source files
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
            $(eval $c_CCDEPFILES := $(call derive,$($c_DEPDIR), $($c_CCSRCFILES),.d)))
# -----------------------------------------------------------



# -----------------------------------------------------------
# LIST: C++ object file list for a given constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent,
# containing the list of static object files 
#
# INPUT:
# The list of C++ source files for the constituent.
#
# OUTPUT:
# Vriables listing each constituent's object files
#
# EXAMPLE:
# c1_CCDEPFILES = file1.o file2.o
#
# where
#  c1               : The constituents name
#  file1.cc file2.cc: The constituents C++ source files
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_CCOBJFILES := $(call derive,$($c_OBJDIR),$($c_CCSRCFILES),.o)))
# -----------------------------------------------------------



# -----------------------------------------------------------
# LIST: C depencency file list for a given constituent
#
# USAGE: 
# Generate a variable, one for each constituent,
# prefixed by the name of the constituent, 
# containing the list of C dependency files for 
# that consistutent.

# INPUT:
# The list of C source files for the constituent.
#
# OUTPUT:
# Vriables listing each constituent's dependency files
#
# EXAMPLE:
# c1_CDEPFILES = file3.d file4.d
#
# where
#  c1             : The constituent's name
#  file3.c file4.c: The constituent's C+ source files
# -----------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_CDEPFILES  := $(call derive,$($c_DEPDIR), $($c_CSRCFILES),.d)))
# -----------------------------------------------------------



# -----------------------------------------------------------
# LIST: C object file list for a givne constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent,
# containing the list of static object files 
#
# INPUT:
# The list of C source files for the constituent.
#
# OUTPUT:
# Variables listing each constituent's object files
#
# EXAMPLE:
# c1_CCDEPFILES = file3.o file4.o
#
# where
#  c1             : The constituents name
#  file3.c file4.c: The constituents C source files
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
	    $(eval $c_COBJFILES  := $(call derive,$($c_OBJDIR), $($c_CSRCFILES),.o)))
# ------------------------------------------------------



# ------------------------------------------------------
#
# LIST: All dependency files for a given constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# dependency files
#
# INPUT
# The C++ and C dependency file list for each constituent
#
# OUTPUT
# Variables listing each constituents dependency file list
#
# EXAMPLE:
# c1_DEPFILES = c1_CCDEPFILES + c1_CDEPFILES
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_DEPFILES   :=  $($c_CCDEPFILES) $($c_CDEPFILES)))
# ------------------------------------------------------


# -----------------------------------------------------------
# LIST: ASM depencency file list for a given constituent
#
# USAGE: 
# Generate a variable, one for each constituent,
# prefixed by the name of the constituent, 
# containing the list of ASM dependency files for 
# that consistutent.

# INPUT:
# The list of ASM source files for the constituent.
#
# OUTPUT:
# Vriables listing each constituent's dependency files
#
# EXAMPLE:
# c1_ASMDEPFILES = file3.d file4.d
#
# where
#  c1             : The constituent's name
#  file3.c file4.c: The constituent's C+ source files
# -----------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_ASMDEPFILES  := $(call derive,$($c_DEPDIR), $($c_ASMSRCFILES),.d)))
# -----------------------------------------------------------



# -----------------------------------------------------------
# LIST: ASM object file list for a given constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent,
# containing the list of static object files 
#
# INPUT:
# The list of ASM source files for the constituent.
#
# OUTPUT:
# Variables listing each constituent's object files
#
# EXAMPLE:
# c1_ASMDEPFILES = file3.o file4.o
#
# where
#  c1             : The constituents name
#  file3.S file4.S: The constituents ASM source files
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
	    $(eval $c_ASMOBJFILES  := $(call derive,$($c_OBJDIR), $($c_ASMSRCFILES),.o)))
# ------------------------------------------------------



# ------------------------------------------------------
#
# LIST: All dependency files for a given constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# dependency files
#
# INPUT
# The C++ and C dependency file list for each constituent
#
# OUTPUT
# Variables listing each constituents dependency file list
#
# EXAMPLE:
# c1_DEPFILES = c1_CCDEPFILES + c1_CDEPFILES
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_DEPFILES   :=  $($c_ASMDEPFILES) \
                           $($c_CCDEPFILES)  \
                           $($c_CDEPFILES)))



# ------------------------------------------------------
#
# LIST: All objects files for a given constituent
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# object files
#
# INPUT
# The C++ and C dependency file list for each constituent
#
# OUTPUT
# Variables listing each constituents dependency file list
#
# EXAMPLE:
# c1_OBJFILES = c1_CCOBJFILES + c1_COBJFILES
# ------------------------------------------------------
$(foreach c,$(CONSTITUENTS),\
  $(eval $c_OBJFILES   :=  $($c_ASMOBJFILES) \
                           $($c_CCOBJFILES)  \
                           $($c_COBJFILES)))
# ------------------------------------------------------



# ------------------------------------------------------
#
# LIST: All C++ files for all constituents
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# dependency files
#
# INPUT
# The C++/C dependency file list for each constituents
#
# OUTPUT
# Variables listing C++ dependency file list for all
# constituents
#
# EXAMPLE:
# CCDEPFILES = c1_CCOBJFILES + c2_CCOBJFILES
# ------------------------------------------------------
CCDEPFILES := $(foreach c,$(CONSTITUENTS),$($c_CCDEPFILES))
# ------------------------------------------------------


# ------------------------------------------------------
#
# LIST: All C files for all constituents
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# dependency files
#
# INPUT
# The C++/C dependency file list for each constituents
#
# OUTPUT
# Variables listing C++ dependency file list for all
# constituents
#
# EXAMPLE:
# CDEPFILES = c1_CDEPFILES + c2_CDEPFILES
# ------------------------------------------------------
CDEPFILES  := $(foreach c,$(CONSTITUENTS),$($c_CDEPFILES))
# ------------------------------------------------------


# ------------------------------------------------------
#
# LIST: All ASM dependency files for all constituents
#
# USAGE:
# Generate a variable, one for each constituent
# prefixed by the name of the constituent, of all the
# dependency files
#
# INPUT
# The C++/C dependency file list for each constituents
#
# OUTPUT
# Variables listing C++ dependency file list for all
# constituents
#
# EXAMPLE:
# CDEPFILES = c1_COBJFILES + c2_COBJFILES
# ------------------------------------------------------
ASMDEPFILES  := $(foreach c,$(CONSTITUENTS),$($c_ASMDEPFILES))
# ------------------------------------------------------




# ======================================================================
# CANNED RECIPES:
# These capture the nut's and bolt's of a target gets built when the
# procedure is complex.  Currently, only the C++ and C dependency files
# use canned recipes
# ======================================================================




# ======================================================================
# TEMPLATES:
# Templates are used in conjunction with a foreach an the make 'eval'
# function to generate the pattern rules and recipes for the each
# constituent's target.
#
# CCDEP_template:
# Pattern rule to generate the dependency files for a given constituents
# C++ source files.
#
# CCOBJ_template:
# Pattern rule to generate the object files for a given constituents
# C++ source files.
#
# CDEP_template:
# Pattern rule to generate the dependency files for a given constituents
# C source files.
#
# COBJ_template:
# Pattern rule to generate the object files for a given constituents
# C source files.
#
# LINK_EXE_template
# Pattern rule to generate an executable constituent
#
# ALIAS_EXE_template
# Generate an alias (phony target) generate an executable constituent
#
# LINK_RO_template
# Pattern rule to generate an relocatable object constituent. This is
# sometimes known as partial linking
#
# ALIAS_RO_template
# Generate an alias (phony target) for a relocatable partially linked
# constituent
#
#
# ======================================================================




# ---------------------------------------------------------------
# RULE TEMPLATE: CCDEP_template
# 
# USAGE:
# Generates the rule needed to produce the dependency files from
# a constituent's list of C++ source files
#
# ---------------------------------------------------------------
define CCDEP_template
$($(1)_CCDEPFILES) : | $($(1)_DEPDIR)
$($(1)_CCDEPFILES) : $($(1)_DEPDIR)/%.d : $($(1)_SRCDIR)/%.cc
	@echo "    Update dependencies........ $$(call pkgfile,$$<)"
	$(DBG_DEP_CC)@$(CXX) -MM -MT $$@ -MT $$(call objfile,$$@)  \
                                             $$(call _CPPFLAGS,$1) \
                                             $$(call _FLAGS,CXXFLAGS,$1) \
                                             $$< > $$@
#	$(update_cc_dependency)
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: CCOBJ_template
# 
# USAGE:
# Generates the rule needed to produce the object files from
# a constituent's list of C++ source files
#
# ---------------------------------------------------------------
define CCOBJ_template
$($(1)_CCOBJFILES) : | $($(1)_OBJDIR)
$($(1)_CCOBJFILES) :   $($(1)_OBJDIR)/%.o : $($(1)_SRCDIR)/%.cc \
                                            $($(1)_DEPDIR)/%.d
	@echo "    Compile.................... $$(call pkgfile,$$<)"
	$(DBG_CMP_CC)$(CXX)  -c $(call _CPPFLAGS,$1) \
                                $(call _FLAGS,CXXFLAGS,$1) -o $$@ $$<
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: CCLOBJ_template
# 
# USAGE:
# Generates the rule needed to produce the position independent 
# object files from a constituent's list of C++ source files
#
# ---------------------------------------------------------------
define CCLOBJ_template
$($(1)_CCOBJFILES) : | $($(1)_OBJDIR)
$($(1)_CCOBJFILES) :   $($(1)_OBJDIR)/%.o : $($(1)_SRCDIR)/%.cc \
                                            $($(1)_DEPDIR)/%.d
	@echo "    Compile (shr).............. $$(call pkgfile,$$<)"
	$(DBG_CMP_CC)$(CXX)  -c $$(call _CPPFLAGS,$1) -fPIC \
                                $$(call _FLAGS,CXXFLAGS,$1) -o $$@ $$<
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: CDEPFILE_template
# 
# USAGE:
# Generates the rule needed to produce the dependency files from
# a constituent's list of C source files
#
# ---------------------------------------------------------------
define CDEP_template
$($(1)_CDEPFILES)  : | $($(1)_DEPDIR)
$($(1)_CDEPFILES)  :   $($(1)_DEPDIR)/%.d : $($(1)_SRCDIR)/%.c
	@echo "    Update dependencies........ $$(call pkgfile,$$<)"
	$(DBG_DEP_C)$(CC) -MM -MT $$@ -MT $$(call objfile,$$@)       \
                                  $$(call _CPPFLAGS,$1)      \
                                  $$(call _FLAGS,CFLAGS,$1)  $$< > $$@
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: COBJ_template
# 
# USAGE:
# Generates the rule needed to produce the object files from
# a constituent's list of C source files
#
# ---------------------------------------------------------------
define COBJ_template
$($(1)_COBJFILES) : | $($(1)_OBJDIR)
$($(1)_COBJFILES) :   $($(1)_OBJDIR)/%.o : $($(1)_SRCDIR)/%.c  \
                                           $($(1)_DEPDIR)/%.d
	@echo "    Compile.................... $$(call pkgfile,$$<)"
	$(DBG_CMP_C)$(CC)  -c $$(call _CPPFLAGS,$1) \
                              $$(call _FLAGS,CFLAGS,$1) -o $$@ $$<
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: ASMDEPFILE_template
# 
# USAGE:
# Generates the rule needed to produce the dependency files from
# a constituent's list of ASM source files
#
# ---------------------------------------------------------------
define ASMDEP_template
$($(1)_ASMDEPFILES)  : | $($(1)_DEPDIR)
$($(1)_ASMDEPFILES)  :   $($(1)_DEPDIR)/%.d : $($(1)_SRCDIR)/%.c
	@echo "    Update dependencies........ $$(call pkgfile,$$<)"
	$(DBG_DEP_ASM) $(CC) -MM -MT $$@ -MT $$(call objfile,$$@)       \
                              $$(call _CPPFLAGS,$1)      \
                              $$(call _FLAGS,ASMFLAGS,$1)  $$< > $$@
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: ASMOBJ_template
# 
# USAGE:
# Generates the rule needed to produce the object files from
# a constituent's list of ASM source files
#
# ---------------------------------------------------------------
define ASMOBJ_template
$($(1)_ASMOBJFILES) : | $($(1)_OBJDIR)
$($(1)_ASMOBJFILES) :   $($(1)_OBJDIR)/%.o : $($(1)_SRCDIR)/%.c  \
                                           $($(1)_DEPDIR)/%.d
	@echo "    Compile.................... $$(call pkgfile,$$<)"
	$(DBG_CMP_ASM)$(CC)  -c $$(call _CPPFLAGS,$1) \
                                $$(call _FLAGS,ASMFLAGS,$1) -o $$@ $$<
endef
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: CLOBJ_template
# 
# USAGE:
# Generates the rule needed to produce the position independent
# object files from a constituent's list of C source files
#
# ---------------------------------------------------------------
define CLOBJ_template
$($(1)_COBJFILES) : | $($(1)_OBJDIR)
$($(1)_COBJFILES) : $($(1)_OBJDIR)/%.o : $($(1)_SRCDIR)/%.c  \
                                         $($(1)_DEPDIR)/%.d
	@echo "    Compile (shr).............. $$(call pkgfile,$$<)"
	$(DBG_CMP_C)$(CC)  -c $$(call _CPPFLAGS,$1) -fPIC \
                              $$(call _FLAGS,CFLAGS,$1) -o $$@ $$<
endef
# ---------------------------------------------------------------




# ===========
# EXECUTABLES
# ===========


# ---------------------------------------------------------------
# RULE TEMPLATE: LINK_EXE_template
#
# USAGE:
# Generates the rule needed to produce the exeuctable file from
# a constituent's object files and libraries
#
# INPUT:
# The list of a constituent's object files and libaries
#
# OUTPUT:
# The pattern rule used to create the executable.
#
# ---------------------------------------------------------------

ifeq ($(TARGET_OS),linux)

define LINK_EXE_template
$($(1)_EXE) : | $$(dir $(1)_EXE)
$($(1)_EXE) : $($(1)_OBJFILES) $($(1)_ROS) $($(1)_SOS)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_EXE)$(CXX) $$($1_LDFLAGS) $(LDFLAGS)   \
                              $$(call _FLAGS,LOADLIBS,$1) \
                              $$(call _FLAGS,LDLIBS,$1)   \
                              -o $$@ $$^ 
endef

else

ifeq ($(TARGET_OS),darwin)

define LINK_EXE_template
$($(1)_EXE) : | $$(dir $(1)_EXE)
$($(1)_EXE) : $($(1)_OBJFILES) $($(1)_ROS) $($(1)_SOS)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_EXE)$(CXX) $$($1_LDFLAGS) $(LDFLAGS)   \
                              $$(call _FLAGS,LOADLIBS,$1) \
                              $$(call _FLAGS,LDLIBS,$1)   \
                              -o $$@ $$^ 
endef

else

define LINK_EXE_template
$($(1)_EXE) : | $$(dir $(1)_EXE)
$($(1)_EXE) : $($(1)_OBJFILES) $($(1)_ROS) $($(1)_SOS)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_EXE)$(CXX) $$($1_LDFLAGS) $(LDFLAGS)                \
                              $$(call _FLAGS,LOADLIBS,$1)              \
                              $$(call _FLAGS,LDLIBS,$1)                \
        -Wl,-soname=$(if $($1_SONAME),$($1_SONAME),$($1_NAMESPACE):$1) \
                              -o $$@ $$^ 
endef

endif
endif

# ---------------------------------------------------------------


# ======================================================================
#
# INSTALL BIN DIRECTORY
# -------------------
# Populates the install bin directory
# ======================================================================
$(PKG_BLD_EXES) : $(PKG_BLD_BINDIR)/% : $(PKG_EXP_BINDIR)/% | $(dir $^)
	@echo "    Copying.......(install).... $(call buildfile,$@)"
	@cp $^ $@
#@rm -f $@
#@ln -s $^ $@
#======================================================================


#======================================================================
#
# INSTALL LIB DIRECTORY
# -------------------
# Populates the install lib directory
# ======================================================================
$(PKG_BLD_SOS) : $(PKG_BLD_LIBDIR)/% : $(PKG_EXP_LIBDIR)/% | $(dir $^)
	@echo "    Symbolic Link..(install)... $(call buildfile,$@)"
	@rm -f $@
	@ln -s $^ $@
# ======================================================================



# ---------------------------------------------------------------
# RULE: ALIAS_EXE_teomplate
#
# USAGE:
# Produces a aliased target name (i.e. a phony target) for an
# constituent executable.
#
# EXAMPLE: 
# foo_ALIAS := foo  -- in a user's make file will produce
#                      a phony target 'foo'allowing the user
#                      to type 'make foo'
#
# NOTE:
# The alias is defined if and only if foo_ALIAS is defined
# ---------------------------------------------------------------
define ALIAS_EXE_template

ifdef $(1)_ALIAS
  .PHONY: $($(1)_BLD_ALIAS)
  $($(1)_ALIAS) : $($(1)_BLD_EXE)
endif

endef
# ======================================================================




# ==========================================
#
# RELOCATABLEs
# ------------
# These are partially linked static objects.
#
# ==========================================


# ----------------------------------------------------------------------
# RELOCATABLE: Linking
# -----------------------------
define LINK_RO_template
$($(1)_RO) : | $$(dir $(1)_RO)
$($(1)_RO) : $($(1)_OBJFILES)
	@echo "    Partial link............... $$(call binaryfile,$$@)"
	$(DBG_LINK_RO)$(LD) -r -nostdlib $$($1_LDFLAGS)              \
                      $$(call _FLAGS,LOADLIBS,$1) \
                      $$(call _FLAGS,LDLIBS,$1)   \
                      -o $$@ $$^
endef
# ---------------------------------------------------------------------



# ---------------------------------------------------------------------
# RELOCATABLE Alias
# Example: bar__ALIAS := bar_ro
# -----------------------------
define ALIAS_RO_template

ifdef $(1)_ALIAS
  .PHONY: $($(1)_ALIAS)
  $($(1)_ALIAS) : $($(1)_RO)
endif
endef
# ---------------------------------------------------------------------


# ---------------------------------------------------------------------
# RELOCATABLES: End
# ======================================================================


# ================
# SHARED LIBRARIES
# ================

# ---------------------------------------------------------------
# SO_LDFLAGS
# 
# Defines the linker flags needed/used to build a shareable image
# ---------------------------------------------------------------
#SO_LDFLAGS :=  -shared                                         \
#               -Wl,--hash-style=gnu                            \
#               -Wl,--no-undefined                              \
#               -Wl,--allow-shlib-undefined                     \
#               -Wl,--unresolved-symbols=ignore-in-shared-libs
# ---------------------------------------------------------------



# ---------------------------------------------------------------
# RULE TEMPLATE: LINK_SO_template
#
# USAGE:
# Generates the rule needed to produce the shared library from
# a constituent's position independent object files and libraries
#
# INPUT:
# The list of a constituent's object files and libaries
#
# OUTPUT:
# The pattern rule used to create the executable.
#
# ---------------------------------------------------------------
ifeq ($(TARGET_OS),linux)

#$(info linux)

define LINK_SO_template
$($1_SO) $($1_SO_M) $($1_SO_Mm) : $($1_SO_Mmp)
	@echo "    Symbolic Link..(export).... $$(call exportfile,$$@)"
	@rm -f $$@
	@ln -sT $$(notdir $$^) $$@
$($(1)_SO_Mmp) : $($(1)_OBJFILES)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_SO)$(CXX) $(SO_LDFLAGS) -Wl,-soname=$($1_SONAME) \
                             $$($1_LDFLAGS)                         \
                             $$(call _FLAGS,LOADLIBS,$1)            \
                             $$(call _FLAGS,LDLIBS,$1)              \
                             -o $$@ $$^
endef


# ---------------------------------------------------------------
# RULE: ALIAS_SO_template
#
# USAGE:
# Produces a aliased target name (i.e. a phony target) for an
# constituent executable.
#
# EXAMPLE: 
# foo_ALIAS := foo  -- in a user's make file will produce
#                      a phony target 'foo'allowing the user
#                      to type 'make foo'
#
# NOTE:
# The alias is defined if and only if foo_ALIAS is defined
# ---------------------------------------------------------------
define ALIAS_SO_template

ifdef $(1)_ALIAS
  .PHONY: $($(1)_ALIAS)
  $($(1)_ALIAS) : $($(1)_SO_Mmp) $($1_SO) $($1_SO_M) $($1_SO_Mm)
endif

endef


else

ifeq ($(TARGET_OS),darwin)

#$(info "darwin")

define LINK_SO_template
$($1_LIB_DYLIB) $($1_LIB_M_DYLIB) $($1_LIB_Mm_DYLIB) : $($1_LIB_Mmp_DYLIB)
	@echo "    Symbolic Link..(export).... $$(call exportfile,$$@)"
	@rm -f $$@
	@ln -s $$(notdir $$^)  $$@
$($(1)_LIB_Mmp_DYLIB) : $($(1)_OBJFILES)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_SO)$(CXX) $(DYLIB_LDFLAGS)                       \
                             -dynamiclib                            \
                             -install_name $($1_DYLIBNAME)          \
                             -compatibility_version $($1_COMVER)    \
                             -current_version $($1_CURVER)          \
                             $$($1_LDFLAGS)                         \
                             $$(call _FLAGS,LOADLIBS,$1)            \
                             $$(call _FLAGS,LDLIBS,$1)              \
                             -o $$@ $$^
endef


# ---------------------------------------------------------------
# RULE: ALIAS_SO_template
#
# USAGE:
# Produces a aliased target name (i.e. a phony target) for an
# constituent executable.
#
# EXAMPLE: 
# foo_ALIAS := foo  -- in a user's make file will produce
#                      a phony target 'foo'allowing the user
#                      to type 'make foo'
#
# NOTE:
# The alias is defined if and only if foo_ALIAS is defined
# ---------------------------------------------------------------
define ALIAS_SO_template

ifdef $(1)_ALIAS
  .PHONY: $($(1)_ALIAS)
   $($(1)_ALIAS) : $($(1)_Mmp_DYLIB) $($1_DYLIB) $($1_SO_M) $($1_Mm_DYLIB)
endif

endef


else

#$(info OTHER)

define LINK_SO_template
$($(1)_SO) : $($(1)_OBJFILES)
	@echo "    Link....................... $$(call exportfile,$$@)"
	$(DBG_LINK_SO)$(CXX) $(SO_LDFLAGS) -Wl,-soname=$($1_SONAME) \
                             $$($1_LDFLAGS)                         \
                             $$(call _FLAGS,LOADLIBS,$1)            \
                             $$(call _FLAGS,LDLIBS,$1)              \
                             -o $$@ $$^
endef

# ---------------------------------------------------------------
# RULE: ALIAS_SO_template
#
# USAGE:
# Produces a aliased target name (i.e. a phony target) for an
# constituent executable.
#
# EXAMPLE: 
# foo_ALIAS := foo  -- in a user's make file will produce
#                      a phony target 'foo'allowing the user
#                      to type 'make foo'
#
# NOTE:
# The alias is defined if and only if foo_ALIAS is defined
# ---------------------------------------------------------------
define ALIAS_SO_template

ifdef $(1)_ALIAS
  .PHONY: $($(1)_ALIAS)
  $($(1)_ALIAS) : $($(1)_SO_Mmp) $($1_SO) $($1_SO_M) $($1_SO_Mm)
endif

endef


endif
endif

# ---------------------------------------------------------------



# ======================================================================


# ----------------------------------------------------------------------
# __STATICS: 
# Captures the list of products that use static object files in their
# build.
#
# __PICS
# Captures the list of products that use position indepedent object
# files in their build.
# ----------------------------------------------------------------------
__STATICS := $(EXECUTABLES) $(RELOCATABLES)
__PICS    := $(SHAREABLES)
# ----------------------------------------------------------------------



$(foreach c,$(CONSTITUENTS),$(eval $(call CDEP_template,$c)))
$(foreach c,$(CONSTITUENTS),$(eval $(call CCDEP_template,$c)))

$(foreach c,$(__STATICS),$(eval $(call COBJ_template,$c)))
$(foreach c,$(__STATICS),$(eval $(call CCOBJ_template,$c)))
$(foreach c,$(__PICS),$(eval $(call CLOBJ_template,$c)))
$(foreach c,$(__PICS),$(eval $(call CCLOBJ_template,$c)))



$(foreach x,$(EXECUTABLES),$(eval $(call LINK_EXE_template,$x)))
$(foreach x,$(EXECUTABLES),$(eval $(call ALIAS_EXE_template,$x)))

$(foreach r,$(RELOCATABLES),$(eval $(call LINK_RO_template,$r)))
$(foreach r,$(RELOCATABLES),$(eval $(call ALIAS_RO_template,$r)))

$(foreach s,$(SHAREABLES),$(eval $(call LINK_SO_template,$s)))
$(foreach s,$(SHAREABLES),$(eval $(call ALIAS_SO_template,$s)))




print_list_goals    := print_flags           \
                     print_dependencyfiles \
                     print_objectfiles     \
                     print_directories     \
                     print_tools           \
                     print_start_of_build  \
                     print_end_of_build

make_build_goals := make_build_rootdir     \
                    make_build_directories \
                    make_build_includes 


nondependency_goals := $(print_list_goals) $(make_build_goals)
dependency_goals    := $(filter-out $(nondependency_goals), $(MAKECMDGOALS))
#$(info nondependency_goals = $(nondependency_goals))
#$(info MAKECMDGOALS        = $(MAKECMDGOALS))
#$(info dependency_goals    = $(dependency_goals))

ifneq ($(dependency_goals),)
 __DEPENDENCIES := $(foreach c,$(CONSTITUENTS),$(PKG_BINARY_ROOT)/$c/dep/*.d)
 -include $(__DEPENDENCIES)
endif



.PHONY: all 
.PHONY: make_directories
.PHONY: $(make_build_goals)
.PHONY: $(print_list_goals)
.PHONE: depend includes


all:                             \
	print_start_of_build     \
	make_directories         \
                                 \
	make_build_includes      \
	$(PKG_EXP_EXES)          \
	$(PKG_EXP_ROS)           \
	$(PKG_EXP_SOS)           \
        $(PKG_BLD_SOS)           \
        $(PKG_BLD_EXES)          \
	                         \
	print_end_of_build



includes depend:                \
	print_start_of_build    \
	make_directories        \
	make_build_includes     \
	$(CCDEPFILES)           \
	$(CDEPFILES)            \
	print_end_of_build



# -------------------------
# Encapsulate how to create
# a new directory.
# -------------------------
define make_directory
@mkdir -p $@
endef


make_build_rootdir: \
	$(filter-out $(wildcard $(PRJ_BUILD_ROOT)),    $(PRJ_BUILD_ROOT))

make_build_incdir:  \
        $(filter-out $(wildcard $(PKG_BLD_INCDIR)), $(PKG_BLD_INCDIR))

make_build_libdir:  \
        $(filter-out $(wildcard $(PKG_BLD_LIBDIR)), $(PKG_BLD_LIBDIR))

make_build_bindir:  \
        $(filter-out $(wildcard $(PKG_BLD_BINDIR)), $(PKG_BLD_BINDIR))  \
	$(foreach x,$(EXECUTABLES),                                     \
                  $(filter_out $(widdcard $(PKG_BLD_BINDIR)/$x),       \
                                          $(PKG_BLD_BINDIR)/$x))

make_build_directories:   \
        make_build_incdir \
	make_build_libdir \
        make_build_bindir


make_directories:                                                        \
	$(filter-out $(wildcard $(PKG_BINARY_ROOT)), $(PKG_BINARY_ROOT)) \
	$(filter-out $(wildcard $(PKG_DEPDIRS)),     $(PKG_DEPDIRS))     \
	$(filter-out $(wildcard $(PKG_OBJDIRS)),     $(PKG_OBJDIRS))     \
	$(filter-out $(wildcard $(PKG_EXPORT_ROOT)), $(PKG_EXPORT_ROOT)) \
	$(filter-out $(wildcard $(PKG_BINDIRS)),     $(PKG_BINDIRS))     \
	$(filter-out $(wildcard $(PKG_LIBDIRS)),     $(PKG_LIBDIRS))     \
        make_build_directories

make_build_includes:           \
	make_build_incdir      \
	$(PKG_BLD_INCTARGETS)  \
	$(PKG_BLD_INCS)


$(PKG_BINARY_ROOT):
	@echo  "    Create binary root   dir .. $@"
	$(make_directory)

$(PKG_DEPDIRS):
	@echo  "    Create dependency   dirs .. $(call binaryfile,$@)"
	$(make_directory)

$(PKG_OBJDIRS):
	@echo  "    Create object       dirs .. $(call binaryfile,$@)"
	$(make_directory)

$(PKG_EXPORT_ROOT):
	@echo  "    Create export root   dir .. $@"
	$(make_directory)



#$(PKG_BINDIRS):  -- Not sub-directoring executables
$(PKG_EXP_BINDIR):

	@echo  "    Create binary  directory .. $(call exportfile,$@)"
	$(make_directory)


#$(PKG_LIBDIRS): -- Not sub-directoring libraries
$(PKG_EXP_LIBDIR):
	@echo  "    Create library directory .. $(call exportfile,$@)"
	$(make_directory)

$(PRJ_BUILD_ROOT):
	@echo  "    Create build root    dir .. $@"

$(PKG_BLD_INCDIR):
	@echo  "    Create build include dir .. $@"
	$(make_directory)

$(PKG_BLD_LIBDIR):
	@echo  "    Create build lib     dir .. $@"
	$(make_directory)

$(PKG_BLD_BINDIR):
	@echo  "    Create build bin     dir .. $@"
	$(make_directory)



print_start_of_build:
	@echo    "Start build of package........ $(PRJNAME) - $(TARGET)"


print_dependencyfiles:
	@echo "Dependency files"
	@echo $(CCDEPFILES) $(CDEPFILES) | sed 's/ /\n'/g

print_objectfiles:
	@echo "Object     files"
	@echo $(CCOBJFILES) $(COJBFILES) | sed 's/ /\n'/g

print_directories:
	@echo  $(ECHO_OPT)                                                    \
               "\nPackage source root............... $(PKG_SOURCE_ROOT)"      \
               "\nPackage binary root............... $(PKG_BINARY_ROOT)"      \
               "\n Dependency directories........... $(call binarylist,       \
                                                        $(PKG_DEPDIRS))"      \
	       "\n Object     directories........... $(call binarylist,       \
                                                        $(PKG_OBJDIRS))"      \
               "\nPackage export root............... $(PKG_EXPORT_ROOT)"      \
	       "\n Library    directories........... $(call exportlist,       \
	                                             $(PKG_EXP_LIBDIR))"      \
               "\n Binary     directories............$(call exportlist,       \
	                                             $(PKG_EXP_BINDIR))"      \
               "\nPackage build root................ $(PRJ_BUILD_ROOT)"       \
               "\n Include    directories............$(call buildlist,        \
                                                     $(PKG_BLD_INCDIR))"      \
               "\n Library    directories............$(call buildlist,        \
                                                     $(PKG_BLD_LIBDIR))"      \
               "\n Binary     directories............$(call buildlist,        \
                                                     $(PKG_BLD_BINDIR))"      \
               "\n"

print_end_of_build:
	@echo $(ECHO_OPT) \
              "End   build of package........ $(PRJNAME) - $(TARGET)\n" 


print_flags:
	@echo "Begin print goals for ......... $(PRJNAME) - $(TARGET)"
	@echo "CFLAGS  : $(CFLAGS)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS : $(LDFLAGS)"
	@echo "INCLUDES: $(INCLUDES)"
	@echo "End  print goals for ......... $(PRJNAME) - $(TARGET)\n"

print_tools:
	@echo $(ECHO_OPT)                    \
              "\nTools"                      \
	      "\nC  compiler: $(CC)"         \
	      "\nCC compiler: $(CXX)"        \
              "\nAS asembler: $(AS)"         \
	      "\nLD loader  : $(LD)"         \
	      "\nLD flags   : $(LDFLAGS)"

endif

