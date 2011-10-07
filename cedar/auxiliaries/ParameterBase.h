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

    File:        ParameterBase.h

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

#ifndef CEDAR_PROC_PARAMETER_BASE_H
#define CEDAR_PROC_PARAMETER_BASE_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QObject>

// Functions for boost intrusive pointer.
extern void intrusive_ptr_add_ref(cedar::aux::ParameterBase *pObject);
extern void intrusive_ptr_release(cedar::aux::ParameterBase *pObject);


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::ParameterBase : public QObject, public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend void ::intrusive_ptr_add_ref(cedar::aux::ParameterBase *pObject);
  friend void ::intrusive_ptr_release(cedar::aux::ParameterBase *pObject);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ParameterBase(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault = true);

  //!@brief Destructor
  virtual ~ParameterBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  bool getReadAutomatically() const;
  void setReadAutomatically(bool value);

  bool getHasDefault() const;
  void setHasDefault(bool value);

  bool isConstant() const;
  void setConstant(bool value);

  virtual void setTo(const cedar::aux::ConfigurationNode& node) = 0;
  virtual void putTo(cedar::aux::ConfigurationNode& root) = 0;
  virtual void makeDefault() = 0;

  bool isHidden() const;
  void setHidden(bool hide);

  void emitChangedSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  signals:
    void valueChanged();
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

  //! Whether the parameter should be read automatically. If not, the user has to read it by hand.
  bool mAutoRead;

  //! Whether a default value should be set
  bool mHasDefault;

  //! Whether this parameter can be changed during runtime.
  bool mConstant;

  //! Whether this parameter is hidden. This is relevant, e.g., for the gui.
  bool mIsHidden;

  //! Reference counter for boost intrusive pointer.
  unsigned int mReferenceCount;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ParameterBase

#endif // CEDAR_PROC_PARAMETER_BASE_H

