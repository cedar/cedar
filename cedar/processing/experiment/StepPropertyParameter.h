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
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/StepPropertyParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief A parameter to set a property of a certain step
 *
 *          The property could either be a parameter or an output or a buffer.
 *          The property type should be defined when creating an instance of this parameter.
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
  //! enum defining the type of the property represented by this parameter
  enum PropertyType
  {
    //! type parameter
    PARAMETER_VALUE,
    //! type parameter
    PARAMETER,
    //! type output slot
    OUTPUT,
    //! type buffer slot
    BUFFER
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StepPropertyParameter(cedar::aux::Configurable *pOwner, const std::string& name);

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

   /*!@brief Sets the property name of the step.
    *               It should always be checked if the property name is still available
    */
   void setParameterPath(const std::string& poperty);

   //!@brief Sets the step.
   void setStep(cedar::proc::ConnectablePtr step);

   //!@brief Sets the path of the element.
   void setElementPath(const std::string& step);

   //! Returns the path of the currently selected element.
   std::string getElementPath() const;

   //! Returns the path of the currently selected element.
   const std::string& getParameterPath() const;

   //!@brief Returns the step name
   cedar::proc::ConnectablePtr getStep() const;

   //!@brief Sets the type of the property. Could be either PARAMETER, OUTPUT or BUFFER
   void setType(cedar::proc::experiment::StepPropertyParameter::PropertyType type);

   //!@brief Returns the type of the property
   cedar::proc::experiment::StepPropertyParameter::PropertyType getType() const;

   /*!@brief Returns the desired data of the step.
    *
    *           Should only be called if the type is BUFFER or OUTPUT
    */
   cedar::aux::ConstDataPtr getData() const;

   //! returns a list of data names for given data role
   std::vector<std::string> getListOfData(cedar::proc::DataRole::Id role) const;

   /*!@brief Returns the ParameterPtr of the property
    *
    *           Should only be called if the type is PARAMETER
    */
   cedar::aux::ParameterPtr getParameter() const;


   /*!@brief Returns a deep copy of the ParameterPtr
    *
    *           Should only be called if the type is PARAMETER
    */
   cedar::aux::ParameterPtr getParameterCopy() const;

   //!@brief Sets the types of parameters that should be allowed for this parameter
   void allowType(const std::string& type);

   //!@brief Checks if a type is allowed for this parameter
   bool isAllowType(const std::string& type);

   //!@brief Unset a types of parameters that should be allowed for this parameter
   void disallowType(const std::string& type);

   //!@brief Returns all types that are allowed
   const std::vector<std::string>& getAllowedTypes();

   //! Returns true if a parameter is selected
   bool isParameterSelected() const;

   std::vector<std::string> getListOfParameters();

   //! Checks the validity of the parameter.
   bool checkValidity(std::string& errors) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
//!@brief If the property is changed, the parameter copy will be updated
  void updateParameterCopy();

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
  //!@brief The path of the element.
  std::string mElementPath;

  //! Weak pointer to the element.
  cedar::proc::ConnectableWeakPtr mElement;

  //!@brief The property
  std::string mParameterPath;

  //!@brief The type
  cedar::proc::experiment::StepPropertyParameter::PropertyType mType;

  //!@brief The copy of the step parameter
  cedar::aux::ParameterPtr mParameterCopy;

  //!@brief The allowed types
  std::vector<std::string> allowedTypes;

}; // class cedar::proc::experiment::StepPropertyParameter

#endif // CEDAR_PROC_EXPERIMENT_STEP_PROPERTY_PARAMETER_H

