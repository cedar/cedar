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

    File:        VectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_VECTOR_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/VectorParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/casts.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/VectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <vector>

/*!@brief A class that abstracts access to the widgets for a vector parameter.
 *
 *        Behavior in this class is overridden in template specializations.
 */
template <typename ValueT, class WidgetT>
class cedar::aux::gui::VectorParameterAbstraction
{
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Creates a widget for the given index.
   *
   * @param index Index of the vector that the widget represents.
   * @return The created widget.
   */
  static WidgetT* create(unsigned int index)
  {
    WidgetT* p_widget = new WidgetT();
    p_widget->setToolTip(QString("Set value for entry number %1.").arg(index));
    return p_widget;
  }

  /*!@brief Connects the widget's value changed signal to the appropriate receiver.
   *
   * @return True if the connection was established correctly.
   */
  static bool connectValueChange(cedar::aux::gui::Parameter* /* pParameter */, WidgetT* /* pWidget */)
  {
    // empty default implementation -- use template specialization to implement for specific classes
    return false;
  }

  /*!@brief Returns the value of the given widget.
   */
  static ValueT getValue(WidgetT*)
  {
    // empty as default implementation -- use template specialization to implement for specific classes
  }

  /*!@brief Sets the given value in the given widget.
   */
  static void setValue(WidgetT*, const ValueT&)
  {
    // empty as default implementation -- use template specialization to implement for specific classes
  }

  /*!@brief Applies the parameter's properties (constancy etc.) to the widget.
   *
   * @param pWidget   Widget to which to apply the properties
   * @param parameter Parameter whose properties are to be applied.
   * @param index The index of the underlying vector parameter entry
   */
  static void applyProperties
              (
                WidgetT* pWidget,
                boost::intrusive_ptr<cedar::aux::VectorParameter<ValueT> > parameter,
                size_t index
              )
  {
    pWidget->setDisabled(parameter->isConstantAt(index));
  }
};

/*!@brief A generic base class for gui representations of cedar::aux::VectorParameters.
 */
template
<
  typename ValueT,
  class WidgetT,
  class Abstraction = cedar::aux::gui::VectorParameterAbstraction<ValueT, WidgetT>
>
class cedar::aux::gui::VectorParameter : public cedar::aux::gui::Parameter,
                                         public Abstraction
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of value stored in the vector parameter.
  typedef ValueT ValueType;

  //! Widget type used for displaying individual values.
  typedef WidgetT WidgetType;

  //! Type of the parameter.
  typedef cedar::aux::VectorParameter<ValueT> Parameter;

  //! Widget abstraction used.
  typedef Abstraction WidgetAbstraction;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(Parameter);
  //!@endcond


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VectorParameter(QWidget* pParent = NULL)
  :
  cedar::aux::gui::Parameter(pParent)
  {
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(0, 0, 0, 0);
    p_layout->setSpacing(0);
    this->setLayout(p_layout);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! react to value change in one of the sub widgets
  virtual void widgetValueChanged(WidgetT* pWidget)
  {
    size_t index = this->mWidgets.size();

    for (size_t i = 0; i < this->mWidgets.size(); ++i)
    {
      if (this->mWidgets[i] == pWidget)
      {
        index = i;
        break;
      }
    }

    // if this fails, the widget was not found in the list of the ones managed by this gui parameter
    CEDAR_DEBUG_ASSERT(index < this->mWidgets.size());

    CEDAR_DEBUG_ASSERT(this->mWidgets.size() == this->parameter()->size());

    if (WidgetAbstraction::getValue(this->mWidgets[index]) != this->parameter()->at(index))
    {
      this->parameter()->setValue(index, WidgetAbstraction::getValue(this->mWidgets[index]), true);
    }
  }


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Slot that is called whenever the parameter displayed by the the widget is changed.
  void parameterChanged()
  {
    this->recreateWidgets();
  }

  //!@brief Slot that is called whenever the properties of the parameter displayed by the the widget is changed.
  void propertiesChanged()
  {
    this->recreateWidgets();
  }

  //!@brief Slot that is called whenever the value of the parameter displayed by the the widget is changed.
  void valueChanged()
  {
    ParameterPtr parameter = this->parameter();

    CEDAR_DEBUG_NON_CRITICAL_ASSERT(parameter->size() == this->mWidgets.size());

    std::vector<ValueType> values;

    parameter->lockForRead();
    values.resize(this->mWidgets.size());
    for (size_t i = 0; i < this->mWidgets.size(); ++i)
    {
      values[i] = parameter->at(i);
    }
    parameter->unlock();
    for (size_t i = 0; i < values.size(); ++i)
    {
      if (WidgetAbstraction::getValue(this->mWidgets[i]) != values[i])
      {
        WidgetAbstraction::setValue(this->mWidgets[i], values[i]);
      }
    }
  }

  //!@brief Recreates the widgets for the individual parameter entries.
  void recreateWidgets()
  {
    ParameterPtr parameter = this->parameter();

    // Don't discard old widgets, reuse them!
    if (this->mWidgets.size() > parameter->size())
    {
      for (size_t i = parameter->size(); i < this->mWidgets.size(); ++i)
      {
        delete this->mWidgets[i];
      }
    }

    parameter->lockForRead();
    size_t num_widgets = parameter->size();
    this->mWidgets.resize(num_widgets, NULL);

    for (size_t i = 0; i < this->mWidgets.size(); ++i)
    {
      if (this->mWidgets[i] == NULL)
      {
        this->mWidgets[i] = WidgetAbstraction::create(i);
        this->layout()->addWidget(this->mWidgets[i]);
      }
      WidgetAbstraction::applyProperties(this->mWidgets[i], parameter, i);
      WidgetAbstraction::setValue(this->mWidgets[i], parameter->at(i));
      WidgetAbstraction::connectValueChange(this, this->mWidgets[i]);


    }
    parameter->unlock();

    // apply the proper tab order to all widgets
    for (size_t i = 1; i < this->mWidgets.size(); ++i)
    {
      // tell qt that widgets[i-1] precedes widgets[i]
      QWidget::setTabOrder(this->mWidgets[i - 1], this->mWidgets[i]);
    }

    emit heightChanged();
  }

  //!@brief Returns the parameter displayed by this widget as the proper type.
  inline ParameterPtr parameter()
  {
    return cedar::aux::asserted_pointer_cast<Parameter>(this->getParameter());
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Vector of the widgets used for displaying the parameter entries. (placed in protected to allow unit testing)
  std::vector<WidgetType*> mWidgets;
private:
  // none yet

}; // class cedar::aux::gui::VectorParameter

#endif // CEDAR_AUX_GUI_VECTOR_PARAMETER_H

