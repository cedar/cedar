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

    File:        Element.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_ELEMENT_H
#define CEDAR_PROC_ELEMENT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/Trigger.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
  #include <boost/signals2.hpp>
#endif
#include <set>
#include <string>


/*!@brief Base class for Elements in a processing architecture.
 *
 *        Each element is described by a name that uniquely identifies it within a processing module.
 */
class cedar::proc::Element
:
virtual public cedar::aux::NamedConfigurable,
public boost::enable_shared_from_this<cedar::proc::Element>
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Element();

  //!@brief The destructor.
  virtual ~Element();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief sets the group at which this element is registered
  void setGroup(cedar::proc::GroupPtr group);

  //!@brief get the group at which this element is registered
  cedar::proc::GroupPtr getGroup();

  //!@brief get the group at which this element is registered as const
  cedar::proc::ConstGroupPtr getGroup() const;

  //!@brief copy a configuration from another instance of the same class (type check included)
  void copyFrom(cedar::aux::ConstConfigurablePtr src);

  //!@brief copy a configuration to another instance of the same class (type check included)
  void copyTo(cedar::aux::ConfigurablePtr target) const;

  //!@brief updates the trigger chains of this element
  virtual void updateTriggerChains(std::set<cedar::proc::Trigger*>& visited);

  //! Returns the full path of the element.
  std::string getFullPath() const;

  //! call the reset function of this element
  virtual void callReset();


  //! will be called after the architecture has been loaded
  virtual void afterArchitectureLoaded();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! connects to group changed signal
  inline boost::signals2::connection connectToGroupChanged(boost::function<void()> slot)
  {
    return this->mGroupChanged.connect(slot);
  }

  /*! Checks if the given string is a valid name for an element. Valid strings, among other things, cannot contain dots.
   */
  void validateNameStringFormat(const std::string& newName) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  virtual void validateName(const std::string& newName) const;

  //! resets this element
  virtual void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! the network this element is registered at
  cedar::proc::GroupWeakPtr mRegisteredAt;

  //! Signal that is emitted whenever the element's network changes
  boost::signals2::signal<void()> mGroupChanged;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:
  // none yet

}; // class cedar::proc::Element

#endif // CEDAR_PROC_ELEMENT_H

