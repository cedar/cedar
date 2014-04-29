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

// SYSTEM INCLUDES
#include <boost/filesystem.hpp>

cedar::aux::EnumType<cedar::aux::ImageDatabase::Type> cedar::aux::ImageDatabase::Type::mType("cedar::aux::ImageDatabase::Type::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::ImageDatabase::Type::Id cedar::aux::ImageDatabase::Type::ScanFolder;
#endif // CEDAR_COMPILER_MSVC


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ImageDatabase::ImageDatabase()
{
}

cedar::aux::ImageDatabase::Image::Image()
:
mClassId(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
    0,
    group_name
  );
}

void cedar::aux::ImageDatabase::readDatabase(const cedar::aux::CommandLineParser& parser)
{
  cedar::aux::Path path = parser.getValue<std::string>("database-path");
  cedar::aux::Enum type = parser.getValue<cedar::aux::Enum>("database-type");

  this->readDatabase(path, type.name());
}

void cedar::aux::ImageDatabase::Image::setClassId(ClassId classId)
{
  this->mClassId = classId;
}

void cedar::aux::ImageDatabase::appendImage(ImagePtr sample)
{
  mImages.push_back(sample);

  for (const auto& tag : sample->getTags())
  {
    auto iter = this->mImagesByTag.find(tag);
    if (iter == this->mImagesByTag.end())
    {
      this->mImagesByTag[tag] = std::set<ImagePtr>();
      iter = this->mImagesByTag.find(tag);
    }
    iter->second.insert(sample);
  }
}

void cedar::aux::ImageDatabase::readDatabase(const cedar::aux::Path& path, const std::string& dataBaseType)
{
  auto enum_value = Type::type().get(dataBaseType);

  switch (enum_value)
  {
    case Type::ScanFolder:
      this->scanDirectory(path);
      break;

    default:
      CEDAR_THROW(cedar::aux::UnknownTypeException, "The database type \"" + dataBaseType + "\" is not known.");
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
      __PRETTY_FUNCTION__
    );
    return;
  }

  if (!path.isDirectory())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Database path \"" + path.toString() + "\" is not a directory.",
      __PRETTY_FUNCTION__
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

  for (std::string file : files)
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
    std::string extension = "." + parts.back();
    if (parts.size() > 0 && extension == ".png")
    {
      if (parts.size() == 3)
      {
        std::string classname = parts.at(0);
        std::string tags = parts.at(1);

        ImagePtr sample(new Image());
        sample->setClassId(this->getOrCreateClass(classname));
        sample->setFileName(file);
        sample->appendTags(tags);

        this->appendImage(sample);
      }
    }
  }

  // TODO
//  this->readAnnotations(path);
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
  for (auto tag_imageset_pair : this->mImagesByTag)
  {
    tags.insert(tag_imageset_pair.first);
  }
  return tags;
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
  for (auto iter = tag_split.begin(); iter != tag_split.end(); ++iter)
  {
    this->mTags.insert(*iter);
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
