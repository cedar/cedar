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

    File:        PictureGrabber.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 01 08

    Description: Header for the @em cedar::dev::sensors::visual::PictureGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_PICTURE_GRABBER_H

// LOCAL INCLUDES
#include "GrabberInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES






/*! \class PictureGrabber
 * 	\brief This grabber grabs images from a picture-file
 */



class cedar::dev::sensors::visual::PictureGrabber : public GrabberInterface
{

     //------------------------------------------------------------------------
      // Init 
      //------------------------------------------------------------------------
       public:

            /*! \brief  Constructor for a single-file grabber 
             * 	\param grabberName 		The name for this grabber
             *  \param configFileName	Filename for the configuration
             * 	\param pictureFileName	Filename to grab from
             */
            PictureGrabber(std::string configFileName,
                           std::string pictureFileName);


            /*! \brief Constructor for a stereo-file grabber 
             * 	\param grabberName 		The name for this grabber
             *  \param configFileName	Filename for the configuration
             * 	\param pictureFileName0	Filename to grab from for channel 0
             * 	\param pictureFileName1	Filename to grab from for channel 1
             */
            PictureGrabber(std::string configFileName,
                       std::string pictureFileName0,
                       std::string pictureFileName1);

            /*! \brief Destructor */
            ~PictureGrabber();

      
		//------------------------------------------------------------------------
		// Members
		//------------------------------------------------------------------------

		protected:
        
            /*! \brief The filenames
             *
             */
            std::vector<std::string> mSourceFileName;



      //------------------------------------------------------------------------
      // Methods
      //------------------------------------------------------------------------
		public:
		
			/*! \brief Set a new picture to grab from
			 *  \remarks
			 *      Supported image-types depend on operating system and installed libs. <br>
			 * 		  For details look at the OpenCV-documentation (Section "imread").
			 *  \param channel which should be changed (default is 0).
			 *  \param FileName of the newly used picture.
			 */ 	
			bool setSourceFile(unsigned int channel, const std::string& FileName );





		  //------------------------------------------------------------------------
		  // From GrabberInterface
		  //------------------------------------------------------------------------
        protected:
        
            bool    onInit();
            bool    onGrab();

            bool    onDeclareParameters   ();
	
            std::string onGetSourceInfo(unsigned int channel) const;
	


};

#endif
