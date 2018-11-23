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

    File:        Channel.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Communication channel for a component or device (e.g., serial communication).

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_CHANNEL_H
#define CEDAR_DEV_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/DeclarationManagerTemplate.h"

// FORWARD DECLARATIONS
#include "cedar/devices/Channel.fwd.h"


/*!@brief Communication channel for a component or device (e.g., serial communication).
 *
 * @todo describe more.
 */
class cedar::dev::Channel : public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& stream, const cedar::dev::Channel& channel);

  friend std::ostream& operator<<(std::ostream& stream, cedar::dev::ConstChannelPtr channel);

  friend std::ostream& operator<<(std::ostream& stream, cedar::dev::ChannelPtr channel);

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Channel();

  //!@brief Destructor
  virtual ~Channel();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Opens the channel.
  void open();

  //!@brief Closes the channel.
  void close();

  //!@brief Returns whether the channel is open.
  virtual bool isOpen() const = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Executed before the code in @em openHook.
  virtual void preOpenHook(){};

  //!@brief This method implements opening of the channel.
  virtual void openHook() = 0;

  //!@brief Executed after the code in @em openHook.
  virtual void postOpenHook(){};

  //!@brief Executed before the code in @em closeHook.
  virtual void preCloseHook(){};

  //!@brief This method implements closing of the channel.
  virtual void closeHook() = 0;

  //!@brief Executed after the code in @em closeHook.
  virtual void postCloseHook(){};

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

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
  // none yet

}; // class cedar::dev::Channel


// Code for the channel factory manager ================================================================================

#include "cedar/auxiliaries/FactoryManager.h"


namespace cedar
{
  namespace dev
  {
    //!@brief The manager of all sigmoind instances
    typedef cedar::aux::FactoryManager<cedar::dev::ChannelPtr> ChannelManager;
//
//    //!@brief The singleton object of the TransferFunctionFactory.
//    typedef cedar::aux::Singleton<cedar::dev::ChannelManager> ChannelManagerSingleton;

    // this will be used for managing declarations; we still have to define a declaration class (see below)
   typedef cedar::aux::DeclarationManagerTemplate<cedar::dev::ChannelPtr> ChannelDeclarationManager;

   typedef cedar::aux::Singleton<cedar::dev::ChannelDeclarationManager> ChannelDeclarationManagerSingleton;

   typedef cedar::aux::FactoryManager<cedar::dev::ChannelPtr> ChannelFactoryManager;


    //!@cond SKIPPED_DOCUMENTATION
//    CEDAR_INSTANTIATE_DEV_TEMPLATE(cedar::aux::Singleton<cedar::dev::ChannelManager>);
    //!@endcond
  }
}

CEDAR_DEV_SINGLETON(ChannelManager);
CEDAR_DEV_SINGLETON(ChannelDeclarationManager);
CEDAR_DEV_SINGLETON(ChannelFactoryManager);


#endif // CEDAR_DEV_CHANNEL_H

