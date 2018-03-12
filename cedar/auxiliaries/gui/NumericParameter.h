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
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/casts.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/NumericParameter.fwd.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>

// policy for widgets

/*!@brief Class that abstracts the interface for numeric widgets.
 *
 *        Where widgets differ from the default interface, it can be overridden via template specialization.
 */
template <typename ValueT, class WidgetT>
class cedar::aux::gui::NumericWidgetPolicy
{
  public:
    /*!@brief Sets the value in the widget.
     *
     * @param pWidget  Widget whose value is to be set.
     * @param newValue Value to set.
     */
    static void setValue(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setValue(newValue);
    }

    /*!@brief Gets the value from the widget.
     *
     * @param pWidget  Widget whose value to get.
     */
    static ValueT getValue(WidgetT* pWidget)
    {
      return pWidget->value();
    }

    /*!@brief Sets the minimal value allowed in the widget.
     *
     * @param pWidget  Widget whose minimum is to be set.
     * @param newValue Minimum to be set.
     */
    static void setMinimum(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setMinimum(newValue);
    }

    /*!@brief Sets the maximal value allowed in the widget.
     *
     * @param pWidget  Widget whose maximum is to be set.
     * @param newValue Maximum to be set.
     */
    static void setMaximum(WidgetT* pWidget, const ValueT& newValue)
    {
      pWidget->setMaximum(newValue);
    }

    /*!@brief Sets the precision of the value, i.e., the number of digits after the decimal point.
     *
     * @param pWidget   Widget for which the precision is to be set.
     * @param precision New precision.
     *
     * @remarks This only affects widgets that display floating-point values.
     */
    static void setPrecision(WidgetT* pWidget, int precision)
    {
    }

    static void setSingleStep(WidgetT* pWidget, double singleStep)
    {
      pWidget->setSingleStep(singleStep);
    }

    static void manuallyChangedValue(WidgetT*, const ValueT&, const ValueT&)
    {
    }

    /*!@brief Creates a new widget of the given type.
     *
     * @param pParent The parent of the widget.
     * @return The created widget.
     */
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
  //! Type of the values stored in this widget.
  typedef ValueT ValueType;

  //! The parameter for the type.
  typedef cedar::aux::NumericParameter<ValueType> ValueParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ValueParameter);
  //!@endcond

  //! Policy used for interfacing with the widgets.
  typedef cedar::aux::gui::NumericWidgetPolicy<ValueT, WidgetType> WidgetPolicy;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  NumericParameter(QWidget *pParent = NULL)
  :
  Parameter(pParent)
  {
    this->setLayout(new QHBoxLayout());
    this->mpWidget = WidgetPolicy::create();
    this->mpWidget->setAlignment(Qt::AlignRight);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->addWidget(this->mpWidget);
    WidgetPolicy::setPrecision(this->mpWidget, 4);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Reacts to a change of the parameter displayed by this widget.
  void parameterChanged()
  {
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    // first, apply widget min/max values
    this->propertiesChanged();

    parameter->lockForRead();
    ValueT value = parameter->getValue();
    parameter->unlock();

    WidgetPolicy::setValue(this->mpWidget, value);
  }

  //! Override from the parent class.
  void setReadOnly(bool readOnly)
  {
    CEDAR_DEBUG_ASSERT(this->mpWidget != nullptr);
    this->mpWidget->setReadOnly(readOnly);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*! Call this method whenever the value of the widget changes
   *
   * @remarks This is a workaround for qt's inability to moc templated classes.
   */
  void widgetValueChanged()
  {
    ValueType value = WidgetPolicy::getValue(this->mpWidget);
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    ValueType oldvalue = parameter->getValue();

    parameter->setValue(value, true);

    WidgetPolicy::manuallyChangedValue(this->mpWidget, value, oldvalue);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Reacts to a change in the properties of the parameter and applies them to the widget.
  void propertiesChanged()
  {
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    bool blocked = this->mpWidget->blockSignals(true);

    parameter->lockForRead();
    this->mpWidget->setDisabled(parameter->isConstant());
    WidgetPolicy::setMinimum(this->mpWidget, parameter->getMinimum());
    WidgetPolicy::setMaximum(this->mpWidget, parameter->getMaximum());
    WidgetPolicy::setSingleStep(this->mpWidget, parameter->getSingleStep());
    parameter->unlock();

    this->mpWidget->blockSignals(blocked);
  }

  //! Reacts to a change in the value of the parameter and applies them to the widget.
  void valueChanged()
  {
    ValueParameterPtr parameter = cedar::aux::asserted_pointer_cast<ValueParameter>(this->getParameter());

    bool blocked = this->mpWidget->blockSignals(true);
    parameter->lockForRead();
    if (parameter->getValue() != WidgetPolicy::getValue(this->mpWidget))
    {
      WidgetPolicy::setValue(this->mpWidget, parameter->getValue());
    }
    parameter->unlock();
    this->mpWidget->blockSignals(blocked);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Widget used for displaying the parameter.
  WidgetType* mpWidget;
private:
  // none yet

}; // class cedar::aux::gui::NumericParameter

#endif // CEDAR_AUX_GUI_NUMERIC_PARAMETER_H

