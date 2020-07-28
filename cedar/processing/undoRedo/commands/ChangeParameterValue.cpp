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

    File:        ChangeParameterValue.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2020 07 28

    Description: Source file for the class cedar::proc::undoRedo::commands::ChangeParameterValue.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/auxiliaries/NumericParameter.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

template <class T>
cedar::proc::undoRedo::commands::ChangeParameterValue<T>::ChangeParameterValue(cedar::aux::NumericParameter<T>* parameter, T oldValue, T newValue)
{
  this->mpParameter = parameter;
  this->mOldValue = oldValue;
  this->mNewValue = newValue;
}

template <class T>
cedar::proc::undoRedo::commands::ChangeParameterValue<T>::~ChangeParameterValue()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

template <class T>
void cedar::proc::undoRedo::commands::ChangeParameterValue<T>::undo(){
  this->mpParameter->setValue(this->mOldValue);
}

template <class T>
void cedar::proc::undoRedo::commands::ChangeParameterValue<T>::redo(){
  this->mpParameter->setValue(this->mNewValue);
}
