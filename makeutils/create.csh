#! /bin/sh

#
#  USAGE:
#    create <stem> <package-name>
#


# -- create <stem>/<package-name>
mkdir -p $1/$2
echo Created: $1/$2

# -- create <stem>/<package-name>/user
mkdir -p $1/$2/user
echo Created: $1/$2/user

# -- create <stem>/<package-name>/user/source
mkdir -p $1/$2/user/source
echo Created: $1/$2/user/source

# -- create <stem>/<package-name>/user/source/cc/ and inc, src and make directories
mkdir -p $1/$2/user/source/cc
echo Created: $1/$2/user/source/cc

mkdir -p $1/$2/user/source/cc/$2
echo Created: $1/$2/user/source/cc/$2

mkdir -p $1/$2/user/source/cc/src
echo Created: $1/$2/user/source/cc/src

mkdir -p $1/$2/user/source/cc/src
echo Created: $1/$2/user/source/cc/ptd

mkdir -p $1/$2/user/source/cc/make
echo Created: $1/$2/user/source/cc/make


# -- create the barebones Makefile 
script=$(readlink -f "$0")
basedir=$(dirname "$script")
cp $basedir/Makefile_template $1/$2/user/source/cc/make/Makefile
echo Created: $1/$2/user/source/cc/make/Makefile
unset script
unset basedir
