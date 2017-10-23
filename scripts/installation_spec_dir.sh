#! /bin/sh
# ----------------------------------------------------------------------
#
# PURPOSE:
# Return the machine-capability-os specific installation subdirectory. 
#
# USAGE:
# $ <root>/installation_spec_dir.sh
#
#
# AUTHOR:
# jjrussell
#
# DATE:
# 2017.09.21
#
# NOTES:
# This is meant to be a helper function for the settings.sh and
# settings.csh scripts.  In general, it will not be called by the user,
# although nothing prohibits that.
#
# METHOD:
# The /proc/cpuinfo file is scanned by grep for the 'flags' line.
# This line is captured and then scanned for the supported options.
#
# FYI:
# At one time this was done using a executable. This works, but 
# naturally demands that this exe be built before using it.  While this
# will almost certainly be the case, using grep seems to be a more 
# robust solution, that is as long as one is limited to Linux. 
# 
# The advantage of this method is that the directories do not have to
# exist before executing this script, so the jacketing settings.csh and 
# settings.sh scripts can now be safely put into login scripts without
# fear-of-failure.  Using the executalbe, the directory containing the
# executable along with the executable had to exist, meaning a make have
# had to be previously peformed.
#
# ----------------------------------------------------------------------


### -----------------------------
### Find the machine and os names
### -----------------------------
machine=`uname -m`
os=`uname -s`


### --------------------------------------
### The convention is to lower case the OS
### --------------------------------------
if [ $os == "Linux" ]; then
      os="linux"
fi


### ------------------------------------------------------------
### Get the supported advanced options
### ------------------------------------------------------------
### Scan /proc/cpuinfo for the first occurance of the flags line
### ------------------------------------------------------------
flags=`grep -m1 "^flags" /proc/cpuinfo`


### -------------
### Look for avx2
### -------------
x=`grep " avx2 " <<< ${flags}`
if [ $? -eq 0 ]; then
    opt=avx2
else
    ### ------------
    ### Look for avx
    ### ------------
    x=`grep " avx " <<< ${flags}`

    if [ $? -eq 0 ]; then
        opt=avx
    else
        ### ------------------
        ### Default to generic
        ### ------------------
        opt=gen
    fi
fi

### ---------------------------------------------------------
### Construct the machine-capability-os specific subdirectory
### and return it as the value of this script
### ---------------------------------------------------------
install_spec_dir=${machine}-${opt}-${os}
echo $install_spec_dir

exit 0