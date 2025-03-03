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

    File:        DoubleVectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 11 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef UNIT_TEST_AUX_GUI_NUMERIC_VECTOR_PARAMETER_DOUBLE_VECTOR_PARAMETER_H
#define UNIT_TEST_AUX_GUI_NUMERIC_VECTOR_PARAMETER_DOUBLE_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/processing/auxiliaries/gui/NumericVectorParameter.h"
#include "cedar/processing/auxiliaries/gui/DoubleVectorParameter.h"

/* A widget for unit testing cedar::aux::DoubleVectorParameters.
 */
class DoubleVectorParameter : public cedar::proc::aux::gui::DoubleVectorParameter
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
  DoubleVectorParameter(QWidget* pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool empty() const;
  unsigned int size() const;
  cedar::proc::aux::gui::IgnoreLocaleDoubleSpinBox* widgetAt(int i);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
//private slots:


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class DoubleVectorParameter

#endif // UNIT_TEST_AUX_GUI_NUMERIC_VECTOR_PARAMETER_DOUBLE_VECTOR_PARAMETER_H
