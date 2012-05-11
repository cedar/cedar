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

    File:        NetReaderSource.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Thu 03 Nov 2011 04:47:26 PM CET

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_NETREADERSTEP_H
#define CEDAR_NETREADERSTEP_H

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/processing/steps/namespace.h"

// PROJECT INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/net/Reader.h"

// SYSTEM INCLUDES

namespace cedar {
  namespace proc {
    namespace steps {
      class NetReaderSource;
    }
  }
}

/*!@brief a step which reads a matrix over the network (needs YARP) */
class cedar::proc::steps::NetReaderSource : public cedar::proc::Step
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
  NetReaderSource();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);
  void onStop();
  void onStart();


public slots:

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
protected:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:
  //!@brief the reader object (RAII)
  cedar::aux::net::Reader< cedar::aux::MatData::DataType > *mpReader;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:

}; // class cedar::proc::steps::NetReaderSource

#endif

#endif // CEDAR_PROC_STEPS_STATIC_GAIN_H

