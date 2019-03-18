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

    File:        LocalWriter.h

    Maintainer:  Raul Grieben
    Email:       raul.grienen@ini.ruhr-uni-bochum.de
    Date:        Thu 20 Dez 2018

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_LOCAL_WRITER_SINK_H
#define CEDAR_LOCAL_WRITER_SINK_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sinks/LocalWriter.fwd.h"

// SYSTEM INCLUDES

/*!@brief A step which sends matrices locally  */
class cedar::proc::sinks::LocalWriter : public cedar::proc::Step
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
  LocalWriter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void onStart();
  void onStop();


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
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);
  //!@brief Resets the step and recreates the yarp connection.
  void reset();
  void connect();
  void validatePortName(const std::string& portName) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;
  std::string oldName = "";

public:
    static std::map<std::string,cedar::aux::ConstMatDataPtr>& mData()
    {
      static std::map<std::string,cedar::aux::ConstMatDataPtr> mData;
      return mData;
    }

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::StringParameterPtr _mPort;

}; // class cedar::proc::sinks::LocalWriter

#endif // CEDAR_LOCAL_WRITER_SINK_H
