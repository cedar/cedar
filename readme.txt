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
#   Date:        2010 11 29
#
#   Description: Cedar readme file
#
#   Credits:
#
#=======================================================================================================================

[Insert warm welcoming message here.] :)

While being an open source library, cedar is developed by the Autonomous
Robotics group at the Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum
in Germany. If you would like to contact us, write an email to ar@ini.rub.de.

Our postal address is:

Autonomous Robotics Group
Institut fuer Neuroinformatik
Ruhr-Universitaet Bochum
Universitaetsstr. 150
D-44801 Bochum
Germany

You can also reach us by phone under +49 234 32-28967.

Compact installation instructions (for linux and MacOS):

1. Create a copy of cedar.conf.example (in this folder) and name it cedar.conf
2. Take a look at 'cedar.conf' and see if you would like to make any changes
   (e.g., installation prefix, external include paths, ...).
3. Create an out-of-source build folder and change into it:
   $ mkdir build
   $ cd build 
4. Call cmake to generate UNIX makefiles:
   $ cmake ..
5. Compile cedar:
   $ make # you can append '-j n' to split the compilation process into n threads
[Optional: Run all unit tests:
   $ make test
]
[Optional: Create the documentation:
   $ make doxygen
]
6a. For all Debian based Linux distributions: Create installation package and install it:
   $ make package
   $ sudo dpkg -i cedar-<version>-Linux.deb
6b. Manual installation:
   $ make install
