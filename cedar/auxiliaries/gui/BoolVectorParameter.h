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

    File:        BoolVectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 18

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_BOOL_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_BOOL_VECTOR_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/VectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/BoolVectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <QCheckBox>

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
      inline bool cedar::aux::gui::VectorParameterAbstraction<bool, QCheckBox>::connectValueChange
                  (
                    cedar::aux::gui::Parameter* pParameter,
                    QCheckBox* pWidget
                  )
      {
        return QObject::connect(pWidget, SIGNAL(stateChanged(int)), pParameter, SLOT(widgetValueChanged(int)));
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline bool cedar::aux::gui::VectorParameterAbstraction<bool, QCheckBox>::getValue(QCheckBox* pWidget)
      {
        return pWidget->checkState() == Qt::Checked;
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void cedar::aux::gui::VectorParameterAbstraction<bool, QCheckBox>::setValue
                  (
                    QCheckBox* pWidget,
                    const bool& value
                  )
      {
        if (value)
        {
          pWidget->setCheckState(Qt::Checked);
        }
        else
        {
          pWidget->setCheckState(Qt::Unchecked);
        }
      }
      //!@endcond
    }
  }
}

/*!@brief A widget for displaying parameters that contain vectors of bools.
 */
class cedar::aux::gui::BoolVectorParameter : public cedar::aux::gui::VectorParameter<bool, QCheckBox>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::gui::VectorParameter<bool, QCheckBox> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  BoolVectorParameter(QWidget* pParent = NULL);

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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::BoolVectorParameter

#endif // CEDAR_AUX_GUI_BOOL_VECTOR_PARAMETER_H

