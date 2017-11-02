### Release Notes

#### Overview
These are the project wide release notes.  More detailed release information can be found in subdirectories placed in context/package specific directories. By convention all are named **sdf/** for Software Development Folders to make finding and recognizing them fairly easy.

A list of deficiencies is included here in the hopes that the next release will address them.  My fear is that this will become a wish list (which is encouraged) but will result in this list getting longer, not shorter (which is not good).

####Deficiencies
To a large extent, these are the following set are needed to support being able to place the **install/** directory in a user specified place.  Currently, this directory is fixed to live in the top level project directory.

1. Add support to the make files to arbitrarily place the **install/** directory.
2. Make the include files in the **install/** directory copies, not symbolic links to the original files.
3. Make the actual shareable in the **install/** directory a copy, not a synbolic link.
4. Formalize support for multiple versions in the **install/** directory.
5. The setup script, currently in **scripts/settings.sh/csh** needs to have a copy placed in the **install/** directory.
6. A method to locate the include directory for use when compiling user code is needed.  Basically something to set -I <project-include-path>. This reduces the burden on the user to knowing only where his/her **install/** directory is located.  This could be as easy as setting an environment varible (PROTO-DUNE-DAM-INC), but want to think this through a bit more.
7. Add visibility attributes to trim the exposed symbols to the public interface.

In the **dam** package

1. The AVX support is now just the generic code.  An **AVX** optimized version needs to be written.

---
##### VERSION : 0.0.2
##### RELEASED: 2017.11.02
##### By      : JJRussell
---

#### Summary
This release is primarily directed to address the needs of the offline.  Online usage should remain the same.

1. Added support for the MAC. 
2. Changed many symbolic links to be relative rather than absolute.
3. Changed names of executables that were far to generic

#### Details
##### MAC Support
Added support for builds on the MAC.  Surprisingly, there were almost no changes because of this to the package specific (here **dam**) Makefile.  Not unsurprisingly, the files supporting the make in **makeutils/** where heavily munged.

The setup script in **scripts/settings.sh/.csh** were modified to work on the MAC.  DYLD_LIBRARY_PATH is set on the MAC. There were a number of changes needed to handle MAC specific details (like *readlink -f* does not exist).

##### Symbolic Links
The symbolic links implementing versioned library support were, in the appropriate places, changed from absolute to relative links.  This makes it much easier to move the subdirectory containing the versioned library to a new location.

##### Name Changes
There were name changes because of a very poor inital name. They were far too generic to be exposed by placing them in the search path.

  1. reader -> PdReaderTest  
  1.  wibFrame_test -> PdWibFrameTest


---
##### VERSION : 0.0.1
##### RELEASED: `2017.10.23`
##### By      : JJRussell
---

This comes much closer to a production release.  This is still a **0.x.x**
release meaning it is still evolving.  I am not confident that I have
isolated the user from anticipated changes.  It may take one more round
and a little experience.


#### SHAREABLES
Where appropriate, packages produce 3 shareables

1. One for generic x86 64-bit architectures
2. One for those CPUs with AVX support
3. One for those CPUS with AVX2 support


##### SETUP SCRIPTS
A setup script, scripts/settings.csh or settings.sh will setup PATH
and LD_LIBRARY_PATH to select the most efficient implementation for
that executing platform.



