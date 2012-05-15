/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        NetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Sun 06 Nov 2011

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NET_WRITER_SINK_H
#define CEDAR_NET_WRITER_SINK_H

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/processing/sinks/namespace.h"

// PROJECT INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/net/Writer.h"

// SYSTEM INCLUDES

/*!@brief A step which sends matrices over the network (needs YARP)  */
class cedar::proc::sinks::NetWriter : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NetWriter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void onStart();
  void onStop();


public slots:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::DataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

private:
  //!@brief the writer object (RAII)

  boost::shared_ptr< cedar::aux::net::Writer< cedar::aux::MatData::DataType > > mWriter;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:

}; // class cedar::proc::sinks::NetWriter

#endif // CEDAR_USE_YARP

#endif // CEDAR_PROC_STEPS_STATIC_GAIN_H