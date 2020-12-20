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

    File:        Log.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description: Header for the \em cedar::aux::Log class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_COPY_H
#define CEDAR_PROC_GUI_COPY_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/Copy.fwd.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

#ifndef Q_MOC_RUN
#include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN
#include <QWidget>
#include <vector>
#include <string>
#include <stdexcept>
#include <QApplication>
#include <QHBoxLayout>
#include <QTextEdit>
#include <map>


//!@brief A class for Scripting with Python to copy/create things.
class cedar::proc::gui::Copy : public QWidget
{
  Q_OBJECT;

  void setObjectName(QString qString)
  {
  }

public:

  Copy();
  //!@brief The standard constructor.
  Copy(QWidget *pParent = NULL);


  //! Destructor.
  ~Copy();


public slots:
  void reset();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:


protected:

};

#endif // CEDAR_PROC_GUI_COPY_H

