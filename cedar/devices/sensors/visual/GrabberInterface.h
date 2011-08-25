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
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::devices::sensors::visual::GrabberInterface class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBER_INTERFACE_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBER_INTERFACE_H

// LOCAL INCLUDES
#include "namespace.h"
#include "auxiliaries/LoopedThread.h"
#include "exceptions/GrabberRecordingException.h"
#include "exceptions/GrabberSnapshotException.h"

// PROJECT INCLUDES
#include <auxiliaries/exceptions/InitializationException.h>
#include <auxiliaries/exceptions/IndexOutOfRangeException.h>
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>

// SYSTEM INCLUDES
#include <string>
#include <vector>


// ------------------------------------------------------------------------------------------------------------------ //




    // ------------------------------------------------------------------------------------------------
    // class GrabberInterface
    // ------------------------------------------------------------------------------------------------


    /*! \class GrabberInterface
     *  \brief This is the base class for all grabber.

        \remarks
            This class implements all the common features, each grabber has to be capable of.
            That is e.g. managing the configuration file, recording, threaded grabbing and stuff like this.
            The name of the grabber is stored in _mName which is defined in cedar::aux::Base

        \remarks
            Initialize in the constructor of a derived class the filenames or camera-device-names.
            At the end of the constructor call doInit with grabbername and the number of channels you use.
            To get an example look at the VideoGrabber-class.
    */

    class cedar::dev::sensors::visual::GrabberInterface : public cedar::aux::LoopedThread
    {


      //----------------------------------------------------------------------------
      // constructor and destructor
      //----------------------------------------------------------------------------


      public:

        /// Constructor...
        GrabberInterface(std::string configFileName = "");

        /// Destructor...
        virtual ~GrabberInterface();


      //------------------------------------------------------------------------
      // Internal members
      //------------------------------------------------------------------------

      protected:
            
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
             *  	for each capture one */
            std::vector<cv::VideoWriter> mVideoWriterVector;

            /*! @brief  Flag if recording is on     */
            bool mRecord;

            /*! @brief  mNumCams should be used instead of mImageMatVector.size()
             * \remarks
             * 		Initialization should be done in the construktor of the derived class.
             */
            unsigned int mNumCams;


            /*! @brief Read/write lock
             *  \remarks
             *      Used for concurrent access to the mImageMatVector - matrices
             */
            QReadWriteLock* mpReadWriteLock;
            

            
            /*! @brief  Release all cv::Mat and CvCaptures...
             *  \remarks
             *    Not used here. std::vector<> calls the destructor for all containing objects
             */
            void releaseImagesAndCaptures    ();


            /*! @brief  This function initialize the common grabber.
             *  \remarks  Have to be called at the end of the constructor of the derived class. <br><br>
             * 		The initialization is a little bit tricky, but it restores and initializes all 
             * 		local parameters from the configuration interface before onInit of the 
             * 		derived class will be invoked.<br>
             * 		The single steps are executed in the following order:
             *    1. call of declareParameter() from derived class. <br>
             *    2. restore parameter from configfile or set default values. <br>
             *    3. call onInit() from derived class. <br>
             *        In this function you should initialize your grabbers. <br>
             *    For an example look at \see VideoGrabber or NetGrabber
             *  \see onInit, declareParameter
             *
             */
            void doInit(std::string& grabberName, unsigned int numCams);

            /*! @brief  Periodically call of grab()
             *  \remarks For details have a look at cedar::aux::LoopedThread
             */
            void step(double time);
      
            //inline void checkNumCams(const std::string& exceptionInfo ) const;


      
      //------------------------------------------------------------------------
      // For derived classes
      //------------------------------------------------------------------------

      protected:

            /*! @brief  This method is called during initialization.
                \remarks
                    Override this method in the derived classes to do their initialization.<br>
                    The Vector mImageMatVector have to be initialized with the right amount of matrices in this function.<br>
                    This function is called in the /see GrabberInterface::GrabberInterface after initialization is complete.<br>
                    Parameters from the configfile restored before onInit is invoked. So don't overwrite them.<br>
                    For examples, look at the classes VideoGrabber, CameraGrabber and PictureGrabber.
            */
            virtual bool    onInit  ()  { return true; }

            /*! @brief  This is the grabbing method.
                \remarks
                    This method is called by grab() and has to be implemented by the inherited grabber.
                    This is where the grabbing takes place.
            */
            virtual bool    onGrab  () { return true; }
           
            /*! @brief  Declare parameters which should be saved in config-file
             *  \remarks
             *    Should be overrided in derived class.<br>
             *    Declare parameters for derived class in this method.<br>
             * 	  Saved values are loaded at the beginning of GrabberInterface.init()
             * 	\see doInit
             */
            virtual bool   onDeclareParameters () { return true; } ;
            
            /*! \brief Get information about the used device, i.e. the filename or the mount-point
                 \remarks
                     Usefull if you are using multiple-channels and you want to check the sources.
                     Have to be overrided in derived class to supply correct informations
                 \param channel
                     This is the index of the source you want the name of the source from.<br>
                     In the mono case you do not need to supply this value. Default is 0.<br>
                     In the stereo case it may be 0 or 1.
             */
            virtual std::string  getPhysicalSourceInformation(unsigned int channel=0) const = 0; 
            
           

      //------------------------------------------------------------------------
      // The public GrabberInterface to interact with grabber
      //------------------------------------------------------------------------

        public:

            /*! \brief Get the number of currently used channels (sources).
                \remarks
                    Works exactly the same way if image or avi files are used.
            */
            unsigned int getNumCams()  const;


            /*! \brief Get the size of a specified camera-channel.
                 \remarks
                     Gets the size of the Image as CvSize.
                 \throw cedar::aux::exc::IndexOutOfRangeException
                 \param channel
                     This is the index of the source you want the size of the picture from.<br>
                     In the mono case you do not need to supply this value. Default is 0.<br>
                     In the stereo case it may be 0 or 1.
             */
             CvSize  getSize (unsigned int channel = 0) const;


			/*! \brief Get the framerate of the loopedthread speed for grabbing
			 *  \remarks
			 * 		This value doesn't indicate, if the thread is running or not.
			 * 	\see LoopedThread::start(), LoopedThread::stop()
			 */
      double getFPS() const;

			/*! \brief Set the framerate of the loopedthread speed for grabbing
			 *  \remarks
			 * 		This value doesn't indicate, if the thread is running or not.
			 * 		The LoopedThread have to be restarted for changing the framerate.
			 * 		This is done in this function, but keep it in mind.
			 * 	\see LoopedThread::start(), LoopedThread::stop()
			 */
			void setFPS(double fps);






            //------------------------------------------------------------------------
            // Grabbing
            //------------------------------------------------------------------------


            /*! \brief Grab a new image (or images in stereo case) from the source (channel, picture, avi...).
                \remarks
                    This method is called automatically within a loop by all threaded grabber.<br>
                    If you use not threaded grabber you must call it by yourself.
            */
            bool grab  ();


            /*! \brief Get an Image in a cv::Mat structure
                \remarks
                    With this method you can get the grabbed image.
                \param channel This is the index of the source you want the picture from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
                \see
                    getNumCams
            */
            cv::Mat  getImage(unsigned int channel = 0)  const;

            /*! \brief Get a pointer to the QReadWriteLock.
             *  \remarks
             *      Used for concurrent reading/writing to the image matrices
            */
            QReadWriteLock* getReadWriteLockPointer ()   const;


            //------------------------------------------------------------------------
            // Snapshot functions
            //------------------------------------------------------------------------



            /*! \brief Set the snapshot filenames for all defined channels
             *  \param snapshotName This is the filename for the snapshot with extension
             *  \remarks
             *    In the mono case, filename is used without changes<br>
             *    In the stereo case, the filenames are constructed like<br>
             *    snapshotName_wo_ext[ch:ChannelIndex:].snapshotName_ext<br>
             *    For example: snapshot[0].jpg for the first channel by default
             *  \see
             *    setSnapshotName
             *  \see
             *    getSnapshotName
             */
            void setSnapshotName (const std::string& snapshotName);


            /*! \brief Set the snapshot filename for the given channel
             *

                 The image format is chosen based on the filename extension.

                 \param channel This is the index of the source you want the picture from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
                 \param
                     snapshotName This is the filename for the snapshot with extension

                 \remarks
                  Currently, the following file formats are supported by OpenCV:
                <pre>
                  always supported:
                    Windows bitmaps            *.bmp, *.dib
                    Portable image format      *.pbm, *.pgm, *.ppm
                    Sun rasters                *.sr, *.ras
                </pre>
                <pre>
                 see the OpenCV documentation (depends on system and installed libs on the system)
                    JPEG files                 *.jpeg, *.jpg, *.jpe
                    JPEG 2000 files            *.jp2
                    Portable Network Graphics  *.png
                    TIFF files                 *.tiff, *.tif
                </pre>
                \see
                    getSnapshotName
                \see
                    saveSnapshot

              */
            void setSnapshotName (unsigned int channel, const std::string& snapshotName );

            /*! \brief Get the current name of the snapshot file.
                 \param channel This is the index of the source you want the snapshot from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
            */
            std::string getSnapshotName(unsigned int channel) const;


            /*! \brief Save a snapshot to a file.
                \remarks
                    Save only one snapshot from the given channel
                \param
                    channel is the index of the source you want the snapshot from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
            */
            bool saveSnapshot        (unsigned int channel = 0)     const;

            /*! \brief Save snapshots of all channels
             *  \remarks
             *    For every capture-device saveSnapshot() is invoked
             *  \see
             *    saveSnapshot
             */
            bool saveSnapshotAllCams   ()                  const;




            //------------------------------------------------------------------------
            // Record functions
            //------------------------------------------------------------------------



            /*! \brief Set the recording filenames for all defined channels
             *  \param
             *    recordName This is the filename for the snapshot with extension
             *  \remarks
             *    In the mono case, filename is used without changes<br>
             *    In the stereo case, the filenames are constructed like<br>
             *    recordName_wo_ext[ch:CameraIndex:].recordName_ext.<br>
             *    For example: startRecording[ch0].avi for the first camera by default
             *  \see
             *    setRecordName, getRecordName

             */
            void setRecordName (const std::string& recordName);


            /*! \brief Set the recording filename for the given camera
                 \param
                    channel This is the index of the source you want the picture from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
                 \param
                   recordName This is the filename for the startRecording with extension
        \remarks
                 The file format is chosen by the init-string in startRecording().<br>
                \see
                    startRecording, setRecordName, getRecordName
              */
            void setRecordName (unsigned int channel, const std::string& recordName );

            /*! \brief Get the current name of the recording file for the given camera
                 \param
                    camera This is the index of the source you want the picture from.<br>
                    In the mono case you do not need to supply this value. Default is 0.<br>
                    In the stereo case it may be 0 or 1.
            */
            std::string getRecordName(unsigned int channel) const;

            
            /*! \brief Initialize and start recording
             *
             * Create the VideoWriter structures and set the record flag.
                \param
                    fps Defines the framerate of the avi-file
                \param
                    fourcc 4-character code of codec used to compress the frames.<br>
                    For example, CV_FOURCC('P','I','M,'1') is a MPEG-1 codec, CV_FOURCC('M','J','P','G') is a motion-jpeg codec etc.<br>
                    Or CV_FOURCC('M','P','4','2')
                    Default is 0, for raw recording
                \param
                    color Determins if recording is in color or black/white mode.
                    Default value is true.

                \remarks
                    Always all cameras recording. To set the filename use setRecordName()<br>
                    By default record.avi as filename is used (in the mono case) <br><br>
                    The transcoding is entirely done via the opencv-API.<br>
                    So see the OPENCV manual to determine the usable FOURCC's
                \par
                   for supported codecs, have a look at:<br>
                   /usr/local/src/OpenCV_<YOUR_VERSION>/modules/highgui/src/cap_ffmpeg.cpp<br>
                   http://www.fourcc.org/codecs.php<br>

                \see
                    setRecordName
            */
            //virtual bool startRecording        (double fps, int fourcc=CV_FOURCC('M','P','4','2'), bool color=true)    = 0;
            bool startRecording        (double fps, int fourcc=0, bool color=true);

            /*! \brief Stop all recordings
             *  \remarks
             *    All VideoWriter structures are released. There is no possibiltiy to append another recording.<br>
             *    If recording will be restarted without changing the recording filenames, the old files will be overwritten
             */
            bool stopRecording   ();



    }; // class GrabberInterface
    



#endif
