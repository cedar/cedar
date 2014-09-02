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

    File:        Component.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Abstract component of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_COMPONENT_H
#define CEDAR_DEV_COMPONENT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/Channel.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Data.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <map>

/*!@brief Base class for components of robots.
 */
class cedar::dev::Component : public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::dev::ComponentSlot;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  enum DataType
  {
    MEASURED,
    COMMANDED
  };

private:
  struct DataSlot
  {
    cedar::aux::DataPtr mData;
    boost::function<void ()> mUpdateFunction;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Component();

  //!@brief Constructor taking an externally created channel
  Component(cedar::dev::ChannelPtr channel);

  //!@brief Destructor
  ~Component();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the channel associated with the component.
  inline cedar::dev::ChannelPtr getChannel() const
  {
    return mChannel;
  }

  inline void setChannel(cedar::dev::ChannelPtr channel)
  {
    this->mChannel = channel;
  }

  std::vector<std::string> getDataNames(cedar::dev::Component::DataType type) const;

  cedar::aux::DataPtr getCommandedData(const std::string& name) const;

  cedar::aux::DataPtr getMeasuredData(const std::string& name) const;

  void updateMeasuredValues();

  void updateCommandedValues();

  void updateValues(cedar::dev::Component::DataType type);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  inline cedar::dev::ComponentSlotWeakPtr getSlot()
  {
    return this->mSlot;
  }

  inline void setSlot(cedar::dev::ComponentSlotWeakPtr slot)
  {
    this->mSlot = slot;
  }
 
  void addCommandedData(const std::string& name, cedar::aux::DataPtr data, boost::function<void()> updateFun);

  void addMeasuredData(const std::string& name, cedar::aux::DataPtr data, boost::function<void()> updateFun);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void addData
  (
    cedar::dev::Component::DataType type,
    const std::string& name, cedar::aux::DataPtr data,
    boost::function<void()> updateFun
  );

  cedar::aux::DataPtr getData(cedar::dev::Component::DataType type, const std::string& name) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! channel of communication
  cedar::dev::ChannelPtr mChannel;
  cedar::dev::ComponentSlotWeakPtr mSlot;

  //! The data of the channel, i.e., measured and commanded values.
  std::map<cedar::dev::Component::DataType, std::map<std::string, DataSlot> > mData;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet
}; // class cedar::dev::Component


// Code for the component factory manager ==============================================================================

#include "cedar/auxiliaries/FactoryManager.h"


namespace cedar
{
  namespace dev
  {
    //!@brief The manager of all sigmoind instances
    typedef cedar::aux::FactoryManager<cedar::dev::ComponentPtr> ComponentManager;

    //!@brief The singleton object of the TransferFunctionFactory.
//    typedef cedar::aux::Singleton<cedar::dev::ComponentManager> ComponentManagerSingleton;
  }
}
CEDAR_DEV_SINGLETON(ComponentManager);

#endif // CEDAR_DEV_COMPONENT_H
