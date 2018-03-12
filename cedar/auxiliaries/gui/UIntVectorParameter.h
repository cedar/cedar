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

    File:        UIntVectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_UINT_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_UINT_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/UIntVectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <QSpinBox>
#include <limits.h>

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
      inline bool VectorParameterAbstraction<unsigned int, QSpinBox>::connectValueChange
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
      inline unsigned int VectorParameterAbstraction<unsigned int, QSpinBox>::getValue(QSpinBox* pWidget)
      {
        return pWidget->value();
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void VectorParameterAbstraction<unsigned int, QSpinBox>::setValue
                  (
                    QSpinBox* pWidget,
                    const unsigned int& value
                  )
      {
        pWidget->setValue(value);
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void NumericVectorParameterAbstraction<unsigned int, QSpinBox>::setMinimum
                  (
                    QSpinBox* pWidget,
                    const unsigned int& limit
                  )
      {
        unsigned int minimum = limit;
        if (minimum > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
          minimum = static_cast<unsigned int>(std::numeric_limits<int>::max());
        }
        pWidget->setMinimum(minimum);
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void NumericVectorParameterAbstraction<unsigned int, QSpinBox>::setMaximum
                  (
                    QSpinBox* pWidget,
                    const unsigned int& limit
                  )
      {
        unsigned int maximum = limit;
        if (maximum > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
          maximum = static_cast<unsigned int>(std::numeric_limits<int>::max());
        }
        pWidget->setMaximum(maximum);
      }
      //!@endcond
    }
  }
}


/*!@brief Widget for manipulating vectors of unsigned integer values.
 */
class cedar::aux::gui::UIntVectorParameter : public cedar::aux::gui::NumericVectorParameter
                                                    <
                                                      unsigned int,
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
  typedef cedar::aux::gui::NumericVectorParameter<unsigned int, QSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UIntVectorParameter(QWidget *pParent = NULL);

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

}; // class cedar::aux::gui::UIntVectorParameter

#endif // CEDAR_AUX_GUI_UINT_VECTOR_PARAMETER_H
