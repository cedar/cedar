//
// Created by fred on 1/7/21.
//

#ifndef CEDAR_COPYOBJECT_H
#define CEDAR_COPYOBJECT_H
#include "PythonQt.h"
#include <cedar/processing/gui/Scene.h>
#include <cedar/processing/gui/Settings.h>
#include <cedar/processing/gui/Group.h>
#include <cedar/processing/Group.h>
#include <cedar/processing/DataSlot.h>
#include <cedar/processing/ElementDeclaration.h>
#include <QObject>
#include <QGraphicsSceneDragDropEvent>
#include <cedar/auxiliaries/Log.h>
#include <cedar/auxiliaries/gui/Settings.h>

class CoPYObject
{
public:
  CoPYObject() {}
  CoPYObject(const int& test) {_test = test;}

  int _test;

  CoPYObject* object;

  /*!@brief create a Group in Subgroup*/
  void setObject(CoPYObject* pObject) {object = pObject;};

  /*!@brief create a Group in Subgroup*/
  void setScene(cedar::proc::gui::Scene* pScene) {_mpScene = pScene;}

  /*!@brief create a Group in Subgroup*/
  void setGroup(cedar::proc::GroupPtr pGroup) {_mpGroup = pGroup; _mpRootGroup = pGroup;}

  /*!@brief create a Group in Subgroup*/
  void setGroup(const QString& groupId);

  /*!@brief create a Group in Subgroup*/
  void setGroup();

  /*!@brief create a Group in Subgroup*/
  void setTest2(int pTest) {_test = pTest;}

  /*!@brief create a Group in Subgroup*/
  void createElem(const QString& classId, const int& x, const int& y, const int& amount);

  /*!@brief create a Group in Subgroup*/
  void createGroup(const QString& classId, const int& x, const int& y);

  /*!@brief connect two Steps at indexed DataSlots*/
  void connectSlots(const QString& first, const int& firstSlot, const QString& second, const int& secondSlot);

  void copyTo(const QString &fromStep, const QString &targetStep);


  cedar::proc::GroupPtr _mpGroup;

  cedar::proc::GroupPtr _mpRootGroup;

  cedar::proc::gui::Scene* _mpScene;
private:
  cedar::proc::StepPtr getStepByName(const std::string& stepId);
  cedar::proc::GroupPtr getGroupByName(const std::string name);
};

Q_DECLARE_METATYPE(CoPYObject)
Q_DECLARE_METATYPE(CoPYObject*)
Q_DECLARE_METATYPE(CoPYObject**)

class CoPYObjectWrapper : public QObject
{
  Q_OBJECT
public:

  CoPYObjectWrapper(const int& test);
  CoPYObjectWrapper();

  CoPYObject* o;

  CoPYObject* getO(){return this->o;};

public Q_SLOTS:
  CoPYObject* new_CoPYObject(const int& test) {
    CoPYObject* o = new CoPYObject(test);
    o -> setObject(o);
    return o;
  }

  CoPYObject* object(){return this-> o;}

  int test(CoPYObject* o) {return o-> _test; }

  void setTest(CoPYObject* o, const int& test) {o->_test = test;}

  void setGroup(CoPYObject* o, const QString& groupId) {o->setGroup(groupId);}

  void setGroup(CoPYObject* o) {o->setGroup();}

  void create(CoPYObject* o, const QString& classId, const int& x, const int& y) { o->createElem(classId, x, y, 1);}

  void connect(CoPYObject* o, const QString& first, const int& firstSlot, const QString& second, const int& secondSlot) { o->connectSlots(first, firstSlot, second, secondSlot);}

  void connect(CoPYObject* o, const QString& first, const QString& second) { o->connectSlots(first, 0, second, 0);}

  void create(CoPYObject* o, const QString& classId, const int& x, const int& y, const int& amount) { o->createElem(classId, x, y, amount);}

  void createGroup(CoPYObject* o, const QString& groupId, const int& x, const int& y) { o->createGroup(groupId, x, y);}

  void copy(CoPYObject* o, const QString& source, const QString& target) {o->copyTo(source, target);}
};

#endif //CEDAR_COPYOBJECT_H
