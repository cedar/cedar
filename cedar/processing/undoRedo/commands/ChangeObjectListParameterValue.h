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

    File:        ChangeObjectListParameterValue.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 28

    Description: Header file for the class cedar::proc::undoRedo::commands::ChangeParameterValue.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_LIST_PARAMETER_VALUE_H
#define CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_LIST_PARAMETER_VALUE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectListParameter.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/Element.h"

// FORWARD DECLARATIONS
#include "cedar/processing/undoRedo/commands/ChangeObjectListParameterValue.fwd.h"

// SYSTEM INCLUDES

/*!@ Parameter change command
 *
 * UndoCommand Implementation for ObjectListParameter
 */
class cedar::proc::undoRedo::commands::ChangeObjectListParameterValue : public cedar::proc::undoRedo::commands::ChangeParameterValue<std::string, cedar::aux::ObjectListParameter>
{
  //--------------------------------------------------------------------------------------------------------------------
  // enums
  //--------------------------------------------------------------------------------------------------------------------
public:

  enum Action
  {
    CREATE,
    DELETE
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Constructor for adding objects
  ChangeObjectListParameterValue(cedar::aux::ObjectListParameter* parameter, std::string type,
                                 cedar::aux::NamedConfigurable* owner, cedar::proc::gui::Scene* scene = nullptr)
  :
  ChangeParameterValue(parameter, type, type, owner, scene),
  mIndex(-1),
  mAction(CREATE)
  {
    std::string typeStr = this->mOldValue;
    std::vector<std::string> parts;
    cedar::aux::split(this->mOldValue, ".", parts);
    if(parts.size() > 0)
    {
      typeStr = parts[parts.size() - 1];
    }

    if(owner != nullptr)
    {
      setText(QString::fromStdString("[+] " + typeStr + ":" + this->mParentIdentifier + ":"
                  + this->mParameterIdentifier));
    }
    else
    {
      setText(QString::fromStdString("[+] " + typeStr + ":" + this->mpParameter->getName() + ": - Error -"));
    }
  }

  //!@brief Constructor for removing objects
  ChangeObjectListParameterValue(cedar::aux::ObjectListParameter* parameter, int index,
                                 cedar::aux::NamedConfigurable* owner, cedar::proc::gui::Scene* scene = nullptr)
  :
  ChangeParameterValue(parameter, parameter->getTypeOfObject(parameter->getConfigurableChild(index)),
                       parameter->getTypeOfObject(parameter->getConfigurableChild(index)), owner, scene),
  mAction(DELETE),
  mIndex(index)
  {
    std::string typeStr = this->mOldValue;
    std::vector<std::string> parts;
    cedar::aux::split(this->mOldValue, ".", parts);
    if(parts.size() > 0)
    {
      typeStr = parts[parts.size() - 1];
    }

    if(owner != nullptr)
    {
      setText(QString::fromStdString("[-] " + typeStr + ": " + this->mParentIdentifier + ": " + this->mParameterIdentifier));
    }
    else
    {
      setText(QString::fromStdString("[-] " + typeStr + ": " + this->mpParameter->getName() + ": - Error -"));
    }
  }

  //!@brief Destructor
  virtual ~ChangeObjectListParameterValue()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void createObject(){
    // If index is specified, insert, otherwise push_back
    if(this->mIndex >= 0 && this->mIndex < this->mpParameter->size())
    {
      this->mpParameter->insert(this->mIndex, this->mOldValue);
    }
    else
    {
      this->mpParameter->pushBack(this->mOldValue);
      this->mIndex = this->mpParameter->size() - 1;
    }

    // Load saved configuration, if it was saved
    auto kernelListParameter = dynamic_cast<cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>*>(this->mpParameter);
    if(kernelListParameter != nullptr && !this->mConfig.empty())
    {
      cedar::aux::kernel::Kernel* kernel = kernelListParameter->at(this->mIndex).get();
      if(kernel != nullptr)
      {
        kernel->readConfiguration(this->mConfig);
      }
    }
  }

  void removeObject(){
    CEDAR_ASSERT(this->mIndex >= 0 && this->mIndex < this->mpParameter->size())

    // Save configuration. This is only possible if the type of ObjectListParameterTemplate is known. If more
    // ObjectListParameters get implemented, these can be added here and in the createObject() function
    if(auto kernelListParameter = dynamic_cast<cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel>*>(this->mpParameter))
    {
      kernelListParameter->at(this->mIndex)->writeConfiguration(this->mConfig);
    }
    else
    {
      this->mConfig.clear();
    }
    // Remove object from list
    this->mpParameter->removeObject(this->mIndex);
  }

  bool mergeWith(const QUndoCommand* other) override
  {
    return false;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  void undoAction() override
  {
    switch(this->mAction){
      case CREATE:
        removeObject();
        break;
      case DELETE:
        createObject();
        break;
    }
  }

  void redoAction() override
  {
    switch(this->mAction){
      case CREATE:
        createObject();
        break;
      case DELETE:
        removeObject();
        break;
    }
  }

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

  int mIndex;
  Action mAction;
  cedar::aux::ConfigurationNode mConfig;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::undoRedo::commands::ChangeObjectListParameterValue

#endif // CEDAR_PROC_UNDO_REDO_COMMANDS_CHANGE_OBJECT_LIST_PARAMETER_VALUE_H

