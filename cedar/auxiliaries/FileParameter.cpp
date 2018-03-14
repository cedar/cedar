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

    File:        FileParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// parameter declaration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registerParameter()
  {
    typedef cedar::aux::ParameterDeclaration<cedar::aux::FileParameterPtr> Declaration;
    CEDAR_GENERATE_POINTER_TYPES(Declaration);

    DeclarationPtr declaration(new Declaration("string", "cedar.aux.FileParameter"));
    declaration->declare();

    return true;
  }

  bool registered = registerParameter();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::FileParameter::FileParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name,
  cedar::aux::FileParameter::Mode mode
)
:
cedar::aux::Parameter(pOwner, name, false),
mValue(this->getCurrentArchitectureFileDirectory()),
mDefault(this->getCurrentArchitectureFileDirectory()),
mMode(mode),
mPathMode(cedar::aux::FileParameter::PATH_MODE_ABSOLUTE),
mConstantPathMode(false)
{
}

cedar::aux::FileParameter::FileParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name,
  cedar::aux::FileParameter::Mode mode,
  const std::string& defaultValue
)
:
cedar::aux::Parameter(pOwner, name, true),
mValue(QString::fromStdString(defaultValue)),
mDefault(QString::fromStdString(defaultValue)),
mMode(mode),
mPathMode(cedar::aux::FileParameter::PATH_MODE_ABSOLUTE),
mConstantPathMode(false)
{
//  std::cout<<"FileParameter Constructor! Current Architecture Path " << this->getCurrentArchitectureFileDirectory().absolutePath().toStdString() <<std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::FileParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  bool oldFileFormat = true;

  auto mode_iter = node.find("isRelative");
  if (mode_iter != node.not_found())
  {
    auto pathMode = node.get<bool>("isRelative") ? FileParameter::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR : FileParameter::PATH_MODE_ABSOLUTE;
    this->mPathMode = pathMode;
    oldFileFormat = false;
  }

  auto path_iter = node.find("path");
  if (path_iter != node.not_found())
  {
    if(this->mPathMode == FileParameter::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR )
    {
      this->mValue.setCurrent(this->getCurrentArchitectureFileDirectory().absolutePath());
    }
    this->mValue.setPath(QString::fromStdString(node.get<std::string>("path")));
    oldFileFormat = false;
  }

  //Old version for legacy architecture files
  if(oldFileFormat)
  {
    this->mValue.setPath(QString::fromStdString(node.get_value<std::string>()));
    this->setPathMode(FileParameter::PATH_MODE_ABSOLUTE);
  }

}

void cedar::aux::FileParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode fileParaNode;

  fileParaNode.put("path",this->getPath());
  fileParaNode.put("isRelative",this->mPathMode == FileParameter::PathMode::PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR);

  root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), fileParaNode));

  // root.put(this->getName(), this->getPath());
}

void cedar::aux::FileParameter::copyValueFrom(cedar::aux::ConstParameterPtr other)
{
  auto other_file = boost::dynamic_pointer_cast<cedar::aux::ConstFileParameter>(other);
  CEDAR_DEBUG_ASSERT(other_file);
  this->mPathMode = other_file->mPathMode;
  this->mMode = other_file->mMode;
  this->mDefault = other_file->mDefault;
  this->setValue(other_file->getValue());
}

bool cedar::aux::FileParameter::canCopyFrom(cedar::aux::ConstParameterPtr other) const
{
  return static_cast<bool>(boost::dynamic_pointer_cast<cedar::aux::ConstFileParameter>(other));
}

void cedar::aux::FileParameter::setValue(const std::string& value)
{
  if(this->getPathMode()==PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR)
  {
//    QDir combinedPath1 = this->getCurrentArchitectureFileDirectory().relativeFilePath(QString::fromStdString(value));
    QDir combinedPath = this->getCurrentArchitectureFileDirectory().filePath(QString::fromStdString(value));
//    std::cout<<"c1 : " << combinedPath1.absolutePath().toStdString() << " c2: " << combinedPath2.absolutePath().toStdString() << std::endl;
    this->mValue = combinedPath;
  }
  else
  {
    this->mValue = QString::fromStdString(value);
  }

//  std::cout<<"FileParameter::setValue(string). value:  " << value << " mValue: "<< this->mValue.absolutePath().toStdString() << std::endl;


  this->emitChangedSignal();
}

void cedar::aux::FileParameter::setValue(const QDir& value)
{
  this->mValue = value;

  this->emitChangedSignal();
}

void cedar::aux::FileParameter::makeDefault()
{
  this->setValue(this->mDefault);
}

const QDir& cedar::aux::FileParameter::getValue() const
{
  return this->mValue;
}

std::string cedar::aux::FileParameter::getPath(bool forceAbsolutePath) const
{

  //All the Grabberclasses expect this filePath, but maybe this could me removed in the long term. The other return values seem to work, but only during initialisation errors are thrown.
  if(forceAbsolutePath)
  {
    return this->mValue.path().toStdString();
  }

  switch (this->mPathMode)
  {
    default:
    case PATH_MODE_ABSOLUTE:
      return this->mValue.absolutePath().toStdString();

    case PATH_MODE_RELATIVE_TO_WORKING_DIR:
      return QDir::current().relativeFilePath(this->mValue.absolutePath()).toStdString();

    case PATH_MODE_RELATIVE_TO_CURRENT_ARCHITECTURE_DIR:
      return this->getCurrentArchitectureFileDirectory().relativeFilePath(this->mValue.absolutePath()).toStdString();
  }
}

cedar::aux::FileParameter::Mode cedar::aux::FileParameter::getMode() const
{
  return this->mMode;
}

void cedar::aux::FileParameter::setPathMode(cedar::aux::FileParameter::PathMode mode)
{
  this->mPathMode = mode;
  this->emitChangedSignal();
}


cedar::aux::FileParameter::PathMode cedar::aux::FileParameter::getPathMode()
{
  return this->mPathMode;
}


QDir cedar::aux::FileParameter::getCurrentArchitectureFileDirectory() const
{
  auto lastArchitectureFile = cedar::aux::SettingsSingleton::getInstance()->getCurrentArchitectureFileName();
  QDir curArchitectureDir(QString::fromStdString(lastArchitectureFile));
  curArchitectureDir.cdUp(); // Get the folder not the file!
  //      std::cout<<"Currently the absolutePath to the last architecturefile is: " << curArchitectureDir.absolutePath().toStdString() <<std::endl;
//      std::cout<<"The absolute path to the value is: " << this->mValue.absolutePath().toStdString() <<std::endl;
//      std::cout<<"\tThe relative path is then: " << curArchitectureDir.relativeFilePath(this->mValue.absolutePath()).toStdString() <<std::endl;

  return curArchitectureDir;
}


void cedar::aux::FileParameter::setPathModeConstant(bool isConstant)
{
  mConstantPathMode = isConstant;
  this->emitChangedSignal();
}

bool cedar::aux::FileParameter::isPathModeConstant()
{
  return mConstantPathMode;
}