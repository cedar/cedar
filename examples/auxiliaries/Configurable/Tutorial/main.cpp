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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include <cedar/auxiliaries/Configurable.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/gui/PropertyPane.h>

// SYSTEM INCLUDES
#include <QApplication>
#include <iostream>

//!@cond SKIPPED_DOCUMENTATION
class MyConfigurableClass : public cedar::aux::Configurable
{
  private:
    // standard member declaration
    cedar::aux::DoubleParameterPtr mMyParameter;
  public:
    MyConfigurableClass()
    : // this starts the member initialization list
    // now let's initialize the member:
    mMyParameter
    (
      new cedar::aux::DoubleParameter
      (
        this, // this tells the parameter what configurable it belongs to
        "my parameter", // this is the name of the parameter; it is used to identify it in the configurtion file
                        // as well as the user interface
        1.0, // this is the default value for the parameter; if you leave this out, an exception is thrown when there
             // is no value specified for the parameter in a configuration file
        0.0, // the minimum value the parameter can have
        100.0 // the maximum value for the parameter
      )
    )
    {
    }

    inline double getMyParameter() const
    {
      return this->mMyParameter->getValue();
    }

    inline void setMyParameter(double newValue)
    {
      // note, that you have to call setValue because this also emits a signal notifying anyone interested of the change
      // this is also explained in later tutorials
      this->mMyParameter->setValue(newValue);
    }
};

// first, we generate the pointer types for our class; this saves us some typing
CEDAR_GENERATE_POINTER_TYPES(MyConfigurableClass); // include cedar/defines.h for this

class A : public cedar::aux::Configurable
{
  private:
    cedar::aux::DoubleParameterPtr mMyParameter;

  public:
    A()
    :
    mMyParameter(new cedar::aux::DoubleParameter(this, "AParameter", 1.0, 0.0, 100.0))
    {
    }
};
CEDAR_GENERATE_POINTER_TYPES(A);

class B : public cedar::aux::Configurable
{
  private:
    // note that this has to be a pointer
    APtr mAMember;
    // ...

  public:
    B()
    :
    mAMember(new A())
    {
      // this declares mAMember as a child
      this->addConfigurableChild("a_member", mAMember);
    }
};

class GroupDemo : public cedar::aux::Configurable
{
  private:
    cedar::aux::DoubleParameterPtr mAParameter;

  public:
    GroupDemo()
    {
      // note that initialization of parameters is no longer done in the initializer list

      // create configurable to represent the group
      cedar::aux::ConfigurablePtr group(new cedar::aux::Configurable());

      // add the group as a child
      this->addConfigurableChild("my group", group);

      // add parameters to the group
      mAParameter = cedar::aux::DoubleParameterPtr
                    (
                      new cedar::aux::DoubleParameter
                      (
                        group.get(), // this adds the parameter to the group; note, that the .get() is required but
                                     // doesn't violate the smartpointiness of the parameter because
                                     // cedar::aux::DoubleParameterPtr is defined as an intrusive_ptr
                        "a parameter", // rest looks the same as introduced above
                        1.0,
                        0.0,
                        100.0
                      )
                    );
    }
};

CEDAR_GENERATE_POINTER_TYPES(GroupDemo);
//!@endcond

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // assume this is your main method or a similar code
  MyConfigurableClassPtr my_object(new MyConfigurableClass());

  // let's set some value for our parameter:
  my_object->setMyParameter(5.0);

  // now we can write this to a file:
  my_object->writeJson("my_config_file.json");

  // we can now create a second object and read the same file:
  MyConfigurableClassPtr second_object(new MyConfigurableClass());
  second_object->readJson("my_config_file.json");

  std::cout << "Read parameter value: " << second_object->getMyParameter() << std::endl;

  GroupDemoPtr my_configurable_object(new GroupDemo());

  cedar::aux::gui::PropertyPane *p_pane = new cedar::aux::gui::PropertyPane();
  p_pane->show(); // makes qt show the widget
  p_pane->display(my_configurable_object);

  my_configurable_object->writeJson("group_demo.json");

  app.exec();

  return 0;
}

