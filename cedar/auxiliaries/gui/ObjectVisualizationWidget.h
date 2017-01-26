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

    File:        ObjectVisualizationWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 06

    Description: Header for the @em cedar::aux::gui::ObjectVisualizationWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_OBJECT_VISUALIZATION_WIDGET_H
#define CEDAR_AUX_GUI_OBJECT_VISUALIZATION_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ObjectVisualizationWidget.fwd.h"

// SYSTEM INCLUDES
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QWidget>

//!@brief A simple widget to monitor and change the state of a rigid body visualization
class cedar::aux::gui::ObjectVisualizationWidget
:
public QWidget
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
   */
  ObjectVisualizationWidget(const cedar::aux::gl::ObjectVisualizationPtr rigidBodyVisualization, QWidget* parent = 0);

  ~ObjectVisualizationWidget();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:

  /*!@brief set the pointer to the RigidBody
   *
   * @param    pObjectVisualization pointer to the new ObjectVisualization
   */
  void setObjectVisualization(cedar::aux::gl::ObjectVisualizationPtr pObjectVisualization);

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
  void setColor(double);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::aux::gl::ObjectVisualizationPtr mpObjectVisualization;
  QCheckBox *mpVisibleCheckBox;
  QCheckBox *mpWireFrameCheckBox;
  QCheckBox *mpLcfCheckBox;
  QLineEdit *mpAxisLengthLineEdit;
  QDoubleSpinBox *mpRedDoubleSpinBox;
  QDoubleSpinBox *mpGreenDoubleSpinBox;
  QDoubleSpinBox *mpBlueDoubleSpinBox;
};

#endif /* CEDAR_AUX_GUI_OBJECT_VISUALIZATION_WIDGET_H */
