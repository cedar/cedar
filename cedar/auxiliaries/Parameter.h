/*======================================================================================================================

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Parameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 01

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PARAMETER_H
#define CEDAR_PROC_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/IntrusivePtrBase.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief Base class for all parameters.
 *
 *        This is the base class for all parameters that can be registered in the cedar::aux::Configurable class.
 *
 * @see   @ref ParametersConcept for a description of the parameters concept.
 */
class cedar::aux::Parameter : public QObject, public cedar::aux::IntrusivePtrBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Configurable;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Parameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault = true);

  //!@brief Destructor
  virtual ~Parameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get the flag "read automatically"
  bool getReadAutomatically() const;
  //!@brief set the flag "read automatically"
  void setReadAutomatically(bool value);

  //!@brief return if this parameter has a default value
  bool getHasDefault() const;
  //!@brief set if this parameter has a default value
  void setHasDefault(bool value);

  //!@brief return if this parameter is constant
  bool isConstant() const;
  //!@brief set if this parameter is constant
  void setConstant(bool value);

  //!@brief set this parameter to a value, read from a configuration node
  virtual void readFromNode(const cedar::aux::ConfigurationNode& node) = 0;
  //!@brief write value to a configuration node
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const = 0;
  //!@brief set parameter to default
  virtual void makeDefault() = 0;

  //!@brief return flag if this parameter is hidden (for GUI)
  bool isHidden() const;
  //!@brief set flag if this parameter is hidden (for GUI)
  void setHidden(bool hide);

  //!@brief emit the value changed signal
  void emitChangedSignal();
  //!@brief emit the property changed signal
  void emitPropertyChangedSignal();

  //!@brief Returns the value of the @em mChanged flag.
  bool isChanged() const
  {
    return this->mChanged;
  }

  //!@brief Returns the name of the object.
  //!@return Name of the object.
  const std::string& getName() const;

  //!@brief Sets the name of the object to the given name.
  //!@param name New name of the object.
  void setName(const std::string& name);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief set the changed flag to a given state
  virtual void setChangedFlag(bool changed);

signals:
  //!@brief a signal that is emitted each time the value of a parameter changes
  void valueChanged();

  //!@brief a signal that is emitted each time a characteristic of this parameter changes (for example vector size)
  void propertyChanged();

  //!@brief Signal that is emitted when mChanged is changed.
  void changedFlagChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The owner of this parameter, i.e., the object using it.
  cedar::aux::Configurable *mpOwner;

  //! The name of the parameter.
  std::string mName;

  //! Whether the parameter should be read automatically. If not, the user has to read it by hand.
  bool mAutoRead;

  //! Whether a default value should be set
  bool mHasDefault;

  //! Whether this parameter can be changed during runtime.
  bool mConstant;

  //! Whether this parameter is hidden. This is relevant, e.g., for the gui.
  bool mIsHidden;

  //! Flag that indicates whether this parameter was changed since the last file reading.
  bool mChanged;
}; // class cedar::aux::Parameter


#endif // CEDAR_PROC_PARAMETER_H
