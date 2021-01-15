//
// Created by fred on 1/7/21.
//

#include "CoPYObject.h"

#include <cedar/processing/GroupDeclarationManager.h>
#include <cedar/processing/gui/Element.h>
#include <cedar/auxiliaries/casts.h>
#include <cedar/processing/gui/StepItem.h>

void CoPYObject::createElem(const QString& classId, const int& x, const int& y, const int& amount)
{
  //todo dont hardcode vertical size of steps
  for(int i = 0; i < amount * 45; i += 45)
  {
    _mpScene->createElement(_mpGroup, classId.toStdString(), QPointF(x, y + i));
  }
}

void CoPYObject::createGroup(const QString& groupId, const int& x, const int& y)
{
  auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
          (
                  groupId.toStdString(),
                  _mpGroup,
                  false
          );
  this->_mpScene->getGraphicsItemFor(elem)->setPos(QPointF(x,y));
}

void CoPYObject::copyTo(const QString &fromStep, const QString &targetStep)
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

void CoPYObject::connectSlots(const QString& first, const int& firstSlot, const QString& second, const int& secondSlot)
{
  std::string source_group;
  std::string source_slot_name;
  std::string target_group;
  std::string target_slot_name;
  cedar::aux::splitFirst(first.toStdString(), ".", source_group, source_slot_name);
  cedar::aux::splitFirst(second.toStdString(), ".", target_group, target_slot_name);
  std::cout << "Group 1: " + source_group << std::endl;
  std::cout << "Slot 1: " + source_slot_name << std::endl;

  auto sourceSlot = this->getGroupByName(source_group)
          ->getElement<cedar::proc::Connectable>(source_slot_name)
                  ->getOrderedDataSlots(1)[firstSlot];
  auto targetSlot = this->getGroupByName(target_group)
          ->getElement<cedar::proc::Connectable>(target_slot_name)
                  ->getOrderedDataSlots(0)[secondSlot];
  cedar::proc::Group::connectAcrossGroups
  (
          cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(sourceSlot),
          cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(targetSlot)
          );
}

void CoPYObject::setGroup(const QString& groupId){
  auto group = getGroupByName(groupId.toStdString());
  if(group != nullptr){
    _mpGroup = group;
  }
}

void CoPYObject::setGroup(){
  _mpGroup = _mpRootGroup;
  std::cout << _mpRootGroup->getName() << std::endl;
}

cedar::proc::GroupPtr CoPYObject::getGroupByName(const std::string groupId){
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

cedar::proc::StepPtr CoPYObject::getStepByName(const std::string& stepId){
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


// Class CoPYObjectWrapper
CoPYObjectWrapper::CoPYObjectWrapper(const int& test)
{
  this->o = new_CoPYObject(test);
}
CoPYObjectWrapper::CoPYObjectWrapper()
{
  CoPYObjectWrapper(1);
}


