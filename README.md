An Automatic Matrix Differentiation Library
===========================================

Overview
--------

We have started implementing our ideas in C++ on
[GitHub](https://github.com/pkambadu/AMD). We first started off by
implementing symbolic matrix differentiation. Currently, we are in the
process of extending this facility for distributed matrices (using
[Elemental](http://libelemental.org)). As we progress in our
implementation, we will add more information here. 

As with most software, AMD can be built with a variety of options. The
bare bones version uses symbolic matrices and has three dependencies:

C/C++ compiler.

CMake (version 2.8 or higher).

Boost (version 1.55 or higher).

-   random,  shared\_ptr, program\_options

The second option is to build AMD with support for real matrix
(Elemental matrices) support. For this, the additional requirements are:

-   C++ compiler with C++-11 support (eg., GCC version 4.8 or higher) 
-   MPI (strong preference for MPICH2)
-   BLAS and LAPACK
-   Elemental (Version 0.83 or higher)

Building documentation requires:

Doxygen (reasonably recent version)

Latex tools

-   latex, dvips, ps2pdf, makeindex

Building python bindings for AMD requires:

Python (version 2.7 or higher)

-   numpy, scipy, swig, mpi4py

### Installation Instructions

It is nearly impossible to give complete installation instructions for
all the software that we use. Fortunately, all of the software
dependencies listed above have their own webpages with detailed
installation instructions. However, we attempt to provide a simplified
set of instructions for a typical system --- [Ubuntu
12.04](http://releases.ubuntu.com/12.04/) --- that will run AMD.

### GCC

For the bare bones installation, the system default C and C++ compilers
are sufficient. If Elemental support is needed, we need a compiler that
supports the C++-11 standard. We now outline how to install C++-11
compliant compiler. It is typical for most users to use the
[GCC](http://gcc.gnu.org) C++ compiler suite. Some of AMD's
 dependencies (like Elemental) use C++-11 language features, which are
enabled only in version 4.8 or higher. On 12.04, the default compilers
available on the repositories are 4.6.  Therefore, the first step is to
add the relevant Personal Package Archives (PPA). 

-   Add the PPA to your local system using the following commands:



`# sudo apt-get install python-software-properties`

`# sudo add-apt-repository ppa:ubuntu-toolchain-r/test`

`# sudo apt-get update`

-   Install GCC 4.8 and make gcc-4.8 the default gcc:

`# sudo apt-get install gcc-4.8 gfortran-4.8 g++-4.8`

`# sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50`



Alternately, you can compile GCC from its sources in the following
manner:

-    Download the source code.

`# wget http://www.netgull.com/gcc/releases/gcc-4.8.2/gcc-4.8.2.tar.bz2`

-   Unzip GCC package.

`# tar -xvjf <file name>`

-   Install dependencies.

`# sudo apt-get install libgmp-dev libmpfr-dev libmpc-dev libc6-dev gcc-multilib`

-   Add /usr/lib32 to LIBRARY\_PATH

`# export LIBRARY_PATH=/usr/lib32:$LIBRARY_PATH`

-   Build and install GCC

`# cd gcc-4.8.2 # mkdir build`

`# cd build`

`# ../configure --prefix=<installation path>`

`# make `

`# make install`

### MPI

If Elemental support is desired, we need MPI. We recommend installing
[MPICH2](http://en.wikipedia.org/wiki/MPICH2), although any MPI-2
compliant software should be sufficient. You can install MPI following
the instructions:



`# sudo apt-get install -y libcr-dev`

`# sudo apt-get install -y mpich2 libmpich2-dev mpich2-doc`

### Python

If python bindings are desired, we need relevant python libraries along
with the python interpreter. AMD requires python 2.7 or higher. In
addition, we also need fairly recent versions
of [scipy](http://ww.scipy.org), [numpy](http://www.numpy.org), [mpi4py](http://mpi4py.scipy.org),
and [swig](http://www.swig.org). We also recommend
installing [ipython](http://ipython.org). Make sure that you have
installed MPI on your system before installing mpi4py. You can install
all these requirements by running the following commands:



`# sudo apt-get install -y python-dev`

`# sudo apt-get install -y python-setuptools`

`# sudo apt-get install -y python-numpy python-scipy python-numpy-doc`

`# sudo apt-get install -y swig swig-doc swig-examples`

`# sudo apt-get install -y python-mpi4py`

`# sudo apt-get install -y ipython`



Note that MPI also needs to be installed for mpi4py to function.

### CMake

[CMake](http://www.cmake.org/) is an open-source, cross-platform build
system that is routinely used in many open source software. We (and
Elemental) need CMake 2.8 or higher. For example, you can install CMake
2.8.11.2 following the instructions:

`# wget http://www.cmake.org/files/v2.8/cmake-2.8.11.2.tar.gz`

`# tar -xvfz cmake-2.8.11.2.tar.gz`

`# cd cmake-2.8.11.2`

`# mkdir build`

`# cd build`

`# ../bootstrap --prefix=<installation path>`

`# make`

`# make install`



Please make sure that the installation path is in your PATH environment
variable.

### Boost

[Boost](http://www.boost.org/) is a set of libraries for C++
programming. If only a base installation of AMD is needed, we need only
boost::shared\_ptr, boost::program\_options, and boost::random
libraries. If Elemental support is required, we need boost::mpi. If
python bindings are needed, then boost::python is also needed. Note that
AMD requires Boost version 1.55.0 or higher. You can install boost
following the instructions:

`# wget http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz`

`# tar xvfz boost_1_55_0.tar.gz`

`# cd boost_1_55_0`

`# ./bootstrap.sh --with-libraries=program_options,mpi,python,random --prefix=<installation path> `

`#echo "using mpi ;" >> project-config.jam`

`# ./b2 link=static,shared`

`# ./b2 install --prefix=<installation path>`



Note that in the instructions above, we have left out boost::shared\_ptr
because it is a header-only library.

### BLAS and LAPACK

[BLAS](http://www.netlib.org/blas/)and
[LAPACK](http://www.netlib.org/lapack/) are APIs for linear algebra
routines. There are several free and non-free software that implement
the  BLAS and LAPACK API. For simplicity, we recommend installing the
default version which comes free with most distributions using the
following commands:



`# sudo apt-get install -y libatlas3gf-base `

`# sudo apt-get install -y libatlas-dev `

`# sudo apt-get install -y liblapack-dev`



If you are really after performance, consider installing something like
Intel's MKL or IBM's ESSL.

### Elemental

[Elemental](http://libelemental.org/) is an open-source library for
distributed-memory dense linear algebra that wraps around BLAS and
LAPACK. We recommend installing version 0.83 or greater:

`# wget http://libelemental.org/pub/releases/elemental-0.83.tgz`

`# tar xvfz elemental-0.83.tgz`

`# cd elemental-0.83`

`# mkdir build`

`# cd build`

`# cmake ../ -DUSE_SWIG=on -DCMAKE_INSTALL_PREFIX=<installation path>`

`# make `

`# make install`

`# cp *.py ${HOME}/.local/lib/python2.7/dist-packages/`

`# cp _*.so ${HOME}/.local/lib/python2.7/dist-packages/`

### Doxygen

If you want to build documentation for AMD, then it is required to
install doxygen. Doxygen can be installed using the following
instruction:



`# sudo apt-get install doxygen`

### Latex tools

If you want to build documentation for AMD, then we need some latex
tools that can be installed using texlive-latex packages



`# sudo apt-get install texlive-latex-base`

`# sudo apt-get install texlive-latex-recommended`

`# sudo apt-get install texlive-latex-extra`

Downloading and building AMD
----------------------------

Currently, AMD can perform symbolic matrix differentiation. Shortly, we
will add functionality to differentiate real-valued distributed
Elemental matrices. Depending on your use, there are different targets
that can be enabled on the command line while configuring:

1.  For real matrices, please add `-DUSE_ELEMENTAL=on`.
2.  `For test cases, please add -DBUILD_TESTS=on`.
3.  `For documentation, please add -DBUILD_DOCS=on`.
4.  `For example, please add -DBUILD_EXAMPLES=on`.
5.  `For tutorial, please add ``-DBUILD_TUTORIAL=on`.

AMD is under active development and there is no release candidate as
yet. You can download and build the AMD software using the following
instructions:

`# git clone https://github.com/pkambadu/AMD`

`# cd AMD`

`# mkdir build`

`# cd build`

`# cmake ../ -DBUILD_TESTS=on`

`# make`



If Elemental has been installed in a non-default location, please set
the environment variable ELEMENTAL\_ROOT to point to the installation.
Similarly, if Boost has been installed in a non-standard location,
please set the environment variable BOOST\_ROOT before running cmake.
For example,



`# cmake ../ -DELEMENTAL_ROOT=<install-prefix> -DBOOST_ROOT=<install-prefix> <other-options> ../`

### Testing the installation

There is three test cases and one Symbolic Calculator in 
in `AMD/build/tests/`. Please run the
test cases to see if there are any error messages.

The Symbolic Calculator can be used as 
`./SymbolicCalculator 'expression' row=? col=?`

For example:
`./SymbolicCalculator 'trace(A*trans(X))' row=4 col=4`



