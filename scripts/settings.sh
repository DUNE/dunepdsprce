# ----------------------------------------------------------------------
#
# Setup PATH and LD_LIBRARY_PATH to point to host specific bin and lib
# directories
#
# USAGE:
# $ source <root>/settings.csh [install-root]
#
# Where:
#    install-root: This is an optional parameter locating the host
#                  specific installation root. An example is
#                     ~user/protodune/install/x86_64-avx-linux"
#                  This is used to override the defaults, for example
#                  if the installation directory has been placed 
#                  somewhere other than the standard place or if,
#                  for testing, purposes, one wishes to use images
#                  and libraries other than these.  An example would
#                  be to test avx code on a machine that normally 
#                  runs avx2 code.
#
# AUTHOR:
# jjrussell
#
# DATE:
# 2017.09.21
#
#
# METHOD:
# If no parameters are provided, this script auto-locates the appropriate
# directories.
#
# The script assumes that the installation directory is located at the same
# level as the script directory with a name install/.  The directory
# containing the script directory is located and "install/" is tacked
# on.  
#
# The next step determines the host specfic installation directory.
# This directory is assumed to be located directly below the "install/"
# directory and has the form
#
#     <machine>-<opt>-<os>
#
# Where
#      machine: The result of a "uname -m"
#          opt: The result of executing a capabilties program 
#           os: The non-capitalized version of "uname -s"
#
# The tricky one is locating and exectuing the capabilities program.
# This program is assume located in 
#
#      <root>/install/<machine>-gen-<os>/bin/capabilities
#
# i.e the generic directory. This program will return the 'opt' string
# appropriate for this machine.
#
# HISTORY
#
#       When  Who   What
# ----------  ---   -----------------------------------------------------
# 2017.10.29  jjr   On the MAC, readlink returns an empty string if the
#                   the file is not a symbolic link.  If an empty string
#                   is returned, the original file name is used.
#
# 2017.10.28  jjr   Added support for the MAC
#                   Removed the -f on readlink, it means something very
#                   different on the MAC. On LINUX it 'canocalizes' the
#                   the filename.  This is not necessary, but seemed did
#                   it because it seemed to be good form.
#
#                   Added -n to  suppress the newline which is only 
#                   necessary when displaying to the terminal.
# ----------------------------------------------------------------------
called=${BASH_SOURCE[0]}

os=`uname -s`

### ------------------------------
### Make sure the file was sourced
### ------------------------------
if [ $called != $0 ]; then 

   ### ---------------------------------------
   ### Correctly called by sourcing the script
   ### Get the absolute path to the script 
   ### ---------------------------------------
   if [ `uname` == Linux ]; then script_fn=`readlink -fn $called`
   else script_fn=`perl -e "use Cwd 'abs_path'; print abs_path('$called')"`; fi

else

   ### ---------------------------------------------------
   ### Incorrectly called by directly executing the script
   ### ---------------------------------------------------
   echo "Error: This file must be sourced to set PATH and (DY)LD_LIBRARY_PATH"
   exit -1

fi


### -------------------------------
### Establish the installation root
### -------------------------------
if [ -z $1 ]; then

   ### -------------------------------------------
   ### Find the root directory of this script file
   ### -------------------------------------------
   script_dir=`dirname $script_fn`
   install_root=`dirname $script_dir`/install
   ### --- echo "script  file name= $script_fn"
   ### --- echo "script  dir      = $script_dir"
else

   ### -------------------
   ### User specified root
   ### -------------------
   install_root=$1

fi

# -- echo "install root     = ${install_root}"


### ----------------------------------------------------------------
### Construct the machine-capability-os specific installation path
### This is used to locate the /bin and /lib directories that 
### provide optimal performance
### ----------------------------------------------------------------
install_path=${install_root}/`${script_dir}/installation_spec_dir.sh`
### --- echo "install path     = ${install_path}"

### ------------------
### Setup library path
### ------------------
if [ ${os} == "Linux" ]; then


   if [ -n "${LD_LIBRARY_PATH}" ]; then
      export LD_LIBRARY_PATH=${install_path}/lib:${LD_LIBRARY_PATH}
   else
      export LD_LIBRARY_PATH=${install_path}/lib:
   fi

else

   if [ ${os} == "Darwin" ]; then

      if [ -n "${DYLD_LIBRARY_PATH}" ]; then
          export DYLD_LIBRARY_PATH=${install_path}/lib:${DYLD_LIBRARY_PATH}
      else
          export DYLD_LIBRARY_PATH=${install_path}/lib:
      fi

   else
       
     echo "Error:: Unrecognized os = ${os}"
     exit -1
      
   fi

fi

### ------------------
### Setup binary  path
### ------------------

if [ -n "${PATH}" ]; then
   export PATH=${install_path}/bin:${PATH}
else
   export PATH=${install_path}/bin:
fi


export PDDAM_LIB_INC=${install_path}/include
export PDDAM_LIB_LIB=${install_path}/lib
