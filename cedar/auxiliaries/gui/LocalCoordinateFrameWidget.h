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

    File:        LocalCoordinateFrameWidget.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 02 01

    Description: Header for the @em cedar::aux::gui::LocalCoordinateFrameWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_LOCAL_COORDINATE_FRAME_WIDGET_H
#define CEDAR_AUX_GUI_LOCAL_COORDINATE_FRAME_WIDGET_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/LocalCoordinateFrameWidget.fwd.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QWidget>

//!@brief A simple widget to monitor and change the state of a rigid body
class cedar::aux::gui::LocalCoordinateFrameWidget
:
public QWidget,
public cedar::aux::Configurable
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
   *@param localCoordinateFrame pointer to the LocalCoordinateFrame being controlled by the widget
   *@param parent parent parameter of QWidget
   */
  LocalCoordinateFrameWidget(const cedar::aux::LocalCoordinateFramePtr localCoordinateFrame, QWidget* parent = 0);

  ~LocalCoordinateFrameWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // Qt events
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief implementation of handling routine for Qt timer events
   *
   * @param    event
   */
  void timerEvent(QTimerEvent* event);

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:

  /*!@brief set the pointer to the LocalCoordinateFrame
   *
   * @param    pLocalCoordinateFrame pointer to the new LocalCoordinateFrame
   */
  void setLocalCoordinateFrame(cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame);

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

protected:
  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------

private slots:
  void decimalsChanged();
  
private:
  inline unsigned int getDecimals() const
  {
    return this->_mDecimals->getValue();
  }

  void initWindow();

public slots:

  //!@brief read data from the rigid body and display them
  void update();

private slots:

  void rotateXPos();
  void rotateXNeg();
  void rotateYPos();
  void rotateYNeg();
  void rotateZPos();
  void rotateZNeg();
  void positionChanged(double);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  static const int mUpdateInterval = 100;
  cedar::aux::LocalCoordinateFramePtr mpLocalCoordinateFrame;
  QGridLayout *mpGridLayout;
  QDoubleSpinBox *mpTranslationXSpinBox;
  QDoubleSpinBox *mpTranslationYSpinBox;
  QDoubleSpinBox *mpTranslationZSpinBox;
  int mDecimals;
  int mRotationInterval;
  double mSingleTranslationStep;
  double mSingleRotationStep;
  double mXMin;
  double mXMax;
  double mYMin;
  double mYMax;
  double mZMin;
  double mZMax;

  //----------------------------------------------------------------------------
  // parameters
  //----------------------------------------------------------------------------
private:
  cedar::aux::UIntParameterPtr _mDecimals;
};

#endif /* CEDAR_AUX_GUI_LOCAL_COORDINATE_FRAME_WIDGET_H */
