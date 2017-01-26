Documentation of Code Style Issues
==================================

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

   File:        IssueDocumentation.md

   Maintainer:  Oliver Lomp
   Email:       oliver.lomp@ini.ruhr-uni-bochum.de
   Date:        2013 12 16

   Description: 

   Credits:

=======================================================================================================================
-->

Here, you can find more detailed information about the issues printed by the automatic code check script.


Compattibilty Issues
====================

C0001 - Using a std class without including the header
------------------------------------------------------

On newer versions of clang/Mac OS, a STL implementation different from the one in gcc is used. Thus, some headers are no
longer included indirectly. In order to avoid compilation problems on Mac OS, all STL headers for used classes should be
included directly.

C0002 - Using boost::shared_dynamic_cast
----------------------------------------

This function was removed in newer boost versions. Its replacement is boost::dynamic_pointer_cast. shared_dynamic_cast
can be replaced by dynamic_pointer_cast directly without any other changes and works equivalently.

C0003 - Guarding boost includes
----------------------------------------

When using certain versions of Qt and boost, there is an incompatibility of certain macros defined in these libraries.
These are issues we cannot fix directly; instead, we need to work around them by guarding boost includes with
#ifndef Q_MOC_RUN. The correct version should look like this:

    #ifndef Q_MOC_RUN
        #include <boost/signals2/signal.hpp>
    #endif


Miscellaneous Issues
====================

M0002 & M0002 - Outdated copyright information
----------------------------------------------

All files should have all consecutive years since 2011 in their copyright header in a fixed format, e.g., Copyright
2011, 2012, 2013 ...