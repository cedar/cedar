/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Path.fwd.h"

// SYSTEM INCLUDES
#include <deque>
#include <string>


/*!@brief A class for representing paths to files and directories.
 */
class cedar::aux::Path
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& stream, const cedar::aux::Path& path)
  {
    stream << path.toString();
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The default constructor.
  Path();

  //!@brief Constructor that reads a path from a std::string.
  Path(const std::string& path);

  //!@brief Constructor that reads a path from a const char* string.
  Path(const char* path);

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

  //! Returns true if there are no entries in this path, i.e., the path is "".
  bool isEmpty() const;

  //! Sets the stored path from a string.
  void setPath(const std::string& path);

  //! Returns the stored protocol.
  const std::string& getProtocol() const;

  //! Returns the base directory for storing global cedar settings.
  static cedar::aux::Path globalCofigurationBaseDirectory();

  //! Returns a string representation of the path.
  std::string toString(bool withProtocol = false) const;

  /*! Returns the directory pointed to by the path. If this path is already a directory, the path itself is returned.
   *  Otherwise, if this path points to a file, the filename is removed.
   */
  cedar::aux::Path getDirectory() const;

  /*! Returns only the filename.
   */
  std::string getFileNameOnly() const;

  //! Appends the given path to this one.
  cedar::aux::Path operator+ (const cedar::aux::Path& other) const;

  //! Assigns a path.
  cedar::aux::Path& operator= (const std::string& pathStr);

  //! Assigns a path.
  cedar::aux::Path& operator= (const char* pathStr);

  //! Test whether the path actually exists.
  bool exists() const;

  //! Test whether the path points to a directory.
  bool isDirectory() const;

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
  static const std::string M_PROTOCOL_RESOURCE_STR;

}; // class cedar::aux::Path

#endif // CEDAR_AUX_PATH_H

