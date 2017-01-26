List of changes
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

   File:        changelog.md

   Maintainer:  all cedar developers
   Email:       cedar@ini.rub.de
   Date:        2012 01 12

   Description: Cedar changelog

   Credits:

=======================================================================================================================
-->

Below, you can find a list of current changes of cedar. Please keep in mind that we try to keep our interfaces as stable as possible. This doesn't mean the software is crash- or bug-free.

Currently, there are still a few classes and frameworks that are under development. Here's a list of interfaces that are
likely to undergo major changes:
- everything in devices
- the class CoordinateTransformation in processing::steps

known issues
============
  - On Windows, some of the GL drawing functionality for kinematic chains and related classes is not implemented.
  - Using multiple 3D convolutions in separate threads slows down all involved convolutions
  - Some of the interactive CAREN tests may not run if you do not have the corresponding meshes.
  - Using the field plot and switching dimensionality might lead to messed up plots. Please reopen the field plot.
  - Singleton-related classes may sometimes cause crashes when programs exit. If you experience random crashes when your
    own programs exit and the stack contains cedar::aux::Log::getMemoryDebugFlag(), this may be the cause. As a
    workaround, make sure to reset all smart pointers at the end of your main method (the usual cause are global-scope
    smart pointers that send a log message after the log singleton was freed.)
  - In rare cases, moving elements into groups breaks the triggering chain, which leads to steps not being iterated
    correctly. This can be fixed by saving and loading the architecture. If you encounter such a (reproducible)
    situation, please let us know.
  - Single elements cannot be loaded as linked templates into architectures. They are simply added to the architecture.
  - When a net reader step revalidates its output while the architecture is running, this can lead to connections
    temporarily flashing in different colors. This can take a while, depending on how complicated the revalidation
    process is, but is normal. Connections also sometimes are blue for a short time.
  - When net readers and writers are in the same architecture, this can sometimes lead to crashes. This *seems* to be a
    YARP issue.
  - cedar::proc::Group writes and reads the bool parameter "isLooped", which is no longer needed. This will be disabled
    in the next major release.

Unreleased
==========
Nothing here. This is a released version.


Released versions
=================

The following are the changes made in the release versions.

Version 6.0.0
-------------

- Robotics Components:
  - The new robotics framework unifies the communication with hardware in the new Component class. A corresponding Component step allows for easy and standarized interaction inside the GUI, thus allowing you to switch your robot component (or switch from hardware to simulation).
  - Configuration of robot hardware is stored in the JSON format in the ressources folder
  - GUI: connect to your Robot in the new Robot manager
  - GUI: create a Component step and link it to the desired hardware Component (of your robot)
  - code: The Component class handles the buffering of incoming and outgoing data and threaded processing of it.
  - code: Children of Components implement their variations of accessing the Component buffers. Typical Children are: KinematicChain
  - code: The Robot class now acts as a container for Components. the way to intstantiate a component and robot is now:

    auto robot = boost::make_shared< cedar::dev::Robot >;
    robot->readJson("resource://robots/myrobot/myconfiguration.json");
    auto arm = robot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");

  - code: The children of the Channel class implement the methods talking to the hardware. There is only one Channel type of a kind in any Robot. Typical channels are: simulation, serial.


Version 5.1.0
-------------
- cedar
  - fixed issues with compatibility with opencv 2 and 3
  - fixed some issues under OSX

- tools
  - overhaul of the data processing tool (plotting of recorded data)
  - image database improvements 
  
- cedar::proc
  - Architecture scripts can now set a status that is displayed in the script manager.
  - Architecture widgets can now add buttons for calling step actions.
  - When steps go into warning/error states, the GUI now indicates this in the status bar.
  - Deprecated the Noise source - it is misleading when using any kind of Euler integration.
  - Groups automatically determine if they are looped by checking if any contained elements are looped. Manually setting
    the isLooped parameter is now obsolete.
  - Connections between data slots can now be established across groups.
- cedar::aux
  - Added an option to save the image displayed in an image plot to its right-click menu.
  - The netreader/writers now support matrices of arbitrary dimensionality (issue #248). Unfortunately, this means that
    older cedar versions will no longer be able to communicate with the new version as the protocol had to be changed.
  - The data recorder can now save in two different formats: CSV (the old and standard one; this format writes data as
    a CSV file and truncates some of the numbers, but is platform independent) and Compact (this is a largely binary
    format; a human-readable header precedes data, but matrices are written out as they are in memory, i.e., in binary
    format; what this looks like depends on the platform and is slightly more complex to read in, however, it uses much
    less space than the CSV format, is therefore faster to write out, and does not truncate matrix values).
    Which format is used can be chosen in the settings dialog of cedar.

Version 5.0.0
-------------

- build system
  - fixed an error that made it imposssible to use opencv versions that are not in /usr/local/include.
  - cedar should now compile with the opencv 3.0 beta.

- cedar::aux
  - There is a new plot called Multi0DPlot. This is mainly intended to be used for showing the state of (EB) nodes in an
    architecture, thus, it should be used in conjunction with an architecture widget. See the tutorial for architecture
    widgets for details.
- cedar::dev
  - Fixed some issues with video playback in the video grabber.
- cedar::proc
  - The use of (Looped)Triggers has been remodeled completely. They are no longer shown in the architecture. Instead,
    each looped element is, by default, connected to a default trigger that is created automatically. If you want to use
    more triggers, you can now do so via the simulation control widget (shift + t, or click the corresponding button in
    the toolbar/menu). This allows you to add triggers, remove them and change their settings.
    There is also a new mode for seeing what steps are connected to what triggers. This can be enabled from the toolbar
    and the menu ("color elements by trigger"). In addition to showing what trigger elements are connected to, hovering
    over a step in this mode will also display in what order it will trigger its subsequent steps.
  - The time factor setting is now saved per architecture. New architectures start with time factor 1. 
  - You can now declare favorite element classes in the elements tab. Right-click on any element to favorite/unfavorite
    the class.
  - There is now a search bar in the element class list. 
  - The UI elements (log, elements, properties etc.) are now locked by default and cannot be moved. You can unlock them
    in the windows menu.
  - The net reader and writer steps now work much more smoothly than before. They now automatically revalidate their
    connections as soon as possible, and indicate that they are not receiving data by turning yellow.

Version 4.1.0
-------------

- cedar::aux
  - You can now enable grid lines in line plots from the right-click menu.
- cedar::proc
  - You can now add special widgets to architectures that allow you to define special plots and more. These can be
    opened from the cedar gui, and their layout and contents are read from files. See the "Architecture Widgets"
    tutorial for details on how to use this feature.
  - ElementDeclarations can now provide a ConnectableIconView. This allows users to specify a custom class for
    displaying the icon of a step. Among other things this can be used to change the icon based on the value of a
    parameter. One such example is the static gain, which now changes its color based on the sign of the gain factor.
    Note, that this is optional, and the default behavior is the same as before. Details on how to implement such a view
    can be found in the processing framework tutorial.
  - Double-clicking a boost in the boost control widget now shows the boost in the architecture.
  - In the settings of cedar, you can now choose the default plot that opens for 2d matrices.
  - The start/stop buttons of the cedar gui have been remodeled. There is now a start button which, as before starts all
    triggers in an achitecture. It also starts the global timer. When pushed, it turns into a pause button which halts
    all triggers as well as the global timer. Pushing the pause-button will turn it back to a start button which can
    then be used to resume the simulation. An addded reset button can reset the simulation; i.e., it resets the
    architecture as well as the global timer.
  - Added a python plotting script for recorded data (in tools/recordedDataProcessor).
  - Added a trial framework for running experiments with cedar architectures (in the GUI, Tools>Experiments...).
  - Plugins can now declare C++ scripts which can be executed from the GUI. These allow running arbitrary code on a
    processing group. More details can be found in a tutorial in the documentation.
- cedar::dyn
  - On Windows, it should no longer be necessary to load this part of cedar as a plugin.

Version 4.0.0
-------------

- processingIde
  - The processingIde is now simply called cedar. Conversely, the processingCL is now called cedar-shell.

- cedar
  - There is now a new configurable widget. It uses a far more appropriate tree view for displaying parameters of a
    step, but is otherwise quite similar to the old one.
      
- cedar::proc
  - DataSlots can now be marked as serializable. What this means is that the data stored in them will be stored in the
    architecture when it is saved, and restored later on when it is loaded. In addition, users can manually save and
    load data for these slots from the right-click menu of step items.
  - Groups now work as intended: one can freely move elements between groups, existing connections are preserved, using 
    group connectors if necessary. In the gui, grouping is drag-and-drop-based.
  - Group templates can now be added in two ways:
    1. They can be copied into the current architecture. Any changes to the original template will not be reflected.
    2. They can be linked into the architecture. Only the file- and groupname will be stored, and the group will be re-
       loaded from the stored file every time the architecture is loaded. Note: linked groups cannot be edited to avoid
       conflicts. Any edits must be made to the file containing the imported group.
    To link in templates, hold ctrl while dragging them from the elements tab.
  - Step configurations can now be copied and pasted in the GUI using Ctrl+C and Ctrl+Shift+V.
  - In the GUI, one can use Ctrl+F to bring up a search window to center on specific steps
  - cedar now allows to slow down or speed up the global timers, effectively allowing for (0.0,2.0] x real-time.
  - cedar now notices if there were any changes made to an architecture and notifies the user about this when closing a
    file.
  - Looped triggers now have a tool tip showing statistics.
  - Improved performance of triggering chains.
  - Improved loading and connecting time for large architectures.
  - Reordered some menu entries in cedar. The 'Manage plugins ..." dialog is now found under Tools.
  - Data slots and connections give feedback about their validity and possible errors. This only works for elements
    using input type checks.

Version 3.0.0
-------------

- build system
  - libQGLViewer is now optional. If it is not present, many of the GL components of cedar, including the kinematic chain
    simulator will likely not function properly.
- general
  - The plugin system has been changed. It is no longer specific to the processing framework, although it is not (yet)
    used elsewhere. Plugins can now be added in an improved manager. They are found in a different way, now: a list of
    user-defined paths is searched for a plugin of a given name by appending certain directories (such as the plugin
    name itself and a "build" folder.) Using this new functionality, architectures now store which plugins they require.
- cedar:unit
  - The unit framework was replaced by typedefs to the boost unit framework.
- cedar::aux
  - There is now a command line parser available. This is intended to make software that performs experiments easy to
    customize; it features auto-generated help texts, reading and writing of options to file and some sanity checking of
    user inputs. It is still being developed.
- cedar::dev
  - A framework for robots and components was added.
- cedar::proc
  - Step can no longer be executed in a separate thread (nobody uses this functionality).
  - There is now a tool in the processingIde that lists all steps along with their execution times. This can be helpful
    in finding out which steps make your architectures slow.

Version 2.0.0
-------------
- general
  - Parameters of several classes are marked as "advanced" based on how often one normally uses these. Affected are
    for example anchor, shift, and limit of kernels and threshold of sigmoids.
- build system
  - enabled C++0x / C++11 compiler flag for GCC
  - cedar should now also compile when using clang.
  - on Windows platform: we now require MSVC 2012 (express)
- cedar::aux
  - when using LoopedThread a running QApplication or QCoreApplication is now required for its QT event loop (call exec() for the app).
  - DoubleData now inherits MatData. It therefore is also no longer just a typedef of a template, so make sure you
    include the proper header (cedar/auxiliaries/DoubleData) in your code.
  - Parameters and Configurables can now be marked as advanced. Those that are, are not displayed to the user unless
    they opt to have them displayed by checking a box in the property pane.
  - Log entries in gui::Log can now be removed using the right-click context menu.
  - Parameters of LoopedThread are now set to constant depending on the current mode (e.g., blocking "simulated time").
  - Memory debug output is turned off by default, can be activated through a configuration file.
- cedar::aux::gui
  - LinePlots now have an option in their context menu to set fixed limits for the y axis.
  - There is now a history plot for 1D matrices.
  - We added line and surface plots that are based on Vtk. This means that instead of (or in addition to) qwt and
    qwtplot3d, you can now use these plots.
  - Some plotting libraries (qwt and qwtplot3d) have now become optional.
  - ImagePlots can now display a legend. There is also an option to set a fixed scale for the color.
    These functions can be accessed via the right-click menu (note that they are only available if a matrix is being
    plotted. For images, these options don't exist.)
- cedar::dev
  - KinematicChain instances now need to be started via ->start() by the implementor. They do not start threading on
    their own, anymore.
  - There is now a visualization for the Caren robot.
- cedar::proc
  - The processingIde can now be started with the "--no-log" option. This will redirect all log messages to the terminal
    and can be very helpful when steps send too many log messages, leading to a lock-up of the user interface.
  - There is now a system to automatically perform input type checks in processing steps; this can replace writing of
    the determineInputValidity function. See the documentation and processing tutorial for details.
  - In the processingIde, you can now select a default display mode for steps. The available modes are: icon and text
    (old behavior, newly created steps are always shown with icon and text), icon only (newly created steps show only
    an icon) and text for looped (newly created steps always show their icon, text is only shown if the steps are looped,
    this is the new default)
  - gui::PluginManagerDialog now allows you to remove unused entries of plugins.
  - The "plot all" function can now be replaced by other plots where appropriate. For example, for neural fields, the
    field plot now replaces it. In such cases, plot all can still be accessed from the "defined plots" menu.
  - The processingIde now offers a tool for checking architecture consistency. It is still a very early version and will
    be extended in the future. For now, it only checks if all looped steps in the current architecture are connected to
    a looped trigger. The tool can be accessed from the "windows" menu.
  - LoopedTriggers in the processingIde can now be iterated manually, i.e., you can push a button to perform a single step.
  - The processingIde now has a boost control widget. From it, you can activate and change the strength of any boost in
    the current architecture. The widget can be accessed from the "tools" menu, or by pressing control+b.
  - Connections can now be toggled to being "smart", i.e., having heuristically determined corners to unclutter 
    architectures - this mode is fully automatic, so there's now way to move any lines around by hand.
  - DataSlots now scale when you drag a connection close to them. You can configure (and disable) this behavior in the
    tools -> settings -> user interface menu of the processingIde.
  - Please note that step icons can now only have the svg format. If you have an icon in a different format, please
    embed it in a svg file using, e.g., inkscape.
  - Connections of selected steps in the processingIde are now hightlighted. This behavior is active by default and can
    be disabled in the settings menu.
- cedar::dyn
  - The SpaceToRateCode step is now stable in any situation (i.e., for any time scale > 0, for any kind of input.)   


Version 1.0.10
-------------
- cedar::proc::gui
  - Fixed an issue that could lead to an exception when connecting renamed looped triggers.
    (see also issue #10 in stable)
- cedar::dyn
  - Fixed a potential deadlock in the neural field that occurred when the activation was set as output and an exception occurred during the Euler step.


Version 1.0.9
-------------
- cedar::proc
  - Fixed a bug in the resize step that lead to weird results when the step was being used with 1d inputs.
- cedar::aux::conv
  - using 3D convolutions in separate threads doesn't crash anymore, but the introduced lock slows down
    all involved convolutions (FFTW plan creation is not thread-safe). This will be fixed with a rewrite of FFTW code.


Version 1.0.8
-------------
- build system
  - fixed issues when linking to non-qt4 versions of QWT and libQGLViewer
    when qt3 and qt4 versions were present on the system.
  - Disabled the as-needed switch of gcc/ld; on newer Linux versions, this caused cedardyn to not be loaded properly at
    runtime, which meant fields and other steps from that library not showing up properly. This problem still persists
    on Windows/MSVC.
- cedar::aux::net
  - fixed issues with GCC 4.7 and the NetTransporter (Reader/Writer) code
    (only occured when the optional YARP dependency was included)


Version 1.0.7
-------------
- cedar::aux::gui
  - fixed a bug in how matrix plots handle changing data dimensionality
- cedar::proc
  - fixed a potential crash in Network::getElement("path") 


Version 1.0.6
-------------
- General
  - added support for OpenCV 2.2
  - fixed build on Windows
- cedar::aux::conv
  - fixed a memory leak in FFTW-based convolution

Version 1.0.5
-------------
- Build system
  - 'make install' can be used without having Doxygen installed

Version 1.0.4
-------------
- Build system
  - Moved the version information to a separate file (cmake/version.txt).
- cedar::aux::math
  - Fixed a bug in the function gaussMatrix that discretized the center position of cyclic Gaussian curves.

Version 1.0.3
-------------
- cedar::aux
  - Fixed some minor bad code in the gui::LinePlot
  
- cedar::proc
  - Fixed a bug in the projection step that could lead to a crash when the destination dimensions of a 3d to a 2d matrix
    were swapped.

Version 1.0.2
-------------
- cedar::aux
  - Fixed some threading issues in the gui::ImagePlot.
  
- cedar::proc
  - Fixed an unitialized value in the color conversion step.

Version 1.0.1
-------------
- cedar::aux
  - Fixed a bug in the SemiLinear transfer function; the threshold is now applied properly.

Version 1.0.0
-------------
- cedar::aux
  - The image plot, when clicked, now shows the value of the pixel under the cursor.
  
- cedar::proc
  - IMPORTANT: when connections are removed, the inputConnectionChanged method is now called (as should always have been
    the case); please test your steps accordingly and make sure you check whether the new data is not a null pointer!
  - You can now define plots in ElementDeclarations. Plots, in this case, are a list of data that is to be plotted
    together.
  - The resize step now orders its dimensions consistently in two-dimensional cases; unfortunately, this means that
    all old architectures will need to flip their size order (used to be [width, height], is now [height, width]).
  - Plot widgets now no longer dock in the processingIde!
  - Steps can now take care of locking their inputs and outputs manually by setting a flag. This may be useful if steps
    lock data for a long time even though they are not accessing it, potentially preventing other steps in the
    processing chain from running.
  - The processingIde can now export architectures into SVG files.

Version 0.9.1
-------------
- general
  - cedar now accepts both qwt versions 5.x and 6.x
- cedar::aux
  - added a slice plot for 3D matrix data
  - implemented full mode for the opencv convolution engine
  - removed the ConfigurationInterface (and the option to use CEDAR_LIBCONFIG_LEGACY_MODE)
  - FileParameters now can be set to have their path relative to the current working directory.
  - A new annotation system has been introduced. Because of this, ImageData is now deprecated and should be switched to
    MatData.
- cedar::dev
  - added MountedCameraViewer, simulating cameras fixed on a kinematic chain in OpenGL
- cedar::proc
  - added a processing step for convolutions
  - parameters are now locked in the compute method of processing steps
  - as a very first attempt to reduce clutter in larger architectures, steps now have display modes that can accessed
    and set via the right-click menu in the processingIde. 
  - interface change for cedar::proc::Connectable (raising the level of const-correctness):
    virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                            (
                                              cedar::proc::ConstDataSlotPtr slot,
                                              cedar::aux::DataPtr data  // not const
                                            ) const;
    is now
    virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                            (
                                              cedar::proc::ConstDataSlotPtr slot,
                                              cedar::aux::ConstDataPtr data // introduced ConstDataPtr here
                                            ) const;
  - cedar::aux::math::Sigmoid now inherits from cedar::aux::math::TransferFunction, with TransferFunction being
    a generalization of a sigmoid function (a mapping R -> R)
- build system
  - added a find script for FFTW that also searches for the include directory
  - added defines for the current cedar version; this makes it possible to stay compatible to older versions, should
    this ever be needed.
  - removed dependency on libconfig

Version 0.9
-----------
- cedar::aux
  - fixed a critical bug in LoopedThread::singleStep, which used microseconds instead of milliseconds for step()
  - cedar::aux::Configurable now inherits from boost::noncopyable, explicitly stating that you should never use
    the copy constructor. Instead, you should use the functions Configurable::copyTo and Configurable::copyFrom.
  - Templated functions in cedar::aux::math now have an "alias" version that can be called without a template parameter.
    This function automatically decides which template version to call based on the matrix types.
  - Added a function cedar::aux::System::locateResource that helps with finding cedar resources (i.e., config files
    etc.) independent of the current working directory. See the documentation for details.
  - the cedar::aux::Base class was removed
  - The old ConfigurationInterface is being phased out now. For this purpose, a transitional interface has been
    developed that should mostly work with the old code. If you run into trouble, add the following line to your
    cedar configuration file:
    set (CEDAR_LIBCONFIG_LEGACY_MODE      1           CACHE BOOL "Use libconfig legacy mode.")
    If you set this flag to 0, libconfig++ is no longer needed as a dependency! Some unit tests may fail, though.
  - merged AbstractFactory and AbstractFactoryDerived with Factory and FactoryDerived (including some changes to the
    interfaces)
  - introduced new FactoryManager class
  - gl::Object has been renamed to gl::RigidBody
  - There are some new Parameter types: ObjectListParameter and ObjectParameter. They allow dynamically selecting the
    type of one or more objects. For example, these allow to dynamically select the type of the Sigmoid as well as the
    combination of kernels in the neural field.
  - Added a new Log class to cedar::aux. Using this is the new standard logging mechanism (over std::cout), as it can be
    redirected at will, e.g., to the console (default) or some user interface. To access the log, use the corresponding
    cedar::aux::LogSingleton::getInstance(). See the Logging tutorial in the documentation or cedar::aux::Log.
  - There is a new widget for displaying log messages: cedar::aux::gui::Log.
  - NumericParameters (DoubleParameter, IntParameter, UIntParameter) no longer need minimum and maximum values. They
    also have a new constructor that takes a math::Limits object which is the new, recommended way of setting the
    limits. The same is true for NumericVectorParameters.
  - Added Data classes for stereo images and image sets.
  - There is now a generalized structure for convolution. This structure wraps opencv and, optionally, fftw convolution!
    For more, see the documentation for the cedar::aux::conv::Convolution class.
  - The net namespace was added. This namespace contains wrappers around yarp's functionality and is available if cedar
    is compiled with yarp.
    
- cedar::dev
  - Added several classes for grabbing pictures, videos and cameras in cedar::dev::sensors::visual.

- cedar::proc
  - added directional arrows for graphical connections in processingIde
  - when loading a plugin, the PluginProxy now also looks for a plugin description file. This file may contain some
    information about your plugin in case you don't want to hardcode it, e.g., it can contain icon paths for processing
    steps. See documentation of cedar::proc::PluginDeclaration for details.
  - Overhauled the plotting system. You can now select different types of plots for data in the processingIde where
    available.
  - added grouping functionality for processing
  - added promotion of data slots for networks
  - Some outputs and parameters have been renamed. If your architectures fail to load, check that the connections use
    the correct names.

- processingIde
  - The IDE now remembers whether or not snap to grid is active.
  - Implemented zooming in the IDE; apart from the controls that are directly visible, scrolling is also possible with
    the mouse wheel (hold down ctrl)
  - The property pane now indicates when parameters have been changed since the last save. 
  - more verbose tooltip for data slots
  - there is now a settings dialog; more settings will be added later.
  - the tab order in the property pane is now working correctly

- cedar::dyn
  - NeuralField now supports changing the sigmoid type, the number and type of kernel modes as well as dynamic exchange
    of the convolution method
    
- build system
  - Projects depending on cedar no longer need to define __LINUX__ etc.; this is now done automatically by the build
    system. 
  - files in the resource folder are now moved to ${CEDAR_INSTALL_PREFIX}/share/resources. Use the
    cedar::aux::System::locateResource function to locate them.
  - It is no longer necessary to specify CEDAR_BUILD_DIR in the cedar.conf file. Instead, if the variable is not
    specified, the directory in which you call cmake is automatically selected.

Version 0.5.1
-------------
- cedar::proc::steps
  - fixed compilation bug of cedar::proc::steps::GaussInput on Windows platforms

Version 0.5
-----------
first release!

