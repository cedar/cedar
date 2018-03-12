Troubleshooting
===============

 <!--
=======================================================================================================================

   Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
   This file is part of cedar.

   cedar is free software: you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation, either version 3 of the License, or (at your
   option) any later version.

   cedar is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with cedar. If not, see <http://www.gnu.org/licenses/>.

=======================================================================================================================

   Institute:   Ruhr-Universitaet Bochum
                Institut fuer Neuroinformatik

   File:        troubleshooting.md

   Maintainer:  all cedar developers
   Email:       cedar@ini.rub.de
   Date:        2013 08 19

   Description: Cedar troubleshooting

   Credits:

=======================================================================================================================
-->

Installing cedar
----------------

* **Which compilers are supported?**

> In general, we support GCC versions >= 4.4.5 (including MinGW ports), recent versions of Clang, and MSVC versions >= 2010. As mentioned above, our main priority is the Linux operating system, so we cannot guarantee that compiling cedar on a different operating system / with a different compiler works out of the box. We are, however, able to help you with the most common problems when trying out non-Linux solutions, so just drop us a message if you are experiencing any problems.

* **When running cmake, I get errors about a missing OpenCV find package routine.**

> Make sure that CEDAR_OPENCV_CMAKE_DIR in your cedar.conf file points to the proper directory. This is usually the build directory from which you called cmake for building opencv. Using precompiled versions of OpenCV doesn't seem to work because the relevant files don't seem to be shipped with them.

* **CMake does not find one of the libraries cedar depends upon, even though that library is installed. What should I do?**

> Make sure to put the right paths to the installed libraries in cedar.conf. cedar tries to find both includes and libraries by matching some of the required files (for example, some important header files, which exist since a certain release of an external library). You may also encounter problems with different names for shared objects.

* **I am having troubles with boost during compilation. What should I do?**

> Please check if your boost version matches our minimal requirement 1.47. There is also an issue with version 1.49 and C++11, which we cannot fix since this appears to be a bug in boost itself. If you have an older version of boost or version 1.49, try switching to a newer version (1.50+) and check that cedar finds this particular version and not the old one.

* **During linking, I get unresolved references to some dependencies such as OpenCV.**

> This is most likely caused by having multiple versions of the same library on your system. This happens for example on Linux machines, if there is an old version of (insert any library) installed via apt-get, and a second, more recent one was manually compiled. Please make sure that CMake has found both includes and run-time libraries of the same version. On Windows, pre-compiled versions of dependencies (the ones you get out of installation packages) may be compiled with a different compiler (version) than the one used for cedar. Please try to compile those dependencies from source with the matching compiler.

* **If I do *make doc*, creation of the documentation fails due to a missing refman.tex.**

> This is a known bug in your Doxygen version. Use the html-doc instead or upgrade your Doxygen.


* **Some of the unit tests fail. Help!**

> There may be several causes for this. Unfortunately, unit tests do not work well on Windows. Also, if you see a lot of unit tests with the gui prefix failing, this may be because you are not in an environment where GUIs can be opened. If you are using ssh, try adding -X.

Using cedar
-----------

* **I'm using cedar together with plugins. Why does the GUI crash on start-up or during use (e.g., loading an architecture)?**

> The plugins are most likely compiled or linked to a different version of cedar. You can disable loading the plugins by using a command line option --no-plugins (see --help). If your GUI now starts up without crashing, you should recompile all plugins using your current cedar version. In some cases, outdated plugins are loaded without crashing and may lead to crashes later on while using or shutting down cedar. In this case, please also recompile your plugins everytime you update your core cedar.

* **When using 3D neural fields, exceptions occur once the triggers are started.**

> You might want to check if the convolution engine of this field is set to FFTW. Since FFTW is an optional depedency, it might not occur in the list of engines. If it is set to FFTW, check that the field size in each dimension is larger than or equal to the kernel size (with kernel size being *limit times sigma* or *limit times sigma + 1* if the first number is even).
