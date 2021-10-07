//
// Created by fred on 1/7/21.
//


#include <cedar/configuration.h>

#ifndef CEDAR_PROC_GUI_COPY_OBJECT_H
#define CEDAR_PROC_GUI_COPY_OBJECT_H

#ifdef CEDAR_USE_COPY
#include "PythonQt.h"
#include <cedar/processing/gui/Scene.h>
#include <cedar/processing/gui/Settings.h>
#include <cedar/processing/gui/PythonQtConsole.fwd.h>
#include <cedar/processing/gui/Group.h>
#include <cedar/processing/Group.h>
#include <cedar/processing/DataSlot.h>
#include <cedar/processing/ElementDeclaration.h>
#include <QObject>
#include <QGraphicsSceneDragDropEvent>
#include <cedar/auxiliaries/Log.h>
#include <cedar/auxiliaries/gui/Settings.h>
#include <cedar/processing/gui/CoPYObject.fwd.h>

class cedar::proc::gui::CoPYObject : public QObject
{
Q_OBJECT
public:
  CoPYObject(cedar::proc::gui::PythonQtConsole* pConsole) {pQtConsole = pConsole;}
  ~CoPYObject(){

}
  cedar::proc::GroupPtr _mpGroup;

  cedar::proc::GroupPtr _mpRootGroup;

  cedar::proc::gui::Scene* _mpScene;

  cedar::proc::gui::PythonQtConsole* pQtConsole;

  /*!@brief setter for a reference to the scene*/
  void setScene(cedar::proc::gui::Scene* pScene) {_mpScene = pScene;}

  void setGroup(cedar::proc::GroupPtr pGroup) {_mpGroup = pGroup; _mpRootGroup = pGroup;}

public slots:
  /*!@brief setter for a constant reference to the Root, callable by cpp*/

  /*!@brief setter to change the scope we are coding/creating in*/
  void setGroup(const QString& groupId);

  /*!@brief create an Element in the GroupScope*/
  QStringList createElem(const QString& classId, const int& x, const int& y, const int& amount);

  /*!@brief create a Group in Subgroup*/
  void createGroup(const QString& classId, const int& x, const int& y, const int& amount);

  /*!@brief connect two Steps at indexed DataSlots*/
  void connectSlots(const QString& source, const int& sourceSlot, const QString& target, const int& targetSlot);

  /*!@brief connect two Steps at indexed DataSlots*/
  void disconnectSlots(const QString& source, const int& sourceSlot, const QString& target, const int& targetSlot);

  //!@brief copy StepConfiguration from source to target
  void copyTo(const QString &fromStep, const QString &targetStep);

  //!@brief set specific parameter of given elementString
  void setParameter(const QString &elem, const QString &param, const QVariant &value);

private:
  cedar::proc::StepPtr getStepByName(const std::string& elementIdentifier);
  cedar::proc::GroupPtr getGroupByName(const std::string& name);
  void jumpToStep(cedar::proc::Element* elementIdentifier);
  /*!@brief throw an error as well as in cedar and in python*/
  void throwError(std::string msg);
};

/*Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject)
Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject*)*/

class cedar::proc::gui::CoPYObjectWrapper : public QObject
{
Q_OBJECT
public:

  CoPYObjectWrapper(){};

  ~CoPYObjectWrapper()
  {};
/*
  CoPYObject *o;

  CoPYObject *getO()
  { return this->o; };*/

public Q_SLOTS:

  /* Methods to call in Python eg. (py.setGroup), calling slots in CPP-CoPY-Object */

  void setGroup(const QString &groupId = "root")
  { emit setGroupSig(groupId);}

  void connect(const QVariant &first, const QVariant &second, const int &firstSlot = 0,
               const int &secondSlot = 0);

  void disconnect(const QVariant &first, const QVariant &second, const int &firstSlot = 0,
               const int &secondSlot = 0);

  QStringList create(const QString &classId, const int &x, const int &y, const int &amount = 1)
  {
    return emit createSig(classId, x, y, amount);
  }

  void createGroup(const QString &groupId, const int &x, const int &y, const int &amount = 1)
  { emit createGroupSig(groupId, x, y, amount); }

  void copy(const QString &source, const QVariant &target);

  void setParameter(const QString &elem, const QString &param, const QVariant& value){
    emit setParameterSig(elem, param, value);
  }
signals:
  QStringList createSig(const QString &classId, const int &x, const int &y, const int &amount);
  void setParameterSig(const QString &elem, const QString &param, const QVariant& value);
  void copySig(const QString &source, const QString &target);
  void createGroupSig(const QString &groupId, const int &x, const int &y, const int &amount);
  void connectSig(const QString &src, const int &firstSlot, const QString &tgt, const int &targetSlot);
  void disconnectSig(const QString &src, const int &firstSlot, const QString &tgt, const int &targetSlot);
  void setGroupSig(const QString &groupId);
};
#endif //CEDAR_USE_COPY
#endif //CEDAR_PROC_GUI_COPY_OBJECT_H
