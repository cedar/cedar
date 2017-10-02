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

    File:        ParameterLink.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 18

    Description: Header file for the class cedar::aux::ParameterLink.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PARAMETER_LINK_H
#define CEDAR_AUX_PARAMETER_LINK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ParameterLink.fwd.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief Base for classes that link different parameters together.
 *
 *        A parameter link is an association between two or more parameters. The values of the associated parameters
 *        are then linked, meaning that when one value is changed, another one is changed as well.
 */
class cedar::aux::ParameterLink : public QObject, public cedar::aux::Configurable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ParameterLink();

  //!@brief Destructor
  virtual ~ParameterLink();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the parameters linked by this parameter link.
  void setLinkedParameters(cedar::aux::ParameterPtr source, cedar::aux::ParameterPtr target);

  //! Returns the source parameter.
  cedar::aux::ParameterPtr getSource() const;

  //! Returns the target parameter.
  cedar::aux::ParameterPtr getTarget() const;

  //! Sets the source of the link. Will link the parameters if a target is already set.
  void setSource(cedar::aux::ParameterPtr parameter);

  //! Sets the source of the link to a nullptr.
  void unsetSource();

  //! Sets the target of the link. Will link the parameters if a source is already set.
  void setTarget(cedar::aux::ParameterPtr parameter);

  //! Sets the target of the link to a nullptr.
  void unsetTarget();

  //! Checks if the two parameter can be linked to each other.
  bool canLink(cedar::aux::ParameterPtr source, cedar::aux::ParameterPtr target);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void callSourceChanged();

  void callTargetChanged();

  virtual void sourcePropertiesChanged();

  virtual void targetPropertiesChanged();

private:
  virtual void sourceChanged() = 0;

  virtual void targetChanged() = 0;

private:
  virtual bool checkIfLinkable(cedar::aux::ConstParameterPtr source, cedar::aux::ConstParameterPtr target) const = 0;

  void applyProperties(cedar::aux::ConstParameterPtr source, cedar::aux::ParameterPtr target);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ParameterPtr mSource;
  cedar::aux::ParameterPtr mTarget;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ParameterLink

#include "cedar/auxiliaries/FactoryManager.h"

namespace cedar
{
  namespace aux
  {
    //! a factory manager for parameter links
    typedef cedar::aux::FactoryManager<cedar::aux::ParameterLinkPtr> ParameterLinkFactoryManager;
  }
}

CEDAR_AUX_SINGLETON(ParameterLinkFactoryManager);

#endif // CEDAR_AUX_PARAMETER_LINK_H

