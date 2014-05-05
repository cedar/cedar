/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// SYSTEM INCLUDES
#include <boost/bind.hpp>
#include <QReadLocker>
#include <QWriteLocker>

#define COMPONENT_CV_MAT_TYPE CV_64F

  //----------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------------------------------------------------

  // constructor
  cedar::dev::Component::Component()
  {
    mIOThread = std::unique_ptr<cedar::aux::LoopFunctionInThread>(
                                  new cedar::aux::LoopFunctionInThread( 
                                    boost::bind(&cedar::dev::Component::loopIO,
                                                this,
                                                _1) ));


    mIOThread->connectToStartSignal(boost::bind(&cedar::dev::Component::processStart, this));
  mIOThread->setStepSize(10);

  }

  cedar::dev::Component::~Component()
  {
    // the thread will stopped when mIOThread is destructed, anyway, but we
    // try to send the stop request as early as possible ...
    mIOThread->requestStop();
  }

  //----------------------------------------------------------------------------------------------------------------------
  // methods
  //----------------------------------------------------------------------------------------------------------------------
  void cedar::dev::Component::setCommandDimensionality(ComponentDataType type, unsigned int dim)
  {
    // todo: locking
    // todo: lazyInitialize if already installed type
    mInstalledCommandDimensions[type] = dim;
  }

  void cedar::dev::Component::setMeasurementDimensionality(ComponentDataType type, unsigned int dim)
  {
    // todo: locking
    // todo: lazyInitialize if already installed type
    mInstalledMeasurementDimensions[type] = dim;
  }

  void cedar::dev::Component::setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim)
  {
    setCommandDimensionality(type, dim);
    setMeasurementDimensionality(type, dim);
  }

  void cedar::dev::Component::resetUserCommandUnlocked(ComponentDataType type)
  {
    auto found = mInstalledCommandDimensions.find(type);

    if (found == mInstalledCommandDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mUserSubmittedCommands.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeUserCommandUnlocked(ComponentDataType type)
  {
    auto found = mUserSubmittedCommands.member().find(type);

    // lazy initialization
    if (found == mUserSubmittedCommands.member().end()
        || found->second->getData().empty())
    {
      // cast away const-ness
      resetUserCommandUnlocked(type);
    }
  }

  void cedar::dev::Component::resetIOCommandUnlocked(ComponentDataType type)
  {
    auto found = mInstalledCommandDimensions.find(type);

    if (found == mInstalledCommandDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mIOSubmittedCommands.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
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

  void cedar::dev::Component::lazyInitializeIOCommandUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mIOSubmittedCommands,
      boost::bind(&cedar::dev::Component::resetIOCommandUnlocked, this, _1),
      type
    );
//    auto found = mIOSubmittedCommands.member().find(type);
//
//    if (found == mIOSubmittedCommands.member().end()
//        || found->second->getData().empty())
//    {
//      // cast away const for lazy init to work
//      resetIOCommandUnlocked(type);
//    }
  }

  void cedar::dev::Component::resetUserMeasurementUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mUserRetrievedMeasurements.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeUserMeasurementUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mUserRetrievedMeasurements,
      boost::bind(&cedar::dev::Component::resetUserMeasurementUnlocked, this, _1),
      type
    );
//    auto found = mUserRetrievedMeasurements.member().find(type);
//
//  // problem: NEED to initialize with correct value!
//    if (found == mUserRetrievedMeasurements.member().end()
//        || found->second.empty())
//    {
//  // TODO: throw!
//
//      // lazy initialization
//      // cast away const for lazy init to work
//      resetUserMeasurementUnlocked(type);
//    }
  }

  void cedar::dev::Component::resetLastIOMeasurementUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mLastIORetrievedMeasurements.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeLastIOMeasurementUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mLastIORetrievedMeasurements,
      boost::bind(&cedar::dev::Component::resetLastIOMeasurementUnlocked, this, _1),
      type
    );
//    auto found = mLastIORetrievedMeasurements.member().find(type);
//
//  // problem: NEED to initialize with correct value!
//    if (found == mLastIORetrievedMeasurements.member().end()
//        || found->second.empty())
//    {
//  // TODO: throw!
//
//      // lazy initialization
//      // cast away const for lazy init to work
//      resetLastIOMeasurementUnlocked(type);
//    }
  }


  void cedar::dev::Component::resetIOMeasurementUnlocked(ComponentDataType type)
  {
    auto found = mInstalledMeasurementDimensions.find(type);

    if (found == mInstalledMeasurementDimensions.end())
    {
      // todo
    }

    auto dim = found->second;

    mIORetrievedMeasurements.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
  }

  void cedar::dev::Component::lazyInitializeIOMeasurementUnlocked(ComponentDataType type)
  {
    this->lazyInitializeMember
    (
      mIORetrievedMeasurements,
      boost::bind(&cedar::dev::Component::resetIOMeasurementUnlocked, this, _1),
      type
    );
//    auto found = mIORetrievedMeasurements.member().find(type);
//
//    if (found == mIORetrievedMeasurements.member().end()
//        || found->second.empty())
//    {
//      // cast away const for lazy init to work
//      resetIOMeasurementUnlocked(type);
//    }
  }

  void cedar::dev::Component::installCommandType(ComponentDataType type)
  {
    // lazyInitialize whetehr already installed and throw TODO
    mInstalledCommandTypes.push_back( type );
  }

  void cedar::dev::Component::installMeasurementType(ComponentDataType type)
  {
    // lazyInitialize whetehr already installed and throw TODO
    mInstalledMeasurementTypes.push_back( type );
  }

  void cedar::dev::Component::installCommandAndMeasurementType(ComponentDataType type)
  {
    installCommandType( type );
    installMeasurementType( type );
  }

  void cedar::dev::Component::resetComponent()
  {
    {
      //!@todo As this isn't locked in a canonical lock order, this may lead to deadlocks; use cedar::aux::Lockable?
      QWriteLocker lock2(mUserRetrievedMeasurements.getLockPtr());
      QWriteLocker lock2b(mLastIORetrievedMeasurements.getLockPtr());
      QWriteLocker lock4(mIORetrievedMeasurements.getLockPtr());

      for( ComponentDataType &type : mInstalledMeasurementTypes )
      {
        resetUserMeasurementUnlocked(type);
        resetLastIOMeasurementUnlocked(type);
        resetIOMeasurementUnlocked(type);
      }
    }

    {
      QWriteLocker lock1(mUserSubmittedCommands.getLockPtr());
      QWriteLocker lock3(mIOSubmittedCommands.getLockPtr());

      for( ComponentDataType &type : mInstalledCommandTypes )
      {
        resetUserCommandUnlocked(type);
        resetIOCommandUnlocked(type);
      }
    }
  }

  void cedar::dev::Component::restrictUserCommandsTo(ComponentDataType type)
  {
    // todo check for installed
    // todo locking
    mUserCommandRestriction = type;
  }

  void cedar::dev::Component::applyIOCommandsAs(ComponentDataType type)
  {
    // toto check for installe
    mIOCommandRestriction = type;
  }

  void cedar::dev::Component::submitUserCommand(ComponentDataType type, cv::Mat data)
  {
    // todo: lazyInitialize for command restrictions

    // todo: issue a console-warning something if IO is not running

    QWriteLocker lock(mUserSubmittedCommands.getLockPtr());

    setUserCommandUnlocked(type, data);
  }

  void cedar::dev::Component::submitInitialUserCommand(ComponentDataType type, cv::Mat data)
  {
    // todo: lazyInitialize for command restrictions

    // IO is not meant to be running
    // todo: throw if already running!
    // todo: throw if not empty

    // todo: there is no lock here. I think we dont need one, sure?

    setInitialUserCommandUnlocked(type, data);
  }


  void cedar::dev::Component::submitUserCommandIndex(ComponentDataType type, int index, double value) 
  {
    // todo: lazyInitialize for command restrictions

    // todo: issue a console-warning something if IO is not running

    QWriteLocker lock(mUserSubmittedCommands.getLockPtr());

    setUserCommandIndexUnlocked(type, index, value);
  }


  cv::Mat cedar::dev::Component::fetchUserMeasurement(ComponentDataType type) const
  {
    QReadLocker lock(mUserRetrievedMeasurements.getLockPtr());

    auto ret =  getUserMeasurementUnlocked(type);
    return ret;
  }

  double cedar::dev::Component::fetchUserMeasurementIndex(ComponentDataType type, int index) const
  {
    QReadLocker lock(mUserRetrievedMeasurements.getLockPtr());

    auto ret = getUserMeasurementIndexUnlocked(type, index);

    return ret;
  }

  cv::Mat cedar::dev::Component::fetchLastIOMeasurement(ComponentDataType type) const
  {
    QReadLocker lock(mLastIORetrievedMeasurements.getLockPtr());

    auto ret =  getLastIOMeasurementUnlocked(type);
    return ret;
  }

  double cedar::dev::Component::fetchLastIOMeasurementIndex(ComponentDataType type, int index) const
  {
    QReadLocker lock(mLastIORetrievedMeasurements.getLockPtr());

    auto ret = getLastIOMeasurementIndexUnlocked(type, index);

    return ret;
  }





  void cedar::dev::Component::registerIOCommandHook(ComponentDataType type, CommandFunctionType fun)
  {
    // todo checks
    // todo locks
    mSubmitCommandHooks[ type ] = fun;
  }

  void cedar::dev::Component::registerIOMeasurementHook(ComponentDataType type, MeasurementFunctionType fun)
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

  void cedar::dev::Component::registerIOMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
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
//  std::cout << "registered IO measurement trafo from " << from << " to: " << to << std::endl;  

  }

  void cedar::dev::Component::loopIO(double time)
  {
    // its important to get the currently scheduled commands out first
    // (think safety first). this assumes serial communication, of course
    loopIOCommands(time);
    loopIOMeasurements(time); // note, the post-measurements transformations also take time
  }

  void cedar::dev::Component::loopIOCommands(double)
  {
#if 1
    // todo: check locking in this function, forgot some stuff ...
    ComponentDataType type_for_IO, type_from_user;

    if (mUserSubmittedCommands.member().size() == 0)
    {
      return; // this is not a problem
    }
    if (mSubmitCommandHooks.size() == 0)
    {
      // todo: throw
      return;
    }

    // evaluate which user command type to use:
    if (mUserCommandRestriction)
    {
      type_from_user = mUserCommandRestriction.get();
    }
    else
    {
      // guess command type to use
      // safe only if there was only one command hook registered
      if (mUserSubmittedCommands.member().size() > 1)
      {
          // todo: throw
        std::cout << "error: submitted too many commands of different types! ... " <<  std::endl;
        for (auto &what : mUserSubmittedCommands.member() )
        {
          std::cout << " " << what.first << std::endl;
        }
        return;
      }

      // we know the map has exactly one entry
      type_from_user = mUserSubmittedCommands.member().begin()->first;
    }

    // evaluate command type for IO:
    if (mIOCommandRestriction)
    {
        type_for_IO = mIOCommandRestriction.get();
//  std::cout << "commands restricted to ... " << type_for_IO  << std::endl;    
    }
    else
    {
      // guess IO type to use. easy if there is only one hook
      if (mSubmitCommandHooks.size() > 1)
      {
  // todo: throw 
//  std::cout << "error: cannot guess what IO command to send ... " << type_for_IO  << std::endl;
        return;
      }

      type_for_IO = mSubmitCommandHooks.begin()->first;
    }

    cv::Mat userData, ioData;
    {
      QReadLocker lock(mUserSubmittedCommands.getLockPtr());
      userData = getUserCommandUnlocked( type_from_user ).clone();
    }

    // do we need to transform the input?
    if (type_for_IO != type_from_user)
    {
      // does a transformation exist?
      auto found1 = mCommandTransformationHooks.find(type_from_user);
      if (found1 == mCommandTransformationHooks.end())
      {
        // todo throw:
//  std::cout << "missing appropriate transformation from command (" << type_from_user << std::endl;    
        return;
      }

      auto found2 = found1->second.find(type_for_IO);
      if (found2 == found1->second.end())
      {
        // todo throw:
//  std::cout << "missing appropriate transformation to IO command for:" << type_for_IO << " (from: " << type_from_user << ")" << std::endl;    
        return;

      }

      QReadLocker lock1(mUserRetrievedMeasurements.getLockPtr());
      ioData = (found2->second)(userData);
    }
    else
    {
      ioData = userData.clone();
    }

    auto hook_found = mSubmitCommandHooks.find(type_for_IO);
    if (hook_found == mSubmitCommandHooks.end())
    {
//  std::cout << "submit hook not found" << std::endl;    
      // todo
      return;
    }

    (hook_found->second)(ioData);
#endif

    mUserSubmittedCommands.member().clear();
  }

  // update the IO Cache
void cedar::dev::Component::loopIOMeasurements(double)
{
  std::vector< ComponentDataType > types_to_transform;
  std::vector< ComponentDataType > types_we_measured;;

  // lock measurements 
  QWriteLocker lock1(mIORetrievedMeasurements.getLockPtr());

  mIORetrievedMeasurements.member().clear();

  // thinks I can get directly from HW:
  for( auto& type : mInstalledMeasurementTypes )
  {
//  std::cout << " test"  << type <<    std::endl;
    auto found = mRetrieveMeasurementHooks.find( type );

    if (found != mRetrieveMeasurementHooks.end())
    {
      // execute the hook:
      mIORetrievedMeasurements.member()[ type ]->setData((found->second)());
      types_we_measured.push_back( type );
//std::cout << "we measured a tyep for: " << type << std::endl;      
//std::cout << "  it was: " <<  mIORetrievedMeasurements[ type ] << std::endl;
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
          mIORetrievedMeasurements.member()[ missing_type ]->setData((found2->second)( mIORetrievedMeasurements.member()[ measured_type ]->getData() ));
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
  // lock IO cache todo
  // lock caches
  QReadLocker lock1(mLastIORetrievedMeasurements.getLockPtr());
  QWriteLocker lock2(mUserRetrievedMeasurements.getLockPtr());
  QWriteLocker lock3(mIORetrievedMeasurements.getLockPtr());

  mLastIORetrievedMeasurements.member() = mUserRetrievedMeasurements.member();
    // todo: is this really a deep copy?
  mUserRetrievedMeasurements.member() = mIORetrievedMeasurements.member();
  mIORetrievedMeasurements.member().clear();

  lock1.unlock();
  lock2.unlock();

  emit updatedUserMeasurementSignal();
}

void cedar::dev::Component::startIO()
{
  // todo: warn if no measurements and no commands are set:wu
  mIOThread->start();
}

void cedar::dev::Component::stopIO()
{
  mIOThread->stop();
}

void cedar::dev::Component::start()
{
  startIO();
}

void cedar::dev::Component::stop()
{
  stopIO();
}

unsigned int cedar::dev::Component::getIOStepSize()
{
  return mIOThread->getStepSize();
}

void cedar::dev::Component::setStepSize(double d)
{
  mIOThread->setStepSize(d);
}

void cedar::dev::Component::setIdleTime(double d)
{
  mIOThread->setIdleTime(d);
}

void cedar::dev::Component::setSimulatedTime(double d)
{
  mIOThread->setSimulatedTime(d);
}

bool cedar::dev::Component::isRunning()
{
  return mIOThread->isRunning();
}

void cedar::dev::Component::startTimer(double d)
{
  // this does nothing. think
}

void cedar::dev::Component::stopTimer()
{
}

void cedar::dev::Component::setUserCommandUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeUserCommandUnlocked(type);
  mUserSubmittedCommands.member()[ type ]->setData(data.clone());
}

void cedar::dev::Component::setInitialUserCommandUnlocked(ComponentDataType& type, cv::Mat data)
{
  mInitialUserSubmittedCommands.member()[ type ]->setData(data.clone());
}

void cedar::dev::Component::setUserCommandIndexUnlocked(ComponentDataType& type,int index, double value) 
{
  lazyInitializeUserCommandUnlocked(type);
  mUserSubmittedCommands.member()[ type ]->getData().at<double>(index, 0) = value;
}

cv::Mat cedar::dev::Component::getUserCommandUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeUserCommandUnlocked(type);

  auto found = mUserSubmittedCommands.member().find(type);

  if (found == mUserSubmittedCommands.member().end())
  {
    // todo: kann nicht passieren wg lazyInitializeUsercommandUnlocked; throw
  }

  return found->second->getData();
}


void cedar::dev::Component::setUserMeasurementUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeUserMeasurementUnlocked(type);
  mUserRetrievedMeasurements.member()[ type ]->setData(data.clone());
}


cv::Mat cedar::dev::Component::getUserMeasurementUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeUserMeasurementUnlocked(type);
  auto found = mUserRetrievedMeasurements.member().find(type);

// problem: NEED to initialize with correct value!
  if (found == mUserRetrievedMeasurements.member().end())
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
  return getUserMeasurementUnlocked(type).at<double>(index,0);
}

cv::Mat cedar::dev::Component::getLastIOMeasurementUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeLastIOMeasurementUnlocked(type);
  auto found = mLastIORetrievedMeasurements.member().find(type);

// problem: NEED to initialize with correct value!
  if (found == mLastIORetrievedMeasurements.member().end())
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

double cedar::dev::Component::getLastIOMeasurementIndexUnlocked(ComponentDataType& type, int index) const
{
  return getLastIOMeasurementUnlocked(type).at<double>(index,0);
}


void cedar::dev::Component::setIOCommandUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeIOCommandUnlocked(type);
  mIOSubmittedCommands.member()[ type ]->setData(data);
}

cv::Mat cedar::dev::Component::getIOCommandUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeIOCommandUnlocked(type);
  auto found = mIOSubmittedCommands.member().find(type);

  if (found == mIOSubmittedCommands.member().end())
  {
    // todo throw. cannot happen
    CEDAR_ASSERT(false && "this should not happen.");
  }

  return found->second->getData();
}

void cedar::dev::Component::setIOMeasurementUnlocked(ComponentDataType& type, cv::Mat data)
{
  lazyInitializeIOMeasurementUnlocked(type);
  mIORetrievedMeasurements.member()[ type ]->setData(data);
}


cv::Mat cedar::dev::Component::getIOMeasurementUnlocked(ComponentDataType& type) const
{
  const_cast<Component*>(this)->lazyInitializeIOMeasurementUnlocked(type);
  auto found = mIORetrievedMeasurements.member().find(type);

  //!@todo Throw
  CEDAR_ASSERT(found != mIORetrievedMeasurements.member().end());
//  if (found == mIORetrievedMeasurements.end() )
//  {
//  }

  return found->second->getData();
}

cv::Mat cedar::dev::Component::integrateIO(cv::Mat data, ComponentDataType type)
{
  double timestep;

  timestep= mIOThread->getStepSize();

  QReadLocker lock(mUserRetrievedMeasurements.getLockPtr());
  return ( data * ( timestep / 1000.0 ) ) + getUserMeasurementUnlocked(type);;
}

cv::Mat cedar::dev::Component::integrateIOTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  double timestep;

  timestep= mIOThread->getStepSize();

  QReadLocker lock(mUserRetrievedMeasurements.getLockPtr());
  return ( ( data * (timestep / 1000.0 ) + getUserMeasurementUnlocked(type1) )
           * ( timestep / 1000.0 ) )
         + getUserMeasurementUnlocked(type2);
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateIO(cv::Mat data, ComponentDataType type)
{
  double timestep;

  timestep= mIOThread->getStepSize();

// todo: check locking here
  return ( data - getLastIOMeasurementUnlocked(type) )
         / ( timestep / 1000.0 );
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateIOTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  double timestep;

  timestep= mIOThread->getStepSize();

// todo: check locking here
  return ( ( data - getLastIOMeasurementUnlocked(type1) )
           / ( timestep / 1000.0 ) 
           - getLastIOMeasurementUnlocked(type2) )
         / ( timestep / 1000.0 );
}


void cedar::dev::Component::processStart()
{
  // this is the initial run:


  // todo: test that mUserCommands is empty!

  if (!mInitialUserSubmittedCommands.member().empty())
  {
     // do as if the initial user command was the user command
     QWriteLocker lock1(mUserSubmittedCommands.getLockPtr());
     
     mUserSubmittedCommands.member() = mInitialUserSubmittedCommands.member();
     lock1.unlock();
     loopIOCommands(0.0);
  }

  // get measurements (blocking!) when the thread is started ...
  loopIOMeasurements(0.0);
}

