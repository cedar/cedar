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

    File:        NeuralFieldView.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Header file for the class cedar::dyn::gui::NeuralFieldView.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_GUI_NEURAL_FIELD_VIEW_H
#define CEDAR_DYN_GUI_NEURAL_FIELD_VIEW_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/DefaultConnectableIconView.h"

// FORWARD DECLARATIONS
#include "cedar/dynamics/gui/NeuralFieldView.fwd.h"

// SYSTEM INCLUDES
#include <QObject>

#include <QReadWriteLock>


/*!@brief Responsible for changing the icon of DNFs.
 */
class cedar::dyn::gui::NeuralFieldView : public QObject, public cedar::proc::gui::DefaultConnectableIconView
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (compiler generated)

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! reacts to changes in the connectable
  void connectableChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private slots:
  void updateIconDimensionality();
  void updateActivityIcon(bool isActive);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dyn::gui::NeuralFieldView

#endif // CEDAR_DYN_GUI_NEURAL_FIELD_VIEW_H

