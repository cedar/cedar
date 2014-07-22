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

//!@todo Replace asserts by proper exceptions.

//----------------------------------------------------------------------------------------------------------------------
// private class
//----------------------------------------------------------------------------------------------------------------------

class cedar::dev::Component::DataCollection
{
  public:
    void installType(cedar::dev::Component::ComponentDataType type)
    {
      QWriteLocker locker(this->mInstalledTypes.getLockPtr());
      // lazyInitialize whether already registered and throw TODO
      mInstalledTypes.member().push_back(type);
    }

    std::vector<cedar::dev::Component::ComponentDataType> getInstalledTypes() const
    {
      QReadLocker locker(this->mInstalledTypes.getLockPtr());
      auto copy = this->mInstalledTypes.member();
      return copy;
    }

    std::vector<cedar::dev::Component::ComponentDataType> getInstalledTypesUnlocked() const
    {
      return this->mInstalledTypes.member();
    }

    cedar::aux::DataPtr getDeviceData(const cedar::dev::Component::ComponentDataType &type)
    {
      //!@todo Check that the measurement type exists
      QReadLocker locker(this->mDeviceRetrievedData.getLockPtr());
      this->lazyInitializeDeviceBufferUnlocked(type);

      auto iter = this->mDeviceRetrievedData.member().find(type);
      CEDAR_DEBUG_ASSERT(iter != this->mDeviceRetrievedData.member().end());

      auto ptr_copy = iter->second;
      return ptr_copy;
    }

    void setDimensionality(cedar::dev::Component::ComponentDataType type, unsigned int dim)
    {
      // todo: locking
      // todo: lazyInitialize if already registered type

      mInstalledDimensions[type] = dim;
    }

    cv::Mat getDeviceRetrievedBufferUnlocked(ComponentDataType& type)
    {
      return this->getBufferUnlocked(mDeviceRetrievedData, type);
    }

    void setDeviceRetrievedBuffer(ComponentDataType& type, cv::Mat data)
    {
      this->setData(mDeviceRetrievedData, type, data);
    }

    void setDeviceRetrievedBufferUnlocked(ComponentDataType& type, cv::Mat data)
    {
      this->setDataUnlocked(mDeviceRetrievedData, type, data);
    }

    void resetDeviceRetrievedBufferUnlocked(ComponentDataType type)
    {
      this->resetBufferUnlocked(mDeviceRetrievedData, type);
    }

    cv::Mat getDeviceSubmittedBufferUnlocked(ComponentDataType& type)
    {
      return this->getBufferUnlocked(mDeviceSubmittedData, type);
    }

    void setDeviceSubmittedBufferUnlocked(ComponentDataType& type, cv::Mat data)
    {
      this->setDataUnlocked(mDeviceSubmittedData, type, data);
    }

    void resetDeviceSubmittedBufferUnlocked(ComponentDataType type)
    {
      this->resetBufferUnlocked(mDeviceSubmittedData, type);
    }

    void resetUserBufferUnlocked(cedar::dev::Component::ComponentDataType type)
    {
      this->resetBufferUnlocked(mUserBuffer, type);
    }

    void resetPreviousDeviceBufferUnlocked(ComponentDataType type)
    {
      this->resetBufferUnlocked(mPreviousDeviceBuffer, type);
    }

    void lazyInitializeUserBufferUnlocked(ComponentDataType type)
    {
      this->lazyInitializeUnlocked(mUserBuffer, type);
    }

    void lazyInitializeDeviceBufferUnlocked(ComponentDataType type)
    {
      this->lazyInitializeUnlocked(mDeviceSubmittedData, type);
    }

    void lazyInitializePreviousDeviceBufferUnlocked(ComponentDataType type)
    {
      this->lazyInitializeUnlocked(mPreviousDeviceBuffer, type);
    }

    void setUserBuffer(ComponentDataType type, cv::Mat data)
    {
      this->setData(mUserBuffer, type, data);
    }

    void setUserBufferUnlocked(const ComponentDataType& type, const cv::Mat& data)
    {
      this->setDataUnlocked(mUserBuffer, type, data);
    }

    void setUserBufferIndex(ComponentDataType type, int index, double value)
    {
      this->setDataIndex(mUserBuffer, type, index, value);
    }

    void setUserBufferIndexUnlocked(const ComponentDataType& type, int index, double value)
    {
      this->setDataIndexUnlocked(mUserBuffer, type, index, value);
    }

    cv::Mat getUserBuffer(ComponentDataType type) const
    {
      return this->getBuffer(mUserBuffer, type);
    }

    double getUserBufferIndex(ComponentDataType type, int index) const
    {
      return this->getBufferIndex(mUserBuffer, type, index);
    }

    cv::Mat getUserBufferUnlocked(ComponentDataType& type) const
    {
      return this->getBufferUnlocked(mUserBuffer, type);
    }

    double getUserDataIndexUnlocked(ComponentDataType& type, int index) const
    {
      return getBufferIndexUnlocked(mUserBuffer, type, index);
    }

    cv::Mat getPreviousDeviceBuffer(ComponentDataType type) const
    {
      return this->getBuffer(mPreviousDeviceBuffer, type);
    }

    cv::Mat getPreviousDeviceBufferUnlocked(ComponentDataType type) const
    {
      return this->getBufferUnlocked(mPreviousDeviceBuffer, type);
    }

    double getPreviousDeviceBufferIndex(ComponentDataType type, int index) const
    {
      return this->getBufferIndex(mPreviousDeviceBuffer, type, index);
    }

    double getPreviousDeviceBufferIndexUnlocked(ComponentDataType type, int index) const
    {
      return this->getBufferIndexUnlocked(mPreviousDeviceBuffer, type, index);
    }

    void registerTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
    {
      // todo checks
      // todo locks


      auto found = this->mTransformationHooks.find( from );

      if (found == this->mTransformationHooks.end())
      {
        this->mTransformationHooks[ from ] = InnerTransformationHookContainerType{ {to, fun} };
      }
      else
      {
        auto found2 = (found->second).find( to );

        // there cannot be a transformation hook set already
        CEDAR_ASSERT(found2 == (found->second).end());

        (found->second)[ to ] = fun;
      }
      //  std::cout << "registered user command trafo from " << from << " to: " << to << std::endl;
    }

    boost::optional<TransformationFunctionType> findTransformationHook(ComponentDataType from, ComponentDataType to) const
    {
      auto found_outer = this->mTransformationHooks.find(from);

      if (found_outer != this->mTransformationHooks.end())
      {
        const auto& inner = found_outer->second;
        auto found_inner = inner.find(to);

        if (found_inner != inner.end())
        {
          return found_inner->second;
        }
      }

      // if one of the maps doesn't contain one of the keys, return an empty value
      return boost::optional<TransformationFunctionType>();
    }

  private:
    cv::Mat getBuffer(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type) const
    {
      QReadLocker lock(bufferData.getLockPtr());

      auto ret = getBufferUnlocked(bufferData, type);
      return ret;
    }

    cv::Mat getBufferUnlocked(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType& type) const
    {
      //!@todo CONST CAST! CONST CAST! CONST CAST! CONST CAST! CONST CAST! CONST CAST! CONST CAST! CONST CAST! CONST CAST! Mutti there is a CONST CAST here!
      const_cast<DataCollection*>(this)->lazyInitializeUnlocked(const_cast<cedar::aux::LockableMember<BufferDataType>&>(bufferData), type);
      auto found = bufferData.member().find(type);

    // problem: NEED to initialize with correct value!
      if (found == bufferData.member().end())
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

    double getBufferIndex(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type, int index) const
    {
      QReadLocker lock(bufferData.getLockPtr());

      auto ret = getBufferIndexUnlocked(bufferData, type, index);
      return ret;
    }

    double getBufferIndexUnlocked(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type, int index) const
    {
      return getBufferUnlocked(bufferData, type).at<double>(index, 0);
    }

    void setData(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType& type, const cv::Mat& data)
    {
      // todo: lazyInitialize for command restrictions

      // todo: issue a console-warning something if Device is not running

      QWriteLocker lock(bufferData.getLockPtr());

      setDataUnlocked(bufferData, type, data);
    }

    void setDataUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType& type, const cv::Mat& data)
    {
      lazyInitializeUnlocked(bufferData, type);
      bufferData.member()[type]->setData(data.clone());
    }

    void setDataIndex(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType& type, int index, double value)
    {
      // todo: lazyInitialize for command restrictions

      // todo: issue a console-warning something if Device is not running

      QWriteLocker lock(bufferData.getLockPtr());

      setDataIndexUnlocked(bufferData, type, index, value);
    }

    void setDataIndexUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType& type, int index, double value)
    {
      lazyInitializeUnlocked(bufferData, type);
      bufferData.member()[type]->getData().at<double>(index, 0) = value;
    }

    void lazyInitializeUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type)
    {
      auto found = bufferData.member().find(type);

      if (found == bufferData.member().end() || found->second->getData().empty())
      {
        // cast away const for lazy init to work
        resetBufferUnlocked(bufferData, type);
      }
    }

    void resetBufferUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type)
    {
      auto found = mInstalledDimensions.find(type);

      CEDAR_ASSERT(found != mInstalledDimensions.end());
      auto dim = found->second;

      bufferData.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
    }

  public:
    //!@todo the distinction between submitted and received may not be necessary as only measurements seem to receive, and only command submit
    cedar::aux::LockableMember<BufferDataType> mDeviceRetrievedData;

    // Cache for the Device-interface
    cedar::aux::LockableMember<BufferDataType> mDeviceSubmittedData; // was: mDeviceSubmittedCommands

    std::map<ComponentDataType, unsigned int> mInstalledDimensions;

    cedar::aux::LockableMember<BufferDataType> mUserBuffer; // was: mUserCommandBuffer, mUserMeasurementsBuffer

    decltype(mUserBuffer) mInitialUserSubmittedData; // was: mInitialUserSubmittedCommands

    // Cache for the user-interface
    cedar::aux::LockableMember< BufferDataType > mPreviousDeviceBuffer; // was: mPreviousDeviceMeasurementsBuffer

    TransformationHookContainerType mTransformationHooks;

  private:
    cedar::aux::LockableMember<std::vector<cedar::dev::Component::ComponentDataType> > mInstalledTypes;
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::Component::init()
{
  this->mMeasurementData = cedar::dev::Component::DataCollectionPtr(new cedar::dev::Component::DataCollection());
  this->mCommandData = cedar::dev::Component::DataCollectionPtr(new cedar::dev::Component::DataCollection());
  mDeviceThread = std::unique_ptr<cedar::aux::LoopFunctionInThread>(
                                new cedar::aux::LoopFunctionInThread(
                                  boost::bind(&cedar::dev::Component::stepDevice,
                                              this,
                                              _1) ));


  mDeviceThread->connectToStartSignal(boost::bind(&cedar::dev::Component::processStart, this));
  mDeviceThread->setStepSize(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
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

std::vector<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getInstalledMeasurementTypes() const
{
  return this->mMeasurementData->getInstalledTypes();
}

std::vector<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getInstalledCommandTypes() const
{
  return this->mCommandData->getInstalledTypes();
}

cedar::aux::DataPtr cedar::dev::Component::getDeviceMeasurementData(const ComponentDataType &type) //!@todo Const?
{
  return this->mMeasurementData->getDeviceData(type);
}

cedar::aux::DataPtr cedar::dev::Component::getDeviceCommandData(const ComponentDataType &type) //!@todo Const?
{
  return this->mCommandData->getDeviceData(type);
}

void cedar::dev::Component::setCommandDimensionality(ComponentDataType type, unsigned int dim)
{
  this->mCommandData->setDimensionality(type, dim);
}

void cedar::dev::Component::setMeasurementDimensionality(ComponentDataType type, unsigned int dim)
{
  this->mMeasurementData->setDimensionality(type, dim);
}

void cedar::dev::Component::setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim)
{
  setCommandDimensionality(type, dim);
  setMeasurementDimensionality(type, dim);
}

void cedar::dev::Component::installCommandType(ComponentDataType type)
{
  this->mCommandData->installType(type);
}

void cedar::dev::Component::installMeasurementType(ComponentDataType type)
{
  this->mCommandData->installType(type);
}

void cedar::dev::Component::installCommandAndMeasurementType(ComponentDataType type)
{
  installCommandType(type);
  installMeasurementType(type);
}

void cedar::dev::Component::resetComponent()
{
  {
    //!@todo As this isn't locked in a canonical lock order, this may lead to deadlocks; use cedar::aux::Lockable?
    //!@todo This can be done using cedar::aux::LockSet, but we must write a LockSetLocker first
    QWriteLocker lock2(this->mMeasurementData->mUserBuffer.getLockPtr());
    QWriteLocker lock2b(this->mMeasurementData->mPreviousDeviceBuffer.getLockPtr());
    QWriteLocker lock4(this->mMeasurementData->mDeviceRetrievedData.getLockPtr());

    for (ComponentDataType &type : this->mMeasurementData->getInstalledTypesUnlocked())
    {
      this->mMeasurementData->resetUserBufferUnlocked(type);
      this->mMeasurementData->resetPreviousDeviceBufferUnlocked(type);
      this->mMeasurementData->resetDeviceRetrievedBufferUnlocked(type);
    }
  }

  {
    QWriteLocker lock1(this->mCommandData->mUserBuffer.getLockPtr());
    QWriteLocker lock3(this->mCommandData->mDeviceSubmittedData.getLockPtr());

    for(ComponentDataType &type : this->mCommandData->getInstalledTypes())
    {
      this->mCommandData->resetUserBufferUnlocked(type);
      this->mCommandData->resetDeviceSubmittedBufferUnlocked(type);
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
  this->mCommandData->setUserBuffer(type, data);
}

void cedar::dev::Component::setInitialUserCommandBuffer(ComponentDataType type, cv::Mat data)
{
  // todo: lazyInitialize for command restrictions

  // Device is not meant to be running
  // todo: throw if already running!
  // todo: throw if not empty

  // todo: there is no lock here. I think we dont need one, sure?

  this->mCommandData->mInitialUserSubmittedData.member()[type]->setData(data.clone());
}


void cedar::dev::Component::setUserCommandBufferIndex(ComponentDataType type, int index, double value)
{
  this->mCommandData->setUserBufferIndex(type, index, value);
}

cv::Mat cedar::dev::Component::getUserMeasurementBuffer(ComponentDataType type) const
{
  return this->mMeasurementData->getUserBuffer(type);
}

double cedar::dev::Component::getUserMeasurementBufferIndex(ComponentDataType type, int index) const
{
  return this->mMeasurementData->getUserBufferIndex(type, index);
}

cv::Mat cedar::dev::Component::getPreviousDeviceMeasurementBuffer(ComponentDataType type) const
{
  return this->mMeasurementData->getPreviousDeviceBuffer(type);
}

double cedar::dev::Component::getPreviousDeviceMeasurementBufferIndex(ComponentDataType type, int index) const
{
  return this->mMeasurementData->getPreviousDeviceBufferIndex(type, index);
}

void cedar::dev::Component::registerDeviceCommandHook(ComponentDataType type, CommandFunctionType fun)
{
  // cannot replace existing hook
  CEDAR_ASSERT(this->mSubmitCommandHooks.find(type) == this->mSubmitCommandHooks.end());
  // todo locks
  mSubmitCommandHooks[ type ] = fun;
}

void cedar::dev::Component::registerDeviceMeasurementHook(ComponentDataType type, MeasurementFunctionType fun)
{
  // cannot replace existing hook
  CEDAR_ASSERT(this->mRetrieveMeasurementHooks.find(type) == this->mRetrieveMeasurementHooks.end());
  // todo locks
  mRetrieveMeasurementHooks[ type ] = fun;
}

void cedar::dev::Component::registerUserCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
{
  this->mCommandData->registerTransformationHook(from, to, fun);
}

void cedar::dev::Component::registerDeviceMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
{
  this->mMeasurementData->registerTransformationHook(from, to, fun);
}

void cedar::dev::Component::stepDevice(cedar::unit::Time time)
{
  // its important to get the currently scheduled commands out first
  // (think safety first). this assumes serial communication, of course
  stepDeviceCommands(time);
  stepDeviceMeasurements(time); // note, the post-measurements transformations also take time
}

void cedar::dev::Component::stepDeviceCommands(cedar::unit::Time)
{
#if 1
  // todo: check locking in this function, forgot some stuff ...
  ComponentDataType type_for_Device, type_from_user;

  if (this->mCommandData->mUserBuffer.member().size() == 0)
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
    if (this->mCommandData->mUserBuffer.member().size() > 1)
    {
        // todo: throw
      std::cout << "error: setted too many commands of different types! ... " <<  std::endl;
      for (auto &what : this->mCommandData->mUserBuffer.member() )
      {
        std::cout << " " << what.first << std::endl;
      }
      return;
    }

    // we know the map has exactly one entry
    type_from_user = this->mCommandData->mUserBuffer.member().begin()->first;
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
    CEDAR_ASSERT(mSubmitCommandHooks.size() == 1);

    type_for_Device = mSubmitCommandHooks.begin()->first;
  }

  cv::Mat userData, ioData;
  {
    QReadLocker lock(this->mCommandData->mUserBuffer.getLockPtr());
    userData = this->mCommandData->getUserBufferUnlocked( type_from_user ).clone();
  }

  // do we need to transform the input?
  if (type_for_Device != type_from_user)
  {
    auto hook = this->mCommandData->findTransformationHook(type_from_user, type_for_Device);

    // hook must exist
    CEDAR_ASSERT(hook);

    QReadLocker lock1(this->mMeasurementData->mUserBuffer.getLockPtr());
    // call hook
    ioData = hook.get()(userData);
  }
  else
  {
    ioData = userData.clone();
  }

  auto hook_found = mSubmitCommandHooks.find(type_for_Device);
  CEDAR_ASSERT (hook_found != mSubmitCommandHooks.end());

  (hook_found->second)(ioData);
#endif

  this->mCommandData->mUserBuffer.member().clear();
}

// update the Device Cache
void cedar::dev::Component::stepDeviceMeasurements(cedar::unit::Time)
{
  std::vector< ComponentDataType > types_to_transform;
  std::vector< ComponentDataType > types_we_measured;;

  // lock measurements 
  QWriteLocker lock1(this->mMeasurementData->mDeviceRetrievedData.getLockPtr());

  //!@todo What was the purpose of this clear? reimplement
//  mDeviceRetrievedMeasurements.member().clear();

  // thinks I can get directly from HW:
  for (const auto& type : this->mMeasurementData->getInstalledTypes())
  {
//  std::cout << " test"  << type <<    std::endl;
    auto found = mRetrieveMeasurementHooks.find( type );

    if (found != mRetrieveMeasurementHooks.end())
    {
      // execute the hook:
      this->mMeasurementData->mDeviceRetrievedData.member()[ type ]->setData((found->second)());
      types_we_measured.push_back( type );
//std::cout << "we measured a tyep for: " << type << std::endl;      
//std::cout << "  it was: " <<  this->mMeasurementData->mDeviceRetrievedData[ type ] << std::endl;
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
      auto hook = this->mMeasurementData->findTransformationHook(measured_type, missing_type);
      if (hook.is_initialized())
      {
        // call measurement hook
        hook.get()(this->mMeasurementData->mDeviceRetrievedData.member()[ measured_type ]->getData());
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
  QReadLocker lock1(this->mMeasurementData->mPreviousDeviceBuffer.getLockPtr());
  QWriteLocker lock2(this->mMeasurementData->mUserBuffer.getLockPtr());
  QWriteLocker lock3(this->mMeasurementData->mDeviceRetrievedData.getLockPtr());

  // todo: are these really deep copies? -> no, mDeviceRetrievedMeasurements contains data ptrs
  this->mMeasurementData->mPreviousDeviceBuffer.member() = this->mMeasurementData->mUserBuffer.member();
  this->mMeasurementData->mUserBuffer.member() = this->mMeasurementData->mDeviceRetrievedData.member();
  //!@todo What was the purpose of this clear? reimplement
//  this->mMeasurementData->mDeviceRetrievedData.member().clear();

  lock1.unlock();
  lock2.unlock();

  emit updatedUserMeasurementSignal();
}

void cedar::dev::Component::startDevice()
{
  if (this->mCommandData->getInstalledTypes().empty() && this->mMeasurementData->getInstalledTypes().empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "No commands or measurements set in device",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

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

cedar::unit::Time cedar::dev::Component::getDeviceStepSize()
{
  return mDeviceThread->getStepSize();
}

void cedar::dev::Component::setStepSize(const cedar::unit::Time& time)
{
  mDeviceThread->setStepSize(time);
}

void cedar::dev::Component::setIdleTime(const cedar::unit::Time& time)
{
  mDeviceThread->setIdleTime(time);
}

void cedar::dev::Component::setSimulatedTime(const cedar::unit::Time& time)
{
  mDeviceThread->setSimulatedTime(time);
}

bool cedar::dev::Component::isRunning()
{
  return mDeviceThread->isRunning();
}

bool cedar::dev::Component::isRunningNolocking()
{
  return mDeviceThread->isRunningNolocking();
}

void cedar::dev::Component::startTimer(double)
{
  // this does nothing. think
}

void cedar::dev::Component::stopTimer()
{
}

cv::Mat cedar::dev::Component::integrateDevice(cv::Mat data, ComponentDataType type)
{
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

  QReadLocker lock(this->mMeasurementData->mUserBuffer.getLockPtr());
  //!@todo check if this uses the right time step to integrate
  cv::Mat result = data * unitless + this->mMeasurementData->getUserBufferUnlocked(type);
  return result;
}

cv::Mat cedar::dev::Component::integrateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

  QReadLocker lock(this->mMeasurementData->mUserBuffer.getLockPtr());
  //!@todo check if this uses the right time step to integrate
  cv::Mat result = ( ( data * unitless + this->mMeasurementData->getUserBufferUnlocked(type1) )
      * unitless )
    + this->mMeasurementData->getUserBufferUnlocked(type2);
  return result;
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateDevice(cv::Mat data, ComponentDataType type)
{
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

// todo: check locking here
  //!@todo check if this uses the right time step to differentiate
  return ( data - this->mMeasurementData->getPreviousDeviceBufferUnlocked(type) )
         / unitless;
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

// todo: check locking here
  //!@todo check if this uses the right time step to differentiate
  cv::Mat result = (( data - this->mMeasurementData->getPreviousDeviceBufferUnlocked(type1) )  / unitless
      - this->mMeasurementData->getPreviousDeviceBufferUnlocked(type2) ) / unitless;
  return result;
}


void cedar::dev::Component::processStart()
{
  // this is the initial run:


  // todo: test that mUserCommands is empty!
  cedar::unit::Time time = 0.0 * cedar::unit::seconds;
  if (!this->mCommandData->mInitialUserSubmittedData.member().empty())
  {
     // do as if the initial user command was the user command
     QWriteLocker lock1(this->mCommandData->mUserBuffer.getLockPtr());
     
     this->mCommandData->mUserBuffer.member() = this->mCommandData->mInitialUserSubmittedData.member();
     lock1.unlock();
     stepDeviceCommands(time);
  }

  // get measurements (blocking!) when the thread is started ...
  stepDeviceMeasurements(time);
}

