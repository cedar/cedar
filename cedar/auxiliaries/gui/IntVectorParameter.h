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

    File:        IntVectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 12

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_INT_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_INT_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/IntVectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <QSpinBox>


//----------------------------------------------------------------------------------------------------------------------
// template specializations
//----------------------------------------------------------------------------------------------------------------------
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      //!@cond SKIPPED_DOCUMENTATION
      template <>
      inline bool cedar::aux::gui::VectorParameterAbstraction<int, QSpinBox>::connectValueChange
                  (
                    cedar::aux::gui::Parameter* pParameter,
                    QSpinBox* pWidget
                  )
      {
        return QObject::connect(pWidget, SIGNAL(valueChanged(int)), pParameter, SLOT(widgetValueChanged(int)));
      }
      //!@endcond


      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline int cedar::aux::gui::VectorParameterAbstraction<int, QSpinBox>::getValue(QSpinBox* pWidget)
      {
        return pWidget->value();
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void cedar::aux::gui::VectorParameterAbstraction<int, QSpinBox>::setValue
                  (
                    QSpinBox* pWidget,
                    const int& value
                  )
      {
        pWidget->setValue(value);
      }
      //!@endcond
    }
  }
}

/*!@brief Widget for manipulating vectors of integer values.
 */
class cedar::aux::gui::IntVectorParameter : public cedar::aux::gui::NumericVectorParameter
                                                   <
                                                     int,
                                                     QSpinBox
                                                   >
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::NumericVectorParameter<int, QSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IntVectorParameter(QWidget *pParent = NULL);

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
private slots:
  void widgetValueChanged(int);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::IntVectorParameter

#endif // CEDAR_AUX_GUI_INT_VECTOR_PARAMETER_H
