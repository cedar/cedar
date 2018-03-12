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
    Date:        2011 07 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_DOUBLE_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_DOUBLE_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericVectorParameter.h"
#include <cedar/auxiliaries/gui/IgnoreLocaleDoubleSpinBox.h>
// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/DoubleVectorParameter.fwd.h"
// SYSTEM INCLUDES


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
      inline bool cedar::aux::gui::VectorParameterAbstraction<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::connectValueChange
                  (
                    cedar::aux::gui::Parameter* pParameter,
                    cedar::aux::gui::IgnoreLocaleDoubleSpinBox* pWidget
                  )
      {
        return QObject::connect(pWidget, SIGNAL(valueChanged(double)), pParameter, SLOT(widgetValueChanged(double)));
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline double cedar::aux::gui::VectorParameterAbstraction<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::getValue(cedar::aux::gui::IgnoreLocaleDoubleSpinBox* pWidget)
      {
        return pWidget->value();
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void cedar::aux::gui::VectorParameterAbstraction<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::setValue
                  (
                    cedar::aux::gui::IgnoreLocaleDoubleSpinBox* pWidget,
                    const double& value
                  )
      {
        pWidget->setValue(value);
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline cedar::aux::gui::IgnoreLocaleDoubleSpinBox* cedar::aux::gui::VectorParameterAbstraction<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox>::create(unsigned int index)
      {
        auto p_widget = new cedar::aux::gui::IgnoreLocaleDoubleSpinBox();
        p_widget->setToolTip(QString("Set value for entry number %1.").arg(index));
        p_widget->setDecimals(4);
        return p_widget;
      }
      //!@endcond
    }
  }
}

/*!@brief A widget for manipulating cedar::aux::DoubleVectorParameters.
 */
class cedar::aux::gui::DoubleVectorParameter : public cedar::aux::gui::NumericVectorParameter
                                                      <
                                                        double,
                                                        cedar::aux::gui::IgnoreLocaleDoubleSpinBox
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
  typedef cedar::aux::gui::NumericVectorParameter<double, cedar::aux::gui::IgnoreLocaleDoubleSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DoubleVectorParameter(QWidget *pParent = NULL);

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
  void widgetValueChanged(double);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::DoubleVectorParameter

#endif // CEDAR_AUX_GUI_DOUBLE_VECTOR_PARAMETER_H
