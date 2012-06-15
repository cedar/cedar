/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Grabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::devices::sensors::visual::Grabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>
#include <boost/utility.hpp>      //boost::noncopyable
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>


/*! @class cedar::dev::sensors::visual::Grabber
 *  @brief This is the base class for all grabber.
 *
 *      This class implements all the common features, each grabber has to be capable of.
 *      That is e.g. managing the configuration file, recording, threaded grabbing and other stuff like this.

 *   <br><br>
 *   USAGE:
 *     - to set a name for the grabber: getName, setName form cedar::base<br>
 *     - to control the grabbing-thread: <br>
 *             => isRunning() : from QThread<br>
 *             => startGrabber(), stopGrabber() <br>
 *             => setFps(), getFps(), getFpsMeasured <br>
 *     - grabbing: <br>
 *             => grab manually a new image: grab()  <br>
 *             => get the image from the internal buffer: getImage() <br>
 *     - recording: startRecording(), stopRecording()  <br>
 *     - snapshots: saveSnapshot(), saveSnapshotAllCams(), getSnapshotName()<br>
 *
 *    @remarks For grabber developers <br>
 *      If you would like to create your own derived grabber:
 *      Initialize in the constructor of a derived class the filenames or camera-device-names.
 *      At the end of the constructor, call doInit() with the number of channels you use and with a default name.
 *      At the beginning of the destructor, call doCleanUp() and do the cleanup in this function.
 *      To get an example look at the VideoGrabber or the TestGrabber-class. <br><br>
 *
 *
 */
class cedar::dev::sensors::visual::Grabber
:
public cedar::aux::LoopedThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

protected:
  //!@cond SKIPPED_DOCUMENTATION

  ///! @brief Structure to store all channel related stuff inside
  struct GrabberChannel
  {
    //! the picture frame
    cv::Mat mImageMat;

    //! Filename for snapshot
    std::string mSnapshotName;

    //! Filename for recording
    std::string mRecordName;

    ///! for recordings
    cv::VideoWriter mVideoWriter;

    /// The channel information
    std::string mChannelInfo;
  };

  CEDAR_GENERATE_POINTER_TYPES(GrabberChannel);

  ///! @brief Typedef for a vector containing the instances of all used grabbers
  typedef std::vector<cedar::dev::sensors::visual::Grabber*> GrabberInstancesVector;

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //!@endcond


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

protected:
  /*! @brief The standard constructor.
   *  @remarks
   *    The constructor is protected, because no instance of Grabber should be instantiated.
   *    Use a derived class instead.
   *  @param
   *    configFileName The filename where the configuration parameters should be stored in
   */
  Grabber(const std::string& configFileName);

public:

  //!@brief Destructor
  virtual ~Grabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief This method does an emergency cleanup of ALL instantiated grabbers
   *
   *  This method is called in the crash handler. After a call of this method, all grabbers are in an invalid state
   *  and the grabbers or your program have to be terminated.
   */
  static void emergencyCleanup();

  /*! @brief This method registers a signal handler for the signals SIGINT, SIGABRT and SIGSEGV
   *
   *  A crash handler should be used, especially if a firewire camera grabber is instantiated.
   *  The internal handler catch the mentioned signals and calls the emergencyCleanup() method.
   *  If you implement your own handler, call the static member emergencyCleanup() in your method.
   *
   *  @see emergencyCleanup, doCleanUp
   */
  static void installCrashHandler();


  /*! @brief Get the number of currently used channels (sources).
   */
  unsigned int getNumCams() const;

  /*! @brief Get the size of a specified camera-channel as cv::Size.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @param channel
   *       This is the index of the source you want the size of the picture from.<br>
   *       In the mono case you do not need to supply this value. Default is 0.<br>
   *       In the stereo case it may be 0 or 1.
   */
  cv::Size getSize(unsigned int channel=0 ) const;

  /*! @brief Get information about the used device, i.e. the filename or the mount-point
   *
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @param channel
   *       This is the index of the source you want the name of the source from.<br>
   *       In the mono case you do not need to supply this value. Default is 0.<br>
   *       In the stereo case it may be 0 or 1.
   */
  std::string& getSourceInfo(unsigned int channel=0);

  //------------------------------------------------------------------------
  //Thread related methods
  //------------------------------------------------------------------------

  /*! @brief Get the framerate which LoopedThread is set for grabbing
   *
   *      This value doesn't indicate, if the thread is running or not.
   *      To get the actual grabbing speed use getFpsMeasured.
   *  @see startGrabber(), stopGrabber(), QThread::isRunning()
   */
  double getFps() const;

  /*! @brief Set the framerate of the loopedthread speed for grabbing
   *
   *          This value doesn't indicate, if the thread is running or not.
   *          The LoopedThread have to be restarted for changing the framerate.
   *          This is done in this function, but keep it in mind.
   *  @see startGrabber(), stopGrabber()
   */
  void setFps(double fps);

  /*! @brief Get the framerate of actual grabbing speed
   *
   *          Calculates the framerate by counting the invocation of the grab() method
   *          This value is updated every 5 (default value set in defines.h) grabbed frames
   */
  double getFpsMeasured() const;

  /*! @brief Stop the grabbing thread
   *
   *          This method invokes internally LoopedThread::stop() and does
   *          some cleanup like stopRecording or set the measured FPS to zero
   */
  void stopGrabber();

  /*! @brief Start the grabbing thread
   *
   *          This method invokes internally LoopedThread::start() and does
   *          some initialization due to the measurement of FPS. <br>
   *
   *          To control the grabbing speed (i.e. the FPS) use
   *          setFps(), getFps() or getFpsMeasured()
   */
  void startGrabber();


  //------------------------------------------------------------------------
  //Grabbing methods
  //------------------------------------------------------------------------

  /*! @brief Get an Image in a cv::Mat structure

   *      With this method you can get the grabbed image.
   *  @param channel
   *		  This is the index of the source you want the picture from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see
   *      getNumCams
   */
  cv::Mat getImage(unsigned int channel=0) const;


  /*! @brief Grab a new image (or images in stereo case) from the source (channel, picture, avi...).
   *
   *      This method is called automatically by the grabber thread.<br>
   *      If you don't use the grabber thread you have to call it by yourself to get new images.
   *  @throw cedar::aux::GrabberGrabException Thrown on an error while grabbing
   *  @throw cedar::aux::GrabberRecordingException Thrown on an error while recording
   *  @see startGrabber, setFps
   */
  void grab();

  /*! @brief Get a pointer to the QReadWriteLock.
   *
   *      Used for concurrent reading/writing to the image matrices
   */
  QReadWriteLock* getReadWriteLockPointer() const;


  //------------------------------------------------------------------------
  //Snapshot methods
  //------------------------------------------------------------------------

  /*! @brief Set the snapshot filenames for all defined channels
   *
   *		In the mono case, filename is used without changes<br>
   *		In the stereo case, the filenames are constructed like<br>
   *		snapshotName_wo_ext[ch:ChannelIndex:].snapshotName_ext<br>
   *		For example: snapshot[0].jpg for the first channel by default
   *  @param snapshotName This is the filename for the snapshot with extension
   *  @see
   *		getSnapshotName
   */
  void setSnapshotName(const std::string& snapshotName);

  /*! @brief Set the snapshot filename for the given channel
   *
   *   The image format is chosen based on the filename extension.
   *
   *   @param channel This is the index of the source you want the picture from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *   @param
   *       snapshotName This is the filename for the snapshot with extension
   *   @throw cedar::aux::IndexOutOfRangeException
   *
   *   @remarks
   *    Currently, the following file formats are supported by OpenCV:
   *  <pre>
   *    always supported:
   *      Windows bitmaps            *.bmp, *.dib
   *      Portable image format      *.pbm, *.pgm, *.ppm
   *      Sun rasters                *.sr, *.ras
   *  </pre>
   *  <pre>
   *   see the OpenCV documentation (depends on system and installed libs on the system)
   *      JPEG files                 *.jpeg, *.jpg, *.jpe
   *      JPEG 2000 files            *.jp2
   *      Portable Network Graphics  *.png
   *      TIFF files                 *.tiff, *.tif
   *  </pre>
   *  @see
   *      getSnapshotName
   *  @see
   *      saveSnapshot
   *
   */
  void setSnapshotName(unsigned int channel, const std::string& snapshotName);

  /*! @brief Get the current name of the snapshot file.
   *
   *  @param channel
   *		This is the index of the source you want the snapshot from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  std::string getSnapshotName(unsigned int channel=0) const;

  /*! @brief Save a snapshot to a file.
   *
   *      Saves only one snapshot from the given channel
   *  @param
   *      channel is the index of the source you want the snapshot from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @throw cedar::aux::GrabberSnapshotException Thrown on error while storing the image to the filesystem
   */
  void saveSnapshot(unsigned int channel=0) const;

  /*! @brief Save snapshots of all channels
   *
   *    For every capture-device saveSnapshot() is invoked
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @throw cedar::aux::GrabberSnapshotException Thrown on error while storing the image to the filesystem
   *  @see saveSnapshot(unsigned int)
   */
  void saveSnapshotAllCams() const;

  /*! @brief Write the actual used parameters to configuration
   *
   *    The configuration have to be saved manually with this function. There is no automatic saving for
   *    example at the end of the program.
   *
   *  @return The integer return value of ConfigurationInterface::writeConfiguration() is casted to boolean
   *
   *  @remarks For grabber developers<br>
   *    This method evokes onWriteConfiguration of the grabber. Implement in the derived class this function,
   *    if there are any parameters have to be changed right before saving
   *  @see onWriteConfiguration
   */
  bool writeConfiguration();


  //------------------------------------------------------------------------
  //Record methods
  //------------------------------------------------------------------------


  /*! @brief Set the recording filenames for all defined channels
   *  @param
   *		recordName This is the filename for the snapshot with extension
   *  @remarks
   *		In the mono case, filename is used without changes<br>
   *		In the stereo case, the filenames are constructed like<br>
   *		recordName_wo_ext[ch:CameraIndex:].recordName_ext.<br>
   *		For example: startRecording[ch0].avi for the first camera by default
   *  @see
   *		setRecordName, getRecordName
   *
   */
  void setRecordName(const std::string& recordName);

  /*! @brief Set the recording filename for the given camera
   *
   *    The file format is chosen by the ending of the filename, the file encoding depends on the
   *    parameter of startRecording().
   *  @param
   *		channel This is the index of the source you want the picture from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @param
   *     recordName This is the filename for the startRecording with extension
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see
   *      startRecording, setRecordName, getRecordName
   */
  void setRecordName(unsigned int channel,const std::string& recordName);

  /*! @brief Get the current name of the recording file for the given camera
   *  @param
   *      channel This is the index of the source you want the picture from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  const std::string& getRecordName(unsigned int channel=0) const;

  /*! @brief Initialize and startGrabber recording
   *
   *      If the grabbing thread isn't running, startRecording will startGrabber the Thread
   *      and stopRecording will stopGrabber it.
   *
   * Create the VideoWriter structures and set the record flag.
   *  @param
   *      fps Defines the framerate of the recorded avi-file. It is independent from the speed of
   *      the grabbing thread. But the number of pictures will be the same. So if you grab on a slower speed than
   *      this fps-parameter, the recorded video will be shorter.
   *  @param
   *      fourcc 4-character code of codec used to compress the frames.<br>
   *      Examples:<br>
   *      CV_FOURCC('P','I','M,'1') is a MPEG-1 codec<br>
   *		  CV_FOURCC('M','J','P','G') is a motion-jpeg codec<br>
   *      CV_FOURCC('M','P','4','2') is also a motion-jpeg codec<br>
   *      Default is 0, for raw recording
   *  @param
   *      color Determins if recording is in color or black/white mode.
   *      Default value is true.
   *
   *  @remarks
   *      Always all cameras recording. To set the filename use setRecordName()<br>
   *      By default record.avi is used as filename (in the mono case) <br><br>
   *      The transcoding is entirely done via the opencv-API.<br>
   *      Look at the OPENCV manual to determine the usable FOURCC's
   *  @par
   *     for supported codecs, have a look at:<br>
   *     /usr/local/src/OpenCV_{YOUR_VERSION}/modules/highgui/src/cap_ffmpeg_impl.hpp<br>
   *     http://www.fourcc.org/codecs.php<br>
   *
   *  @see
   *      setRecordName, getFps, VideoGrabber::getSourceFps
   */
  //!@todo: enum-Klasse fuer fourcc
  bool startRecording(double fps,int fourcc = 0,bool color = true, bool startThread = true);

  /*! @brief Stop all recordings
   *  @remarks
   *		All VideoWriter structures are released. There is no possibiltiy to append another recording.<br>
   *		If recording will be restarted without changing the recording filenames, the old files will be
   *		overwritten
   */
  void stopRecording();

  /*! @brief Get the state of the recording-flag
   *
   */
  bool isRecording() const;

  //!@cond SKIPPED_DOCUMENTATION
  /*! @brief Defines the additions to the filename.
   *
   *   Will be used in a stereo-grabber if you use setSnapshotName("SnapshotFilename")
   *   or setRecordName("RecordFilename") to set both filenames at once
   *
   *  @param channel
   *    This is the index of the source you want the snapshot from.
   *
   *   @remarks default is "_ch[channel]", for example "_ch0" for channel 0
   */
  std::string getChannelSaveFilenameAddition(int channel) const;
  //!@endcond

  //!@brief Returns the name of the grabber instance.
  inline std::string getName() const
  {
    return this->_mName->getName();
  }


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


  /*! @brief  This function initialize the grabber.
   *
   *  @remarks For grabber developers <br>
   *          Have to be called in the constructor of the derived class. <br><br>
   *          This is the first part of the initialization.
   *          The initialization is a little bit tricky, but it restores and initializes all
   *          local parameters from the configuration interface before Grabber::applyInit()
   *          and onInit of the derived class will be invoked.<br><br>
   *          On the other side, it is possible to change restored parameter in the constructor of a
   *          derived class between this two steps (i.e. readInit(...); YOUR_PARAMETER_CHANGE; applyInit() )
   *
   *  @par
   *          If onInit() fails, onCleanUp() will be invoked in order to clean up already initialized channels.
   *          <br><br>
   *          For an example look at VideoGrabber, NetGrabber or TestGrabber
   *  @see onInit, declareParameter, onCleanUp
   *
   *  @param numCams The init-function need to know how many channels there are
   *  @param defaultGrabberName This name is used as a default name, which will
   *         be stored in the configuration file. To change the name
   *  @see cedar::aux::ConfigurationInterface::setName
   */
  void readInit(unsigned int numCams,const std::string& defaultGrabberName);

  /*! @brief This function applies the former read initialization and then it calls onInit of the derived class
   *   @see onInit()
   */
  void applyInit();

  /*! @brief  Periodically call of grab()
   *
   *  For details have a look at cedar::aux::LoopedThread
   */
  void step(double time);

  /*! @brief Call this method at the beginning of the destructor in the derived class
   *
   *      This method does the necessary clean up (like stopGrabber recording or stopping the grabbing thread)
   *      and then it invokes the onCleanUp() method of the derived class.<br><br>
   *      Note:<br>You have to call this method in the destructor of a derived class.<br><br>
   *  @remarks
   *      This method will also be invoked by the static emergencyCleanup() member
   *  @see onCleanUp
   */
  void doCleanUp();

  //------------------------------------------------------------------------
  //For derived classes
  //------------------------------------------------------------------------


  /*! @brief  This method is called during initialization.
   *
   *      Override this method in the derived classes to do their initialization.<br>
   *      The Vector mImageMatVector have to be initialized with the right amount of matrices in this function.<br>
   *      This function is called in the /see Grabber::Grabber after initialization is
   *      complete.<br>
   *      Parameters from the configfile restored before onInit is invoked. So don't overwrite them.<br>
   *      For examples, look at the classes VideoGrabber, CameraGrabber and PictureGrabber.
   */

  virtual bool onInit();

  /*! @brief  This method is invoked during destruction of the class.
   *
   *      This function is invoked by the doCleanUp method in the Grabber.
   *      Override this method in the derived class and do only the absolutely necessary cleanup here.<br><br>
   *
   *   @note
   *      This method is also invoked by the CTRL-C handler. Therefore, it is the only possibility
   *      to do the necessarily cleanup like hardware-reinit, shutting down cameras or to close file handles.
   *
   *      The dynamic allocated memory of the class should be freed in the standard destructor of the derived class.
   *  @see doCleanUp
   */
  virtual void onCleanUp();

  /*! @brief  This is the grabbing method.
   *
   *      This method is called by grab() and has to be implemented by the inherited grabber.
   *      This is where the grabbing takes place.
   */
  virtual bool onGrab();

  /*! @brief  Declare parameters which should be saved in config-file
   *
   *		Should be overrided in derived class.<br>
   *		Declare parameters for derived class in this method.<br>
   *		Saved values are loaded at the beginning of Grabber.init()
   *  @see doInit
   */
  virtual bool onDeclareParameters();

  /*! @brief  In this method you can re-check or modify parameters
   *
   *    Should be overrided in derived class.<br>
   *    Modify or actualize parameters declared in onDeclareParameters.<br>
   *    This method only will be evoked right before the parameters are saved with
   *    cedar::aux::ConfigurationInterface::writeConfiguration().<br>
   *    Save your Parameters with Grabber::writeConfiguration()
   *  @see onDeclareParameters, writeConfiguration
   */
  virtual bool onWriteConfiguration();


  /*! @brief Get information about the used device, i.e. the filename or the mount-point
   *
   *		You have to implement this method in the derived class. Set the informations
   *		about the channels there. This is the only true virtual function of the Grabberinterface.
   *   @param channel
   *		This is the index of the source you want the name of the source from.<br>
   *		In the mono case you do not need to supply this value. Default is 0.<br>
   *		In the stereo case it may be 0 or 1.
   *
   *   @remarks For Grabber developers<br>
   *       This is the only pure virtual member. It have to be implemented in derived class
   *       to supply correct informations
   */
  virtual void onUpdateSourceInfo(unsigned int channel=0) = 0;


  /*! @brief Create and initialize the channel-structure for only one channel
   *
   *  For grabber-developers:<br>
   *  Should be overrided in derived class, if more informations on a channel have to be stored
   */
  virtual void onAddChannel();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:


  /*! @brief Callback function to respond to a captured CTRL-C event
   *
   *      This function calls the static emergencyCleanup function of all registered grabbers and then exit(1).
   *   @see
   *      emergencyCleanup, doCleanUp, installCrashHandler
   *   @param signalNo The captured signal. Only CTRL-C is implemented
   */
  static void interruptSignalHandler(int signalNo);

  /*! Get the pointer to the channel structure of the specified channel
   *  @param channel The channel number of the wanted channel structure
   */
  inline GrabberChannelPtr getChannel(unsigned int channel)
  {
    return mChannels.at(channel);
  }

  /*! Get the const pointer to the channel structure of the specified channel
   *  @param channel The channel number of the wanted channel structure
   */
  inline GrabberChannelPtr getChannel(unsigned int channel) const
  {
    return mChannels.at(channel);
  }



  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:

    ///! For every grabbing-channel one channel structure
    std::vector<GrabberChannelPtr> mChannels;


    /*! @brief  Flag if recording is on     */
    bool mRecord;

    /*! @brief  mNumCams should be used instead of mImageMatVector.size()
     * @remarks
     *          Initialization should be done in the constructor of the derived class.
     */
    unsigned int mNumCams;


    /*! @brief Read/write lock
     *  @remarks
     *          Used for concurrent access to the mImageMatVector - matrices
     */
    QReadWriteLock* mpReadWriteLock;
    
    /*! @brief The actual measured fps of grabbing
     *
     */
    double mFpsMeasured;
    
private:
    
    ///! @brief Flag which indicates if the GrabberThread was started during startRecording
    bool mGrabberThreadStartedOnRecording;

    ///! @brief Flag which indicates if the CleanUp was already done (perhaps due to an error)
    bool mCleanUpAlreadyDone;
    
    ///! @brief Timestamp at startGrabber time to measure the real fps of grabbing
    boost::posix_time::ptime mFpsMeasureStart;

    ///! @brief Timestamp at end time to measure the real fps of grabbing
    boost::posix_time::ptime mFpsMeasureStop;

    ///! @brief Counter to measure the real fps of grabbing
    unsigned int mFpsCounter;

    ///! @brief The vector containing references to the instances of all created grabbers
    static GrabberInstancesVector mInstances;

    ///! @brief Flag which indicates if the CTRL-C Handler should be registered or not
    static bool mRegisterTerminationHandler;

    ///! @brief Flag, if this is the first instance of a grabber (used for the ctrl-c handler)
    bool mFirstGrabberInstance;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //------------------------------------------------------------------------
  // Defines the how often getFpsMeasured() will be updated (in frames).
  // Default value is every 5 frames
  static const int UPDATE_FPS_MEASURE_FRAME_COUNT = 5;

private:
  cedar::aux::StringParameterPtr _mName;

  //--------------------------------------------------------------------------------------------------------------------
  // constants
  //--------------------------------------------------------------------------------------------------------------------

  // Defines the default file formats of recordings and snapshot.
  // This extension will only be used, if there is no extension
  // in the new name parameter given to setSnapshotName() or setRecordName()
public:
  //!@brief The default extension for recording files. The extension determine the type of the file
  static const std::string mGrabberDefaultRecordExtension;
  //!@brief The default extension for a snapshot. The extension determine the type of the file
  static const std::string mGrabberDefaultSnapshotExtension;

}; //class cedar::dev::sensors::visual::Grabber

#endif //CEDAR_DEV_SENSORS_VISUAL_GRABBER_H
