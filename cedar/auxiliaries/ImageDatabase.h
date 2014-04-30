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

    File:        ImageDatabase.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 04 28

    Description: Header file for the class cedar::aux::ImageDatabase.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_IMAGE_DATABASE_H
#define CEDAR_AUX_IMAGE_DATABASE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Enum.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/Path.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ImageDatabase.fwd.h"
#include "cedar/auxiliaries/CommandLineParser.fwd.h"

// SYSTEM INCLUDES
#include <boost/bimap.hpp>
#include <iostream>
#include <set>


/*!@brief A class for finding images and annotation information from an image database.
 */
class cedar::aux::ImageDatabase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef unsigned int ClassId;

  //! Represents an image in the database and the corresponding annotations.
  class Image
  {
  public:
    Image();

    void setFileName(const cedar::aux::Path& fileName)
    {
      this->mFileName = fileName;
    }

    const cedar::aux::Path& getFileName() const
    {
      return this->mFileName;
    }

    void setClassId(ClassId classId);

    ClassId getClassId() const
    {
      return this->mClassId;
    }

    void appendTags(const std::string&);

    const std::set<std::string>& getTags() const
    {
      return this->mTags;
    }

  private:
    ClassId mClassId;

    cedar::aux::Path mFileName;

    std::set<std::string> mTags;
  };
  CEDAR_GENERATE_POINTER_TYPES(Image);

  /*!@brief Type of image database to read.
   */
  class Type
  {
    public:
      typedef cedar::aux::EnumId Id;

    public:
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(ScanFolder, "ScanFolder"));
      }

      static const cedar::aux::EnumBase& type()
      {
        return *(mType.type());
      }
      static const cedar::aux::ImageDatabase::Type::TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      static const Id ScanFolder = 0;

    protected:
      // none yet
    private:
      static cedar::aux::EnumType<cedar::aux::ImageDatabase::Type> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImageDatabase();

  bool hasClass(const std::string& className) const;

  /*!@brief Returns all images with a given tag.
   *
   */
  std::set<ImagePtr> getImagesWithTag(const std::string& tag) const;

  /*!@brief Returns all images with a set of given tags.
   *
   * @param tags A list of tags, separated by ",".
   */
  std::set<ImagePtr> getImagesWithTags(const std::string& tags) const;

  /*!@brief Returns all images with a list of given tags.
   */
  std::set<ImagePtr> getImagesWithTags(const std::vector<std::string>& tags) const;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void readDatabase(const cedar::aux::Path& path, const std::string& dataBaseType);

  /*!@brief Reads the database using the settings from the given command line parser.
   *
   * @param parser A parser that has been prepared using cedar::aux::ImageDatabase::addCommandLineOptions.
   */
  void readDatabase(const cedar::aux::CommandLineParser& parser);

  /*!@brief Writes a summary of the database (number of classes etc.) to the given output stream.
  */
  void writeSummary(std::ostream& stream = std::cout);

  /*! Adds a set of options to the command line parser that allow control of database reading.
   */
  static void addCommandLineOptions(cedar::aux::CommandLineParser& parser);

  //! Returns the number of images in this database.
  size_t getImageCount() const;

  //! Returns the number of distinct classes in this database.
  size_t getClassCount() const;

  //! Returns the number of distinct tags in this database.
  size_t getTagCount() const;

  //! Returns a set containing all the tags in this database.
  std::set<std::string> listTags() const;

  //! Returns a set of all the classes in this database.
  std::set<std::string> listClasses() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void scanDirectory(const cedar::aux::Path& path);

  ClassId getOrCreateClass(const std::string& className);

  ClassId createClass(const std::string& className);

  void createClass(const std::string& className, ClassId setId);

  ClassId getClass(const std::string& className) const;

  const std::string& getClass(ClassId id) const;

  void appendImage(ImagePtr sample);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  boost::bimap<std::string, ClassId> mClassIdAssociations;

  std::map<std::string, std::set<ImagePtr> > mImagesByTag;

  std::vector<ImagePtr> mImages;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ImageDatabase

#endif // CEDAR_AUX_IMAGE_DATABASE_H

