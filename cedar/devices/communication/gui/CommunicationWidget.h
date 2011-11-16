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

    File:        CommunicationWidget.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for testing the class Communication.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_COM_GUI_COMMUNICATION_WIDGET_H
#define CEDAR_DEV_COM_GUI_COMMUNICATION_WIDGET_H

// LOCAL INCLUDES

#include "cedar/devices/communication/Communication.h"
#ifdef DEBUG
  #include "cedar/devices/debug/communication/gui/ui_CommunicationWidget.h"
#else
  #include "cedar/devices/communication/gui/ui_CommunicationWidget.h"
#endif
#include "cedar/devices/communication/gui/namespace.h"

// PROJECT INCLUDES

#include "cedar/auxiliaries/gui/BaseWidget.h"

// SYSTEM INCLUDES

#include <Qt>
#include <QString>

/*!@brief Graphical User Interface for testing the class Communication.
 *
 * Type the string to be sent into 'command' and click 'send'. The answer of the device is then displayed in 'answer'.
 */
class cedar::dev::com::gui::CommunicationWidget : public cedar::aux::gui::BaseWidget, private Ui_CommunicationWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

private:

  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Constructs the GUI.
  //!@param peCommunication Pointer to the communication-class to be tested.
  CommunicationWidget(cedar::dev::com::Communication *peCommunication);

  //!@brief Destructs the GUI.
  virtual ~CommunicationWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public slots:

  /*!@brief Sends the string.
   *
   * This function calls 'send' of the linked communication class with the string typed into 'command' as parameter.
   */
  void send();

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

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  // pointer to the Communication-class.
  cedar::dev::com::Communication *mpeCommunication;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  // none yet

}; // class cedar::dev::com::gui::CommunicationWidget

#endif // CEDAR_DEV_COM_GUI_COMMUNICATION_WIDGET_H

