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

    File:        NumericVectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_NUMERIC_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_NUMERIC_VECTOR_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/VectorParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/NumericVectorParameter.fwd.h"

// SYSTEM INCLUDES

/*!@brief Class that abstracts the interface of widgets used for numeric parameter vectors.
 *
 *        The behavior for different widget and value types can be overridden by template specialization.
 */
template <typename ValueT, class WidgetT>
class cedar::aux::gui::NumericVectorParameterAbstraction
:
public cedar::aux::gui::VectorParameterAbstraction<ValueT, WidgetT>
{
  public:
    /*!@brief Applies properties of a given parameter to the given widget.
     *
     * @param pWidget   Widget to which to apply the parameters.
     * @param parameter Parameter whose properties to apply.
     * @param index     Index of the widget.
     */
    static void applyProperties
                (
                  WidgetT* pWidget,
                  boost::intrusive_ptr<cedar::aux::VectorParameter<ValueT> > parameter,
                  size_t index
                )
    {
      typedef cedar::aux::NumericVectorParameter<ValueT> NumericVector;
      auto numeric_parameter = cedar::aux::asserted_pointer_cast<NumericVector>(parameter);
      pWidget->setDisabled(parameter->isConstantAt(index));
      bool signals_blocked = pWidget->blockSignals(true);
      setMinimum(pWidget, numeric_parameter->getMinimum());
      setMaximum(pWidget, numeric_parameter->getMaximum());
      pWidget->setAlignment(Qt::AlignRight);
      pWidget->blockSignals(signals_blocked);
      pWidget->setSingleStep( numeric_parameter->getSingleStep() );
    }

    /*!@brief Sets the minimum allowed value in the widget.
     *
     * @param pWidget Widget for which to set the value.
     * @param limit   The minimum value for the widget.
     */
    static void setMinimum(WidgetT* pWidget, const ValueT& limit)
    {
      pWidget->setMinimum(limit);
    }

    /*!@brief Sets the maximum allowed value in the widget.
     *
     * @param pWidget Widget for which to set the value.
     * @param limit   The maximum value for the widget.
     */
    static void setMaximum(WidgetT* pWidget, const ValueT& limit)
    {
      pWidget->setMaximum(limit);
    }
};

/*!@brief Generic class for widgets that display numeric vector parameters.
 */
template <typename ValueT, class WidgetT>
class cedar::aux::gui::NumericVectorParameter : public cedar::aux::gui::VectorParameter
                                                       <
                                                         ValueT,
                                                         WidgetT,
                                                         cedar::aux::gui::NumericVectorParameterAbstraction
                                                         <
                                                           ValueT, WidgetT
                                                         >
                                                       >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::VectorParameter
          <
            ValueT,
            WidgetT,
            cedar::aux::gui::NumericVectorParameterAbstraction
            <
              ValueT, WidgetT
            >
          >
          Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NumericVectorParameter(QWidget* pParent = NULL)
  :
  Base(pParent)
  {
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
}; // class cedar::aux::gui::NumericVectorParameter

#endif // CEDAR_AUX_GUI_NUMERIC_VECTOR_PARAMETER_H

