//
// Created by fred on 1/7/21.
//
#include <cedar/configuration.h>

#ifndef CEDAR_PROC_GUI_COPY_OBJECT_H
#define CEDAR_PROC_GUI_COPY_OBJECT_H

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
#include <cedar/processing/gui/CoPYObject.fwd.h>

class cedar::proc::gui::CoPYObject
{
public:
  CoPYObject() {}
  ~CoPYObject(){}

  /*!@brief setter for a reference to the scene*/
  void setScene(cedar::proc::gui::Scene* pScene) {_mpScene = pScene;}

  /*!@brief setter for a constant reference to the Root, callable by cpp*/
  void setGroup(cedar::proc::GroupPtr pGroup) {_mpGroup = pGroup; _mpRootGroup = pGroup;}

  /*!@brief setter to change the scope we are coding/creating in*/
  void setGroup(const QString& groupId);

  /*!@brief create an Element in the GroupScope*/
  QStringList createElem(const QString& classId, const int& x, const int& y, const int& amount);

  /*!@brief create a Group in Subgroup*/
  void createGroup(const QString& classId, const int& x, const int& y, const int& amount);

  /*!@brief connect two Steps at indexed DataSlots*/
  void connectSlots(const QString& source, const int& sourceSlot, const QString& target, const int& targetSlot);

  //!@brief copy StepConfiguration from source to target
  void copyTo(const QString &fromStep, const QString &targetStep);

  cedar::proc::GroupPtr _mpGroup;

  cedar::proc::GroupPtr _mpRootGroup;

  cedar::proc::gui::Scene* _mpScene;
private:
  cedar::proc::StepPtr getStepByName(const std::string& stepId);
  cedar::proc::GroupPtr getGroupByName(const std::string& name);
};

Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject)
Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject*)

class cedar::proc::gui::CoPYObjectWrapper : public QObject
{
Q_OBJECT
public:

  CoPYObjectWrapper();

  ~CoPYObjectWrapper()
  {};

  CoPYObject *o;

  CoPYObject *getO()
  { return this->o; };

public Q_SLOTS:

  CoPYObject *new_CoPYObject()
  {
    o = new CoPYObject();
    return o;
  }

  CoPYObject *object()
  { return this->o; }

  /* Methods to call in Python eg. (py.setGroup), redirecting to CPP-CoPY-Object */

  void setGroup(CoPYObject *o, const QString &groupId = "root")
  { o->setGroup(groupId); }

  void connect(CoPYObject *o, const QVariant &first, const QVariant &second, const int &firstSlot = 0,
               const int &secondSlot = 0);

  QStringList create(CoPYObject *o, const QString &classId, const int &x, const int &y, const int &amount = 1)
  { return o->createElem(classId, x, y, amount); }

  void createGroup(CoPYObject *o, const QString &groupId, const int &x, const int &y, const int &amount = 1)
  { o->createGroup(groupId, x, y, amount); }

  void copy(CoPYObject *o, const QString &source, const QVariant &target);
};
#endif //CEDAR_PROC_GUI_COPY_OBJECT_H
