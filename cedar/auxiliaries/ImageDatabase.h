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
#ifndef Q_MOC_RUN
  #include <boost/bimap.hpp>
  #include <boost/optional.hpp>
#endif // Q_MOC_RUN
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <utility>


/*!@brief A class for finding images and annotation information from an image database.
 */
class cedar::aux::ImageDatabase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type used for representing the id of classes.
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
      //! Sets the path to the file containing the segmentation mask of a sample.
      void setSegmentationMask(const cedar::aux::Path& maskPath)
      {
        this->mSegmentationMask = maskPath;
      }

      //! Returns a path to the segmentation mask of the annotated sample.
      const cedar::aux::Path& getSegmentationMask() const
      {
        return this->mSegmentationMask;
      }

    private:
      cedar::aux::Path mSegmentationMask;
  };
  CEDAR_GENERATE_POINTER_TYPES(ETH80Annotation);

  /*! An annotation containing ground-truth information about object identity.
   */
  class ClassIdAnnotation : public Annotation
  {
  public:
    //! Constructor.
    ClassIdAnnotation(cedar::aux::ImageDatabase::ClassId classId)
    :
    mClassId(classId)
    {
    }

    void setClassId(cedar::aux::ImageDatabase::ClassId classId)
    {
      this->mClassId = classId;
    }

    cedar::aux::ImageDatabase::ClassId getClassId() const
    {
      return this->mClassId;
    }

  private:
    cedar::aux::ImageDatabase::ClassId mClassId;
  };
  CEDAR_GENERATE_POINTER_TYPES(ClassIdAnnotation);

  /*! An annotation containing ground-truth information about object pose.
   *
   * @todo Switch to boost::optional instead of using a bool for each pose component.
   */
  class ObjectPoseAnnotation : public Annotation
  {
  public:
    //! Constructor.
    ObjectPoseAnnotation();

    //! Sets the position of the sample.
    void setPosition(double dxFromCenter, double dyFromCenter);

    //! Sets the orientation of the sample.
    void setOrientation(double orientation);

    //! Sets the scale of the sample.
    void setScale(double factor);

    //! Returns the x position of the sample.
    double getX() const
    {
      return this->mX;
    }

    //! Returns the y position of the sample.
    double getY() const
    {
      return this->mY;
    }

    //! Returns the orientation of the sample.
    double getOrientation() const
    {
      return this->mOrientation;
    }

    //! Returns the scale of the sample.
    double getScale() const
    {
      return this->mScale;
    }

    //! Checks, if a scale annotation is present.
    bool hasScaleAnnotation() const
    {
      return this->mHasScale;
    }
    
    //! Returns the difference in pixel between the estimated and the annotated x position, negative values indicate that the estimated position is to the left of the annotated position.
    double evaluateXposition(double dxFromCenterEstimation) const;
    
    //! Returns the difference in pixel between the estimated and the annotated y position, negative values indicate that the estimated position is to the top of the annotated position.
    double evaluateYposition(double dxFromCenterEstimation) const;
    
    //! Returns the difference in degree between the estimated and the annotated orientation, negative values indicate that the estimated orientation is rotated clockwise to the annotated orientation.
    double evaluateOrientation(double orientationEstimation) const;
    
    //! Returns the difference between the estimated and the annotated scale, negative values indicate that the estimated scale is smaller than the annotated scale.
    double evaluateScale(double scaleEstimation) const;

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
  
  //! Aggregate annotation for multiple object poses.
  class MultiObjectPoseAnnotation : public Annotation
  {
  public:  
    //! Returns the key of the last element of mObjectMap +1 or 0 if it is empty.
    int getNewKey();
    
    //! Sets an annotation at the given id.
    void setAnnotation(std::string& object, AnnotationPtr annotation, int id);
    
    //! Erases the element with the key = id of mObjectMap, does nothing if the key does not exist.
    void removeAnnotation(int id);
    
    //! Returns a vector of object and annotationPtr pairs.
    std::map<int, std::pair<std::string, ConstAnnotationPtr>> getObjectAnnotationPairMap() const;
    
    //! Returns the last added annotation.
    ConstAnnotationPtr getLastAddedAnnotation() const;
    
    //! Returns the last added annotation cast to a specific type.
    template <typename T>
    boost::shared_ptr<T> getLastAddedAnnotation()
    {
      return boost::const_pointer_cast<T>(static_cast<const MultiObjectPoseAnnotation*>(this)->getLastAddedAnnotation<const T>());
    }

    //! Returns the last added annotation cast to a specific type.
    template <typename T>
    boost::shared_ptr<T> getLastAddedAnnotation() const
    {
      auto annotation = boost::dynamic_pointer_cast<T>(this->getLastAddedAnnotation());
      if (annotation)
      {
        return annotation;
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::NotFoundException,
          "Last added annotation could not be converted to the requested type."
        );
      }
    }
    
    //! Returns an annotation with a specific id.
    ConstAnnotationPtr getAnnotation(int id) const;
    
    //! Returns an annotation with a specific id.
    template <typename T>
    boost::shared_ptr<T> getAnnotation(int id)
    {
      return boost::const_pointer_cast<T>(static_cast<const MultiObjectPoseAnnotation*>(this)->getAnnotation<const T>(id));
    }

    //! Returns an annotation with a specific id.
    template <typename T>
    boost::shared_ptr<T> getAnnotation(int id) const
    {
      auto annotation = boost::dynamic_pointer_cast<T>(this->getAnnotation(id));
      if (annotation)
      {
        return annotation;
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::NotFoundException,
          "Last added annotation could not be converted to the requested type."
        );
      }
    }

  private:
    std::map<int, std::pair<std::string, ConstAnnotationPtr>> mObjectMap;

  };
  CEDAR_GENERATE_POINTER_TYPES(MultiObjectPoseAnnotation);
  
  //! Annotation for a video frame.
  class FrameAnnotation : public Annotation
  {
  public:  
    //! Sets the annotation for a frame.
    void setAnnotation(AnnotationPtr annotation, int frame = 0);
    
    //! Returns true if an annotation exists or can be interpolated for the frame and false if not.
    bool hasAnnotation(int frame = 0) const;
    
    //! Returns an interpolated annotation at a specific frame or NULL if annotation cannot be interpolated.
    ConstAnnotationPtr getAnnotation(int frame = 0) const;
    
    //! Returns an annotation at a specific frame.
    template <typename T>
    boost::shared_ptr<T> getAnnotation(int frame = 0)
    {
      return boost::const_pointer_cast<T>(static_cast<const FrameAnnotation*>(this)->getAnnotation<const T>(frame));
    }

    //! Returns an annotation at a specific frame.
    template <typename T>
    boost::shared_ptr<T> getAnnotation(int frame = 0) const
    {
      auto annotation = boost::dynamic_pointer_cast<T>(this->getAnnotation(frame));
      if (annotation)
      {
        return annotation;
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::NotFoundException,
          "Annotation at specified frame could not be converted to the requested type."
        );
      }
    }
    
    //! Returns the keyframe of the closest key annotation before frame, or -1 if there is no key annotation before frame.
    int getPrevKeyframe(int frame);
    
    //! Returns the keyframe of the closest key annotation after frame, or -1 if there is no key annotation after frame.
    int getNextKeyframe(int frame);
    
    //! Returns the map of all key frames and annotations.
    std::map<int, AnnotationPtr> getKeyframeAnnotations();
    
    //! Returns true if a key frame annotation for the frame exists.
    bool isKeyframeAnnotation(int frame);
    
    //! Deletes the key frame for the frame.
    void deleteKeyFrameAnnotation(int frame);
    
  private:
    std::map<int, AnnotationPtr> mFrameAnnotationMapping;
  
  };
  CEDAR_GENERATE_POINTER_TYPES(FrameAnnotation);

  //! Represents an image in the database and the corresponding annotations.
  class Image
  {
  public:
    //! Constructor.
    Image();

    //! Sets the path to the image.
    void setFileName(const cedar::aux::Path& fileName);

    //! Returns the path to the image.
    const cedar::aux::Path& getFileName() const
    {
      return this->mFileName;
    }

    //! Sets the correct class id for the image.
    void setClassId(ClassId classId);

    //! Returns the correct class id for the image.
    ClassId getClassId() const;

    //! Appends a set of tags to the image.
    void appendTags(const std::string&);

    //! Returns the set of tags set for this image.
    const std::set<std::string>& getTags() const
    {
      return this->mTags;
    }

    //! Sets an annotation for the given id.
    void setAnnotation(const std::string& annotationId, AnnotationPtr annotation);

    //! Checks, if an annotation is present for the given id.
    bool hasAnnotation(const std::string& annotationId) const;

    //! Returns an annotation with a specific id.
    ConstAnnotationPtr getAnnotation(const std::string& annotationId) const;

    //! Returns an annotation with the given id, cast to a specific type.
    template <typename T>
    boost::shared_ptr<T> getAnnotation(const std::string& annotationId)
    {
      return boost::const_pointer_cast<T>(static_cast<const Image*>(this)->getAnnotation<const T>(annotationId));
    }

    //! Returns an annotation with the given id, cast to a specific type.
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

    //! Returns the number of rows in the image. Note, that this is slow because the image may be read during this call.
    unsigned int getImageRows() const;

    //! Returns the number of columns in the image. Note, that this is slow because the image may be read during this call.
    unsigned int getImageColumns() const;

    //! Checks whether the given tag is one of the tags set for this image.
    bool hasTag(const std::string& tag) const;

  private:
    void readImage() const;

  private:
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
      //! Type of the id values of the enum class.
      typedef cedar::aux::EnumId Id;

      //! Type of the enum class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

      //! Registers the enum values in the enum class.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(ScanFolder, "ScanFolder"));
        mType.type()->def(cedar::aux::Enum(ETH80CroppedClose, "ETH80CroppedClose"));
        mType.type()->def(cedar::aux::Enum(COIL100, "COIL100"));
      }

      //! Returns the base enum type.
      static const cedar::aux::EnumBase& type()
      {
        return *(mType.type());
      }

      //! Returns the base enum type as a pointer.
      static const cedar::aux::ImageDatabase::Type::TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      //! Scans a folder for all images in it. Only files with names matching the format class.tag1,tag2,... are used.
      static const Id ScanFolder = 0;
      //! Reads the cropped-close variant of the ETH-80 database.
      static const Id ETH80CroppedClose = 1;
      //! Reads the COIL-100 database.
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
  //! Checks, if the given class is present in the database.
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

  /*!@brief Returns all images that have all of the given tags.
   *
   * @param tags A list of tags, separated by ",".
   */
  std::set<ImagePtr> getImagesWithAllTags(const std::string& tags) const;

  /*!@brief Returns all images that have all of the given tags.
   */
  std::set<ImagePtr> getImagesWithAllTags(const std::vector<std::string>& tags) const;

  //! Reads the database from the given path, using the given type.
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

  //! Returns the image corresponding to the given file path.
  ImagePtr findImageByFilename(const cedar::aux::Path& fileName) const;
  
  //! Returns true if the extension is a known image file extension.
	static bool isKnownImageExtension(std::string extension);
	
  //! Returns true if the extension is a known video file extension.
	static bool isKnownVideoExtension(std::string extension);

	/*!@brief   Orders the given set of (training) images by class.
	 *
	 * @remarks This function assumes that the images contain just a single object, i.e., that getClassId() on each image
	 *          returns a valid value.
	 */
	static std::vector<ImagePtr> orderTrainingImagesByClassId(const std::set<ImagePtr>& images);

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

  void selectImagesByClasses(std::set<ImagePtr>& images, const std::vector<std::string> classNames) const;

  void selectImagesFromNRandomClasses(std::set<ImagePtr>& images, unsigned int numberOfClasses, const boost::optional<unsigned int>& seed) const;

  static void selectImagesWithClassesInSet(std::set<ImagePtr>& images, const std::set<ClassId>& acceptedClasses);

  void scanDirectory(const cedar::aux::Path& path);

  //! Reads in images from the ETH80 database.
  void readETH80CroppedClose(const cedar::aux::Path& path);

  //! Reads in images from the COIL100 database.
  void readCOIL100(const cedar::aux::Path& path);

  void readAnnotations(const cedar::aux::Path& path);
  
  void readMultiAnnotations(const cedar::aux::Path& path);

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
public:
  //! Standard name used for storing object pose annotations.
  static const std::string M_STANDARD_OBJECT_POSE_ANNOTATION_NAME;

  //! Standard name used for storing a multi-object-pose annotation.
  static const std::string M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME;

  //! Standard name used for storing object image annotations.
  static const std::string M_STANDARD_OBJECT_IMAGE_ANNOTATION_NAME;

  //! Standard name used for storing frame annotations.
  static const std::string M_STANDARD_FRAME_OBJECT_ANNOTATION_NAME;

  //! Standard name used for storing class id annotations.
  static const std::string M_STANDARD_CLASS_ID_ANNOTATION_NAME;

  //! The file extensions used for finding images.
  static const std::vector<std::string> M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS;
  
  //! The file extensions used for finding videos.
  static const std::vector<std::string> M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS;
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

