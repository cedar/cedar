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

    File:        NetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 03 Nov 2011

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NET_READER_STEP_H
#define CEDAR_NET_READER_STEP_H

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/net/Reader.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/NetReader.fwd.h"

// SYSTEM INCLUDES

/*!@brief a step which reads a matrix over the network (needs YARP) */
class cedar::proc::sources::NetReader : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::net::Reader<cedar::aux::MatData::DataType> Reader;
  CEDAR_GENERATE_POINTER_TYPES(Reader);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NetReader();

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
  //!@brief the reader object
  cedar::aux::LockableMember<ReaderPtr> mReader;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::StringParameterPtr _mPort;

}; // class cedar::proc::sources::NetReader

#endif

#endif // CEDAR_PROC_STEPS_STATIC_GAIN_H

