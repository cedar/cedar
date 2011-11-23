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

    File:        ProjectionMappingParameter.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H
#define CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QComboBox>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::ProjectionMappingParameter : public cedar::proc::gui::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMappingParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~ProjectionMappingParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  void parameterPointerChanged();

  void currentIndexChanged(int index);

  //!@brief Handles changes in the displayed parameter's properties, e.g., a resizing of the vector.
  void propertyChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<QComboBox*> mComboBoxes;
  std::vector<bool> mOutputDimensionUsed;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ProjectionMappingParameter

#endif // CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H

