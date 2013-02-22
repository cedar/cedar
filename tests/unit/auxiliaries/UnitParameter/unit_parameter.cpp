/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 21

    Description: This test checks the functionality of the UnitParameter.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/LengthParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"
#include "cedar/auxiliaries/VelocityParameter.h"
#include "cedar/auxiliaries/PlaneAngleParameter.h"
#include "cedar/auxiliaries/FrequencyParameter.h"
#include "cedar/auxiliaries/AngularVelocityParameter.h"
#include "cedar/auxiliaries/AccelerationParameter.h"

// SYSTEM INCLUDES
#include <boost/filesystem.hpp>


class TestConfigurable : public cedar::aux::Configurable
{
public:
  TestConfigurable()
  :
  mLength
  (
    new cedar::aux::LengthParameter
    (
      this,
      "length",
      0 * boost::units::si::meters
    )
  ),
  mTime
  (
    new cedar::aux::TimeParameter
    (
      this,
      "time",
      0 * boost::units::si::seconds
    )
  ),
  mVelocity
  (
    new cedar::aux::VelocityParameter
    (
      this,
      "velocity",
      1 * boost::units::si::meters / boost::units::si::second
    )
  ),
  mPlaneAngle
  (
    new cedar::aux::PlaneAngleParameter
    (
      this,
      "plane angle",
      1 * boost::units::si::radian
    )
  ),
  mFrequency
  (
    new cedar::aux::FrequencyParameter
    (
      this,
      "frequency",
      1 * boost::units::si::hertz
    )
  ),
  mAngularVelocity
  (
    new cedar::aux::AngularVelocityParameter
    (
      this,
      "angular velocity",
      1 * boost::units::si::radian_per_second
    )
  ),
  mAcceleration
  (
    new cedar::aux::AccelerationParameter
    (
      this,
      "acceleration",
      1.0 * boost::units::si::meters_per_second_squared
    )
  )
  {
  }

  cedar::aux::LengthParameterPtr mLength;
  cedar::aux::TimeParameterPtr mTime;
  cedar::aux::VelocityParameterPtr mVelocity;
  cedar::aux::PlaneAngleParameterPtr mPlaneAngle;
  cedar::aux::FrequencyParameterPtr mFrequency;
  cedar::aux::AngularVelocityParameterPtr mAngularVelocity;
  cedar::aux::AccelerationParameterPtr mAcceleration;
};

CEDAR_GENERATE_POINTER_TYPES(TestConfigurable);


template <typename T>
int check(const boost::units::quantity<T>& expected, boost::intrusive_ptr<cedar::aux::UnitParameterTemplate<T> > param)
{
  if (param->getValue() != expected)
  {
    std::cout << "  <<ERROR>>: Wrong quantity/unit was read for parameter \"" << param->getName()
              << "\": expected " << expected
              << ", read: " << param->getValue() << std::endl;
    return 1;
  }
  else
  {
    std::cout << "  Parameter \"" << param->getName() << "\" was read correctly ("
              << param->getValue() << " == " << expected << ")." << std::endl;
    return 0;
  }
}

int test_reading
    (
      const std::string& fileName,
      const boost::units::quantity<boost::units::si::length>& expectedLength,
      const boost::units::quantity<boost::units::si::time>& expectedTime,
      const boost::units::quantity<boost::units::si::velocity>& expectedVelocity,
      const boost::units::quantity<boost::units::si::plane_angle>& expectedPlaneAngle,
      const boost::units::quantity<boost::units::si::frequency>& expectedFrequency,
      const boost::units::quantity<boost::units::si::angular_velocity>& expectedAngularVelocity,
      const boost::units::quantity<boost::units::si::acceleration>& expectedAcceleration
    )
{
  int errors = 0;
  std::cout << "Testing reading of file \"" << fileName << "\"." << std::endl;

  {
    TestConfigurablePtr conf(new TestConfigurable());

    conf->readJson(fileName);

    errors += check(expectedLength, conf->mLength);
    errors += check(expectedTime, conf->mTime);
    errors += check(expectedVelocity, conf->mVelocity);
    errors += check(expectedPlaneAngle, conf->mPlaneAngle);
    errors += check(expectedFrequency, conf->mFrequency);
    errors += check(expectedAngularVelocity, conf->mAngularVelocity);
    errors += check(expectedAcceleration, conf->mAcceleration);
  }

  std::cout << "Reading of file \"" << fileName << "\" finished with " << errors << " error(s)." << std::endl;
  return errors;
}

int test_writing
    (
      const std::string& fileName,
      const boost::units::quantity<boost::units::si::length>& expectedLength,
      const boost::units::quantity<boost::units::si::time>& expectedTime,
      const boost::units::quantity<boost::units::si::velocity>& expectedVelocity,
      const boost::units::quantity<boost::units::si::plane_angle>& expectedPlaneAngle,
      const boost::units::quantity<boost::units::si::frequency>& expectedFrequency,
      const boost::units::quantity<boost::units::si::angular_velocity>& expectedAngularVelocity,
      const boost::units::quantity<boost::units::si::acceleration>& expectedAcceleration
    )
{
  int errors = 0;
  std::cout << "Testing writing of file \"" << fileName << "\"." << std::endl;

  TestConfigurablePtr conf(new TestConfigurable());

  conf->mLength->setValue(expectedLength);
  conf->mTime->setValue(expectedTime);
  conf->mVelocity->setValue(expectedVelocity);
  conf->mPlaneAngle->setValue(expectedPlaneAngle);
  conf->mFrequency->setValue(expectedFrequency);
  conf->mAngularVelocity->setValue(expectedAngularVelocity);
  conf->mAcceleration->setValue(expectedAcceleration);

  conf->writeJson(fileName);

  errors += test_reading
            (
              fileName,
              conf->mLength->getValue(),
              conf->mTime->getValue(),
              conf->mVelocity->getValue(),
              conf->mPlaneAngle->getValue(),
              conf->mFrequency->getValue(),
              conf->mAngularVelocity->getValue(),
              conf->mAcceleration->getValue()
            );

  boost::filesystem::remove(fileName.c_str());

  std::cout << "Writing of file \"" << fileName << "\" finished with " << errors << " error(s)." << std::endl;
  return errors;
}

int main(int, char**)
{
  int errors = 0;

  errors += test_reading
            (
              "test1-read.json",
              1.0 * boost::units::si::meters,
              5.0 * boost::units::si::seconds,
              1.0 * boost::units::si::meters / boost::units::si::seconds,
              2.0 * boost::units::si::radians,
              20.0 * boost::units::si::hertz,
              5.0 * boost::units::si::radian_per_second,
              7.0 * boost::units::si::meters_per_second_squared
            );
  errors += test_reading
            (
              "test2-read.json",
              1.0 * boost::units::si::meters,
              5.0 * boost::units::si::seconds,
              1.0 * boost::units::si::meters / boost::units::si::seconds,
              2.0 * boost::units::si::radians,
              20.0 * boost::units::si::hertz,
              5.0 * boost::units::si::radian_per_second,
              7.0 * boost::units::si::meters_per_second_squared
            );
  errors += test_reading
            (
              "test3-read.json",
              1.0 * boost::units::si::meters,
              5.0 * boost::units::si::seconds,
              1.0 * boost::units::si::meters / boost::units::si::seconds,
              2.0 * boost::units::si::radians,
              20.0 * boost::units::si::hertz,
              5.0 * boost::units::si::radian_per_second,
              7.0 * boost::units::si::meters_per_second_squared
            );
  errors += test_writing
            (
              "test1-write.json",
              3.0 * boost::units::si::meters,
              2.0 * boost::units::si::seconds,
              -5.0 * boost::units::si::meters / boost::units::si::seconds,
              8.0 * boost::units::si::radians,
              8.0 * boost::units::si::hertz,
              -6.0 * boost::units::si::radian_per_second,
              -8.0 * boost::units::si::meters_per_second_squared
            );

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
