# MIPS-VM
A MIPS processor simulation written in C++ using Qt5

## Build
This program uses CMake as the build tool. In-source builds are not allowed.  Execute the following sequence from the command line:
```
cd src
mkdir build && cd build
cmake ..
make
```

## Run
Usage: `simmips --gui <input file>` for the Qt5 version, or `simmips <input file>` for the CLI

![MIPS VM Screenshot](https://github.com/anthvasquez/MIPS-VM/blob/master/MIPS%20VM%20Demo.png)

Code uploaded from Spring 2018.
