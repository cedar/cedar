/*======================================================================================================================

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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/devices/sensors/visual/RecordingFormat.h"
#include "cedar/devices/sensors/visual/GrabberChannel.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>
#include <vector>
#include <string>

/*! @class cedar::dev::sensors::visual::Grabber
 *  @brief This is the base class for all grabber.
 *
 *      This class implements all the common features, each grabber has to be capable of.
 *      That is e.g. managing the configuration file, recording, threaded grabbing and other stuff like this.
 *
 *   <br><br>
 *   USAGE:
 *     - to set a name for the grabber: getName, setName form cedar::base<br>
 *     - to control the grabbing-thread: <br>
 *             => isRunning() : from QThread<br>
 *             => start(), stop() <br>
 *             => setFramerate(), getFramerate(), getMeasuredFramerate <br>
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
 *      To get an example, have a look at the VideoGrabber or the TestGrabber-class. <br><br>
 *
 *
 */
class cedar::dev::sensors::visual::Grabber
:
virtual public cedar::aux::NamedConfigurable,
public cedar::aux::LoopedThread
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

public:

  //! @brief Typedef for a vector containing the instances of all used grabbers
  typedef std::vector<cedar::dev::sensors::visual::Grabber*> GrabberInstancesVector;

  //!@cond SKIPPED_DOCUMENTATION
  typedef cedar::aux::FactoryManager<cedar::dev::sensors::visual::GrabberChannelPtr> ChannelManager;
  typedef cedar::aux::Singleton<cedar::dev::sensors::visual::Grabber::ChannelManager> ChannelManagerSingleton;
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

protected:
  /*! @brief The constructor for a single channel instance
   *  @param grabberName The name of your grabber
   *  @param pChannel An already initialized channel structure of the derived grabber
   *  @remarks
   *    The constructor is protected, because no instance of Grabber should be instantiated.
   *    Use a derived class instead.
   */
  Grabber(const std::string& grabberName, GrabberChannelPtr pChannel);

  /*! @brief The constructor for a single channel instance
   *  @param grabberName The name of your grabber
   *  @param pChannel0 An already initialized channel structure of the derived grabber for channel 0
   *  @param pChannel1 An already initialized channel structure of the derived grabber for channel 1
   *  @remarks
   *    The constructor is protected, because no instance of Grabber should be instantiated.
   *    Use a derived class instead.
   */
  Grabber(const std::string& grabberName, GrabberChannelPtr pChannel0, GrabberChannelPtr pChannel1);

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

  /*! @brief This function applies the former read initialization (read via readJson)
   *
   *    If a grabber was already in grabbing mode, then a the old grabber will be deleted an a new one created
   *    (with the changed parameters)
   *
   *    Call this method after readJson() to apply the new values.
   *    It is possible to change values between readJson() and applyParameters() via the set-Methods
   *    @see setRecordName(), @see setName(), @see CameraGrabber::setSettings()
   */
  bool applyParameter();

  /*!@brief This function indicates, if the grabber is already in grabbing mode
   *
   * This is useful, if you want to check if the automatically created grabbers from a config-file are fully
   * functional, or if there was a problem during startup with the default parameters.
   *
   * At an instantiation, the grabber always will be created. If the capture-device also will be created depends on
   * the default parameter. If there is no error, then the capture-device will be created and the grabber is fully
   * functional.
   *
   * On an error, only the grabber is created, without the capture-device. It is possible to adjust the parameter
   * and try to create the capture-device again with the applyParameter() method
   */
  bool isCreated() const;


  /*! @brief Resets the Grabber to initialization values, without opening the device
   *
   *  Call this method if you want to change settings of the capture device, which couldn't be set if the device is
   *  already opened (like the size of the frames of a camera-grabber)
   */
  void closeGrabber();

  //! @brief Get the number of currently used channels (sources).
  unsigned int getNumChannels() const;

  /*! @brief Get the size of a specified camera-channel as cv::Size.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @param channel
   *       This is the index of the source you want the size of the picture from.<br>
   *       In the mono case you do not need to supply this value. Default is 0.<br>
   *       In the stereo case it may be 0 or 1.
   */
  cv::Size getSize(unsigned int channel = 0) const;

  /*! @brief Get information about the used device, i.e. the filename or the mount-point
   *
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @param channel
   *       This is the index of the source you want the name of the source from.<br>
   *       In the mono case you do not need to supply this value. Default is 0.<br>
   *       In the stereo case it may be 0 or 1.
   */
  std::string getSourceInfo(unsigned int channel = 0) const;

  //------------------------------------------------------------------------
  // Thread related methods
  //------------------------------------------------------------------------

  /*! @brief Get the framerate which LoopedThread is set for grabbing
   *
   *      This value doesn't indicate, if the thread is running or not.
   *      To get the actual grabbing speed use getMeasuredFramerate.
   *  @see start(), stop(), QThread::isRunning()
   */
  double getFramerate() const;

  /*! @brief Set the framerate of the loopedthread speed for grabbing
   *
   *          This value doesn't indicate, if the thread is running or not.
   *          The LoopedThread have to be restarted for changing the framerate.
   *          This is done in this function, but keep it in mind.
   *  @see start(), stop()
   */
  void setFramerate(double fps);

  /*! @brief Get the framerate of actual grabbing speed
   *
   *          Calculates the framerate by counting the invocation of the grab() method
   *          This value is updated every 5 (default value set in defines.h) grabbed frames
   */
  double getMeasuredFramerate() const;

  //!@deprecated Use the stop() method (see cedar::aux::TreaadWrapper) instead.
  CEDAR_DECLARE_DEPRECATED(void stopGrabber())
  {
    this->stop();
  }

  //!@deprecated Use the start() method (see cedar::aux::ThreadWrapper) instead.
  CEDAR_DECLARE_DEPRECATED(void startGrabber())
  {
    this->start();
  }


  //------------------------------------------------------------------------
  // Grabbing methods
  //------------------------------------------------------------------------

  /*! @brief Get an Image in a cv::Mat structure

   *      With this method you can get the grabbed image.
   *  @param channel
   *      This is the index of the source you want the picture from.<br>
   *      In the mono case you do not need to supply this value. Default is 0.<br>
   *      In the stereo case it may be 0 or 1.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   *  @see
   *      getNumChannels
   */
  cv::Mat getImage(unsigned int channel=0) const;


  /*! @brief Grab a new image (or images in stereo case) from the source (channel, picture, avi...).
   *
   *      This method is called automatically by the grabber thread.<br>
   *      If you don't use the grabber thread:<br>
   *        - you have to call it by yourself to get new images.<br>
   *        - before and after grabbing, set/unset the "isgrabbing"-flag via the setIsGrabbing(bool) method to<br>
   *          indicate, that the grabbing is active.
   *
   *  @throw cedar::aux::GrabberGrabException Thrown on an error while grabbing
   *  @throw cedar::aux::GrabberRecordingException Thrown on an error while recording
   *  @see start, setFramerate
   */
  void grab();

  /*! @brief Get a pointer to the QReadWriteLock.
   *
   *      Used for concurrent reading/writing to the image matrices
   */
  QReadWriteLock* getReadWriteLockPointer() const;


  //------------------------------------------------------------------------
  // Snapshot methods
  //------------------------------------------------------------------------

  /*! @brief Set the snapshot filenames for all defined channels
   *
   *    In the mono case, filename is used without changes<br>
   *    In the stereo case, the filenames are constructed like<br>
   *    snapshotName_wo_ext[ch:ChannelIndex:].snapshotName_ext<br>
   *    For example: snapshot[0].jpg for the first channel by default
   *  @param snapshotName This is the filename for the snapshot with extension
   *  @see
   *    getSnapshotName
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
   *    This is the index of the source you want the snapshot from.<br>
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


  //------------------------------------------------------------------------
  // Record methods
  //------------------------------------------------------------------------


  /*! @brief Set the recording filenames for all defined channels
   *  @param
   *    recordName This is the filename for the snapshot with extension
   *  @remarks
   *    In the mono case, filename is used without changes<br>
   *    In the stereo case, the filenames are constructed like<br>
   *    recordName_wo_ext[ch:CameraIndex:].recordName_ext.<br>
   *    For example: startRecording[ch0].avi for the first camera by default
   *  @see
   *    setRecordName, getRecordName
   *
   */
  void setRecordName(const std::string& recordName);

  /*! @brief Set the recording filename for the given camera
   *
   *    The file format is chosen by the ending of the filename, the file encoding depends on the
   *    parameter of startRecording().
   *  @param
   *    channel This is the index of the source you want the picture from.<br>
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
  const std::string getRecordName(unsigned int channel=0) const;

  /*! @brief Initialize and start recording
   *
   *      If the grabbing thread isn't running, startRecording will start the Thread
   *      and stopRecording will stop it.
   *
   * Create the VideoWriter structures and set the record flag.
   *  @param
   *      fps Defines the framerate of the recorded avi-file. It is independent from the speed of
   *      the grabbing thread. But the number of pictures will be the same. So if you grab on a slower speed than
   *      this fps-parameter, the recorded video will be shorter.
   *  @param
   *      recFormat Determins the format of the recordings
   *      Default is cedar::dev::sensors::visual::RecordingFormat::RECORD_RAW for raw recording
   *  @param
   *      color Determins if recording is in color or black/white mode.
   *      Default value is true.
   *  @param
   *      startThread Determins if the grabberthread should be started on recording
   *      Default value is true.
   *
   *  @remarks
   *      Always all cameras recording. To set the filename use setRecordName()<br>
   *      By default record.avi is used as filename (in the mono case) <br><br>
   *      The transcoding is entirely done via the opencv-API.<br>
   *      Look at the OPENCV manual to determine the usable FOURCC's
   *
   *  @throw GrabberRecordingException Thrown, if not all channels could be recorded
   *
   *  @see
   *      setRecordName, getFramerate, VideoGrabber::getSourceFps
   */

  void startRecording
  (
    double fps,
    cedar::dev::sensors::visual::RecordingFormat::Id recFormat
      = cedar::dev::sensors::visual::RecordingFormat::RECORD_RAW,
    bool color = true,
    bool startThread = true
  );

  /*! @brief Stop all recordings
   *  @remarks
   *    All VideoWriter structures are released. There is no possibiltiy to append another recording.<br>
   *    If recording will be restarted without changing the recording filenames, the old files will be
   *    overwritten
   */
  void stopRecording();

  /*! @brief Get the state of the recording-flag */
  bool isRecording() const;

  /*! @brief Defines the additions to the filename.
   *
   *   This method will be used in a stereo-grabber if you use setSnapshotName("SnapshotFilename")
   *   or setRecordName("RecordFilename") to set both filenames at once
   *
   *  @param channel
   *    This is the index of the source you want the snapshot from.
   *
   *  @remarks The extension is hardcoded to "_ch<channel>", for example "_ch0" for channel 0.
   *
   */
  std::string getChannelSaveFilenameAddition(int channel) const;


  /*! @brief Set the grabbing-flag when the grabbing is done manually
   *
   *   You have to set/unset this, if your grabbing is done through a periodic call of grab() method
   *   and not with the build-in thread.
   *
   *   Also used in the processing Step "Camera" when running with a looped trigger
   */
  void setIsGrabbing(bool isGrabbing);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*! @brief  Periodically calls the grab() method.
   *
   *  For details have a look at cedar::aux::LoopedThread
   */
  void step(cedar::unit::Time time);

  /*! @brief Call this method at the beginning of the destructor in the derived class
   *
   *      This method does the necessary clean up (like stop recording or stopping the grabbing thread)
   *      and then it invokes the onCleanUp() method of the derived class.<br><br>
   *      Note:<br>You have to call this method in the destructor of a derived class.<br><br>
   *  @remarks
   *      This method will also be invoked by the static emergencyCleanup() member
   *  @see onCleanUp
   */
  void doCleanUp();

  //------------------------------------------------------------------------
  // For derived classes
  //------------------------------------------------------------------------


  /*! @brief Create the channels, initializes them and read the first frames.
   */
  virtual void onCreateGrabber() = 0;

  /*! @brief Resets the Grabber to initialization values, closes previously opened cv::VideoCaptures or similar
   *    used classes.
   *
   *    Call this method if you want to change settings of the capture device, which couldn't be set if the device is
   *    already opened (like the size of the frames of a camera-grabber)
   *
   *  @remarks For Grabber developers<br>
   *    You have only reset the derived class things.
   *    The recording, the grabbing-thread, the image-mat and those things introduced in the base-class are already
   *    reset in the closeGrabber() method of the base-class
   */
  virtual void onCloseGrabber() = 0;

  /*! @brief  This method is invoked during destruction of the class.
   *
   *      This function is invoked by the doCleanUp method in the Grabber.
   *      Override this method in the derived class and do only the absolutely necessary cleanup here.<br><br>
   *
   *  @note
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
   *  @throw cedar::dev::sensors::visual::GrabberGrabException Thrown, if the grabbed image is empty
   *      or if the grabbing fails
   *  @param channel The channel to grab from
   */
  virtual void onGrab(unsigned int channel) = 0;


  /*! @brief Get the channel informations from the derived grabber-classes
   *
   *    This method is used internally to update the channel info string. It will be called after the onCreate()-method
   *    of the derived grabber-classes is invoked.
   *
   *  @param channel The channel which to get the informations
   */
  virtual std::string onGetSourceInfo(unsigned int channel) = 0;

  /*!@brief Get the Image buffer
   *
   *    Before this buffer can be read out or written in, it have to be locked with the mpReadWriteLock() lock!
   *
   *  @param channel This is the index of the source you want to print the properties.
   *  @throw cedar::aux::IndexOutOfRangeException Thrown, if channel doesn't fit to number of channels
   */
  cv::Mat& getImageMat(unsigned int channel);

  /*! @brief Updates the info string of the given channel
   *
   *   Call this method in the derived class in the method onGetSourceInfo()
   *   to update the infos about the channel source
   *
   *  @param channel The channel you want to update
   *  @param info The new info-string for that channel
   */
  void setChannelInfoString(unsigned int channel, std::string info);

  /*! @brief Set the internal used flag with locking
   *
   *  @param isCreated Flag, if the Grabber is already created or not
   */
  void setIsCreated(bool isCreated);

  /*! @brief Create the out of range error message for the channel parameter
   *
   *    This message is used internally to build the error message for the range check of the paramter "channel"
   *  @param channel The channel to create the message
   */
  inline std::string buildChannelErrorMessage(unsigned int channel) const
  {
    std::string msg = "Channel " + cedar::aux::toString(channel) + "is out of range! Only channel 0";
    if (this->getNumChannels() > 1)
    {
      msg = msg + ".." + cedar::aux::toString(this->getNumChannels());
    }
    msg = msg + " available";
    return msg;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  /*! @brief This function does internal variable initialization in grabber constructor
   *  @param grabberChannels A vector with all channels you want to initialize
   */
  void init(std::vector<cedar::dev::sensors::visual::GrabberChannelPtr> grabberChannels);

  /*! @brief Callback function to respond to a captured CTRL-C event
   *
   *      This function calls the static emergencyCleanup function of all registered grabbers and then exit(1).
   *  @see
   *     emergencyCleanup, doCleanUp, installCrashHandler
   *  @param signalNo The captured signal. Only CTRL-C is implemented
   */
  static void interruptSignalHandler(int signalNo);

  /*! Get the pointer to the channel structure of the specified channel
   *  @param channel The channel number of the wanted channel structure
   */
  inline GrabberChannelPtr getGrabberChannel(unsigned int channel)
  {
    return _mChannels->at(channel);
  }

  /*! Get the const pointer to the channel structure of the specified channel
   *  @param channel The channel number of the wanted channel structure
   */
  inline GrabberChannelPtr getGrabberChannel(unsigned int channel) const
  {
    return _mChannels->at(channel);
  }

  /*! @brief  Called when the grabbing thread is started.
   *
   *          This method invokes internally LoopedThread::start() and does
   *          some initialization due to the measurement of FPS. <br>
   *
   *          To control the grabbing speed (i.e. the FPS) use
   *          setFramerate(), getFramerate() or getMeasuredFramerate()
   */
  void prepareStart();

  /*! @brief Called when grabbing thread quits (finishes)
   *
   *          This method does
   *          some cleanup like stopRecording or set the measured FPS to zero
   */
  void processQuit();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:

    //! @brief Read/write lock used for concurrent access to the mImageMatVector - matrices
    QReadWriteLock* mpReadWriteLock;
    
    //! @brief The actual measured fps of grabbing
    double mFpsMeasured;
    
    /*! @brief  Flag if recording is on     */
    bool mRecording;

private:

    //! @brief Flag which indicates if the capture devices of all channels are correctly created or not
    bool mCaptureDeviceCreated;

    //! @brief Used for concurrent access to the mCaptureDeviceCreated flag
    QReadWriteLock* mpLockCaptureDeviceCreated;

    //! Flag, that determins if grabbing is on either via the LoopedThread or via the LoopedTrigger from the gui
    bool mIsGrabbing;

    //! @brief Used for concurrent access to the mIsGrabbing flag
    QReadWriteLock* mpLockIsGrabbing;

    //! @brief Used for blocking other threads while grabber is created
    QReadWriteLock* mpLockIsCreating;

    //! @brief Flag which indicates if the GrabberThread was started during startRecording
    bool mGrabberThreadStartedOnRecording;

    //! @brief Flag which indicates if the CleanUp was already done (perhaps due to an error)
    bool mCleanUpAlreadyDone;
    

    //! @brief Timestamp at start time to measure the real fps of grabbing
    boost::posix_time::ptime mFpsMeasureStart;

    //! @brief Timestamp at end time to measure the real fps of grabbing
    boost::posix_time::ptime mFpsMeasureStop;

    //! @brief Counter to measure the real fps of grabbing
    unsigned int mFpsCounter;

    //! @brief The vector containing references to the instances of all created grabbers
    static GrabberInstancesVector mInstances;

    //! @brief Flag which indicates if the CTRL-C Handler should be registered or not
    static bool mRegisterTerminationHandler;

    //! @brief Flag, if this is the first instance of a grabber (used for the ctrl-c handler)
    bool mFirstGrabberInstance;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //------------------------------------------------------------------------
  //! @brief Constant which defines how often getMeasuredFramerate() will be updated (in frames).
  static const int UPDATE_FPS_MEASURE_FRAME_COUNT = 5;

  //! @brief A vector which contains for every grabbing-channel one channel structure
  ChannelParameterPtr _mChannels;

private:

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
