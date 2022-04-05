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

    You should have received a Copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CoPYObject.cpp

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_COPY
//CEDAR INCLUDES

#include "cedar/auxiliaries/FileLog.fwd.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/gui/CoPYObject.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/PythonQtConsole.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/undoRedo/commands/ChangeObjectListParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeObjectParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValueTemplate.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteConnection.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteElement.h"
#include "cedar/processing/undoRedo/commands/CreateGroupTemplate.h"

#ifdef CEDAR_USE_PYTHONSTEP
#include "cedar/processing/steps/PythonScript.h"
#include "cedar/processing/steps/PythonScriptLooped.h"
#endif // CEDAR_USE_PYTHONSTEP

// SYSTEM INCLUDES

#include <QGraphicsSceneDragDropEvent>


QStringList
cedar::proc::gui::CoPYObject::createElem(const QString &classId, const int &x, const int &y, const QString &groupname,
                                         const int &amount)
{
  try
  {
    QStringList list;
    //todo dont hardcode vertical size of steps
    for (int i = 0; i < amount * 45; i += 45)
    {
      auto group = getGroupByName(groupname.toStdString());
      GROUP_ASSERT(group, groupname.toStdString());
      CLASS_ASSERT(classId);
      auto elem = mpScene->createElement(group, classId.toStdString(), QPointF(x, y + i)).get();
      if (cedar::proc::Step *step = dynamic_cast<cedar::proc::Step *>(elem))
      {
        list.append(QString::fromStdString(step->getFullPath()));
      }
      if (cedar::proc::Group *group = dynamic_cast<cedar::proc::Group *>(elem))
      {
        list.append(QString::fromStdString(group->getFullPath()));
      }
      jumpToStep(elem);
      mpScene->getGraphicsItemFor(elem)->deleteElement();
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::CreateDeleteElement(classId.toStdString(),
                                                                       getGroupByName(groupname.toStdString()),
                                                                       mpScene, true, QPointF(x, y + i)));

    }
    mpScene->snapAllItemsToGrid();
    return list;
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
    QStringList l;
    return l;
  }

}

QStringList cedar::proc::gui::CoPYObject::createGroupTemplate(const QString &templateId, const int &x, const int &y,
                                                              const QString &groupId, const int &amount)
{
  try
  {
    QStringList list;
    //todo dont hardcode vertical size of steps
    for (int i = 0; i < amount * 45; i += 45)
    {
      auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup(
              templateId.toStdString(), getGroupByName(groupId.toStdString()), false).get();
      if (cedar::proc::Step *step = dynamic_cast<cedar::proc::Step *>(elem))
      {
        list.append(QString::fromStdString(step->getFullPath()));
      }
      if (cedar::proc::Group *group = dynamic_cast<cedar::proc::Group *>(elem))
      {
        list.append(QString::fromStdString(group->getFullPath()));
      }
      jumpToStep(elem);
      mpScene->getGraphicsItemFor(elem)->deleteElement();
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateGroupTemplate(
              new cedar::proc::GroupDeclaration(templateId.toStdString(), "", "", ""),
              getGroupByName(groupId.toStdString()), new QGraphicsSceneDragDropEvent(), QPointF(x, y + i), mpScene));
    }
    return list;
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
    QStringList l;
    return l;
  }
}

void cedar::proc::gui::CoPYObject::copyTo(const QString &fromStep, const QString &targetStep)
{
  try
  {
    auto sourceElement = getStepByName(fromStep.toStdString());
    auto targetElement = getStepByName(targetStep.toStdString());
    STEP_ASSERT(sourceElement, fromStep.toStdString());
    STEP_ASSERT(targetElement, targetStep.toStdString());
    targetElement->copyFrom(sourceElement);
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

void
cedar::proc::gui::CoPYObject::connectSlots(const QString &source, const QVariant &sourceSlot, const QString &target,
                                           const QVariant &targetSlot, const bool &disconnect)
{

  try
  {
    auto sourceElement = getStepByName(source.toStdString());
    auto targetElement = getStepByName(target.toStdString());

    #ifdef CEDAR_USE_PYTHONSTEP
    if(dynamic_cast<cedar::proc::steps::PythonScript*>(targetElement.get()) || dynamic_cast<cedar::proc::steps::PythonScriptLooped*>(targetElement.get()))
    {
      CEDAR_THROW(cedar::aux::ExceptionBase, "You can't connect to PythonScript Steps in CoPY due to Python issues.");
    }
    #endif // CEDAR_USE_PYTHONSTEP

    STEP_ASSERT(sourceElement, source.toStdString());
    STEP_ASSERT(targetElement, target.toStdString());

    auto sourceSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->mpScene->getElementByFullPath(
            source.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(1);
    auto targetSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->mpScene->getElementByFullPath(
            target.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(0);
    bool srcSlotIsString = false;
    bool tgtSlotIsString = false;
    int sourceSlotIndex = -1;
    int targetSlotIndex = -1;
    if (sourceSlot.userType() == QMetaType::QString)
    {
      srcSlotIsString = true;
    } else
    {
      sourceSlotIndex = sourceSlot.toInt();
    }
    if (targetSlot.userType() == QMetaType::QString)
    {
      tgtSlotIsString = true;
    } else
    {
      targetSlotIndex = targetSlot.toInt();
    }
    if (srcSlotIsString)
    {
      int i = 0;
      for (cedar::proc::DataSlotPtr slt: sourceSlots)
      {
        if (slt->getName() == sourceSlot.toString().toStdString())
        {
          sourceSlotIndex = i;
          break;
        }
        i++;
      }
    }
    if (tgtSlotIsString)
    {
      int i = 0;
      for (cedar::proc::DataSlotPtr slt: targetSlots)
      {
        if (slt->getName() == targetSlot.toString().toStdString())
        {
          targetSlotIndex = i;
          break;
        }
        i++;
      }
    }
    if (sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or
        targetSlotIndex > (targetSlots.size() - 1))
    CEDAR_THROW(cedar::aux::ExceptionBase, "Wrong Slot Name or Index");

    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
            dynamic_cast<cedar::proc::gui::GraphicsBase *>(dynamic_cast<cedar::proc::gui::StepItem *>(mpScene->getGraphicsItemFor(
                    sourceElement))->getSlotItem(sourceSlots[sourceSlotIndex]->getRole(),
                                                 sourceSlots[sourceSlotIndex]->getName())),
            dynamic_cast<cedar::proc::gui::GraphicsBase *>(dynamic_cast<cedar::proc::gui::StepItem *>(mpScene->getGraphicsItemFor(
                    targetElement))->getSlotItem(targetSlots[targetSlotIndex]->getRole(),
                                                 targetSlots[targetSlotIndex]->getName())), !disconnect, false));

    jumpToStep(dynamic_cast<cedar::proc::Element *>(getStepByName(target.toStdString()).get()));
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

void cedar::proc::gui::CoPYObject::setParameter(const QString &elem, const QString &paramName, const QVariant &value)
{
  try
  {
    auto assertStep = getStepByName(elem.toStdString());
    STEP_ASSERT(assertStep, elem.toStdString());
    jumpToStep(dynamic_cast<cedar::proc::Element *>(getStepByName(elem.toStdString()).get()));
    cedar::aux::ParameterPtr param = this->getStepByName(elem.toStdString())->getParameter(
            paramName.toStdString().c_str());
    if (param->isConstant())
    {
      return;
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<double> *>(param.get()))
    {
      if (!(value.userType() == QMetaType::Double || value.userType() == QMetaType::Int))
      {
        CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + paramName.toStdString() + " by Step " + elem.toStdString()  +" has to be of type double.");
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<double>(paramSet, paramSet->getValue(),
                                                                                        value.toDouble(),
                                                                                        param->getNamedConfigurableOwner(),
                                                                                        mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<std::string> *>(param.get()))
    {
      if (!(value.userType() == QMetaType::QString))
      {
        CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + paramName.toStdString() + " by Step " + elem.toStdString()  +" has to be a String.");
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string>(paramSet,
                                                                                             paramSet->getValue(),
                                                                                             value.toString().toStdString(),
                                                                                             param->getNamedConfigurableOwner(),
                                                                                             mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<double> *>(param.get()))
    {

      int originalSize = paramSet->getValue().size();
      QList<QVariant> items = value.toList();
      CEDAR_ASSERT(items.length() == originalSize)
      std::vector<double> vector;
      for (int i = 0; i < originalSize; i++)
      {
        if (!(items[i].userType() == QMetaType::Double || items[i].userType() == QMetaType::Int))
        {
          CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + paramName.toStdString() + " by Step " + elem.toStdString()  +" has to be a list with only doubles.");
        }
        vector.push_back(items[i].toDouble());
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::vector<double>, cedar::aux::VectorParameter<double>>(
                      paramSet, paramSet->getValue(), vector, param->getNamedConfigurableOwner(), mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<unsigned int> *>(param.get()))
    {
      int originalSize = paramSet->getValue().size();
      QList<QVariant> items = value.toList();
      CEDAR_ASSERT(items.length() == originalSize)
      std::vector<unsigned int> vector;
      for (int i = 0; i < originalSize; i++)
      {
        if (!(items[i].userType() == QMetaType::UInt))
        {
          CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + paramName.toStdString() + " by Step " + elem.toStdString()  +" has to be a list with only unsigned ints.");
        }
        vector.push_back(items[i].toUInt());
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::vector<unsigned int>, cedar::aux::VectorParameter<unsigned int>>(
                      paramSet, paramSet->getValue(), vector, param->getNamedConfigurableOwner(), mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<unsigned int> *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<unsigned int>(paramSet,
                                                                                              paramSet->getValue(),
                                                                                              value.toUInt(),
                                                                                              param->getNamedConfigurableOwner(),
                                                                                              mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<bool> *>(param.get()))
    {
      if (!(value.userType() == QMetaType::Bool || value == 1 || value == 0))
      {
        CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + paramName.toStdString() + " by Step " + elem.toStdString()  +" has to be a boolean.");
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<bool>(paramSet, paramSet->getValue(),
                                                                                      value.toBool(),
                                                                                      param->getNamedConfigurableOwner(),
                                                                                      mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::EnumParameter *>(param.get()))
    {
      cedar::aux::Enum targetId;
      bool error = false;
      if(value.userType() == QMetaType::QString)
      {
        targetId = paramSet->getEnumDeclaration().getFromPrettyString(value.toString().toStdString());
      }
      else if (value.userType() == QMetaType::Int){
        targetId = paramSet->getEnumDeclaration().get(value.toInt());
      }
      std::vector<cedar::aux::Enum> listOfAllTypes = paramSet->getEnumDeclaration().list();
      if(!paramSet->isEnabled(targetId) || !(std::find(listOfAllTypes.begin(), listOfAllTypes.end(), targetId) != listOfAllTypes.end())){
        CEDAR_THROW(cedar::aux::ExceptionBase,
                    "No matching enum type" + value.toString().toStdString() + "in Parameter " +
                    paramName.toStdString() + " found. Use for example the index or the pretty string as \"" + listOfAllTypes[0].prettyString() + "\".");
      }
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string, cedar::aux::EnumParameter>(
                      paramSet, paramSet->getValue().name(),
                      targetId.name(),
                      param->getNamedConfigurableOwner(), false, mpScene));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ObjectParameter *>(param.get()))
    {
      std::vector<std::string> typeList;
      paramSet->listTypes(typeList);
      std::string newType;
      if (value.canConvert<int>()) newType = typeList[value.toDouble()];
      for (std::string type: typeList)
      {
        if (QString::fromStdString(type).contains(value.toString()))
        {
          newType = type;
          break;
        }
      }
      if (!newType.empty())
      {
        cedar::proc::gui::Ide::pUndoStack->push(
                new cedar::proc::undoRedo::commands::ChangeObjectParameterValue(paramSet, paramSet->getTypeId(),
                                                                                newType,
                                                                                param->getNamedConfigurableOwner(),
                                                                                mpScene));
      } else
      {
        CEDAR_THROW(cedar::aux::ExceptionBase,
                    "No matching object type" + value.toString().toStdString() + "in Parameter " +
                    paramName.toStdString() + " found. Use for example the index 0 or \"" + typeList[0] + "\".");
      }

    } else if (auto paramSet = dynamic_cast<cedar::aux::FileParameter *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string, cedar::aux::FileParameter>(
                      paramSet, paramSet->getPath(), value.toString().toStdString(), param->getNamedConfigurableOwner(),
                      mpScene));

    } else
    {
      CEDAR_THROW(cedar::aux::ExceptionBase, "No matching parameter type for " + paramName.toStdString() + " found.");
    }
    param->emitChangedSignal();
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

QStringList cedar::proc::gui::CoPYObject::getElementsByGroup(const QString &groupId)
{
  std::map<std::basic_string<char>, boost::shared_ptr<cedar::proc::Element>> elemMap = getGroupByName(
          groupId.toStdString())->getElements();
  std::map<std::basic_string<char>, boost::shared_ptr<cedar::proc::Element>>::iterator it = elemMap.begin();
  QStringList elemList;
  while (it != elemMap.end())
  {
    if (auto step = dynamic_cast<cedar::proc::Step *>(it->second.get()))
    {
      elemList.append(QString::fromStdString(it->first));
    }
    it++;
  }
  return elemList;
}

void cedar::proc::gui::CoPYObject::addObjectList(const QString &step, const QString &param, const QString &type)
{
  try
  {
    auto assertStep = getStepByName(step.toStdString()).get();
    STEP_ASSERT(assertStep, step.toStdString());
    jumpToStep(dynamic_cast<cedar::proc::Element *>(assertStep));
    cedar::aux::ParameterPtr parameter = this->getStepByName(step.toStdString())->getParameter(
            param.toStdString().c_str());
    if (!parameter)
    CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + param.toStdString() + " not found.");
    if (auto objectListParameter = boost::dynamic_pointer_cast<cedar::aux::ObjectListParameter>(parameter))
    {
      cedar::proc::gui::Ide::pUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeObjectListParameterValue(objectListParameter.get(),
                                                                                  type.toStdString(),
                                                                                  parameter->getNamedConfigurableOwner(),
                                                                                  mpScene));
    }
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

cedar::proc::GroupPtr cedar::proc::gui::CoPYObject::getGroupByName(const std::string &groupId)
{
  if (groupId == "root")
  {
    return mpRootGroup;
  }
  auto list = mpScene->items();
  for (QGraphicsItem *item: list)
  {
    if (auto group = dynamic_cast<cedar::proc::gui::Group *>(item))
    {
      if (group->getGroup()->getFullPath() == groupId)
      {
        return group->getGroup();
      }
    }
  }
  return nullptr;
}

cedar::proc::StepPtr cedar::proc::gui::CoPYObject::getStepByName(const std::string &elementIdentifier)
{
  try
  {
    auto step = dynamic_cast<cedar::proc::gui::StepItem *>(this->mpScene->getElementByFullPath(
            elementIdentifier.c_str()));
    STEP_ASSERT(step, elementIdentifier);
    return step->getStep();
  } catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
    return nullptr;
  }
}

void cedar::proc::gui::CoPYObject::throwError(std::string msg)
{
  pQtConsole->reset(msg, true);
}

void cedar::proc::gui::CoPYObject::jumpToStep(cedar::proc::Element *element)
{
  if (auto item = this->mpScene->getGraphicsItemFor(element))
  {
    item->setSelected(true);
    item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
    mpScene->getParentView()->centerOn(item);
  }
}

cedar::proc::gui::CoPYObject::~CoPYObject()
{

}

cedar::proc::gui::CoPYObjectWrapper::~CoPYObjectWrapper()
{

}


void
cedar::proc::gui::CoPYObjectWrapper::connect(const QVariant &source, const QVariant &target, const QVariant &sourceSlot,
                                             const QVariant &targetSlot)
{
  for (QString src: source.toStringList())
  {
    for (QString tgt: target.toStringList())
    {
      emit connectSig(src, sourceSlot, tgt, targetSlot, false);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::disconnect(const QVariant &source, const QVariant &target,
                                                     const QVariant &sourceSlot, const QVariant &targetSlot)
{
  for (QString src: source.toStringList())
  {
    for (QString tgt: target.toStringList())
    {
      emit connectSig(src, sourceSlot, tgt, targetSlot, true);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::copyAllParameters(const QString &source, const QVariant &target)
{
  for (QString tgt: target.toStringList())
  {
    emit copySig(source, tgt);
  }
};

#endif
