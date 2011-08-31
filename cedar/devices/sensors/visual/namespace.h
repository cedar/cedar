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

    File:        namespace.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Namespace  for the @em cedar::devices::sensors::visual.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H
#define CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>

//------------------------------------------------------------------------
// namespace for classes
//------------------------------------------------------------------------

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      //! \brief Namespace for all visual sensors like cameras or camera-dummies (i.e. avi-file, picture)
      namespace visual
      {
        // /** obsolete */
        //class GrabberManager;

        //! @brief The common interface for all grabber \see GrabberInterface */
        class GrabberInterface;
        //!@brief smart pointer for GrabberInterface
        typedef boost::shared_ptr<GrabberInterface> GrabberInterfacePtr;
      
        
        //! @brief A grabber to grab from AVI files \see VideoGrabber */
        class VideoGrabber;
        //! @brief smart pointer for VideoGrabber
        typedef boost::shared_ptr<VideoGrabber> VideoGrabberPtr;
		
        
        //! @brief A grabber to grab from image files \see PictureGrabber */
        class NetGrabber;
        //! @brief smart pointer for NetGrabber
        typedef boost::shared_ptr<NetGrabber> NetGrabberPtr;
		
		
        //! @brief A grabber to grab from a YARP Server \see NetGrabber */
        class PictureGrabber;
        //! @brief smart pointer for PictureGrabber
        typedef boost::shared_ptr<PictureGrabber> PictureGrabberPtr;
        
        
        //! @brief A grabber to grab from a Cameras \see CameraGrabber */
        class CameraGrabber;
        //! @brief smart pointer for PictureGrabber
        typedef boost::shared_ptr<CameraGrabber> CameraGrabberPtr;
        

      }
    }
  }
}



#endif /* NAMESPACE_H_ */
