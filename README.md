![Build Status](https://github.com/n0f4ph4mst3r/SimpleFTP/actions/workflows/ccpp-windows-x64.yml/badge.svg)
![Build Status](https://github.com/n0f4ph4mst3r/SimpleFTP/actions/workflows/ccpp-ubuntu-gcc.yml/badge.svg)
![Build Status](https://github.com/n0f4ph4mst3r/SimpleFTP/actions/workflows/ccpp-ubuntu-clang.yml/badge.svg)
![Build Status](https://github.com/n0f4ph4mst3r/SimpleFTP/actions/workflows/ccpp-macos-gcc.yml/badge.svg)
![Build Status](https://github.com/n0f4ph4mst3r/SimpleFTP/actions/workflows/ccpp-macos-clang.yml/badge.svg)
![Github Releases](https://img.shields.io/github/v/release/n0f4ph4mst3r/SimpleFTP)

# SimpleFTP
Description
-----------
A simple FTP client for working with files.

**IMPORTANT!** The version is unstable, so use it to work with small files/directories. Make sure your server supports the [RFC 2428](https://www.rfc-editor.org/rfc/rfc2428.html) and [RFC 3659](https://www.rfc-editor.org/rfc/rfc3659) specifications (in particular, commands such as *EPSV* and *MLSD*).

Build
-----
## Prerequisites

* **cmake** >= 3.26.0
* **vcpkg** more recent than 2023-02-9 (e.g. commit id [8e986ec51ae9728ff44b854390eef168f88ec687](https://github.com/microsoft/vcpkg/commit/8e986ec51ae9728ff44b854390eef168f88ec687))
* **A working compiler supporting C++ 20 functions



### Cmake

On Windows, please use the [prebuilt binaries](https://cmake.org/download/). Ensure you select one of the options to add cmake to the system path.

On Linux, `cmake` is usually available from the system package manager. Alternatively, `sudo pip3 install cmake` can be used to install the latest version of CMake.

### Vcpkg
Clone the repo with the `--recurse-submodules` flag

    git clone --recurse-submodules https://github.com/n0f4ph4mst3r/SimpleFTP
	
Run `./vcpkg/bootstrap-vcpkg.sh` or `.\vcpkg\bootstrap-vcpkg.bat`.

## Building

### Windows
I build using Microsoft Visual Studio 2019. However, it is likely possible to build it with Visual Studio 2022.

### Linux
I use GCC 9, but any compilers supporting C++ 20 functions should work.

#### Building on Windows

1. Clone this repository from GitHub project.

For example, in the git-bash run:

    git clone --recurse-submodules https://github.com/n0f4ph4mst3r/SimpleFTP
	
2. Run `.\vcpkg\bootstrap-vcpkg.bat`.

3. Configure project using CMake:

       $ mkdir build && cd build
       $ cmake .. -G "Visual Studio 16 2019"
    
4. Open Visual Studio project:

       $ .\\SimpleFTP.sln
    
    and build using the IDE.
	   

#### Linux & Other OS

1. Clone repository from GitHub project.

       git clone --recurse-submodules https://github.com/n0f4ph4mst3r/SimpleFTP
	
2. Run `./vcpkg/bootstrap-vcpkg.sh`.
    
3. Configure project using CMake:

       $ mkdir build && cd build
       $ cmake .. -G "Unix Makefiles" 
	   
   `Ninja` generator working too:

       $ cmake .. -G "Ninja" 
	   
	Use `-DCMAKE_BUILD_TYPE=Debug` for debug configuration.
  
4. Install project
   
   Makefiles:

       $ make install
	   
   Ninja:
   
	   $ ninja install
    
5. Test the build:

       $ cd ../bin
       $ ./ClientApp