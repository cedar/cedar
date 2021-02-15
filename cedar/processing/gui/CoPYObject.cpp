//
// Created by fred on 1/7/21.
//

#include <cedar/processing/gui/CoPYObject.h>

#include <cedar/processing/GroupDeclarationManager.h>
#include <cedar/processing/gui/Element.h>
#include <cedar/auxiliaries/casts.h>
#include <cedar/processing/gui/StepItem.h>

QStringList cedar::proc::gui::CoPYObject::createElem(const QString& classId, const int& x, const int& y, const int& amount)
{
  QStringList list;
  QString groupname;
  (_mpGroup == _mpRootGroup)? groupname = "root":groupname = QString::fromUtf8(_mpGroup->getName().c_str());
  //todo dont hardcode vertical size of steps
  for(int i = 0; i < amount * 45; i += 45)
  {
    QString stepName = QString::fromUtf8(_mpScene->
            createElement(_mpGroup, classId.toStdString(), QPointF(x, y + i))
            ->getName().c_str());
    list.append(groupname + "." + stepName);
  }
  return list;
}

void cedar::proc::gui::CoPYObject::createGroup(const QString& groupId, const int& x, const int& y, const int& amount)
{
  auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
          (
                  groupId.toStdString(),
                  _mpGroup,
                  false
          );
  this->_mpScene->getGraphicsItemFor(elem)->setPos(QPointF(x,y));
}

void cedar::proc::gui::CoPYObject::copyTo(const QString &fromStep, const QString &targetStep)
{
  std::string source_group, source_slot_name, target_group, target_slot_name;
  cedar::aux::splitFirst(fromStep.toStdString(), ".", source_group, source_slot_name);
  cedar::aux::splitFirst(targetStep.toStdString(), ".", target_group, target_slot_name);

  this->getStepByName(target_slot_name)->copyFrom(this->getStepByName(source_slot_name));

  /*cedar::proc::StepPtr pSource = this->getGroupByName(source_group)->getElement<cedar::proc::Step>(source_slot_name);
  cedar::proc::gui::StepItem* source = dynamic_cast<cedar::proc::gui::StepItem*>(_mpScene->getStepItemFor(pSource))
  cedar::proc::Step *source = this->getGroupByName(source_group)->getElement<cedar::proc::StepPtr>(source_slot_name);
  this->getGroupByName(target_group)->getElement<cedar::proc::Element>(target_slot_name)->copyFrom(_mpScene->getStepItemFor(source));*/
}

void cedar::proc::gui::CoPYObject::connectSlots(const QString& source, const int& sourceSlotIndex, const QString& target, const int& targetSlotIndex)
{
  std::string source_group;
  std::string source_slot_name;
  std::string target_group;
  std::string target_slot_name;
  cedar::aux::splitFirst(source.toStdString(), ".", source_group, source_slot_name);
  cedar::aux::splitFirst(target.toStdString(), ".", target_group, target_slot_name);

  auto sourceSlot = this->getGroupByName(source_group)
          ->getElement<cedar::proc::Connectable>(source_slot_name)
                  ->getOrderedDataSlots(1)[sourceSlotIndex];
  auto targetSlot = this->getGroupByName(target_group)
          ->getElement<cedar::proc::Connectable>(target_slot_name)
                  ->getOrderedDataSlots(0)[targetSlotIndex];
  cedar::proc::Group::connectAcrossGroups
  (
          cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(sourceSlot),
          cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(targetSlot)
          );
}

void cedar::proc::gui::CoPYObject::setGroup(const QString& groupId){
  auto group = getGroupByName(groupId.toStdString());
  if(group != nullptr){
    _mpGroup = group;
  }
}

cedar::proc::GroupPtr cedar::proc::gui::CoPYObject::getGroupByName(const std::string& groupId){
  if(groupId == "root")
  {
    return _mpRootGroup;
  }
  auto list = _mpScene->items();
  for(QGraphicsItem* item : list){
    if(auto group = dynamic_cast<cedar::proc::gui::Group*>(item)){
      if(group->getGroup()->getName() == groupId)
      {
        return group->getGroup();
      }
    }
  }
}

cedar::proc::StepPtr cedar::proc::gui::CoPYObject::getStepByName(const std::string& stepId){
  auto list = _mpScene->items();
  for(QGraphicsItem* item : list){
    if(auto step = dynamic_cast<cedar::proc::gui::StepItem*>(item)){
      if(step->getStep()->getName() == stepId)
      {
        return step->getStep();
      }
    }
  }
}


// Class cedar::proc::gui::CoPYObjectWrapper
cedar::proc::gui::CoPYObjectWrapper::CoPYObjectWrapper()
{
  this->o = new_CoPYObject();
}

void cedar::proc::gui::CoPYObjectWrapper::connect(CoPYObject* o, const QVariant& source, const QVariant& target, const int& sourceSlot,
                                                  const int& targetSlot)
{
  for(QString src : source.toStringList())
  {
    for(QString tgt : target.toStringList())
    {
      o->connectSlots(src,  sourceSlot, tgt, targetSlot);
    }
  }
};

void cedar::proc::gui::CoPYObjectWrapper::copy(CoPYObject* o, const QString& source, const QVariant& target)
{
  for(QString tgt : target.toStringList())
  {
    o->copyTo(source, tgt);
  }
};


