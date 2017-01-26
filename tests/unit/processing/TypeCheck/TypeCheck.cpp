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

    File:        ArchitectureIO.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 14

    Description: Unit test that first writes a processing architecture to a file and then attempts to load it again.

    Credits:

======================================================================================================================*/

#include "cedar/processing/typecheck/TypeCheck.h"
#include "cedar/processing/typecheck/DerivedFrom.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/Data.h"

class MatDerivative : public cedar::aux::MatData
{
};
CEDAR_GENERATE_POINTER_TYPES(MatDerivative);

class DataDerivative : public cedar::aux::Data
{
};
CEDAR_GENERATE_POINTER_TYPES(DataDerivative);

class TestStep : public cedar::proc::Step
{
  public:
    TestStep()
    {
      slot1 = this->declareInput("input1");

      slot1->setCheck(cedar::proc::typecheck::DerivedFrom<cedar::aux::MatData>());
    }

    cedar::proc::DataSlotPtr slot1;

  private:
    void compute(const cedar::proc::Arguments&)
    {
    }
};
CEDAR_GENERATE_POINTER_TYPES(TestStep);

int main(int, char**)
{
  unsigned int errors = 0;

  // empty -- the checks tested here don't use the slot at the moment
  cedar::proc::DataSlotPtr slot;
  MatDerivativePtr mat_deriv(new MatDerivative());
  DataDerivativePtr data_deriv(new DataDerivative());

  std::cout << "Checking some type check implementations." << std::endl << std::endl;

  {
    cedar::proc::typecheck::TypeCheckPtr
      derived_from(new cedar::proc::typecheck::DerivedFrom<cedar::aux::MatData>());
    std::string info;
    if (derived_from->check(slot, mat_deriv, info) != cedar::proc::DataSlot::VALIDITY_VALID)
    {
      std::cout << "ERROR: mat_deriv is not recognized by DerivedFrom<MatData>" << std::endl;
      ++errors;
    }

    if (derived_from->check(slot, data_deriv, info) != cedar::proc::DataSlot::VALIDITY_ERROR)
    {
      std::cout << "ERROR: data_deriv is recognized by DerivedFrom<MatData>" << std::endl;
      ++errors;
    }
  }

  std::cout << "Checking processing step implementation" << std::endl;
  {
    TestStepPtr test_step(new TestStep());

    cedar::proc::DataSlot::VALIDITY validity;

    validity = test_step->slot1->checkValidityOf(mat_deriv);
    if (validity != cedar::proc::DataSlot::VALIDITY_VALID)
    {
      std::cout << "ERROR: mat_deriv is not recognized by data slot." << std::endl;
      ++errors;
    }

    validity = test_step->slot1->checkValidityOf(data_deriv);
    if (validity != cedar::proc::DataSlot::VALIDITY_ERROR)
    {
      std::cout << "ERROR: data_deriv is recognized by data slot" << std::endl;
      ++errors;
    }
  }


  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
