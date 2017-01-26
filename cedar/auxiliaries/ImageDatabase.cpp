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

    File:        ImageDatabase.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 04 28

    Description: Source file for the class cedar::aux::ImageDatabase.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ImageDatabase.h"
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/opencv_helper.h"

// SYSTEM INCLUDES
#include <boost/filesystem.hpp>
#include <fstream>
#include <algorithm>
#include <random>

cedar::aux::EnumType<cedar::aux::ImageDatabase::Type> cedar::aux::ImageDatabase::Type::mType("cedar::aux::ImageDatabase::Type::");

const std::string cedar::aux::ImageDatabase::M_STANDARD_OBJECT_POSE_ANNOTATION_NAME = "object pose";
const std::string cedar::aux::ImageDatabase::M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME = "multi_object_pose";
const std::string cedar::aux::ImageDatabase::M_STANDARD_OBJECT_IMAGE_ANNOTATION_NAME = "object";
const std::string cedar::aux::ImageDatabase::M_STANDARD_FRAME_OBJECT_ANNOTATION_NAME = "frame_object";
const std::string cedar::aux::ImageDatabase::M_STANDARD_CLASS_ID_ANNOTATION_NAME = "class id";

const std::vector<std::string> cedar::aux::ImageDatabase::M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS = {"png", "ppm"};
const std::vector<std::string> cedar::aux::ImageDatabase::M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS = {"avi", "mpeg", "mp4", "flv", "ogg", "vob", "mpg"};

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::ImageDatabase::Type::Id cedar::aux::ImageDatabase::Type::ScanFolder;
#endif // CEDAR_COMPILER_MSVC


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ImageDatabase::ImageDatabase()
{
}

cedar::aux::ImageDatabase::ObjectPoseAnnotation::ObjectPoseAnnotation()
:
mX(0), mY(0), mHasPosition(false),
mOrientation(0), mHasOrientation(false),
mScale(1.0), mHasScale(false)
{
}

cedar::aux::ImageDatabase::Image::Image()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::ImageDatabase::ObjectPoseAnnotation::setPosition(double dxFromCenter, double dyFromCenter)
{
  this->mHasPosition = true;
  this->mX = dxFromCenter;
  this->mY = dyFromCenter;
}

void cedar::aux::ImageDatabase::ObjectPoseAnnotation::setOrientation(double orientation)
{
  this->mHasOrientation = true;
  this->mOrientation = orientation;
}

void cedar::aux::ImageDatabase::ObjectPoseAnnotation::setScale(double factor)
{
  this->mHasScale = true;
  this->mScale = factor;
}

double cedar::aux::ImageDatabase::ObjectPoseAnnotation::evaluateXposition(double dxFromCenterEstimation) const
{
	double difference = dxFromCenterEstimation - this->mX;
	return difference;
}

double cedar::aux::ImageDatabase::ObjectPoseAnnotation::evaluateYposition(double dyFromCenterEstimation) const
{
	double difference = dyFromCenterEstimation - this->mY;
	return difference;
}

double cedar::aux::ImageDatabase::ObjectPoseAnnotation::evaluateOrientation(double orientationEstimation) const
{
  while(orientationEstimation < 0)
  {
   	orientationEstimation += 360;
  }
  
  double orientation = this->mOrientation;
  while(orientation < 0)
  {
    orientation += 360;
  }
  
  double difference = orientationEstimation - orientation;     
  while (difference > 180.0)
  {
    difference -= 360.0;
  }
  while (difference < -180.0)
  {
    difference += 360.0;
  }
        
	return difference;
}

double cedar::aux::ImageDatabase::ObjectPoseAnnotation::evaluateScale(double scaleEstimation) const
{
	double difference = scaleEstimation - this->mScale;
	return difference;
}

int cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::getNewKey()
{
  int key = 0;
  if (!this->mObjectMap.empty())
  {
    auto lastElement = this->mObjectMap.end();
    lastElement--;
    key = lastElement->first + 1;
  }
  return key;
}
 
void cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::setAnnotation(std::string& object, AnnotationPtr annotation, int id)
{
  std::pair<std::string, cedar::aux::ImageDatabase::ConstAnnotationPtr> pair = std::make_pair (object, annotation);

  this->mObjectMap.insert(std::make_pair<int&, std::pair<std::string, ConstAnnotationPtr>& >(id, pair));
}

void cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::removeAnnotation(int id)
{
  this->mObjectMap.erase(id);
}

std::map<int, std::pair<std::string, cedar::aux::ImageDatabase::ConstAnnotationPtr>> 
   cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::getObjectAnnotationPairMap() const
{
  return this->mObjectMap;
}

cedar::aux::ImageDatabase::ConstAnnotationPtr
  cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::getLastAddedAnnotation() const
{
  auto lastElement = this->mObjectMap.end();
  lastElement--;
  return lastElement->second.second;
}

cedar::aux::ImageDatabase::ConstAnnotationPtr 
  cedar::aux::ImageDatabase::MultiObjectPoseAnnotation::getAnnotation(int id) const
{
  auto iter = this->mObjectMap.find(id);
  if (iter != this->mObjectMap.end())
  {
    return iter->second.second;
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "An annotation with that id does not exists.");
  }
}

void cedar::aux::ImageDatabase::FrameAnnotation::setAnnotation(AnnotationPtr annotation, int frame)
{
  this->mFrameAnnotationMapping[frame] = annotation;
}

bool cedar::aux::ImageDatabase::FrameAnnotation::hasAnnotation(int frame) const
{
  if (this->getAnnotation(frame))
  {
    return true;
  }
  return false;
}

cedar::aux::ImageDatabase::ConstAnnotationPtr
  cedar::aux::ImageDatabase::FrameAnnotation::getAnnotation(int frame) const
{
  auto iter = this->mFrameAnnotationMapping.find(frame);
  if (iter != this->mFrameAnnotationMapping.end())
  {
    return iter->second;
  }
  else
  {
    try
    {
      cedar::aux::ImageDatabase::ObjectPoseAnnotationPtr prev;
      cedar::aux::ImageDatabase::ObjectPoseAnnotationPtr next;
      double prevFrame = 0.;
      double nextFrame = 0.;
      
      for(auto iter = this->mFrameAnnotationMapping.begin(); iter!= this->mFrameAnnotationMapping.end(); ++iter)
      {
        //find closest annotations
        if (iter->first <= frame)
        {
          prev = boost::dynamic_pointer_cast<cedar::aux::ImageDatabase::ObjectPoseAnnotation>(iter->second);
          prevFrame = iter->first;
        }
        else if (iter->first > frame)
        {
          next = boost::dynamic_pointer_cast<cedar::aux::ImageDatabase::ObjectPoseAnnotation>(iter->second);
          nextFrame = iter->first;
          break;
        }
      }
      if (!prev)
      {
        return cedar::aux::ImageDatabase::ConstAnnotationPtr();
      }
      else if (!next)
      {
        return prev;
      }
      else
      {
        //interpolate linear
        cedar::aux::ImageDatabase::ObjectPoseAnnotationPtr poseAnnotation = ObjectPoseAnnotationPtr(new ObjectPoseAnnotation);
        
        double frameDiff = (1.*(frame-prevFrame))/(1.*(nextFrame-prevFrame));
        
        //position
        double xInterp = prev->getX()+(next->getX()-prev->getX())*frameDiff;
        double yInterp = prev->getY()+(next->getY()-prev->getY())*frameDiff;
        poseAnnotation->setPosition(xInterp,yInterp);
        
        //orientation
        double prevOri = prev->getOrientation();
        double nextOri = next->getOrientation();
        
        double oridiff = nextOri-prevOri;
        while (oridiff > 180.0)
        {
          oridiff -= 360.0;
        }
        while (oridiff < -180.0)
        {
          oridiff += 360.0;
        }
        
        double oriInterp = prevOri+(oridiff)*frameDiff;
        while (oriInterp > 180.0)
        {
          oriInterp -= 360.0;
        }
        
        poseAnnotation->setOrientation(oriInterp);
        
        //scale
        double scaleInterp = prev->getScale()+(next->getScale()-prev->getScale())*frameDiff;
        poseAnnotation->setScale(scaleInterp);
        
        return poseAnnotation;
      }
    }
    catch (cedar::aux::NotFoundException e)
    {
      CEDAR_THROW(cedar::aux::NotFoundException, "Interpolation for annotation type not implemented");
    }

    return cedar::aux::ImageDatabase::ConstAnnotationPtr();
  }
}

std::map<int, cedar::aux::ImageDatabase::AnnotationPtr>
  cedar::aux::ImageDatabase::FrameAnnotation::getKeyframeAnnotations()
{
  return this->mFrameAnnotationMapping;
}

int cedar::aux::ImageDatabase::FrameAnnotation::getPrevKeyframe(int frame)
{
  int prevFrame = -1;
  
  for(auto iter = this->mFrameAnnotationMapping.begin(); iter!= this->mFrameAnnotationMapping.end(); ++iter)
  {
    //find previous annotation
    if (iter->first < frame)
    {
      prevFrame = iter->first;
    }
    else if (iter->first > frame)
    {
      break;
    }
  }
  return prevFrame;
}

int cedar::aux::ImageDatabase::FrameAnnotation::getNextKeyframe(int frame)
{
  int nextFrame = -1;
  
  for(auto iter = this->mFrameAnnotationMapping.begin(); iter!= this->mFrameAnnotationMapping.end(); ++iter)
  {
    //find next annotation
    if (iter->first > frame)
    {
      nextFrame = iter->first;
      break;
    }
  }
  return nextFrame;
}

bool cedar::aux::ImageDatabase::FrameAnnotation::isKeyframeAnnotation(int frame)
{
  auto iter = this->mFrameAnnotationMapping.find(frame);
  if (iter != this->mFrameAnnotationMapping.end())
  {
    return true;
  }
  else
  {
    return false;
  }
}

void cedar::aux::ImageDatabase::FrameAnnotation::deleteKeyFrameAnnotation(int frame)
{
  this->mFrameAnnotationMapping.erase(frame);
}

bool cedar::aux::ImageDatabase::Image::hasTag(const std::string& tag) const
{
  return this->mTags.find(tag) != this->mTags.end();
}

void cedar::aux::ImageDatabase::Image::readImage() const
{
  this->mImage = cv::imread(this->mFileName.absolute().toString());
  
  if (this->mImage.empty())
  {
    cv::VideoCapture capture(this->mFileName.absolute().toString(false));
    capture.set(CEDAR_OPENCV_CONSTANT(CAP_PROP_POS_FRAMES), 0);
    capture.read(this->mImage);
  }
}

void cedar::aux::ImageDatabase::Image::setFileName(const cedar::aux::Path& fileName)
{
  this->mFileName = fileName;
}

unsigned int cedar::aux::ImageDatabase::Image::getImageRows() const
{
  if (this->mImage.empty())
  {
    this->readImage();
  }
  return static_cast<unsigned int>(this->mImage.rows);
}

unsigned int cedar::aux::ImageDatabase::Image::getImageColumns() const
{
  if (this->mImage.empty())
  {
    this->readImage();
  }
  return static_cast<unsigned int>(this->mImage.cols);
}

cedar::aux::ImageDatabase::ImagePtr cedar::aux::ImageDatabase::findImageByFilename(const cedar::aux::Path& fileName) const
{
  for (auto image : this->mImages)
  {
    if (image->getFileName() == fileName)
    {
      return image;
    }
  }

  CEDAR_THROW(cedar::aux::NotFoundException, "An image with the filename \"" + fileName.toString() + "\" could not be found.");
}

bool cedar::aux::ImageDatabase::isKnownImageExtension(std::string extension)
{
	for(auto it = M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS.begin(); it != M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS.end(); ++it)
  {
  	if (extension == *it)
  	{
     return true;
    }
  }
  return false;
}

bool cedar::aux::ImageDatabase::isKnownVideoExtension(std::string extension)
{
	for(auto it = M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS.begin(); it != M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS.end(); ++it)
  {
  	if (extension == *it)
  	{
     return true;
    }
  }
  return false;
}

std::vector<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::shuffle(const std::set<ImagePtr>& images)
{
  std::vector<cedar::aux::ImageDatabase::ImagePtr> shuffled;
  shuffled.insert(shuffled.begin(), images.begin(), images.end());
  std::random_shuffle(shuffled.begin(), shuffled.end());
  return shuffled;
}

std::vector<cedar::aux::ImageDatabase::ImagePtr>
  cedar::aux::ImageDatabase::orderTrainingImagesByClassId(const std::set<ImagePtr>& images)
{
  std::set<ClassId> ids;

  for (auto image : images)
  {
    ids.insert(image->getClassId());
  }

  std::vector<ImagePtr> ordered_samples;
  for (auto current_id : ids)
  {
    for (auto image : images)
    {
      if (image->getClassId() == current_id)
      {
        ordered_samples.push_back(image);
      }
    }
  }

  return ordered_samples;
}

void cedar::aux::ImageDatabase::addCommandLineOptions(cedar::aux::CommandLineParser& parser)
{
  std::string group_name = "image database settings";
  parser.defineValue
  (
    "database-path",
    "Base path for the image database to read.",
    0,
    group_name
  );

  parser.defineEnum
  (
    "database-type",
    "Type of image database to read.",
    Type::typePtr(),
    Type::ScanFolder,
    0,
    group_name
  );

  std::string sample_selection_group_name = "sample selection";

  //!@todo This should be an enum.
  parser.defineValue
  (
    "sample-training-selection-mode",
    "Which mode to use for selecting training samples.",
    "by_tag",
    0,
    sample_selection_group_name
  );

  parser.defineValue
  (
    "sample-training-tags",
    "Tag combination to use for selecting the training samples.",
    "train",
    0,
    sample_selection_group_name
  );

  //!@todo This should be an enum.
  parser.defineValue
  (
    "sample-testing-selection-mode",
    "Which mode to use for selecting testing samples. Possible values: not_in_training_set, by_tag, by_class, all",
    "not_in_training_set",
    0,
    sample_selection_group_name
  );

  parser.defineValue
  (
    "sample-testing-tags",
    "Tag combination to use for selecting the testing samples.",
    "test",
    0,
    sample_selection_group_name
  );

  parser.defineValue
  (
    "sample-testing-classes",
    "A class name or list of class names used for testing. If blank, all classes are used. Format: \"class 1, class 2, ..., class N\"",
    "",
    0,
    sample_selection_group_name
  );

  parser.defineValue
  (
    "restrict-sample-selection",
    "An instruction on how to restrict the sample selection. Possible instructions are: \n"
    "  \"unrestricted\": no restrictions,\n"
    "  \"first N\": selects only samples belonging to one of the first N classes.\n"
    "  \"random N [seed M]\": selects a random subset of N classes from the database. The optional seed is used to initialize the RNG.\n"
    "  \"any_of 'class A' 'class B' ... 'class N'\": only images that contain any of the specified class(es) are used.",
    "unrestricted",
    0,
    sample_selection_group_name
  );
}

cedar::aux::Enum cedar::aux::ImageDatabase::getDatabaseType(const cedar::aux::CommandLineParser& parser)
{
  return parser.getValue<cedar::aux::Enum>("database-type");
}

void cedar::aux::ImageDatabase::readDatabase(const cedar::aux::CommandLineParser& parser)
{
  cedar::aux::Path path = parser.getValue<std::string>("database-path");
  cedar::aux::Enum type = cedar::aux::ImageDatabase::getDatabaseType(parser);

  this->readDatabase(path, type.name());
}

void cedar::aux::ImageDatabase::Image::setAnnotation(const std::string& annotationId, AnnotationPtr annotation)
{
  this->mAnnotations[annotationId] = annotation;
}

bool cedar::aux::ImageDatabase::Image::hasAnnotation(const std::string& annotationId) const
{
  return this->mAnnotations.find(annotationId) != this->mAnnotations.end();
}

cedar::aux::ImageDatabase::ConstAnnotationPtr
  cedar::aux::ImageDatabase::Image::getAnnotation(const std::string& annotationId) const
{
  auto iter = this->mAnnotations.find(annotationId);

  if (iter == this->mAnnotations.end())
  {
    std::string known_annotations;

    if (!this->mAnnotations.empty())
    {
      bool first = true;
      for (auto name_annotation_pair : this->mAnnotations)
      {
        if (first)
        {
          first = false;
        }
        else
        {
          known_annotations += ", ";
        }
        known_annotations += "\"" + name_annotation_pair.first + "\"";
      }
    }
    else
    {
      known_annotations = "none";
    }
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "Could not find an annotation with the id \"" + annotationId + "\" in image " + this->getFileName() +
          ". Known annotations are: " + known_annotations
    );
  }

  return iter->second;
}

cedar::aux::ImageDatabase::ClassId cedar::aux::ImageDatabase::Image::getClassId() const
{
  auto annotation = this->getAnnotation<const ClassIdAnnotation>(cedar::aux::ImageDatabase::M_STANDARD_CLASS_ID_ANNOTATION_NAME);
  return annotation->getClassId();
}

void cedar::aux::ImageDatabase::Image::setClassId(cedar::aux::ImageDatabase::ClassId classId)
{
  ClassIdAnnotationPtr annotation;
  if (this->hasAnnotation(cedar::aux::ImageDatabase::M_STANDARD_CLASS_ID_ANNOTATION_NAME))
  {
    annotation = this->getAnnotation<ClassIdAnnotation>(cedar::aux::ImageDatabase::M_STANDARD_CLASS_ID_ANNOTATION_NAME);
    annotation->setClassId(classId);
  }
  else
  {
    annotation = ClassIdAnnotationPtr(new ClassIdAnnotation(classId));
    this->setAnnotation(cedar::aux::ImageDatabase::M_STANDARD_CLASS_ID_ANNOTATION_NAME, annotation);
  }
}

std::set<cedar::aux::ImageDatabase::ImagePtr>
  cedar::aux::ImageDatabase::getImagesWithAllTags(const std::string& tagsStr) const
{
  std::vector<std::string> tags;
  cedar::aux::split(tagsStr, ",", tags);
  return this->getImagesWithAllTags(tags);
}

std::set<cedar::aux::ImageDatabase::ImagePtr>
  cedar::aux::ImageDatabase::getImagesWithAllTags(const std::vector<std::string>& tags) const
{
  std::set<ImagePtr> result;
  for (auto image : this->mImages)
  {
    bool has_all_tags = true;
    for (const auto& tag : tags)
    {
      if (!image->hasTag(tag))
      {
        has_all_tags = false;
        break;
      }
    }

    if (has_all_tags)
    {
      result.insert(image);
    }
  }

  return result;
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImagesWithAnyTags(const std::string& tagsStr) const
{
  std::vector<std::string> tags;
  cedar::aux::split(tagsStr, ",", tags);
  return this->getImagesWithAnyTags(tags);
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImagesWithAnyTags(const std::vector<std::string>& tags) const
{
  std::set<ImagePtr> samples;

  for (const auto& tag : tags)
  {
    auto tag_samples = this->getImagesWithTag(tag);
    samples.insert(tag_samples.begin(), tag_samples.end());
  }

  return samples;
}
std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImagesWithClass(cedar::aux::ImageDatabase::ClassId classId) const
{
  std::set<ImagePtr> images;

  for (auto image : this->mImages)
  {
    if (image->getClassId() == classId)
    {
      images.insert(image);
    }
  }

  return images;
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImagesWithTag(const std::string& tag) const
{
  std::set<ImagePtr> images;

  for (auto image : this->mImages)
  {
    if (image->hasTag(tag))
    {
      images.insert(image);
    }
  }

  return images;
}

void cedar::aux::ImageDatabase::appendImage(ImagePtr sample)
{
  mImages.push_back(sample);
}

cedar::aux::ImageDatabase::ImagePtr cedar::aux::ImageDatabase::findImageWithFilenameNoPath(const std::string& filenameWithoutExtension)
{
  for (const auto& image : this->mImages)
  {
  	//check images
  	for(auto it = M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS.begin(); it != M_STANDARD_KNOWN_IMAGE_FILE_EXTENSIONS.end(); ++it)
  	{
  		if (image->getFileName().getFileNameOnly() == filenameWithoutExtension + "." + *it)
  		{
      	return image;
    	}
    }
    
    //check videos
    for(auto it = M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS.begin(); it != M_STANDARD_KNOWN_VIDEO_FILE_EXTENSIONS.end(); ++it)
  	{
  		if (image->getFileName().getFileNameOnly() == filenameWithoutExtension + "." + *it)
  		{
      	return image;
    	}
    }
  }
  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find sample " + filenameWithoutExtension);
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImages() const
{
  std::set<cedar::aux::ImageDatabase::ImagePtr> images;
  images.insert(this->mImages.begin(), this->mImages.end());
  return images;
}


void cedar::aux::ImageDatabase::readDatabase(const cedar::aux::Path& path, const std::string& dataBaseType)
{
  auto enum_value = Type::type().get(dataBaseType);

  switch (enum_value)
  {
    case Type::ScanFolder:
      this->scanDirectory(path);
      break;

    case Type::ETH80CroppedClose:
      this->readETH80CroppedClose(path);
      break;

    case Type::COIL100:
      this->readCOIL100(path);
      break;

    default:
      CEDAR_THROW(cedar::aux::UnknownTypeException, "The database type \"" + dataBaseType + "\" is not known.");
  }
}

void cedar::aux::ImageDatabase::readAnnotations(const cedar::aux::Path& path)
{
  cedar::aux::Path annotation_file_path = path;
  std::ifstream annotation_file(annotation_file_path.absolute().toString() + "/annotations");

  if (!annotation_file.is_open())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Annotation file not found.",
      "void cedar::aux::ImageDataBase::readAnnotations(const cedar::aux::Path&)"
    );
    return;
  }

  std::string current_image;
  ImagePtr image;

  while (!annotation_file.eof())
  {
    std::string line;
    std::getline(annotation_file, line);

    std::vector<std::string> parts;
    cedar::aux::split(line, ":", parts);

    for (size_t p = 0; p < parts.size(); ++p)
    {
      parts[p] = cedar::aux::replace(parts[p], " ", "");
    }

    switch (parts.size())
    {
      case 2:
      {
        std::string left = parts[0];
        std::string right = parts[1];
       
        if (right.empty())
        {
          current_image = left;
          try
          {
            image = this->findImageWithFilenameNoPath(current_image);
            image->setAnnotation(M_STANDARD_OBJECT_POSE_ANNOTATION_NAME, ObjectPoseAnnotationPtr(new ObjectPoseAnnotation()));
          }
          catch (cedar::aux::NotFoundException)
          {
            // make the image a nullptr, skip the reset of the annotations until the next image
            image.reset();
          }
        }
        else
        {
          if (!image)
          {
            continue;
          }

          if (left == "position")
          {
            CEDAR_ASSERT(right.size() > 3);
            right = right.substr(1, right.length() - 2);
            std::vector<std::string> position_str;
            cedar::aux::split(right, ",", position_str);
            CEDAR_ASSERT(position_str.size() == 2);
            int x = cedar::aux::fromString<int>(position_str[0]);
            int y = cedar::aux::fromString<int>(position_str[1]);

            auto annotation = image->getAnnotation<ObjectPoseAnnotation>(M_STANDARD_OBJECT_POSE_ANNOTATION_NAME);
            double region_rows, region_cols;
            region_cols = static_cast<double>(image->getImageColumns());
            region_rows = static_cast<double>(image->getImageRows());

            double rel_x, rel_y;

            rel_x = static_cast<double>(x) - region_cols/2.0;
            rel_y = static_cast<double>(y) - region_rows/2.0;
            annotation->setPosition(rel_x, rel_y);
          }
          else if (left == "orientation")
          {
            auto annotation = image->getAnnotation<ObjectPoseAnnotation>(M_STANDARD_OBJECT_POSE_ANNOTATION_NAME);
            double phi = cedar::aux::fromString<double>(right);
            annotation->setOrientation(phi);
          }
          else if (left == "scale-factor")
          {
            auto annotation = image->getAnnotation<ObjectPoseAnnotation>(M_STANDARD_OBJECT_POSE_ANNOTATION_NAME);
            double scale = cedar::aux::fromString<double>(right);
            annotation->setScale(scale);
          }
          else
          {
            std::cout << "WARNING: UNUSED ANNOTATION LINE: \"" << left << "\"=>\"" << right << "\"" << std::endl;
          }
        }
        break;
      }

      default:
        break;
    }
  }
}

void cedar::aux::ImageDatabase::readMultiAnnotations(const cedar::aux::Path& path)
{
  cedar::aux::Path annotation_file_path = path;
  std::ifstream annotation_file(annotation_file_path.absolute().toString() + "/annotations");

  if (!annotation_file.is_open())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Annotation file not found.",
      "void cedar::aux::ImageDataBase::readMultiAnnotations(const cedar::aux::Path&)"
    );
    return;
  }

  std::string current_image;
  ImagePtr image;
  bool foundObject = false;
  bool isFrameAnnotation = false;
  unsigned int frame = 0;

  while (!annotation_file.eof())
  {
    std::string line;
    std::getline(annotation_file, line);

    std::vector<std::string> parts;
    cedar::aux::split(line, ":", parts);

    for (size_t p = 0; p < parts.size(); ++p)
    {
      parts[p] = cedar::aux::replace(parts[p], " ", "");
      parts[p] = cedar::aux::replace(parts[p], "\t", "");
    }

    switch (parts.size())
    {
      case 2:
      {
        std::string left = parts[0];
        std::string right = parts[1];

        if (right == M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME)
        {
          try
          {
            image = this->findImageWithFilenameNoPath(left);
            image->setAnnotation(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME,
               MultiObjectPoseAnnotationPtr(new MultiObjectPoseAnnotation()));
          }
          catch (cedar::aux::NotFoundException)
          {
            std::cout << "WARNING: image not found: "<< left << std::endl;
            // make the image a nullptr, skip the reset of the annotations until the next image
            image.reset();
          }
          foundObject = false;
          isFrameAnnotation = false;
        }
        else if (image && right == M_STANDARD_OBJECT_IMAGE_ANNOTATION_NAME)
        {
          current_image = left;
          foundObject = true;
          isFrameAnnotation = false;
          auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
          int key = annotation->getNewKey();
          annotation->setAnnotation(current_image, ObjectPoseAnnotationPtr(new ObjectPoseAnnotation()), key);
        }
        else if (image && right == M_STANDARD_FRAME_OBJECT_ANNOTATION_NAME)
        {
          current_image = left;
          foundObject = true;
          isFrameAnnotation = true;
          auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
          int key = annotation->getNewKey();
          annotation->setAnnotation(current_image, FrameAnnotationPtr(new FrameAnnotation()), key);
        }
        else
        {
          if (!image || !foundObject)
          {
            continue;
          }
          if (left == "frame")
          {
            frame = cedar::aux::fromString<unsigned int>(right);
            auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
            auto frameAnnotation= annotation->getLastAddedAnnotation<FrameAnnotation>();
            frameAnnotation->setAnnotation(ObjectPoseAnnotationPtr(new ObjectPoseAnnotation), frame);
          }
          else if (left == "position")
          {
            CEDAR_ASSERT(right.size() > 3);
            right = right.substr(1, right.length() - 2);
            std::vector<std::string> position_str;
            cedar::aux::split(right, ",", position_str);
            CEDAR_ASSERT(position_str.size() == 2);
            int x = cedar::aux::fromString<int>(position_str[0]);
            int y = cedar::aux::fromString<int>(position_str[1]);

            auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
            
            double region_rows, region_cols;
            region_cols = static_cast<double>(image->getImageColumns());
            region_rows = static_cast<double>(image->getImageRows());

            double rel_x, rel_y;
            rel_x = static_cast<double>(x) - region_cols/2.0;
            rel_y = static_cast<double>(y) - region_rows/2.0;

            if (isFrameAnnotation)
            {
              auto lastFrameAnnotation = annotation->getLastAddedAnnotation<FrameAnnotation>();
              auto poseAnnotation = lastFrameAnnotation->getAnnotation<ObjectPoseAnnotation>(frame);
              poseAnnotation->setPosition(rel_x, rel_y);
            }
            else
            {
              auto poseAnnotation = annotation->getLastAddedAnnotation<ObjectPoseAnnotation>();
              poseAnnotation->setPosition(rel_x, rel_y);
            }
          }
          else if (left == "orientation")
          {
            auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
            double phi = cedar::aux::fromString<double>(right);
            
            if (isFrameAnnotation)
            {
              auto lastFrameAnnotation = annotation->getLastAddedAnnotation<FrameAnnotation>();
              auto poseAnnotation = lastFrameAnnotation->getAnnotation<ObjectPoseAnnotation>(frame);
              poseAnnotation->setOrientation(phi);        
            }
            else
            {
              auto poseAnnotation = annotation->getLastAddedAnnotation<ObjectPoseAnnotation>();
              poseAnnotation->setOrientation(phi);
            } 
          }
          else if (left == "scale-factor")
          {
            auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
            double scale = cedar::aux::fromString<double>(right);
            
            if (isFrameAnnotation)
            {
              auto lastFrameAnnotation = annotation->getLastAddedAnnotation<FrameAnnotation>();
              auto poseAnnotation = lastFrameAnnotation->getAnnotation<ObjectPoseAnnotation>(frame);
              poseAnnotation->setScale(scale);
            }
            else
            {
              auto poseAnnotation = annotation->getLastAddedAnnotation<ObjectPoseAnnotation>();
              poseAnnotation->setScale(scale);
            } 
          }
          else if (left == "removed")
          {
            bool isRemoved = cedar::aux::fromString<bool>(right);
            if (isRemoved && isFrameAnnotation)
            {
              auto annotation = image->getAnnotation<MultiObjectPoseAnnotation>(M_STANDARD_MULTI_OBJECT_POSE_ANNOTATION_NAME);
              annotation->getLastAddedAnnotation<FrameAnnotation>()->setAnnotation(cedar::aux::ImageDatabase::AnnotationPtr(), frame);
            }
          }
          else
          {
            std::cout << "WARNING: UNUSED ANNOTATION LINE: \"" << left << "\"=>\"" << right << "\"" << std::endl;
          }
        }
        break;
      }

      default:
        break;
    }
  }
}

void cedar::aux::ImageDatabase::scanDirectory(const cedar::aux::Path& path)
{
  boost::filesystem::directory_iterator end_iter;

  if (!path.exists())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Database path \"" + path.toString() + "\" does not exist.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  if (!path.isDirectory())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Database path \"" + path.toString() + "\" is not a directory.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }
  std::set<std::string> files;
  for (boost::filesystem::directory_iterator dir_iter(path.absolute().toString()); dir_iter != end_iter ; ++dir_iter)
  {
    if (boost::filesystem::is_regular_file(dir_iter->status()))
    {
      std::string file = cedar::aux::toString(*dir_iter);
      files.insert(file);
    }
  }

  for (auto file : files)
  {
    std::string file_no_dir, restpath;

    // separate file into filename, path
    cedar::aux::splitLast(file, "/", restpath, file_no_dir);
    if (cedar::aux::endsWith(file, "\""))
    {
      file = file.substr(0, file.length() - 1);
    }
    if (cedar::aux::endsWith(file_no_dir, "\""))
    {
      file_no_dir = file_no_dir.substr(0, file_no_dir.length() - 1);
    }

    if (file.at(0) == '\"')
    {
      file = file.substr(1);
    }

    // split filenames in the format category.tag.extension
    std::vector<std::string> parts;
    cedar::aux::split(file_no_dir, ".", parts);

    CEDAR_DEBUG_ASSERT(!parts.empty());
    std::string extension = parts.back();

    if (parts.size() > 0 && (isKnownImageExtension(extension) || isKnownVideoExtension(extension)))
    {
      if (parts.size() == 3)
      {
        std::string classname = parts.at(0);
        std::string tags = parts.at(1);

        ImagePtr sample(new Image());
        sample->setFileName(file);
        sample->appendTags(tags);
        ClassId class_id;
        // only training images create new classes
        try
        {
          if (sample->hasTag("train"))
          {
            class_id = this->getOrCreateClass(classname);
          }
          else
          {
            class_id = this->getClass(classname);
          }
          sample->setClassId(class_id);
        }
        catch (const cedar::aux::UnknownNameException&)
        {
          // happens when no id is known for the class
        }

        this->appendImage(sample);
      }
    }
  }

  this->readAnnotations(path);
  
  this->readMultiAnnotations(path);
}

void cedar::aux::ImageDatabase::readCOIL100(const cedar::aux::Path& path)
{
  std::map<std::string, std::string> object_names;
  object_names["obj1"] = "cold tablets";
  object_names["obj2"] = "onion";
  object_names["obj3"] = "boat";
  object_names["obj4"] = "tomato";
  object_names["obj5"] = "gum";
  object_names["obj6"] = "yellow-orange car";
  object_names["obj7"] = "iced tea";
  object_names["obj8"] = "yellow-green car";
  object_names["obj9"] = "dental floss";
  object_names["obj10"] = "espresso cup";
  object_names["obj11"] = "brown cup";
  object_names["obj12"] = "wood blocks";
  object_names["obj13"] = "moisturizer";
  object_names["obj14"] = "lucky cat";
  object_names["obj15"] = "yellow car";
  object_names["obj16"] = "flower cup";
  object_names["obj17"] = "cat figure";
  object_names["obj18"] = "white cup";
  object_names["obj19"] = "fire truck";
  object_names["obj20"] = "alien head";
  object_names["obj21"] = "wooden tool";
  object_names["obj22"] = "deodorant";
  object_names["obj23"] = "red car";
  object_names["obj24"] = "vitamin tablets";
  object_names["obj25"] = "pot";
  object_names["obj26"] = "chicken flavor";
  object_names["obj27"] = "green car";
  object_names["obj28"] = "frog";
  object_names["obj29"] = "cat food";
  object_names["obj30"] = "vase";

  for (const auto& file : path.listFiles())
  {
    std::string extension = file.getExtension();
    if (!isKnownImageExtension(extension))
    {
      continue;
    }

    std::string file_name = file.getFileNameWithoutExtension();
    std::vector<std::string> splits;
    cedar::aux::split(file_name, "__", splits);

    if (splits.size() != 2)
    {
      continue;
    }

    auto object = splits.at(0);
    const auto& view = splits.at(1);

    size_t number_start = object.find_first_of("0123456789");
    CEDAR_ASSERT(number_start != std::string::npos);
    std::string object_number_str = object.substr(number_start);
    unsigned int object_number = cedar::aux::fromString<unsigned int>(object_number_str);

    auto name_iter = object_names.find(object);
    if (name_iter != object_names.end())
    {
      object += " (";
      object += name_iter->second;
      object += ")";
    }

    auto class_id = this->getOrCreateClass(object, object_number);

    ImagePtr image(new Image());
    image->setClassId(class_id);
    image->setFileName(file);
    image->appendTags(view);
    this->appendImage(image);
  }
}

void cedar::aux::ImageDatabase::readETH80CroppedClose(const cedar::aux::Path& path)
{
  // check all folders for the right contents
  for (const auto& folder : path.listSubdirectories())
  {
    std::string class_and_instance = folder.getLast();
    auto sep = class_and_instance.find_last_not_of("0123456789");
    if (sep == std::string::npos)
    {
      // folder name does not fit the [className][InstanceNo] pattern; skip it
      continue;
    }
    std::string class_name = class_and_instance.substr(0, sep + 1);
    std::string instance = class_and_instance.substr(sep + 1);

    auto class_id = this->getOrCreateClass(class_name);
    for (const auto& file : folder.listFiles())
    {
      ImagePtr image(new Image());
      image->setClassId(class_id);
      image->setFileName(file);

      std::string tags;

      std::string file_name = file.getFileNameWithoutExtension();
      std::vector<std::string> splits;
      cedar::aux::split(file_name, "-", splits);
      if (splits.size() != 3)
      {
        // file names must have the format [class name][instance no]-[angle]-[angle]
        continue;
      }

      std::string angle_v_str, angle_h_str;
      angle_v_str = splits.at(1);
      angle_h_str = splits.at(2);

      image->appendTags("v" + angle_v_str);
      image->appendTags("h" + angle_h_str);

      ETH80AnnotationPtr annotation(new ETH80Annotation());

      cedar::aux::Path segmentation = folder;
      segmentation.append("maps");
      segmentation.append(file.getFileNameWithoutExtension() + "-map.png");

      if (segmentation.exists())
      {
        annotation->setSegmentationMask(segmentation);
      }

      image->setAnnotation("eth80 annotation", annotation);

      this->appendImage(image);
    }
  }
}

cedar::aux::ImageDatabase::ClassId
  cedar::aux::ImageDatabase::getOrCreateClass(const std::string& className, cedar::aux::ImageDatabase::ClassId suggestedId)
{
  if (this->hasClass(className))
  {
    return this->getClass(className);
  }
  else
  {
    this->createClass(className, suggestedId);
    return suggestedId;
  }
}

cedar::aux::ImageDatabase::ClassId cedar::aux::ImageDatabase::getOrCreateClass(const std::string& className)
{
  if (!this->hasClass(className))
  {
    return this->createClass(className);
  }
  else
  {
    return this->getClass(className);
  }
}

bool cedar::aux::ImageDatabase::hasClass(const std::string& className) const
{
  return this->mClassIdAssociations.left.find(className) != this->mClassIdAssociations.left.end();
}

cedar::aux::ImageDatabase::ClassId cedar::aux::ImageDatabase::getClass(const std::string& className) const
{
  auto iter = this->mClassIdAssociations.left.find(className);

  if (iter == this->mClassIdAssociations.left.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "The class name \"" + className + " is not known.");
  }

  return iter->second;
}

size_t cedar::aux::ImageDatabase::getImageCount() const
{
  return this->mImages.size();
}

size_t cedar::aux::ImageDatabase::getClassCount() const
{
  return this->listClasses().size();
}

size_t cedar::aux::ImageDatabase::getTagCount() const
{
  return this->listTags().size();
}

std::set<std::string> cedar::aux::ImageDatabase::listTags() const
{
  std::set<std::string> tags;
  for (auto image : this->mImages)
  {
    for (const auto& tag : image->getTags())
    {
      tags.insert(tag);
    }
  }
  return tags;
}

std::set<cedar::aux::ImageDatabase::ClassId> cedar::aux::ImageDatabase::listIds() const
{
  std::set<ClassId> ids;
  for (auto class_id_pair : this->mClassIdAssociations.left)
  {
    ids.insert(class_id_pair.second);
  }
  return ids;
}

std::set<std::string> cedar::aux::ImageDatabase::listClasses() const
{
  std::set<std::string> classes;
  for (auto class_id_pair : this->mClassIdAssociations.left)
  {
    classes.insert(class_id_pair.first);
  }
  return classes;
}

const std::string& cedar::aux::ImageDatabase::getClass(cedar::aux::ImageDatabase::ClassId id) const
{
  auto iter = this->mClassIdAssociations.right.find(id);

  if (iter == this->mClassIdAssociations.right.end())
  {
    CEDAR_THROW(cedar::aux::UnknownTypeException, "The class id \"" + cedar::aux::toString(id) + "\" is not known.");
  }

  return iter->second;
}

cedar::aux::ImageDatabase::ClassId cedar::aux::ImageDatabase::createClass(const std::string& className)
{
  if (this->hasClass(className))
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "The class name \"" + className + " already exists.");
  }

  ClassId new_index;
  if (this->mClassIdAssociations.empty())
  {
    new_index = 0;
  }
  else
  {
    new_index = (--this->mClassIdAssociations.right.end())->first + 1;
  }

  this->createClass(className, new_index);

  return new_index;
}

void cedar::aux::ImageDatabase::createClass(const std::string& className, ClassId setId)
{
  // the new index shouldn't exist
  CEDAR_DEBUG_ASSERT(this->mClassIdAssociations.right.find(setId) == this->mClassIdAssociations.right.end());

  this->mClassIdAssociations.insert(boost::bimap<std::string, ClassId>::value_type(className, setId));
}

void cedar::aux::ImageDatabase::Image::appendTags(const std::string& tags)
{
  std::vector<std::string> tag_split;
  cedar::aux::split(tags, ",", tag_split);
  for (const auto& tag : tag_split)
  {
    this->mTags.insert(tag);
  }
}

void cedar::aux::ImageDatabase::writeSummary(std::ostream& stream)
{
  auto class_count = this->getClassCount();
  auto tag_count = this->getTagCount();
  stream << "Read database with "
      << this->getImageCount() << " images, "
      << class_count << " classes and "
      << tag_count << " tags."
      << std::endl;

  stream << std::endl;

  if (class_count > 0)
  {
    auto classes = this->listClasses();
    stream << "The classes are:" << std::endl;
    for (auto class_iter = classes.begin(); class_iter != classes.end(); ++class_iter)
    {
      auto class_name = *class_iter;
      stream << " - " << class_name << " (id = " << this->getClass(class_name) << ")" << std::endl;
    }
    stream << std::endl;
  }

  if (tag_count > 0)
  {
    auto tags = this->listTags();
    stream << "The tags are:" << std::endl;
    for (auto tag_iter = tags.begin(); tag_iter != tags.end(); ++tag_iter)
    {
      auto tag_name = *tag_iter;
      stream << " - " << tag_name << std::endl;
    }
    stream << std::endl;
  }
}

void cedar::aux::ImageDatabase::selectImages(std::set<ImagePtr>& images, cedar::aux::CommandLineParser& options) const
{
  std::string instruction_str = options.getValue<std::string>("restrict-sample-selection");
  std::string instruction, arguments;
  cedar::aux::splitFirst(instruction_str, " ", instruction, arguments);

  if (instruction == "first")
  {
    CEDAR_ASSERT(!arguments.empty());
    unsigned int number = cedar::aux::fromString<unsigned int>(arguments);

    this->selectImagesFromFirstNClasses(images, number);
  }
  else if (instruction == "random")
  {
    CEDAR_ASSERT(!arguments.empty());
    std::vector<std::string> argument_strs;
    cedar::aux::split(arguments, " ", argument_strs);
    CEDAR_DEBUG_ASSERT(argument_strs.size() > 0);
    unsigned int number = cedar::aux::fromString<unsigned int>(argument_strs.at(0));

    boost::optional<unsigned int> seed;

    for (size_t part = 1; part < argument_strs.size(); ++part)
    {
      const auto& part_str = argument_strs.at(part);
      if (part_str == "seed")
      {
        CEDAR_ASSERT(argument_strs.size() > part + 1)
        seed = cedar::aux::fromString<unsigned int>(argument_strs.at(part + 1));
        ++part;
      }
    }
    this->selectImagesFromNRandomClasses(images, number, seed);
  }
  else if (instruction == "any_of")
  {
    CEDAR_ASSERT(!arguments.empty());

    std::vector<std::string> accepted_classes;

    bool in_string = false;
    std::string current = "";
    for (size_t i_c = 0; i_c < arguments.size(); ++i_c)
    {
      char c = arguments.at(i_c);
      if (in_string)
      {
        switch (c)
        {
          case '\'':
            in_string = false;
            accepted_classes.push_back(current);
            current = "";
            break;

          default:
            current += c;
            break;
        }
      }
      else
      {
        switch (c)
        {
          case '\'':
            in_string = true;
            break;

          default:
            break;
        }
      }

    }

    this->selectImagesByClasses(images, accepted_classes);
  }
  else if (instruction == "unrestricted")
  {
    // nothing to do: no restrictions apply
    return;
  }
  else
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Instruction \"" + instruction + "\" is not known; instruction string: \"" + instruction_str + "\"");
  }
}

void cedar::aux::ImageDatabase::selectImagesByClasses(std::set<ImagePtr>& images, const std::vector<std::string> classNames) const
{
  std::set<ClassId> accepted_classes;

  for (const auto& class_name : classNames)
  {
    accepted_classes.insert(this->getClass(class_name));
  }

  selectImagesWithClassesInSet(images, accepted_classes);
}

void cedar::aux::ImageDatabase::selectImagesFromFirstNClasses(std::set<ImagePtr>& images, unsigned int numberOfClasses) const
{
  std::set<ClassId> accepted_classes;

  unsigned int count = 0;
  for (auto id_name_pair : this->mClassIdAssociations.right)
  {
    if (count < numberOfClasses)
    {
      accepted_classes.insert(id_name_pair.first);
    }
    else
    {
      break;
    }
    ++count;
  }

  selectImagesWithClassesInSet(images, accepted_classes);
}

void cedar::aux::ImageDatabase::selectImagesFromNRandomClasses
     (
       std::set<ImagePtr>& images,
       unsigned int numberOfClasses,
       const boost::optional<unsigned int>& seed
     ) const
{

  std::vector<ClassId> class_ids;

  for (const auto& id_name_pair : this->mClassIdAssociations.right)
  {
    class_ids.push_back(id_name_pair.first);
  }

  std::default_random_engine engine;
  if (seed)
  {
    engine = std::default_random_engine(seed.get());
  }
  else
  {
    engine = std::default_random_engine(time(0));
  }
  std::shuffle(class_ids.begin(), class_ids.end(), engine);

  CEDAR_ASSERT(numberOfClasses < class_ids.size());

  std::set<ClassId> accepted_classes;
  accepted_classes.insert(class_ids.begin(), class_ids.begin() + numberOfClasses);
  selectImagesWithClassesInSet(images, accepted_classes);
}

void cedar::aux::ImageDatabase::selectImagesWithClassesInSet(std::set<ImagePtr>& images, const std::set<ClassId>& acceptedClasses)
{
  for (auto iter = images.begin(); iter != images.end();)
  {
    auto image = *iter;
    if (acceptedClasses.find(image->getClassId()) == acceptedClasses.end())
    {
      // image's class is not an accepted class => remove image from set of images
      iter = images.erase(iter);
    }
    else
    {
      // image's class is an accepted class => keep it, go to the next one
      ++iter;
    }
  }
}


std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getTestImages(cedar::aux::CommandLineParser& parser) const
{
  std::set<cedar::aux::ImageDatabase::ImagePtr> images;

  //!@todo redundant code with getTrainingImages; they only differ in some strings
  std::string mode = parser.getValue<std::string>("sample-testing-selection-mode");
  //!@todo This should be an enum
  if (mode == "not_in_training_set")
  {
    // get all the training images
    auto training_set = this->getTrainingImages(parser);

    // find all images not in the training set
    images.clear();
    for (auto image : this->mImages)
    {
      if (training_set.find(image) == training_set.end())
      {
        images.insert(image);
      }
    }
  }
  else if (mode == "by_tag")
  {
    std::string tag_selection = parser.getValue<std::string>("sample-testing-tags");
    images = this->getImagesByTagStr(tag_selection);
  }
  else if (mode == "by_class")
  {
    std::string class_selection = parser.getValue<std::string>("sample-testing-classes");

    std::vector<std::string> classes;
    cedar::aux::split(class_selection, ",", classes);
    images.clear();

    for (const auto& class_name : classes)
    {
      auto normalized = cedar::aux::removeLeadingAndTrailingWhiteSpaces(class_name);
      auto class_images = this->getImagesWithClass(this->getClass(normalized));
      images.insert(class_images.begin(), class_images.end());
    }
  }
  else if (mode == "all")
  {
    // add all images to the test set
    for (auto image : this->mImages)
    {
      images.insert(image);
    }
  }
  else
  {
    CEDAR_ASSERT(false && "Unknown sample selection mode.");
  }

  this->selectImages(images, parser);
  return images;
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getTrainingImages(cedar::aux::CommandLineParser& parser) const
{
  std::set<cedar::aux::ImageDatabase::ImagePtr> images;
  std::string mode = parser.getValue<std::string>("sample-training-selection-mode");

  //!@todo This should be an enum
  if (mode == "by_tag")
  {
    std::string tag_selection = parser.getValue<std::string>("sample-training-tags");
    images = this->getImagesByTagStr(tag_selection);
  }
  else
  {
    CEDAR_ASSERT(false && "Unknown sample selection mode.");
  }

  this->selectImages(images, parser);
  return images;
}

std::set<cedar::aux::ImageDatabase::ImagePtr> cedar::aux::ImageDatabase::getImagesByTagStr(const std::string& tagStr) const
{
  auto open_br = tagStr.find('(');
  if (open_br == std::string::npos)
  {
    return this->getImagesWithTag(tagStr);
  }

  std::string mode = tagStr.substr(0, open_br);
  CEDAR_ASSERT(tagStr.at(tagStr.size() - 1) == ')');
  std::string tags_str = tagStr.substr(open_br + 1, tagStr.size() - open_br - 2);
  tags_str = cedar::aux::removeWhiteSpaces(tags_str);
  if (mode == "any")
  {
    return this->getImagesWithAnyTags(tags_str);
  }
  else if (mode == "all")
  {
    return this->getImagesWithAllTags(tags_str);
  }
  else
  {
    CEDAR_THROW(cedar::aux::InvalidValueException, "Tag selection mode \"" + mode + "\" is not known.");
  }
}
