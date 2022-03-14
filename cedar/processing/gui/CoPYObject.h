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

    File:        CoPYObject.h

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:

    Credits:

======================================================================================================================*/
//CEDAR CONFIGURATION

#include <cedar/configuration.h>

#ifndef CEDAR_PROC_GUI_COPY_OBJECT_H
#define CEDAR_PROC_GUI_COPY_OBJECT_H

#ifdef CEDAR_USE_COPY

//FORWARD DECLARATIONS

#include "cedar/processing/gui/PythonQtConsole.fwd.h"
#include "cedar/processing/gui/CoPYObject.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/Group.fwd.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Settings.h"

// SYSTEM INCLUDES
#include "PythonQt.h"
#include <QObject>
#include <QGraphicsSceneDragDropEvent>


class cedar::proc::gui::CoPYObject : public QObject
{

#define STEP_ASSERT(expr, step) if (!(expr)) { std::string info = "Step not found: " + step; CEDAR_THROW(cedar::aux::ExceptionBase, info); }
#define CLASS_ASSERT(classId) std::vector<std::string> types;cedar::proc::ElementManagerSingleton::getInstance()->getFactoryManager()->listTypes(types); if(!(std::find(types.begin(), types.end(), classId.toStdString()) != types.end())) {CEDAR_THROW(cedar::aux::ExceptionBase, "Class " + classId.toStdString() + " not found");}
#define GROUP_ASSERT(expr, group) if (!(expr)) { std::string info = "Group not found: " + group; CEDAR_THROW(cedar::aux::ExceptionBase, info); }
Q_OBJECT
public:
  CoPYObject(cedar::proc::gui::PythonQtConsole *pConsole)
  { pQtConsole = pConsole; }

  ~CoPYObject();

  cedar::proc::GroupPtr mpRootGroup;

  cedar::proc::gui::Scene *mpScene;

  cedar::proc::gui::PythonQtConsole *pQtConsole;

  /*!@brief setter for a reference to the scene*/
  void setScene(cedar::proc::gui::Scene *pScene)
  { mpScene = pScene; }

  void setGroup(cedar::proc::GroupPtr pGroup)
  { mpRootGroup = pGroup; }

public slots:
  /*!@brief setter for a constant reference to the Root, callable by cpp*/

  /*!@brief create an Element in the GroupScope*/
  QStringList createElem(const QString &classId, const int &x, const int &y, const QString &groupId, const int &amount);

  /*!@brief create a Group in Subgroup*/
  QStringList
  createGroupTemplate(const QString &classId, const int &x, const int &y, const QString &groupId, const int &amount);

  /*!@brief return all elements of a Group*/
  QStringList getElementsByGroup(const QString &groupId);

  /*!@brief connect two Steps at indexed DataSlots*/
  void
  connectSlots(const QString &source, const QVariant &sourceSlot, const QString &target, const QVariant &targetSlot,
               const bool &disconnect);

  //!@brief copy StepConfiguration from source to target
  void copyTo(const QString &fromStep, const QString &targetStep);

  //!@brief set specific parameter of given elementString
  void setParameter(const QString &elem, const QString &param, const QVariant &value);

  //!@brief add Object to ObjectListParameter of given step, paramName and type
  void addObjectList(const QString &step, const QString &param, const QString &type);

private:
  cedar::proc::StepPtr getStepByName(const std::string &elementIdentifier);

  cedar::proc::GroupPtr getGroupByName(const std::string &name);

  void jumpToStep(cedar::proc::Element *elementIdentifier);

  /*!@brief throw an error as well as in cedar and in python*/
  void throwError(std::string msg);
};

/*Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject)
Q_DECLARE_METATYPE(cedar::proc::gui::CoPYObject*)*/

class cedar::proc::gui::CoPYObjectWrapper : public QObject
{
Q_OBJECT
public:

  CoPYObjectWrapper()
  {};

  ~CoPYObjectWrapper();

public Q_SLOTS:

  /* Methods to call in Python eg. (py.setGroup), calling slots in CPP-CoPY-Object */

  void connect(const QVariant &first, const QVariant &second, const QVariant &firstSlot = 0,
               const QVariant &secondSlot = 0);

  void disconnect(const QVariant &first, const QVariant &second, const QVariant &firstSlot = 0,
                  const QVariant &secondSlot = 0);

  QStringList
  create(const QString &classId, const int &x, const int &y, const QString &groupId = "root", const int &amount = 1)
  {
    return emit createSig(classId, x, y, groupId, amount);
  }

  QStringList
  createGroupTemplate(const QString &templateId, const int &x, const int &y, const QString &groupId = "root",
                      const int &amount = 1)
  { return emit createGroupTemplateSig(templateId, x, y, groupId, amount); }

  void copyAllParameters(const QString &source, const QVariant &target);

  void setParameter(const QString &elem, const QString &param, const QVariant &value)
  {
    emit setParameterSig(elem, param, value);
  }

  void addObjectList(const QString &step, const QString &param, const QString &type)
  {
    emit addObjectListSig(step, param, type);
  }

  QStringList getElementsByGroup(const QString &group)
  {
    return emit getElementsByGroupSig(group);
  };
signals:

  QStringList createSig(const QString &classId, const int &x, const int &y, const QString &groupId, const int &amount);

  void setParameterSig(const QString &elem, const QString &param, const QVariant &value);

  void copySig(const QString &source, const QString &target);

  QStringList createGroupTemplateSig(const QString &templateId, const int &x, const int &y, const QString &groupId,
                                     const int &amount);

  void connectSig(const QString &src, const QVariant &firstSlot, const QString &tgt, const QVariant &targetSlot,
                  const bool &disconnect);

  void addObjectListSig(const QString &step, const QString &param, const QString &type);

  QStringList getElementsByGroupSig(const QString &group);
};

#endif //CEDAR_USE_COPY
#endif //CEDAR_PROC_GUI_COPY_OBJECT_H
