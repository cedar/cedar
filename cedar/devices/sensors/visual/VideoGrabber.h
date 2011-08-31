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

    File:        VideoGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::VideoGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_VIDEO_GRABBER_H

// LOCAL INCLUDES
#include "GrabberInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES



// ------------------------------------------------------------------------------------------------------------------

/*! \class VideoGrabber
 * 	\brief This grabber grabs images from avi-files
 */
 
    class cedar::dev::sensors::visual::VideoGrabber : public GrabberInterface
    {
  

      //------------------------------------------------------------------------
      // Init 
      //------------------------------------------------------------------------
       public:

            /*! \brief  Constructor for a single-file grabber 
             * 	\param grabberName 		The name for this grabber
             *  \param configFileName	Filename for the configuration
             * 	\param aviFileName		Filename to grab from
             */
            VideoGrabber(std::string configFileName,
                       std::string aviFileName);


            /*! \brief Constructor for a stereo-file grabber 
             * 	\param grabberName 		The name for this grabber
             *  \param configFileName	Filename for the configuration
             * 	\param aviFileName0		Filename to grab from for channel 0
             * 	\param aviFileName1		Filename to grab from for channel 1
             */
            VideoGrabber(std::string configFileName,
                       std::string aviFileName0,
                       std::string aviFileName1);

            /*! \brief Destructor */
            ~VideoGrabber();

      
		//------------------------------------------------------------------------
		// Internal
		//------------------------------------------------------------------------

		protected:
        
            /*! \brief The filenames
             *
             */
            std::vector<std::string> mSourceFileName;

            /*! \brief Indicates if looping is on
             */
            bool        _mLoop;

            /*! \brief Factor for grabbing speed
             *
             *  \remarks the speed stored in the AVI-File will be multiplied with this factor
             *    so effective FPS should be _mSpeedFactor*AVI-Speed
             */
            double      _mSpeedFactor;

            /*! \brief Count of frames of the shortest file
             *  \remarks
             *     Used for scrolling in the avi-file
             *  \see
             *    setPositionRel, getPositionRel, setPositionAbs, setPositionRel
             */
            unsigned int mFramesCount;

           /*! \brief This vector contains the needed captures.
            *   One for every avi-file.
                \see
                    mImageMatVector
            */
            std::vector<cv::VideoCapture> mCaptureVector;


      //------------------------------------------------------------------------
      // Methods
      //------------------------------------------------------------------------

		public:
 
            /*! \brief Turn looping on or off (i.e. restart on end) */
            void    setLoop(bool loop = true);

            /*! \brief set the factor for grabbing speed
             *
             *  \remarks the speed stored in the AVI-File will be multiplied with this factor.<br>
             *   Effective FPS should be SpeedFactor*AVI-Speed<br>
             *
             *  \remarks
             *    The grabberthread have to be restarted to take effect.<br>
             *    This is done internally (by calling setFPS), but keep that in mind.
             *  \see
             *    setFPS
             */
            void    setSpeedFactor(double speedFactor);


            /*! \brief Get the factor for grabbing speed
             *
             *  \remarks the speed stored in the AVI-File will be multiplied with this factor
             *    so effective FPS should be _mSpeedFactor*AVI-Speed
             */
            double  getSpeedFactor();
            
            /*! \brief Set postion in the AVI-Files relative
             *  \param newPositionRel New position in the range is from 0..1
             *  \remarks
             *    Range is from 0..1<br>
             *    For absolute positioning use setPositionAbs()
             */
            void      setPositionRel(double newPositionRel);


            /*! \brief Return the actual position relative to the file size.
             *  \return Range is from 0..1
             */
           double    getPositionRel()   ;


            /*! \brief Set the position in the avi-files absolute
             *  \param newPositionAbs New position in the range is from 0..FrameCount
             *  \remarks
             *      For relative positioning use setPostionRel()
             */
            void  setPositionAbs(unsigned int newPositionAbs);


            /*! \brief Return the position in the file, i.e. the framenumber.
             *  \return Range is from 0..FrameCount 
             */
            unsigned int    getPositionAbs()   ;

            /*! \brief Get the count of frames in the AVI
             *  \remarks
             *    In the case of a stereo grabber and different length
             *    the shortest avi-file determine the length
             */
            unsigned int    getFrameCount() ;



            /*! \brief This passes the arguments directly to the corresponding capture
						 * 	 \remarks	With this Method, it is possible to get Information on any channel.	
						 *   \param channel This is the index of the source you want parameter value.<br>
             *   \param propId This is any supported property-Id<br>
             *     If property-id is not supported or unknown, return value will be 0.
             *   \remarks see OpenCV documentation for VideoCapture::get() for details
             */
             double getAviParam(unsigned int channel,int propId);

             /*! \brief Get fps for the given camera
              *  \remarks
              *    Default channel is 0
              */
             double getAviParamFps (unsigned int channel=0) ;

             /*! \brief Get fourcc for the given camera
              *  \remarks
              *    Default channel is 0
              */
             double getAviParamFourcc (unsigned int channel=0);


        protected:

		  //------------------------------------------------------------------------
		  // From GrabberInterface
		  //------------------------------------------------------------------------

            bool    onInit();
            
            /*! \brief Grab on all available files
             *  \remarks
             *      The shortest file determine the end
             *      In case of looping through the files, the shortest file define the restart moment
             */
            bool    onGrab();

            bool  onDeclareParameters();
            
            std::string onGetSourceInfo(unsigned int channel) const;


    } ;



#endif
