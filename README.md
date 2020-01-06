# proto-dune-dam-lib
proto-DUNE Data Access Method (DAM) Library  

This repository was forked on Januray 6, 2019 from slaclab/proto-dune-dam-lib.  DUNE intends to provide long-term support for this library in order to be able to read ProtoDUNE-SP data read out with the SLAC RCE hardware for the future.

Author: JJRussell  
Email  : <russell@slac.stanford.edu>

### Purpose
This provides C++ classes that access protoDUNE data as written by the RCEs. Currently it consists of one package **dam**, but, if necessary, future packages may be added.

### Release Information
Project wide release information is provided in the Release.md file that resides in the top level directory.  Detailed release information can be found in subdirectories all named **sdf/** (Software Development Folders).


### Quick Start 
THIS NEEDS TO BE WRITTEN.  

This truly an unfortunate state of affairs, since this is what is most useful.  I tried, but the current release has so many deficiencies that I was left with two choices  

- Document the current as is situation with all its warts. I started this but it was too embarrassing.  
- Fix the situation. Unfortunately this takes time to do and test that is not available now.
- So I did the logical thing, drop back 15 yards and punt.

### Organization
The code base is structured to recognize and address the different needs of three audiences and to shield these audiences from details needed only by the others. In order of the largest to smallest, these are

 - **Package users** - These are people who, for the most part, care only about compiling, linking and running against the package's products.
 - **Package builders** - These are people who check out the code, build it and make it available to the general users
 - **Package developers** - These are people actively developing and maintaining the code base.  

 #### Package Users
 These people generally only need to know three things

  - Where are the includes to compile against
  - Where are the shareables to link and run against
  - Where are the utility executables

 The scheme chosen is to mirror the Linux model of a *usr/include*, *usr/lib* and */usr/bin*. However to accomodate the fact that products of multiple platforms may reside in a shared file system, these directories live as subdirectories under a platform specific directory. A script is provided to setup environment variables that localize the platform for the user's current platform.  The setup script is in *scripts/settings.sh or settings.csh.*
   
	A platform = a machine architecture (*e.g.* x86_64) + a machine variant (*e.g.* supports AVX2) + an operating system (*e.g.e Linux, Darwin)
	
	#### Package Builders
	These people need to build the code's products and make it available to the *Package Users*.  This is accomplished by executing the make files found in the package subdirectories.  To do this an installation directory must be provided. The default is to place this under the root directory of the checked out code. 
	 
	**Note:** Currently the ability to separate the installation directory from the root directory is not supported.  *This is a deficiency that needs addressing.*
	
	**Note:** Currently one must execute the Makefile in each package's *make/* subdirectory.  *This is a deficiency that needs addressing.* There should be master make file that does the complete build.
	
	### Package Developers
	This is the smallest group. They are actively developing and maintaining the code. As such they need the most detailed knowledge.
 

### Directory Structure
The directory structure is the vehicle used to realize the goals of the organization philosophy.

The top level directory structure immediately under the **proto-dune-dam-lib** root directory consists of a few dedicated directories and the package specific directories.  

***Dedicated Directories***  
Under **proto-dune-dam-lib/** are  

>**Readme.md** - This file  
   
>**makeutils/** - Holds the make utility files used in building the package. The contents of this directory are generally not of interest to package developers or users.  
  
>**data/** - Holds sample data used for testings  
  
>**scripts/** - Scripts used to setup the necessary environment variables.  Currently the important ones are *LD_LIBRARY_PATH/DYLD_LIBRARY_PATH* (Linux/Mac) and *PATH*.  

>**install/** - Directory to hold the *proto-dune-dam-lib** installation products. These include the include files used to compile code and shareables used to link and run against.  

>Currently this directory resides in this directory tree.  This is defeciency that needs to be corrected. The installation directory should be chosen by end-user. 

  
Note: Because the package directories exist at the same level, they cannot be any of the above names.

 
***Packages***    
Packages contain three main subdirectories, **source/**, **binary/** and **export/** of which only **source/** is of general interest.  The remaining two contain the build products. These are considered emphemeral and not stored in the *GIT* repository.  For completeness  
>
>**\<pkg\>/binary/** - Stores the intermediate products *e.g.* dependency files and object files used in the build.  


>**\<pkg/\>export/** - Contains the final products which will be exported to the user's installation directory.

The source directory is main interest of package developers.

>**\<pkg\>/source/** - Currently there is only one package, **dam**, but all package directories follow a standard layout under this directory.  At the highest level are language specific subdirectories, *e.g.* **cc**, **python**, etc.   

>An example of *C* and **C++** layout follows.  For definitiveness, I will use the **dam** package as an example.


>>**cc/**  - Subdirectory holding all files relevant to *C* and **C++** code.
   
>>>**make/** - Contains the Makefile for this package's *C/C++* code.  

>>>**dam/** - Holds the include files. It uses the common convention of naming the include directory after the package so that include file syntax in code will appear in the code as   
  
>>>**\#include "dam/File.hh"**.   
  
>>>The files in this top level directory provide the high level interface that will be adequate for most users. In general, when compiling code outside users will access these files via the **install/** directory.  The following subdirectories are **dam** specific, but other packages are encouraged to follow a similiar convention. These subdirectories provide lower level access upon which the high level interface is built. 
    
>>>>**access/**  - Lower level, but still publically supported, interface include files. These are provided for debugging and diagnostics.  In some circumstances, because they provide a richer set of methods, may allow the user to produce higher performance code.  
  
>>>>**records/** - Defines the layout of the raw binary data. Even though these are in the publically accessible include directory, they are provided strictly for reference and  **should never** be included in any user code.  They are subject to change as the binary format evolves.  
 
>>>**ptd/** - Package test directory. Contains code used in unit tests of the package.  

>>>**src/** - Holds the C++ source code  

>>**python/**




