/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        StepPropertyParameter.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 13

    Description: Header file for the class cedar::proc::experiment::StepPropertyParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_STEP_PROPERTY_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_STEP_PROPERTY_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/StepPropertyParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::experiment::StepPropertyParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  enum PropertyType
  {
    PARAMETER,
    OUTPUT,
    BUFFER
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StepPropertyParameter(cedar::aux::Configurable *pOwner, const std::string&);

  //!@brief Destructor
  virtual ~StepPropertyParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief set this parameter to a value, read from a configuration node
   void readFromNode(const cedar::aux::ConfigurationNode& node);

   //!@brief write value to a configuration node
   void writeToNode(cedar::aux::ConfigurationNode& root) const;

   //!@brief set parameter to default
   void makeDefault();

   //! @brief If possible, copies the value from another parameter into this parameter.
   void copyValueFrom(cedar::aux::ConstParameterPtr other);

   //!@brief Checks if this parameter can copy its value from the given one.
   bool canCopyFrom(cedar::aux::ConstParameterPtr other) const;

   void setProperty(const std::string& poperty);

   const std::string& getProperty() const;

   void setStep(const std::string& step);

   const std::string& getStep() const;

   void setType(cedar::proc::experiment::StepPropertyParameter::PropertyType type);

   cedar::proc::experiment::StepPropertyParameter::PropertyType getType() const;

   cedar::aux::DataPtr getData() const;

   cedar::aux::ParameterPtr getParameter() const;

   cedar::aux::ParameterPtr getParameterCopy() const;

   void allowType(const std::string& type);

   bool isAllowType(const std::string& type);

   void disallowType(const std::string& type);

   const std::vector<std::string>& getAllowedTypes();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void updatePropertyCopy();

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
  std::string mStep;
  std::string mProperty;
  cedar::proc::experiment::StepPropertyParameter::PropertyType mType;
  cedar::aux::ParameterPtr mParameterCopy;
  std::vector<std::string> allowedTypes;
  //cedar::aux::DataPtr mDataCopy;

}; // class cedar::proc::experiment::StepPropertyParameter

#endif // CEDAR_PROC_EXPERIMENT_STEP_PROPERTY_PARAMETER_H

