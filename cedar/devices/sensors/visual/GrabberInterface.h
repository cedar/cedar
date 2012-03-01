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

    File:        GrabberInterface.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::devices::sensors::visual::GrabberInterface class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBER_INTERFACE_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBER_INTERFACE_H

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/defines.h"
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/devices/sensors/visual/Exceptions.h"
#include "cedar/auxiliaries/exceptions/InitializationException.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>
#include <boost/utility.hpp>      //boost::noncopyable

#ifdef ENABLE_CTRL_C_HANDLER
  #include <signal.h>               //CTRL-C handler
#endif


#ifdef ENABLE_CTRL_C_HANDLER

  /* \brief Typedef for a vector containing the instances of all used grabbers
   *
   */
  typedef std::vector<cedar::dev::sensors::visual::GrabberInterface*> GrabberInstancesVector;
#endif


/*! \class cedar::dev::sensors::visual::GrabberInterface
 *  \brief This is the base class for all grabber.
 *
 *  \remarks
 *      This class implements all the common features, each grabber has to be capable of.
 *      That is e.g. managing the configuration file, recording, threaded grabbing and other stuff like this.
 *      The name of the grabber is stored in _mName which is defined in cedar::aux::Base
 *
 *   USAGE:
 *     - to set a name for the grabber: getName, setName form cedar::base<br>
 *     - to control the grabbing-thread: <br>
 *             => isRunning() : from QThread<br>
 *             => start(), stop() <br>
 *             => setFps(), getFps(), getFpsMeasured <br>
 *     - grabbing: <br>
 *             => grab manually a new image: grab()  <br>
 *             => get the image from the internal buffer: getImage() <br>
 *     - recording: startRecording(), stopRecording()  <br>
 *     - snapshots: saveSnapshot(), saveSnapshotAllCams(), getSnapshotName()<br>
 *
 *      <br><br>
 *      If you would like to create your own derived grabber:
 *      Initialize in the constructor of a derived class the filenames or camera-device-names.
 *      At the end of the constructor, call doInit() with the number of channels you use and with a default name.
 *      At the beginning of the destructor, call doCleanUp() and do the cleanup in this function.
 *      To get an example look at the VideoGrabber-class. <br><br>
 *
 *
 */
class cedar::dev::sensors::visual::GrabberInterface
:
public cedar::aux::LoopedThread,
public boost::noncopyable
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

protected:
  /*! @brief The standard constructor.
   *  \remarks
   *    The constructor is protected, because no instance of GrabberInterface should be instantiated
   *    Use a derived class instead.
   *  \param
   *    configFileName The filename where the configuration parameters should be stored in
   */
  GrabberInterface(
                    const std::string& configFileName// = ""
                  );

public:

  //!@brief Destructor
  virtual ~GrabberInterface();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

    /*! \brief Get the number of currently used channels (sources).
     */
    unsigned int getNumCams() const;

    /*! \brief Get the size of a specified camera-channel.
     *   \remarks
     *       Gets the size of the Image as cv::Size.
     *   \throw cedar::aux::exc::IndexOutOfRangeException
     *   \param channel
     *       This is the index of the source you want the size of the picture from.<br>
     *       In the mono case you do not need to supply this value. Default is 0.<br>
     *       In the stereo case it may be 0 or 1.
     */
    cv::Size getSize(
                      unsigned int channel = 0
                    ) const;

    /*! \brief Get information about the used device, i.e. the filename or the mount-point
     *   \remarks
     *       Usefull if you are using multiple-channels and you want to check the sources.
     *       Have to be overrided in derived class to supply correct informations
     *   \param channel
     *       This is the index of the source you want the name of the source from.<br>
     *       In the mono case you do not need to supply this value. Default is 0.<br>
     *       In the stereo case it may be 0 or 1.
     *   \see onGetPhysicalSourceInformation
     */
    virtual std::string getSourceInfo(
                                       unsigned int channel = 0
                                     ) const;

    //------------------------------------------------------------------------
    //Thread related methods
    //------------------------------------------------------------------------

    /*! \brief Get the framerate which loopedthread is set for grabbing
     *  \remarks
     *          This value doesn't indicate, if the thread is running or not.
     *  \see start(), stop(), QThread::isRunning()
     */
    double getFps() const;

    /*! \brief Set the framerate of the loopedthread speed for grabbing
     *  \remarks
     *          This value doesn't indicate, if the thread is running or not.
     *          The LoopedThread have to be restarted for changing the framerate.
     *          This is done in this function, but keep it in mind.
     *  \see start(), stop()
     */
    void setFps(
                 double fps
               );

    /*! \brief Get the framerate of actual grabbing speed
     *  \remarks
     *          Calculates the framerate by counting the invocation of the grab() method
     *          This value is updated every 5 (default value set in defines.h) grabbed frames
     */
    double getFpsMeasured() const;

    /*! \brief Stop the grabbing thread
     *  \remarks
     *          This method invokes internally LoopedThread::stop() and does
     *          some cleanup like stopRecording or set the measured FPS to zero
     */
    void stop();

    /*! \brief Start the grabbing thread
     *  \remarks
     *          This method invokes internally LoopedThread::start() and does
     *          some initialization due to the measurement of FPS.
     *  \par
     *          To control the grabbing speed (i.e. the FPS) use
     *          setFps(), getFps() or getFpsMeasured()
     */
    void start();


    //------------------------------------------------------------------------
    //Grabbing methods
    //------------------------------------------------------------------------

    /*! \brief Get an Image in a cv::Mat structure
     *  \remarks
     *      With this method you can get the grabbed image.
     *  \param channel
     *		This is the index of the source you want the picture from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     *  \see
     *      getNumCams
     */
    cv::Mat getImage(
                      unsigned int channel = 0
                    ) const;


    /*! \brief Grab a new image (or images in stereo case) from the source (channel, picture, avi...).
     *  \remarks
     *      This method is called automatically by the grabber thread.<br>
     *      If you don't use the grabber thread you have to call it by yourself to get new images.
     *  \see 
     *      start()
     *      setFps()
     *      
     */
    bool grab();

    /*! \brief Get a pointer to the QReadWriteLock.
     *  \remarks
     *      Used for concurrent reading/writing to the image matrices
     */
    QReadWriteLock *getReadWriteLockPointer() const;


    //------------------------------------------------------------------------
    //Snapshot methods
    //------------------------------------------------------------------------

    /*! \brief Set the snapshot filenames for all defined channels
     *  \param snapshotName This is the filename for the snapshot with extension
     *  \remarks
     *		In the mono case, filename is used without changes<br>
     *		In the stereo case, the filenames are constructed like<br>
     *		snapshotName_wo_ext[ch:ChannelIndex:].snapshotName_ext<br>
     *		For example: snapshot[0].jpg for the first channel by default
     *  \see
     *		setSnapshotName
     *  \see
     *		getSnapshotName
     */
    void setSnapshotName(
                          const std::string& snapshotName
                        );

    /*! \brief Set the snapshot filename for the given channel
     *
     *
     *   The image format is chosen based on the filename extension.
     *
     *   \param channel This is the index of the source you want the picture from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     *   \param
     *       snapshotName This is the filename for the snapshot with extension
     *
     *   \remarks
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
     *  \see
     *      getSnapshotName
     *  \see
     *      saveSnapshot
     *
     */
    void setSnapshotName(
                          unsigned int channel,
                          const std::string& snapshotName
                        );

    /*! \brief Get the current name of the snapshot file.
     *  \param channel
     *		This is the index of the source you want the snapshot from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     */
    std::string getSnapshotName(
                                 unsigned int channel = 0
                               ) const;

    /*! \brief Save a snapshot to a file.
     *  \remarks
     *      Save only one snapshot from the given channel
     *  \param
     *      channel is the index of the source you want the snapshot from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     */
    bool saveSnapshot(
                       unsigned int channel = 0
                     ) const;

    /*! \brief Save snapshots of all channels
     *  \remarks
     *    For every capture-device saveSnapshot() is invoked
     *  \see
     *    saveSnapshot
     */
    bool saveSnapshotAllCams() const;
    
    /*! \brief Write the actual used parameters to configuration
     *  \remarks
     *    The configuration have to be saved manually with this function.
     *    This method evokes onWriteConfiguration of the grabber
     *  \see onWriteConfiguration
     */
    bool writeConfiguration();


    //------------------------------------------------------------------------
    //Record methods
    //------------------------------------------------------------------------


    /*! \brief Set the recording filenames for all defined channels
     *  \param
     *		recordName This is the filename for the snapshot with extension
     *  \remarks
     *		In the mono case, filename is used without changes<br>
     *		In the stereo case, the filenames are constructed like<br>
     *		recordName_wo_ext[ch:CameraIndex:].recordName_ext.<br>
     *		For example: startRecording[ch0].avi for the first camera by default
     *  \see
     *		setRecordName, getRecordName
     *
     */
    void setRecordName(
                        const std::string& recordName
                      );

    /*! \brief Set the recording filename for the given camera
     *   \param
     *		channel This is the index of the source you want the picture from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     *   \param
     *     recordName This is the filename for the startRecording with extension
     * \remarks
     *		The file format is chosen by the init-string in startRecording().<br>
     *  \see
     *      startRecording, setRecordName, getRecordName
     */
    void setRecordName(
                        unsigned int channel,
                        const std::string& recordName
                      );

    /*! \brief Get the current name of the recording file for the given camera
     *   \param
     *      channel This is the index of the source you want the picture from.<br>
     *      In the mono case you do not need to supply this value. Default is 0.<br>
     *      In the stereo case it may be 0 or 1.
     */
    std::string getRecordName(
                               unsigned int channel = 0
                             ) const;

    /*! \brief Initialize and start recording
     *  \remarks
     *      If the grabbing thread isn't running, startRecording will start the Thread
     *      and stopRecording will stop it.
     *
     * Create the VideoWriter structures and set the record flag.
     *  \param
     *      fps Defines the framerate of the recorded avi-file. It is independent from the speed of
     *      the grabbing thread. But the number of pictures will be the same. So if you grab on a slower speed than 
     *      this fps-parameter, the recorded video will be shorter.
     *  \param
     *      fourcc 4-character code of codec used to compress the frames.<br>
     *      Examples:<br>
     *      CV_FOURCC('P','I','M,'1') is a MPEG-1 codec<br>
     *		  CV_FOURCC('M','J','P','G') is a motion-jpeg codec<br>
     *      CV_FOURCC('M','P','4','2') is also a motion-jpeg codec<br>
     *      Default is 0, for raw recording
     *  \param
     *      color Determins if recording is in color or black/white mode.
     *      Default value is true.
     *
     *  \remarks
     *      Always all cameras recording. To set the filename use setRecordName()<br>
     *      By default record.avi is used as filename (in the mono case) <br><br>
     *      The transcoding is entirely done via the opencv-API.<br>
     *      Look at the OPENCV manual to determine the usable FOURCC's
     *  \par
     *     for supported codecs, have a look at:<br>
     *     /usr/local/src/OpenCV_{YOUR_VERSION}/modules/highgui/src/cap_ffmpeg.cpp<br>
     *     http://www.fourcc.org/codecs.php<br>
     *
     *  \see
     *      setRecordName, getFps, VideoGrabber::getSourceFps
     */
    bool startRecording(
                         double fps,
                         int fourcc = 0,
                         bool color = true
                       );

    /*! \brief Stop all recordings
     *  \remarks
     *		All VideoWriter structures are released. There is no possibiltiy to append another recording.<br>
     *		If recording will be restarted without changing the recording filenames, the old files will be
     *		overwritten
     */
    bool stopRecording();

    /*! \brief Get the state of the recording-flag
     *
     */
    bool isRecording() const;


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:


    /*! @brief  This function initialize the common grabber.
     *  \remarks  Have to be called at the end of the constructor of the derived class. <br><br>
     *          This is the first part of the initialization.
     *          The initialization is a little bit tricky, but it restores and initializes all
     *          local parameters from the configuration interface before onInit of the
     *          derived class will be invoked.<br><br>
     *          On the other side, it is possible to change restored parameter in the constructor of a
     *          derived class between this two steps (i.e. readInit(...); YOUR_PARAMETER_CHANGE; applyInit() )
     *
     *  \par
     *          If onInit() fails, onCleanUp() will be invoked in order to clean up alread initialized channels.<br><br>
     *          For an example look at VideoGrabber, NetGrabber or TestGrabber
     *  \see onInit, declareParameter, onCleanUp
     *
     *  \param numCams The init-function need to know how many channels there are
     *  \param defaultGrabberName This name is used as a default name, which will
     *         be stored in the configuration file. To change the name
     *  \see cedar::aux::ConfigurationInterface::setName
     */
    void readInit(
                 unsigned int       numCams,
                 const std::string& defaultGrabberName
               );
    /*! @brief This function applies the former read initialization and then it calls onInit in derived class
     *
     *   \see onInit()
     */
    void applyInit();

    /*! @brief  Periodically call of grab()
     *  \remarks For details have a look at cedar::aux::LoopedThread
     */
    void step(
               double time
             );

    /*! @brief Call this method at the beginning of the destructor in the derived class
     *  \remarks This method does the necessary clean up (like stop recording or stop the grabberthread)
     *      and then it invokes the onCleanUp() method of the derived class.<br><br>
     *      Note:<br>You have to call this method in the destructor of a derived class.<br><br>
     *  \see onCleanUp
     */
    void doCleanUp();
    
    //------------------------------------------------------------------------
    //For derived classes
    //------------------------------------------------------------------------


    /*! @brief  This method is called during initialization.
     *  \remarks
     *      Override this method in the derived classes to do their initialization.<br>
     *      The Vector mImageMatVector have to be initialized with the right amount of matrices in this function.<br>
     *      This function is called in the /see GrabberInterface::GrabberInterface after initialization is
     *      complete.<br>
     *      Parameters from the configfile restored before onInit is invoked. So don't overwrite them.<br>
     *      For examples, look at the classes VideoGrabber, CameraGrabber and PictureGrabber.
     */
    virtual bool onInit() { return true; }

    /*! @brief  This method is invoked during destruction of the class.
     *  \remarks
     *      This function is invoked by the doCleanUp method in the GrabberInterface.
     *      Override this method in the derived class and do only the absolutely necessary cleanup here.<br><br>
     *      
     *   \note
     *      This method is also invoked by the CTRL-C handler. Therefore, it is the only possibility
     *      to do the necessarily cleanup like hardware-reinit, shutting down cameras or to close file handles.
     *
     *      The dynamic allocated memory of the class should be freed in the standard destructor.  
     *  \see doCleanUp
     */
    virtual void onCleanUp() { }

    /*! @brief  This is the grabbing method.
     *  \remarks
     *      This method is called by grab() and has to be implemented by the inherited grabber.
     *      This is where the grabbing takes place.
     */
    virtual bool onGrab() { return true; }

    /*! @brief  Declare parameters which should be saved in config-file
     *  \remarks
     *		Should be overrided in derived class.<br>
     *		Declare parameters for derived class in this method.<br>
     *		Saved values are loaded at the beginning of GrabberInterface.init()
     *  \see doInit
     */
    virtual bool onDeclareParameters() { return true; };

    /*! @brief  In this method you can re-check or modify parameters
     *  \remarks
     *    Should be overrided in derived class.<br>
     *    Modify or actualize parameters declared in onDeclareParameters.<br>
     *    This method only will be evoked right before the parameters are saved with
     *    cedar::aux::ConfigurationInterface::writeConfiguration().<br>
     *    Save your Parameters with GrabberInterface::writeConfiguration()
     *  \see onDeclareParameters, writeConfiguration
     */
    virtual bool onWriteConfiguration() { return true; };


    /*! \brief Get information about the used device, i.e. the filename or the mount-point
     *   \remarks
     *		You have to implement this method in the derived class. Set the informations
     *		about the channels there. This is the only true virtual function of the Grabberinterface.
     *   \param channel
     *		This is the index of the source you want the name of the source from.<br>
     *		In the mono case you do not need to supply this value. Default is 0.<br>
     *		In the stereo case it may be 0 or 1.
     */
    virtual std::string onGetSourceInfo(
                                         unsigned int channel = 0
                                       ) const = 0;


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  #ifdef ENABLE_CTRL_C_HANDLER
    /*! \brief Callback function to respond to a captured CTRL-C event
     *   \remarks
     *      This function calls the doCleanUp function of all registerd grabbers and then exit().
     *   \see
     *      doCleanUp
     *   \param signalNo The captured signal. Only CTRL-C is implemented
     */
    static void sigIntHandler(int signalNo);

    /*! \brief The vector containing the instances of all created grabbers
     *
     */
    static GrabberInstancesVector mInstances;
  #endif


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
    /*! @brief  This vector contains the images grabbed by this grabber. One image per channel.
     *
     *    \remarks
     *        For every channel the last grabbed Image is in this vector.<br>
     *    \remarks
     *        The initialization of the matrices have to be done in the
     *        child classes.
     *    \see VideoGrabber::onInit doInit
     */
    std::vector<cv::Mat> mImageMatVector;

    /*! @brief  The names of the snapshots */
    std::vector<std::string> mSnapshotNames;

    /*! @brief  The names of the records  */
    std::vector<std::string> mRecordNames;

    /*! @brief  The names of the VideoWriter classes
     *          for each capture one */
    std::vector<cv::VideoWriter> mVideoWriterVector;

    /*! @brief  Flag if recording is on     */
    bool mRecord;

    /*! @brief  mNumCams should be used instead of mImageMatVector.size()
     * \remarks
     *          Initialization should be done in the constructor of the derived class.
     */
    unsigned int mNumCams;


    /*! @brief Read/write lock
     *  \remarks
     *          Used for concurrent access to the mImageMatVector - matrices
     */
    QReadWriteLock* mpReadWriteLock;
    
    /*! @brief The actual measured fps of grabbing
     *
     */
    double mFpsMeasured;
    

private:
    
    /*! @brief Flag which indicates if the GrabberThread was started during startRecording
     */
    bool mGrabberThreadStartedOnRecording;

    /*! @brief Flag which indicates if the CleanUp was already done (perhaps due to an error)
     */
    bool mCleanUpAlreadyDone;
    
    /*! @brief Timestamp at start time to measure the real fps of grabbing
     */
    boost::posix_time::ptime mFpsMeasureStart;

    /*! @brief Timestamp at end time to measure the real fps of grabbing
     */
    boost::posix_time::ptime mFpsMeasureStop;

    /*! @brief Counter to measure the real fps of grabbing
     */
    int mFpsCounter;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet


}; //class cedar::dev::sensors::visual::GrabberInterface


#endif //CEDAR_DEV_SENSORS_VISUAL_GRABBER_INTERFACE_H
