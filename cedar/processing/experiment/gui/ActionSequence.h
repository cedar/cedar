/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ActionSequence.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Header file for the class cedar::proc::experiment::gui::ActionSequence.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_ACTION_SEQUENCE_H
#define CEDAR_PROC_EXPERIMENT_GUI_ACTION_SEQUENCE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/ActionSequence.fwd.h"
#include "cedar/processing/experiment/ActionSequence.fwd.h"
#include "cedar/processing/gui/ExperimentDialog.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QBoxLayout>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::experiment::gui::ActionSequence : public QWidget
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ActionSequence(cedar::proc::experiment::ActionSequencePtr sequence, cedar::proc::gui::ExperimentDialog* pParent);

  //!@brief Destructor
  virtual ~ActionSequence();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
void remove();
  // none yet

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
  QVBoxLayout* mLayout;
  QVBoxLayout* mActions;
  QHBoxLayout* mCondition;
  cedar::proc::experiment::ActionSequencePtr mSequence;
  cedar::proc::gui::ExperimentDialog* mpParent;

}; // class cedar::proc::experiment::gui::ActionSequence

#endif // CEDAR_PROC_EXPERIMENT_GUI_ACTION_SEQUENCE_H

