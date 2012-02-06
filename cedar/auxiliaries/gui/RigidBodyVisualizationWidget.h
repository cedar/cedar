/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        RigidBodyVisualizationWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 06

    Description: Header for the @em cedar::aux::gui::RigidBodyVisualizationWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_RIGID_BODY_VISUALIZATION_WIDGET_H
#define CEDAR_AUX_GUI_RIGID_BODY_VISUALIZATION_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gl/Object.h"

// SYSTEM INCLUDES
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

//!@brief A simple widget to monitor and change the state of a rigid body visualization
//!@todo I removed the CEDAR_DEV_LIB_EXPORT here, check if this still runs on Windows.
//class CEDAR_DEV_LIB_EXPORT cedar::dev::robot::gui::RigidBodyVisualizationWidget
class cedar::aux::gui::RigidBodyVisualizationWidget
:
public QWidget,
public cedar::aux::ConfigurationInterface
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  Q_OBJECT

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------

public:
  /*!@brief Constructor without configuration
   *
   *@param rigidBodyVisualization pointer to a rigid body visualization object
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  RigidBodyVisualizationWidget(const cedar::aux::gl::ObjectPtr &rigidBodyVisualization, QWidget* parent = 0);

  ~RigidBodyVisualizationWidget();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:

  /*!@brief set the pointer to the RigidBody
   *
   * @param    pRigidBody pointer to the new RigidBody
   */
  void setRigidBodyVisualization(cedar::aux::gl::ObjectPtr pRigidBodyVisualization);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

protected:

  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------

private:

  void initWindow();

public slots:

  //!@brief read data from the rigid body and display them
  void update();

private slots:

  void setVisibilityState(int state);
  void setWireFrameState(int state);
  void setLcfState(int state);
  void setAxisLength();

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::aux::gl::ObjectPtr mpRigidBodyVisualization;
  QCheckBox* mpVisibleCheckBox;
  QCheckBox* mpWireFrameCheckBox;
  QCheckBox* mpLcfCheckBox;
  QLineEdit* mpAxisLengthLineEdit;
};

#endif /* CEDAR_AUX_GUI_RIGID_BODY_VISUALIZATION_WIDGET_H */
