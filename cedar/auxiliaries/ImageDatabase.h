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
#include "cedar/auxiliaries/exceptions.h"

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

  /*! A base class for all kinds of image annotations.
   */
  class Annotation
  {
  public:
    virtual ~Annotation()
    {
    }
  };
  CEDAR_GENERATE_POINTER_TYPES(Annotation);

  /*! An annotation containing information for images in the ETH80 database.
   */
  class ETH80Annotation : public Annotation
  {
    public:
      void setSegmentationMask(const cedar::aux::Path& maskPath)
      {
        this->mSegmentationMask = maskPath;
      }

      const cedar::aux::Path& getSegmentationMask() const
      {
        return this->mSegmentationMask;
      }

    private:
      cedar::aux::Path mSegmentationMask;
  };
  CEDAR_GENERATE_POINTER_TYPES(ETH80Annotation);

  /*! An annotation containing information about object pose.
   */
  class ObjectPoseAnnotation : public Annotation
  {
  public:
    ObjectPoseAnnotation();

    void setPosition(double dxFromCenter, double dyFromCenter);

    void setOrientation(double orientation);

    void setScale(double factor);

    double getX() const
    {
      return this->mX;
    }

    double getY() const
    {
      return this->mY;
    }

    double getOrientation() const
    {
      return this->mOrientation;
    }

    double getScale() const
    {
      return this->mScale;
    }

    bool hasScaleAnnotation() const
    {
      return this->mHasScale;
    }

  private:
    double mX;
    double mY;
    bool mHasPosition;

    double mOrientation;
    bool mHasOrientation;

    double mScale;
    bool mHasScale;
  };
  CEDAR_GENERATE_POINTER_TYPES(ObjectPoseAnnotation);

  //! Represents an image in the database and the corresponding annotations.
  class Image
  {
  public:
    Image();

    void setFileName(const cedar::aux::Path& fileName);

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

    void setAnnotation(const std::string& annotationId, AnnotationPtr annotation);

    bool hasAnnotation(const std::string& annotationId) const;

    ConstAnnotationPtr getAnnotation(const std::string& annotationId) const;

    template <typename T>
    boost::shared_ptr<T> getAnnotation(const std::string& annotationId)
    {
      return boost::const_pointer_cast<T>(static_cast<const Image*>(this)->getAnnotation<const T>(annotationId));
    }

    template <typename T>
    boost::shared_ptr<T> getAnnotation(const std::string& annotationId) const
    {
      auto annotation = boost::dynamic_pointer_cast<T>(this->getAnnotation(annotationId));
      if (annotation)
      {
        return annotation;
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::NotFoundException,
          "Annotation with ID \"" + annotationId + "\" could not be converted to the requested type."
        );
      }
    }

    unsigned int getImageRows() const;

    unsigned int getImageColumns() const;

    //! Checks whether the given tag is one of the tags set for this image.
    bool hasTag(const std::string& tag) const;

  private:
    void readImage() const;

  private:
    ClassId mClassId;

    cedar::aux::Path mFileName;

    std::set<std::string> mTags;

    std::map<std::string, AnnotationPtr> mAnnotations;

    mutable cv::Mat mImage;
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
        mType.type()->def(cedar::aux::Enum(ETH80CroppedClose, "ETH80CroppedClose"));
        mType.type()->def(cedar::aux::Enum(COIL100, "COIL100"));
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
      static const Id ETH80CroppedClose = 1;
      static const Id COIL100 = 2;

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

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool hasClass(const std::string& className) const;

  /*!@brief Returns all images with a given tag.
   *
   */
  std::set<ImagePtr> getImagesWithTag(const std::string& tag) const;

  /*!@brief Returns all images that have one or more of the given tags.
   *
   * @param tags A list of tags, separated by ",".
   */
  std::set<ImagePtr> getImagesWithAnyTags(const std::string& tags) const;

  /*!@brief Returns all images that have one or more of the given tags.
   */
  std::set<ImagePtr> getImagesWithAnyTags(const std::vector<std::string>& tags) const;

  CEDAR_DECLARE_DEPRECATED(std::set<ImagePtr> getImagesWithTags(const std::string& tags) const)
  {
    return this->getImagesWithAnyTags(tags);
  }

  /*!@brief Returns all images that have all of the given tags.
   *
   * @param tags A list of tags, separated by ",".
   */
  std::set<ImagePtr> getImagesWithAllTags(const std::string& tags) const;

  /*!@brief Returns all images that have all of the given tags.
   */
  std::set<ImagePtr> getImagesWithAllTags(const std::vector<std::string>& tags) const;

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

  //! Returns a set of all class ids stored in this database.
  std::set<ClassId> listIds() const;

  //! Returns the id of a class given by a string identifier.
  ClassId getClass(const std::string& className) const;

  //! Returns the identifying string for a given class id.
  const std::string& getClass(ClassId id) const;

  //! Returns a set containing all images in the database.
  std::set<ImagePtr> getImages() const;

  //! Returns a set of all images matching the given class.
  std::set<ImagePtr> getImagesWithClass(ClassId classId) const;

  /*! Returns a set of training images selected with the options set in the given command line parser.
   *
   * @remarks The options that are processed can be added via cedar::aux::ImageDatabase::addCommandLineOptions.
   */
  std::set<ImagePtr> getTrainingImages(cedar::aux::CommandLineParser& parser) const;

  /*! Returns a set of training images with the options set in the given command line parser.
   *
   * @remarks The options that are processed can be added via cedar::aux::ImageDatabase::addCommandLineOptions.
   */
  std::set<ImagePtr> getTestImages(cedar::aux::CommandLineParser& parser) const;

  //! Shuffles a set of images.
  static std::vector<ImagePtr> shuffle(const std::set<ImagePtr>& images);


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void selectImages(std::set<ImagePtr>& images, cedar::aux::CommandLineParser& options) const;

  void selectImagesFromFirstNClasses(std::set<ImagePtr>& images, unsigned int numberOfClasses) const;

  void scanDirectory(const cedar::aux::Path& path);

  //! Reads in images from the ETH80 database.
  void readETH80CroppedClose(const cedar::aux::Path& path);

  //! Reads in images from the COIL100 database.
  void readCOIL100(const cedar::aux::Path& path);

  void readAnnotations(const cedar::aux::Path& path);

  //! Creates a new class id for the class name if it does not yet exist.
  ClassId getOrCreateClass(const std::string& className);

  //! Sets the given class id for the class name if it does not yet exist.
  ClassId getOrCreateClass(const std::string& className, ClassId suggestedId);

  ClassId createClass(const std::string& className);

  void createClass(const std::string& className, ClassId setId);

  void appendImage(ImagePtr sample);

  ImagePtr findImageWithFilenameNoPath(const std::string& filenameWithoutExtension);

  std::set<cedar::aux::ImageDatabase::ImagePtr> getImagesByTagStr(const std::string& tagStr) const;

  //! Reads the database type from the command line parser.
  static cedar::aux::Enum getDatabaseType(const cedar::aux::CommandLineParser& parser);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  boost::bimap<std::string, ClassId> mClassIdAssociations;

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

