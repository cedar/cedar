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

    File:        UIntParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_UINT_PARAMETER_H
#define CEDAR_AUX_GUI_UINT_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/NumericParameter.h"
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QSpinBox>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// template specialization for QSpinBox
//----------------------------------------------------------------------------------------------------------------------
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      template<>
      inline void NumericWidgetPolicy<unsigned int, QSpinBox>::setPrecision(QSpinBox*, int)
      {
        // integal precision is ignored
      }

      template<>
      inline void NumericWidgetPolicy<unsigned int, QSpinBox>::setMinimum(QSpinBox* pWidget, const unsigned int& newValue)
      {
        unsigned int new_limit = newValue;
        if (new_limit > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
          cedar::aux::LogSingleton::getInstance()->warning
          (
            "Minimum for unsigned int parameter exceed qt's capabilities -- limiting to max int.",
            "void cedar::aux::gui::NumericWidgetPolicy<unsigned int, QSpinBox>::setMinimum(WidgetT*, const ValueT&)"
          );
          new_limit = std::numeric_limits<int>::max();
        }
        pWidget->setMinimum(static_cast<int>(new_limit));
      }

      template<>
      inline void NumericWidgetPolicy<unsigned int, QSpinBox>::setMaximum(QSpinBox* pWidget, const unsigned int& newValue)
      {
        unsigned int new_limit = newValue;
        if (new_limit > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
          cedar::aux::LogSingleton::getInstance()->warning
          (
            "Maximum for unsigned int parameter exceed qt's capabilities -- limiting to max int.",
            "void cedar::aux::gui::NumericWidgetPolicy<unsigned int, QSpinBox>::setMinimum(WidgetT*, const ValueT&)"
          );
          new_limit = std::numeric_limits<int>::max();
        }
        pWidget->setMaximum(static_cast<int>(new_limit));
      }
    }
  }
}

/*!@brief Widget for representing and manipulating a cedar::aux::UIntParameter.
 */
class cedar::aux::gui::UIntParameter : public cedar::aux::gui::NumericParameter<unsigned int, QSpinBox>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::NumericParameter<unsigned int, QSpinBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UIntParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~UIntParameter();

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

}; // class cedar::aux::gui::UIntParameter


#endif // CEDAR_AUX_GUI_UINT_PARAMETER_H
