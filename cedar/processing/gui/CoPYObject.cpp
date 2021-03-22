//
// Created by fred on 1/7/21.
//

#include <cedar/processing/gui/CoPYObject.h>

#include <cedar/processing/GroupDeclarationManager.h>
#include <cedar/processing/gui/Element.h>
#include <cedar/auxiliaries/casts.h>
#include <cedar/processing/gui/StepItem.h>
#include <cedar/auxiliaries/ObjectParameterTemplate.h>
#include <cedar/auxiliaries/math/TransferFunction.h>

QStringList
cedar::proc::gui::CoPYObject::createElem(const QString &classId, const int &x, const int &y, const int &amount)
{
  QStringList list;
  QString groupname;
  (_mpGroup == _mpRootGroup) ? groupname = "root" : groupname = QString::fromUtf8(_mpGroup->getName().c_str());
  //todo dont hardcode vertical size of steps
  for (int i = 0; i < amount * 45; i += 45)
  {
    cedar::proc::Step *step = dynamic_cast<cedar::proc::Step *>(_mpScene->createElement(_mpGroup, classId.toStdString(),
                                                                                        QPointF(x, y + i)).get());
    QString stepName = QString::fromUtf8(step->getName().c_str());
    _mpScene->getStepItemFor(step)->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
    list.append(groupname + "." + stepName);
  }
  return list;
}

void cedar::proc::gui::CoPYObject::createGroup(const QString &groupId, const int &x, const int &y, const int &amount)
{
  auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
          (
                  groupId.toStdString(),
                  _mpGroup,
                  false
          );
  this->_mpScene->getGraphicsItemFor(elem)->setPos(QPointF(x, y));
}

void cedar::proc::gui::CoPYObject::copyTo(const QString &fromStep, const QString &targetStep)
{
  std::string source_group, source_step, target_group, target_step;
  cedar::aux::splitFirst(fromStep.toStdString(), ".", source_group, source_step);
  cedar::aux::splitFirst(targetStep.toStdString(), ".", target_group, target_step);

  this->getStepByName(target_group, target_step)->copyFrom(this->getStepByName(source_group, source_step));

  /*cedar::proc::StepPtr pSource = this->getGroupByName(source_group)->getElement<cedar::proc::Step>(source_slot_name);
  cedar::proc::gui::StepItem* source = dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getStepItemFor(pSource))
  cedar::proc::Step *source = this->getGroupByName(source_group)->getElement<cedar::proc::StepPtr>(source_slot_name);
  this->getGroupByName(target_group)->getElement<cedar::proc::Element>(target_slot_name)->copyFrom(_mpScene->getStepItemFor(source));*/
}

void
cedar::proc::gui::CoPYObject::connectSlots(const QString &source, const int &sourceSlotIndex, const QString &target,
                                           const int &targetSlotIndex)
{
  std::string source_group, source_step, target_group, target_step;
  cedar::aux::splitFirst(source.toStdString(), ".", source_group, source_step);
  cedar::aux::splitFirst(target.toStdString(), ".", target_group, target_step);

  auto sourceSlots = this->getGroupByName(source_group)
          ->getElement<cedar::proc::Connectable>(source_step)
          ->getOrderedDataSlots(1);
  auto targetSlots = this->getGroupByName(target_group)
          ->getElement<cedar::proc::Connectable>(target_step)
          ->getOrderedDataSlots(0);

  if(sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or targetSlotIndex > (targetSlots.size() - 1))
    throw std::invalid_argument("Wrong Slot Index");

  cedar::proc::Group::connectAcrossGroups
          (
                  cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(sourceSlots[sourceSlotIndex]),
                  cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(targetSlots[targetSlotIndex])
          );

}

void cedar::proc::gui::CoPYObject::setParameter(const QString &elem, const QString &paramName, const QVariant &value)
{
  std::string groupId, stepName;
  cedar::aux::splitFirst(elem.toStdString(), ".", groupId, stepName);
  cedar::aux::ParameterPtr param = this->getStepByName(groupId, stepName)->getParameter(
          paramName.toStdString().c_str());
  if (param->isConstant())
  {
    return;
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<double> *>(param.get()))
  {
    paramSet->setValue(value.toDouble());
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<std::string> *>(param.get()))
  {
    paramSet->setValue(value.toString().toStdString());
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
    paramSet->setValue(vector);
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
    paramSet->setValue(vector);
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<unsigned int> *>(param.get()))
  {
    paramSet->setValue(value.toUInt());
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<double> *>(param.get()))
  {
    paramSet->setValue(value.toDouble());
  } else if (auto paramSet = dynamic_cast<cedar::aux::ParameterTemplate<bool> *>(param.get()))
  {
    paramSet->setValue(value.toBool());
  } else if (auto paramSet = dynamic_cast<cedar::aux::EnumParameter *>(param.get()))
  {
    std::string enumId = value.toString().toStdString();
    if (!paramSet->isEnabled(paramSet->getEnumDeclaration().getFromPrettyString(enumId)))
      return;
    paramSet->setValue(paramSet->getEnumDeclaration().getFromPrettyString(enumId).id());
  } else if (auto paramSet = dynamic_cast<cedar::aux::ObjectParameter *>(param.get()))
  {
    std::vector<std::string> typeList;
    paramSet->listTypes(typeList);
    for (std::string type: typeList)
    {
      if (QString::fromStdString(type).contains(value.toString()))
      {
        paramSet->setType(type);
        break;
      }
    }
  } /*else if (auto paramSet = dynamic_cast<cedar::aux::ObjectListParameter *>(param.get()))
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
    throw std::invalid_argument("No matching ParameterType found.");
  }
  param->emitChangedSignal();
}

void cedar::proc::gui::CoPYObject::setGroup(const QString &groupId)
{
  auto group = getGroupByName(groupId.toStdString());
  if (group != nullptr)
  {
    _mpGroup = group;
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
      if (group->getGroup()->getName() == groupId)
      {
        return group->getGroup();
      }
    }
  }
  throw std::invalid_argument("Group \"" + groupId + "\" was not found");
}

cedar::proc::StepPtr cedar::proc::gui::CoPYObject::getStepByName(const std::string &groupId, const std::string &stepId)
{
  auto list = _mpScene->items();
  for (QGraphicsItem *item : list)
  {
    if (auto step = dynamic_cast<cedar::proc::gui::StepItem *>(item))
    {
      if (step->getStep()->getName() == stepId && step->getStep()->getGroup() == this->getGroupByName(groupId))
      {
        return step->getStep();
      }
    }
  }
  throw std::invalid_argument("Step \"" + groupId + "." + stepId + "\" was not found");
}


// Class cedar::proc::gui::CoPYObjectWrapper
cedar::proc::gui::CoPYObjectWrapper::CoPYObjectWrapper()
{
  this->o = new_CoPYObject();
}

void cedar::proc::gui::CoPYObjectWrapper::connect(CoPYObject *o, const QVariant &source, const QVariant &target,
                                                  const int &sourceSlot,
                                                  const int &targetSlot)
{
  for (QString src : source.toStringList())
  {
    for (QString tgt : target.toStringList())
    {
      o->connectSlots(src, sourceSlot, tgt, targetSlot);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::copy(CoPYObject *o, const QString &source, const QVariant &target)
{
  for (QString tgt : target.toStringList())
  {
    o->copyTo(source, tgt);
  }
};


