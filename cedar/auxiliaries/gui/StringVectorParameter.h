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

    File:        StringVectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 05 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_STRING_VECTOR_PARAMETER_H
#define CEDAR_AUX_GUI_STRING_VECTOR_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/VectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/StringVectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <QLineEdit>
#include <string>

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
      inline bool VectorParameterAbstraction<std::string, QLineEdit>::connectValueChange
                  (
                    cedar::aux::gui::Parameter* pParameter,
                    QLineEdit* pWidget
                  )
      {
        return QObject::connect(pWidget, SIGNAL(textChanged(const QString&)), pParameter, SLOT(widgetValueChanged(const QString&)));
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline std::string VectorParameterAbstraction<std::string, QLineEdit>::getValue(QLineEdit* pWidget)
      {
        return pWidget->text().toStdString();
      }
      //!@endcond

      //!@cond SKIPPED_DOCUMENTATION
      template<>
      inline void VectorParameterAbstraction<std::string, QLineEdit>::setValue
                  (
                    QLineEdit* pWidget,
                    const std::string& value
                  )
      {
        pWidget->setText(QString::fromStdString(value));
      }
      //!@endcond
    }
  }
}


/*!@brief Widget for manipulating vectors of unsigned integer values.
 */
class cedar::aux::gui::StringVectorParameter : public cedar::aux::gui::VectorParameter
                                                    <
                                                      std::string,
                                                      QLineEdit
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
  typedef cedar::aux::gui::VectorParameter<std::string, QLineEdit> Base;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StringVectorParameter(QWidget *pParent = NULL);

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
  void widgetValueChanged(const QString&);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::StringVectorParameter

#endif // CEDAR_AUX_GUI_STRING_VECTOR_PARAMETER_H
