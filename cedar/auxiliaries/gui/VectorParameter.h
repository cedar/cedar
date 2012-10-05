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
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/VectorParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <vector>

template <typename ValueT, class WidgetT>
class cedar::aux::gui::VectorParameterAbstraction
{
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  static WidgetT* create(unsigned int index)
  {
    WidgetT* p_widget = new WidgetT();
    p_widget->setToolTip(QString("Set value for entry number %1.").arg(index));
    return p_widget;
  }

  static bool connectValueChange(cedar::aux::gui::Parameter* /* pParameter */, WidgetT* /* pWidget */)
  {
    // empty default implementation -- use template specialization to implement for specific classes
    return false;
  }

  static ValueT getValue(WidgetT*)
  {
    // empty as default implementation -- use template specialization to implement for specific classes
  }

  static void setValue(WidgetT*, const ValueT&)
  {
    // empty as default implementation -- use template specialization to implement for specific classes
  }

  static void applyProperties(WidgetT* pWidget, boost::intrusive_ptr<cedar::aux::VectorParameter<ValueT> > parameter)
  {
    pWidget->setDisabled(parameter->isConstant());
  }
};

/*!@brief A generic base class for gui representations of cedar::aux::VectorParameters.
 *
 * @todo describe more.
 *
 * @todo Use this class for the other vector parameters as well; introduce a numeric version of it
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
  typedef ValueT ValueType;
  typedef WidgetT WidgetType;
  typedef cedar::aux::VectorParameter<ValueT> Parameter;
  typedef Abstraction WidgetAbstraction;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(Parameter);


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

    this->parameter()->set(index, WidgetAbstraction::getValue(this->mWidgets[index]), true);
  }


  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void parameterChanged()
  {
    this->recreateWidgets();
  }

  void propertiesChanged()
  {
    this->recreateWidgets();
  }

  void valueChanged()
  {
    ParameterPtr parameter = this->parameter();

    CEDAR_DEBUG_ASSERT(parameter->size() == this->mWidgets.size());

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
      WidgetAbstraction::setValue(this->mWidgets[i], values[i]);
    }
  }

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
      WidgetAbstraction::applyProperties(this->mWidgets[i], parameter);
      WidgetAbstraction::setValue(this->mWidgets[i], parameter->at(i));
      WidgetAbstraction::connectValueChange(this, this->mWidgets[i]);
    }
    parameter->unlock();

    emit heightChanged();
  }

  inline ParameterPtr parameter()
  {
    return cedar::aux::asserted_pointer_cast<Parameter>(this->getParameter());
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<WidgetType*> mWidgets;

}; // class cedar::aux::gui::VectorParameter

#endif // CEDAR_AUX_GUI_VECTOR_PARAMETER_H

