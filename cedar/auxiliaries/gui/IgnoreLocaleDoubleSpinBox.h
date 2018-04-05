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

    File:        IgnoreLocaleDoubleSpinBox.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 03 08

    Description: Header file for the class cedar::aux::gui::IgnoreLocaleDoubleSpinBox.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_IGNORE_LOCALE_DOUBLE_SPIN_BOX_H
#define CEDAR_AUX_GUI_IGNORE_LOCALE_DOUBLE_SPIN_BOX_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/IgnoreLocaleDoubleSpinBox.fwd.h"

// SYSTEM INCLUDES
#include <QDoubleSpinBox>
#include <QKeyEvent>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::aux::gui::IgnoreLocaleDoubleSpinBox: public QDoubleSpinBox
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IgnoreLocaleDoubleSpinBox(QWidget *parent = 0);

  //!@brief Destructor
  virtual ~IgnoreLocaleDoubleSpinBox();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
  double valueFromText(const QString &text) const override;

  void keyPressEvent(QKeyEvent *event) override;

public slots:
  void updatePrecision(double newPrecision);

public :
  signals:
  void inputPrecisionChanged(double newPrecision) const;
  //-- ------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  double getLastDefinedDigit(std::string textInput) const;


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  double mLastDefinedDigit;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::IgnoreLocaleDoubleSpinBox

#endif // CEDAR_AUX_GUI_IGNORE_LOCALE_DOUBLE_SPIN_BOX_H

