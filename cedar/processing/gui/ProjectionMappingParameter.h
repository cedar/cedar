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

    File:        ProjectionMappingParameter.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 16

    Description: GUI elements for the projection mapping parameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H
#define CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ProjectionMappingParameter.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <vector>


/*!@brief GUI elements for the projection mapping parameter.
 *
 * The mapping of each input dimension is visualized by a combo box, from which the index of the corresponding
 * output dimension can be selected. For compressing projections (i.e., input dim > output dim), the excess input
 * dimensions can be configured to be dropped (by selecting "drop"). If the mapping configured by the user is
 * erroneous in the given context, the combo boxes will be colored red and the associated projection will be in
 * an exception state (i.e., the architecture cannot be run). The combo boxes will turn green as soon as the
 * configuration is corrected.
 */
class cedar::proc::gui::ProjectionMappingParameter : public cedar::aux::gui::Parameter
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
  //!@brief Handles changes in the parameter.
  void parameterPointerChanged();

  //!@brief Handles the change of a value in one of the combo boxes.
  void currentIndexChanged(int index);

  //!@brief Handles changes in the displayed parameter's properties, e.g., a resizing of the vector.
  void valueChanged();

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
  /*!@brief Update the validity of the mapping based on the current setting in the GUI
   * and the context (i.e., the connected steps).
   */
  void updateValidity();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! the combo boxes making up the GUI
  std::vector<QComboBox*> mComboBoxes;

}; // class cedar::proc::gui::ProjectionMappingParameter

#endif // CEDAR_PROC_GUI_PROJECTION_MAPPING_PARAMETER_H
