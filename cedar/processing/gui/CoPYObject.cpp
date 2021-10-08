//
// Created by fred on 1/7/21.
//

#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/CoPYObject.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/gui/Element.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteElement.h"
#include "cedar/processing/undoRedo/commands/CreateGroupTemplate.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteConnection.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValueTemplate.h"
#include "cedar/processing/undoRedo/commands/ChangeObjectListParameterValue.h"
#include "cedar/processing/undoRedo/commands/ChangeObjectParameterValue.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/StepItem.h"
#include <QGraphicsSceneDragDropEvent>

#ifdef CEDAR_USE_COPY
QStringList cedar::proc::gui::CoPYObject::createElem(const QString &classId, const int &x, const int &y, const QString &groupname, const int &amount)
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
      auto elem = _mpScene->createElement(group, classId.toStdString(),
                                          QPointF(x, y + i)).get();
      if (cedar::proc::Step *step = dynamic_cast<cedar::proc::Step *>(elem))
      {
        list.append(QString::fromStdString(step->getFullPath()));
      }
      if (cedar::proc::Group *group = dynamic_cast<cedar::proc::Group *>(elem))
      {
        list.append(QString::fromStdString(group->getFullPath()));
      }
      jumpToStep(elem);
      _mpScene->getGraphicsItemFor(elem)->deleteElement();
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteElement(
              QPointF(x, y + i), classId.toStdString(), getGroupByName(groupname.toStdString()), _mpScene, true));

    }
    _mpScene->snapAllItemsToGrid();
    return list;
  }
  catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
    QStringList l;
    return l;
  }

}

QStringList cedar::proc::gui::CoPYObject::createGroupTemplate(const QString &templateId, const int &x, const int &y, const QString &groupId, const int &amount)
{
  try
  {
    QStringList list;
    //todo dont hardcode vertical size of steps
    for (int i = 0; i < amount * 45; i += 45)
    {
      auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
              (
                      templateId.toStdString(),
                      getGroupByName(groupId.toStdString()), false
              ).get();
      if (cedar::proc::Step *step = dynamic_cast<cedar::proc::Step *>(elem))
      {
        list.append(QString::fromStdString(step->getFullPath()));
      }
      if (cedar::proc::Group *group = dynamic_cast<cedar::proc::Group *>(elem))
      {
        list.append(QString::fromStdString(group->getFullPath()));
      }
      jumpToStep(elem);
      _mpScene->getGraphicsItemFor(elem)->deleteElement();
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateGroupTemplate(
              new cedar::proc::GroupDeclaration(templateId.toStdString(), "", "", ""),
              getGroupByName(groupId.toStdString()), new QGraphicsSceneDragDropEvent(), QPointF(x, y + i), _mpScene));
    }
    return list;
  }
  catch (cedar::aux::ExceptionBase e)
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
  }
  catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

void
cedar::proc::gui::CoPYObject::connectSlots(const QString &source, const int &sourceSlotIndex, const QString &target,
                                           const int &targetSlotIndex)
{
  try
  {
    auto sourceElement = getStepByName(source.toStdString());
    auto targetElement = getStepByName(target.toStdString());
    STEP_ASSERT(sourceElement, source.toStdString());
    STEP_ASSERT(targetElement, target.toStdString());

    auto sourceSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
            source.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(1);
    auto targetSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
            target.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(0);

    if (sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or
        targetSlotIndex > (targetSlots.size() - 1))
      CEDAR_THROW(cedar::aux::ExceptionBase, "Wrong Slot Index");

    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
            dynamic_cast<cedar::proc::gui::GraphicsBase*>(dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getGraphicsItemFor(sourceElement))->getSlotItem(sourceSlots[sourceSlotIndex]->getRole(), sourceSlots[sourceSlotIndex]->getName())),
            dynamic_cast<cedar::proc::gui::GraphicsBase*>(dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getGraphicsItemFor(targetElement))->getSlotItem(targetSlots[targetSlotIndex]->getRole(), targetSlots[targetSlotIndex]->getName())),
            true,
            false));

    jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(target.toStdString()).get()));
  }
  catch (cedar::aux::ExceptionBase e) {
    throwError(e.getMessage());
  }
}

void
cedar::proc::gui::CoPYObject::disconnectSlots(const QString &source, const int &sourceSlotIndex, const QString &target,
                                           const int &targetSlotIndex)
{
  try
  {
    auto sourceElement = getStepByName(source.toStdString());
    auto targetElement = getStepByName(target.toStdString());
    STEP_ASSERT(sourceElement, source.toStdString());
    STEP_ASSERT(targetElement, target.toStdString());

    auto sourceSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
            source.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(1);
    auto targetSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
            target.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(0);

    if (sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or
        targetSlotIndex > (targetSlots.size() - 1))
    CEDAR_THROW(cedar::aux::ExceptionBase, "Wrong Slot Index");

    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
            dynamic_cast<cedar::proc::gui::GraphicsBase*>(dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getGraphicsItemFor(sourceElement))->getSlotItem(sourceSlots[sourceSlotIndex]->getRole(), sourceSlots[sourceSlotIndex]->getName())),
            dynamic_cast<cedar::proc::gui::GraphicsBase*>(dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getGraphicsItemFor(targetElement))->getSlotItem(targetSlots[targetSlotIndex]->getRole(), targetSlots[targetSlotIndex]->getName())),
            false,
            false));
    jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(target.toStdString()).get()));
  }
  catch (cedar::aux::ExceptionBase e) {
    throwError(e.getMessage());
  }
}


void cedar::proc::gui::CoPYObject::setParameter(const QString &elem, const QString &paramName, const QVariant &value)
{
  try
  {
    auto assertStep = getStepByName(elem.toStdString());
    STEP_ASSERT(assertStep, elem.toStdString());
    jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(elem.toStdString()).get()));
    cedar::aux::ParameterPtr param = this->getStepByName(elem.toStdString())->getParameter(
            paramName.toStdString().c_str());
    if (param->isConstant())
    {
      return;
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<double> *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<double>(
              paramSet, paramSet->getValue(), value.toDouble(), param->getNamedConfigurableOwner(), _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<std::string> *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string>(
              paramSet, paramSet->getValue(), value.toString().toStdString(), param->getNamedConfigurableOwner(), _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<double> *>(param.get()))
    {
      int originalSize = paramSet->getValue().size();
      QList<QVariant> items = value.toList();
      CEDAR_ASSERT(items.length() == originalSize)
      std::vector<double> vector;
      for (int i = 0; i < originalSize; i++)
      {
        vector.push_back(items[i].toDouble());
      }
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::vector<double>, cedar::aux::VectorParameter<double>>(
              paramSet, paramSet->getValue(), vector, param->getNamedConfigurableOwner(), _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::VectorParameter<unsigned int> *>(param.get()))
    {
      int originalSize = paramSet->getValue().size();
      QList<QVariant> items = value.toList();
      CEDAR_ASSERT(items.length() == originalSize)
      std::vector<unsigned int> vector;
      for (int i = 0; i < originalSize; i++)
      {
        vector.push_back(items[i].toUInt());
      }
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::vector<unsigned int>, cedar::aux::VectorParameter<unsigned int>>(
              paramSet, paramSet->getValue(), vector, param->getNamedConfigurableOwner(), _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<unsigned int> *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<unsigned int>(
              paramSet, paramSet->getValue(), value.toUInt(), param->getNamedConfigurableOwner(), _mpScene
      ));
    }  else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<bool> *>(param.get()))
    {
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<bool>(
              paramSet, paramSet->getValue(), value.toBool(), param->getNamedConfigurableOwner(), _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::EnumParameter *>(param.get()))
    {
      std::string enumId = value.toString().toStdString();
      if (!paramSet->isEnabled(paramSet->getEnumDeclaration().getFromPrettyString(enumId)))
        return;

      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeParameterValueTemplate<std::string, cedar::aux::EnumParameter>(
              paramSet, paramSet->getValue().name(), paramSet->getEnumDeclaration().getFromPrettyString(enumId).name(), param->getNamedConfigurableOwner(), false, _mpScene
      ));
    } else if (auto paramSet = dynamic_cast<cedar::aux::ObjectParameter *>(param.get()))
    {
      std::vector<std::string> typeList;
      paramSet->listTypes(typeList);
      for (std::string type: typeList)
      {
        if (QString::fromStdString(type).contains(value.toString()))
        {
          cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeObjectParameterValue(
                  paramSet, paramSet->getTypeId(), type, param->getNamedConfigurableOwner(), _mpScene
          ));
        }
      }
    }
    /*else if (auto paramSet = dynamic_cast<cedar::aux::ObjectListParameter *>(param.get()))
  {
    std::vector<std::string> typeList;
    paramSet->listTypes(typeList);
    for (std::string type: typeList)
    {
      if (QString::fromStdString(type).contains(value.toString()))
      {
        paramSet->setType();
        break;
      }
    }
  }*/else
    {
      CEDAR_THROW(cedar::aux::ExceptionBase, "No matching parameter type for " + paramName.toStdString() + " found.");
    }
    param->emitChangedSignal();
  }
  catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

void cedar::proc::gui::CoPYObject::addObjectList(const QString &step, const QString &param, const QString &type)
{
  try
  {
    auto assertStep = getStepByName(step.toStdString()).get();
    STEP_ASSERT(assertStep, step.toStdString());
    jumpToStep(dynamic_cast<cedar::proc::Element*>(assertStep));
    cedar::aux::ParameterPtr parameter = this->getStepByName(step.toStdString())->getParameter(
            param.toStdString().c_str());
    if(!parameter) CEDAR_THROW(cedar::aux::ExceptionBase, "Parameter " + param.toStdString() + " not found.");
    if(auto objectListParameter = boost::dynamic_pointer_cast<cedar::aux::ObjectListParameter>(parameter))
    {
      cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::ChangeObjectListParameterValue(
              objectListParameter.get(), type.toStdString(), parameter->getNamedConfigurableOwner(), _mpScene));
    }
  }
  catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
  }
}

cedar::proc::GroupPtr cedar::proc::gui::CoPYObject::getGroupByName(const std::string &groupId)
{
  if (groupId == "root")
  {
    return _mpRootGroup;
  }
  auto list = _mpScene->items();
  for (QGraphicsItem *item : list)
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
    auto step = dynamic_cast<cedar::proc::gui::StepItem *>(this->_mpScene->getElementByFullPath(
            elementIdentifier.c_str()));
    STEP_ASSERT(step, elementIdentifier);
    return step->getStep();
  }
  catch (cedar::aux::ExceptionBase e)
  {
    throwError(e.getMessage());
    return nullptr;
  }
}

void cedar::proc::gui::CoPYObject::throwError(std::string msg)
{
  pQtConsole->reset(msg, true);
}

void cedar::proc::gui::CoPYObject::jumpToStep(cedar::proc::Element* element){
  if (auto item = this->_mpScene->getGraphicsItemFor(element))
  {
    item->setSelected(true);
    item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
    _mpScene->getParentView()->centerOn(item);
  }
}


void cedar::proc::gui::CoPYObjectWrapper::connect(const QVariant &source, const QVariant &target,
                                                  const int &sourceSlot,
                                                  const int &targetSlot)
{
  for (QString src : source.toStringList())
  {
    for (QString tgt : target.toStringList())
    {
      emit connectSig(src, sourceSlot, tgt, targetSlot);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::disconnect(const QVariant &source, const QVariant &target,
                                                  const int &sourceSlot,
                                                  const int &targetSlot)
{
  for (QString src : source.toStringList())
  {
    for (QString tgt : target.toStringList())
    {
      emit disconnectSig(src, sourceSlot, tgt, targetSlot);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::copy(const QString &source, const QVariant &target)
{
  for (QString tgt : target.toStringList())
  {
    emit copySig(source, tgt);
  }
};
#endif
