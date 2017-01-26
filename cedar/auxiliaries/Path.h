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
#include "cedar/auxiliaries/PathTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Path.fwd.h"

// SYSTEM INCLUDES
#include <deque>
#include <string>
#include <vector>


namespace cedar
{
  namespace aux
  {
    //! Policy for paths that determines the separator for path entries.
    //!@todo On windows: use forward and backward slash, where appropriate
    class FilesystemPathSeparator
    {
      public:
        //! Splits the string based on the separator specified via template argument.
        static std::vector<std::string> separate(const std::string&)
        {
          // nothing to do, cedar::aux::Path does not use this
          return std::vector<std::string>();
        }

        static std::string joinComponents(const std::vector<std::string>& components)
        {
          return cedar::aux::join(components, "/");
        }
    };
  }
}

/*!@brief A class for representing paths to files and directories.
 */
class cedar::aux::Path : public cedar::aux::PathTemplate<cedar::aux::FilesystemPathSeparator, std::string>
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::PathTemplate<cedar::aux::FilesystemPathSeparator, std::string> PathBase;

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

private:
  //!@brief Constructs this object from a base path.
  Path(const PathBase& path);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the absolute path to the file/directory stored in this path.
   *
   *        If the stored path is a resource, the location of the resource is looked up.
   */
  cedar::aux::Path absolute(bool showInLog = true) const;

  //! Returns true if the stored path is a path to a resource, false otherwise.
  bool isResource() const;

  //! Returns true if the path is an absolute path.
  bool isAbsolute() const;

  //! Returns true if the path is relative.
  bool isRelative() const;

  //! Returns true if the file will be searched for in a unit test.
  bool isTestFile() const;

  //! Returns true if the path is relative to a plugin folder, i.e., starts with the protocol plugin://
  bool isPluginRelative() const;

  //! Returns the stored protocol.
  const std::string& getProtocol() const;

  //! Returns the base directory for storing global cedar settings.
  static cedar::aux::Path globalCofigurationBaseDirectory();

  //! Returns the path, but without the protocol.
  std::string toString() const;

  //! Returns a string representation of the path.
  std::string toString(bool withProtocol) const;

  void fromString(const std::string& string);

  /*! Returns the directory pointed to by the path. If this path is already a directory, the path itself is returned.
   *  Otherwise, if this path points to a file, the path without the filename is returned.
   */
  cedar::aux::Path getDirectory() const;

  /*! Returns only the filename.
   */
  std::string getFileNameOnly() const;

  /*! Returns the filename without extension
   */
  std::string getFileNameWithoutExtension() const;

  /*! Returns the extension of the file.
   *
   * For example, for "a/folder/foo.bar", this function will return "bar"
   */
  std::string getExtension() const;

  /*! Checks if the path contains an extension.
   */
  bool hasExtension() const;

  /*! Sets the filename, i.e., the last component of the path.
   */
  void setFileName(const std::string& fileName);

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

  //! Lists all the direct subdirectories of this path.
  std::vector<cedar::aux::Path> listSubdirectories() const;

  //! Lists all the files that are in the path.
  std::vector<cedar::aux::Path> listFiles() const;

  //! Lists all the files in the path whose absolute path matches the given regular expression.
  std::vector<cedar::aux::Path> listFilesThatMatchRe(const std::string& regexp) const;

  //! Ensures that all directories that are part of this path exist.
  void createDirectories() const;

  //! Returns the separator for paths (depends on the operating system).
  static std::string separator();

  //! Returns a formatted timestamp to be used in file and directory names.
  static std::string getTimestampForFileName();

  virtual bool operator== (const cedar::aux::Path& other) const;

  virtual bool operator< (const cedar::aux::Path& other) const;

  //! slice operator
  Path operator()(size_t start, size_t end) const;

  //! Slices the path, starting from the given index.
  Path operator()(size_t start) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static void splitFileNameAndExtension(const std::string& fileNameAndExtension, std::string& fileName, std::string& extension);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The protocol used for finding the absolute path. Can be: resource, absolute, relative
  std::string mProtocol;

  static const std::string M_PROTOCOL_ABSOLUTE_STR;
  static const std::string M_PROTOCOL_RESOURCE_STR;
  static const std::string M_PROTOCOL_PLUGIN_STR;
  static const std::string M_PROTOCOL_TEST_STR;

}; // class cedar::aux::Path

#endif // CEDAR_AUX_PATH_H

