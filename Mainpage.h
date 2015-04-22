/**
@mainpage
This C++ library is being developed as a collection of methods and tools for improving OCR post-correction. Currently the library is developed in the context of the IMPACT project (http://www.impact-project.eu). 

- {@link installation Installation}: Instructions for the installation of the library.
- {@link profiler_manual Profiler}: Look at the {@link profiler_manual Profiler Manual} to find out about why and how you want 
  to use this module.



<p>
Though running quite stable in many tests, the library is constantly under construction and development.<br>
<b>So far, none of the interfaces documented here are fixed or guaranteed to remain unchanged in future versions. Both software and documentation are not complete in some places.</b><br>
</p>
<p>Feedback, bug reports or suggestions are always appreciated. Please mail to uli@cis.uni-muenchen.de</p>

@version 0.1
@author Ulrich Reffle
@date 2009


@image html "doc/impact_logo.jpg"


@page installation Installation


To compile the OCRC library and all command line tools, you need the build
program 'cmake', which is part of virtually any linux distribution.

First, install the 'csl' library. It contains crucial classes used by OCRC. 

Then, unpack the source tarball and change into its root directory. Then type:

@code
$ mkdir build
$ cd build
$ cmake -DCSL_INCLUDE_DIR=<path-to-csl> -DCSL_LIBRARY=<path-to-csl-lib> ..
@endcode
The last command must contain the correct paths to your csl build.

Now the command

@code
make
@endcode

should build the library in ./lib and all executables in ./bin .

Also,
@code
make install
@endcode
installs the most important components to the directory specified by CMAKE_INSTALL_PREFIX. 
You can set this variable with another -D flag to cmake above, or using the configuration tool
'ccmake'. However the maintenance of what to install is somehow neglected at the moment.


 */
