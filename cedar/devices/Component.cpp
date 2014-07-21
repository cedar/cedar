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

    File:        Component.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Abstract component of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>
#include <QReadLocker>
#include <QWriteLocker>

#define COMPONENT_CV_MAT_TYPE CV_64F

  //----------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------------------------------------------------

void cedar::dev::Component::init()
  {
    mDeviceThread = std::unique_ptr<cedar::aux::LoopFunctionInThread>(
                                  new cedar::aux::LoopFunctionInThread( 
                                    boost::bind(&cedar::dev::Component::stepDevice,
                                                this,
                                                _1) ));


    mDeviceThread->connectToStartSignal(boost::bind(&cedar::dev::Component::processStart, this));
    mDeviceThread->setStepSize(10);
  }

  // constructor
  cedar::dev::Component::Component()
  {
    init();
  }

cedar::dev::Component::Component(cedar::dev::ChannelPtr channel)
:
mChannel(channel)
  {
    init();
  }

  cedar::dev::Component::~Component()
  {
    // the thread will stopped when mDeviceThread is destructed, anyway, but we
    // try to send the stop request as early as possible ...
    mDeviceThread->requestStop();
  }

  //----------------------------------------------------------------------------------------------------------------------
  // methods
  //----------------------------------------------------------------------------------------------------------------------
  void cedar::dev::Component::setCommandDimensionality(ComponentDataType type, unsigned int dim)
  {
    // todo: locking
    // todo: lazyInitialize if already registered type
    mInstalledCommandDimensions[type] = dim;
  }

  void cedar::dev::Component::setMeasurementDimensionality(ComponentDataType type, unsigned int dim)
  {
    // todo: locking
    // todo: lazyInitialize if already registered type
    mInstalledMeasurementDimensions[type] = dim;
  }

  void cedar::dev::Component::setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim)
  {
    setCommandDimensionality(type, dim);
    setMeasurementDimensionality(type, dim);
  }

  void cedar::dev::Component::resetUserCommandBufferUnlocked(ComponentDataType type)
  {
    auto found = mInstalledCommandDimensions.find(type);

    if (found == mInstalledCommandDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mUserCommandBuffer.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeUserCommandBufferUnlocked(ComponentDataType type)
  {
    auto found = mUserCommandBuffer.member().find(type);

    // lazy initialization
    if (found == mUserCommandBuffer.member().end()
        || found->second->getData().empty())
    {
      // cast away const-ness
      resetUserCommandBufferUnlocked(type);
    }
  }

  void cedar::dev::Component::resetDeviceCommandBufferUnlocked(ComponentDataType type)
  {
    auto found = mInstalledCommandDimensions.find(type);

    if (found == mInstalledCommandDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mDeviceSubmittedCommands.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeMember
       (
         cedar::aux::LockableMember<std::map< ComponentDataType, cedar::aux::MatDataPtr> >& lockableMember,
         boost::function<void (ComponentDataType)> initFun,
         ComponentDataType type
       )
  {
    auto found = lockableMember.member().find(type);

    if (found == lockableMember.member().end()
        || found->second->getData().empty())
    {
      // cast away const for lazy init to work
      initFun(type);
    }
  }

  void cedar::dev::Component::lazyInitializeDeviceCommandBufferUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mDeviceSubmittedCommands,
      boost::bind(&cedar::dev::Component::resetDeviceCommandBufferUnlocked, this, _1),
      type
    );
//    auto found = mDeviceSubmittedCommands.member().find(type);
//
//    if (found == mDeviceSubmittedCommands.member().end()
//        || found->second->getData().empty())
//    {
//      // cast away const for lazy init to work
//      resetDeviceCommandBufferUnlocked(type);
//    }
  }

  void cedar::dev::Component::resetUserMeasurementBufferUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mUserMeasurementsBuffer.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeUserMeasurementBufferUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mUserMeasurementsBuffer,
      boost::bind(&cedar::dev::Component::resetUserMeasurementBufferUnlocked, this, _1),
      type
    );
//    auto found = mUserMeasurementsBuffer.member().find(type);
//
//  // problem: NEED to initialize with correct value!
//    if (found == mUserMeasurementsBuffer.member().end()
//        || found->second.empty())
//    {
//  // TODO: throw!
//
//      // lazy initialization
//      // cast away const for lazy init to work
//      resetUserMeasurementBufferUnlocked(type);
//    }
  }

  void cedar::dev::Component::resetPreviousDeviceMeasurementBufferUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mPreviousDeviceMeasurementsBuffer.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializePreviousDeviceMeasurementBufferUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mPreviousDeviceMeasurementsBuffer,
      boost::bind(&cedar::dev::Component::resetPreviousDeviceMeasurementBufferUnlocked, this, _1),
      type
    );
//    auto found = mPreviousDeviceMeasurementsBuffer.member().find(type);
//
//  // problem: NEED to initialize with correct value!
//    if (found == mPreviousDeviceMeasurementsBuffer.member().end()
//        || found->second.empty())
//    {
//  // TODO: throw!
//
//      // lazy initialization
//      // cast away const for lazy init to work
//      resetPreviousDeviceMeasurementBufferUnlocked(type);
//    }
  }


  void cedar::dev::Component::resetDeviceMeasurementBufferUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mDeviceRetrievedMeasurements.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeDeviceMeasurementBufferUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mDeviceRetrievedMeasurements,
      boost::bind(&cedar::dev::Component::resetDeviceMeasurementBufferUnlocked, this, _1),
      type
    );
//    auto found = mDeviceRetrievedMeasurements.member().find(type);
//
//    if (found == mDeviceRetrievedMeasurements.member().end()
//        || found->second.empty())
//    {
//      // cast away const for lazy init to work
//      resetDeviceMeasurementBufferUnlocked(type);
//    }
  }

  void cedar::dev::Component::installCommandType(ComponentDataType type)
  {
    // lazyInitialize whetehr already registered and throw TODO
    mInstalledCommandTypes.push_back( type );
  }

  void cedar::dev::Component::installMeasurementType(ComponentDataType type)
  {
    // lazyInitialize whetehr already registered and throw TODO
    mInstalledMeasurementTypes.push_back( type );
  }

  void cedar::dev::Component::installCommandAndMeasurementType(ComponentDataType type)
  {
    registerCommandType( type );
    registerMeasurementType( type );
  }

  void cedar::dev::Component::resetComponent()
  {
    {
      //!@todo As this isn't locked in a canonical lock order, this may lead to deadlocks; use cedar::aux::Lockable?
      QWriteLocker lock2(mUserMeasurementsBuffer.getLockPtr());
      QWriteLocker lock2b(mPreviousDeviceMeasurementsBuffer.getLockPtr());
      QWriteLocker lock4(mDeviceRetrievedMeasurements.getLockPtr());

      for( ComponentDataType &type : mInstalledMeasurementTypes )
      {
        resetUserMeasurementBufferUnlocked(type);
        resetPreviousDeviceMeasurementBufferUnlocked(type);
        resetDeviceMeasurementBufferUnlocked(type);
      }
    }

    {
      QWriteLocker lock1(mUserCommandBuffer.getLockPtr());
      QWriteLocker lock3(mDeviceSubmittedCommands.getLockPtr());

      for( ComponentDataType &type : mInstalledCommandTypes )
      {
        resetUserCommandBufferUnlocked(type);
        resetDeviceCommandBufferUnlocked(type);
      }
    }
  }

  void cedar::dev::Component::applyDeviceCommandsAs(ComponentDataType type)
  {
    // todo check for registered
    mDeviceCommandSelection = type;
  }

  void cedar::dev::Component::setUserCommandBuffer(ComponentDataType type, cv::Mat data)
  {
    // todo: lazyInitialize for command restrictions

    // todo: issue a console-warning something if Device is not running

    QWriteLocker lock(mUserCommandBuffer.getLockPtr());

    setUserCommandBufferUnlocked(type, data);
  }

  void cedar::dev::Component::setInitialUserCommandBuffer(ComponentDataType type, cv::Mat data)
  {
    // todo: lazyInitialize for command restrictions

    // Device is not meant to be running
    // todo: throw if already running!
    // todo: throw if not empty

    // todo: there is no lock here. I think we dont need one, sure?

    setInitialUserCommandBufferUnlocked(type, data);
  }


  void cedar::dev::Component::setUserCommandBufferIndex(ComponentDataType type, int index, double value) 
  {
    // todo: lazyInitialize for command restrictions

    // todo: issue a console-warning something if Device is not running

    QWriteLocker lock(mUserCommandBuffer.getLockPtr());

    setUserCommandIndexUnlocked(type, index, value);
  }


  cv::Mat cedar::dev::Component::getUserMeasurementBuffer(ComponentDataType type) const
  {
    QReadLocker lock(mUserMeasurementsBuffer.getLockPtr());

    auto ret =  getUserMeasurementBufferUnlocked(type);
    return ret;
  }

  double cedar::dev::Component::getUserMeasurementBufferIndex(ComponentDataType type, int index) const
  {
    QReadLocker lock(mUserMeasurementsBuffer.getLockPtr());

    auto ret = getUserMeasurementIndexUnlocked(type, index);

    return ret;
  }

  cv::Mat cedar::dev::Component::getPreviousDeviceMeasurementBuffer(ComponentDataType type) const
  {
    QReadLocker lock(mPreviousDeviceMeasurementsBuffer.getLockPtr());

    auto ret =  getPreviousDeviceMeasurementBufferUnlocked(type);
    return ret;
  }

  double cedar::dev::Component::getPreviousDeviceMeasurementBufferIndex(ComponentDataType type, int index) const
  {
    QReadLocker lock(mPreviousDeviceMeasurementsBuffer.getLockPtr());

    auto ret = getPreviousDeviceMeasurementIndexUnlocked(type, index);

    return ret;
  }





  void cedar::dev::Component::registerDeviceCommandHook(ComponentDataType type, CommandFunctionType fun)
  {
    // todo checks
    // todo locks
    mSubmitCommandHooks[ type ] = fun;
  }

  void cedar::dev::Component::registerDeviceMeasurementHook(ComponentDataType type, MeasurementFunctionType fun)
  {
    // todo checks
    // todo locks
    mRetrieveMeasurementHooks[ type ] = fun;
  }

  void cedar::dev::Component::registerUserCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
  {
    // todo checks
    // todo locks


    auto found = mCommandTransformationHooks.find( from );

    if (found == mCommandTransformationHooks.end())
    {
      mCommandTransformationHooks[ from ] = InnerTransformationHookContainerType{ {to, fun} };
    }
    else
    {
      auto found2 = (found->second).find( to );
      if (found2 == (found->second).end())
      {
        (found->second)[ to ] = fun;
      }
      else
      {
std::cout << "ERROR: reregistering transformation hook cmd from " << from << " to " << to << std::endl;
        // TODO: throw

        return;
      }
    }
//  std::cout << "registered user command trafo from " << from << " to: " << to << std::endl;  
  }

  void cedar::dev::Component::registerDeviceMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
  {
    // todo checks
    // todo locks

    auto found = mMeasurementTransformationHooks.find( from );

    if (found == mMeasurementTransformationHooks.end())
    {
      mMeasurementTransformationHooks[ from ] = InnerTransformationHookContainerType{ {to, fun} };
    }
    else
    {
      auto found2 = (found->second).find( to );
      if (found2 == (found->second).end())
      {
        (found->second)[ to ] = fun;
      }
      else
      {
std::cout << "ERROR: reregistering transformation hook measurment from " << from << " to " << to << std::endl;
        // TODO: throw

        return;
      }
    }
//  std::cout << "registered Device measurement trafo from " << from << " to: " << to << std::endl;  

  }

  void cedar::dev::Component::stepDevice(double time)
  {
    // its important to get the currently scheduled commands out first
    // (think safety first). this assumes serial communication, of course
    stepDeviceCommands(time);
    stepDeviceMeasurements(time); // note, the post-measurements transformations also take time
  }

  void cedar::dev::Component::stepDeviceCommands(double)
  {
#if 1
    // todo: check locking in this function, forgot some stuff ...
    ComponentDataType type_for_Device, type_from_user;

    if (mUserCommandBuffer.member().size() == 0)
    {
      return; // this is not a problem
    }
    if (mSubmitCommandHooks.size() == 0)
    {
      // todo: throw
      return;
    }

    {
      // guess command type to use
      // safe only if there was only one command hook registered
      if (mUserCommandBuffer.member().size() > 1)
      {
          // todo: throw
        std::cout << "error: setted too many commands of different types! ... " <<  std::endl;
        for (auto &what : mUserCommandBuffer.member() )
        {
          std::cout << " " << what.first << std::endl;
        }
        return;
      }

      // we know the map has exactly one entry
      type_from_user = mUserCommandBuffer.member().begin()->first;
    }

    // evaluate command type for Device:
    if (mDeviceCommandSelection)
    {
        type_for_Device = mDeviceCommandSelection.get();
//  std::cout << "commands restricted to ... " << type_for_Device  << std::endl;    
    }
    else
    {
      // guess Device type to use. easy if there is only one hook
      if (mSubmitCommandHooks.size() > 1)
      {
  // todo: throw 
//  std::cout << "error: cannot guess what Device command to send ... " << type_for_Device  << std::endl;
        return;
      }

      type_for_Device = mSubmitCommandHooks.begin()->first;
    }

    cv::Mat userData, ioData;
    {
      QReadLocker lock(mUserCommandBuffer.getLockPtr());
      userData = getUserCommandBufferUnlocked( type_from_user ).clone();
    }

    // do we need to transform the input?
    if (type_for_Device != type_from_user)
    {
      // does a transformation exist?
      auto found1 = mCommandTransformationHooks.find(type_from_user);
      if (found1 == mCommandTransformationHooks.end())
      {
        // todo throw:
//  std::cout << "missing appropriate transformation from command (" << type_from_user << std::endl;    
        return;
      }

      auto found2 = found1->second.find(type_for_Device);
      if (found2 == found1->second.end())
      {
        // todo throw:
//  std::cout << "missing appropriate transformation to Device command for:" << type_for_Device << " (from: " << type_from_user << ")" << std::endl;    
        return;

      }

      QReadLocker lock1(mUserMeasurementsBuffer.getLockPtr());
      ioData = (found2->second)(userData);
    }
    else
    {
      ioData = userData.clone();
    }

    auto hook_found = mSubmitCommandHooks.find(type_for_Device);
    if (hook_found == mSubmitCommandHooks.end())
    {
//  std::cout << "set hook not found" << std::endl;    
      // todo
      return;
    }

    (hook_found->second)(ioData);
#endif

    mUserCommandBuffer.member().clear();
  }

  // update the Device Cache
void cedar::dev::Component::stepDeviceMeasurements(double)
{
  std::vector< ComponentDataType > types_to_transform;
  std::vector< ComponentDataType > types_we_measured;;

  // lock measurements 
  QWriteLocker lock1(mDeviceRetrievedMeasurements.getLockPtr());

  mDeviceRetrievedMeasurements.member().clear();

  // thinks I can get directly from HW:
  for( auto& type : mInstalledMeasurementTypes )
  {
//  std::cout << " test"  << type <<    std::endl;
    auto found = mRetrieveMeasurementHooks.find( type );

    if (found != mRetrieveMeasurementHooks.end())
    {
      // execute the hook:
      mDeviceRetrievedMeasurements.member()[ type ]->setData((found->second)());
      types_we_measured.push_back( type );
//std::cout << "we measured a tyep for: " << type << std::endl;      
//std::cout << "  it was: " <<  mDeviceRetrievedMeasurements[ type ] << std::endl;
    }
    else
    {
//  std::cout << "    need to transform " << type << std::end;      
      // store things we need to calculate afterwards
      types_to_transform.push_back( type );
    }
  }

  for (auto& missing_type : types_to_transform )
  {
    bool nothing_found = true;
    for ( auto& measured_type : types_we_measured )
    {
      auto found1 = mMeasurementTransformationHooks.find( measured_type );

      if (found1 != mMeasurementTransformationHooks.end())
      {

        auto found2 = (found1->second).find( missing_type );

        if (found2 != (found1->second).end())
        {
          mDeviceRetrievedMeasurements.member()[ missing_type ]->setData((found2->second)( mDeviceRetrievedMeasurements.member()[ measured_type ]->getData() ));
          nothing_found = false;
          break;
        }
      }
    }
  }

  lock1.unlock();
  // todo: make this non-blocking for this looped thread
  updateUserMeasurements();
}

void cedar::dev::Component::updateUserMeasurements()
{
  // lock Device cache todo
  // lock caches
  QReadLocker lock1(mPreviousDeviceMeasurementsBuffer.getLockPtr());
  QWriteLocker lock2(mUserMeasurementsBuffer.getLockPtr());
  QWriteLocker lock3(mDeviceRetrievedMeasurements.getLockPtr());

  mPreviousDeviceMeasurementsBuffer.member() = mUserMeasurementsBuffer.member();
    // todo: is this really a deep copy?
  mUserMeasurementsBuffer.member() = mDeviceRetrievedMeasurements.member();
  mDeviceRetrievedMeasurements.member().clear();

  lock1.unlock();
  lock2.unlock();

  emit updatedUserMeasurementSignal();
}

void cedar::dev::Component::startDevice()
{
  // todo: warn if no measurements and no commands are set:wu
  mDeviceThread->start();
}

void cedar::dev::Component::stopDevice()
{
  mDeviceThread->stop();
}

void cedar::dev::Component::start()
{
  startDevice();
}

void cedar::dev::Component::stop()
{
  stopDevice();
}

unsigned int cedar::dev::Component::getDeviceStepSize()
{
  return mDeviceThread->getStepSize();
}

void cedar::dev::Component::setStepSize(double d)
{
  mDeviceThread->setStepSize(d);
}

void cedar::dev::Component::setIdleTime(double d)
{
  mDeviceThread->setIdleTime(d);
}

void setStepSize(const cedar::unit::Time& time)
{
  mDeviceThread->setStepSize(time);
}

void setIdleTime(const cedar::unit::Time& time)
{
  mDeviceThread->setIdleTime(time);
}

void cedar::dev::Component::setSimulatedTime(double d)
{
  mDeviceThread->setSimulatedTime(d);
}

bool cedar::dev::Component::isRunning()
{
  return mDeviceThread->isRunning();
}

bool cedar::dev::Component::isRunningNolocking()
{
  return mDeviceThread->isRunningNolocking();
}

void cedar::dev::Component::startTimer(double d)
{
  // this does nothing. think
}

void cedar::dev::Component::stopTimer()
{
}

void cedar::dev::Component::setUserCommandBufferUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeUserCommandBufferUnlocked(type);
  mUserCommandBuffer.member()[ type ]->setData(data.clone());
}

void cedar::dev::Component::setInitialUserCommandBufferUnlocked(ComponentDataType& type, cv::Mat data)
{
  mInitialUserSubmittedCommands.member()[ type ]->setData(data.clone());
}

void cedar::dev::Component::setUserCommandIndexUnlocked(ComponentDataType& type,int index, double value) 
{
  lazyInitializeUserCommandBufferUnlocked(type);
  mUserCommandBuffer.member()[ type ]->getData().at<double>(index, 0) = value;
}

cv::Mat cedar::dev::Component::getUserCommandBufferUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeUserCommandBufferUnlocked(type);

  auto found = mUserCommandBuffer.member().find(type);

  if (found == mUserCommandBuffer.member().end())
  {
    // todo: kann nicht passieren wg lazyInitializeUsercommandUnlocked; throw
  }

  return found->second->getData();
}


void cedar::dev::Component::setUserMeasurementBufferUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeUserMeasurementBufferUnlocked(type);
  mUserMeasurementsBuffer.member()[ type ]->setData(data.clone());
}


cv::Mat cedar::dev::Component::getUserMeasurementBufferUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeUserMeasurementBufferUnlocked(type);
  auto found = mUserMeasurementsBuffer.member().find(type);

// problem: NEED to initialize with correct value!
  if (found == mUserMeasurementsBuffer.member().end())
  {
    // todo: kann nicht passieren, throw
    // todo: DOCH, kann passieren, wenn Messung geholt wird, bevor Messung kam ...
    // todo: warning werfen, wenn letzte Messung zu lange her ...
    //!@todo this'll print a warning in case these things go wrong; replace by throw
    CEDAR_NON_CRITICAL_ASSERT(false && "This should not happen");
    // lazy initialization
    // cast away const for lazy init to work
  }

  return found->second->getData();
}

double cedar::dev::Component::getUserMeasurementIndexUnlocked(ComponentDataType& type, int index) const
{
  return getUserMeasurementBufferUnlocked(type).at<double>(index,0);
}

cv::Mat cedar::dev::Component::getPreviousDeviceMeasurementBufferUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializePreviousDeviceMeasurementBufferUnlocked(type);
  auto found = mPreviousDeviceMeasurementsBuffer.member().find(type);

// problem: NEED to initialize with correct value!
  if (found == mPreviousDeviceMeasurementsBuffer.member().end())
  {
    // todo: kann nicht passieren, throw
    // todo: DOCH, kann passieren, wenn Messung geholt wird, bevor Messung kam ...
    // todo: warning werfen, wenn letzte Messung zu lange her ...
    //!@todo this'll print a warning in case these things go wrong; replace by throw
    CEDAR_NON_CRITICAL_ASSERT(false && "This should not happen");
    // lazy initialization
    // cast away const for lazy init to work
  }

  return found->second->getData();
}

double cedar::dev::Component::getPreviousDeviceMeasurementIndexUnlocked(ComponentDataType& type, int index) const
{
  return getPreviousDeviceMeasurementBufferUnlocked(type).at<double>(index,0);
}


void cedar::dev::Component::setDeviceCommandBufferUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeDeviceCommandBufferUnlocked(type);
  mDeviceSubmittedCommands.member()[ type ]->setData(data);
}

cv::Mat cedar::dev::Component::getDeviceCommandBufferUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeDeviceCommandBufferUnlocked(type);
  auto found = mDeviceSubmittedCommands.member().find(type);

  if (found == mDeviceSubmittedCommands.member().end())
  {
    // todo throw. cannot happen
    CEDAR_ASSERT(false && "this should not happen.");
  }

  return found->second->getData();
}

void cedar::dev::Component::setDeviceMeasurementBufferUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeDeviceMeasurementBufferUnlocked(type);
  mDeviceRetrievedMeasurements.member()[ type ]->setData(data);
}


cv::Mat cedar::dev::Component::getDeviceMeasurementBufferUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeDeviceMeasurementBufferUnlocked(type);
  auto found = mDeviceRetrievedMeasurements.member().find(type);

  //!@todo Throw
  CEDAR_ASSERT(found != mDeviceRetrievedMeasurements.member().end());
//  if (found == mDeviceRetrievedMeasurements.end() )
//  {
//  }

  return found->second->getData();
}

cv::Mat cedar::dev::Component::integrateDevice(cv::Mat data, ComponentDataType type)
{
  double timestep;

  timestep= mDeviceThread->getStepSize();

  QReadLocker lock(mUserMeasurementsBuffer.getLockPtr());
  return ( data * ( timestep / 1000.0 ) ) + getUserMeasurementBufferUnlocked(type);;
}

cv::Mat cedar::dev::Component::integrateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  double timestep;

  timestep= mDeviceThread->getStepSize();

  QReadLocker lock(mUserMeasurementsBuffer.getLockPtr());
  return ( ( data * (timestep / 1000.0 ) + getUserMeasurementBufferUnlocked(type1) )
           * ( timestep / 1000.0 ) )
         + getUserMeasurementBufferUnlocked(type2);
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateDevice(cv::Mat data, ComponentDataType type)
{
  double timestep;

  timestep= mDeviceThread->getStepSize();

// todo: check locking here
  return ( data - getPreviousDeviceMeasurementBufferUnlocked(type) )
         / ( timestep / 1000.0 );
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  double timestep;

  timestep= mDeviceThread->getStepSize();

// todo: check locking here
  return ( ( data - getPreviousDeviceMeasurementBufferUnlocked(type1) )
           / ( timestep / 1000.0 ) 
           - getPreviousDeviceMeasurementBufferUnlocked(type2) )
         / ( timestep / 1000.0 );
}


void cedar::dev::Component::processStart()
{
  // this is the initial run:


  // todo: test that mUserCommands is empty!

  if (!mInitialUserSubmittedCommands.member().empty())
  {
     // do as if the initial user command was the user command
     QWriteLocker lock1(mUserCommandBuffer.getLockPtr());
     
     mUserCommandBuffer.member() = mInitialUserSubmittedCommands.member();
     lock1.unlock();
     stepDeviceCommands(0.0);
  }

  // get measurements (blocking!) when the thread is started ...
  stepDeviceMeasurements(0.0);
}

