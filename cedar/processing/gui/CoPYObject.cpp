//
// Created by fred on 1/7/21.
//
#include <cedar/processing/gui/View.h>
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
    auto elem = _mpScene->createElement(_mpGroup, classId.toStdString(),
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
  }
  _mpScene->snapAllItemsToGrid();

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
  jumpToStep(elem.get());
}

void cedar::proc::gui::CoPYObject::copyTo(const QString &fromStep, const QString &targetStep)
{

  this->getStepByName(fromStep.toStdString())->copyFrom(this->getStepByName(targetStep.toStdString()));
}

void
cedar::proc::gui::CoPYObject::connectSlots(const QString &source, const int &sourceSlotIndex, const QString &target,
                                           const int &targetSlotIndex)
{
  auto assertSource = getStepByName(source.toStdString());
  auto assertTarget = getStepByName(target.toStdString());

  auto sourceSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
          source.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(1);
  auto targetSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
          target.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(0);

  if (sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or
      targetSlotIndex > (targetSlots.size() - 1))
    throwError("Wrong Slot Index");

  cedar::proc::Group::connectAcrossGroups
          (
                  cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(sourceSlots[sourceSlotIndex]),
                  cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(targetSlots[targetSlotIndex])
          );
  jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(target.toStdString()).get()));
}

void
cedar::proc::gui::CoPYObject::disconnectSlots(const QString &source, const int &sourceSlotIndex, const QString &target,
                                           const int &targetSlotIndex)
{
  auto assertSource = getStepByName(source.toStdString());
  auto assertTarget = getStepByName(target.toStdString());

  auto sourceSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
          source.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(1);
  auto targetSlots = dynamic_cast<cedar::proc::gui::Connectable *>(this->_mpScene->getElementByFullPath(
          target.toStdString().c_str()))->getConnectable()->getOrderedDataSlots(0);

  if (sourceSlotIndex < 0 or sourceSlotIndex > (sourceSlots.size() - 1) or targetSlotIndex < 0 or
      targetSlotIndex > (targetSlots.size() - 1))
    throwError("Wrong Slot Index");

  cedar::proc::Group::disconnectAcrossGroups
          (
                  cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(sourceSlots[sourceSlotIndex]),
                  cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(targetSlots[targetSlotIndex])
          );
  jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(target.toStdString()).get()));

}


void cedar::proc::gui::CoPYObject::setParameter(const QString &elem, const QString &paramName, const QVariant &value)
{
  jumpToStep(dynamic_cast<cedar::proc::Element*>(getStepByName(elem.toStdString()).get()));

  cedar::aux::ParameterPtr param = this->getStepByName(elem.toStdString())->getParameter(
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
    throwError("No matching ParameterType found.");
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
      if (group->getGroup()->getFullPath() == groupId)
      {
        return group->getGroup();
      }
    }
  }
  throwError("Group \"" + groupId + "\" was not found");
}

cedar::proc::StepPtr cedar::proc::gui::CoPYObject::getStepByName(const std::string &elementIdentifier)
{
  if (auto step = dynamic_cast<cedar::proc::gui::StepItem *>(this->_mpScene->getElementByFullPath(
          elementIdentifier.c_str())))
  {
    return step->getStep();
  }
  throwError("Step \"" + elementIdentifier + "\" was not found");
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

