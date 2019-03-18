/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        LocalReader.h

    Maintainer:  Raul Grieben
    Email:       raul.grienen@ini.ruhr-uni-bochum.de
    Date:        Thu 20 Dez 20181

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_LOCAL_READER_STEP_H
#define CEDAR_LOCAL_READER_STEP_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/LocalReader.fwd.h"

// SYSTEM INCLUDES

/*!@brief a step which reads a matrix over the network (needs YARP) */
class cedar::proc::sources::LocalReader : public cedar::proc::Step
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
  LocalReader();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void onStop();
  void onStart();

  /*! Returns the name of the port.
   */
  inline const std::string& getPort() const
  {
    return this->_mPort->getValue();
  }

  /*! Sets the name of the port.
   */
  inline void setPort(const std::string& port)
  {
    this->_mPort->setValue(port);
  }

public slots:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Resets the step and recreates the yarp connection.
  void reset();

  //!@brief calls emitOutputPropertiesChangedSignal on all outputs
  //! needed because emitOutputPropertiesChanged() is protected
  void emitOutputPropertiesChangedSignalOnAction();

  void connect();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::StringParameterPtr _mPort;

}; // class cedar::proc::sources::LocalReader

#endif // CEDAR_LOCAL_READER_STEP_H

