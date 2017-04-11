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

    File:        KinematicChainWidget.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Header for the @em cedar::dev::KinematicChainWidget class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GUI_KINEMATIC_CHAIN_WIDGET_H
#define CEDAR_DEV_GUI_KINEMATIC_CHAIN_WIDGET_H

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/devices/gui/namespace.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/gui/KinematicChainCommandWidget.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QWidget>

/*!@brief A simple widget to access all the joints via GUI
 *
 * @todo This class does not conform to the usual template.
 */
class cedar::dev::gui::KinematicChainWidget
:
public QWidget
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  Q_OBJECT

  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------

public:
  /*!@brief Constructor
   *
   *@param kinematicChain pointer to a kinematic chain
   *@param parent parent parameter of QWidget
   *@param f WindowFlags for QWidget
   */
  KinematicChainWidget(cedar::dev::KinematicChainPtr kinematicChain, QWidget* parent = 0, Qt::WindowFlags f = 0);

  ~KinematicChainWidget();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:
  /*!@brief access the monitor sub-widget
   * @return pointer to the monitor widget
   */
  cedar::dev::gui::KinematicChainMonitorWidget* getMonitorWidget();

  /*!@brief access the command sub-widget
   * @return pointer to the command widget
   */
  cedar::dev::gui::KinematicChainCommandWidget* getCommandWidget();

  std::string getPath() const;

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

protected:

  // none yet

  //----------------------------------------------------------------------------
  // private methods
  //----------------------------------------------------------------------------

private:

  void init(cedar::dev::KinematicChainPtr kinematicChain);

  //----------------------------------------------------------------------------
  // members
  //----------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::gui::KinematicChainMonitorWidget* mpMonitorWidget;
  cedar::dev::gui::KinematicChainCommandWidget* mpCommandWidget;
  cedar::dev::KinematicChainPtr mpKinematicChain;
};

#endif // CEDAR_DEV_GUI_KINEMATIC_CHAIN_WIDGET_H
