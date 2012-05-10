#=======================================================================================================================
#
#   Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
# 
#   This file is part of cedar.
#
#   cedar is free software: you can redistribute it and/or modify it under
#   the terms of the GNU Lesser General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   cedar is distributed in the hope that it will be useful, but WITHOUT ANY
#   WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
#   License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with cedar. If not, see <http://www.gnu.org/licenses/>.
#
#=======================================================================================================================
#
#   Institute:   Ruhr-Universitaet Bochum
#                Institut fuer Neuroinformatik
#
#   File:        readme.txt
#
#   Maintainer:  Mathis Richter
#   Email:       mathis.richter@ini.rub.de
#   Date:        2011 11 12
#
#   Description: Cedar readme file
#
#   Credits:
#
#=======================================================================================================================


#=======================================================================================================================
# CEDAR
#=======================================================================================================================

Welcome to cedar!

While being an open source library, cedar is developed primarily by the Autonomous
Robotics group at the Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum
in Germany. If you would like to contact us, write an email to cedar@ini.rub.de.

Our postal address is:

Autonomous Robotics Group
Institut fuer Neuroinformatik
Ruhr-Universitaet Bochum
Universitaetsstr. 150
D-44801 Bochum
Germany

You can also reach us by phone under +49 234 32-28967.

Our website can be found here:
http://cedar.ini.rub.de/

Our repositories are hosted on bitbucket:
https://bitbucket.org/cedar

There, you can also find our issue-tracker (look under testing).

#=======================================================================================================================
# INSTALLING CEDAR
#=======================================================================================================================

=== Install all dependencies ===

cedar comes with a bunch of dependencies on other libraries. Here is what you
have to install before you can compile our library. The version numbers are
the oldest versions supported, unless noted otherwise.

   * CMake
   * Boost 1.47
   * OpenCV 2.2
   * Qt 4.6.2
   * qwt <= 5.2.1 (no newer versions)
   * qwtplot3d 0.3
   * libqglviewer 2.3.6
   * libconfig 1.4
   * Doxygen 

optional dependencies:
   * YARP 2.3.6 (for YARP features like network connectivity)
   * libDC1394 (for firewire cameras)

=== Clone the cedar repository ===

hg clone https://bitbucket.org/cedar/testing [your cedar directory]


=== Compilation ===

1. Change into the cedar repository.
   $ cd [your cedar directory]
2. Create a copy of cedar.conf.example (in this folder) and name it 'cedar.conf'.
3. Take a look at 'cedar.conf' make any changes for your individual system
   (e.g., installation prefix, external include paths, ...) with an editor of your
   choosing (e.g., vim).
   $ vim cedar.conf
4. Create an out-of-source build folder and change into it:
   $ mkdir build
   $ cd build 
5. Call cmake to generate UNIX makefiles:
   $ cmake ..
6. Compile cedar:
   $ make # you can append '-j n' to split the compilation process into n threads
[Optional: Run all unit tests to check whether everything works:
   $ make test
]
[Optional: Create the documentation (it will be generated in the folder 'build/doc'):
   $ make doc
   Note, that this may generate some warnings that you can usually ignore safely.
]
7a. For all Debian based Linux distributions: Create installation package and install it:
   $ make package
   $ sudo dpkg -i cedar-<version>-Linux.deb
7b. Manual installation:
   $ make install


=== Play around with the processing framework ===

If you don't want to start coding right away, you can have a look at our processing framework:

1. Go into the binary folder of the cedar repository:
   $ cd cedar/bin
2. Start the processing framework:
   $ ./processingIde
3. Now, drag and drop elements from the top onto the working area and connect them up.
   Have fun!
