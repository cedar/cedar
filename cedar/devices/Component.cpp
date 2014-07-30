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
#include "cedar/devices/Channel.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/auxiliaries/Timer.h"
#include "cedar/auxiliaries/MovingAverage.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>
#include <QReadLocker>
#include <QWriteLocker>

#define COMPONENT_CV_MAT_TYPE CV_64F

//----------------------------------------------------------------------------------------------------------------------
// private class
//----------------------------------------------------------------------------------------------------------------------

class cedar::dev::Component::DataCollection
{
  public:
    DataCollection()
    :
    mCommunicationErrorCount(20)
    {
    }

    void installType(cedar::dev::Component::ComponentDataType type, const std::string& name)
    {
      if (this->hasType(type))
      {
        CEDAR_THROW(DuplicateTypeException, "This type already exists.");
      }
      QWriteLocker locker(this->mInstalledTypes.getLockPtr());
      QWriteLocker locker_b(this->mInstalledNames.getLockPtr());
      mInstalledTypes.member().insert(type);
      mInstalledNames.member()[type] = name;
      cedar::aux::LockSet lock_set;
      cedar::aux::append(lock_set, mDeviceRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mDeviceSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mUserBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mPreviousDeviceBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mInitialUserSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::LockSetLocker lock_set_locker(lock_set);
      this->lazyInitializeUnlocked(mDeviceRetrievedData, type);
      this->lazyInitializeUnlocked(mDeviceSubmittedData, type);
      this->lazyInitializeUnlocked(mUserBuffer, type);
      this->lazyInitializeUnlocked(mPreviousDeviceBuffer, type);
      this->lazyInitializeUnlocked(mInitialUserSubmittedData, type);
    }

    std::string getNameForType(cedar::dev::Component::ComponentDataType type) const
    {
      QWriteLocker locker(this->mInstalledNames.getLockPtr());
      auto iter = this->mInstalledNames.member().find(type);
      if (iter == this->mInstalledNames.member().end())
      {
        CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "The requested type is not installed.");
      }
      std::string copy = iter->second;
      return copy;
    }

    cedar::dev::Component::ComponentDataType getTypeForName(const std::string& name) const
    {
      QWriteLocker locker(this->mInstalledNames.getLockPtr());
      for (auto type_name_pair : this->mInstalledNames.member())
      {
        if (type_name_pair.second == name)
        {
          auto copy = type_name_pair.first;
          return copy;
        }
      }
      CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "No type with the name \"" + name + "\" was found.");
    }

    std::set<cedar::dev::Component::ComponentDataType> getInstalledTypes() const
    {
      QReadLocker locker(this->mInstalledTypes.getLockPtr());
      auto copy = this->mInstalledTypes.member();
      return copy;
    }

    std::set<cedar::dev::Component::ComponentDataType> getInstalledTypesUnlocked() const
    {
      return this->mInstalledTypes.member();
    }

    bool hasType(const cedar::dev::Component::ComponentDataType &type) const
    {
      QReadLocker locker(this->mInstalledTypes.getLockPtr());
      bool found = (this->mInstalledTypes.member().find(type) != this->mInstalledTypes.member().end());
      return found;
    }

    cedar::aux::DataPtr getDeviceData(const cedar::dev::Component::ComponentDataType &type)
    {
      return this->getData(this->mDeviceRetrievedData, type);
    }

    void setDimensionality(cedar::dev::Component::ComponentDataType type, unsigned int dim)
    {
      cedar::aux::LockSet lock_set;
      cedar::aux::append(lock_set, mDeviceRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mDeviceSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mUserBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mPreviousDeviceBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mInitialUserSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::LockSetLocker locker(lock_set);
      mInstalledDimensions[type] = dim;

      if (this->hasType(type))
      {
        this->resetBufferUnlocked(mDeviceRetrievedData, type);
        this->resetBufferUnlocked(mDeviceSubmittedData, type);
        this->resetBufferUnlocked(mUserBuffer, type);
        this->resetBufferUnlocked(mPreviousDeviceBuffer, type);
        this->resetBufferUnlocked(mInitialUserSubmittedData, type);
      }
    }

    unsigned int getDimensionality(cedar::dev::Component::ComponentDataType type) const
    {
      auto iter = this->mInstalledDimensions.find(type);
      if (iter == this->mInstalledDimensions.end())
      {
        CEDAR_THROW(DimensionalityNotSetException, "No dimensionality set for the given type.");
      }
      return iter->second;
    }

    cv::Mat getDeviceRetrievedBufferUnlocked(ComponentDataType type)
    {
      return this->getBufferUnlocked(mDeviceRetrievedData, type);
    }

    void setDeviceRetrievedBuffer(ComponentDataType type, cv::Mat data)
    {
      this->setData(mDeviceRetrievedData, type, data);
    }

    void setDeviceRetrievedBufferUnlocked(ComponentDataType type, cv::Mat data)
    {
      this->setDataUnlocked(mDeviceRetrievedData, type, data);
    }

    cv::Mat getDeviceSubmittedBufferUnlocked(ComponentDataType type)
    {
      return this->getBufferUnlocked(mDeviceSubmittedData, type);
    }

    void setDeviceSubmittedBufferUnlocked(ComponentDataType type, cv::Mat data)
    {
      this->setDataUnlocked(mDeviceSubmittedData, type, data);
    }

    void resetDeviceRetrievedBufferUnlocked(ComponentDataType type)
    {
      this->resetBufferUnlocked(mDeviceRetrievedData, type);
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

    void setUserBuffer(ComponentDataType type, cv::Mat data)
    {
      this->setData(mUserBuffer, type, data);
    }

    void setUserBufferUnlocked(const ComponentDataType type, const cv::Mat& data)
    {
      this->setDataUnlocked(mUserBuffer, type, data);
    }

    void setUserBufferIndex(ComponentDataType type, int index, double value)
    {
      this->setDataIndex(mUserBuffer, type, index, value);
    }

    void setUserBufferIndexUnlocked(const ComponentDataType type, int index, double value)
    {
      this->setDataIndexUnlocked(mUserBuffer, type, index, value);
    }

    cv::Mat getUserBuffer(ComponentDataType type) const
    {
      return this->getBuffer(mUserBuffer, type);
    }

    cedar::aux::ConstMatDataPtr getUserData(ComponentDataType type) const
    {
      return this->getData(this->mUserBuffer, type);
    }

    cedar::aux::MatDataPtr getUserData(ComponentDataType type)
    {
      return this->getData(this->mUserBuffer, type);
    }

    double getUserBufferIndex(ComponentDataType type, int index) const
    {
      return this->getBufferIndex(mUserBuffer, type, index);
    }

    cv::Mat getUserBufferUnlocked(ComponentDataType type) const
    {
      return this->getBufferUnlocked(mUserBuffer, type);
    }

    double getUserDataIndexUnlocked(ComponentDataType type, int index) const
    {
      return this->getBufferIndexUnlocked(mUserBuffer, type, index);
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
      QWriteLocker locker(this->mTransformationHooks.getLockPtr());
      auto found = this->mTransformationHooks.member().find(from);

      if (found == this->mTransformationHooks.member().end())
      {
        this->mTransformationHooks.member()[from] = InnerTransformationHookContainerType{ {to, fun} };
      }
      else
      {
        auto found2 = (found->second).find( to );

        // there cannot be a transformation hook set already
        if (found2 != (found->second).end())
        {
          std::string from_str = this->getNameForType(from);
          std::string to_str = this->getNameForType(to);

          CEDAR_THROW
          (
            cedar::dev::Component::DuplicateTransformationHookException,
            "A transformation hook from \"" + from_str + "\" to \"" + to_str + "\" is already set."
          );
        }

        (found->second)[to] = fun;
      }
    }

    boost::optional<TransformationFunctionType> findTransformationHook(ComponentDataType from, ComponentDataType to) const
    {
      QReadLocker locker(this->mTransformationHooks.getLockPtr());
      auto found_outer = this->mTransformationHooks.member().find(from);

      if (found_outer != this->mTransformationHooks.member().end())
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

    void defineGroup(const std::string& groupName)
    {
      QWriteLocker locker(this->mGroups.getLockPtr());
      if (this->hasGroupUnlocked(groupName))
      {
        CEDAR_THROW(cedar::dev::Component::DuplicateGroupNameException, "The group name \"" + groupName + "\" already exists.");
      }
      this->mGroups.member()[groupName] = std::vector<cedar::dev::Component::ComponentDataType>();
    }

    std::vector<std::string> listGroups() const
    {
      QReadLocker locker(this->mGroups.getLockPtr());
      std::vector<std::string> groups;
      groups.reserve(this->mGroups.member().size());
      for (const auto& group_vector_pair : this->mGroups.member())
      {
        groups.push_back(group_vector_pair.first);
      }
      return groups;
    }

    void addTypeToGroup(const std::string& groupName, const ComponentDataType& commandType)
    {
      QWriteLocker locker(this->mGroups.getLockPtr());
      auto iter = this->mGroups.member().find(groupName);
      if (iter == this->mGroups.member().end())
      {
        CEDAR_THROW(GroupNameNotFoundException, "Could not find a group with the name \"" + groupName + "\".");
      }
      iter->second.push_back(commandType);
    }

    bool hasGroup(const std::string& groupName)
    {
      QReadLocker locker(this->mGroups.getLockPtr());
      bool exists = this->hasGroupUnlocked(groupName);
      return exists;
    }

    bool hasGroups() const
    {
      QReadLocker locker(this->mGroups.getLockPtr());
      bool has_groups = this->mGroups.member().size() > 0;
      return has_groups;
    }

    std::vector<ComponentDataType> getTypesInGroup(const std::string& groupName) const
    {
      QReadLocker locker(this->mGroups.getLockPtr());
      auto iter = this->mGroups.member().find(groupName);
      if (iter == this->mGroups.member().end())
      {
        CEDAR_THROW(GroupNameNotFoundException, "Could not find a group with the name \"" + groupName + "\".");
      }

      std::vector<ComponentDataType> copy = iter->second;
      return copy;
    }

    double getCommunicationErrorRate() const
    {
      QReadLocker locker(this->mCommunicationErrorCount.getLockPtr());
      double error_rate = 0.0;
      if (this->mCommunicationErrorCount.member().size() > 0)
      {
        error_rate = this->mCommunicationErrorCount.member().getAverage();
      }
      return error_rate;
    }

    std::vector<std::string> getLastErrors() const
    {
      QReadLocker locker(mLastCommunicationErrorMessages.getLockPtr());
      std::vector<std::string> errors;
      errors.assign(this->mLastCommunicationErrorMessages.member().begin(), this->mLastCommunicationErrorMessages.member().end());
      return errors;
    }

    void countSuccessfullCommunication()
    {
      QWriteLocker locker(mCommunicationErrorCount.getLockPtr());
      mCommunicationErrorCount.member().append(0.0);
    }

    void countCommunicationError(const cedar::dev::CommunicationException& exception)
    {
      // count the error
      {
        QWriteLocker locker(mCommunicationErrorCount.getLockPtr());
        mCommunicationErrorCount.member().append(1.0);
      }

      // log the error message
      {
        QWriteLocker locker(mLastCommunicationErrorMessages.getLockPtr());
        mLastCommunicationErrorMessages.member().push_back(exception.exceptionInfo());

        // remove any excess messages
        while (this->mLastCommunicationErrorMessages.member().size() > 5)
        {
          this->mLastCommunicationErrorMessages.member().pop_front();
        }
      }
    }

  private:
    bool hasGroupUnlocked(const std::string& groupName)
    {
      return this->mGroups.member().find(groupName) != this->mGroups.member().end();
    }

    cedar::aux::MatDataPtr getData(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type)
    {
      return boost::const_pointer_cast<cedar::aux::MatData>(const_cast<ConstDataCollection*>(this)->getData(bufferData, type));
    }

    cedar::aux::ConstMatDataPtr getData(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type) const
    {
      if (!this->hasType(type))
      {
        CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "The given type does not exist.");
      }

      QReadLocker locker(bufferData.getLockPtr());

      auto iter = bufferData.member().find(type);
      CEDAR_DEBUG_ASSERT(iter != bufferData.member().end());

      auto ptr_copy = iter->second;
      if (!iter->second || iter->second->getData().empty())
      {
        CEDAR_THROW(cedar::dev::Component::DimensionalityNotSetException, "The data has not yet been initialized.");
      }
      return ptr_copy;
    }

    cv::Mat getBuffer(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type) const
    {
      QReadLocker lock(bufferData.getLockPtr());

      auto ret = getBufferUnlocked(bufferData, type);
      return ret;
    }

    cv::Mat getBufferUnlocked(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type) const
    {
      if (!this->hasType(type))
      {
        CEDAR_THROW(TypeNotFoundException, "This type is not installed.");
      }
      auto found = bufferData.member().find(type);

      // problem: NEED to initialize with correct value!
      if (found == bufferData.member().end())
      {
        // todo: kann nicht passieren, throw
        // todo: DOCH, kann passieren, wenn Messung geholt wird, bevor Messung kam ...
        // todo: warning werfen, wenn letzte Messung zu lange her ...
        //!@todo this'll print a warning in case these things go wrong; replace by throw
        CEDAR_NON_CRITICAL_ASSERT(false && "This should not happen");
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
      CEDAR_DEBUG_ASSERT(getBufferUnlocked(bufferData, type).rows > index);
      return getBufferUnlocked(bufferData, type).at<double>(index, 0);
    }

    void setData(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, const cv::Mat& data)
    {
      QWriteLocker lock(bufferData.getLockPtr());
      setDataUnlocked(bufferData, type, data);
    }

    void setDataUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, const cv::Mat& data)
    {
      bufferData.member()[type]->setData(data.clone());
    }

    void setDataIndex(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, int index, double value)
    {
      QWriteLocker lock(bufferData.getLockPtr());
      setDataIndexUnlocked(bufferData, type, index, value);
    }

    void setDataIndexUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, int index, double value)
    {
      CEDAR_DEBUG_ASSERT(bufferData.member()[type]);
      bufferData.member()[type]->getData().at<double>(index, 0) = value;
    }

    void lazyInitializeUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type)
    {
      auto found = bufferData.member().find(type);

      if (found == bufferData.member().end())
      {
        bufferData.member()[type] = cedar::aux::MatDataPtr(new cedar::aux::MatData());
      }
      else // already initialized
      {
        CEDAR_THROW(AlreadyInitializedException, "Cannot initialize a buffer that is already initialized.");
      }
    }

    void resetBufferUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type)
    {
      auto found = mInstalledDimensions.find(type);

      if (found == mInstalledDimensions.end())
      {
        CEDAR_THROW
        (
          cedar::dev::Component::DimensionalityNotSetException,
          "Cannot reset buffer: no dimensionality set for \"" + this->getNameForType(type) + "\"."
        );
      }
      auto dim = found->second;
      bufferData.member()[type]->setData(cv::Mat::zeros(dim, 1, COMPONENT_CV_MAT_TYPE));
    }

  public:
    //@todo: most of these should probably be private

    //!@todo the distinction between submitted and received may not be necessary as only measurements seem to receive, and only command submit. js: YES
    cedar::aux::LockableMember<BufferDataType> mDeviceRetrievedData;

    // Cache for the Device-interface
    cedar::aux::LockableMember<BufferDataType> mDeviceSubmittedData; // was: mDeviceSubmittedCommands

    cedar::aux::LockableMember<BufferDataType> mUserBuffer; // was: mUserCommandBuffer, mUserMeasurementsBuffer

    cedar::aux::LockableMember<BufferDataType> mInitialUserSubmittedData; // was: mInitialUserSubmittedCommands

    // Cache for the user-interface
    cedar::aux::LockableMember<BufferDataType> mPreviousDeviceBuffer; // was: mPreviousDeviceMeasurementsBuffer

  private:
    std::map<ComponentDataType, unsigned int> mInstalledDimensions;

    cedar::aux::LockableMember<TransformationHookContainerType> mTransformationHooks;

    cedar::aux::LockableMember<std::set<cedar::dev::Component::ComponentDataType> > mInstalledTypes;

    cedar::aux::LockableMember<std::map<cedar::dev::Component::ComponentDataType, std::string> > mInstalledNames;

    cedar::aux::LockableMember<std::map<std::string, std::vector<cedar::dev::Component::ComponentDataType> > > mGroups;

    //! A member that contains the counts of errors for the last stepDevice* calls.
    cedar::aux::LockableMember<cedar::aux::MovingAverage<double> > mCommunicationErrorCount;

    //! The last things that went wrong during communication.
    cedar::aux::LockableMember<std::deque<std::string> > mLastCommunicationErrorMessages;
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

  // virtual can't be called in the inherit. This is why all children
  // must call it!
  //brakeNow();

  mDeviceThread->stop();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> cedar::dev::Component::getLastCommandCommunicationErrors() const
{
  return this->mCommandData->getLastErrors();
}

std::vector<std::string> cedar::dev::Component::getLastMeasurementCommunicationErrors() const
{
  return this->mMeasurementData->getLastErrors();
}

void cedar::dev::Component::getCommunicationErrorRates(double& commands, double& measurements) const
{
  commands = this->mCommandData->getCommunicationErrorRate();
  measurements = this->mMeasurementData->getCommunicationErrorRate();
}

cedar::unit::Time cedar::dev::Component::retrieveLastStepMeasurementsDuration()
{
  QReadLocker locker(this->mLastStepMeasurementsTime.getLockPtr());
  cedar::unit::Time last = this->mLastStepMeasurementsTime.member().get();
  this->mLastStepMeasurementsTime.member().reset(); // clear last measurement
  return last;
}

cedar::unit::Time cedar::dev::Component::retrieveLastStepCommandsDuration()
{
  QReadLocker locker(this->mLastStepCommandsTime.getLockPtr());
  cedar::unit::Time last = this->mLastStepCommandsTime.member().get();
  this->mLastStepCommandsTime.member().reset(); // clear last measurement
  return last;
}

bool cedar::dev::Component::hasLastStepMeasurementsDuration() const
{
  QReadLocker locker(this->mLastStepMeasurementsTime.getLockPtr());
  bool set = this->mLastStepMeasurementsTime.member().is_initialized();
  return set;
}

bool cedar::dev::Component::hasLastStepCommandsDuration() const
{
  QReadLocker locker(this->mLastStepCommandsTime.getLockPtr());
  bool set = this->mLastStepCommandsTime.member().is_initialized();
  return set;
}


void cedar::dev::Component::defineCommandGroup(const std::string& groupName)
{
  this->mCommandData->defineGroup(groupName);
}

std::vector<std::string> cedar::dev::Component::listCommandGroups() const
{
  return this->mCommandData->listGroups();
}

void cedar::dev::Component::addCommandTypeToGroup(const std::string& groupName, const ComponentDataType& commandType)
{
  this->mCommandData->addTypeToGroup(groupName, commandType);
}

bool cedar::dev::Component::hasCommandGroups() const
{
  return this->mCommandData->hasGroups();
}

std::vector<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getCommandsInGroup(const std::string& groupName) const
{
  return this->mCommandData->getTypesInGroup(groupName);
}

unsigned int cedar::dev::Component::getCommandDimensionality(ComponentDataType type) const
{
  return this->mCommandData->getDimensionality(type);
}

cedar::dev::Component::ComponentDataType cedar::dev::Component::getCommandTypeForName(const std::string& name) const
{
  return this->mCommandData->getTypeForName(name);
}

std::string cedar::dev::Component::getNameForCommandType(ComponentDataType type) const
{
  return this->mCommandData->getNameForType(type);
}

std::string cedar::dev::Component::getNameForMeasurementType(ComponentDataType type) const
{
  return this->mMeasurementData->getNameForType(type);
}

cedar::dev::Component::ComponentDataType cedar::dev::Component::getMeasurementTypeForName(const std::string& name) const
{
  return this->mMeasurementData->getTypeForName(name);
}

std::set<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getInstalledMeasurementTypes() const
{
  return this->mMeasurementData->getInstalledTypes();
}

std::set<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getInstalledCommandTypes() const
{
  return this->mCommandData->getInstalledTypes();
}

cedar::aux::DataPtr cedar::dev::Component::getMeasurementData(const ComponentDataType &type)
{
  return this->mMeasurementData->getUserData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getMeasurementData(const ComponentDataType &type) const
{
  return this->mMeasurementData->getUserData(type);
}

cedar::aux::DataPtr cedar::dev::Component::getUserCommandData(const ComponentDataType &type)
{
  return this->mCommandData->getUserData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getUserCommandData(const ComponentDataType &type) const
{
  return this->mCommandData->getUserData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getDeviceCommandData(const ComponentDataType &type) const
{
  return this->mCommandData->getDeviceData(type);
}

cedar::aux::DataPtr cedar::dev::Component::getDeviceCommandData(const ComponentDataType &type)
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

void cedar::dev::Component::installCommandType(ComponentDataType type, const std::string& name)
{
  this->mCommandData->installType(type, name);
}

void cedar::dev::Component::installMeasurementType(ComponentDataType type, const std::string& name)
{
  this->mMeasurementData->installType(type, name);
}

void cedar::dev::Component::installCommandAndMeasurementType(ComponentDataType type, const std::string& name)
{
  this->installCommandType(type, name);
  this->installMeasurementType(type, name);
}

void cedar::dev::Component::resetComponent()
{
  {
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, this->mMeasurementData->mUserBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mMeasurementData->mPreviousDeviceBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mMeasurementData->mDeviceRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockSetLocker locker(locks);

    for (auto& type : this->mMeasurementData->getInstalledTypesUnlocked())
    {
      this->mMeasurementData->resetUserBufferUnlocked(type);
      this->mMeasurementData->resetPreviousDeviceBufferUnlocked(type);
      this->mMeasurementData->resetDeviceRetrievedBufferUnlocked(type);
    }
  }

  {
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, this->mCommandData->mUserBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mCommandData->mDeviceSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockSetLocker locker(locks);

    for(auto& type : this->mCommandData->getInstalledTypes())
    {
      this->mCommandData->resetUserBufferUnlocked(type);
      this->mCommandData->resetDeviceSubmittedBufferUnlocked(type);
    }
  }
}

void cedar::dev::Component::applyDeviceCommandsAs(ComponentDataType type)
{
  if (!this->mCommandData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "The given type is not installed.");
  }
  mDeviceCommandSelection = type;
}

void cedar::dev::Component::setUserCommandBuffer(ComponentDataType type, cv::Mat data)
{
  CEDAR_DEBUG_ASSERT( mDeviceThread->isRunning() ); //@todo: change to warning

  this->checkExclusivenessOfCommand(type);
  QWriteLocker locker(this->mUserCommandUsed.getLockPtr());
  this->mCommandData->setUserBuffer(type, data);
  this->mUserCommandUsed.member().insert(type);
}

void cedar::dev::Component::setInitialUserCommandBuffer(ComponentDataType type, cv::Mat data)
{
  if (this->isRunning())
  {
    // cannot set initial commands if a component is already running
    CEDAR_THROW(AlreadyRunningException, "Cannot set initial commands if device is running.");
  }

  QWriteLocker(this->mCommandData->mInitialUserSubmittedData.getLockPtr());
  this->mCommandData->mInitialUserSubmittedData.member()[type]->setData(data.clone());
}


void cedar::dev::Component::setUserCommandBufferIndex(ComponentDataType type, int index, double value)
{
  this->checkExclusivenessOfCommand(type);
  QWriteLocker locker(this->mUserCommandUsed.getLockPtr());
  this->mCommandData->setUserBufferIndex(type, index, value);
  this->mUserCommandUsed.member().insert(type);
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
  if (!this->mCommandData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "Cannot register command hook: type is not installed.");
  }

  // cannot replace existing hook
  if (this->mSubmitCommandHooks.find(type) != this->mSubmitCommandHooks.end())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A command hook is already set for \"" + this->getNameForCommandType(type) + "\".");
  }
  // todo locks
  mSubmitCommandHooks[ type ] = fun;
}

void cedar::dev::Component::registerDeviceMeasurementHook(ComponentDataType type, MeasurementFunctionType fun)
{
  if (!this->mMeasurementData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "Cannot register measurement hook: type is not installed.");
  }

  // cannot replace existing hook
  if (this->mRetrieveMeasurementHooks.find(type) != this->mRetrieveMeasurementHooks.end())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A measurement hook is already set for \"" + this->getNameForMeasurementType(type) + "\".");
  }
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
  try
  {
    stepDeviceCommands(time);
    this->mCommandData->countSuccessfullCommunication();
  }
  catch (const cedar::dev::CommunicationException& e)
  {
    this->mCommandData->countCommunicationError(e);
  }
  try
  {
    stepDeviceMeasurements(time); // note, the post-measurements transformations also take time
    this->mMeasurementData->countSuccessfullCommunication();
  }
  catch (const cedar::dev::CommunicationException& e)
  {
    this->mMeasurementData->countCommunicationError(e);
  }
}

void cedar::dev::Component::stepDeviceCommands(cedar::unit::Time)
{
  cedar::aux::Timer timer;

  // todo: check locking in this function, forgot some stuff ...
  ComponentDataType type_for_Device, type_from_user;
  cv::Mat userData, ioData;

  QReadLocker user_command_locker(this->mUserCommandUsed.getLockPtr());


  if (this->mUserCommandUsed.member().size() == 0
      && !this->mController)
  {
    return; // this is not a problem
  }
  if (mSubmitCommandHooks.size() == 0)
  {
    // @todo: throw
    return;
  }

  if (this->mController)
  {
    type_from_user = this->mController->mBufferType;
    userData = (this->mController->mCallback)();
  }
  else // do not use Controller Callback:
  {
    // guess command type to use
    // safe only if there was only one command hook registered
    if (this->mUserCommandUsed.member().size() > 1)
    {
      std::string set_commands;
      for (const auto &what : this->mUserCommandUsed.member())
      {
        if (!set_commands.empty())
        {
          set_commands += ", ";
        }
        set_commands += this->getNameForCommandType(what);
      }
      CEDAR_THROW
      (
        CouldNotGuessCommandTypeException,
        "Could not guess the type of the command because too many commands have been set. Set commands are: " + set_commands
      );
    }

    // we know the map has exactly one entry
    type_from_user = *(this->mUserCommandUsed.member().begin());

    {
      QReadLocker lock(this->mCommandData->mUserBuffer.getLockPtr());
      userData = this->mCommandData->getUserBufferUnlocked(type_from_user).clone();
    }
  }
  user_command_locker.unlock();

  //  this->mUserCommandUsed.clear();

  // evaluate command type for Device:
  if (mDeviceCommandSelection)
  {
    type_for_Device = mDeviceCommandSelection.get();
    //  std::cout << "commands restricted to ... " << type_for_Device  << std::endl;    
  }
  else
  {
    // guess Device type to use. easy if there is only one hook
    if (mSubmitCommandHooks.size() != 1)
    {
      CEDAR_THROW
        (
         cedar::dev::Component::CouldNotGuessDeviceTypeException,
         "Could not guess device type: too many submit hooks. Please select a device type manually." 
        ); 
    }

    type_for_Device = mSubmitCommandHooks.begin()->first;
  }


  // do we need to transform the input?
  if (type_for_Device != type_from_user)
  {
    auto hook = this->mCommandData->findTransformationHook(type_from_user, type_for_Device);

    // hook must exist
    if (!hook)
    {
      std::string user_type_str = this->mCommandData->getNameForType(type_from_user);
      std::string device_type_str = this->mCommandData->getNameForType(type_for_Device);
      CEDAR_THROW(cedar::dev::Component::HookNotFoundException, "Could not find a command hook from \"" + user_type_str + "\" to \"" + device_type_str + "\".");
    }

    QReadLocker lock1(this->mMeasurementData->mUserBuffer.getLockPtr());
    // call hook
    ioData = hook.get()(userData);
  }
  else
  {
    ioData = userData.clone();
  }

  auto hook_found = mSubmitCommandHooks.find(type_for_Device);
  if (hook_found == mSubmitCommandHooks.end())
  {
    CEDAR_THROW(cedar::dev::Component::HookNotFoundException, "Could not find a submit hook for \"" + this->mMeasurementData->getNameForType(type_for_Device) + "\".");
  }

  (hook_found->second)(ioData);

  //!@todo this should never be necessary
//  this->mCommandData->mUserBuffer.member().clear();

  QWriteLocker time_locker(this->mLastStepCommandsTime.getLockPtr());
  this->mLastStepCommandsTime.member() = timer.elapsed();
}

// update the Device Cache
void cedar::dev::Component::stepDeviceMeasurements(cedar::unit::Time)
{
  cedar::aux::Timer timer;

  std::vector< ComponentDataType > types_to_transform;
  std::vector< ComponentDataType > types_we_measured;

  // lock measurements 
  QWriteLocker lock1(this->mMeasurementData->mDeviceRetrievedData.getLockPtr());

  // thinks I can get directly from HW:
  for (const auto& type : this->mMeasurementData->getInstalledTypes())
  {
//  std::cout << " test"  << type <<    std::endl;
    auto found = mRetrieveMeasurementHooks.find( type );

    if (found != mRetrieveMeasurementHooks.end())
    {
      // execute the hook:
      this->mMeasurementData->setDeviceRetrievedBufferUnlocked(type, (found->second)() );
      types_we_measured.push_back(type);
    }
    else
    {
      // store things we need to calculate afterwards
      types_to_transform.push_back(type);
    }
  }

  for (auto& missing_type : types_to_transform )
  {
    // bool nothing_found = true; //!@todo What is the purpose of this?
    for (auto& measured_type : types_we_measured )
    {
      auto hook = this->mMeasurementData->findTransformationHook(measured_type, missing_type);
      if (hook.is_initialized())
      {
        // call measurement hook
        this->mMeasurementData->mDeviceRetrievedData.member()[missing_type]->setData(
        hook.get()(this->mMeasurementData->mDeviceRetrievedData.member()[ measured_type ]->getData()));
      }
    }
  }

  lock1.unlock();
  // todo: make this non-blocking for this looped thread
  updateUserMeasurements();

  QWriteLocker time_locker(this->mLastStepMeasurementsTime.getLockPtr());
  this->mLastStepMeasurementsTime.member() = timer.elapsed();
}

void cedar::dev::Component::updateUserMeasurements()
{
  // lock Device cache todo
  // lock caches
  cedar::aux::LockSet locks;
  cedar::aux::append(locks, this->mMeasurementData->mPreviousDeviceBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, this->mMeasurementData->mUserBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, this->mMeasurementData->mDeviceRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::LockSetLocker locker(locks);

  // todo: are these really deep copies? -> no, mDeviceRetrievedMeasurements contains data ptrs
  for (auto type : this->mMeasurementData->getInstalledTypes())
  {
    this->mMeasurementData->mPreviousDeviceBuffer.member()[type]->getData() = this->mMeasurementData->mUserBuffer.member()[type]->getData().clone();
    this->mMeasurementData->mUserBuffer.member()[type]->getData() = this->mMeasurementData->mDeviceRetrievedData.member()[type]->getData().clone();
    this->mMeasurementData->mDeviceRetrievedData.member()[type]->getData() = 0.0;
  }

  locker.unlock();

  emit updatedUserMeasurementSignal();
}

void cedar::dev::Component::startDevice()
{
  // do not re-enter, do not stop/start at the same time
  QMutexLocker lockerGeneral(&mGeneralAccessLock);

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
  // do not re-enter, do not stop/start at the same time
  QMutexLocker lockerGeneral(&mGeneralAccessLock);

  brakeNow(); // this will wait for one step and need to access the Mutex below 

  mDeviceThread->requestStop(); // stop more quickly

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
//  std::cout << "Integrate once!" << std::endl;
//  std::cout << data << std::endl;
//  std::cout << this->mMeasurementData->getUserBufferUnlocked(type) << std::endl;
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);
//  std::cout << unitless << std::endl;
//  std::cout << "Integrate once (FIN!)!" << std::endl;
  QReadLocker lock(this->mMeasurementData->mUserBuffer.getLockPtr());
  //!@todo check if this uses the right time step to integrate
  cv::Mat result = data * unitless + this->mMeasurementData->getUserBufferUnlocked(type);
  return result;
}

cv::Mat cedar::dev::Component::integrateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
//    std::cout << "Integrate twice!" << type1 << " " << type2 << std::endl;
//    std::cout << data << std::endl;
//    std::cout << this->mMeasurementData->getUserBufferUnlocked(type1) << std::endl;
//    std::cout << this->mMeasurementData->getUserBufferUnlocked(type2) << std::endl;
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);
//    std::cout << unitless << std::endl;
//    std::cout << "Integrate twice (FIN!)!" << std::endl;
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
//  std::cout << "Differentiate once!" << std::endl;
//  std::cout << data << std::endl;
//  std::cout << this->mMeasurementData->getUserBufferUnlocked(type) << std::endl;
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

//  std::cout << unitless << std::endl;
//  std::cout << "Differentiate once (FIN!)!" << std::endl;
// todo: check locking here
  //!@todo check if this uses the right time step to differentiate
  return ( data - this->mMeasurementData->getUserBufferUnlocked(type) )
         / unitless;
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateDeviceTwice(cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
  cedar::unit::Time timestep = mDeviceThread->getStepSize();
  double unitless = timestep / (1.0 * cedar::unit::second);

// todo: check locking here
  //!@todo check if this uses the right time step to differentiate
  cv::Mat result = (( data - this->mMeasurementData->getUserBufferUnlocked(type1) )  / unitless
      - this->mMeasurementData->getUserBufferUnlocked(type2) ) / unitless;
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

void cedar::dev::Component::clearUserCommand()
{
  QWriteLocker user_command_locker(this->mUserCommandUsed.getLockPtr());
  this->mUserCommandUsed.member().clear();
}

void cedar::dev::Component::checkExclusivenessOfCommand(ComponentDataType type)
{
  QReadLocker user_command_locker(this->mUserCommandUsed.getLockPtr());
  if (this->mUserCommandUsed.member().size() > 0)
  {
    if (this->mUserCommandUsed.member().find(type) == this->mUserCommandUsed.member().end())
    {
      // a different command type is already set, throw!
      CEDAR_THROW(CouldNotGuessCommandTypeException, "You used more than one type of commands. Component cannot handle this.");
    }
  }
}

void cedar::dev::Component::startBraking()
{
  clearUserCommand();
  clearController();
  if (!applyBrakeController())
  {
    //@todo try again and then default to brakeNow()
    brakeNow();
  }
}

void cedar::dev::Component::brakeNow()
{
  if (!isRunning())
    return; // well ... nothing do, right?

  clearUserCommand();
  clearController();

  if (!applyBrakeNow())
  {
    //@todo: wait short time, try again and then panic TODO TODO
  }
  else
  {
    CEDAR_DEBUG_ASSERT(mDeviceThread->isRunning());
   
    // force sending the command
    mDeviceThread->waitUntilStepped();

    // paranoid:
    clearUserCommand(); 
    clearController();

    // TODO: set lock on all incoming commands and controllers
    // TODO: we also need to test if the vel measurements are 0
  }
}

void cedar::dev::Component::crashbrake()
{
  applyCrashbrake();
}

bool cedar::dev::Component::applyCrashbrake()
{
  clearUserCommand();
  clearController();
  if (!applyBrakeNow()) // dummy default
  {
    // @todo: panic
  }

  return true;
}

void cedar::dev::Component::clearController()
{
  mController.reset();  
}

void cedar::dev::Component::setController(ComponentDataType type, cedar::dev::Component::ControllerCallback fun)
{
  mController= ControllerCollectionPtr( new cedar::dev::Component::ControllerCollection{ type, fun } );
}

void cedar::dev::Component::waitUntilCommunicated() const
{
  CEDAR_ASSERT( mDeviceThread );
  mDeviceThread->waitUntilStepped();

  // include any waiting for synchronous responses here ...
}

