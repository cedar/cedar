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

    File:        NumericParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_NUMERIC_PARAMETER_H
#define CEDAR_AUX_GUI_NUMERIC_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>

// policy for widgets

template <typename ValueT, class WidgetT>
class cedar::aux::gui::NumericWidgetPolicy
{
  public:
    static void setValue(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setValue(newValue);
    }

    static ValueT getValue(WidgetT* pWidget)
    {
      return pWidget->value();
    }

    static void setMinimum(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setMinimum(newValue);
    }

    static void setMaximum(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setMaximum(newValue);
    }

    static void setPrecision(WidgetT* pWidget, int precision)
    {
      pWidget->setDecimals(precision);
    }

    static WidgetT* create(QWidget *pParent = NULL)
    {
      return new WidgetT(pParent);
    }
};

/*!@brief Base class for numeric parameter widgets.
 */
template <typename ValueT, class WidgetType>
class cedar::aux::gui::NumericParameter : public cedar::aux::gui::Parameter,
                                          cedar::aux::gui::NumericWidgetPolicy<ValueT, WidgetType>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef ValueT ValueType;
  typedef cedar::aux::NumericParameter<ValueType> ValueParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ValueParameter);
  typedef cedar::aux::gui::NumericWidgetPolicy<ValueT, WidgetType> WidgetPolicy;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  NumericParameter(QWidget *pParent = NULL)
  :
  Parameter(pParent)
  {
    this->setLayout(new QHBoxLayout());
    this->mpWidget = WidgetPolicy::create();
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->mpWidget);
    WidgetPolicy::setPrecision(this->mpWidget, 4);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void parameterChanged()
  {
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    parameter->lockForRead();
    WidgetPolicy::setValue(this->mpWidget, parameter->getValue());
    parameter->unlock();

    this->propertiesChanged();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*! Call this method whenver the value of the widget changes
   *
   * @remarks This is a workaround for qt's inability to moc templated classes.
   */
  void widgetValueChanged()
  {
    ValueType value = WidgetPolicy::getValue(this->mpWidget);

    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    parameter->setValue(value, true);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void propertiesChanged()
  {
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    parameter->lockForRead();
    this->mpWidget->setDisabled(parameter->isConstant());
    WidgetPolicy::setMinimum(this->mpWidget, parameter->getMinimum());
    WidgetPolicy::setMaximum(this->mpWidget, parameter->getMaximum());
    parameter->unlock();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  WidgetType* mpWidget;
private:
  // none yet

}; // class cedar::aux::gui::NumericParameter

#endif // CEDAR_AUX_GUI_NUMERIC_PARAMETER_H

