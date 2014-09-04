ofxCX
=====================================

Introduction
------------
ofxCX (hereafter referred to as CX) is a "total conversion mod" for [openFrameworks](http://www.openframeworks.cc) (often abbreviated oF) that 
is designed to be used used for creating psychology experiments. OpenFrameworks and CX are based on C++, which
is a very good language for anything requiring a high degree of timing precision. OpenFrameworks and CX are both
free and open source. See [this page](http://www.kylehardman.com/psychology/c-experiment-software) for a summary of the features of CX.

The documentation for CX can be found in the docs subfolder and in the code files. A generally fairly up-to-date pdf version of the documentation is contained in docs/CX_Manual.pdf (or try [this link](https://sites.google.com/site/kylehardmancom/files/CX_Manual.pdf?attredirects=0&d=1)). You can generate 
other formats of the documentation by using Doxygen with the Doxyfile given in the `docs` subfolder.

License
-------
This addon is distributed under the MIT license (see license.md).

Installation
------------
Drop the contents of this repository into a subdirectory directory under %openFrameworksDirectory%/addons 
(typically addons/ofxCX). See the manual for more installation information, like how to get openFrameworks 
and how to use the examples for CX.

Compatibility
------------
CX only works with oF version 0.8.0, so make sure you are using that version and not the latest version.

It has only been tested on Windows and Linux. It should work completely on OSx.

As far as compilers/IDEs are concerned, CX has been tested under Visual Studio 2012/2013 with the VS2012 compiler and Code::Blocks for Windows using GCC. On Linux, CX has been tested with Code::Blocks using GCC.

Known issues
------------
This is beta software, don't expect it to be bug-free. Please report any issues you have in the issue tracker.

There is a silly naming incompatibility on Linux, where an X11 header defines a symbol called Display. CX has a symbol called `CX::Instances::Display` that is used in all of the examples without the namespace qualifiers. As a result, the examples won't compile without aliasing the CX `Display` or adding the `Instances::` namespace qualifier. Aliasing is easily done with `CX_Display& Disp = CX::Instances::Display;` at the top of the examples and then renaming the CX `Display` in the examples to `Disp`.

Version history
------------
There has not yet been a versioned release of CX.
