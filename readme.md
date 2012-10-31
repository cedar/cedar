# License

Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum,
Germany
 
This file is part of cedar.

cedar is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

cedar is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with cedar. If not, see <http://www.gnu.org/licenses/>.

---

# cedar

Welcome to cedar!

While being an open source library, cedar is developed primarily by the
Autonomous Robotics group at the Institut fuer Neuroinformatik,
Ruhr-Universitaet Bochum in Germany. If you would like to contact us, write an
email to cedar@ini.rub.de.

Our postal address is:

Autonomous Robotics Group  
Institut fuer Neuroinformatik  
Ruhr-Universitaet Bochum  
Universitaetsstr. 150  
D-44801 Bochum  
Germany

You can also reach us by phone under +49 234 32-28967.

The current release of cedar as well as documentation can be found on our [official website](http://cedar.ini.rub.de/). Our repositories and issue-trackers are hosted on [bitbucket](https://bitbucket.org/cedar).

# Installing cedar

## Install all dependencies
cedar comes with a bunch of dependencies on other libraries. Here is what you
have to install before you can compile our library. The version numbers are
the oldest versions supported.

* CMake
* Boost 1.47
* OpenCV 2.2
* Qt 4.6.2
* qwt 5.2.1
* qwtplot3d 0.3
* libqglviewer 2.3.6

optional dependencies:

* YARP 2.3.6 (for YARP features like network connectivity)
* libDC1394 (for firewire cameras)
* fftw-3.2.2 (for FFT-based convolutions)
* Doxygen (to generate HTML or LaTeX documentation)

### Ubuntu 12.04 and 12.10:
We provide a Debian meta-package, which installs all dependencies you need
to compile cedar under Ubuntu 12.04 and 12.10. The package can be downloaded
from our bitbucket page.

    wget https://bitbucket.org/cedar/dependencies/downloads/cedar-dependencies.deb

You will need a program like gdebi to install the package. gdebi is able to
install local Debian packages while taking into consideration its dependencies
- apt cannot do this at the moment. If you do not have gdebi installed, get it
now.

    sudo apt-get install gdebi

Now you can install all of cedar's dependencies using gdebi and the Debian
package you downloaded earlier.

    sudo gdebi cedar-dependencies.deb

As part of the installation, the Debian package will also download, compile,
and install a patched version of qwtplot3d (0.3), which sadly is no longer
available anywhere else on the web.


## Clone the cedar repository

There are two ways to get the cedar sources.

1. You can use Mercurial, a distributed version control system (similar to
Git). Install it through your package manager

        sudo apt-get install mercurial

    and then clone our official repository.

        hg clone https://bitbucket.org/cedar/stable [your cedar directory]

2. If you do not want to use Mercurial, you can simply download a tarball
containing the cedar sources from our
[bitbucket page](https://bitbucket.org/cedar/stable/downloads/)

        wget https://bitbucket.org/cedar/stable/get/tip.tar.gz

    unpack it

        tar xzf tip.tar.gz

    and rename the resulting directory to something a bit cleaner than the
    randomly generated name chosen by bitbucket

        mv cedar-stable-[random letters] [your cedar directory]

## Compilation

1. Change into the cedar directory.

        cd [your cedar directory]

2. Create a copy of cedar.conf.example (in this folder) and name it
   'cedar.conf'.

        cp cedar.conf.example cedar.conf

3. By default, `cedar.conf` is set up to work on Ubuntu 12.04 and 12.10. If you
   are on a different system, take a look at the file and make any changes for
   your individual system (e.g., installation prefix, external include paths)
   with an editor of your choosing (e.g., vim).

        vim cedar.conf

4. Create an out-of-source build folder and change into it:

        mkdir build
        cd build 

5. Call cmake to generate UNIX makefiles:

        cmake ..

6. Compile cedar. You can save some time by compiling in multiple threads in
   parallel. To split the compilation process into n threads, supply the
   parameter `-j n` to the following command:

        make

7. (optional) Run all unit tests to check whether everything works:

        make test

8. (optional) (only works when doxygen is installed) Create the documentation. It
   will be generated in the folder `build/doc`:

        make doc

   Note, that this may generate some warnings that you can usually ignore
   safely.
]

7. (optional) Install cedar

        sudo make install


## Play around with the processing framework

If you don't want to start coding right away, you can have a look at our
processing framework:

1. Go into the binary folder of the cedar repository:

        cd cedar/bin

2. Start the processing framework:

        ./processingIde

3. Now, drag and drop elements from the top onto the working area and connect
   them up.

   Have fun!