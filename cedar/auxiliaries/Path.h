/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Path.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 15

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PATH_H
#define CEDAR_AUX_PATH_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <deque>


/*!@brief A class for representing paths to files and directories.
 */
class cedar::aux::Path
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Path(const std::string& path = std::string());

  //!@brief Destructor
  virtual ~Path();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the absolute path to the file/directory stored in this path.
   *
   *        If the stored path is a resource, the location of the resource is looked up.
   */
  cedar::aux::Path absolute() const;

  //! Returns true if the stored path is a path to a resource, false otherwise.
  bool isResource() const;

  bool isAbsolute() const;

  //! Sets the stored path from a string.
  void setPath(const std::string& path);

  //! Returns the stored protocol.
  const std::string& getProtocol() const;

  //! Returns a string representation of the path.
  std::string toString(bool withProtocol = false) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The protocol used for finding the absolute path. Can be: resource, absolute, relative
  std::string mProtocol;

  //! Components of the path.
  std::deque<std::string> mComponents;

  static const std::string M_PROTOCOL_ABSOLUTE_STR;

}; // class cedar::aux::Path

#endif // CEDAR_AUX_PATH_H

