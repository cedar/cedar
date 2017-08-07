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

    File:        IntParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 03 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_INT_PARAMETER_H
#define CEDAR_AUX_GUI_INT_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/IntParameter.fwd.h"

// SYSTEM INCLUDES
#include <QSpinBox>

//----------------------------------------------------------------------------------------------------------------------
// template specialization for QSpinBox
//----------------------------------------------------------------------------------------------------------------------
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void NumericWidgetPolicy<int, QSpinBox>::setSingleStep(QSpinBox* widget, double step)
      {
        if (step < 1.0)
          step= 1.0;

        widget->setSingleStep( step );
      }

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void NumericWidgetPolicy<int, QSpinBox>::setPrecision(QSpinBox*, int)
      {
        // integal precision is ignored
      }
      //!@endcond
    }
  }
}

/*!@brief Widget for representing and manipulating a cedar::aux::IntParameter.
 */
class cedar::aux::gui::IntParameter : public cedar::aux::gui::NumericParameter<int, QSpinBox>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::NumericParameter<int, QSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IntParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~IntParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void parameterChanged();

private slots:
  /*!@brief Reacts to a change of the parameter's value.
   */
  void valueChanged(int value);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::IntParameter

#endif // CEDAR_AUX_GUI_INT_PARAMETER_H
