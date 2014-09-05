Readme
======

# License

Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum,
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

You can also reach us by phone +49 234 32-25564.

The current release of cedar as well as documentation can be found on our
[official website](http://cedar.ini.rub.de/). Our repositories and issue trackers
are hosted on [bitbucket](https://bitbucket.org/cedar).

# Installing cedar

## Supported operating systems
For now, Ubuntu Linux is the only fully supported operating system for cedar. The
following manual for installing cedar is geared toward Ubuntu Linux and other
Debian-based Linux distributions. That being said, we have successfully
installed cedar on all major operating systems (other Linux distributions,
MacOS, and Windows). In a lot of cases, the installation is not as easy and you
will have to get creative here and there - but it is possible. Bear with us as
we try to make cedar more platform independent.

## Supported compilers
In general, we support GCC versions >= 4.4.5 (including MinGW ports) and MSVC versions >= 2012. 
You might also use Clang on Mac OS, but its support is provisional. As mentioned above, 
our main priority is the Linux operating system, so we cannot guarantee that compiling cedar 
on a different operating system / with a different compiler works out of the box. We are, however, 
able to help you with the most common problems when trying out non-Linux solutions, so just 
drop us a message if you are experiencing any problems.

## Install all dependencies

### Ubuntu

We provide a Debian meta-package, which installs all dependencies you need
to compile cedar under Ubuntu. For the versions 12.04, 12.10, and 13.04, the following package can be downloaded from our bitbucket page.

    wget https://bitbucket.org/cedar/dependencies/downloads/cedar-dependencies-ubuntu12.xx.deb

For version 13.10, you need a different package since the depending package names have changed.

    wget https://bitbucket.org/cedar/dependencies/downloads/cedar-dependencies-ubuntu13.10.deb

You will need a program like gdebi to install the package. gdebi is able to
install local Debian packages while taking into consideration its dependencies
- apt cannot do this at the moment. If you do not have gdebi installed, get it
now.

    sudo apt-get install gdebi

Now you can install all of cedar's dependencies using gdebi and the Debian
package you downloaded earlier.

    sudo gdebi cedar-dependencies.deb

As part of the installation, the Debian package will download, compile, and
install a patched version of *qwtplot3d* (0.3), which sadly is no longer
available anywhere else on the web. We are working on removing this dependency
from cedar but for now it is necessary for 3D plotting of data.

### Mac OS X 10.8.x & 10.9.x
We highly recommend using [homebrew](http://mxcl.github.com/homebrew/) to install the
above dependencies. Homebrew has recipes for all of cedar's dependencies
except for *qwtplot3d*. *qwtplot3d* does not compile on 10.9.x.

Your Clang version should be up to date, i.e. `Apple LLVM version 5.x (based on LLVM 3.3svn)` or newer. Just install the most recent XCode and you should be good to go.

#### Installing qwtplot3d on OS X 10.8.x
Download the source code of *qwtplot3d* from our
[repository](https://bitbucket.org/cedar/dependencies/downloads/qwtplot3d-0.3.0-patched.tar.gz)
then open a terminal and go to the folder containing the downloaded file. Run

    tar xvf qwtplot3d-0.3.0-patched.tar.gz

Switch into the extracted folder `qwtplot3d` and run

    qmake && make

followed by

    sudo make install

if *qwtplot3d* compiled successfully.

#### Link the frameworks
cedar currently does not support Mac frameworks. Therefore you have to create
symbolic links to the frameworks so that cedar can find the installed
dependencies. If you have installed the dependencies with homebrew, you can
simply copy the following statement, fill in the appropriate version numbers,
and execute it. Otherwise you may have to adapt the paths to fit your system
environment.

    sudo ln -s "/usr/local/Cellar/libqglviewer/[__VERSION__]/QGLViewer.framework" "/Library/Frameworks/QGLViewer.framework"
    
    sudo ln -s "/Library/Frameworks/QGLViewer.framework/QGLViewer" "/Library/Frameworks/QGLViewer.framework/libQGLViewer.dylib"
    
    sudo ln -s "/usr/local/Cellar/qwt/[__VERSION__]/lib/qwt.framework/Versions/6/Headers" "/usr/local/Cellar/qwt/[__VERSION__]/lib/qwt.framework/Headers"
    
    sudo ln -s "/usr/local/Cellar/qwt/[__VERSION__]/lib/qwt.framework/Versions/6/qwt" "/usr/local/Cellar/qwt/[__VERSION__]/lib/qwt.framework/qwt"

#### Do use homebrew
As afore-mentioned we highly recommend to use homebrew to install all of cedar's dependencies. If you do that installing cedar is a breeze. If you do run into trouble it's because your system isn't *ready to brew*. `brew doctor` will help you with that. Just enter `brew doctor` into your shell and follow the instructions until the *doctor* tells you that your system is *ready to brew*.

You may not find all dependencies you need in the default homebrew formula repository, you will have to `brew tap homebrew/sciences` and maybe `brew tap homebrew/versions`. Usually homebrew can help you to find what you need, please refer to the [homebrew manual](https://github.com/Homebrew/homebrew/wiki) for that.

Also be attentive to which version of a dependency you install. A common pitfall is that `brew install vtk` will install vtk 6.x while we recommend to use vtk 5.10 which has a separate formula `vtk5` which will not show up until you have tapped `homebrew/versions`. An easy way to find out the version a formula will install is to call `brew info [formula-name]` without the brackets.

### Other operating systems

If you are using a different operating system, unfortunately you will have to
install all dependencies manually, using either the package manager of your
system or compiling the dependencies from source. Here is what you have to
install before you can compile cedar. The version numbers are the oldest
versions supported.

* CMake
* Boost >= 1.47 (except version 1.49, which apparently does not work with c++11)
* OpenCV >= 2.2
* Qt >= 4.6.2 (Qt5 may work, but is not yet supported officially)

optional dependencies:

* qwt 5.2.1
* libqglviewer 2.3.6
* YARP 2.3.6 (for YARP features like network connectivity)
* libDC1394 (for firewire cameras)
* fftw-3.2.2 (for FFT-based convolutions)
* Doxygen (to generate HTML or LaTeX documentation)
* qwtplot3d 0.3 (to display surface plots)
* VTK 5.6 (alternative library for surface and line plots)

We depend on a particular version of qwtplot3d, which we had to patch to
fit our needs. You can get the
[source code](https://bitbucket.org/cedar/dependencies/downloads/qwtplot3d-0.3.0-patched.tar.gz)
of this library from our dependencies repository.

To use VTK you need to install the VTK Library with Qt support and the C++ Header files. The latter is usually included in a VTK-Dev package. The former can be achieved by building VTK from source with Qt-Support enabled or by installing a VTK and a VTK-Qt package, given your package manager provides one. If you were to use apt-get for instance, you'd have to install the following packages:
* libvtk5.8
* libvtk5.8-qt4
* libvtk5-dev
* libvtk5-qt4-dev

## Clone the cedar repository

There are two ways to get the cedar sources.

1. You can use Mercurial, a distributed version control system (similar to
Git). Install it through your package manager

        sudo apt-get install mercurial

    and then clone our official repository. Replace [your cedar directory] with
    a meaningful name for the folder in which the repository will be cloned
    (e.g., `cedar` or `cedar.release`)

        hg clone https://bitbucket.org/cedar/release [your cedar directory]

2. If you do not want to use Mercurial, you can simply download a tarball
containing the cedar sources from our
[bitbucket page](https://bitbucket.org/cedar/release/downloads/)

        wget https://bitbucket.org/cedar/release/get/tip.tar.gz

    unpack it

        tar xzf tip.tar.gz

    and rename the resulting directory. In the following command, replace
    [random characters] with whatever the uncompressed tarball is called (it is
    randomly generated by bitbucket). Replace [your cedar directory] with a
    meaningful name for the folder (e.g., `cedar` or `cedar.release`).

        mv cedar-release-[random characters] [your cedar directory]

## Compilation

1. Change into the cedar directory.

        cd [your cedar directory]

2. Create a copy of cedar.conf.example (in this folder) and name it
   `cedar.conf`.

        cp cedar.conf.example cedar.conf

3. By default, `cedar.conf` is set up to work on Ubuntu 12.04 and 12.10. If you
   are running Ubuntu, skip this step. If you are running a different system,
   take a look at the file and make any changes for your individual system
   (e.g., installation prefix, external include paths) with an editor of your
   choosing (e.g., vim).

        vim cedar.conf

4. Create an out-of-source build folder and change into it:

        mkdir build
        cd build 

5. Generate makefiles specific for your platform to be able to compile cedar:

        cmake ..

6. Compile cedar. You can save some time by compiling in multiple threads in
   parallel. To split the compilation process into n threads, add the
   optional parameter `-j n` to the following command:

        make [-j n]

7. (optional) Run all unit tests to check whether everything works:

        make test

8. (optional) (only works when doxygen is installed) Create the documentation.
    It will be generated in the folder `build/doc`:

        make doc

    Note that this may generate some warnings that you can usually ignore
    safely.

9. (optional) Install cedar

        sudo make install


## Play around with the processing framework

If you don't want to start coding right away, you can have a look at our
processing framework:

1. Go into the binary folder of the cedar repository:

        cd [your cedar directory]/bin

2. Start cedar's GUI:

        ./cedar

3. Now, drag and drop elements from the top onto the working area and connect
   them up.

   Have fun!
   
## Reporting issues

Our repositories and issue trackers
are hosted on [bitbucket](https://bitbucket.org/cedar).

We always welcome any features, bug-reports and other things.

Note: we try to be warning-free. If you encounter warnings when compiling
cedar on your system, let us know!