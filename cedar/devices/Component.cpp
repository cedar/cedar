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

    File:        Component.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.rub.de
    Date:        2012 11 26

    Description: Abstract component of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/devices/Channel.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/auxiliaries/Timer.h"
#include "cedar/auxiliaries/MovingAverage.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "boost/lexical_cast.hpp"
#include <QReadLocker>
#include <QWriteLocker>


#define COMPONENT_CV_MAT_TYPE CV_32F

//----------------------------------------------------------------------------------------------------------------------
// private class
//----------------------------------------------------------------------------------------------------------------------

class cedar::dev::Component::DataCollection
{
  public:
    DataCollection():
    mCommunicationErrorCount(20)
    {
    }

    virtual ~DataCollection()
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
      this->lazyInitializeMembers(type);
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

    void setDimensionality(cedar::dev::Component::ComponentDataType type, DimensionalityType dim, int matrixType)
    {      
      mInstalledDimensions[type] = dim;

      this->resetBuffers(type, matrixType);
    }

    cedar::dev::Component::DimensionalityType getDimensionality(cedar::dev::Component::ComponentDataType type) const
    {
      auto iter = this->mInstalledDimensions.find(type);
      if (iter == this->mInstalledDimensions.end())
      {
        CEDAR_THROW(DimensionalityNotSetException, "No dimensionality set for the given type.");
      }
      return iter->second;
    }

    void resetUserSideBufferUnlocked(cedar::dev::Component::ComponentDataType type, int matrixType)
    {
      this->resetBufferUnlocked(mUserSideBuffer, type, matrixType);
    }

    void resetPreviousDeviceSideBufferUnlocked(ComponentDataType type,int matrixType)
    {
      this->resetBufferUnlocked(mPreviousDeviceSideBuffer, type, matrixType);
    }

    void setUserSideBuffer(ComponentDataType type, cv::Mat data)
    {
      // reference to the old buffer matrix
      cv::Mat &buffer = mUserSideBuffer.member()[type]->getData();

      // get size and type of old buffer
      auto &buff_size = buffer.size;
      const int buff_type = buffer.type();

      if(data.size == buff_size && data.type() == buff_type)
      {
        this->setData(mUserSideBuffer, type, data);
      }
      else
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          "New buffer object has wrong size or type. Old Buffer Size: "+boost::lexical_cast<std::string>(buffer.rows)+","+boost::lexical_cast<std::string>(buffer.cols)+" New Size: "+boost::lexical_cast<std::string>(data.rows)+","+boost::lexical_cast<std::string>(data.cols)+". Old Buffer Type: "+boost::lexical_cast<std::string>(buff_type)+ " New Type: "+ boost::lexical_cast<std::string>(data.type()),
          CEDAR_CURRENT_FUNCTION_NAME);
      }
    }

    void setUserSideBufferUnlocked(const ComponentDataType type, const cv::Mat& data)
    {
      this->setDataUnlocked(mUserSideBuffer, type, data);
    }

    void setUserSideBufferIndex(ComponentDataType type, int index, float value)
    {
      this->setDataIndex(mUserSideBuffer, type, index, value);
    }

    void setUserSideBufferIndexUnlocked(const ComponentDataType type, int index, float value)
    {
      this->setDataIndexUnlocked(mUserSideBuffer, type, index, value);
    }

    cv::Mat getUserSideBuffer(ComponentDataType type) const
    {
      return this->getBuffer(mUserSideBuffer, type);
    }

    cedar::aux::ConstMatDataPtr getUserSideData(ComponentDataType type) const
    {
      return this->getData(this->mUserSideBuffer, type);
    }

    cedar::aux::MatDataPtr getUserSideData(ComponentDataType type)
    {
      return this->getData(this->mUserSideBuffer, type);
    }

    float getUserSideBufferIndex(ComponentDataType type, int index) const
    {
      return this->getBufferIndex(mUserSideBuffer, type, index);
    }

    cv::Mat getUserSideBufferUnlocked(ComponentDataType type) const
    {
      return this->getBufferUnlocked(mUserSideBuffer, type);
    }

    float getUserSideDataIndexUnlocked(ComponentDataType type, int index) const
    {
      return this->getBufferIndexUnlocked(mUserSideBuffer, type, index);
    }

    cv::Mat getPreviousDeviceSideBuffer(ComponentDataType type) const
    {
      return this->getBuffer(mPreviousDeviceSideBuffer, type);
    }

    cv::Mat getPreviousDeviceSideBufferUnlocked(ComponentDataType type) const
    {
      return this->getBufferUnlocked(mPreviousDeviceSideBuffer, type);
    }

    float getPreviousDeviceSideBufferIndex(ComponentDataType type, int index) const
    {
      return this->getBufferIndex(mPreviousDeviceSideBuffer, type, index);
    }

    float getPreviousDeviceSideBufferIndexUnlocked(ComponentDataType type, int index) const
    {
      return this->getBufferIndexUnlocked(mPreviousDeviceSideBuffer, type, index);
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

    void defineUserSelectableCommandTypeSubset(const std::string& groupName)
    {
      QWriteLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      if (this->hasUserSelectableCommandTypeSubsetUnlocked(groupName))
      {
        CEDAR_THROW(cedar::dev::Component::DuplicateUserSelectableCommandTypeSubsetNameException, "The group name \"" + groupName + "\" already exists.");
      }
      this->mUserSelectableCommandTypeSubsets.member()[groupName] = std::vector<cedar::dev::Component::ComponentDataType>();
    }

    void setActiveUserSelectableCommandTypeSubset(const std::string& groupName)
    {
      this->mActiveUserSelectableCommandTypeSubset= groupName;
    }

    std::vector<std::string> listUserSelectableCommandTypeSubsets() const
    {
      QReadLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      std::vector<std::string> groups;
      groups.reserve(this->mUserSelectableCommandTypeSubsets.member().size());
      for (const auto& group_vector_pair : this->mUserSelectableCommandTypeSubsets.member())
      {
        groups.push_back(group_vector_pair.first);
      }
      return groups;
    }

    void addTypeToUserSelectableCommandTypeSubset(const std::string& groupName, const ComponentDataType& commandType)
    {
      QWriteLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      auto iter = this->mUserSelectableCommandTypeSubsets.member().find(groupName);
      if (iter == this->mUserSelectableCommandTypeSubsets.member().end())
      {
        CEDAR_THROW(UserSelectableCommandTypeSubsetNameNotFoundException, "Could not find a group with the name \"" + groupName + "\".");
      }
      iter->second.push_back(commandType);
    }

    bool hasUserSelectableCommandTypeSubset(const std::string& groupName)
    {
      QReadLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      bool exists = this->hasUserSelectableCommandTypeSubsetUnlocked(groupName);
      return exists;
    }

    bool hasUserSelectableCommandTypeSubsets() const
    {
      QReadLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      bool has_groups = this->mUserSelectableCommandTypeSubsets.member().size() > 0;
      return has_groups;
    }

    std::vector<ComponentDataType> getTypesInUserSelectableCommandTypeSubset(const std::string& groupName) const
    {
      QReadLocker locker(this->mUserSelectableCommandTypeSubsets.getLockPtr());
      auto iter = this->mUserSelectableCommandTypeSubsets.member().find(groupName);
      if (iter == this->mUserSelectableCommandTypeSubsets.member().end())
      {
        CEDAR_THROW(UserSelectableCommandTypeSubsetNameNotFoundException, "Could not find a group with the name \"" + groupName + "\".");
      }

      std::vector<ComponentDataType> copy = iter->second;
      return copy;
    }

    float getCommunicationErrorRate() const
    {
      QReadLocker locker(this->mCommunicationErrorCount.getLockPtr());
      float error_rate = 0.0;
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

  protected:
    virtual void lazyInitializeMembers(cedar::dev::Component::ComponentDataType type)
    {
      cedar::aux::LockSet lock_set;
      cedar::aux::append(lock_set, mUserSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mPreviousDeviceSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mInitialUserSideSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::LockSetLocker lock_set_locker(lock_set);
      this->lazyInitializeUnlocked(mUserSideBuffer, type);
      this->lazyInitializeUnlocked(mPreviousDeviceSideBuffer, type);
      this->lazyInitializeUnlocked(mInitialUserSideSubmittedData, type);
    }

    virtual void resetBuffers(cedar::dev::Component::ComponentDataType type, int matrixType)
    {
      cedar::aux::LockSet lock_set;
      cedar::aux::append(lock_set, mUserSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mPreviousDeviceSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::append(lock_set, mInitialUserSideSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
      cedar::aux::LockSetLocker locker(lock_set);
      if (this->hasType(type))
      {
        this->resetBufferUnlocked(mUserSideBuffer, type, matrixType);
        this->resetBufferUnlocked(mPreviousDeviceSideBuffer, type, matrixType);
        this->resetBufferUnlocked(mInitialUserSideSubmittedData, type, matrixType);
      }
    }

    bool hasUserSelectableCommandTypeSubsetUnlocked(const std::string& groupName)
    {
      return this->mUserSelectableCommandTypeSubsets.member().find(groupName) != this->mUserSelectableCommandTypeSubsets.member().end();
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

    float getBufferIndex(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type, int index) const
    {
      QReadLocker lock(bufferData.getLockPtr());

      auto ret = getBufferIndexUnlocked(bufferData, type, index);
      return ret;
    }

    float getBufferIndexUnlocked(const cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type, int index) const
    {
      const auto &buffer = getBufferUnlocked(bufferData, type);
      CEDAR_DEBUG_ASSERT(buffer.rows > index);
      return buffer.at<float>(index, 0);
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

    void setDataIndex(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, int index, float value)
    {
      QWriteLocker lock(bufferData.getLockPtr());
      setDataIndexUnlocked(bufferData, type, index, value);
    }

    void setDataIndexUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, const ComponentDataType type, int index, float value)
    {
      CEDAR_DEBUG_ASSERT(bufferData.member()[type]);
      bufferData.member()[type]->getData().at<float>(index, 0) = value;
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

    void resetBufferUnlocked(cedar::aux::LockableMember<BufferDataType>& bufferData, ComponentDataType type,int matrixType)
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

      // todo: check for higher-order tensors
      bufferData.member()[type]->setData(cv::Mat::zeros(dim[0], dim[1], matrixType));
    }

  public:
    //@todo: most of these should probably be private

    cedar::aux::LockableMember<BufferDataType> mUserSideBuffer; // was: mUserSideCommandBuffer, mUserSideMeasurementsBuffer

    cedar::aux::LockableMember<BufferDataType> mInitialUserSideSubmittedData; // was: mInitialUserSideSubmittedCommands

    // Cache for the user-interface
    cedar::aux::LockableMember<BufferDataType> mPreviousDeviceSideBuffer; // was: mPreviousDeviceSideMeasurementsBuffer

  private:
    std::map<ComponentDataType, cedar::dev::Component::DimensionalityType> mInstalledDimensions;

    cedar::aux::LockableMember<TransformationHookContainerType> mTransformationHooks;

    cedar::aux::LockableMember<std::set<cedar::dev::Component::ComponentDataType> > mInstalledTypes;

    cedar::aux::LockableMember<std::map<cedar::dev::Component::ComponentDataType, std::string> > mInstalledNames;

    cedar::aux::LockableMember<std::map<std::string, std::vector<cedar::dev::Component::ComponentDataType> > > mUserSelectableCommandTypeSubsets;

    cedar::aux::LockableMember<std::string> mActiveUserSelectableCommandTypeSubset;

    //! A member that contains the counts of errors for the last stepCommunication* calls.
    cedar::aux::LockableMember<cedar::aux::MovingAverage<float> > mCommunicationErrorCount;

    //! The last things that went wrong during communication.
    cedar::aux::LockableMember<std::deque<std::string> > mLastCommunicationErrorMessages;

};

class cedar::dev::Component::MeasurementDataCollection : public cedar::dev::Component::DataCollection
{
public:
  cedar::aux::DataPtr getDeviceSideData(const cedar::dev::Component::ComponentDataType &type)
  {
    return this->getData(this->mDeviceSideRetrievedData, type);
  }

  cv::Mat getDeviceSideRetrievedBufferUnlocked(ComponentDataType type)
  {
    return this->getBufferUnlocked(mDeviceSideRetrievedData, type);
  }

  void setDeviceSideRetrievedBuffer(ComponentDataType type, cv::Mat data)
  {
    this->setData(mDeviceSideRetrievedData, type, data);
  }

  void setDeviceSideRetrievedBufferUnlocked(ComponentDataType type, cv::Mat data)
  {
    this->setDataUnlocked(mDeviceSideRetrievedData, type, data);
  }

  void resetDeviceSideRetrievedBufferUnlocked(ComponentDataType type ,int matrixType)
  {
    this->resetBufferUnlocked(mDeviceSideRetrievedData, type, matrixType);
  }

public:
  cedar::aux::LockableMember<BufferDataType> mDeviceSideRetrievedData;

protected:
  void lazyInitializeMembers(cedar::dev::Component::ComponentDataType type)
  {
    this->DataCollection::lazyInitializeMembers(type);

    QWriteLocker locker(mDeviceSideRetrievedData.getLockPtr());
    this->lazyInitializeUnlocked(mDeviceSideRetrievedData, type);
  }

  virtual void resetBuffers(cedar::dev::Component::ComponentDataType type, int matrixType)
  {
    this->DataCollection::resetBuffers(type, matrixType);

    QWriteLocker locker(mDeviceSideRetrievedData.getLockPtr());
    if (this->hasType(type))
    {
      this->resetBufferUnlocked(mDeviceSideRetrievedData, type, matrixType);
    }
  }
};

// command data is also a measruement collection because the actual result of commanding the values is measured
class cedar::dev::Component::CommandDataCollection : public cedar::dev::Component::MeasurementDataCollection
{
public:
  cv::Mat getDeviceSideSubmittedBufferUnlocked(ComponentDataType type)
  {
    return this->getBufferUnlocked(mDeviceSideSubmittedData, type);
  }

  void setDeviceSideSubmittedBufferUnlocked(ComponentDataType type, cv::Mat data)
  {
    this->setDataUnlocked(mDeviceSideSubmittedData, type, data);
  }

  void resetDeviceSideSubmittedBufferUnlocked(ComponentDataType type, int matrixType)
  {
    this->resetBufferUnlocked(mDeviceSideSubmittedData, type, matrixType);
  }

public:
  // Cache for the DeviceSide-interface
  cedar::aux::LockableMember<BufferDataType> mDeviceSideSubmittedData; // was: mDeviceSideSubmittedCommands

protected:
  void lazyInitializeMembers(cedar::dev::Component::ComponentDataType type)
  {
    this->MeasurementDataCollection::lazyInitializeMembers(type);

    QWriteLocker locker(mDeviceSideSubmittedData.getLockPtr());
    this->lazyInitializeUnlocked(mDeviceSideSubmittedData, type);
  }

  virtual void resetBuffers(cedar::dev::Component::ComponentDataType type, int matrixType)
  {
    this->MeasurementDataCollection::resetBuffers(type, matrixType);

    QWriteLocker locker(mDeviceSideSubmittedData.getLockPtr());
    if (this->hasType(type))
    {
      this->resetBufferUnlocked(mDeviceSideSubmittedData, type, matrixType);
    }
  }
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::Component::init()
{
  this->mControllerFinished= true;
  this->mDestroying= false;

  this->mMeasurementData = boost::make_shared<cedar::dev::Component::MeasurementDataCollection>();
  this->mCommandData = boost::make_shared<cedar::dev::Component::CommandDataCollection>();
  mCommunicationThread = std::unique_ptr<cedar::aux::LoopFunctionInThread>(
                                new cedar::aux::LoopFunctionInThread(
                                  boost::bind(&cedar::dev::Component::stepCommunication,
                                              this,
                                              _1) ));


  mCommunicationThread->connectToStartSignal(boost::bind(&cedar::dev::Component::processStart, this));
  mCommunicationThread->setStepSize(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));

  this->mLostTime = 0.0 * cedar::unit::seconds;

  mWatchDogThread = std::unique_ptr<cedar::aux::LoopFunctionInThread>(
                                new cedar::aux::LoopFunctionInThread(
                                  boost::bind(
                                    (void(*)(cedar::unit::Time))
                                      &cedar::dev::Component::stepStaticWatchDog,
                                    _1) ));

  mTooSlowCounter.member() = 0;
  mNotReadyForCommandsCounter.member() = 0;
  mWatchDogCounter.member() = 0;
  mSuppressUserSideInteraction = false;
}

// constructor
cedar::dev::Component::Component()
:
    mMatrixType(new cedar::aux::StringParameter(this, "cvMatType", "CV_32F"))
{
  init();
}

cedar::dev::Component::Component(cedar::dev::ChannelPtr channel)
:
mChannel(channel),
mMatrixType(new cedar::aux::StringParameter(this, "cvMatType", "CV_32F"))
{
  init();
}

// needs to be called by the end-point of the inheritance tree
void cedar::dev::Component::prepareComponentDestructAbsolutelyRequired()
{
  if (this->isCommunicating())
  {
    stopCommunication();
  }
  mDestructWasPrepared= true;
}

cedar::dev::Component::~Component()
{
  mDestroying= true;

  if (!mDestructWasPrepared)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You forgot to call prepareComponentDestructAbsolutelyRequired() in the child's destructor of " + prettifyName() + "!",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  // the thread will stopped when mCommunicationThread is destructed, anyway, but we
  // try to send the stop request as early as possible ...
  mWatchDogThread->requestStop();
  mCommunicationThread->requestStop();


  // virtual can't be called in the inherit. This is why all children
  // must call it!
  //brakeNow();

  mWatchDogThread->stop();
  mCommunicationThread->stop();

  mWatchDogThread->wait();
  mCommunicationThread->wait();

  // wait for end of communication
  {
    cedar::aux::LockSet locks;

    // lock measurements 
    cedar::aux::append(locks, this->mMeasurementData->mDeviceSideRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    // lock commands 
    cedar::aux::append(locks, this->mCommandData->mDeviceSideRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);

    cedar::aux::append(locks, this->mRetrieveMeasurementHooks.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::append(locks, this->mSubmitCommandHooks.getLockPtr(), cedar::aux::LOCK_TYPE_READ);

    cedar::aux::append(locks, this->mController.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::append(locks, this->mNoCommandHook.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::append(locks, this->mAfterCommandBeforeMeasurementHook.getLockPtr(), cedar::aux::LOCK_TYPE_READ);

    cedar::aux::LockSetLocker locker(locks);
  }
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

void cedar::dev::Component::getCommunicationErrorRates(float& commands, float& measurements) const
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


void cedar::dev::Component::defineUserSelectableCommandTypeSubset(const std::string& groupName)
{
  this->mCommandData->defineUserSelectableCommandTypeSubset(groupName);
}

void cedar::dev::Component::setActiveUserSelectableCommandTypeSubset(const std::string& groupName)
{
  this->mCommandData->setActiveUserSelectableCommandTypeSubset(groupName);
}

std::vector<std::string> cedar::dev::Component::listUserSelectableCommandTypeSubsets() const
{
  return this->mCommandData->listUserSelectableCommandTypeSubsets();
}

void cedar::dev::Component::addCommandTypeToUserSelectableCommandTypeSubset(const std::string& groupName, const ComponentDataType& commandType)
{
  this->mCommandData->addTypeToUserSelectableCommandTypeSubset(groupName, commandType);
}

bool cedar::dev::Component::hasUserSelectableCommandTypeSubsets() const
{
  return this->mCommandData->hasUserSelectableCommandTypeSubsets();
}

std::vector<cedar::dev::Component::ComponentDataType> cedar::dev::Component::getCommandsInUserSelectableCommandTypeSubset(const std::string& groupName) const
{
  return this->mCommandData->getTypesInUserSelectableCommandTypeSubset(groupName);
}

cedar::dev::Component::DimensionalityType cedar::dev::Component::getCommandDimensionality(ComponentDataType type) const
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

QString cedar::dev::Component::getIconPath() const
{
    auto slot = mSlot.lock();
    return slot->getIconPath();
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
  return this->mMeasurementData->getUserSideData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getMeasurementData(const ComponentDataType &type) const
{
  return this->mMeasurementData->getUserSideData(type);
}

cedar::aux::DataPtr cedar::dev::Component::getUserSideCommandData(const ComponentDataType &type)
{
  return this->mCommandData->getUserSideData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getUserSideCommandData(const ComponentDataType &type) const
{
  return this->mCommandData->getUserSideData(type);
}

cedar::aux::ConstDataPtr cedar::dev::Component::getDeviceSideCommandData(const ComponentDataType &type) const
{
  return this->mCommandData->getDeviceSideData(type);
}

cedar::aux::DataPtr cedar::dev::Component::getDeviceSideCommandData(const ComponentDataType &type)
{
  return this->mCommandData->getDeviceSideData(type);
}

void cedar::dev::Component::setCommandDimensionality(ComponentDataType type, DimensionalityType dim)
{
  this->mCommandData->setDimensionality(type, dim,this->getMeasurementMatrixType());
}

void cedar::dev::Component::setMeasurementDimensionality(ComponentDataType type, DimensionalityType dim)
{
  this->mMeasurementData->setDimensionality(type, dim,this->getMeasurementMatrixType());
}

void cedar::dev::Component::setCommandAndMeasurementDimensionality(ComponentDataType type, DimensionalityType dim)
{
  setCommandDimensionality(type, dim);
  setMeasurementDimensionality(type, dim);
}

void cedar::dev::Component::setCommandDimensionality(ComponentDataType type, unsigned int dim)
{
  DimensionalityType fulldim{ dim, 1 };

  this->mCommandData->setDimensionality(type, fulldim,this->getMeasurementMatrixType());
}

void cedar::dev::Component::setMeasurementDimensionality(ComponentDataType type, unsigned int dim)
{
  DimensionalityType fulldim{ dim, 1};

  this->mMeasurementData->setDimensionality(type, fulldim,this->getMeasurementMatrixType());
}

void cedar::dev::Component::setCommandAndMeasurementDimensionality(ComponentDataType type, unsigned int dim)
{
  DimensionalityType fulldim{ dim, 1 };

  setCommandDimensionality(type, fulldim);
  setMeasurementDimensionality(type, fulldim);
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
  int matrixType = this->getMeasurementMatrixType();
  {
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, this->mMeasurementData->mUserSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mMeasurementData->mPreviousDeviceSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mMeasurementData->mDeviceSideRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockSetLocker locker(locks);

    for (auto& type : this->mMeasurementData->getInstalledTypesUnlocked())
    {
      this->mMeasurementData->resetUserSideBufferUnlocked(type, matrixType);
      this->mMeasurementData->resetPreviousDeviceSideBufferUnlocked(type, matrixType);
      this->mMeasurementData->resetDeviceSideRetrievedBufferUnlocked(type, matrixType);
    }
  }

  {
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, this->mCommandData->mUserSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mCommandData->mDeviceSideSubmittedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::LockSetLocker locker(locks);

    for(auto& type : this->mCommandData->getInstalledTypes())
    {
      this->mCommandData->resetUserSideBufferUnlocked(type,matrixType);
      this->mCommandData->resetDeviceSideSubmittedBufferUnlocked(type,matrixType);
    }
  }
}

void cedar::dev::Component::applyDeviceSideCommandsAs(ComponentDataType type)
{
  if (!this->mCommandData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "The given type is not installed.");
  }
  mDeviceSideCommandSelection = type;
}

void cedar::dev::Component::setUserSideCommandBuffer(ComponentDataType type, cv::Mat data)
{
  this->checkExclusivenessOfCommand(type);
  QWriteLocker locker(this->mUserSideCommandUsed.getLockPtr());    

  this->mCommandData->setUserSideBuffer(type, data);
  this->mUserSideCommandUsed.member().insert(type);
}

void cedar::dev::Component::setInitialUserSideCommandBuffer(ComponentDataType type, cv::Mat data)
{
  if (this->isCommunicating())
  {
    // cannot set initial commands if a component is already running
    cedar::aux::LogSingleton::getInstance()->error(
      "Cannot apply initial configuration command buffer, as device is already communicating.",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }

  QWriteLocker(this->mCommandData->mInitialUserSideSubmittedData.getLockPtr());
  std::cout << data;
  this->mCommandData->mInitialUserSideSubmittedData.member()[type]->setData(data.clone());
}


void cedar::dev::Component::setUserSideCommandBufferIndex(ComponentDataType type, int index, float value)
{
  this->checkExclusivenessOfCommand(type);
  QWriteLocker locker(this->mUserSideCommandUsed.getLockPtr());
  this->mCommandData->setUserSideBufferIndex(type, index, value);
  this->mUserSideCommandUsed.member().insert(type);
}

cv::Mat cedar::dev::Component::getUserSideMeasurementBuffer(ComponentDataType type) const
{
  return this->mMeasurementData->getUserSideBuffer(type);
}

float cedar::dev::Component::getUserSideMeasurementBufferIndex(ComponentDataType type, int index) const
{
  return this->mMeasurementData->getUserSideBufferIndex(type, index);
}

cv::Mat cedar::dev::Component::getPreviousDeviceSideMeasurementBuffer(ComponentDataType type) const
{
  return this->mMeasurementData->getPreviousDeviceSideBuffer(type);
}

float cedar::dev::Component::getPreviousDeviceSideMeasurementBufferIndex(ComponentDataType type, int index) const
{
  return this->mMeasurementData->getPreviousDeviceSideBufferIndex(type, index);
}

void cedar::dev::Component::registerCommandHook(ComponentDataType type, CommandFunctionType fun)
{
  if (!this->mCommandData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "Cannot register command hook: type is not installed.");
  }

  QWriteLocker locker(this->mSubmitCommandHooks.getLockPtr());
  // cannot replace existing hook
  if (this->mSubmitCommandHooks.member().find(type) != this->mSubmitCommandHooks.member().end())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A command hook is already set for \"" + this->getNameForCommandType(type) + "\".");
  }
  mSubmitCommandHooks.member()[ type ] = fun;
}

void cedar::dev::Component::registerNoCommandHook(NoCommandFunctionType fun)
{
  QWriteLocker locker(this->mNoCommandHook.getLockPtr());
  // cannot replace existing hook
  if (this->mNoCommandHook.member())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A no-command hook is already set.");
  }
  mNoCommandHook.member() = fun;
}

void cedar::dev::Component::registerNotReadyForCommandHook(NoCommandFunctionType fun)
{
  QWriteLocker locker(this->mNotReadyForCommandHook.getLockPtr());
  // cannot replace existing hook
  if (this->mNotReadyForCommandHook.member())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A not-ready-for-command hook is already set.");
  }
  mNotReadyForCommandHook.member() = fun;
}

void cedar::dev::Component::registerAfterCommandBeforeMeasurementHook(AfterCommandFunctionType fun)
{
  QWriteLocker locker(this->mAfterCommandBeforeMeasurementHook.getLockPtr());
  // cannot replace existing hook
  if (this->mAfterCommandBeforeMeasurementHook.member())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A after-command-before-measurement hook is already set.");
  }
  mAfterCommandBeforeMeasurementHook.member() = fun;
}

void cedar::dev::Component::registerMeasurementHook(ComponentDataType type, MeasurementFunctionType fun)
{
  if (!this->mMeasurementData->hasType(type))
  {
    CEDAR_THROW(cedar::dev::Component::TypeNotFoundException, "Cannot register measurement hook: type is not installed.");
  }

  QWriteLocker locker(this->mRetrieveMeasurementHooks.getLockPtr());
  // cannot replace existing hook
  if (this->mRetrieveMeasurementHooks.member().find(type) != this->mRetrieveMeasurementHooks.member().end())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A measurement hook is already set for \"" + this->getNameForMeasurementType(type) + "\".");
  }
  mRetrieveMeasurementHooks.member()[ type ] = fun;
}

boost::signals2::connection cedar::dev::Component::registerStartCommunicationHook(boost::function<void ()> slot)
{
    return mStartCommunicationHook.connect(slot);
}

boost::signals2::connection cedar::dev::Component::registerConnectedHook(boost::function<void ()> slot)
{
    return mConnectedHook.connect(slot);
}

boost::signals2::connection cedar::dev::Component::registerDisconnectedHook(boost::function<void ()> slot)
{
    return mDisconnectedHook.connect(slot);
}

void cedar::dev::Component::registerCheckCommandHook(cedar::dev::Component::CommandCheckFunctionType fun)
{
  QWriteLocker locker(this->mCheckCommandHook.getLockPtr());
  // cannot replace existing hook
  if (this->mCheckCommandHook.member())
  {
    CEDAR_THROW(cedar::dev::Component::DuplicateHookException, "A check-command hook is already set.");
  }
  mCheckCommandHook.member() = fun;
}

void cedar::dev::Component::registerCommandTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
{
  this->mCommandData->registerTransformationHook(from, to, fun);
}

void cedar::dev::Component::registerMeasurementTransformationHook(ComponentDataType from, ComponentDataType to, TransformationFunctionType fun)
{
  this->mMeasurementData->registerTransformationHook(from, to, fun);
}

void cedar::dev::Component::stepCommunication(cedar::unit::Time time)
{
  // We cannot block in this call. Scenario:
  // stopCommunication is called, thus blocks the mutex, but waits for this function to finish, thus never exists
  // thus, if we cannot lock here, we skip the step
  if (!mGeneralAccessLock.tryLock())
  {
    // remember that we skipped some integration/differentiation time, we have to remember this for future calls!
    this->mLostTime += time;
    return;
  }

  // add the lost time to the dt sent to the step functions, then reset the lost time
  cedar::unit::Time real_time = time + this->mLostTime;
  this->mLostTime = 0.0 * cedar::unit::seconds;

  // lock was acquired, let's make sure its unlocked
  cedar::aux::CallOnScopeExit unlocker(boost::bind(&QMutex::unlock, &this->mGeneralAccessLock));

  if (!mChannel
      || (mChannel && mChannel->isOpen()) )
  {
    // possibly abort a bit more quickly
    if (!mCommunicationThread || mCommunicationThread->stopRequested())
      return;

    // its important to get the currently scheduled commands out first
    // (think safety first). this assumes serial communication, of course
    try
    {
      stepCommandCommunication(real_time);
      this->mCommandData->countSuccessfullCommunication();
    }
    catch (const cedar::dev::CommunicationException& e)
    {
      this->mCommandData->countCommunicationError(e);
    }

    // possibly abort a bit more quickly
    if (!mCommunicationThread || mCommunicationThread->stopRequested())
      return;

    // to send only once per cycle to the HW with new commands and get new measurements
    try
    {
      stepAfterCommandBeforeMeasurementCommunication();
      this->mCommandData->countSuccessfullCommunication();
    }
    catch (const cedar::dev::CommunicationException& e)
    {
      this->mCommandData->countCommunicationError(e);
    }

    // possibly abort a bit more quickly
    if (!mCommunicationThread || mCommunicationThread->stopRequested())
      return;

    try
    {
      stepMeasurementCommunication(real_time); // note, the post-measurements transformations also take time
      this->mMeasurementData->countSuccessfullCommunication();
    }
    catch (const cedar::dev::CommunicationException& e)
    {
      this->mMeasurementData->countCommunicationError(e);
    }
  } // end: channel is open

  // utitlity: warn if consistently much too slow
  if (time > this->getCommunicationStepSize() * 2.0
      && !mControllerFinished) // annoying if we are braking ...
  {
    QWriteLocker lock(&mTooSlowCounter.getLock());

    mTooSlowCounter.member()++;

    // dont scroll ... show warning after 5 tries and then only every few seconds
    if (mTooSlowCounter.member() == 5
        || mTooSlowCounter.member() > 1500)
    {
      std::string s = "";

      if (mTooSlowCounter.member() > 5)
      {
        s = " (repeated " + boost::lexical_cast<std::string>(mTooSlowCounter.member()) + " times)";
      }

      cedar::aux::LogSingleton::getInstance()->warning(
        "Communication with " + prettifyName() + " is consistently "
        "much slower than specified: "
        + boost::lexical_cast<std::string>( time ) 
        + " (effective time) vs "
        + boost::lexical_cast<std::string>( this->getCommunicationStepSize() )
        + " (specified time)" + s,
        CEDAR_CURRENT_FUNCTION_NAME);

      if (mTooSlowCounter.member() > 1500)
      {
        mTooSlowCounter.member()= 6;
      }
    }
  }

  if(mRunningComponentInstancesAliveTime.find(this)!= mRunningComponentInstancesAliveTime.end())
  {
    mRunningComponentInstancesAliveTime[ this ]= boost::posix_time::microsec_clock::local_time();
  }
}

void cedar::dev::Component::stepAfterCommandBeforeMeasurementCommunication()
{
  QReadLocker between_hook_locker(mAfterCommandBeforeMeasurementHook.getLockPtr());

  auto hook_found = mAfterCommandBeforeMeasurementHook.member();
  if (hook_found)
  {
    // registering this is optional
    hook_found();
  }
}

void cedar::dev::Component::stepCommandCommunication(cedar::unit::Time dt)
{
  cedar::aux::Timer timer;

  // todo: check locking in this function, forgot some stuff ...
  ComponentDataType type_for_DeviceSide, type_from_user;
  cv::Mat userData, ioData;

  cedar::aux::LockSet locks;
  cedar::aux::append(locks, this->mUserSideCommandUsed.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, this->mSubmitCommandHooks.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, this->mController.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::LockSetLocker locker(locks);

  // evaluate command type for DeviceSide:
  if (mDeviceSideCommandSelection)
  {
    type_for_DeviceSide = mDeviceSideCommandSelection.get();
    //std::cout << "    commands restricted to ... " << type_for_DeviceSide  << std::endl;    
  }
  else
  {
    QReadLocker submit_command_hooks_locker(mSubmitCommandHooks.getLockPtr());
    // guess DeviceSide type to use. easy if there is only one hook
    if (mSubmitCommandHooks.member().size() != 1)
    {
      // heuristics: 
      

      if (this->mUserSideCommandUsed.member().size() != 0)
      {
        CEDAR_THROW
        (
         cedar::dev::Component::CouldNotGuessDeviceSideTypeException,
         "Could not guess device type: too many submit hooks. Please select a device type manually." 
        ); 
      }
    }

    type_for_DeviceSide = mSubmitCommandHooks.member().begin()->first;
  }

  if (!this->isReadyForCommands())
  {
    QReadLocker nocommand_hook_locker(mNotReadyForCommandHook.getLockPtr());

    auto hook_found = mNotReadyForCommandHook.member();
    if (hook_found)
    {
      // registering this is optional
      hook_found(type_for_DeviceSide);
    }

    QWriteLocker lock(&mNotReadyForCommandsCounter.getLock());

    if (this->mUserSideCommandUsed.member().size() != 0)
    {
      mNotReadyForCommandsCounter.member()++;

      if (mNotReadyForCommandsCounter.member() == 1
         || mNotReadyForCommandsCounter.member() > 5000)
      {
        std::string s = "";

        if (mNotReadyForCommandsCounter.member() > 5)
        {
          s = " (repeated " + boost::lexical_cast<std::string>(mNotReadyForCommandsCounter.member()) + " times)";
        }

        // todo: output component name her (and for the other messages/warnings)
        cedar::aux::LogSingleton::getInstance()->warning(
          "Commands issued but " + prettifyName() + " is not accepting commands" + s,
          CEDAR_CURRENT_FUNCTION_NAME);
      }

      if (mNotReadyForCommandsCounter.member() > 5000)
      {
        mNotReadyForCommandsCounter.member()= 1;
      }
    }

    return;
  }
  else
  {
    QWriteLocker lock(&mNotReadyForCommandsCounter.getLock());

    mNotReadyForCommandsCounter.member()= 0;
  }

  // if there are neither user commands nor a controller, nothing needs to be done
  if (this->mUserSideCommandUsed.member().size() == 0 && !this->mController.member())
  {
    QReadLocker nocommand_hook_locker(mNoCommandHook.getLockPtr());

    auto hook_found = mNoCommandHook.member();
    if (hook_found)
    {
      // registering this is optional
      hook_found(type_for_DeviceSide);
    }

    return;
  }

  // if there are no submit hooks but there is stuff to submit, we have a problem
  if (mSubmitCommandHooks.member().size() == 0)
  {
    CEDAR_THROW(NoSubmitHooksException, "No submit hooks defined for commands.");
  }

  if (!mControllerFinished) // has the controller still something to do?
  {
    type_from_user = this->mController.member()->mBufferType;
    userData = (this->mController.member()->mCallback());

    if(mControllerFinished)
    {
      locker.unlock();
      this->clearController();

      cedar::aux::LogSingleton::getInstance()->message(
        "Control of " + prettifyName() + " now reverted to architecture. ",
        CEDAR_CURRENT_FUNCTION_NAME);
    }
  }
  else // do not use Controller Callback:
  {
    // guess command type to use
    // safe only if there was only one command hook registered
    if (this->mUserSideCommandUsed.member().size() > 1)
    {
      std::string set_commands;
      for (const auto &what : this->mUserSideCommandUsed.member())
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
        "Could not guess the type of the command because too many commands have been set for " + prettifyName() +". Set commands are: " + set_commands
      );

    }

    // we know the map has exactly one entry
    type_from_user = *(this->mUserSideCommandUsed.member().begin());

    QReadLocker lock(this->mCommandData->mUserSideBuffer.getLockPtr());
    userData = this->mCommandData->getUserSideBufferUnlocked(type_from_user).clone();
  }

  locker.unlock();

  //  this->mUserSideCommandUsed.clear();

    if (type_for_DeviceSide != type_from_user && type_from_user > 0) //if hardware support command type use it ;)
    {
        auto it = mSubmitCommandHooks.member().find(type_from_user);
        if (it != mSubmitCommandHooks.member().end())
        {
            type_for_DeviceSide = type_from_user;
            //std::cout << type_for_DeviceSide << " / " << type_from_user << std::endl;
        }
    }

  // do we need to transform the input?
  if (type_for_DeviceSide != type_from_user)
  {
    auto hook = this->mCommandData->findTransformationHook(type_from_user, type_for_DeviceSide);

    // hook must exist
    if (!hook)
    {
      std::string user_type_str = this->mCommandData->getNameForType(type_from_user);
      std::string device_type_str = this->mCommandData->getNameForType(type_for_DeviceSide);
      CEDAR_THROW(cedar::dev::Component::HookNotFoundException, "Could not find a command hook from \"" + user_type_str + "\" to \"" + device_type_str + "\".");
    }

    QReadLocker lock1(this->mMeasurementData->mUserSideBuffer.getLockPtr());
    // call hook
    ioData = hook.get()(dt, userData);
  }
  else
  {
    ioData = userData.clone();
  }

  if(this->mCheckCommandHook.member())
  {
    //std::cout<<"We have a mCheckCommandHook! We now check "<< type_for_DeviceSide << " and " << ioData <<std::endl;
    if( !( mCheckCommandHook.member()(type_for_DeviceSide, ioData) ) )
    {
      cedar::aux::LogSingleton::getInstance()->error(
         "Could not call command check function.",
          CEDAR_CURRENT_FUNCTION_NAME);
      return;
    }
  }

  // check for NaNs, assuming one element per row
  for(int i=0; i < ioData.rows; ++i)
  {
    if(std::isnan(ioData.at<float>(i, 0)))
    {
      ioData.at<float>(i, 0) = 0;

      cedar::aux::LogSingleton::getInstance()->warning(
        this->prettifyName()+": ioData at " + cedar::aux::toString(i) + " is not a number. Setting it to 0 and continue.",
        CEDAR_CURRENT_FUNCTION_NAME);
    }
  }

  QReadLocker submit_command_hooks_locker(mSubmitCommandHooks.getLockPtr());
  auto hook_found = mSubmitCommandHooks.member().find(type_for_DeviceSide);
  if (hook_found == mSubmitCommandHooks.member().end())
  {
    CEDAR_THROW(cedar::dev::Component::HookNotFoundException, "Could not find a submit hook for \"" + this->mMeasurementData->getNameForType(type_for_DeviceSide) + "\".");
  }

  try
  {
    (hook_found->second)(ioData);
  }
  catch(cedar::dev::IgnoreCommunicationException &e)
  {
//    std::cout<<"Ignore Exception catched in stepCommandCommunication!"<<std::endl;
    // ignore, everthing is fine
  }

  submit_command_hooks_locker.unlock();

  QWriteLocker time_locker(this->mLastStepCommandsTime.getLockPtr());
  this->mLastStepCommandsTime.member() = timer.elapsed();
}

// update the DeviceSide Cache
void cedar::dev::Component::stepMeasurementCommunication(cedar::unit::Time dt)
{
  cedar::aux::Timer timer;

  std::vector< ComponentDataType > types_to_transform;
  std::vector< ComponentDataType > types_we_measured;

  if (isReadyForMeasurements())
  {
    // lock measurements 
    cedar::aux::LockSet locks;
    cedar::aux::append(locks, this->mMeasurementData->mDeviceSideRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    //cedar::aux::append(locks, this->mMeasurementData->mUserSideMeasurementsBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
    cedar::aux::append(locks, this->mRetrieveMeasurementHooks.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
    cedar::aux::LockSetLocker locker(locks);

    if (mDestroying)
      return;

    // thinks I can get directly from HW:
    for (const auto& type : this->mMeasurementData->getInstalledTypes())
    {
      auto found = mRetrieveMeasurementHooks.member().find( type );

      if (found != mRetrieveMeasurementHooks.member().end())
      {
        // execute the hook:
        const cv::Mat data = (found->second)();
        if(data.rows != 0 && data.cols != 0)
        {
          this->mMeasurementData->setDeviceSideRetrievedBufferUnlocked(type, data);
          types_we_measured.push_back(type);
        }
        else
        {
          this->mMeasurementData->setDeviceSideRetrievedBufferUnlocked(type, this->getPreviousDeviceSideMeasurementBuffer(type));
        }
      }
      else
      {
        // store things we need to calculate afterwards
        types_to_transform.push_back(type);
      }
    }

    for (auto& missing_type : types_to_transform )
    {
      for (auto& measured_type : types_we_measured )
      {
        auto hook = this->mMeasurementData->findTransformationHook(measured_type, missing_type);
        if (hook.is_initialized())
        {
          // call transformation hook
          this->mMeasurementData->mDeviceSideRetrievedData.member()[missing_type]->setData
          (
            hook.get()(dt, this->mMeasurementData->mDeviceSideRetrievedData.member()[measured_type]->getData())
          );
        }
      }
    }

    locker.unlock();
  }

  // todo: make this non-blocking for this looped thread
  updateUserSideMeasurements();

  QWriteLocker time_locker(this->mLastStepMeasurementsTime.getLockPtr());
  this->mLastStepMeasurementsTime.member() = timer.elapsed();
}

void cedar::dev::Component::updateUserSideMeasurements()
{
  // this is here to preserve lock order (getInstalledTypes locks internally)
  auto measurement_types = this->mMeasurementData->getInstalledTypes();

  // lock caches
  cedar::aux::LockSet locks;
  cedar::aux::append(locks, this->mMeasurementData->mPreviousDeviceSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_READ);
  cedar::aux::append(locks, this->mMeasurementData->mUserSideBuffer.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::append(locks, this->mMeasurementData->mDeviceSideRetrievedData.getLockPtr(), cedar::aux::LOCK_TYPE_WRITE);
  cedar::aux::LockSetLocker locker(locks);

  for (auto type : measurement_types)
  {
    this->mMeasurementData->mPreviousDeviceSideBuffer.member()[type]->getData() = this->mMeasurementData->mUserSideBuffer.member()[type]->getData().clone();
    this->mMeasurementData->mUserSideBuffer.member()[type]->getData() = this->mMeasurementData->mDeviceSideRetrievedData.member()[type]->getData().clone();
    this->mMeasurementData->mDeviceSideRetrievedData.member()[type]->getData() = 0.0; // Warum 0.0 ? Warum ist das keine Matrix?
  }

  locker.unlock();
  
  if (mDestroying)
    return;

  emit updatedUserSideMeasurementSignal();
}

void cedar::dev::Component::startCommunication(bool suppressUserSideInteraction)
{
#ifdef DEBUG
  std::string s= "";

  if (suppressUserSideInteraction)
  {
    s= " Don't forget to start your architecture to interact.";
  }

  cedar::aux::LogSingleton::getInstance()->systemInfo(
    "Starting communication with " + prettifyName() + " in the background." + s,
    CEDAR_CURRENT_FUNCTION_NAME
  );
#endif  

  setSuppressUserSideInteraction(suppressUserSideInteraction);

  // do not re-enter, do not stop/start at the same time
  QMutexLocker lockerGeneral(&mGeneralAccessLock);

  // start WatchDog early, before calling hardware:
  if (!mWatchDogThread)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Watchdog thread of " + prettifyName() + " somehow disappeared",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }
  else if (mWatchDogThread->isRunning())
  {
    // do not start again
    // do nothing
  }
  else
  {
    mWatchDogThread->setStepSize( 0.2 * cedar::unit::seconds ); 
    mWatchDogThread->start();
  }
  auto now = boost::posix_time::microsec_clock::local_time();
  mRunningComponentInstancesAliveTime[ this ]= now; 
  mRunningComponentInstancesStartTime[ this ]= now;

  if (this->mCommandData->getInstalledTypes().empty() && this->mMeasurementData->getInstalledTypes().empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "No commands or measurements set in " + prettifyName() + ".",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }


  mCommunicationThread->start();


  // workaround to get at least several measurements to be able to differentiate
  mCommunicationThread->waitUntilStepped();
  mCommunicationThread->waitUntilStepped();
  mCommunicationThread->waitUntilStepped();

  mConnectedHook();
}


void cedar::dev::Component::handleStopCommunicationNonBlocking()
{
  // do not hold the general lock here

  mRunningComponentInstancesAliveTime.erase( this );
  mRunningComponentInstancesStartTime.erase( this );

  if (mRunningComponentInstancesAliveTime.empty())
  {
    if (mWatchDogThread)
    {
      mWatchDogThread->stop();
    }
  }
}

void cedar::dev::Component::destroyCommunication()
{
  // since this method can be called when the communication thread hangs,
  // the general lock may be held.

  // do not call mCommunicationThread->stop()
  // maybe call requestStop() ?

  handleStopCommunicationNonBlocking();
}

void cedar::dev::Component::stopCommunication()
{
  QMutexLocker locker_general(&mGeneralAccessLock);

  //  cedar::aux::LogSingleton::getInstance()->systemInfo(
  //    "Stopping communication with " + prettifyName() + ".",
  //    CEDAR_CURRENT_FUNCTION_NAME
  //  );

  // do not re-enter, do not stop/start at the same time
  // first, stop the thread
  mCommunicationThread->requestStop();

  // make sure it is actually stopped
  mCommunicationThread->stop();


  handleStopCommunicationNonBlocking();

  locker_general.unlock();

  if (this->mChannel)
  {
    this->mChannel->close();
  }

  mCommunicationThread->wait();
  mDisconnectedHook();
}

void cedar::dev::Component::start()
{
  clearController();
  startCommunication();
  mConnectedHook();
}

void cedar::dev::Component::stop()
{
  stopCommunication();
  mDisconnectedHook();
}

cedar::unit::Time cedar::dev::Component::getCommunicationStepSize()
{
  return mCommunicationThread->getStepSize();
}

void cedar::dev::Component::setCommunicationStepSize(const cedar::unit::Time& time)
{
  mCommunicationThread->setStepSize(time);
}

void cedar::dev::Component::setStepSize(const cedar::unit::Time& time)
{
  setCommunicationStepSize(time);
}

void cedar::dev::Component::setIdleTime(const cedar::unit::Time& time)
{
  mCommunicationThread->setIdleTime(time);
}

void cedar::dev::Component::setSimulatedTime(const cedar::unit::Time& time)
{
  mCommunicationThread->setSimulatedTime(time);
}

bool cedar::dev::Component::isRunning()
{
  return mCommunicationThread->isRunning();
}

bool cedar::dev::Component::isCommunicating() const
{
  //@todo: add a check if the channel is waiting for async feedback
  return mCommunicationThread->isRunning();
}

bool cedar::dev::Component::isReadyForCommands() const
{
  return isConfigured()
         && !this->getSuppressUserSideInteraction()
         && this->isCommunicating();
}

bool cedar::dev::Component::isReadyForMeasurements() const
{
  return isConfigured()
         && !this->getSuppressUserSideInteraction()
         && this->isCommunicating(); 
}

bool cedar::dev::Component::isReadyForEverything() const
{
  return this->isReadyForMeasurements()
         && this->isReadyForCommands();
}

bool cedar::dev::Component::isCommunicatingNolocking() const
{
  return mCommunicationThread->isRunningNolocking();
}

bool cedar::dev::Component::isRunningNolocking() const
{
  return isCommunicatingNolocking();
}

cv::Mat cedar::dev::Component::integrateComponentData(cedar::unit::Time dt, cv::Mat data, ComponentDataType type)
{
//  std::cout << "Integrate once!" << std::endl;
//  std::cout << data << std::endl;
//  std::cout << this->mMeasurementData->getUserSideBufferUnlocked(type) << std::endl;
  float unitless = dt / (1.0 * cedar::unit::second);
//  std::cout << unitless << std::endl;
//  std::cout << "Integrate once (FIN!)!" << std::endl;
  QReadLocker lock(this->mMeasurementData->mUserSideBuffer.getLockPtr());
  //!@todo check if this uses the right time step to integrate
  cv::Mat result = data * unitless + this->mMeasurementData->getUserSideBufferUnlocked(type);
  return result;
}

cv::Mat cedar::dev::Component::integrateComponentDataTwice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
//    std::cout << "Integrate twice!" << type1 << " " << type2 << std::endl;
//    std::cout << data << std::endl;
//    std::cout << this->mMeasurementData->getUserSideBufferUnlocked(type1) << std::endl;
//    std::cout << this->mMeasurementData->getUserSideBufferUnlocked(type2) << std::endl;
  float unitless = dt / (1.0 * cedar::unit::second);
//    std::cout << unitless << std::endl;
//    std::cout << "Integrate twice (FIN!)!" << std::endl;
  QReadLocker lock(this->mMeasurementData->mUserSideBuffer.getLockPtr());

  //!@todo check if this uses the right time step to integrate
  cv::Mat result = 1.0/2.0 * data.mul(data) * unitless * unitless 
                   + this->mMeasurementData->getUserSideBufferUnlocked(type1)
                     * unitless
                   + this->mMeasurementData->getUserSideBufferUnlocked(type2);
#if 0
   // this is wrong! see 2nd term in Taylor expansion
  cv::Mat result = ( ( data * unitless + this->mMeasurementData->getUserSideBufferUnlocked(type1) )
      * unitless )
    + this->mMeasurementData->getUserSideBufferUnlocked(type2);
#endif

  return result;
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateComponentData(cedar::unit::Time dt, cv::Mat data, ComponentDataType type)
{
  //std::cout << "Differentiate once!" << std::endl;
  //std::cout << data << std::endl;
  //std::cout << dt << std::endl;
  //std::cout << "step size: " << this->mCommunicationThread->getStepSize() << std::endl;
  //std::cout << this->mMeasurementData->getUserSideBufferUnlocked(type) << std::endl;

  float unitless = dt / (1.0 * cedar::unit::second);

  if (unitless == 0.0)
  {
    return cv::Mat::zeros(data.rows, data.cols, this->getMeasurementMatrixType());
  }

  if(data.rows == 0 || data.cols == 0) // have no data? no problem! here, take mine
  {
    data = cv::Mat::zeros(this->mMeasurementData->getUserSideBufferUnlocked(type).rows,
                          this->mMeasurementData->getUserSideBufferUnlocked(type).cols,
                          this->getMeasurementMatrixType());
  }

//  std::cout << unitless << std::endl;
//  std::cout << "Differentiate once (FIN!)!" << std::endl;
// todo: check locking here
  cv::Mat last_data = this->mMeasurementData->getUserSideBufferUnlocked(type);
  //!@todo check if this uses the right time step to differentiate
  return ( data - last_data ) / unitless;
}

// todo: also used for commands
cv::Mat cedar::dev::Component::differentiateComponentDataTwice(cedar::unit::Time dt, cv::Mat data, ComponentDataType type1, ComponentDataType type2)
{
//  std::cout << "Differentiate twice!" << std::endl;
//  std::cout << data << std::endl;
//  std::cout << dt << std::endl;
//  std::cout << "step size: " << this->mCommunicationThread->getStepSize() << std::endl;

  float unitless = dt / (1.0 * cedar::unit::second);

  if (unitless == 0.0)
  {
    return cv::Mat::zeros( data.rows, data.cols, this->getMeasurementMatrixType() );
  }

  if(data.rows == 0 || data.cols == 0) // have no data? no problem! here, take mine
  {
    data = cv::Mat::zeros(this->mMeasurementData->getUserSideBufferUnlocked(type1).rows,
                          this->mMeasurementData->getUserSideBufferUnlocked(type1).cols,
                          this->getMeasurementMatrixType());
  }

  // todo: check locking here
  //!@todo check if this uses the right time step to differentiate
  cv::Mat result = (( data - this->mMeasurementData->getUserSideBufferUnlocked(type1) )  / unitless
      - this->mMeasurementData->getUserSideBufferUnlocked(type2) ) / unitless;
  return result;
}


void cedar::dev::Component::processStart()
{
  // todo: test that mUserSideCommands is empty!
  if (!this->mCommandData->mInitialUserSideSubmittedData.member().empty())
  {
     // do as if the initial user command was the user command
     QWriteLocker lock1(this->mCommandData->mUserSideBuffer.getLockPtr());
     
     this->mCommandData->mUserSideBuffer.member() = this->mCommandData->mInitialUserSideSubmittedData.member();
     //for(int i=0; i < this->mCommandData->mUserSideBuffer.member().size(); ++i)
     //{
     //  std::cout << this->mCommandData->mUserSideBuffer.member()[i] << "\n";
     //}
     lock1.unlock();
  }

  // todo: this will probably not work as expected, anymore
  // this is the initial run, wait for measurements to be in:
  mStartCommunicationHook();
  mConnectedHook();

}

void cedar::dev::Component::clearUserSideCommand()
{
  QWriteLocker user_command_locker(this->mUserSideCommandUsed.getLockPtr());
  this->mUserSideCommandUsed.member().clear();
}

void cedar::dev::Component::clearAllCommands()
{
  QWriteLocker user_command_locker(this->mUserSideCommandUsed.getLockPtr());
  this->mUserSideCommandUsed.member().clear();
  // TODO TODO
//  QWriteLocker dev_command_locker(this->mDeviceSideCommandUsed.getLockPtr());
//  this->mDeviceSideCommandUsed.member().clear();
}

void cedar::dev::Component::checkExclusivenessOfCommand(ComponentDataType type)
{
  QReadLocker user_command_locker(this->mUserSideCommandUsed.getLockPtr());
  if (this->mUserSideCommandUsed.member().size() > 0)
  {
    if (this->mUserSideCommandUsed.member().find(type) == this->mUserSideCommandUsed.member().end())
    {
      // a different command type is already set, throw!
      CEDAR_THROW(CouldNotGuessCommandTypeException, "You used more than one type of commands. Component " + prettifyName() + " cannot handle this.");
    }
  }
}

void cedar::dev::Component::setSuppressUserSideInteraction(bool what)
{
  // todo: lock this but not with the general locker
  mSuppressUserSideInteraction = what;
}

bool cedar::dev::Component::getSuppressUserSideInteraction() const
{
  // todo: lock this but not with the general locker
  return mSuppressUserSideInteraction;
}

void cedar::dev::Component::startBrakingSlowly()
{
  clearAllCommands();
  clearController();

  // todo: test that brake is not already running ...

  if (!applyBrakeSlowlyController())
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "Couldn't brake " + prettifyName() + " slowly, braking fast instead.",
      CEDAR_CURRENT_FUNCTION_NAME
    );

    this->startBrakingNow();
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->systemInfo(
      "Braking " + prettifyName() + " (slowly)",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
}

void cedar::dev::Component::startBrakingNow()
{
  clearAllCommands();
  clearController();

  // todo: test that brake is not already running ...

  if (!applyBrakeNowController())
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "Could not brake " + prettifyName() + " (quickly)",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->message(
      "Braking " + prettifyName() + " hard.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
}

bool cedar::dev::Component::applyCrashbrake()
{
  // dummy behaviour
  clearAllCommands();
  clearController();

  if (!applyBrakeNowController()) // dummy default
  {
    return false;
  }
 
  // AND SEND ...
  stepCommandCommunication( cedar::unit::DEFAULT_TIME_UNIT ); // this is a bit of a hack, but these are special circumstances

  return true;
}

void cedar::dev::Component::crashbrake()
{
  if (!applyCrashbrake())
  {
    std::cout << "[cedar PANIC] crashbrake failed. The shit has hit the fan!" << std::endl;
  }
  else
  {
  }
}

void cedar::dev::Component::clearController()
{
  QWriteLocker locker(mController.getLockPtr());
  mController.member().reset();
  mControllerFinished = true;
}

void cedar::dev::Component::setController(ComponentDataType type, cedar::dev::Component::ControllerCallbackFunctionType fun)
{
  QWriteLocker locker(mController.getLockPtr());
  mController.member() = ControllerCollectionPtr( new cedar::dev::Component::ControllerCollection{ type, fun} );
}

void cedar::dev::Component::clearAll()
{
  this->clearController();
  this->clearAllCommands();
}

void cedar::dev::Component::waitUntilCommunicated() const
{
  CEDAR_ASSERT( mCommunicationThread );

  if (mCommunicationThread->isCurrentThread())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Your are waiting for communication with the Component "
      + prettifyName() + " inside the "
      "communicating thread.",
      "cedar::dev::Component::waitUntilCommunicated()"
    );

  }

  mCommunicationThread->waitUntilStepped();

  // include any waiting for synchronous responses here ...
}

// static member:
std::map< cedar::dev::Component*, boost::posix_time::ptime > cedar::dev::Component::mRunningComponentInstancesAliveTime;
std::map< cedar::dev::Component*, boost::posix_time::ptime > cedar::dev::Component::mRunningComponentInstancesStartTime;


void cedar::dev::Component::handleCrash()
{
  cedar::aux::LogSingleton::getInstance()->warning
                                           (
                                             "Handling crash for robotic components",
                                             "cedar::dev::Component::handleCrash()"
                                           );
  for( auto component_it : mRunningComponentInstancesAliveTime )
  {
    auto componentpointer = component_it.first;

    if (componentpointer != NULL)
    {
      if (componentpointer->isCommunicating())
      {
std::cout << "crash detection: emergency crash braking for " << componentpointer->prettifyName() << ", pointer: " << componentpointer << std::endl;    
        componentpointer->crashbrake();
      }
    }
  }

}

void cedar::dev::Component::startBrakingAllComponentsNow()
{
  for( auto component_it : mRunningComponentInstancesAliveTime )
  {
    auto componentpointer = component_it.first;

    if (componentpointer != NULL)
    {
      componentpointer->startBrakingNow();
    }
  }

}

void cedar::dev::Component::startBrakingAllComponentsSlowly()
{
  for( auto component_it : mRunningComponentInstancesAliveTime )
  {
    auto componentpointer = component_it.first;

    if (componentpointer != NULL)
    {
      componentpointer->startBrakingSlowly();
    }
  }

}

bool cedar::dev::Component::anyComponentsRunning()
{
  return mRunningComponentInstancesAliveTime.size() != 0;
}

// static:
std::unique_ptr<cedar::aux::LoopFunctionInThread> cedar::dev::Component::mWatchDogThread;
// static:
cedar::aux::LockableMember<unsigned int> cedar::dev::Component::mWatchDogCounter = 0;

// static:
void cedar::dev::Component::stepStaticWatchDog(cedar::unit::Time)
{
  auto now = boost::posix_time::microsec_clock::local_time();
  std::vector<cedar::dev::Component*> components_to_delete;
  bool doCountWatchdog= false;

  QWriteLocker lock(&mWatchDogCounter.getLock()); // anti-scrolling

  for( auto component_it : mRunningComponentInstancesAliveTime )
  {    
    auto componentpointer = component_it.first;
    auto starttime = mRunningComponentInstancesStartTime[ component_it.first ];
    auto lasttime = component_it.second;

    if (componentpointer != NULL)
    {
      // note on time_period: tp1 < tp2 is TRUE IFF tp1.end < tp2.begin
      if (boost::posix_time::time_period(lasttime,now) 
          < boost::posix_time::time_period(lasttime + boost::posix_time::milliseconds(500), now ) )
      {
        // do nothing, all is well
      }
      else if (boost::posix_time::time_period(starttime,now) 
               < boost::posix_time::time_period(starttime + boost::posix_time::milliseconds(2*1000), now ) )
      {
        // do nothing, give every component a time to initialize
      }
      else if (boost::posix_time::time_period(starttime,now) 
               < boost::posix_time::time_period(starttime + boost::posix_time::milliseconds( 2*1000 + 250 ), now ) )
      {
        cedar::aux::LogSingleton::getInstance()->systemInfo(
          "waiting for " + componentpointer->prettifyName() + " to initialize ...",
          CEDAR_CURRENT_FUNCTION_NAME);
      }
      else if (boost::posix_time::time_period(starttime,now) 
               < boost::posix_time::time_period(starttime + boost::posix_time::milliseconds( 4*1000 ), now ) )
      {
        // do nothing, wait a bit longer ...
      }
      else
      {
        doCountWatchdog++;

        components_to_delete.push_back(componentpointer);
      }
    }
  }

  if (doCountWatchdog)
  {
    mWatchDogCounter.member()++;
   
    for(auto component : components_to_delete)
    {
      if (mWatchDogCounter.member() == 1
          || mWatchDogCounter.member() > 5000)
      {
        std::string s = "";
          
        if (mWatchDogCounter.member() > 5000)
        {
          s = " (repeated " + boost::lexical_cast<std::string>(mWatchDogCounter.member()) + " times)";

          mWatchDogCounter.member() = 1;
        }

        cedar::aux::LogSingleton::getInstance()->error(
            "Watchdog says: thread of " + component->prettifyName() + " is hanging. You are advised to stop the the component manually." 
            + s,
            CEDAR_CURRENT_FUNCTION_NAME);
      }
    }
  }

#if 0
  if (doCountWatchdog)
  {
    for(auto component : components_to_delete)
    {
      //component->startBrakingNow();
      cedar::aux::LogSingleton::getInstance()->warning(
                    "The component" +component->prettifyName()+ " is not braked by the watchdog anymore!" ,
                    CEDAR_CURRENT_FUNCTION_NAME);
    }
  }
#endif
}

// static:
std::string cedar::dev::Component::describeAllRunningComponents()
{
  std::string s;

  for( auto component_it : mRunningComponentInstancesAliveTime )
  {
    auto componentpointer = component_it.first;

    if (componentpointer != NULL)
    {
      if (s.empty())
      {
        s= componentpointer->prettifyName();
      }
      else
      {
        s += ", " + componentpointer->prettifyName();
      }
    }
  }

  return s;
}

std::string cedar::dev::Component::prettifyName() const
{
  std::string ret; 
  auto slot = mSlot.lock();
 
  ret = this->getName();

  if (ret.length() <= 0)
  {
    if (!slot)
      return "uninitialized robotic component";

    ret = slot->getName();
  }

  if (slot)
  {
    auto config = slot->getConfigurationName();

    if (config.length() > 0)
      ret+= " (" + config + ")";
  }

  return ret;
}

// static:
std::string cedar::dev::Component::describeAllRunningConfigurations()
{
  std::set<std::string> myset;

  for( auto component_it : mRunningComponentInstancesAliveTime )
  {
    auto componentpointer = component_it.first;

    if (componentpointer != NULL)
    {
      myset.insert( componentpointer->prettifyConfiguration() );
    }
  }

  std::string ret= "";
  for( std::string s : myset )
  {
    ret+= s + ", ";
  }
  ret= ret.substr(0, ret.size() -2 );
  ret[0]= std::toupper( ret[0] );
  return ret;
}

std::string cedar::dev::Component::prettifyConfiguration() const
{
  std::string ret; 
  auto slot = mSlot.lock();
 
  ret = this->getName();

  if (ret.length() <= 0)
  {
    if (!slot)
      return "uninitialized robotic component";

    ret = slot->getName();
  }

  if (slot)
  {
    auto config = slot->getConfigurationName();

    if (config.length() > 0)
      return config;
  }

  return ret;
}

int cedar::dev::Component::getMeasurementMatrixType()
{
  std::string typeString = mMatrixType->getValue();

  if(typeString == "CV_32F" || typeString == "CV_32FC1" )
     return CV_32F;
  else if(typeString == "CV_32FC2")
    return CV_32FC2;
  else if(typeString == "CV_32FC3")
    return CV_32FC3;
  else if(typeString == "CV_64F" || typeString == "CV_64FC1" )
         return CV_64F;
  else if(typeString == "CV_64FC2")
      return CV_64FC2;
  else if(typeString == "CV_64FC3")
      return CV_64FC3;
  else if(typeString == "CV_8U" || typeString == "CV_8UC1" )
           return CV_8U;
  else if(typeString == "CV_8UC2")
        return CV_8UC2;
  else if(typeString == "CV_8UC3")
        return CV_8UC3;
  else if(typeString == "CV_8UC4")
         return CV_8UC4;
  else
  {
    std::cout<<"The type: "<< typeString <<" given in the cvMatType parameter is not supported yet. Using the default value CV_64F"<<std::endl;
    return CV_32F;
  }
}